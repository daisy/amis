;******
; Usage:
; makensis /DCUSTOM_LANG_NAME="language name" /DCUSTOM_LANG_ID="lang-code" /DCUSTOM_HELP help_file.html setup-amis3-langpack.nsi 
;
; example:
; makensis /DCUSTOM_LANG_NAME="U.S. English" /DCUSTOM_LANG_ID="eng-US" /DCUSTOM_HELP="amishelp.html" setup-amis3-langpack.nsi 
;
;/DCUSTOM_LANG_NAME = Identifies the installer EXE and the name of the product during installation
;/DCUSTOM_LANG_ID = The language pack identifier.  If other than "eng-US", both the custom and default language packs are included
;/DCUSTOM_HELP = The name of the help file for the custom language pack.
;**

;******
; product information
;**
!define PRODUCT_NAME "AMIS 3 Language Pack ${CUSTOM_LANG_NAME}"
!define PRODUCT_VERSION "1.0"
!define PRODUCT_PUBLISHER "DAISY Consortium"
!define PRODUCT_WEB_SITE "http://amisproject.org"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\AMIS.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"


;******
; pages, components, and script includes
;**
; required for InstallLib
!include "Library.nsh"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; Support for conditional logic (should be in c:\program files\nsis\include by default)
!include "LogicLib.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; License page
!insertmacro MUI_PAGE_LICENSE "lgpl.txt"

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

;******
; custom defines
;**

;this is the path to the logo file
!define LOGO_DIR "..\..\logo"

;this is the path to your Application Data directory
!define LOCAL_APP_DATA "C:\Documents and Settings\All Users\Application Data"

;******
; directory, installer exe name, etc
;**
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
;this is the name of the installer that gets created.  
OutFile "Setup-amis3-langpack-${CUSTOM_LANG_NAME}.exe"

;InstallDir "$APPDATA\AMIS\settings"
	

ShowInstDetails show
ShowUnInstDetails show

;******
; copy all files, register DLLs, etc
;**
Section "MainSection" SEC01

  ;the settings dir will live here
	Var /GLOBAL SETTINGS_DIR
	
	;figure out the user's application data directory
	;look for the "all users" context
	SetShellVarContext all
	StrCpy $SETTINGS_DIR $APPDATA\AMIS\settings
	
  ;copy the langpack root files
  SetOutPath "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}"
  File "${LOCAL_APP_DATA}\AMIS\settings\lang\${CUSTOM_LANG_ID}\*"
  	
  ;copy the langpack audio
  SetOutPath "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\audio"
  File "${LOCAL_APP_DATA}\AMIS\settings\lang\${CUSTOM_LANG_ID}\audio\*"
  	
  ;copy the langpack help book files (and images)
  SetOutPath "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\help"
  File "${LOCAL_APP_DATA}\AMIS\settings\lang\${CUSTOM_LANG_ID}\help\*"
  SetOutPath "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\help\img"
  File /nonfatal "${LOCAL_APP_DATA}\AMIS\settings\lang\${CUSTOM_LANG_ID}\help\img\*"  
  ;copy the langpack shortcut book files
  SetOutPath "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\shortcuts"
  File "${LOCAL_APP_DATA}\AMIS\settings\lang\${CUSTOM_LANG_ID}\shortcuts\*"
  
SectionEnd

;******
; Create shortcuts to the documentation (found in the language pack)
;*
Section -AdditionalIcons
  CreateShortCut "$SMPROGRAMS\AMIS\Help (${CUSTOM_LANG_NAME}).lnk" "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\help\${CUSTOM_HELP}"
  CreateShortCut "$SMPROGRAMS\AMIS\Keyboard Shortcuts (${CUSTOM_LANG_NAME}).lnk" "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\shortcuts\amiskeys.html"
SectionEnd

;******
; write unintaller and registry strings
; check if we need to install the msvc runtimes
;**
Section -Post
  WriteUninstaller "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\Uninstall-amis3-langpack-${CUSTOM_LANG_NAME}.exe"

SectionEnd

;******
;NSIS init function
;**
Function .onInit

	!insertmacro MUI_INSTALLOPTIONS_EXTRACT "filebrowse.ini"

	;show the splash screen
	File /oname=$PLUGINSDIR\splash.bmp "${LOGO_DIR}\amis.bmp"
	splash::show 1000 $PLUGINSDIR\splash

	; possible values for $0:
	; '1' if the user closed the splash screen early,
	; '0' if everything closed normally, 
	; '-1' if some error occurred.
	Pop $0 
	
	SetShellVarContext all
	StrCpy $SETTINGS_DIR $APPDATA\AMIS\settings

	; make sure the target directory exists
	IfFileExists "$SETTINGS_DIR" End Error
	
	Error:
		MessageBox MB_ICONSTOP "AMIS settings directory $SETTINGS_DIR not found."
		Abort
	
	
 End:
FunctionEnd

;******
; uninstall init
;**
Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

;******
; uninstall complete
;**
Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd


;******
;uninstall process
;**
Section Uninstall

	;figure out the user's application data directory
	;look for the "all users" context
	SetShellVarContext all 
	StrCpy $SETTINGS_DIR $APPDATA\AMIS\settings
	
	Delete "$SMPROGRAMS\AMIS\Help (${CUSTOM_LANG_NAME}).lnk"
  Delete "$SMPROGRAMS\AMIS\Keyboard Shortcuts (${CUSTOM_LANG_NAME}).lnk"

	Delete "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\help\*"
	Delete "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\help\img\*"
	Delete "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\audio\*"
	  
	RMDir "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\help\img"
	RMDir "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\help"
	RMDir "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\audio"
	 
	Delete "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\shortcuts\*"
	RMDir "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\shortcuts"

	Delete "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\*"
	RMDir "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}"  
  
  SetAutoClose true
SectionEnd
