;******
; Usage:
; makensis /DCUSTOM_LANG_NAME="language name" /DCUSTOM_LANG_ID="lang-code" /DCUSTOM_HELP help_file.html setup-amis3.nsi
;
; example:
; makensis /DCUSTOM_LANG_NAME="U.S. English" /DCUSTOM_LANG_ID="eng-US" /DCUSTOM_HELP="amishelp.html" setup-amis3.nsi
;
;/DCUSTOM_LANG_NAME = Identifies the installer EXE and the name of the product during installation
;/DCUSTOM_LANG_ID = The language pack identifier.  If other than "eng-US", both the custom and default language packs are included
;/DCUSTOM_HELP = The name of the help file for the custom language pack.
;**

;******
; product information
;**
!define PRODUCT_NAME "AMIS"
!define PRODUCT_VERSION "3.1"
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

;SAPI page
!include "sapipage.nsh"

;Windows and DirectX versions
!include "getversions.nsh"

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
 
;this function is in the sapipage.nsh header
Page custom SapiPage
 
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\AMIS.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

;******
; custom defines
;**
;this is the path to where the AMIS executable lives
!define BIN_DIR	"..\..\bin"

;this is the path to the logo file
!define LOGO_DIR "..\..\logo"

;the default language setting
!define DEFAULT_LANG_ID "eng-US"
!define DEFAULT_LANG_NAME "U.S. English"

;this is the path to your windows system 32 directory
!define WIN32_DIR "c:\windows\system32"

;this is the path to your Application Data directory
!define LOCAL_APP_DATA "C:\Documents and Settings\All Users\Application Data"

;this is the path to your Visual Studio redistributables directory
!define VS_DIR "C:\Program Files\Microsoft Visual Studio 8\SDK\v2.0\BootStrapper\Packages\vcredist_x86"

;******
; directory, installer exe name, etc
;**
Name "${PRODUCT_NAME} ${PRODUCT_VERSION} (${CUSTOM_LANG_NAME})"
;this is the name of the installer that gets created.  
OutFile "Setup-amis31-${CUSTOM_LANG_NAME}.exe"
InstallDir "$PROGRAMFILES\AMIS"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

