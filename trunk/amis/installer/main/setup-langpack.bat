ECHO setup-langpack langid langname helpfile

makensis /DCUSTOM_LANG_NAME=%2 /DCUSTOM_LANG_ID=%1 /DCUSTOM_HELP=%3 setup-amis3-langpack.nsi 
