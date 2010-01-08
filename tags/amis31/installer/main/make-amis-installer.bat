ECHO off

REM   make-amis-installer.bat langid langname helpfile contrast_translation

SET SETTINGS_DIR=..\..\bin\settings\clean_settings_for_the_installer
SET SETTINGS_DIR_PL=../../bin/settings/clean_settings_for_the_installer
    
ECHO Customizing preferences for %2

del /S/Q "%SETTINGS_DIR%"
mkdir "%SETTINGS_DIR%"
perl setup-amis-prefs.pl ../../bin/settings/amisPrefs.xml.default ui-lang-id %1 > %SETTINGS_DIR_PL%/amisPrefs.xml
perl setup-amis-prefs.pl ../../bin/settings/amisPrefsCompatibilityMode.xml.default ui-lang-id %1 > %SETTINGS_DIR_PL%/amisPrefsCompatibilityMode.xml
perl setup-amis-prefs.pl ../../bin/settings/amisPrefsCompatibilityModeWithDX.xml.default ui-lang-id %1 > %SETTINGS_DIR_PL%/amisPrefsCompatibilityModeWithDX.xml
perl setup-amis-prefs.pl ../../bin/settings/amisPrefsCompatibilityModeWithTTS.xml.default ui-lang-id %1 > %SETTINGS_DIR_PL%/amisPrefsCompatibilityModeWithTTS.xml
perl setup-amis-prefs.pl ../../bin/settings/amisPrefsDebug.xml.default ui-lang-id %1 > %SETTINGS_DIR_PL%/amisPrefsDebug.xml

copy ..\..\bin\settings\amisHistory.xml.default %SETTINGS_DIR%\amisHistory.xml

ECHO Copying latest Jaws scripts
copy ..\..\jaws_scripts\Installers\AmisJFWScripts301.exe ..\..\bin\amis3_jfw_scripts.exe


ECHO renaming contrast css files
SET CONTRAST_FILE="%4"
mkdir "%SETTINGS_DIR%\customStyles"
copy ..\..\bin\settings\css\customStyles\contrast1.css "%SETTINGS_DIR%\customStyles\%CONTRAST_FILE%1.css"
copy ..\..\bin\settings\css\customStyles\contrast2.css "%SETTINGS_DIR%\customStyles\%CONTRAST_FILE%2.css"
copy ..\..\bin\settings\css\customStyles\contrast3.css "%SETTINGS_DIR%\customStyles\%CONTRAST_FILE%3.css"


ECHO Creating installer
"C:\Program Files\NSIS\makensis" /DCUSTOM_LANG_ID=%1 /DCUSTOM_LANG_NAME=%2 /DCUSTOM_HELP=%3 setup-amis3.nsi

ECHO Done
