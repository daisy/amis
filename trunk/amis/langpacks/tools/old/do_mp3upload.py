#!/usr/bin/python
import cgi
import cgitb; cgitb.enable()
from l10n import *

#######################################
#######################################
####  HUGE NOTE:
####  We're not using this file!  See "upload.py" instead
#######################################
#######################################


def main():
	"""get the user info from the form and validate it"""
	form = cgi.FieldStorage()
	#make sure the form has what we expect
	if form != None and form.has_key("datafile") and form["datafile"].value != "":
		uploadData(form)
		return
	else:
		xhtml_die("<p>So sorry -- there was an error.</p>")
		return

def uploadData(form):
    """upload mp3 data to the database """
    stuff = getData(form)
   """ db = connect_to_db("rw")
    cursor=db.cursor()
    #connect = "UPDATE eng_US SET audiodata=NULL, remarks=\"M\" WHERE id=1"
    connect = "UPDATE eng_US SET audiodata=\"" + str(stuff) + "\", remarks=\"marisa was here\" WHERE id=1"
    cursor.execute(connect)
    cursor.close()
    db.close()
    xhtml_out("upload mp3", "reached the end of the uploadData function!")"""
    return 

def getData(form):
    """read the file into a stream and save it to the server"""
    fileitem=form["datafile"]
    if not fileitem.file: return "not fileitem.file"
    fout = file (os.path.join("/public_html/amisproject.org/cgi-bin", fileitem.filename), 'wb')
    while 1:
        chunk = fileitem.file.read(100000)
        if not chunk: break
    fout.write (chunk)
    fout.close()
    return "end"

#main entry point
if __name__ == "__main__": main()

