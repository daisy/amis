; MUI 1.67 compatible ------
!include "MUI.nsh"

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
  ;turn logging on
  LogSet On
FunctionEnd

Section "MainSection" SEC01
    MessageBox MB_OK "Hello, world!"
    
    SetOutPath "$INSTDIR"
	SetOverwrite try
	File "lgpl.txt"
    
SectionEnd

Section -Post
    MessageBox MB_OK "I am post"
    WriteUninstaller "$INSTDIR\Uninstall-nsistest.exe"
    
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
