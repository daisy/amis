#!/usr/bin/python
import MySQLdb
import cgi
import datetime
import sys
import cgitb; cgitb.enable()

# DB connection info (hardcoded)
sys.path.append("/home/daisyfor")
from DB.connect import *
from l10n import *

def main():
	"""entry point into this script
	
		if the form key "action" is "showForm", then the form is displayed.
		if the form key "action" is "saveEntry", then one entry is saved.
	"""
	#check if the user is logged-in
	user = get_user()
	if user == None:
		xhtml_die(NOT_LOGGED_IN)
		return
	
	#get the submitted form
	form = cgi.FieldStorage()
	if form == None or not form.has_key("action") or not form.has_key("viewtype"):
		xhtml_die("<p>Error with x submission form." + str(form))
		return
	
	viewtype = form["viewtype"].value
    
	#decide what to do
	if form["action"].value == "showForm":
		showForm(viewtype)
	elif form["action"].value == "saveEntry":
		saveEntry(form, viewtype)
	
	return
    
def showForm(viewtype):	
	"""show the big formw with all the translation boxes"""
	user = get_user()
	langid = user["langid"]
	
	dropdown = """<form action="/cgi-bin/webform.py" method="POST">
	<select name="viewtype">
	<option value="all">all items</option>
	<option value="newtodo">everything not "ok"</option>
	<option value="new">everything "new"</option>
	<option value="todo">everything "todo" </option>
	</select>
	<input type="submit" value="Change view" />
	<input type="hidden" name="action" value="showForm" />
	</form>"""
	
	htmlForm, numrows = makeHtmlFormTable(langid, viewtype)
	
	if viewtype=="all":
		viewtypeDesc = "all items"
	elif viewtype == "newtodo":
		viewtypeDesc = "all items marked new or todo"
	elif viewtype == "new":
		viewtypeDesc = "all new items"
	elif viewtype == "todo":
		viewtypeDesc = "all todo items"
	
	
	pageTitle = "Translation for AMIS : " + user["langname"]
	pageHeading = """<h1>%(pageTitle)s</h1><p>%(num)s items</p><p>Viewing %(viewtype)s</p>""" \
	% {"pageTitle": pageTitle, "num": numrows, "viewtype": viewtypeDesc}
	
	pageBody = """%(heading)s
	<div id="selectview">%(dropdown)s</div>
	<br/>
	<table>%(table)s</table>""" % \
	{"heading": pageHeading, "dropdown": dropdown, "table": htmlForm}

	xhtml_out(pageTitle, pageBody)
	return
	
def makeHtmlFormTable(langid, viewtype):
	"""make the html page with form entry fields for each database row in the language's table"""
	db = connect_to_db("rw")
	if db == None: 
		xhtml_die("<p>Error connecting to database.</p>")
		return
	
	#change hyphens to underscores to look up the right table in the database
	tableId = langid.replace("-", "_")
	
	connect = "SELECT * FROM " + tableId
	#figure out textflag (and maybe textflag2)
	if viewtype == "new":
		connect += " WHERE textflag = '3'"
	elif viewtype == "todo":
		connect += " WHERE textflag = '2'"
	elif viewtype == "newtodo":
		connect += " WHERE textflag = '2' OR textflag = '3'"
	
	cursor=db.cursor()
	cursor.execute(connect)
	# get the number of rows in the resultset
	numrows = int(cursor.rowcount)
	allrows = cursor.fetchall()
	cursor.close()
	htmlForm = ""
	# process one row at a time
	for x in range(0,numrows):
		htmlForm += """<tr>%s</tr>""" % (processOneRow(allrows[x], db, langid, viewtype))
	
	db.close()
	return htmlForm, numrows

