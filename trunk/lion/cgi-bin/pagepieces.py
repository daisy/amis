# What each view on the main page is for
VIEW_FILTER_DESCRIPTIONS = {
"all": "all items", 
"newtodo": "all items marked new or to-do", 
"new": "all new items", 
"todo": "all to-do items"}

# What each section on the main page is for
SECTION_DESCRIPTIONS = {
"main": "This is the main page.  All the phrases to be translated for the AMIS interface are on this page.", 
"mnemonics": "This is the mnemonics page.  Mnemonics are shortcut letters in a menu item or button.", 
"accelerators": "Accelerators are keyboard shortcuts.  Please translate the shortcut exactly as it appears.",
"accelerators_next": """
This is the accelerators page. Accelerators are keyboard shortcuts for program actions.  
You may use A-Z (U.S. Ascii characters), plus (+), minus (-), Up, Down, Left, Right, or Esc for your custom shortcuts. 
<p class="warning">!! Changing the accelerators is supported only in AMIS 3 and higher.  You can enter accelerators here, but they won't be exported for use in AMIS 2.6.</p>
"""
}

INSTRUCTIONS = {
"main": "Enter the translation:<br/>",
"mnemonics": "Enter a single letter:<br/>",
"accelerators": "Enter a translation for the key:<br/>",
"accelerators_next": "Enter a key combination (A-Z/+/-/Up/Down/Left/Right/Esc):<br/>"
}

ROLE_DESCRIPTIONS = {
"CONTROL": "control, such as a button",
"DIALOG": "title of a dialog window",
"STRING": "general string",
"MENUITEM": "menu item",
"ACCELERATOR": "keyboard shortcut",
"MNEMONIC": "letter (underlined) to press to activate the item"
}
# Title format
TITLE_TEMPLATE = """AMIS translation for %(lang)s: %(section)s section"""

# Warnings format
WARNINGS = {
"mnemonics": """<p class="warning">There is a conflict in <a href="#%(link)s">this group</a></p>""", 
"accelerators": """<p class="warning">There is a conflict because two commands are using the same keyboard shortcut.</p>"""}

# URL of the login form
LOGIN_URL = "http://amisproject.org/translate/login.html"

# Standard error message for not being logged in
NOT_LOGGED_IN = """
<p>You do not appear to be logged in.
Please <a href="%s">try again</a>.</p>""" % LOGIN_URL

# XHTML output
XHTML_TEMPLATE = """Content-type: text/html; charset=utf-8

<?xml version="1.0"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xml:lang="en" lang="en" xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>%(TITLE)s</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <link href="/translate/l10n.css" rel="stylesheet" type="text/css"/>
  </head>
  <body>%(BODY)s</body>
</html>"""

# The dropdown for selecting a view on the main page
# note that the pageid still needs to be filled in at runtime
VIEW_FILTER_DROPDOWN_HTML = """
<form action="/cgi-bin/page.py" method="POST">
<select name="viewfilter">
<option value="all">%(all)s</option>
<option value="newtodo">%(newtodo)s</option>
<option value="new">%(new)s</option>
<option value="todo">%(todo)s</option>
</select>
<input type="submit" value="Change view" />
<input type="hidden" name="pageid" value="%(pageid)s" />
</form>""" % \
{"all": VIEW_FILTER_DESCRIPTIONS["all"], 
"newtodo": VIEW_FILTER_DESCRIPTIONS["newtodo"], 
"new": VIEW_FILTER_DESCRIPTIONS["new"], 
"todo": VIEW_FILTER_DESCRIPTIONS["todo"],
"pageid": "%(pageid)s"}

# Each page will use this body template (except error pages)
BODY_TEMPLATE_HTML = """
<h1>%(title)s</h1>
<div id="navigation">
<a href="page.py?pageid=main">Main</a> | 
<a href="page.py?pageid=mnemonics">Mnemonics</a> | 
<a href="page.py?pageid=accelerators">Accelerators</a>
</div>
<p class="description">%(about_section)s  There are %(num_items)d items on this page.</p>
<div>%(view_filter)s</div>
<br/>
<p>%(message)s</p>
<div>%(specialform)s</div>
<div>%(warning)s</div>
<div>%(form)s</div>
"""
# some pages have the button that checks for conflicts
CONFLICT_BUTTONS = {
"main": "",
"mnemonics": """
<form action="/cgi-bin/actions.py" method="POST">
Press to check for conflicts in each mnemonic group.  Save your work first!
<input type="submit" value="Check for conflicts"/>
<input type="hidden" name="action" value="checkconflicts"/>
<input type="hidden" name="langid" value="%(langid)s"/>
<input type="hidden" name="pageid" value="mnemonics"/>
</form>""",
"accelerators": "",
"accelerators_next": """
<form action="/cgi-bin/actions.py" method="POST">
Press to check for conflicts in the keyboard shortcuts.  Save your work first!
<input type="submit" value="Check for conflicts"/>
<input type="hidden" name="action" value="checkconflicts"/>
<input type="hidden" name="langid" value="%(langid)s"/>
<input type="hidden" name="pageid" value="accelerators"/>
</form><br/>"""
}
# A single row of the form used for all pages
FORM_ROW= """<form action="/cgi-bin/actions.py" method="POST">
<td width="30%%">%(ref_string)s</td>
<td>
    <p class="comments">%(our_remarks)s</p>
    %(instructions)s
    <textarea id="%(xmlid)s" name="translation" cols="%(translate_box_columns)d" rows="%(translate_box_rows)d">%(translation)s</textarea>
    <p class="comments">Remarks from you:</p>
	<textarea class="remarks" name="remarks" cols="64" rows="2">%(translator_remarks)s</textarea>
</td>
<td>
	<p>Status:</p>
	<input name="%(xmlid)s_status" type="radio" %(text_status_ok)s value="1"/>
	<span class="ok">OK</span><br/>
	
	<input name="%(xmlid)s_status" type="radio" %(text_status_todo)s value="2"/>
	<span class="todo">TODO</span><br/>
	
</td>
<td>
	<input type="submit" value="Save" style="align: bottom"/>
	<input type="hidden" name="xmlid" value="%(xmlid)s"/>
	<input type="hidden" name="langid" value="%(langid)s"/>
	<input type="hidden" name="pageid" value="%(pageid)s"/>
	<input type="hidden" name="dataprefix" value="%(dataprefix)s"/>
	<input type="hidden" name="action" value="save"/>
</td>
</form>
"""


MNEMONIC_GROUP_HEADING = """<h2 id=\"group_%(mnemonicgroup)d\">Group #%(num)d</h2>"""