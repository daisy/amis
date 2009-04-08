rem ECHO off

REM   make-amis-installer.bat langid langname helpfile test
REM   The last parameter is for creating an installer ready for testers (full logging in AMIS)
REM   If you are making an installer, be sure to get the latest redist of MSVC libraries from Microsoft.  
REM   The one included in Visual Studio 8, even SP1, is outdated.  
REM   Then change the line under "Copying MSVC redistributable" to copy your local copy.

SET SETTINGS_DIR=..\..\bin\settings\clean_settings_for_the_installer
SET SETTINGS_DIR_PL=../../bin/settings/clean_settings_for_the_installer
    
ECHO Customizing preferences for %2

mkdir "%SETTINGS_DIR%"
IF "%4"=="test" (
    perl setup-amis-prefs.pl ../../bin/settings/amisPrefs.xml.default ui-lang-id %1 logging-level FULL_LOGGING > %SETTINGS_DIR_PL%/amisPrefs.xml
) ELSE (
    perl setup-amis-prefs.pl ../../bin/settings/amisPrefs.xml.default ui-lang-id %1 > %SETTINGS_DIR_PL%/amisPrefs.xml
)
copy ..\..\bin\settings\amisHistory.xml.default %SETTINGS_DIR%\amisHistory.xml

ECHO Copying MSVC redistributable
copy c:\devel\vcredist_x86.exe ..\..\bin\vcredist_x86.exe

ECHO Copying latest Jaws scripts
copy ..\..\jaws_scripts\Installers\AmisJFWScripts301.exe ..\..\bin\amis3_jfw_scripts.exe

ECHO Creating installer
"C:\Program Files\NSIS\makensis" /DCUSTOM_LANG_ID=%1 /DCUSTOM_LANG_NAME=%2 /DCUSTOM_HELP=%3 setup-amis3.nsi

ECHO Done
