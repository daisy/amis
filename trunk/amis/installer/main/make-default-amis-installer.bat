ECHO OFF

REM make-amis-installer.bat creates a default (English) installer

call make-amis-installer.bat "eng-US" "U.S. English" "" "contrast"
SET OUTFILE=Setup-amis31-TEST.exe

move "Setup-amis31-U.S. English.exe" "%OUTFILE%"
