"""
Administrative stuff
=>Decide whether to put this stuff in the admin webpage or move the admin webpage stuff to here
"""
from xml.dom import minidom, Node
import codecs, MySQLdb, sys
import os
import string
#hardcoded DB connection info that's not in SVN is in my local DB module
sys.path.append("../")
sys.path.append("../cgi-bin/")
from page import is_excluded
from DB.connect import *
#python debugger
import pdb
import getopt
#a list of all language table names (not including eng_US or test)
langtables = ("test", "eng_US", "arb", "ben_BD", "fil_PH", "fin_FI", "guj_IN", "hin_IN", "ind_ID", "jpn_JP", "may_MY", "nep_NP", \
"nno_NO", "nob_NO", "sin_LK", "spa_ES", "swe_SE", "tam_IN", "tha_TH", "urd", "vie_VN", "zho_CN")

def setup(xmlfile):
    global doc, db, cursor, warnings, did_setup
    warnings = 0
    doc = minidom.parse(xmlfile)
    db = connect_to_db_from_local_machine("admin")
    cursor = db.cursor()
    did_setup = True


def unsetup():
    global did_setup
    if did_setup == False:
        return
    did_setup = False
    doc.unlink()
    cursor.close()
    db.close()

    
def main(argv):
    global did_setup
    did_setup = False
    try:
        opts, args = getopt.getopt(argv, "hceit:f:z", ["help", "idcheck", "export", "import", "table=", "file=", "throwaway"])
    except getopt.GetoptError, exc:
        print exc.msg
        usage()
        sys.exit(2)
    
    if len(opts) == 0:
        usage()
        sys.exit()
    
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()                     
            sys.exit()                  
        elif opt in ("-t", "--table"):
            table = arg
            if not table in langtables:
                die("table %s does not exist" % table)
        elif opt in ("-f", "--file"):
            xmlfile = arg
        elif opt in ("-c", "--idcheck"):
            action = "idcheck"
        elif opt in ("-e", "--export"):
            action = "export"
        elif opt in ("-i", "--import"):
            action = "import"
        elif opt in ("-z", "--throwaway"):
            action = "throwaway"
    
    if not action:
        usage()
        sys.exit(2)
    
    if action == "idcheck":
        if not xmlfile:
            print ("idcheck needs an xml file")
            usage()
            sys.exit(2)
        setup(xmlfile)
        listMissingItems()
        unsetup()
    elif action == "export":
        if not (table or xmlfile):
            print ("export needs a table and an xml file")
            usage()
            sys.exit(2)
        setup(xmlfile)
        export(table)
        unsetup()
    elif action == "import":
        if not (table or xmlfile):
            usage()
            sys.exit(2)
        setup(xmlfile)
        importXmlToTable(table)
        unsetup()
    elif action == "throwaway":
        setup("../amisAccessibleUi.xml")
        throwaway()
        unsetup()
    
    print "Total warnings: " + str(warnings)


def usage():
    print """   Usage: 
        amisdb.py --help                             Show this information
        amisdb.py --idcheck --file=file              Make sure each table has IDs to match those in the file
        amisdb.py --export --table=id --file=file    Export the table to the template file
        amisdb.py --import --table=id --file=file    Import the file to the table 
        amisdb.py --throwaway                        Execute throwaway code.
        
        -h = --help
        -c = --idcheck
        -e = --export
        -t = --table
        -f = --file
        -z = --throwaway
        
        More information...
        
        ID Check (--idcheck)
            Make sure that each text ID in the specified amisAccessibleUi.xml file has a corresponding entry in every database table.
        
        Export (--export --table=id --file=file)
            Export the contents of the table and use the given file (an amisAccessibleUi.xml file) as the template.  
            The result will be saved as table_id.xml
        
        Import (--import --table=id --file=file)
            Import the xml file into the given table.
            This clears the table first.  
            Use carefully!
        
        Throwaway
            Execute the code in a function called "Throwaway".  Usually just used for one-time DB fixes.
        """


def die(msg):
    print("ERROR: \n" + msg)
    unsetup()
    sys.exit(1)

def warn(msg):
    global warnings
    warnings = warnings + 1
    print ("WARNING: \n" + msg)

def execute(query):
    """I wrote this because I couldn't figure out how to turn on echoing on the MySQLdb object"""
    print query
    cursor.execute(query)

