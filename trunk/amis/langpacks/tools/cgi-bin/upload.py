#!/usr/bin/python
import cgi
import cgitb; cgitb.enable()
import os, sys
from l10n import *

UPLOAD_DIR = "/home/daisyfor/public_html/amisproject.org/translate/temp"
TEMP_FILE = "/home/daisyfor/public_html/amisproject.org/translate/temp/file.txt"

def save_uploaded_file():
    """
    This saves a file uploaded by an HTML form.
    The form_field is the name of the file input field from the form.
    For example, the following form_field would be "file_1":
    <input name="file_1" type="file">
    The upload_dir is the directory where the file will be written.
    If no file was uploaded or if the field does not exist then
    this does nothing.
    """
    form = cgi.FieldStorage()
    if not form.has_key("file_1"): 
        return "Form error % s" % str(form)
    filename = form["file_1"].filename
    filedata = form["file_1"].value
    
    if filename != None and filename != "":
        server_filepath = os.path.join(UPLOAD_DIR, filename)
        f = open(server_filepath, 'wb', 10000)
        f.write(filedata)
        f.close()
        return server_filepath
    else:
        return "Error, here is the form, you try to figure it out. %s" % str(form)


def main():
    msg = save_uploaded_file()
    xhtml_out("OK", "<p>%s</p>" % msg)


if __name__ == "__main__": 
    main()
