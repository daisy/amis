Language packs go in subdirectories of this directory.
The subdirectories should be named as ISO-639.2 plus country code.  For example:
	/eng-US
	/fre-FR
	/hin-IN
	
	For languages with no country association, just use a three-letter code.  E.g. "arb" for Arabic.
	
The langpacks contain a resource DLL, moduleDesc.xml file, label resources (flag and recording of language name), a small DTB which is played upon startup, and a translated copy of the AMIS help DTB.