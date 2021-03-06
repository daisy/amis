;Edit for each language pack
!define PRODUCT_NAME "AMIS Language Pack: Swedish"
!define PRODUCT_VERSION ""
!define PRODUCT_PUBLISHER "DAISY for All Project"
!define PRODUCT_WEB_SITE "http://amis.sf.net"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_DIR_REGKEY "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\AMIS.exe"

; MUI 1.67 compatible ------
!include "MUI.nsh"
; for checking the file version
!include "FileFunc.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "..\main\lgpl.txt"
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
!insertmacro MUI_LANGUAGE "Swedish"

; MUI end ------

Name "${PRODUCT_NAME}"
;Edit for each language pack
OutFile "Setup-AMIS-LanguagePack_Swedish_20080704.exe"
;default install dir
InstallDir "$PROGRAMFILES\AMIS"
;find the install dir of the currently installed AMIS
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Function .onInit
ReadRegStr $0 HKLM "${PRODUCT_DIR_REGKEY}" ""
;see if AMIS is actually installed -- otherwise abort
DetailPrint "AMIS at $0"

StrCmp $0 "" Warning End

Warning:
    MessageBox MB_YESNO \
     "AMIS was not found on this machine.  Would you like to continue copying the language pack files anyway?" \ 
     IDNO AbortInstallation
    Goto End
AbortInstallation:
    Abort
End:
FunctionEnd

Section "MainSection" SEC01
  ;Edit for each language pack
  !define LANGPACK "swe-SE"
  
  SetOutPath "$INSTDIR\settings\lang\${LANGPACK}"
  SetOverwrite try
  File "..\..\bin\settings\lang\${LANGPACK}\*"
  
  SetOutPath "$INSTDIR\settings\lang\${LANGPACK}\audio"
  File "..\..\bin\settings\lang\${LANGPACK}\audio\*"
  
  SetOutPath "$INSTDIR\settings\lang\${LANGPACK}\help"
  File "..\..\bin\settings\lang\${LANGPACK}\help\*"
  
  SetOutPath "$INSTDIR\settings\lang\${LANGPACK}\help\img"
  File "..\..\bin\settings\lang\${LANGPACK}\help\img\*"
  
  SetOutPath "$INSTDIR\settings\lang\${LANGPACK}\start"
  File "..\..\bin\settings\lang\${LANGPACK}\start\*"
  
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\settings\lang\${LANGPACK}\*"
  Delete "$INSTDIR\settings\lang\${LANGPACK}\start\*"
  Delete "$INSTDIR\settings\lang\${LANGPACK}\help\*"
  Delete "$INSTDIR\settings\lang\${LANGPACK}\help\img\*"
  Delete "$INSTDIR\settings\lang\${LANGPACK}\audio\*"

  RMDir "$INSTDIR\settings\lang\${LANGPACK}\start"
  RMDir "$INSTDIR\settings\lang\${LANGPACK}\help\img"
  RMDir "$INSTDIR\settings\lang\${LANGPACK}\help"
  RMDir "$INSTDIR\settings\lang\${LANGPACK}\audio"
  RMDir "$INSTDIR\settings\lang\${LANGPACK}"

  SetAutoClose true
SectionEnd