;******
; copy all files, register DLLs, etc
;**
Section "MainSection" SEC01
	 
    LogText "Testing"

    ;the settings dir will live here
	  Var /GLOBAL SETTINGS_DIR

	  ;figure out the user's application data directore
	  ;look for the "all users" context
	  SetShellVarContext all
	  StrCpy $SETTINGS_DIR $APPDATA\AMIS\settings
	
	  SetOutPath "$INSTDIR"
	  SetOverwrite try
	  File "${BIN_DIR}\AMIS.exe"
	  CreateDirectory "$SMPROGRAMS\AMIS"
    CreateShortCut "$SMPROGRAMS\AMIS\AMIS.lnk" "$INSTDIR\AMIS.exe"
	  CreateShortCut "$DESKTOP\AMIS.lnk" "$INSTDIR\AMIS.exe"
  
    ; this creates a subdir in the start menu that will contain our modified shortcuts for compatibility/debug modes
    CreateDirectory "$SMPROGRAMS\AMIS\Additional"
    CreateShortCut "$SMPROGRAMS\AMIS\Additional\AMIS Debug Mode.lnk" "$INSTDIR\AMIS.exe" "-prefs amisPrefsDebug.xml"
    CreateShortCut "$SMPROGRAMS\AMIS\Additional\AMIS Compatibility Mode.lnk" "$INSTDIR\AMIS.exe" "-prefs amisPrefsCompatibilityMode.xml"
    CreateShortCut "$SMPROGRAMS\AMIS\Additional\AMIS Compatibility Mode With DirectX.lnk" "$INSTDIR\AMIS.exe" "-prefs amisPrefsCompatibilityModeWithDX.xml"
    CreateShortCut "$SMPROGRAMS\AMIS\Additional\AMIS Compatibility Mode With TTS.lnk" "$INSTDIR\AMIS.exe" "-prefs amisPrefsCompatibilityModeWithTTS.xml"
    
	  ;copy the DLLs
	  File "${BIN_DIR}\libambulant_shwin32.dll"
	  File "${BIN_DIR}\xerces-c_3_0.dll"
	  File "${BIN_DIR}\TransformSample.ax"
	  File "${BIN_DIR}\libamplugin_ffmpeg.dll"
	  File "${BIN_DIR}\avformat-52.dll"
	  File "${BIN_DIR}\avcodec-51.dll"
	  File "${BIN_DIR}\avutil-49.dll"
	  File "${BIN_DIR}\SDL.dll"
	  File "${BIN_DIR}\libamplugin_pdtb.dll"
    File "${BIN_DIR}\lzop.exe"
    
    ;copy the xslt and stylesheet jar files
    SetOutPath "$INSTDIR\xslt"
    File "${BIN_DIR}\xslt\org.daisy.util.jar"
    File "${BIN_DIR}\xslt\saxon8.jar"
    File "${BIN_DIR}\xslt\stax-api-1.0.1.jar"
    File "${BIN_DIR}\xslt\wstx-lgpl-3.2.8.jar"
    
    SetOutPath "$INSTDIR\xslt\dtbook"
    File "${BIN_DIR}\xslt\dtbook\dtbook2xhtml.xsl"
    
    SetOutPath "$INSTDIR\xslt\l10n"
    File "${BIN_DIR}\xslt\l10n\l10n.xsl"
    
    ;register the pdtb-ie plugin
    !insertmacro InstallLib REGDLLTLB NOTSHARED NOREBOOT_NOTPROTECTED "${BIN_DIR}\IeDtbPlugin.dll" "$INSTDIR\IeDtbPlugin.dll" "$INSTDIR"
  
    ;register the timescale ocx component
    ExecWait 'regsvr32.exe /s "$INSTDIR\TransformSample.ax"'
  
    ;copy the bookmark readme file
    SetOutPath "$SETTINGS_DIR\bmk"
    File "${BIN_DIR}\settings\bmk\readme.txt"
  
    ;copy the default settings
    SetOutPath "$SETTINGS_DIR"
    ;the prefs files were generated by the setup_prefs script (called before this install script was called)
    File "${BIN_DIR}\settings\clean_settings_for_the_installer\amisPrefs.xml"
    File "${BIN_DIR}\settings\clean_settings_for_the_installer\amisPrefsCompatibilityMode.xml"
    File "${BIN_DIR}\settings\clean_settings_for_the_installer\amisPrefsCompatibilityModeWithDX.xml"
    File "${BIN_DIR}\settings\clean_settings_for_the_installer\amisPrefsCompatibilityModeWithTTS.xml"
    File "${BIN_DIR}\settings\clean_settings_for_the_installer\amisPrefsDebug.xml"
    
    ;preserve the history file if exists
    IfFileExists "$SETTINGS_DIR\amisHistory.xml" after_history_copy
    File "${BIN_DIR}\settings\clean_settings_for_the_installer\amisHistory.xml"

