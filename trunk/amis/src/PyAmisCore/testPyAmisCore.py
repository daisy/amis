import amiscore
import os
import sys

def main():
    book = amiscore.Dtb()
    bmkdir = os.path.join(os.getenv("HOME"), "bmk")
    if not os.path.exists(bmkdir):
        os.mkdir(bmkdir)
    if len(sys.argv) > 1:
        nccfile = sys.argv[1]
    else:
        nccfile = '/Users/jack/Sites/Mountains_skip/ncc.html'
    rv = book.open(nccfile, bmkdir)
    print 'book.open returned', rv
    # avoid segfault when book cannot be opened (JQ)
    if rv:
        p_data = book.startReading(False)
        if p_data:
            #p_data.print_()
            printContextualInfo(book, p_data)
        else:
            print 'startReading returned', p_data
        doPhraseNavInputLoop(book)
    
def doPhraseNavInputLoop(book):
    while True:
        print "Type (n)ext, (p)previous, (s)earch or (q)uit:",
        cmd = sys.stdin.readline()
        cmd = cmd.strip()
        if cmd == "n":
            nextPhrase(book)
        elif cmd == "p":
            prevPhrase(book)
        elif cmd == "s":
            searchText(book)
        elif cmd == "q":
            break
        else:
            print "Unknown command:", cmd
            
def nextPhrase(book):
    p_data = book.nextPhrase()
    if p_data:
        #p_data.print_()
        printContextualInfo(book, p_data)

def prevPhrase(book):
    p_data = book.prevPhrase()
    if p_data:
        #p_data.print_()
        printContextualInfo(book, p_data)

def searchText(book):
    print "Type text to search for:",
    search = sys.stdin.readline()
    search.strip()
    result = book.searchFullText(search)
    if not result:
        print "Not found!!"
        return
    p_data = book.loadSmilFromUrl(result)
    if p_data:
        #p_data.print_()
        printContextualInfo(book, p_data)
    while True:
        print "Search for same text again?  (f)orward or (r)everse direction, or (q)uit:",
        cmd = sys.stdin.readline()
        cmd = cmd.strip()
        result = None
        if cmd == "f":
            result = book.searchFullTextNext()
        elif cmd == "r":
            result = book.searchFullTextPrevious()
        elif cmd == "q":
            return
        else:
            print "Unknown command:", cmd
            return
        if result:
            p_data = book.loadSmilFromUrl(result)
            if p_data:
                #p_data.print_()
                printContextualInfo(book, p_data)
        else:
            print "Not found!"
            
def printContextualInfo(pDtb, pData):
    p_nav = pDtb.getNavModel()
    p_files = pDtb.getFileSet()
    smilfile = p_files.getSmilFile()
    # xxxx clear target, get filename
    p_container = p_nav.getNavMap()
    if p_container:
        print "Section:",
        smilref = smilfile + '#' + pData.getId()
        p_node = p_nav.getNodeForSmilId(smilref, p_container)
        if p_node:
            print p_node.getLabel().getText().getTextString()
        else:
            print "Not found!"
            
if __name__ == "__main__":
    main()
    
