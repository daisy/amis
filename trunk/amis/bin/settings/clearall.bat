rem clear the history, delete all bookmarks, and reset the preferences
rem this is intended for developers, not end users

IF "%1"=="" ECHO Using default Application Data directory
IF "%1"=="" SET DESTINATION=%systemdrive%\Documents and Settings\All Users\Application Data\AMIS\
IF NOT "%1"=="" SET DESTINATION=%1
ECHO Using directory %DESTINATION% (NOT your development directory)

copy "%DESTINATION%settings\amisHistory.xml.default" "%DESTINATION%settings\amisHistory.xml" /y
copy "%DESTINATION%settings\amisPrefs.xml.default' "%DESTINATION%settings\amisPrefs.xml" /y
del  "%DESTINATION%settings\bmk\*.bmk" 