after_history_copy:
    File "${BIN_DIR}\settings\defaultToolbar.xml"
    File "${BIN_DIR}\settings\basicToolbar.xml"
    File "${BIN_DIR}\settings\amisHistory.xml.default"
    File "${BIN_DIR}\settings\clearHistory.bat"
    File "${BIN_DIR}\settings\clearCache.bat"
  
    ;update file permissions so that any user can run AMIS
    AccessControl::GrantOnFile "$SETTINGS_DIR\amisPrefs.xml" "(BU)" "FullAccess"
    AccessControl::GrantOnFile "$SETTINGS_DIR\amisPrefsCompatibilityMode.xml" "(BU)" "FullAccess"
    AccessControl::GrantOnFile "$SETTINGS_DIR\amisPrefsCompatibilityModeWithDX.xml" "(BU)" "FullAccess"
    AccessControl::GrantOnFile "$SETTINGS_DIR\amisPrefsCompatibilityModeWithTTS.xml" "(BU)" "FullAccess"
    AccessControl::GrantOnFile "$SETTINGS_DIR\amisPrefsDebug.xml" "(BU)" "FullAccess"
    
    AccessControl::GrantOnFile "$SETTINGS_DIR\amisHistory.default.xml" "(BU)" "FullAccess"
    AccessControl::GrantOnFile "$SETTINGS_DIR\amisHistory.xml" "(BU)" "FullAccess"
    AccessControl::GrantOnFile "$SETTINGS_DIR\bmk" "(BU)" "FullAccess"
  
    File "${LOCAL_APP_DATA}\AMIS\settings\resource.h.ini"
  
    ;copy the css files
    SetOutPath "$SETTINGS_DIR\css"
    File "${BIN_DIR}\settings\css\*.css"
    SetOutPath "$SETTINGS_DIR\css\customStyles"
    File "${BIN_DIR}\settings\clean_settings_for_the_installer\customStyles\*.css"
    SetOutPath "$SETTINGS_DIR\css\font"
    File "${BIN_DIR}\settings\css\font\*.css"
    SetOutPath "$SETTINGS_DIR\css\font\ie8"
    File "${BIN_DIR}\settings\css\font\ie8\*.css"
    
    ;copy the images
    SetOutPath "$SETTINGS_DIR\img"
    File "${BIN_DIR}\settings\img\*.ico"
    SetOutPath "$SETTINGS_DIR\img\defaultToolbar"
    File "${BIN_DIR}\settings\img\defaultToolbar\*.ico"
    SetOutPath "$SETTINGS_DIR\img\basicToolbar"
    File "${BIN_DIR}\settings\img\basicToolbar\*.ico"
  
    ;copy the lang directory readme file
    SetOutPath "$SETTINGS_DIR\lang"
    File "${BIN_DIR}\settings\lang\readme.txt"

    ;copy the MSVC redistributables installer
    SetOutPath $TEMP
    File "${VS_DIR}\vcredist_x86.exe"
  
    ;copy the jaws scripts installer
    SetOutPath $TEMP
    File "${BIN_DIR}\amis3_jfw_scripts.exe"
  
    ;to support Thai encoding, add this key in HKLM
    ;Software\Classes\MIME\Database\Charset\TIS-620 and set AliasForCharset to windows-874
    WriteRegStr HKLM "Software\Classes\MIME\Database\Charset\TIS-620" "AliasForCharset" "Windows-874"
 
SectionEnd

;******
; copy the default (eng-US) and custom (if different) language packs
;*
Section -CopyLangpacks
  
    ;***
    ;copy the default langpack
    ;***
    ;copy the langpack root files
    SetOutPath "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}"
    File "${LOCAL_APP_DATA}\AMIS\settings\lang\${DEFAULT_LANG_ID}\*"

    ;copy the langpack audio
    SetOutPath "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\audio"
    File "${LOCAL_APP_DATA}\AMIS\settings\lang\${DEFAULT_LANG_ID}\audio\*"

    ;copy the langpack help book files (and images)
    SetOutPath "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\help"
    File "${LOCAL_APP_DATA}\AMIS\settings\lang\${DEFAULT_LANG_ID}\help\*"
    SetOutPath "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\help\img"
    File "${LOCAL_APP_DATA}\AMIS\settings\lang\${DEFAULT_LANG_ID}\help\img\*"

    ;copy the langpack shortcut book files
    SetOutPath "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\shortcuts"
    File "${LOCAL_APP_DATA}\AMIS\settings\lang\${DEFAULT_LANG_ID}\shortcuts\*"

    ;***
    ; copy the custom langpack
    ;***
    ${If} ${CUSTOM_LANG_ID} != "eng-US"
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
    ${EndIf}
