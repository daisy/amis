# Scan an Apple header file, generating a Python file of generator calls.

import sys
import os
import re
from bgenlocations import TOOLBOXDIR, BGENDIR
sys.path.append(BGENDIR)
from bgenCxxSupport import CxxScanner

AMISCORE="../AmisCore/include/"
DO_SCAN=True

def main():
    input = [
        AMISCORE+ "AmisCore.h",
        AMISCORE+ "Error.h",
        AMISCORE+ "Media.h",
        AMISCORE+ "ModuleDescData.h",
        AMISCORE+ "RecentBooks.h",

        AMISCORE+ "dtb/Bookmarks.h",
        AMISCORE+ "dtb/CustomTest.h",
        AMISCORE+ "dtb/Dtb.h",
        AMISCORE+ "dtb/DtbFileSet.h",
        AMISCORE+ "dtb/Metadata.h",
        AMISCORE+ "dtb/Spine.h",
        
        AMISCORE+ "dtb/nav/NavContainer.h",
        AMISCORE+ "dtb/nav/NavList.h",
        AMISCORE+ "dtb/nav/NavMap.h",
        AMISCORE+ "dtb/nav/NavModel.h",
        AMISCORE+ "dtb/nav/NavNode.h",
        AMISCORE+ "dtb/nav/NavPoint.h",
        AMISCORE+ "dtb/nav/NavTarget.h",
##        AMISCORE+ "dtb/nav/NavVisitor.h",
        AMISCORE+ "dtb/nav/PageList.h",
        AMISCORE+ "dtb/nav/PageTarget.h",
        
##        AMISCORE+ "dtb/smil/ContentNode.h",
##        AMISCORE+ "dtb/smil/Node.h",
##        AMISCORE+ "dtb/smil/NodeFactory.h",
##        AMISCORE+ "dtb/smil/ParNode.h",
##        AMISCORE+ "dtb/smil/SeqNode.h",
##        AMISCORE+ "dtb/smil/SmilEngine.h",
##        AMISCORE+ "dtb/smil/SmilEngineConstants.h",
        AMISCORE+ "dtb/smil/SmilMediaGroup.h",
        AMISCORE+ "dtb/smil/SmilTree.h",
##        AMISCORE+ "dtb/smil/SmilVisitor.h",
##        AMISCORE+ "dtb/smil/TimeContainerNode.h",
        
##        AMISCORE+ "io/BookmarksFileIO.h",
        AMISCORE+ "io/ModuleDescReader.h",
##        AMISCORE+ "io/NavFileReader.h",
##        AMISCORE+ "io/NccFileReader.h",
##        AMISCORE+ "io/NcxFileReader.h",
##        AMISCORE+ "io/OpfFileReader.h",
##        AMISCORE+ "io/QuickDataSmilFileReader.h",
##        AMISCORE+ "io/RecentBooksFileIO.h",
##        AMISCORE+ "io/SmilFileReader.h",
##        AMISCORE+ "io/TextSearch.h",
##        AMISCORE+ "io/XercesSaxParseBase.h",
        
##        AMISCORE+ "util/FilePathTools.h",
##        AMISCORE+ "util/findfilecontainer.h",
##        AMISCORE+ "util/xercesutils.h",
            ]
    if DO_SCAN:
        output = "amiscoregen.py"
        defsoutput = None
        scanner = MyScanner(input, output, defsoutput)
        scanner.scan()
        scanner.gentypetest("amiscoretypetest.py")
        scanner.close()
        print "=== Testing definitions output code ==="
        if defsoutput: execfile(defsoutput, {}, {})
    print "=== Done scanning and generating, now importing the generated code ==="
    exec "import amiscoresupport"
    print "=== Done.  It's up to you to compile it now! ==="

       
class MyScanner(CxxScanner):
    silent = 1

    def makeblacklistnames(self):
        return [
           "getLayoutRegions",
           "print", # XXX Lazy for now.
           "MediaNode", # Used only as (semi-) abstract baseclass
           "NavContainer", # ditto
           "NavNode", # ditto
        ]

    def makeblacklisttypes(self):
        return [
            "NavVisitor_ptr", # Visitor interface not exported to Python.
            "SmilVisitor_ptr", # Visitor interface not exported to Python.
            "NodeRefMap", # Not exported to Python (we hope...)
            "NodeRefMap_ptr", # ditto
            "XMLCh_ptr_const_localname", # Jack thinks this one isn't needed
            "XMLCh_ptr_const_qname", # Jack thinks this one isn't needed
            "XMLCh_ptr_const_uri", # Jack thinks this one isn't needed
            "XMLCh_ptrconst", # Jack thinks this one isn't needed
            "amis_StdStringList_ptr", # Not needed from Python (we hope)
            "NavListList_ptr", # Use getNumberOfNavLists and getNavList in stead.
            "Node_ptr", # Not needed from Python (we hope)
            "SeqNode_ptr", # Not needed from Python (we hope)
            "amis_Error", # XXX Lazy: these are passed by value, not ref. Needs work.
            "amis_UrlList_ptr", #Not needed from Python (we hope)
        ]

    def makegreylist(self):
        return [
##            ('#ifdef WITH_QT', [
##                'create_qt_window_factory_unsafe',
##                'create_qt_playable_factory',
##                'create_qt_video_factory',
##                ]
##            ),
        ]

    def makerepairinstructions(self):
        return [
        	# Assume an int pointer is an out-param
##        	(
##        	  [
##        	  	('int_ptr', '*', 'InMode'),
##        	  ], [
##        	    ('int', '*', 'OutMode')
##        	  ]
##        	),

        ]        
if __name__ == "__main__":
    main()