def processOneRow(row, db, langid, viewtype):
	"""gather all data for a single translate-able item"""
	if row != None and len(row) > 0:
		id, xpath, textstring, audiodata, audiouri, textflag, audioflag, remarks = row;
		engString, engRemarks = getEnglishReferenceData(str(id), db)
		
		#decide which radio button to check by default
		textok=""
		texttodo=""
		if (textflag==1): 
			textok = "checked"
			texttodo=""
		elif (textflag==2): 
			textok=""
			texttodo="checked"
		
		
		#otherwise it crashes
		if (remarks == None): remarks = ""
		if (engRemarks == None): engRemarks = ""
		
		#get the fields for a single entry
		formcode = makeHtmlForOneRow(engString, engRemarks, textstring, remarks, str(id), textok, texttodo, langid, viewtype)
		return formcode
	return ""

def makeHtmlForOneRow(engString, engRemarks, textstring, remarks, id, ok, todo, langid, viewtype):
	"""make the html form fields for a single entry"""
	formrow = """<form action="/cgi-bin/webform.py" method="POST">
	<td width="30%%">%(eng_string)s</td>
	<td>
		<p class="comments">%(eng_remarks)s</p>
		Enter the translation:<br/>
		<textarea id="%(id)s" name="translation" cols="64" rows="6">%(translation)s</textarea>
		
		<p class="comments">Remarks from you:</p>
		<textarea class="remarks" name="remarks" cols="64" rows="3">%(translator_remarks)s</textarea>
	</td>
	<td>
		<p>Status:</p>
		<input name="%(id)s_status" type="radio" %(text_status_ok)s value="1"/>
		<span class="ok">OK</span><br/>
		
		<input name="%(id)s_status" type="radio" %(text_status_todo)s value="2"/>
		<span class="todo">TODO</span><br/>
		
	</td>
	<td>
		<input type="submit" value="Save" style="align: bottom"/>
		<input type="hidden" name="itemid" value="%(id)s"/>
		<input type="hidden" name="langid" value="%(langid)s"/>
		<input type="hidden" name="action" value="saveEntry"/>
		<input type="hidden" name="viewtype" value="%(viewtype)s"/>
	</td>
</form>""" % \
{"eng_string": engString, "eng_remarks": engRemarks, "id": id, "translation": textstring, \
"translator_remarks": remarks, "text_status_ok": ok, "text_status_todo": todo, "langid": langid, "viewtype": viewtype}
	return formrow

def getEnglishReferenceData(id, db):
	"""get the english string and remarks based on id"""
	cursor = db.cursor()
	connect = "SELECT textstring, remarks FROM eng_US WHERE id =\"" + id + "\""
	cursor.execute(connect)
	row = cursor.fetchone()
	cursor.close()
	x,y = row
	return x,y

def saveEntry(form, viewtype):
	"""this function saves a single entry from the big html form"""
		
	if form == None or not form.has_key("langid") or not form.has_key("itemid"):
		xhtml_die("<p>Error with submission form." + str(form))
		return
		
	db = connect_to_db("rw")
	if db == None: 
		xhtml_die("<p>Error connecting to database.</p>")
		return
	
	langid = form["langid"].value
	itemid = form["itemid"].value
	status = form["%s_status"%(itemid)].value
	if form.has_key("translation"): 
		textstring = form["translation"].value
	else:
		textstring = ""
	if form.has_key("remarks"):
		remarks = form["remarks"].value
	else:
		remarks = ""
	
	#change hyphens to underscores to look up the right table in the database
	langTableId = langid.replace("-", "_")
	
	cursor = db.cursor()
	sql_update = """UPDATE %(table)s SET textflag= "%(status)s", textstring= "%(translation)s", remarks = "%(remarks)s" WHERE id= "%(id)s" """ % \
	{"table": langTableId, "status": status, "translation": textstring, "remarks": remarks, "id": itemid}
	cursor.execute("set autocommit = 1")
	cursor.execute(sql_update)
	cursor.close()
	db.close()
	
	showForm(viewtype)
	
	return

	
#main entry point
if __name__ == "__main__": main()