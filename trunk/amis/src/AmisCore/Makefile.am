SUBDIRS=testAmisCore
lib_LTLIBRARIES = libAmisCore.la

INCLUDES = -I$(srcdir)/include \
    $(XERCES_INCLUDES) \
    $(AMBULANT_INCLUDES)

libAmisCore_la_SOURCES = \
	src/AmisCore.cpp \
	src/dtb/Bookmarks.cpp \
	src/dtb/CustomTest.cpp \
	src/dtb/Dtb.cpp \
	src/dtb/DtbFileSet.cpp \
	src/dtb/Metadata.cpp \
	src/dtb/nav/NavContainer.cpp \
	src/dtb/nav/NavList.cpp \
	src/dtb/nav/NavMap.cpp \
	src/dtb/nav/NavModel.cpp \
	src/dtb/nav/NavNode.cpp \
	src/dtb/nav/NavPoint.cpp \
	src/dtb/nav/NavTarget.cpp \
	src/dtb/nav/NavVisitor.cpp \
	src/dtb/nav/PageList.cpp \
	src/dtb/nav/PageTarget.cpp \
	src/dtb/smil/ContentNode.cpp \
	src/dtb/smil/Node.cpp \
	src/dtb/smil/NodeFactory.cpp \
	src/dtb/smil/ParNode.cpp \
	src/dtb/smil/SeqNode.cpp \
	src/dtb/smil/SmilMediaGroup.cpp \
	src/dtb/smil/SmilTree.cpp \
	src/dtb/smil/SmilVisitor.cpp \
	src/dtb/smil/TimeContainerNode.cpp \
	src/dtb/Spine.cpp \
	src/Error.cpp \
	src/io/BookmarksFileIO.cpp \
	src/io/ModuleDescReader.cpp \
	src/io/NavFileReader.cpp \
	src/io/NccFileReader.cpp \
	src/io/NcxFileReader.cpp \
	src/io/OpfFileReader.cpp \
	src/io/QuickDataSmilFileReader.cpp \
	src/io/RecentBooksFileIO.cpp \
	src/io/SmilFileReader.cpp \
	src/io/TextSearch.cpp \
	src/io/XercesSaxParseBase.cpp \
	src/io/UrlInputSource.cpp \
	src/Media.cpp \
	src/ModuleDescData.cpp \
	src/RecentBooks.cpp \
	src/util/FilePathTools.cpp
