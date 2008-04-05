#!/usr/bin/python

import cgi

HTMLNS = "http://www.w3.org/1999/xhtml"

def print_head():
  """Print the head of an HTML document"""
  print """<?xml version="1.0"?>
<html xmlns=\"""" + HTMLNS + """\">
  <head>
    <title>Amis localization interface</title>
    <script type="text/javascript" src="/l10ndocs/transform.js"></script>
    <script type="text/javascript" src="/l10ndocs/form.js"></script>
    <link rel="stylesheet" href="/l10ndocs/form.css" type="text/css"/> 
  </head>"""

def print_error(msg):
  """Print an error document"""
  print """  <body>
    <h1>Amis localization interface</h1>
    <p>Error: """ + msg + """</p>
  </body>
</html>"""

def print_choose_form(lang):
  """Print a form chooser dialog"""
  print """  <body>
    <h1>Amis localization interface</h1>
    <p>Choose a translation form below:</p>
    <form action="/cgi-bin/form.py" method="get">
      <input type="hidden" name="lang" value=\"""" + lang + """\"/>
      <p>
        <select name="form">
          <option value="container_file">Container &#8220;File&#8221; (file)</option>
          <option value="container_view">Container &#8220;View&#8221; (view)</option>
          <option value="container_play">Container &#8220;Play&#8221; (play)</option>
          <option value="container_navigate">Container &#8220;Navigate&#8221; (navigate)</option>
          <option value="container_bookmarks">Container &#8220;Bookmarks&#8221; (bookmarks)</option>
          <option value="container_info">Container &#8220;Info&#8221; (info)</option>
          <option value="container_help">Container &#8220;Help&#8221; (help)</option>
          <option value="dialog_IDD_FIND">Dialog &#8220;Find in Text&#8221; (IDD_FIND)</option>
          <option value="dialog_IDD_CDSEARCH">Dialog &#8220;Multiple books found&#8221; (IDD_CDSEARCH)</option>
          <option value="dialog_IDD_ABOUT">Dialog &#8220;About AMIS&#8221; (IDD_ABOUT)</option>
          <option value="dialog_IDD_SEARCHDAISY">Dialog &#8220;Search for DAISY Books&#8221; (IDD_SEARCHDAISY)</option>
          <option value="dialog_IDD_SKIP">Dialog &#8220;Skippable Items&#8221; (IDD_SKIP)</option>
          <option value="dialog_IDD_PROPPAGE4">Dialog &#8220;Language&#8221; (IDD_PROPPAGE4)</option>
          <option value="dialog_IDD_PROPPAGE2">Dialog &#8220;Set Colors&#8221; (IDD_PROPPAGE2)</option>
          <option value="dialog_IDD_CURRENTPOSITION">Dialog &#8220;Current Position&#8221; (IDD_CURRENTPOSITION)</option>
          <option value="dialog_IDD_PROPPAGE1">Dialog &#8220;General Preferences&#8221; (IDD_PROPPAGE1)</option>
          <option value="dialog_IDD_NAVIGATIONOPTIONS">Dialog &#8220;Navigation Options&#8221; (IDD_NAVIGATIONOPTIONS)</option>
          <option value="dialog_IDD_GOTOPAGE">Dialog &#8220;Go To Page&#8221; (IDD_GOTOPAGE)</option>
          <option value="dialog_IDD_FULLSUMMARY">Dialog &#8220;Publication Summary&#8221; (IDD_FULLSUMMARY)</option>
          <option value="promptItemRefs">Prompt item references</option>
          <option value="generalPrompts">General Prompts</option>
        </select>
        <input type="submit" value="Go"/>
      </p>
    </form>
  </body>
</html>"""

def print_transform_body(type, id, lang, url):
  """Print the body stub for client-side transform"""
  print """  <body onload="transform('""" + type + """', '""" + id + \
      """', '""" + lang + """', '""" + url + """');">
    <p id="msg">Sorry, it seems that we are unable to work with your browser
    :(</p>
  </body>
</html>"""

print "Content-type: text/html; charset=UTF-8\n\n",
print_head()
form = cgi.FieldStorage()
if (form.__contains__("lang")):
  lang = form["lang"].value
  if (form.__contains__("form")):
    split = form["form"].value.split("_", 1)
    url = ""
    if (split.__len__() == 2):
      print_transform_body(split[0], split[1], lang, url)
    else:
      print_transform_body(split[0], "", lang, url)
  else:
    print_choose_form(form["lang"].value)
else:
  print_error("lang parameter is not defined.")
