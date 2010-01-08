ECHO off

ECHO setup-langpack langid langname helpfile

"C:\Program Files\NSIS\makensis" /DCUSTOM_LANG_NAME=%2 /DCUSTOM_LANG_ID=%1 /DCUSTOM_HELP=%3 setup-amis3-langpack.nsi 
