#!/usr/bin/python
import MySQLdb
import cgi
import datetime
import sys
import cgitb; cgitb.enable()
import re
# DB connection info (hardcoded)
sys.path.append("/home/daisyfor")
from l10n import *
from pagepieces import *

#need to supply the table name
TEXTFLAGS_SQL = {"new": "and %(table)s.textflag=3", 
                "todo": "and %(table)s.textflag=2", 
                "newtodo": "and (%(table)s.textflag=2 or %(table)s.textflag=3)", 
                "all": ""}
#need to supply the table name
ROLES_SQL = {"main": """ and (%(table)s.role="STRING" or %(table)s.role="CONTROL" or %(table)s.role="DIALOG" or %(table)s.role="MENUITEM") """,
            "mnemonics": """ and %(table)s.role="MNEMONIC" """, 
            "accelerators": """ and %(table)s.role="ACCELERATOR" """}
#the width and height of the text box that translators type into
TRANSLATE_BOX_SIZE = {"main": (64, 3), "mnemonics": (1, 1), "accelerators": (20,1)}

def main():
    """Handle the form and show a page
        page.py?pageid= main | mnemonics | accelerators &
                [viewfilter = all | new | todo | newtodo]
    """
    user = get_user()
    if user == None:
        xhtml_die(NOT_LOGGED_IN)
    
    form = cgi.FieldStorage()
    if form == None or not form.has_key("pageid"):
        xhtml_die("<p>Error with submission form %s </p>." % str(form))
    page_id = form["pageid"].value
    if form.has_key("viewfilter"):
        view_filter = form["viewfilter"].value
    else:
        view_filter = "all"
    # remember the page and its options
    remember_last_page(user["langid"], page_id, view_filter)
    show_page(page_id, view_filter, "")


def show_last_page(langid, warning_text):
    """show the last-viewed page and apply the last-used view option"""
    page_id, view_filter = recall_last_page(langid)
    show_page(page_id, view_filter, warning_text)
    return


def show_page(page_id, view_filter, warning_text):
    """display a page"""
    user = get_user()
    if user == None:
        xhtml_die(NOT_LOGGED_IN)
    
    table = user["langid"].replace("-", "_")
    message = VIEW_FILTER_DESCRIPTIONS[view_filter]
    
    if page_id == "main":
        formhtml, numrows = make_main_form_table(table, view_filter)
    elif page_id == "mnemonics":
        formhtml, numrows = make_mnemonic_form_tables(table, view_filter)
    elif page_id == "accelerators":
        formhtml, numrows = make_accelerator_form_table(table, view_filter)
    
    title = TITLE_TEMPLATE % {"lang": user["langname"], "section": page_id}
    xhtml_out(title, BODY_TEMPLATE_HTML % \
    {"title": title, "message": "Viewing " + message, "about_section": SECTION_DESCRIPTIONS[page_id], "num_items": numrows,
    "warning": warning_text, "form": formhtml, "view_filter": VIEW_FILTER_DROPDOWN_HTML % {"pageid": page_id},
    "specialform": (CONFLICT_BUTTONS[page_id] % {"langid": user["langid"]})})


def make_main_form_table(table, view_filter):
    """Make the form for main page"""
    # get the xmlid, textstring, textflag, remarks from the localized table
    # get the textstring and remarks from the english table
    
    roles_sql = ROLES_SQL["main"] % {"table": table}
    textflags_sql = TEXTFLAGS_SQL[view_filter] % {"table": table}
    request = """
	SELECT %(table)s.xmlid, %(table)s.textstring, %(table)s.textflag, %(table)s.remarks, eng_US.textstring, eng_US.remarks  
	FROM %(table)s, eng_US 
	WHERE %(table)s.xmlid=eng_US.xmlid %(where_roles)s %(where_textflags)s""" % \
	{"table": table, "where_roles": roles_sql, "where_textflags": textflags_sql}
    db = connect_to_db("ro")
    cursor=db.cursor()
    cursor.execute(request)
    rows = cursor.fetchall()
    cursor.close()
    html_form = "<table>"
    for row in rows:
        html_form = html_form + process_row(row, db, "main", INSTRUCTIONS["main"], "")
    html_form = html_form + "</table>"
    db.close()
    return html_form, len(rows)