def listMissingItems():
    """Make sure that every text element in the XML file has a database entry"""
    global doc
    texts = doc.getElementsByTagName("text")
    for t in texts:
        xmlid = t.getAttribute("id")
        for table in langtables:
            execute("SELECT * FROM %s WHERE xmlid=\"%s\"" % (table, xmlid))
            rows = cursor.fetchall()
            if rows == None or len(rows) == 0:
                warn(t.toxml() + " missing in %s" % table)

                
def export(table):
    """
    Export the language table to an XML file
    Note that most accelerators aren't getting exported, only the menu header accelerators (Alt+F, Alt+H, etc).
    The menu header accelerators are being built from the mnemonic for the menu header label.
    When all the accelerators do get exported (AMIS 3), we should be exporting the "actualkeys" field of the accelerator entry.
    """
    execute("SELECT id, xmlid, textstring, audiouri, role, target FROM %s" % table)
    for id, xmlid, textstring, audiouri, role, target in cursor:
        elm = findElementWithId("text", xmlid)
        if elm == None: 
            warn("Text element %s not found." % xmlid)
            continue
        
        # t166 is the stupid copyright symbol that screws everything up!
        if xmlid == "t166":
            elm.firstChild.data = "Copyright (c) 2008 DAISY-for-All"
        else:
            if elm.firstChild.nodeType == Node.TEXT_NODE:
                print textstring
                if role == "ACCELERATOR":
                    if isTextOfAcceleratorForContainer(elm):
                        # generate the accelerator contents as "Alt + <mnemonic_text>"
                        accel_string = "Alt+%s" % getMnemonicForTarget(target, table)
                        elm.firstChild.data = accel_string
                    #else, it's a regular accelerator
                    #we know that one day, we'll export from "actualkeys" too; but for now, just the name is enough
                    else:
                        #just export the accelerators that were included in the webform
                        if is_excluded(xmlid) == False:
                            elm.firstChild.data = textstring
                elif role != "ACCELERATOR":
                    elm.firstChild.data = textstring
            else:
                warn("Text element %s has no contents." % xmlid)
        
        if elm.parentNode != None:
            audio = getFirstChildWithTagName(elm.parentNode, "audio")
            if audio != None:
                if audiouri == None:
                    audiouri = ""
                audio.setAttribute("src", audiouri)
    
    outpath = os.path.join("./", table + ".xml")
    outfile = file(outpath, "w")
    outfile.write(codecs.BOM_UTF8)
    outfile.write(doc.toxml().encode("utf-8"))   
    outfile.write("\n")
    outfile.close()


def importXmlToTable(table):
    """The master import routine
        
        For all text items:
            add a row with the following properties:
                textstring =>text#CDATA
                audiodata => NULL
                audiouri => sibling audio@src
                textflag => look at new or changed flag in XML (and remove the flag after it has been copied to DB)
                audioflag => look at new or changed flag in XML (and remove the flag after it has been copied to DB)
                remarks => ""
                xmlid => @id
                role => run setRoles()
                mnemonicgroup => run findMnemonicGroups()
                target => partially taken care of by findMnemonicGroups(); also run findAcceleratorTargets()
                
                FLAGGING:
                    1 = OK
                    2 = TODO
                    3 = NEW OR CHANGED
    """
    request = "DELETE FROM %s" % table
    execute(request)
    elms = doc.getElementsByTagName("text")
    for el in elms:
        data = parseTextElm(el)
        if data == None: continue
        textstring, audiouri, xmlid, textflag, audioflag = data
        if el.parentNode.tagName == "accelerator":
            keys = "\"%s\"" % el.parentNode.getAttribute("keys")
        else:
            keys="NULL"
        
        request = """INSERT INTO %(table)s (textstring, textflag, audioflag, audiouri, xmlid, actualkeys) 
        VALUES (\"%(textstring)s\", %(textflag)d, %(audioflag)d, \"%(audiouri)s\", \"%(xmlid)s\", %(keys)s)""" \
        % {"table": table, "textstring": textstring, "textflag": textflag, "audioflag": audioflag, "audiouri": audiouri, 
        "xmlid": xmlid, "keys": keys}
        execute(request)
    
    setRoles(table)
    findMnemonicGroups(table)
    findAcceleratorTargets(table)
    return

