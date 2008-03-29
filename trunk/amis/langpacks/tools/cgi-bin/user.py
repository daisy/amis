#!/usr/bin/python
import cgitb; cgitb.enable()
from l10n import *

user = get_user()
if user == None:
  xhtml_die(NOT_LOGGED_IN)
else:
  xhtml_out("OK", "<p>Hello %(luser)s. You are working on %(lang)s.</p>" % \
    {"luser": user["realname"], "lang": user["langname"]})
