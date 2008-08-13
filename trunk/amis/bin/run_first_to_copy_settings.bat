@echo off

ECHO This script copies the AMIS settings folder to the system's application data directory

IF "%1"=="" ECHO Using default Application Data directory
IF "%1"=="" SET DESTINATION=%systemdrive%\Documents and Settings\All Users\Application Data\AMIS\
IF NOT "%1"=="" SET DESTINATION=%1
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

echo done