@echo off

ECHO This script copies the AMIS settings folder to the system's application data directory

IF "%1"=="" (
    ECHO Using default Application Data directory
    SET DESTINATION=%systemdrive%\Documents and Settings\All Users\Application Data\AMIS\
) ELSE (
    SET DESTINATION=%1
)
ECHO %DESTINATION%

echo Removing old files
rmdir /S "%DESTINATION%" /Q

echo creating AMIS directory
mkdir "%DESTINATION%"
mkdir "%DESTINATION%settings"

echo copying settings
xcopy "%CD%\settings" "%DESTINATION%settings" /S /EXCLUDE:xcopy_exclude.txt 

echo creating fresh copies of prefs and history files
copy "%DESTINATION%settings\amisHistory.xml.default" "%DESTINATION%settings\amisHistory.xml" /y
copy "%CD%\settings\amisPrefs.xml.default" "%DESTINATION%settings\amisPrefs.xml" /y
copy "%CD%\settings\amisPrefsCompatibilityMode.xml.default" "%DESTINATION%settings\amisPrefsCompatibilityMode.xml" /y
copy "%CD%\settings\amisPrefsCompatibilityModeWithDX.xml.default" "%DESTINATION%settings\amisPrefsCompatibilityModeWithDX.xml" /y
copy "%CD%\settings\amisPrefsCompatibilityModeWithTTS.xml.default" "%DESTINATION%settings\amisPrefsCompatibilityModeWithTTS.xml" /y
copy "%CD%\settings\amisPrefsDebug.xml.default" "%DESTINATION%settings\amisPrefsDebug.xml" /y
echo done