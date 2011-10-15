ECHO setup-langpack langid langname helpfile

REM if there isn't a custom directory for this language, just use the english installer
IF NOT EXIST ..\%1 GOTO DEFAULT
:CUSTOM
"C:\Program Files\NSIS\makensis" /DCUSTOM_LANG_NAME=%2 /DCUSTOM_LANG_ID=%1 /DCUSTOM_HELP=%3 ..\%1\setup-amis3-langpack.nsi 
GOTO END

:DEFAULT
"C:\Program Files\NSIS\makensis" /DCUSTOM_LANG_NAME=%2 /DCUSTOM_LANG_ID=%1 /DCUSTOM_HELP=%3 ..\eng-US\setup-amis3-langpack.nsi 
GOTO END

:END
ECHO "Done"