rem this script erases the history list

IF "%1"=="" ECHO Using default Application Data directory
IF "%1"=="" SET DESTINATION=%systemdrive%\Documents and Settings\All Users\Application Data\AMIS\
IF NOT "%1"=="" SET DESTINATION=%1
ECHO %DESTINATION%

copy "%DESTINATION%settings\amisHistory.xml.default" "%DESTINATION%settings\amisHistory.xml" /y
