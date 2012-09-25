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
!define PRODUCT_VERSION "3.1.4 beta"
!define PRODUCT_PUBLISHER "DAISY Consortium"
!define PRODUCT_WEB_SITE "http://daisy.org/amis"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\AMIS.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"


;******
; pages, components, and script includes
;**

!include "Registry.nsh"
!include WinVer.nsh

; required for InstallLib
!include "Library.nsh"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; Support for conditional logic (should be in c:\program files\nsis\include by default)
!include "LogicLib.nsh"

;Windows and DirectX versions
!include "..\main\getversions.nsh"
!include "WinVer.nsh"

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
!define MUI_FINISHPAGE_RUN "$INSTDIR\AMIS.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "Hungarian"

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
!define DEFAULT_LANG_NAME "Angol"

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
OutFile "..\build\Setup-amis-${CUSTOM_LANG_NAME}.exe"
InstallDir "$PROGRAMFILES\AMIS"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

;******
; installer init
;**
Function .onInit
    SetOutPath "$INSTDIR"
    
    LogEx::Init true "$INSTDIR\install.log"
    LogEx::Write "${PRODUCT_NAME} ${PRODUCT_VERSION} (${CUSTOM_LANG_NAME})"  
    	
	;show the splash screen
	File /oname=$PLUGINSDIR\splash.bmp "${LOGO_DIR}\amis.bmp"
	splash::show 1000 $PLUGINSDIR\splash
	Pop $0 

    ; the recommendation is at least WinXP SP3
    ${If} ${AtLeastWinXP}
      ${AndIf} ${AtLeastServicePack} 3
      ${OrIf} ${AtLeastWinVista}
          LogEx::Write "Using Windows XP SP3 or higher"
          Goto DxCheck
      
    ; XP SP2 is acceptable but not as good as SP3
    ${ElseIf} ${AtLeastWinXP}
      ${AndIf} ${AtMostWinXP}
      ${AndIf} ${AtMostServicePack} 2
      ${AndIf} ${AtLeastServicePack} 2
         LogEx::Write "Using Windows XP SP2, warning user"
		 IfSilent DxCheck
         MessageBox MB_OK "Az ön számítógépére a Windows XP Service Pack 2 van telepítve.  A Szervízcsomag újabb verziója elérhető a Windows Update webhelyen. Azt ajánljuk, hogy telepítse rendszerére az elérhető frissítéseket."
         Goto DxCheck
         
    ; Other versions of windows are unsupported  
    ; (though Win2K SP4 and various 64 bit versions of windows might work, they have not been tested at this time)
    ; so give the user a chance to abort installation
    ${Else}
          ;find out exactly which OS version was detected
          Push $R0
          Call GetWindowsVersion
          Pop $R0
          LogEx::Write "Operating system not supported.  $R0"
		  IfSilent DxCheck
          MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Figyelmeztetés: Lehetséges, hogy az ön által használt operációs rendszeren az AMIS nem futtatható.  Folytatja a telepítést?" IDYES DxCheck
          LogEx::Write "User chose to abort installation (wrong OS)"
          Abort
      ${EndIf}
        
DxCheck:
    ;check for the directx version
    Call GetDXVersion
    Pop $0
    LogEx::Write "DirectX version $0"
    ${If} $0 < 900
      LogEx::Write "Warning user, wrong DX version"
      IfSilent IeCheck
      MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Figyelmeztetés: Az AMIS A DirectX 9.0 vagy magasabb verzióját igényli.  Folytathatja most a telepítést, azonban a telepítés előtt ajánlott a rendszer frissítése.  Folytatja?" IDYES IeCheck
      LogEx::Write "User chose to abort installation"
      Abort  
    ${EndIf}
    
