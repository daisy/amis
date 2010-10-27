rem this script erases cached book data

IF "%1"=="" ECHO Using default Application Data directory
IF "%1"=="" SET DESTINATION=%systemdrive%\Documents and Settings\All Users\Application Data\AMIS\
IF NOT "%1"=="" SET DESTINATION=%1
ECHO %DESTINATION%

del "%DESTINATION%settings\bmk\*.idx"
