@echo off
IF "%1"=="" ECHO Error: Please specify a language pack directory
IF "%1"=="" GOTO end
@echo on
copy ..\src\DefaultLangpack\AmisLangpack\resource.h ..\..\langpacks\%1%\AmisLangpack\resource.h /Y
