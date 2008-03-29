# Useful stuff for L10n scripts
# Please include this module!

import cgi
import Cookie
import datetime
import MySQLdb
import os
import sys
import cgitb; 
sys.path.append("/home/daisyfor")
from DB.connect import *
from pagepieces import *
cgitb.enable()

COOKIE_USERNAME = "l10n_username"
COOKIE_SESSIONID = "l10n_sessionid"

def xhtml_out(title, body):
  print XHTML_TEMPLATE % {"TITLE": title, "BODY": body}


def xhtml_die(errorbody):
  """Die with an error output to XHTML."""
  xhtml_out("Error", "<h1>Error</h1>%s" % errorbody)
  sys.exit()


def connect_to_db(user):
    db = connect_to_db_from_localhost(user)
    if db == None:
        xhtml_die("<p>Could not connect to database</p>")
    else:
        return db


def get_user():
  """Check that the user's cookie to see if she's logged in and return her
  user info in a dictionary. If not logged in, return None."""
  username, sessionid = read_cookie()
  if username == "" or username == None: return None
  db = connect_to_db("ro")
  cursor = db.cursor()
  fields = ["username", "realname", "password", "email", "langid", \
    "langname", "lastactivity", "svnpath", "sessionid"]
  request = 'SELECT %s FROM users WHERE username="%s" AND sessionid="%s"' % \
    (",".join(fields), username, sessionid)
  cursor.execute(request)
  row = cursor.fetchone()
  cursor.close()
  db.close()
  return dict(zip(fields, row))



def set_cookie(username, sessionid):
  """Set the login cookie."""
  cookie = Cookie.SimpleCookie()
  cookie[COOKIE_USERNAME] = username
  cookie[COOKIE_SESSIONID] = sessionid
  print cookie


def read_cookie():
  """Read user name and session id for the cookie. If there is an error, user
  name will be empty."""
  username = ""
  sessionid = ""
  if os.environ.has_key("HTTP_COOKIE"):
    cookie = Cookie.SimpleCookie()
    cookie.load(os.environ["HTTP_COOKIE"])
    if cookie.has_key(COOKIE_USERNAME) and cookie.has_key(COOKIE_SESSIONID):
      username = cookie[COOKIE_USERNAME].value
      sessionid = cookie[COOKIE_SESSIONID].value
  return username, sessionid


def remember_last_page(langid, lastpage, pageoptions):
    """Record what page and page options the user viewed last"""
    request = "UPDATE users SET lastpage = \"%s\", lastpageoptions = \"%s\" WHERE langid=\"%s\"" % (lastpage, pageoptions, langid)
    db = connect_to_db("rw")
    cursor = db.cursor()
    cursor.execute(request)
    cursor.close()
    db.close()


def recall_last_page(langid):
    """return the lastpage and lastpageoptions for the given language id"""
    request = "SELECT lastpage, lastpageoptions FROM users WHERE langid=\"%s\"" % langid
    db = connect_to_db("rw")
    cursor = db.cursor()
    cursor.execute(request)
    row = cursor.fetchone()
    cursor.close()
    db.close()
    return row


def main():
  xhtml_die("<p>Don't call this directly!</p>")

if __name__ == "__main__": main()