IeCheck:
    ;check for IE version 7 or higher
    Call GetIEVersion
    Pop $0
    LogEx::Write "IE version $0"
    ${If} $0 == 6.00
        LogEx::Write "Warning user about IE6"
		IfSilent End
        MessageBox MB_OK "Ön az Internet Explorer 6-os verziójával rendelkezik.  A 7-es verzió vagy magasabb szükséges az optimális teljesítmény eléréséhez.  Az AMIS telepítése után azt ajánljuk, hogy frissítse a rendszerén futó Internet Explorer böngészőt."
    ${ElseIf} $0 < 6
        LogEx::Write "Warning user about IE version $0"
        IfSilent End
        MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Az Internet Explorer verziója elavult. Elképzelhető, hogy az AMIS nem fog megfelelően működni. Kívánja folytatni?" IDYES End
        LogEx::Write "User chose to abort (wrong IE version)"
        Abort
    ${EndIf}
End:
FunctionEnd

;******
; copy all files, register DLLs, etc
;**
Section "MainSection" SEC01
	;the settings dir will live here
    Var /GLOBAL SETTINGS_DIR
	; the MSVC Redist temporary directory
	Var /GLOBAL AMIS_TEMP
	
    ;figure out the user's application data directore
    ;look for the "all users" context
    SetShellVarContext all
    StrCpy $SETTINGS_DIR $APPDATA\AMIS\settings

	; create a temp directory that's different than the default
	; many windows machines put the default temp directory under the user's own path (including their username)
	; and -- many users have non-ascii names, which can be problematic for some 3rd party installers
	; get the drive letter for the windows drive
	StrCpy $R0 $WINDIR 1
	StrCpy $AMIS_TEMP "$R0:\amistemp"
	CreateDirectory $AMIS_TEMP

    LogEx::Write "Installing AMIS in $INSTDIR"
    LogEx::Write "Installing AMIS settings in $SETTINGS_DIR"
    
    SetOutPath "$INSTDIR"
    SetOverwrite try
    File "${BIN_DIR}\AMIS.exe"
    CreateDirectory "$SMPROGRAMS\AMIS"
    CreateShortCut "$SMPROGRAMS\AMIS\AMIS.lnk" "$INSTDIR\AMIS.exe"
    CreateShortCut "$DESKTOP\AMIS.lnk" "$INSTDIR\AMIS.exe"
  
    ;this creates a subdir in the start menu that will contain our modified shortcuts for compatibility/debug modes
    CreateDirectory "$SMPROGRAMS\AMIS\További lehetőségek"
    CreateShortCut "$SMPROGRAMS\AMIS\További lehetőségek\AMIS hibakeresési mód.lnk" "$INSTDIR\AMIS.exe" "-prefs amisPrefsDebug.xml"
    CreateShortCut "$SMPROGRAMS\AMIS\További lehetőségek\AMIS Kompatibilitási mód.lnk" "$INSTDIR\AMIS.exe" "-prefs amisPrefsCompatibilityMode.xml"
    CreateShortCut "$SMPROGRAMS\AMIS\További lehetőségek\AMIS Kompatibilitási mód DirectX támogatással.lnk" "$INSTDIR\AMIS.exe" "-prefs amisPrefsCompatibilityModeWithDX.xml"
    CreateShortCut "$SMPROGRAMS\AMIS\További lehetőségek\AMIS Kompatibilitási mód TTS támogatással.lnk" "$INSTDIR\AMIS.exe" "-prefs amisPrefsCompatibilityModeWithTTS.xml"
    
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
    File "${BIN_DIR}\IeDtbPlugin.dll"
    
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
    
    ;register the timescale ocx component
    LogEx::Write "Registering TransformSample.ax"
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
    ${IfNot} ${FileExists} "$SETTINGS_DIR\amisHistory.xml"
        File "${BIN_DIR}\settings\clean_settings_for_the_installer\amisHistory.xml"
    ${EndIf}

    File "${BIN_DIR}\settings\defaultToolbar.xml"
    File "${BIN_DIR}\settings\basicToolbar.xml"
    File "${BIN_DIR}\settings\amisHistory.xml.default"
    File "${BIN_DIR}\settings\clearHistory.bat"
    File "${BIN_DIR}\settings\clearCache.bat"
  
    ;update file permissions so that any user can run AMIS
    AccessControl::GrantOnFile "$SETTINGS_DIR\bmk" "(BU)" "FullAccess"
    AccessControl::GrantOnFile "$SETTINGS_DIR" "(BU)" "FullAccess"
    
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
    SetOutPath $AMIS_TEMP
    File "${VS_DIR}\vcredist_x86.exe"
  
    ;copy the jaws scripts installer
    SetOutPath $AMIS_TEMP
    File "${BIN_DIR}\amis3_jfw_scripts.exe"
  
    ;to support Thai encoding, add this key in HKLM
    ;Software\Classes\MIME\Database\Charset\TIS-620 and set AliasForCharset to windows-874
    WriteRegStr HKLM "Software\Classes\MIME\Database\Charset\TIS-620" "AliasForCharset" "Windows-874"
    LogEx::Write "Registered charset alias Windows-874 for TIS-620"
    
	;remove keys - the formatting has changed between this version and the last one
	DeleteRegKey HKCU "Software\Amis\AMIS\UAKs"
	
