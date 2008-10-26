REM   make-amis-installer.bat langid langname

perl setup-amis-prefs.pl ../../bin/settings/amisPrefs.xml.default ui-lang-id %1 > ../../bin/settings/clean_settings_for_the_installer/amisPrefs.xml 
"C:\Program Files\NSIS\makensis" /DCUSTOM_LANG_ID=%1 /DCUSTOM_LANG_NAME=%2 setup-amis3.nsi