End:
SectionEnd

;******
; Install MSVC runtime
;*
Section -MSVCRuntime
  
    Var /GLOBAL MSVC_RUNTIME_INSTALLER
    StrCpy $MSVC_RUNTIME_INSTALLER "$TEMP\vcredist_x86.exe"

    ;check and see if the user needs these files
    Push $R0
    ClearErrors
    ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{837b34e3-7c30-493c-8f6a-2b0f04e2912c}" "Version"
    ; if VS 2005 redist not installed (if there was an error finding the key), install it
    IfErrors InstallVSRedist End
    StrCpy $R0 "-1"

    ;actually install them
InstallVSRedist: 
    ExecWait "$MSVC_RUNTIME_INSTALLER" $0
    StrCmp $0 "0" End Error
Error:
    MessageBox MB_ICONEXCLAMATION "There was an error encountered while attempting to install the Microsoft runtime files.  Please contact technical support if you experience problems running AMIS."
    Goto End
End:
    Delete "$MSVC_RUNTIME_INSTALLER"

SectionEnd

;******
; Install Jaws scripts
;*
Section -JawsScripts
  
    Var /GLOBAL JFW_SCRIPTS_INSTALLER
    StrCpy $JFW_SCRIPTS_INSTALLER "$TEMP\amis3_jfw_scripts.exe"

    ; check if the user has jaws installed, then ask if they want to install the scripts
    ClearErrors
    EnumRegKey $0 HKCU "Software\Freedom Scientific\JAWS" 0
    ; if the key exists (no errors), then ask the user if they want to install jaws scripts.  otherwise go to the end.
    IfErrors End AskUser
  
    ; ask if the user wants to install the scripts
AskUser:
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1 "Would you like to install Jaws for Windows scripts for AMIS?" IDYES InstallScripts
    Goto End

InstallScripts:
    ExecWait "$JFW_SCRIPTS_INSTALLER" $0
    StrCmp $0 "0" End Error

Error:
    MessageBox MB_ICONEXCLAMATION "There was an error encountered while attempting to install Jaws for Windows scripts for AMIS.  Please visit http://amisproject.org to download and install the scripts separately."

End:
    Delete "$JFW_SCRIPTS_INSTALLER"

SectionEnd

;******
; Check if Java is installed
;*
Section -JavaCheck
     ; check if the user has jaws installed, then ask if they want to install the scripts
    ClearErrors
    EnumRegKey $0 HKLM "SOFTWARE\JavaSoft\Java Runtime Environment" "CurrentVersion"
    ; if the key exists (no errors), then check the version.  Else give the user a message.
    IfErrors MessageToUser1

CheckVersion:
    ;Pop $0
    ${If} $0 < 1.6
        MessageBox MB_OK "$0 is less than 1.6"
        Goto MessageToUser2
    ${Else}
        Goto End
    ${EndIf}
    
    ; ask if the user wants to install the scripts
MessageToUser1:
    MessageBox MB_OK "The Java Runtime was not found on your system.  Java support is required for enhanced DAISY 3 text display. After the AMIS installation is complete, you may download Java from http://www.java.com and install it at any time."
    Goto End

MessageToUser2:
    MessageBox MB_OK "Please upgrade your Java Runtime to version 1.6 or higher.  Java support is required for enhanced DAISY 3 text display. After the AMIS installation is complete, you may download Java from http://www.java.com and install it at any time."
    Goto End


End:
SectionEnd