def isTextOfAcceleratorForContainer(elm):
    """
    does this text element belong to an accelerator element that belongs to a container element?
    <container>
        <blah>...</blah>
        <accelerator>
            <text>...</text>
            <audio/>
        </accelerator>
    </container>
    """
    try:
        if elm.parentNode.tagName == "accelerator" and elm.parentNode.parentNode.tagName == "container":
            return True
        else:
            return False
    except Exception, e:
        return False; 


def getMnemonicForTarget(targetId, table):
    """Get the mnemonic letter for the target id"""
    request = "SELECT textstring FROM %s WHERE role=\"MNEMONIC\" and target=\"%s\"" % (table, targetId)
    execute(request)
    row = cursor.fetchone()
    if row == None:
        warn("mnemonic not found for target %s in table %s" % (targetId, table) )
    else:
        return row[0]


def parseTextElm(elm):
    """ elm: a text element
        make sure this text element appears in the following context:
        <parent>
            <text>CDATA</text>
            <audio src="path"/>
        </parent>
        
        return None if there is an error
        otherwise, return (textstring, audiosrc, xmlid, textflag, audioflag)
    """
    if elm.firstChild == None: 
        return None
    if elm.firstChild.nodeType != Node.TEXT_NODE: 
        return None
    if elm.getAttribute("id") == "": 
        warn ("no id for text element with text = %s", elm.firstChild.wholeText)
    if elm.parentNode == None: 
        return None
    audio = getFirstChildWithTagName(elm.parentNode, "audio") 
    if audio == None: 
        return None
    if audio.getAttribute("src") == "": 
        warn("no src for audio near text element with id = %s", elm.getAttribute("id"))
    if elm.getAttribute("flag") == "new": 
        textflag = 3
    else: 
        textflag = 1
    if audio.getAttribute("flag") == "new": 
        audioflag = 3
    else: 
        audioflag = 1
    
    return cleanquotes(elm.firstChild.wholeText), audio.getAttribute("src"), elm.getAttribute("id"), textflag, audioflag

def cleanquotes(string):
    """ replace double quotes by slash-doublequote """
    return string.replace("\"", "\\\"")

def findAcceleratorTargets(table):
    """get the accelerator element text ids and write who their targets are.  the only reason we're using textids is that the code
    is already in place to do the same for mnemonics and it works."""
    elms = doc.getElementsByTagName("accelerator")
    idlist = []
    for el in elms:
        textelm = getFirstChildWithTagName(el, "text")
        idlist.append(textelm.getAttribute("id"))
    idtargets = getTargets(idlist)
    for xmlid, targetid in zip(idlist, idtargets):
        request = "UPDATE %s SET target=\"%s\" WHERE xmlid = \"%s\"" % (table, targetid, xmlid)
        execute(request)

def findMnemonicGroups(table):
    """Identify groups for the mnemonics based on menu and dialog control groupings"""
    groupid = 0
    #container elements
    elms = doc.getElementsByTagName("container") + doc.getElementsByTagName("containers")
    for e in elms:
        items = getItemsInContainer(e)
        getMnemonicsAndWriteData(table, items, groupid)
        groupid = groupid + 1
    
    #dialog elements
    elms = doc.getElementsByTagName("dialog")
    for e in elms:
        items = getItemsInDialog(e)
        getMnemonicsAndWriteData(table, items, groupid)
        groupid = groupid + 1

def getMnemonicsAndWriteData(table, items, groupid):
    """get the mnemonic text ids and their targets, and write it to the database"""
    idlist = []
    for it in items:
        elm = getFirstChildWithTagName(it, "mnemonic")
        if elm != None:
            xmlid = getFirstChildWithTagName(elm, "text").getAttribute("id")
            idlist.append(xmlid)
    
    idtargets = getTargets(idlist)
    for xmlid, targetid in zip(idlist, idtargets):
        request = "UPDATE %s SET mnemonicgroup = %d, target=\"%s\" WHERE xmlid = \"%s\"" % (table, groupid, targetid, xmlid)
        execute(request)

def getTargets(idlist):
    """from the textid of a mnemonic or accelerator, find out who it is intended for"""
    targets = []
    for xmlid in idlist:
        #the <text> element of the mnemonic or accelerator
        elm = findElementWithId("text", xmlid)
        #the parent element of the mnemonic or accelerator
        parentTag = elm.parentNode.parentNode.tagName
        if parentTag == "action" or parentTag == "container" or parentTag == "control" or parentTag == "dialog":
            caption = getFirstChildWithTagName(elm.parentNode.parentNode, "caption")
            textid = getTextIdForCaption(caption)
            targets.append(textid)
    return targets