SectionEnd

;******
; copy the default (eng-US) and custom (if different) language packs
;*
Section -CopyLangpacks
  
    LogEx::Write "Copying default language pack files: ${DEFAULT_LANG_ID}"
    
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
        LogEx::Write "Copying custom language pack files ${CUSTOM_LANG_ID}"
        
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
	
    StrCpy $MSVC_RUNTIME_INSTALLER "$AMIS_TEMP\vcredist_x86.exe"
    LogEx::Write "MSVC Runtime Installer copied to temp dir $AMIS_TEMP"
    
    ;check and see if the user needs these files
    ${registry::KeyExists} "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{7299052B-02A4-4627-81F2-1818DA5D550D}" $0
    
    ${If} $0 == 0
        LogEx::Write "MSVC Runtime already installed on this machine"
    ${Else}
        LogEx::Write "Attempting to install MSVC Runtime"
        ExecWait "$MSVC_RUNTIME_INSTALLER" $0
        
        ${If} $0 != "0"
            LogEx::Write "Error installing MSVC Runtime"
			IfSilent End
            MessageBox MB_ICONEXCLAMATION "Hiba történt A Microsoft futásidejű környezet telepítése közben.  Ha problémát észlel az AMIS futtatása során, vegye fel a kapcsolatot az $\"Informatika a látássérültekért$\" Alapítvány ügyfélszolgálatával!"
        ${Else}
            LogEx::Write "MSVC Runtime installed successfully"
        ${EndIf}
    ${EndIf}
    
End:
    Delete "$MSVC_RUNTIME_INSTALLER"
SectionEnd

Section -JawsScripts
 	;don't install jaws scripts, just delete the exe
	Var /GLOBAL JFW_SCRIPTS_INSTALLER
	StrCpy $JFW_SCRIPTS_INSTALLER "$AMIS_TEMP\amis3_jfw_scripts.exe"
	Delete "$JFW_SCRIPTS_INSTALLER"
SectionEnd

;******
; Check if Java is installed
;*
Section -JavaCheck

    ${registry::KeyExists} "HKEY_LOCAL_MACHINE\SOFTWARE\JavaSoft\Java Runtime Environment" $0
    
    ${If} $0 == 0
        ${registry::Read} "HKEY_LOCAL_MACHINE\SOFTWARE\JavaSoft\Java Runtime Environment" "CurrentVersion" $R0 $R1
    
        StrCmp $R0 "1.6" CorrectVersion Check17
        
        Check17:
            StrCmp $R0 "1.7" CorrectVersion IncorrectVersion
		CorrectVersion:
            LogEx::Write "Correct Java version installed ($R0)"
			Goto End
		IncorrectVersion:
			LogEx::Write "Incorrect Java version ($R0)"
			IfSilent End
        	MessageBox MB_OK "A Java Futtatókörnyezet frissítése az 1.6 vagy magasabb verzióra történő frissítése szükséges a DAISY 3 könyvek szövegének teljeskörű megjelenítéséhez. Az AMIS telepítése után Bármikor letöltheti a futtatókörnyezetet a http://www.java.com oldalról, majd telepítheti azt."
        	Goto End
    ${Else}
        LogEx::Write "Java not found"
		IfSilent End
        MessageBox MB_OK "A Java Futtatókörnyezet 1.6 vagy magasabb verziójának telepítése szükséges a DAISY 3 könyvek szövegének teljeskörű megjelenítéséhez. Az AMIS telepítése után Bármikor letöltheti a futtatókörnyezetet a http://www.java.com oldalról, majd telepítheti azt."
		Goto End
    ${EndIf}
