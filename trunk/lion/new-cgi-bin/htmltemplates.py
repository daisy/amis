#this file will probably be replaced by a nicer templating system

XHTML_TEMPLATE = """
<?xml version="1.0"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xml:lang="en" lang="en" xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>%(TITLE)s</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <link href="http://amisproject.org/translate/l10n.css" rel="stylesheet" type="text/css"/>
  </head>
  <body>%(BODY)s</body>
</html>"""

LOGIN_FORM = """
<h1>Login</h1>
<form action="process_login" method="post">
  <p>
    <label>User name:</label>
    <input type="text" name="username" value=""/>
  </p>
  <p>
    <label>Password:</label>
    <input type="password" name="password" value=""/>
    <input type="submit" value="Submit" />
  </p>
</form>

"""

def login_form():
    return XHTML_TEMPLATE % {"TITLE": "Login", "BODY": LOGIN_FORM}


def login_error():
    body = "<p>There was an error logging in.  Try again?</p>" + LOGIN_FORM
    return XHTML_TEMPLATE % {"TITLE": "Login error -- try again", "BODY": body}

def user_information(user):
    body = "<p>Hello %(user)s. You are working on %(lang)s.</p>" % \
        {"user": user["realname"], "lang": user["langname"]}


def main_menu_links(user):
    user_info = user_information(user)
    body = """
    <h1>Welcome!</h1>
    %(user_info)s
    <h1>Actions</h1>
    <ul>
        <li><a href="TranslateLabels">Translate AMIS labels</a></li>
        <li>Assign AMIS keyboard shortcuts</li>
        <li>Assign mnemonics (single-letter shortcuts)</li>
    </ul>
    """ % {"user_info": user_info}
    return XHTML_TEMPLATE % {"TITLE": "Main Menu", "BODY": body}

