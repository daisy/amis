ECHO OFF

REM make-amis-installer.bat creates a regular installer
REM make-amis-installer.bat test creates an installer for testing AMIS (full logging)

call make-amis-installer.bat "eng-US" "U.S. English" "" %1
IF "%1"=="test" (
    SET OUTFILE=Setup-amis30-rc2-U.S.English-TESTVERSION.exe
) ELSE (
    SET OUTFILE=Setup-amis30-rc2-U.S.English.exe
)
move "Setup-amis30-rc2-U.S. English.exe" "%OUTFILE%"
