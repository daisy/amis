#!/usr/bin/python
import cgi
import cgitb; cgitb.enable()
from l10n import *

def main():
    """Show the administrative page or go to a specific page"""
    #check if the user is logged-in
    user = get_user()
    if user == None:
        xhtml_die(NOT_LOGGED_IN)
        return
		
    #get the submitted form
    form = cgi.FieldStorage()
	#decide what to do
    if form and form.has_key("action"):
        action = form["action"].value
        if action == "show_add_language_form": show_add_language_form("")
        elif action == "process_add_language_form": process_add_language_form(form)
        elif action == "export_from_database":
            if form.has_key("langid"): 
                location = export(form["langid"].value)
                show_admin_main_menu("Exported to " + location)
            else: show_admin_main_menu("Could not export")        
        else: show_admin_main_menu("What do you want me to do?") 
    else: show_admin_main_menu("Form error")

def show_admin_main_menu(msg):
    """Show the main menu"""
    langlist = generate_language_dropdown();	
    admin_page = """
    <p>%(message)s</p>
    <h1>Administrative actions</h1>
    <ul>
        <li><a href="/cgi-bin/admin.py?action=show_add_language_form">Add a new language</a></li>
    </ul>
    <p>Export to XML</p>
    <form action="/cgi-bin/admin.py" method="POST">
    %(language_dropdown)s
    <input type="submit" value="Export" style="align: bottom"/>
    <input type="hidden" name="action" value="export_from_database"/>
    </form>
    """ % \
    {"message": msg, "language_dropdown": langlist}
    xhtml_out("Administrative actions", admin_page)

def show_add_language_form(msg):
    """Display the form for adding a language"""
    add_language_form = """
    <p>%(message)s</p>
    <h1>Add a new language</h1>
    <form action="/cgi-bin/admin.py" method="POST">
    <table summary="add new language">
    <tr>
        <td>username</td>
        <td><textarea cols="35" rows="1" name="username"></textarea></td>
    </tr>
    <tr>
        <td>realname</td>
        <td><textarea cols="35" rows="1" name="realname"></textarea></td>
    </tr>
    <tr>
        <td>password</td>
        <td><textarea cols="35" rows="1" name="password"></textarea></td>
    </tr>
    <tr>
        <td>email</td>
        <td><textarea cols="35" rows="1" name="email"></textarea></td>
    </tr>
    <tr>
        <td>langid (e.g. eng-US)</td>
        <td><textarea cols="35" rows="1" name="langid"></textarea></td>
    </tr>
    <tr>
        <td>langname (e.g. U.S. English)</td>
        <td><textarea cols="35" rows="1" name="langname"></textarea></td>
    </tr>
    <tr>
        <td>svnpath</td>
        <td><textarea cols="35" rows="1" name="svnpath">http://amis.svn.sourceforge.net/viewvc/amis/trunk/Langpacks/TYPE</textarea>
    </table>
	<input type="submit" value="Add" style="align: bottom"/>
	<input type="hidden" name="action" value="process_add_language_form"/>
    </form>
    """ % \
    {"message": msg}
    xhtml_out("Add new language", add_language_form)

def process_add_language_form(form):
    """Add a new language"""
    #read the form data
    if form == None:
        xhtml_die("<p>Error with submission form." + str(form))
        return
    if (not form.has_key("username") or not form.has_key("realname") or not form.has_key("password") or \
     not form.has_key("email") or not form.has_key("langid") or not  form.has_key("langname") or not form.has_key("svnpath")):
        show_add_language_form("please fill in all the fields!")
        return
        
    username = form["username"].value
    realname = form["realname"].value
    password = form["password"].value
    email = form["email"].value
    langid = form["langid"].value
    langname = form["langname"].value
    svnpath = form["svnpath"].value

    db = connect_to_db("admin")
    if db == None: 
        xhtml_die("<p>Error connecting to database.</p>")
        return
    cursor = db.cursor()

    #make sure the username or langid don't already exist
    request = "SELECT * from users WHERE langid =\"" + langid + "\""
    cursor.execute(request)
    result = cursor.fetchall()
    if result != None and len(result) > 0:
        show_add_language_form("sorry, the language " + langid + " already exists.")
        return
    request = "SELECT * from users WHERE username = \"" + username + "\""
    cursor.execute(request)
    result = cursor.fetchall()
    if result != None and len(result) > 0:
         show_add_language_form("sorry, the user " + username + " already exists")
         return
     
    #create the user
    request = """
    INSERT INTO users VALUES ("%(u)s", "%(r)s", "%(p)s", "%(e)s", "%(lid)s", "%(lname)s", NULL, NULL,"%(svn)s", NULL)
    """ % \
    {"u": username, "r": realname, "p": password, "e": email, "lid": langid, "lname": langname, "svn": svnpath} 
    cursor.execute(request)
    langid = langid.replace("-", "_")

    #copy the english table
    request = "CREATE TABLE " + langid + " SELECT * from eng_US"
    cursor.execute(request)

    #flag all "TODO" and clear stuff
    request="UPDATE " + langid + " SET textflag=3, audioflag=3, audiodata=NULL, audiouri=NULL, remarks=NULL"
    cursor.execute(request)
    cursor.close()
    db.close()
    show_admin_main_menu("Done!  Anything else to do?")

def generate_language_dropdown():
    db = connect_to_db("ro")
    if db == None: 
        xhtml_die("<p>Error connecting to database.</p>")
        return
    cursor = db.cursor()
    request = "SELECT langid FROM users"
    cursor.execute(request)
    result = cursor.fetchall()
    dropdown = "<select name=\"langid\">"
    for item in result: 
        item_table = item[0].replace("-", "_")
        dropdown += "<option value=\"" + item_table + "\">" + item[0] + "</option>"
    dropdown += "</select>"
    cursor.close()
    db.close()
    return dropdown
    
#main entry point
if __name__ == "__main__": main()
