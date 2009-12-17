!include WinVer.nsh

; MUI 1.67 compatible ------
!include "MUI.nsh"

!include "Registry.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; License page
!insertmacro MUI_PAGE_LICENSE "lgpl.txt"

; Directory page
!insertmacro MUI_PAGE_DIRECTORY
 
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------


Name "NSIS Test Script"
;this is the name of the installer that gets created.  
OutFile "nsistest.exe"
InstallDir "$PROGRAMFILES\MarisaTestOfNsis"
ShowInstDetails show
ShowUnInstDetails show
 
;*******************
;installation
;*******************
Function .onInit  
  LogEx::Init true "$INSTDIR\install.log"
  LogEx::Write "Init"  
  
  ${IfNot} ${AtLeastWin2000}
      MessageBox MB_OK "XP and above required"
      LogEx::Write "Need better windows!"
      Abort
  ${Else}
      LogEx::Write "Windows version ok"
    ${EndIf}    
FunctionEnd

Section "MainSection" SEC01
  
    SetOutPath "$INSTDIR"
	SetOverwrite try
	File "lgpl.txt"
    
    SetOutPath "$INSTDIR\stuff"
    SetOverwrite try
    File "lgpl.txt"
    
    Var /GLOBAL TEST
    StrCpy $TEST "C:\Program Files\NSIS"
    
    LogEx::Write "Files in $TEST"
    ExecDos::exec 'cmd /C dir "$TEST" /b/s/l/a' "" "output.log"
    LogEx::AddFile "   >" "output.log"
    
        
    ${registry::KeyExists} "HKEY_LOCAL_MACHINE\Software\Axialis\IconWorkshop" $0
    
    MessageBox MB_OK "$0"
    ${If} $0 == 0
        MessageBox MB_OK "Found"
    ${Else}
        MessageBox MB_OK "Not found"
    ${EndIf}
    
End:
SectionEnd
;******
; Check if Java is installed
;*
Section -JavaCheck
    
    ${registry::Read} "HKEY_LOCAL_MACHINE\SOFTWARE\JavaSoft\Java Runtime Environment" "CurrentVersion" $R0 $R1
    
    ${If} $R0 < 1.6
        LogEx::Write "Incorrect Java version or Java not installed (registry key read = $R0)"
        MessageBox MB_OK "Please upgrade your Java Runtime to version 1.6 or higher.  Java support is required for enhanced DAISY 3 text display. After the AMIS installation is complete, you may download Java from http://www.java.com and install it at any time."
    ${Else}
        LogEx::Write "Correct Java version installed (registry key read = $R0)"
    ${EndIf}
    
SectionEnd

Section -Post
    WriteUninstaller "$INSTDIR\Uninstall-nsistest.exe"
    Delete "$INSTDIR\output.log"
    LogEx::Close
SectionEnd

;*******************
;uninstallation
;*******************
Function un.onInit
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
    Abort
FunctionEnd

Section Uninstall
	Delete "$INSTDIR\*"
    RMDir "$INSTDIR"
    SetAutoClose true
SectionEnd

Function un.onUninstSuccess
    HideWindow
    MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd
