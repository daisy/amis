REM   make-amis-installer.bat langid langname helpfile

ECHO Customizing preferences for %2
perl setup-amis-prefs.pl ../../bin/settings/amisPrefs.xml.default ui-lang-id %1 > ../../bin/settings/clean_settings_for_the_installer/amisPrefs.xml 

ECHO Creating installer
"C:\Program Files\NSIS\makensis" /DCUSTOM_LANG_ID=%1 /DCUSTOM_LANG_NAME=%2 /DCUSTOM_HELP=%3 setup-amis3.nsi

ECHO Done