def getTextIdForCaption(elm):
    """Resolve the text for the caption element.  It might be directly nested or it might be a prompt reference.
    The permutations are:
    1. <caption>
        <text>The text</text>
        ...
    
    2. <caption>
        <promptItem refid="id123"/>
        means "go find the text in prompt@id='id123'"
    
    3. <caption>
        <promptItem>
            <text>The text</text>
    
    4. <caption>
        <prompt>
            <promptItem ... can be either with a refid or with nested text
    
    ignore all other cases.
    
    """
    #the obvious case #1
    text = getFirstChildWithTagName(elm, "text")
    #the less-obvious cases
    if text == None:
        promptItem = getFirstChildWithTagName(elm, "promptItem")
        if promptItem == None:
            prompt = getFirstChildWithTagName(elm, "prompt")
            if prompt == None: return ""
            else:
                promptItem = getFirstChildWithTagName(prompt, "promptItem")
                if promptItem == None: return ""
        #promptItem variable better have something in it at this point
        return getTextIdFromPromptItem(promptItem)
    else:
        return text.getAttribute("id")

def getTextIdFromPromptItem(elm):
    """get the text out of a prompt item.  resolve references if necessary."""
    if elm == None:
        return ""
    
    text = getFirstChildWithTagName(elm, "text")
    if text != None:
        return text.getAttribute("id")
    else:
        refid = elm.getAttribute("refid")
        if refid != "":
            prompt = findElementWithId("promptItem", refid)
            return getTextIdFromPromptItem(prompt)
        else:
            return ""

def getItemsInContainer(elm):
    """For an XML element, get the actions and containers. Look one level deep (also look in switches)."""
    items = []
    items.extend(getAllChildrenWithTagName(elm, "action"))
    items.extend(getAllChildrenWithTagName(elm, "container"))
    switches = getAllChildrenWithTagName(elm, "switch")
    for sw in switches:
        items.extend(getAllChildrenWithTagName(sw, "action"))
        items.extend(getAllChildrenWithTagName(sw, "container"))
    return items

def getItemsInDialog(elm):
    """For a dialog element, get the controls"""
    items = []
    items.extend(getAllChildrenWithTagName(elm, "control"))
    return items

def getFirstChildWithTagName(elm, tagname):
    if elm.hasChildNodes() == True:
        for c in elm.childNodes:
            if c.nodeType == Node.ELEMENT_NODE and c.tagName == tagname:
                return c
    return None
    


def getAllChildrenWithTagName(elm, tagname):
    """Get the immediate children (not the grandchildren) of the element with the given tag name"""
    if elm.hasChildNodes() == True:
        elms = []
        for c in elm.childNodes:
            if c.nodeType == Node.ELEMENT_NODE and c.tagName == tagname:
                elms.append(c)
        return elms
    else:
        return None


def findElementWithId(tagname, xmlid):
    """Stupid DOM parser won't find an ID unless the document has a DTD"""
    texts = doc.getElementsByTagName(tagname)
    for t in texts:
        if t.getAttribute("id") == xmlid:
            return t


def setRoles(table):
    execute("SELECT id, xmlid, textstring FROM %s" % table)
    for theid, xmlid, textstring in cursor:
        elm = findElementWithId("text", xmlid)
        if elm != None:
            tag = elm.parentNode.tagName
            if tag == "accelerator": role = "ACCELERATOR"
            elif tag == "mnemonic": role = "MNEMONIC" 
            elif tag == "caption":
                tag = elm.parentNode.parentNode.tagName
                if tag == "action" or tag == "container": role = "MENUITEM"
                elif tag == "control": role = "CONTROL"
                elif tag == "dialog": role = "DIALOG"
                else: role="STRING"
            else:
                role = "STRING"
            request = "UPDATE %s SET role = \"%s\" WHERE id = %s" % (table, role, theid)
            execute(request)


def throwaway():
    """put useful throwaway code here"""
    print "Nothing to execute (how about writing some code?)"


if __name__ == "__main__":
    main(sys.argv[1:])
    