def make_mnemonic_form_tables(table, view_filter):
    """Make the form for the mnemonic page"""
    # first get all the unique mnemonic groups
    request = "SELECT DISTINCT mnemonicgroup FROM %s WHERE mnemonicgroup >= 0" % table
    db = connect_to_db("ro")
    cursor = db.cursor()
    cursor.execute(request)
    rows = cursor.fetchall()
    cursor.close()
    # for each mnemonic group, make a table
    num_rows = 0
    html_form = ""
    group_number = 1
    roles_sql = ROLES_SQL["mnemonics"] % {"table": table}
    textflags_sql = TEXTFLAGS_SQL[view_filter] % {"table": table}
    for r in rows:
        request = """
        SELECT %(table)s.xmlid, %(table)s.textstring, %(table)s.textflag, 
        %(table)s.remarks, %(table)s.target, eng_US.textstring, eng_US.role 
        FROM %(table)s, eng_US
        WHERE %(table)s.xmlid=eng_US.xmlid and %(table)s.mnemonicgroup=%(mnemonicgroup)d %(where_roles)s %(where_textflags)s""" % \
        {"table": table, "where_roles": roles_sql, "where_textflags": textflags_sql, "mnemonicgroup": r[0]}
        cursor = db.cursor()
        cursor.execute(request)
        group = cursor.fetchall()
        cursor.close()
        num_rows = num_rows + len(group)
        # prints the heading "<h2 id="group_0">GROUP #(1...2...3..)</h2>" but gives it an id that corresponds to what's in the database
        if len(group) > 0:
            html_form = html_form + MNEMONIC_GROUP_HEADING % {"mnemonicgroup": r[0], "num": group_number}
            html_form = html_form + "<table>"
            for item in group:
                xmlid, textstring, textflag, remarks, target_id, ref_string, ref_role = item;
                if remarks == None:
                    remarks = ""
                # target_id = the item the mnemonic refers to, e.g. the target of the mnemonic "O" would point to the entry for "Open"
                # make the following substitutions:
                #   our_remarks: a description including the english string with the mnemonic underlined
                eng_word = build_mnemonic_string(db, "eng_US", target_id, ref_string)
                #   ref_string: the translated string with the mnemonic underlined
                ref_string = build_mnemonic_string(db, table, target_id, textstring)
                our_remarks = "This is for a %s.  In English, it is used like this: \"%s\"" % (ROLE_DESCRIPTIONS[ref_role], eng_word) 
                html_form = html_form + process_row((xmlid, textstring, textflag, remarks, ref_string, our_remarks), 
                db, "mnemonics", INSTRUCTIONS["mnemonics"], "")
            html_form = html_form + "</table>"
            group_number = group_number + 1
    return html_form, num_rows

def make_accelerator_form_table(table, view_filter):
    """make the main form for the accelerators page"""
    #filter out space, F11, Alt shortcuts
    #be sure to % html_form with the key masks
    roles_sql = ROLES_SQL["accelerators"] % {"table": table}
    textflags_sql = TEXTFLAGS_SQL[view_filter] % {"table": table}
    request = """
    SELECT %(table)s.xmlid, %(table)s.textstring, %(table)s.textflag, 
    %(table)s.remarks, %(table)s.target, eng_US.textstring, eng_US.role 
    FROM %(table)s, eng_US
    WHERE %(table)s.xmlid=eng_US.xmlid  %(where_roles)s %(where_textflags)s""" % \
    {"table": table, "where_roles": roles_sql, "where_textflags": textflags_sql}
    db = connect_to_db("ro")
    cursor=db.cursor()
    cursor.execute(request)
    rows = cursor.fetchall()
    cursor.close()
    html_form = "<table>"
    excluded_count = 0
    for row in rows:
        xmlid, textstring, textflag, remarks, target_id, ref_string, ref_role = row;
        # certain items are being excluded from the translation
        if is_excluded(xmlid):
            excluded_count = excluded_count + 1
            continue
        key_masks, letter = parse_key_masks(ref_string)
        tmp, translated_letter = parse_key_masks(textstring)
        # target_id = the item the accelerator refers to
        # make the following substitutions:
        #   our_remarks: a description in this form (from the english table) "Ctrl + O (Open)"
        our_remarks = "In English, AMIS uses " + build_accelerator_string(db, "eng_US", target_id, ref_string)
        #   ref_string: the translated string with the accelerator and command
        ref_string = build_accelerator_string(db, table, target_id, textstring)
        instructions = INSTRUCTIONS["accelerators"] + key_masks
        result = process_row((xmlid, translated_letter, textflag, remarks, ref_string, our_remarks),
        db, "accelerators", instructions, key_masks)
        html_form = html_form + result
    html_form = html_form + "</table>"
    db.close()
    return html_form, len(rows) - excluded_count

    
