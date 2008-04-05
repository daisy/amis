#!/usr/bin/python
import MySQLdb
import cgi
import datetime
import sys
import cgitb; cgitb.enable()
# DB connection info (hardcoded)
sys.path.append("/home/daisyfor")
sys.path.append("../")
from l10n import *
from pagepieces import *
from page import show_last_page, is_excluded

def main():
    """Do something and go back to the previous page"""
    form = cgi.FieldStorage()
    if not form.has_key("action") or not form.has_key("pageid"):
        xhtml_die("<p>Error with submission form %s </p>." % str(form))
    if form["action"].value == "save":
        save(form)
    elif form["action"].value == "checkconflicts":
        page_id = form["pageid"].value
        if page_id == "mnemonics":
            check_mnemonic_conflicts(form)
        elif page_id == "accelerators":
            check_accelerator_conflicts(form)


def save(form):
    """save a single item"""
    if not form.has_key("langid") or not form.has_key("xmlid") or not form.has_key("pageid"):
        xhtml_die("<p>Error with submission form %s </p>" % str(form))
    
    pageid = form["pageid"].value
    langid = form["langid"].value
    xmlid = form["xmlid"].value
    
    status_key = "%s_status" % xmlid
    if form.has_key(status_key):
        status = form[status_key].value
    else:
        status = 1
    
    if form.has_key("translation"):
        textstring = form["translation"].value
    else:
        textstring = ""
    
    if form.has_key("remarks"):
        remarks = form["remarks"].value
    else:
        remarks = ""
    
    if pageid == "accelerators" and form.has_key("dataprefix"):
        data_prefix = form["dataprefix"].value
        textstring = data_prefix + textstring
            
    table = langid.replace("-", "_")
    db = connect_to_db("rw")
    cursor = db.cursor()
    request = """
    UPDATE %(table)s SET textflag="%(status)s", textstring="%(translation)s", remarks="%(remarks)s" 
    WHERE xmlid="%(xmlid)s" """ % \
    {"table": table, "status": status, "translation": textstring, "remarks": remarks, "xmlid": xmlid}
    cursor.execute(request)
    cursor.close()
    db.close()
    show_last_page(langid, "")


def check_mnemonic_conflicts(form):
    """check the mnemonic groups for conflicts"""
    if not form.has_key("langid"):
        xhtml_die("<p>Error with submission form %s </p>." % str(form))
    
    langid = form["langid"].value
    table = langid.replace("-", "_")
    request = "SELECT DISTINCT mnemonicgroup FROM %s WHERE mnemonicgroup >= 0" % table
    db = connect_to_db("ro")
    cursor = db.cursor()
    cursor.execute(request)
    rows = cursor.fetchall()
    # for each mnemonic group, make sure that each entry is unique
    conflict_found = False
    warning = ""
    for r in rows:
        group_id = r[0]
        #first get all the items in this mnemonic group
        request = "SELECT id FROM %s WHERE mnemonicgroup = %d" % (table, group_id)
        cursor.execute(request)
        first_set = cursor.fetchall()
        #then get all unique textstrings from this mnemonic group.  
        #this relies on MYSQL being case-insensitive in its comparisons.
        request = "SELECT DISTINCT textstring FROM %s WHERE mnemonicgroup = %d" % (table, group_id)
        cursor.execute(request)
        second_set = cursor.fetchall()
        #if they returned different numbers of results, then probably a mnemonic was repeated
        if len(first_set) != len(second_set):
            group_link = "group_" + str(group_id)
            warning = warning + "<br/>" + (WARNINGS["mnemonics"] % {"link": group_link})
            conflict_found = True
    cursor.close()
    db.close()
    if conflict_found == False:
        warning = "<br/>No conflicts found."
    show_last_page(langid, warning)


def check_accelerator_conflicts(form):
    """
    Look for accelerator key conflicts.  This function checks all accelerators, not just the ones from the form, and
    it ignores Space because that one is used twice (play/pause) and is not allowed to be changed by the user.
    """
    
    if not form.has_key("langid"):
        xhtml_die("<p>Error with submission form %s </p>" % str(form))
    
    langid = form["langid"].value
    table = langid.replace("-", "_")
    request = "SELECT DISTINCT textstring FROM %s WHERE role=\"ACCELERATOR\" and textstring != \"Space\"" % table
    db = connect_to_db("ro")
    cursor = db.cursor()
    cursor.execute(request)
    first_count = cursor.rowcount
    request = "SELECT id FROM %s WHERE role=\"ACCELERATOR\" and textstring != \"Space\"" % table
    cursor.execute(request)
    second_count = cursor.rowcount
    if first_count != second_count:
        warning = WARNINGS["accelerators"] + "<br/>"
    else:
        warning = "<br/>No conflicts found."
    show_last_page(langid, warning)

    
# main entry point
if __name__ == "__main__": 
    main()
