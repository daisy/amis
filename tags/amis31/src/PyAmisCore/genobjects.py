FORMAT="""class %(pname)sObjectDefinition(MyGlobalObjectDefinition):
    %(bname)s
    
%(pname)s_object = %(pname)sObjectDefinition('%(pname)s', '%(pname)sObj', '%(fqcname)s*')
methods_%(pname)s = []
module.addobject(%(pname)s_object)

%(pname)s_ptr = OpaqueByValueType('%(fqcname)s*', '%(pname)sObj')
%(fqpname)s_ptr = %(pname)s_ptr
const_%(pname)s_ptr = OpaqueByValueType('const %(fqcname)s*', '%(pname)sObj')
const_%(fqpname)s_ptr = const_%(pname)s_ptr
"""

INCLFORMAT="""extern PyObject *%(pname)sObj_New(%(fqcname)s* itself);
extern int %(pname)sObj_Convert(PyObject *v, %(fqcname)s* *p_itself);
"""


OBJECTS=[
    "Error.h",
    ("Error", ("amis",), "Error", None),
    
    "Media.h",
    ("MediaNode", ("amis",), "MediaNode", None),
    ("TextNode", ("amis",), "TextNode", "MediaNode"),
    ("AudioNode", ("amis",), "AudioNode", "MediaNode"),
    ("ImageNode", ("amis",), "ImageNode", "MediaNode"),
    ("MediaGroup", ("amis",), "MediaGroup", None),
    
    "ModuleDescData.h",
    ("ModuleDescData", ("amis",), "ModuleDescData", None),
    
    "RecentBooks.h",
    ("RecentBookEntry", ("amis",), "RecentBookEntry", None),
    ("RecentBooks", ("amis",), "RecentBooks", None),
    
    "dtb/Bookmarks.h",
    ("PositionData", ("amis","dtb"), "PositionData", None), # XXXX struct really
    ("PositionMark", ("amis","dtb"), "PositionMark", None), # XXXX struct really
    ("Bookmark", ("amis","dtb"), "Bookmark", "PositionMark"), # XXXX struct really
    ("Hilite", ("amis","dtb"), "Hilite", "PositionMark"), # XXXX struct really
    ("BookmarkSet", ("amis","dtb"), "BookmarkSet", None),
    
    "dtb/CustomTest.h",
    ("CustomTest", ("amis","dtb"), "CustomTest", None),
    ("CustomTestSet", ("amis","dtb"), "CustomTestSet", None),
    
    "dtb/Dtb.h",
    ("Dtb", ("amis","dtb"), "Dtb", None),
    
    "dtb/DtbFileSet.h",
    ("DtbFileSet", ("amis","dtb"), "DtbFileSet", None),
    
    "dtb/Metadata.h",
    ("MetaItem", ("amis","dtb"), "MetaItem", None), # XXXX struct really
    ("Metadata", ("amis","dtb"), "Metadata", None),
    
    "dtb/Spine.h",
    ("Spine", ("amis","dtb"), "Spine", None),
    
    "dtb/nav/NavContainer.h",
    ("NavContainer", ("amis", "dtb", "nav"), "NavContainer", None),

    "dtb/nav/NavList.h",
    ("NavList", ("amis", "dtb", "nav"), "NavList", "NavContainer"),

    "dtb/nav/NavMap.h",
    ("NavMap", ("amis", "dtb", "nav"), "NavMap", "NavContainer"),

    "dtb/nav/NavModel.h",
    ("NavModel", ("amis", "dtb", "nav"), "NavModel", None),

    "dtb/nav/NavNode.h",
    ("NavNode", ("amis", "dtb", "nav"), "NavNode", None),

    "dtb/nav/NavPoint.h",
    ("NavPoint", ("amis", "dtb", "nav"), "NavPoint", "NavNode"),

    "dtb/nav/NavTarget.h",
    ("NavTarget", ("amis", "dtb", "nav"), "NavTarget", "NavNode"),

    "dtb/nav/PageList.h",
    ("PageList", ("amis", "dtb", "nav"), "PageList", "NavList"),

    "dtb/nav/PageTarget.h",
    ("PageTarget", ("amis", "dtb", "nav"), "PageTarget", "NavTarget"),
    
    "dtb/smil/SmilMediaGroup.h",
    ("SmilMediaGroup", ("amis", "dtb", "smil"), "SmilMediaGroup", "MediaGroup"),
    
    "dtb/smil/SmilTree.h",
    ("SmilTree", ("amis", "dtb", "smil"), "SmilTree", "MediaGroup"),
    
    "io/ModuleDescReader.h",
    ("ModuleDescReader", ("amis","io"), "ModuleDescReader", None),
	
]

out = open('amiscoreobjgen.py', 'w')
incout = open('amiscoreincludegen.py', 'w')
modout = open('amiscoremodule.h', 'w')
for item in OBJECTS:
	if type(item) == type(""):
		print >>out, "# From %s:" % item
		inc = '#include "%s"' % item
		print >>incout, 'includestuff = includestuff + \'' + inc + '\\n\''
		continue
	pname, qnames, cname, bname = item
	fqpname = pname
	fqcname = cname
	if qnames:
	   fqpname = '_'.join(qnames + (pname,))
	   fqcname = '::'.join(qnames + (cname,))
	if bname:
		bname = 'basetype = "%s_Type"\n    baseclass = "%s"' % (bname, bname)
	else:
		bname = 'pass'
	print >>out, FORMAT % locals()
	print >>modout, INCLFORMAT % locals()
	