;******
; Create shortcuts and icons
;*
Section -AdditionalIcons
    WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
    CreateShortCut "$SMPROGRAMS\AMIS\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
    CreateShortCut "$SMPROGRAMS\AMIS\Uninstall.lnk" "$INSTDIR\Uninstall-AMIS.exe"
    ${If} ${CUSTOM_LANG_ID} != "eng-US"
        CreateShortCut "$SMPROGRAMS\AMIS\Help (${CUSTOM_LANG_NAME}).lnk" "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\help\${CUSTOM_HELP}"
        CreateShortCut "$SMPROGRAMS\AMIS\Keyboard Shortcuts (${CUSTOM_LANG_NAME}).lnk" "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\shortcuts\amiskeys.html"
    ${EndIf}
    CreateShortCut "$SMPROGRAMS\AMIS\Help (${DEFAULT_LANG_NAME}).lnk" "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\help\amishelp.html"
    CreateShortCut "$SMPROGRAMS\AMIS\Keyboard Shortcuts (${DEFAULT_LANG_NAME}).lnk" "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\shortcuts\amiskeys.html"
SectionEnd

;******
; write unintaller and registry strings
; check if we need to install the msvc runtimes
;**
Section -Post
    WriteUninstaller "$INSTDIR\Uninstall-AMIS.exe"
    WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\AMIS.exe"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall-AMIS.exe"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\AMIS.exe"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

;******
;NSIS init function
;**
Function .onInit
  
  ;turn logging on
  LogSet On

	;load the sapi install screen file, as we may need it
	!insertmacro MUI_INSTALLOPTIONS_EXTRACT "sapipage.ini"
	!insertmacro MUI_INSTALLOPTIONS_EXTRACT "filebrowse.ini"
	
	;show the splash screen
	File /oname=$PLUGINSDIR\splash.bmp "${LOGO_DIR}\amis.bmp"
	splash::show 1000 $PLUGINSDIR\splash
    
	; possible values for $0:
	; '1' if the user closed the splash screen early,
	; '0' if everything closed normally, 
	; '-1' if some error occurred.
	;
	Pop $0 

	;check that the OS is XP, 2000, or Vista
	Call GetWindowsVersion
	Pop $R0
	
CheckWinXP:	
    StrCmp $R0 "XP" DxCheck CheckWin2K
CheckWin2K:
    StrCmp $R0 "2000" DxCheck CheckVista
CheckVista:
    StrCmp $R0 "Vista" DxCheck OSNotSupported

    ;the OS is not supported; warn the user instead of aborting
OSNotSupported:
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Warning: operating system not supported.  AMIS may not work.  Do you want to continue?" IDYES DxCheck
    Abort

DxCheck:
    ;check for the directx version
    Call GetDXVersion
    Pop $0
    IntCmp $0 900 End DxWarning End
