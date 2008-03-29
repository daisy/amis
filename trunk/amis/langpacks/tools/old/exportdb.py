#!/usr/bin/python

import libxml2, MySQLdb, sys
import os
sys.path.append("/home/daisyfor/")
from DB.connect import *

DEFAULT_LOCATION = "/home/daisyfor/public_html/amisproject.org/translate/amisAccessibleUi.xml"
TEMP_LOCATION = "/home/daisyfor/public_html/amisproject.org/translate/temp"

def main ():
    #don't call this directly!  it's just for testing.
    sys.stdout.write("File exported to: " + export("spa_ES"))
    
def export(langid):
    """Export the language to an XML file"""
    document = libxml2.parseFile(DEFAULT_LOCATION)
    internal_export(langid.replace("-", "_"), document)
    file_location = os.path.join(TEMP_LOCATION, langid + ".xml")
    document.saveTo(file_location)
    document.freeDoc()
    return file_location
    
def internal_export(langtable, document):
  """Export the langid table to the XML document passed as second argument."""
  db = connect_to_db_from_localhost("ro")
  cursor = db.cursor()
  cursor.execute("SELECT id, textstring, audiouri FROM %s" % langtable)
  context = document.xpathNewContext()
  rows = cursor.fetchall()
  for row in rows:
    id, textstring, audiouri = row
    try:
      for node in context.xpathEval('//text[@id="%s"]' % id):
        node.children.replaceNode(document.newDocText(textstring))
        context.setContextNode(node)
        for n in context.xpathEval("../audio"):
          n.setProp("src", audiouri)
    except Exception, e:
      sys.stderr.write("No match for %s (%s)\n" % (id, e))
  context.xpathFreeContext()

if __name__ == "__main__": main()
