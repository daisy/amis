The AMIS Localization Web Tools
==
For localizing AMIS via a web interface, the following tools have been created:
	1. A database that stores the translation data
	2. Scripts that import and export XML to and from the database
	3. Scripts that export XML to a templated Microsoft RC file
	4. Various administrative scripts
	5. A web interface for users to edit text strings (and in the future: review mnemonic and accelerator choices)

If you have the good fortune to work on these tools, you will find the following information useful:

How to connect to the database from the shell:
mysql -hdaisy-for-all.org -u[username] -p[password] -A daisyfor_amisl10n

How to connect to the database from a python script:
1. Get the DB folder (it's on our FTP if you have a login), which contains the connection info module
sys.path.append("../")	#location of the parent folder for the DB folder
from DB.connect import *
#username can be "admin", "rw", "ro" for "admin", "read-write", "read-only"
#two options - the first for connecting from a local machine, the second for connecting from within a webscript
db = connect_to_db_from_local_machine("admin")
db = connect_to_db("admin")

Note that with the current server (bluehost), your IP has to be authorized in order to connect from a local machine.  Do the following:
1. Go to http://www.daisy-for-all.org:2082/frontend/bluehost/index.html and login as the site admin
2. Under "Remote MySQL", add your own IP as an Access Host
3. Under "Update contact/billing info", add your own IP under MySQL Whitelisted IPs

THE USERS TABLE

mysql > DESCRIBE users;
+-----------------+--------------+------+-----+---------+-------+
| Field           | Type         | Null | Key | Default | Extra |
+-----------------+--------------+------+-----+---------+-------+
| username        | varchar(20)  | YES  |     | NULL    |       | 
| realname        | varchar(100) | YES  |     | NULL    |       | 
| password        | varchar(20)  | YES  |     | NULL    |       | 
| email           | varchar(50)  | YES  |     | NULL    |       | 
| langid          | varchar(6)   | YES  |     | NULL    |       | 
| langname        | varchar(50)  | YES  |     | NULL    |       | 
| lastlogin       | datetime     | YES  |     | NULL    |       | 
| lastactivity    | datetime     | YES  |     | NULL    |       | 
| svnpath         | varchar(255) | YES  |     | NULL    |       | 
| sessionid       | varchar(36)  | YES  |     | NULL    |       | 
| lastpage        | varchar(11)  | YES  |     | NULL    |       | 
| lastpageoptions | varchar(255) | YES  |     | NULL    |       | 
+-----------------+--------------+------+-----+---------+-------+

Note that table ids cannot be hypenated, so while the language ID will be something like "eng-US", the corresponding table will be called "eng_US"

username:
	The user's login
realname:
	Their real name
password:
	Super-secret password.  Probably unencrypted.
email:
	How to contact this person
langid:
	The language that they're working on.  Uses the hyphenated form like "eng-US"
langname:
	The name of the language that they're working on
lastlogin:
	Date/time of their last login
lastactivity:
	The last time they did anything (useful for timeouts)
svnpath:
	The full path to the svn repository for their language pack
sessionid:
	Their session id
lastpage:
	The last page the user worked on ("main" | "mnemonics" | "accelerators")
lastpageoptions:
	The options for the last page the user worked on (usually the view (all, new, todo) on the main page)

THE LANGUAGE TABLES

mysql > DESCRIBE eng_US;
+---------------+------------------+------+-----+---------+----------------+
| Field         | Type             | Null | Key | Default | Extra          |
+---------------+------------------+------+-----+---------+----------------+
| id            | int(10) unsigned | NO   | PRI | NULL    | auto_increment | 
| textstring    | text             | YES  |     | NULL    |                | 
| audiodata     | longblob         | YES  |     | NULL    |                | 
| audiouri      | varchar(255)     | YES  |     | NULL    |                | 
| textflag      | int(11)          | YES  |     | NULL    |                | 
| audioflag     | int(11)          | YES  |     | NULL    |                | 
| remarks       | text             | YES  |     | NULL    |                | 
| xmlid         | varchar(4)       | YES  |     | NULL    |                | 
| role          | varchar(11)      | YES  |     | NULL    |                | 
| mnemonicgroup | int(11)          | YES  |     | NULL    |                | 
| target        | varchar(4)       | YES  |     | NULL    |                | 
| actualkeys    | varchar(100)     | YES  |     | NULL    |                | 
+---------------+------------------+------+-----+---------+----------------+

Each row represents a text element from the amisAccessibleUi.xml file.

id
	The internal table id, which is managed automatically by the database.
textstring
	The text contents
audiodata
	The raw audio data, presumably after having been uploaded by the user
audiouri
	The path to the audiodata, relative to the language's svn directory.  the audiodata gets moved here manually after the translation is done.
textflag
	1 = ok; 2 = todo (marked by the translator); 3 = new (the corresponding english text is new/changed)
audioflag
	1 = ok; 2 = todo (marked by the translator); 3 = new (the corresponding english text is new/changed)
remarks
	Comments from the translator
xmlid
	The id on the text element in the XML file.  Assumed to be unique in the database as well.
role
	One of the following: "ACCELERATOR" | "MNEMONIC" | "MENUITEM" | "CONTROL" | "DIALOG" | "STRING"
mnemonicgroup
	This number gives the group that the mnemonic belongs to (a group is a menu or dialog screen).  Useful for detecting duplicates.
target
	Gives the text id of the item that the mnemonic or accelerator is for
actualkeys
	The programmatic keys for the shortcut or mnemonic.  For example, the textstring would say "Espacio" in Spanish, but the actual key is still "Space"

THE LOCAL TOOLS
====
In a folder called "local" are tools that can be run locally.

amisdb.py
	Operations related to the database described above.  Requires your IP to be authorized.  See the file itself for usage instructions.

fill in the RC template:
	perl instantiate.pl AmisLangpackTemplate.rc yourAccessibleUi.xml

fill in the help file template:
	perl instantiate.pl yourHelpFileTemplate.html yourAccessibleUi.xml

make an html file with each prompt as a heading
	xsltproc prompts.xslt yourAccessibleUi.xml

fill the accessibleUi.xml file with audio prompts from an NCC-only book containing TOC entries for each prompt:
	The stuff after the pipe ( | ) is for utf8 output
	perl audio-files.pl -d --nobarf NCC_FILE XML_FILE  | perl -pe'BEGIN{binmode STDOUT, ":utf8"}s/&#x([^;]+);/chr(hex($1))/ge' > out.xml
	
	
