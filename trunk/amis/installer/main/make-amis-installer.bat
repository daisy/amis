ECHO off

REM   make-amis-installer.bat langid langname helpfile
REM   If you are making an installer, be sure to get the latest one from Microsoft.  
REM   The one included in Visual Studio 8, even SP1, is outdated.  
REM   Then change the line under "Copying MSVC redistributable" to copy your local copy.

ECHO Customizing preferences for %2
mkdir ..\..\bin\settings\clean_settings_for_the_installer
perl setup-amis-prefs.pl ../../bin/settings/amisPrefs.xml.default ui-lang-id %1 > ../../bin/settings/clean_settings_for_the_installer/amisPrefs.xml
copy ..\..\bin\settings\amisHistory.xml.default ..\..\bin\settings\clean_settings_for_the_installer\amisHistory.xml

ECHO Copying MSVC redistributable
copy c:\devel\vcredist_x86.exe ..\..\bin\vcredist_x86.exe

ECHO Copying latest Jaws scripts
copy ..\..\jaws_scripts\Installers\AmisJFWScripts301.exe ..\..\bin\amis3_jfw_scripts.exe

ECHO Creating installer
"C:\Program Files\NSIS\makensis" /DCUSTOM_LANG_ID=%1 /DCUSTOM_LANG_NAME=%2 /DCUSTOM_HELP=%3 setup-amis3.nsi

ECHO Done
