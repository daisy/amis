;******
; Usage:
; makensis setup-amis3-langpack.nsi
;**

;******
; custom defines
;**
!define LANGID "eng-US"
!define PRODUCT_NAME "AMIS"
!define HELPFILE_NAME "amishelp.html"
!define LANGNAME "U.S. English"

!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\AMIS.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall${PRODUCT_NAME}"
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
; directory, installer exe name, etc
;**
Name "AMIS 3.1.4 beta 3"
;this is the name of the installer that gets created.
OutFile "../../../build\Setup-amis-langpack-${LANGNAME}.exe"

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
  SetOutPath "$SETTINGS_DIR\lang\${LANGID}"
  File "C:\Documents and Settings\All Users\Application Data\AMIS\settings\lang\${LANGID}\*"

  ;copy the langpack audio
  SetOutPath "$SETTINGS_DIR\lang\${LANGID}\audio"
  File "C:\Documents and Settings\All Users\Application Data\AMIS\settings\lang\${LANGID}\audio\*"

  ;copy the langpack help book files (and images)
  SetOutPath "$SETTINGS_DIR\lang\${LANGID}\help"
  File "C:\Documents and Settings\All Users\Application Data\AMIS\settings\lang\${LANGID}\help\*"
  SetOutPath "$SETTINGS_DIR\lang\${LANGID}\help\img"
  File /nonfatal "C:\Documents and Settings\All Users\Application Data\AMIS\settings\lang\${LANGID}\help\img\*"
  ;copy the langpack shortcut book files
  SetOutPath "$SETTINGS_DIR\lang\${LANGID}\shortcuts"
  File "C:\Documents and Settings\All Users\Application Data\AMIS\settings\lang\${LANGID}\shortcuts\*"

SectionEnd

;******
; Create shortcuts to the documentation (found in the language pack)
;*
Section -AdditionalIcons
  CreateShortCut "$SMPROGRAMS\AMIS\Help (U.S. English).lnk" "$SETTINGS_DIR\lang\${LANGID}\help\${HELPFILE_NAME}"
  CreateShortCut "$SMPROGRAMS\AMIS\Keyboard Shortcuts (U.S. English).lnk" "$SETTINGS_DIR\lang\${LANGID}\shortcuts\amiskeys.html"
SectionEnd

;******
; write unintaller and registry strings
; check if we need to install the msvc runtimes
;**
Section -Post
  WriteUninstaller "$SETTINGS_DIR\lang\${LANGID}\Uninstall-amis-langpack-${LANGNAME}.exe"

SectionEnd

;******
;NSIS init function
;**
Function .onInit

	!insertmacro MUI_INSTALLOPTIONS_EXTRACT "..\filebrowse.ini"

		;show the splash screen
		File /oname=$PLUGINSDIR\splash.bmp "../../../../logo\amis.bmp"
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
		IfSilent +2
		MessageBox MB_ICONSTOP "AMIS settings directory $SETTINGS_DIR not found."
		Abort


 End:
FunctionEnd

;******
; uninstall init
;**
Function un.onInit
  IfSilent End
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
End:
FunctionEnd

;******
; uninstall complete
;**
Function un.onUninstSuccess
  HideWindow
  IfSilent End
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
End:
FunctionEnd


;******
;uninstall process
;**
Section Uninstall

	;figure out the user's application data directory
	;look for the "all users" context
	SetShellVarContext all
	StrCpy $SETTINGS_DIR $APPDATA\AMIS\settings

	Delete "$SMPROGRAMS\AMIS\Help (U.S. English).lnk"
  	Delete "$SMPROGRAMS\AMIS\Keyboard Shortcuts (U.S. English).lnk"

	Delete "$SETTINGS_DIR\lang\${LANGID}\help\*"
	Delete "$SETTINGS_DIR\lang\${LANGID}\help\img\*"
	Delete "$SETTINGS_DIR\lang\${LANGID}\audio\*"

	RMDir "$SETTINGS_DIR\lang\${LANGID}\help\img"
	RMDir "$SETTINGS_DIR\lang\${LANGID}\help"
	RMDir "$SETTINGS_DIR\lang\${LANGID}\audio"

	Delete "$SETTINGS_DIR\lang\${LANGID}\shortcuts\*"
	RMDir "$SETTINGS_DIR\lang\${LANGID}\shortcuts"

	Delete "$SETTINGS_DIR\lang\${LANGID}\*"
	RMDir "$SETTINGS_DIR\lang\${LANGID}"

  SetAutoClose true
SectionEnd