DxWarning:
    MessageBox MB_OK "Requires DirectX 9.0 or later.  Please update your copy of DirectX."

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
	
	; unregister the timescale ocx component
    ExecWait 'regsvr32.exe /u /s "$INSTDIR\TransformSample.ax"'
    ; unregister the pdtb dll
    !insertmacro UnInstallLib REGDLLTLB NOTSHARED NOREBOOT_NOTPROTECTED "$INSTDIR\IeDtbPlugin.dll"
  
	Delete "$SETTINGS_DIR\css\*"
	Delete "$SETTINGS_DIR\css\font\*"
    Delete "$SETTINGS_DIR\css\font\ie8\*"
	Delete "$SETTINGS_DIR\css\customStyles\*"
    RMDir "$SETTINGS_DIR\css\font\ie8\"
    RMDir "$SETTINGS_DIR\css\font"
	RMDir "$SETTINGS_DIR\css\customStyles"
	RMDir "$SETTINGS_DIR\css"
	
	Delete "$SETTINGS_DIR\img\*"
	Delete "$SETTINGS_DIR\img\basicToolbar\*"
	Delete "$SETTINGS_DIR\img\defaultToolbar\*"
	RMDir "$SETTINGS_DIR\img\defaultToolbar"
	RMDir "$SETTINGS_DIR\img\basicToolbar"
	RMDir "$SETTINGS_DIR\img"
	
	Delete "$SETTINGS_DIR\amisPrefs.xml"
    Delete "$SETTINGS_DIR\amisPrefsDebug.xml"
    Delete "$SETTINGS_DIR\amisPrefsCompatibilityMode.xml"
    Delete "$SETTINGS_DIR\amisPrefsCompatibilityModeWithDX.xml"
    Delete "$SETTINGS_DIR\amisPrefsCompatibilityModeWithTTS.xml"
    Delete "$SETTINGS_DIR\clearCache.bat"
    Delete "$SETTINGS_DIR\defaultToolbar.xml"
    Delete "$SETTINGS_DIR\basicToolbar.xml"
    Delete "$SETTINGS_DIR\amisHistory.xml.default"
    Delete "$SETTINGS_DIR\clearHistory.bat"
    Delete "$SETTINGS_DIR\resource.h.ini"
    Delete "$SETTINGS_DIR\amisLog.txt"

    Delete "$INSTDIR\xslt\l10n\*"
    RMDir "$INSTDIR\xslt\l10n\"
    Delete "$INSTDIR\xslt\dtbook\*"
    RMDir "$INSTDIR\xslt\dtbook\"
    Delete "$INSTDIR\xslt\*"
    RMDir "$INSTDIR\xslt"    
    Delete "$INSTDIR\*"
    RMDir "$INSTDIR"
    ; this deletes all the registry keys used by NSIS
    DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
    DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"

    Delete "$DESKTOP\AMIS.lnk" 
    
    Delete "$SMPROGRAMS\AMIS\AMIS.lnk"
    Delete "$SMPROGRAMS\AMIS\Additional\AMIS Debug Mode.lnk"
    Delete "$SMPROGRAMS\AMIS\Additional\AMIS Compatibility Mode.lnk"    
    Delete "$SMPROGRAMS\AMIS\Additional\AMIS Compatibility Mode With DirectX.lnk"    
    Delete "$SMPROGRAMS\AMIS\Additional\AMIS Compatibility Mode With TTS.lnk"    
    
    Delete "$SMPROGRAMS\AMIS\Website.lnk"
    Delete "$SMPROGRAMS\AMIS\Uninstall.lnk"
    ${If} ${CUSTOM_LANG_ID} != "eng-US"
        Delete "$SMPROGRAMS\AMIS\Help (${CUSTOM_LANG_NAME}).lnk"
        Delete "$SMPROGRAMS\AMIS\Keyboard Shortcuts (${CUSTOM_LANG_NAME}).lnk"
    ${EndIf}
    Delete "$SMPROGRAMS\AMIS\Help (${DEFAULT_LANG_NAME}).lnk"
    Delete "$SMPROGRAMS\AMIS\Keyboard Shortcuts (${DEFAULT_LANG_NAME}).lnk"

    RMDir "$SMPROGRAMS\AMIS\Additional\"
    RMDir "$SMPROGRAMS\AMIS\"
    
    SetAutoClose true
SectionEnd

;******
;uninstall the langpacks
;**
Section -un.CopyLangpack
	
	Delete "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\help\*"
    Delete "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\help\img\*"
    Delete "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\audio\*"
  
    RMDir "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\help\img"
    RMDir "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\help"
    RMDir "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\audio"

    Delete "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\shortcuts\*"
    RMDir "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\shortcuts"

    Delete "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\*"
    RMDir "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}"

    ${If} ${CUSTOM_LANG_ID} != ""
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
    ${EndIf}

    Delete "$SETTINGS_DIR\lang\readme.txt"
    RMDir "$SETTINGS_DIR\lang"
SectionEnd

Section -un.RemoveHistoryBookmarks
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Remove bookmarks and history?" IDYES Remove IDNO End
Remove:
    Delete "$SETTINGS_DIR\amisHistory.xml"
    Delete "$SETTINGS_DIR\bmk\*"
    RMDir "$SETTINGS_DIR\bmk"
    ; the settings dir should now be empty
    RMDir "$SETTINGS_DIR"
    RMDir "$APPDATA\AMIS\"
End:
SectionEnd
