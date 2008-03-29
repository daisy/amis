#!/usr/bin/python
import cgi
import cgitb; cgitb.enable()
from l10n import *
from admin import *

def get_uuid():
    """Get a uuid from the os to act as a session id."""
    pipe = os.popen("uuidgen", "r")
    uuid = pipe.readline()
    pipe.close()
    return uuid.rstrip()

def get_login_info():
    """Get login info from the form (username, password) or die on error."""
    form = cgi.FieldStorage()
    if form == None or not form.has_key("username") or \
    not form.has_key("password"):
        xhtml_die("""<p>Could not get login information. Please <a href="%s">try again</a>.</p>""" % LOGIN_URL)
    
    return form["username"].value, form["password"].value

def login(username, password):
    """Try to login the user and return None on failure."""
    db = connect_to_db("rw")
    cursor = db.cursor()
    request = """
    SELECT * FROM users WHERE username="%(name)s" AND password="%(pwd)s" """ \
    % {"name": username, "pwd": password}
    cursor.execute(request)
    userinfo = cursor.fetchall()
    if userinfo != None and len(userinfo) > 0:
        now = datetime.datetime.now()
        sessionid = get_uuid()
        request = """
        UPDATE users SET sessionid="%(id)s", lastlogin="%(last)s" WHERE username="%(name)s" """ % \
        {"id": sessionid, "last": now.strftime("%Y-%m-%d %H:%M:%S"), "name": username}
        cursor.execute("set autocommit = 1")
        cursor.execute(request)
        set_cookie(username, sessionid)
    else:
        return None
    
    cursor.close()
    db.close()
    return userinfo

def main():
    username, password = get_login_info()
    userinfo = login(username, password)
    if userinfo == None:
        message = """
        <p>Could not login user %(user)s. Please verify that login and 
        password are correct and <a href="%(login)s">try again</a>.</p>""" % \
        {"user": username, "login": LOGIN_URL}
        xhtml_die(message)
  
    else:
        if username == "admin": 
            show_admin_main_menu("Welcome welcome!")
        else:
            xhtml_out("OK", """<p>Login successful.
            See <a href="/cgi-bin/user.py">your user information</a>, or
            <a href="/cgi-bin/page.py?pageid=main&viewfilter=all">start working</a>.</p>""")

if __name__ == "__main__": main()