def process_row(row, db, page_id, instructions, data_prefix):
    """Process a single row, which is in the form of:
        (xmlid, textstring, textflag, remarks, ref_string, our_remarks)"""
    xmlid, textstring, textflag, remarks, ref_string, our_remarks = row
    langid = get_user()["langid"]
    #otherwise it crashes
    if remarks == None: 
        remarks = ""
	if our_remarks == None: 
	    our_remarks = ""
	#get the values for the radio buttons
    if textflag == 1:
        textok = "checked"
        texttodo = ""
    elif textflag == 2:
        textok = ""
        texttodo = "checked"
    else: #textflag == 3
        textok = ""
        texttodo = ""
    one_table_row = FORM_ROW % \
    {"ref_string": ref_string, "our_remarks": our_remarks, 
    "xmlid": xmlid, "translation": textstring, "translator_remarks": remarks, 
    "text_status_ok": textok, "text_status_todo": texttodo, "langid": langid, 
    "instructions": instructions, "translate_box_columns": TRANSLATE_BOX_SIZE[page_id][0], 
    "translate_box_rows": TRANSLATE_BOX_SIZE[page_id][1], "pageid": page_id, "dataprefix": data_prefix}
    html_form = """<tr>%s</tr>""" % one_table_row
    return html_form


def build_mnemonic_string(db, table, target_id, textstring):
    """build a string where the mnemonic letter (given by textstring) is underlined in the word (referenced by target_id)"""
    cursor = db.cursor()
    cursor.execute("SELECT textstring FROM %s WHERE xmlid = \"%s\"" % (table, target_id))
    row = cursor.fetchone()
    cursor.close()
    if row == None: 
        #TODO warning about invalid mnemonic
        return ""
    word = row[0]
    pos = word.lower().find(textstring.lower())
    if pos == -1:
        return word + ("""(<span style="text-decoration: underline">%s</span>)""" % textstring)
    else:
        # underline the mnemonic
        return word[0:pos] + ("""<span style="text-decoration: underline">%s</span>""" % word[pos]) + word[pos+1:len(word)]

def build_accelerator_string(db, table, target_id, textstring):
    """build a string that looks like this: Ctrl + O (Open)"""
    cursor = db.cursor()
    cursor.execute("SELECT textstring FROM %s WHERE xmlid=\"%s\"" % (table, target_id))
    row = cursor.fetchone()
    cursor.close()
    if row == None:
        #TODO warning about invalid mnemonic
        return ""
    accel_string = "%s (%s)" % (textstring, row[0])
    return accel_string


def parse_key_masks(keys):
    """Take something like 'Ctrl+0' and isolate 'Ctrl+' and 'O'.  Return both parts.  Account for 'Ctrl++' """
    
    last_part=""
    if keys.endswith("+") == True:
        # reverse the string, replace the last '+' with another letter, but remember what we did
        keys = keys[::-1]
        keys.replace("+", "x", 1)
        last_part = "+"
    pos = keys.rfind("+")
    if pos == -1:
        return "", keys
    else:
        if last_part == "":
            last_part = keys[pos+1:len(keys)]
        return keys[0:pos+1], last_part


def is_excluded_deprecated(keys):
    """True if keys contains Alt, Space, or F-anything.  However, this has to be changed to reflect the actual keys vs the name of the keys."""
    # excluded patterns are Alt+__, F__, and Space
    p1 = re.compile("Alt+.*")
    p2 = re.compile("F.*")
    if p1.match(keys) or p2.match(keys) or keys == "Space":
        return True
    else:
        return False
    
def is_excluded(xmlid):
    """This is a much stricter list of what to exclude"""
    include_only = ["t49", "t53", "t57", "t73", "t77", "t84", "t88", "t96", "t99", "t102", "t105"]
    try:
        include_only.index(xmlid)
    except Exception, e:
        return True;
    return False;


# main entry point
if __name__ == "__main__": 
    main()