End:
SectionEnd

;******
; Create shortcuts and icons
;*
Section -AdditionalIcons
    WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
    CreateShortCut "$SMPROGRAMS\AMIS\Weboldal.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
    CreateShortCut "$SMPROGRAMS\AMIS\Eltávolítás.lnk" "$INSTDIR\Uninstall-AMIS.exe"
    ${If} ${CUSTOM_LANG_ID} != "eng-US"
        CreateShortCut "$SMPROGRAMS\AMIS\Súgó (${CUSTOM_LANG_NAME}).lnk" "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\help\${CUSTOM_HELP}"
        CreateShortCut "$SMPROGRAMS\AMIS\Billentyűparancsok (${CUSTOM_LANG_NAME}).lnk" "$SETTINGS_DIR\lang\${CUSTOM_LANG_ID}\shortcuts\amiskeys.html"
    ${EndIf}
    CreateShortCut "$SMPROGRAMS\AMIS\Súgó (${DEFAULT_LANG_NAME}).lnk" "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\help\amishelp.html"
    CreateShortCut "$SMPROGRAMS\AMIS\Billentyűparancsok (${DEFAULT_LANG_NAME}).lnk" "$SETTINGS_DIR\lang\${DEFAULT_LANG_ID}\shortcuts\amiskeys.html"
SectionEnd

;******
; write unintaller and registry strings
; check if we need to install the msvc runtimes
;**
Section -Post

    ;register the pdtb-ie plugin
    LogEx::Write "Installing IeDtbPlugin"
    ExecWait 'regsvr32.exe /s "$INSTDIR\IeDtbPlugin.dll"'    
    

    WriteUninstaller "$INSTDIR\Uninstall-AMIS.exe"
    WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\AMIS.exe"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall-AMIS.exe"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\AMIS.exe"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
    
    LogEx::Write "Summary of files:"
    LogEx::Write ""
    ExecDos::exec 'cmd /C dir "$INSTDIR" /b/s/l/a' "" "$INSTDIR\output.log"
    LogEx::AddFile "   >" "$INSTDIR\output.log"

    LogEx::Write ""
    ExecDos::exec 'cmd /C dir "$SETTINGS_DIR" /b/s/l/a' "" "$INSTDIR\output.log"
    LogEx::AddFile "   >" "$INSTDIR\output.log"
    
    LogEx::Close
    
    Delete $INSTDIR\output.log
	RMDir "$AMIS_TEMP"
SectionEnd


;******
; uninstall init
;**
Function un.onInit
	IfSilent End
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Biztos, hogy el kívánja távolítani az $(^Name) programot minden összetevőjével együtt?" IDYES End
    Abort
End:
FunctionEnd

;******
; uninstall complete
;**
Function un.onUninstSuccess
    HideWindow
	IfSilent End
    MessageBox MB_ICONINFORMATION|MB_OK "Az $(^Name) eltávolítása sikeresen megtörtént."
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
	
	; unregister the timescale ocx component
	; this works on both XP and Vista
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
    
	Delete "$SMPROGRAMS\AMIS\További lehetőségek\*"
	Delete "$SMPROGRAMS\AMIS\*"
	
    RMDir "$SMPROGRAMS\AMIS\További lehetőségek\"
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
	IfSilent Remove
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "El kívánja távolítani a könyvjelzőket és a legutoljára megnyitott könyvek listáját?" IDYES Remove IDNO End
Remove:
    Delete "$SETTINGS_DIR\amisHistory.xml"
    Delete "$SETTINGS_DIR\bmk\*"
    RMDir "$SETTINGS_DIR\bmk"
    ; the settings dir should now be empty
    RMDir "$SETTINGS_DIR"
    RMDir "$APPDATA\AMIS\"
End:
SectionEnd
