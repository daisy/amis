@echo off
if "%1"=="help" echo Usage: copyLangpack LanguageID SourceDirectory BuildType[release or debug]
IF "%1"=="help" GOTO end

IF "%1"=="" ECHO Error: Please specify a language ID
IF "%1"=="" GOTO end
IF "%2"=="" ECHO Error: Source path missing.
IF "%2"=="" GOTO end
IF "%3"=="" ECHO Warning: 'release' or 'debug' not specified. Copying 'release' files by default.
IF "%3"=="" SET %3="Release"

SET DESTINATION=%systemdrive%\Documents and Settings\All Users\Application Data\AMIS\settings\lang\%1\

ECHO Copying %1 (%3) 
ECHO From %2 
ECHO To %DESTINATION%

REM remove old files first
rmdir /S "%DESTINATION%" /Q

mkdir "%DESTINATION%"
mkdir "%DESTINATION%help"
mkdir "%DESTINATION%help\img"
mkdir "%DESTINATION%audio"

copy "%2\AmisLangpack\%3\AmisLangpack.dll" "%DESTINATION%" /y
copy "%2\AmisLangpack\resource.h.ini" "%DESTINATION%" /y
copy "%2\moduleDesc.xml" "%DESTINATION%" /y
copy "%2\thislang.mp3" "%DESTINATION%" /y
copy "%2\flag.ico" "%DESTINATION%" /y
copy "%2\amisAccessibleUi.xml" "%DESTINATION%" /y
copy "%2\help\*.*" "%DESTINATION%help\" /y
copy "%2\help\img\*.*" "%DESTINATION%help\img\" /y
copy "%2\audio\*.*" "%DESTINATION%audio\" /y

pause

:end
::

pause
