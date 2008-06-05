@echo off
if "%1"=="help" echo Usage: copyLangpack LanguageID SourceDirectory DestinationDirectory [release or debug]
IF "%1"=="help" GOTO end

IF "%1"=="" ECHO Error: Please specify a language ID
IF "%1"=="" GOTO end
IF "%2"=="" ECHO Error: Source path missing.
IF "%2"=="" GOTO end
IF "%3"=="" ECHO Error: Destination path missing.
IF "%3"=="" GOTO end
IF "%4"=="" SET %2="Release"
IF "%4"=="" ECHO Warning: 'release' or 'debug' not specified. Copying 'release' files by default.

ECHO Copying %1 (%4) 
ECHO From %2 
ECHO To %3

REM remove old files first
rmdir /S %3\%1 /Q

REM todo: check first if %3 ends in a slash or not
mkdir %3\%1
mkdir %3\%1\help
mkdir %3\%1\help\img
mkdir %3\%1\audio

REM todo: check first if %2 ends in a slash or not
copy %2\AmisLangpack\%4\AmisLangpack.dll %3\%1\ /y
copy %2\AmisLangpack\resource.h.ini %3\%1\ /y
copy %2\moduleDesc.xml %3\%1\ /y
copy %2\thislang.mp3 %3\%1\ /y
copy %2\flag.ico %3\%1\ /y
copy %2\amisAccessibleUi.xml %3\%1\ /y
copy %2\help\*.* %3\%1\help\ /y
copy %2\help\img\*.* %3\%1\help\img\ /y
copy %2\audio\*.* %3\%1\audio\ /y

pause

:end
::

pause
