#!/usr/bin/python

# Manage the database

import getopt
import os

# Harcoded DB connection info, not stored in SVN
os.sys.path.append("../")
from DB.connect import *

def db_connect():
    """Connect to the database."""
    global connected, db, cursor
    trace_msg("Connecting to the database...")
    if not connected:
        db = connect_to_db_from_local_machine("admin")
        cursor = db.cursor()
        connected = True
        trace_msg("... connected.")
    else:
        trace_msg("... was already connected.")

def db_disconnect():
    """Disconnect from the database."""
    global connected, db, cursor
    trace_msg("Disconnecting from the database...")
    if connected:
        cursor.close()
        db.close()
        connected = False
        trace_msg("... disconnected.")
    else:
        trace_msg("... was not connected.")

def execute_query(cursor, q):
    """Execute a query."""
    trace_msg("""Query: "%s".""" % q)
    cursor.execute(q)

def check_language(langid):
    """Check the existence of a table for the given language id."""
    db_connect()
    global cursor
    execute_query(cursor,
        "SELECT langname FROM languages WHERE langid='%s'" % langid)
    for langname in cursor:
        trace_msg("%s is %s." % (langid, langname))
        return True
    return False

def trace_msg(msg):
    """Output a trace message if the trace flag is set."""
    global trace
    if trace:
        os.sys.stderr.write("*** %s\n" % msg)

def warn(msg):
    """Report a warning and increment the warnings count."""
    os.sys.stderr.write("Warning: %s\n" % msg)
    global warnings
    warnings += 1

def die(msg, err=1):
    """Die with an error message and an error code. If the code is 0, don't
die just yet."""
    os.sys.stderr.write("Error: %s\n" % msg)
    db_disconnect()
    if err > 0: os.sys.exit(err)

def usage(code=0):
    """Print usage information and exits with an error code."""
    print """
Usage:

  %(script)s --help                              Show this help message.
  %(script)s --import --language=id --file=file  Import file into table id.

Other options:
    --trace, -t: trace mode (send trace messages to stderr.)

""" % {"script": os.sys.argv[0]}
    os.sys.exit(code)

def main():
    """Parse command line arguments and run."""
    global trace, connected, warnings
    trace = False
    connected = False
    warnings = 0
    try:
        opts, args = getopt.getopt(os.sys.argv[1:], "f:hl:t",
            ["file", "help", "language", "trace"])
    except getopt.GetoptError, e:
        die(e.msg, 0)
        usage(1)
    if len(opts) == 0: usage(1)
    for opt, arg in opts:
        if opt in ("-f", "--file"): file = arg
        elif opt in ("-h", "--help"): usage()
        elif opt in ("-l", "--language"):
            langid = arg
            if not check_language(langid):
                die("""No table for language "%s".""" % langid)
        elif opt in ("-t", "--trace"): trace = True
    db_disconnect()

if __name__ == "__main__": main()
