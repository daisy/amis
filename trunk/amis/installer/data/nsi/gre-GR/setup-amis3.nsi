;******
; Usage:
; makensis setup-amis3.nsi
;**

;******
; custom defines
;**
!define LANGID "gre-GR"
!define JFW_FILENAME "..\..\jaws_scripts\Installers\AmisJFWScripts301.exe"
!define PRODUCT_NAME "AMIS"
!define HELPFILE_NAME "amis_dtbook.html"
!define LANGNAME "Greek"


!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\AMIS.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define COMPATIBILITY_REG_KEY "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers"

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
!include "..\getversions.nsh"
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
!insertmacro MUI_LANGUAGE "Greek"

; MUI end ------


!define DEFAULT_LANG_NAME "Ελληνικά"

;******
; directory, installer exe name, etc
;**
Name "AMIS 3.1.3 (Ελληνικά)"
;this is the name of the installer that gets created.
OutFile "../../../build\Setup-amis-${LANGNAME}.exe"
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
    LogEx::Write "AMIS 3.1.3 (Ελληνικά)"

	;show the splash screen
	File /oname=$PLUGINSDIR\splash.bmp "../../../../logo\amis.bmp"
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
         MessageBox MB_OK "Διαθέτετε Windows XP Service Pack 2.  Υπάρχουν νεότερα service packs και συνιστάται να τρέξετε το Windows Update (αναβάθμιση των Windows) για να ναβαθμιστεί το σύστημά σας."
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
          MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Προειδοίηση: το λειτουργικό σύστημα που δεν υποστηρίζεται.  Το AMIS μάλλον δεν θα λειτουργεί. Θέλετε να συνεχίσετε;" IDYES DxCheck
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
      MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Προειδοποίηση: Το AMIS απαιτεί DirectX 9.0 ή νεότερη έκδοσή του. Μπορείτε να συνεχίσετε με την εγκατάσταση του AMIS, όπμως συνιστάται να αναβαθμίσετε πρώτα το σύστημά σας. Συνέχεια;" IDYES IeCheck
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
        MessageBox MB_OK "Έχετε το Internet Explorer 6. Απαιτείται έκδοση 7 ή νεότερη για καλύτερη απόδοση. Μπορείτε να αναβαθμίσετε το σύστημά σας οποιαδήποτε στιγμή μετά την εγκατάσταση του AMIS."
    ${ElseIf} $0 < 6
        LogEx::Write "Warning user about IE version $0"
        IfSilent End
        MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Η έκδοση του Internet Explorer είναι παλαιά. Το AMIS μπορεί να μην λειτουργεί. Θέλετε να συνεχίσετε;" IDYES End
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
    File "../../../../bin\AMIS.exe"
    CreateDirectory "$SMPROGRAMS\AMIS"
    CreateShortCut "$SMPROGRAMS\AMIS\AMIS.lnk" "$INSTDIR\AMIS.exe"
    CreateShortCut "$DESKTOP\AMIS.lnk" "$INSTDIR\AMIS.exe"

    ;this creates a subdir in the start menu that will contain our modified shortcuts for compatibility/debug modes
    CreateDirectory "$SMPROGRAMS\AMIS\Επιπρόσθετα"
    CreateShortCut "$SMPROGRAMS\AMIS\Επιπρόσθετα\Λειτουργία αποσφαλμάτωσης του AMIS.lnk" "$INSTDIR\AMIS.exe" "-prefs amisPrefsDebug.xml"
    CreateShortCut "$SMPROGRAMS\AMIS\Επιπρόσθετα\Λειτουργία συμβατότητας του AMIS.lnk" "$INSTDIR\AMIS.exe" "-prefs amisPrefsCompatibilityMode.xml"
    CreateShortCut "$SMPROGRAMS\AMIS\Επιπρόσθετα\Λειτουργία συμβατότητας του AMIS με το DirectX.lnk" "$INSTDIR\AMIS.exe" "-prefs amisPrefsCompatibilityModeWithDX.xml"
    CreateShortCut "$SMPROGRAMS\AMIS\Επιπρόσθετα\Λειτουργία συμβατότητας του AMIS με το TTS.lnk" "$INSTDIR\AMIS.exe" "-prefs amisPrefsCompatibilityModeWithTTS.xml"

    ;copy the DLLs
    File "../../../../bin\libambulant_shwin32.dll"
    File "../../../../bin\xerces-c_3_0.dll"
    File "../../../../bin\TransformSample.ax"
    File "../../../../bin\libamplugin_ffmpeg.dll"
    File "../../../../bin\avformat-52.dll"
    File "../../../../bin\avcodec-51.dll"
    File "../../../../bin\avutil-49.dll"
    File "../../../../bin\SDL.dll"
    File "../../../../bin\libamplugin_pdtb.dll"
    File "../../../../bin\lzop.exe"
    File "../../../../bin\IeDtbPlugin.dll"

    ;copy the xslt and stylesheet jar files
    SetOutPath "$INSTDIR\xslt"
    File "../../../../bin\xslt\org.daisy.util.jar"
    File "../../../../bin\xslt\saxon8.jar"
    File "../../../../bin\xslt\stax-api-1.0.1.jar"
    File "../../../../bin\xslt\wstx-lgpl-3.2.8.jar"

    SetOutPath "$INSTDIR\xslt\dtbook"
    File "../../../../bin\xslt\dtbook\dtbook2xhtml.xsl"

    SetOutPath "$INSTDIR\xslt\l10n"
    File "../../../../bin\xslt\l10n\l10n.xsl"

    ;register the timescale ocx component
    LogEx::Write "Registering TransformSample.ax"
    ExecWait 'regsvr32.exe /s "$INSTDIR\TransformSample.ax"'

    ;copy the bookmark readme file
    SetOutPath "$SETTINGS_DIR\bmk"
    File "../../../../bin\settings\bmk\readme.txt"

    ;copy the default settings
    SetOutPath "$SETTINGS_DIR"
    ;the prefs files were generated prior to running this NSIS script. they contain lang-specific modifications.
    File "../..\settings\amisPrefs.xml"
    File "../..\settings\amisPrefsCompatibilityMode.xml"
    File "../..\settings\amisPrefsCompatibilityModeWithDX.xml"
    File "../..\settings\amisPrefsCompatibilityModeWithTTS.xml"
    File "../..\settings\amisPrefsDebug.xml"

    ;preserve the history file if exists
    ${IfNot} ${FileExists} "$SETTINGS_DIR\amisHistory.xml"
        File "../..\amisHistory.xml"
    ${EndIf}

    File "../../../../bin\settings\defaultToolbar.xml"
    File "../../../../bin\settings\basicToolbar.xml"
    File "../../../../bin\settings\amisHistory.xml.default"
    File "../../../../bin\settings\clearHistory.bat"
    File "../../../../bin\settings\clearCache.bat"

    ;update file permissions so that any user can run AMIS
    AccessControl::GrantOnFile "$SETTINGS_DIR\bmk" "(BU)" "FullAccess"
    AccessControl::GrantOnFile "$SETTINGS_DIR" "(BU)" "FullAccess"
    ; set permissions on this too; same as above but works better for non-english/more recent windows versions
    AccessControl::GrantOnFile "$SETTINGS_DIR\bmk" "(S-1-5-32-545)" "FullAccess"
    AccessControl::GrantOnFile "$SETTINGS_DIR" "(S-1-5-32-545)" "FullAccess"

    File "C:\Documents and Settings\All Users\Application Data\AMIS\settings\resource.h.ini"

    ;copy the css files
    SetOutPath "$SETTINGS_DIR\css"
    File "../../../../bin\settings\css\*.css"
    SetOutPath "$SETTINGS_DIR\css\customStyles"
    File "../..\settings\customStyles\*.css"
    SetOutPath "$SETTINGS_DIR\css\font"
    File "../../../../bin\settings\css\font\*.css"
    SetOutPath "$SETTINGS_DIR\css\font\ie8"
    File "../../../../bin\settings\css\font\ie8\*.css"

    ;copy the images
    SetOutPath "$SETTINGS_DIR\img"
    File "../../../../bin\settings\img\*.ico"
    SetOutPath "$SETTINGS_DIR\img\defaultToolbar"
    File "../../../../bin\settings\img\defaultToolbar\*.ico"
    SetOutPath "$SETTINGS_DIR\img\basicToolbar"
    File "../../../../bin\settings\img\basicToolbar\*.ico"

    ;copy the lang directory readme file
    SetOutPath "$SETTINGS_DIR\lang"
    File "../../../../bin\settings\lang\readme.txt"

    ;copy the MSVC redistributables installer
    SetOutPath $AMIS_TEMP
    File "C:\Program Files\Microsoft Visual Studio 8\SDK\v2.0\BootStrapper\Packages\vcredist_x86\vcredist_x86.exe"

    ;copy the jaws scripts installer
    SetOutPath $AMIS_TEMP
    File "../../../src/..\..\jaws_scripts\Installers\AmisJFWScripts301.exe"

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

    ; TODO checking if the default lang = 'eng-US' would make this a little more efficient
    
    LogEx::Write "Copying language pack files: eng-US"
    
    ;copy the langpack root files
    SetOutPath "$SETTINGS_DIR\lang\eng-US"
    File "C:\Documents and Settings\All Users\Application Data\AMIS\settings\lang\eng-US\*"

    ;copy the langpack audio
    SetOutPath "$SETTINGS_DIR\lang\eng-US\audio"
    File "C:\Documents and Settings\All Users\Application Data\AMIS\settings\lang\eng-US\audio\*"

    ;copy the langpack help book files (and images)
    SetOutPath "$SETTINGS_DIR\lang\eng-US\help"
    File "C:\Documents and Settings\All Users\Application Data\AMIS\settings\lang\eng-US\help\*"
    SetOutPath "$SETTINGS_DIR\lang\eng-US\help\img"
    File "C:\Documents and Settings\All Users\Application Data\AMIS\settings\lang\eng-US\help\img\*"

    ;copy the langpack shortcut book files
    SetOutPath "$SETTINGS_DIR\lang\eng-US\shortcuts"
    File "C:\Documents and Settings\All Users\Application Data\AMIS\settings\lang\eng-US\shortcuts\*"

    LogEx::Write "Copying language pack files: gre-GR"

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
    File "C:\Documents and Settings\All Users\Application Data\AMIS\settings\lang\${LANGID}\help\img\*"

    ;copy the langpack shortcut book files
    SetOutPath "$SETTINGS_DIR\lang\${LANGID}\shortcuts"
    File "C:\Documents and Settings\All Users\Application Data\AMIS\settings\lang\${LANGID}\shortcuts\*"

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
            MessageBox MB_ICONEXCLAMATION "Υπήρξε ένα σφάλμα κατά την εγκατάσταση των αρχείων της Microsoft.  Παρακαλούμε καλέστε την τεχνική βοήθεια αν έχετε προβλήματα εγκατάστασης και λειτουργίας του AMIS."
        ${Else}
            LogEx::Write "MSVC Runtime installed successfully"
        ${EndIf}
    ${EndIf}

End:
    Delete "$MSVC_RUNTIME_INSTALLER"
SectionEnd


;******
; Install Jaws scripts
;*
Section -JawsScripts

    Var /GLOBAL JFW_SCRIPTS_INSTALLER
    StrCpy $JFW_SCRIPTS_INSTALLER "$AMIS_TEMP\${JFW_FILENAME}"
    LogEx::Write "Jaws Scripts Installer copied to temp dir $AMIS_TEMP"

    ; check if the user has jaws installed, then ask if they want to install the scripts
    ${registry::KeyExists} "HKEY_CURRENT_USER\SOFTWARE\Freedom Scientific\JAWS" $0

    ${If} $0 == 0
        LogEx::Write "JAWS found on this machine"

		; default for silent installation is "yes" for jaws scripts installation
		IfSilent InstallScripts
        MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1 "Θέλετε να εγκαταστήσετε το Jaws για τα γραπτά κείμενα των Windows στο AMIS;" IDYES InstallScripts IDNO DoNotInstallScripts

    InstallScripts:
        LogEx::Write "Attempting to install JAWS scripts"
        ExecWait "$JFW_SCRIPTS_INSTALLER" $0

        ${If} $0 != "0"
            LogEx::Write "Error installing JAWS scripts"
			IfSilent End
            MessageBox MB_ICONEXCLAMATION "Υπήρξε ένα σφάλμα κατά την εγκατάσταση του Jaws για γραπτά κείμενα των Windows στο AMIS.  Παρακαλούμε επισκεφτείτε την http://daisy.org/amis για να κατεβάσετε και να εγκαταστήσετε τα γραπτά κείμενα χωριστά."
        ${EndIf}
        Goto End

    DoNotInstallScripts:
        LogEx::Write "User declined JAWS scripts installation"
        Goto End

    ${Else}
        LogEx::Write "JAWS not found on this machine."
		Goto End
    ${EndIf}

End:
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
        	MessageBox MB_OK "Παρακαλούμε αναβαθμίστε την Java Runtime σε έκδοση 1.6 ή νεότερη.  Η υποστήριξη της Java είναι απαραίτητη για την βελτίωση της απεικόνισης κειμένου με το DAISY 3. Μετά το τέλος της εγκατάστασης του AMIS, μπορείτε οποιαδήποτε στιγμή να κατεβάσετε και να εγκαταστήσετε την Java από την http://www.java.com."
        	Goto End
    ${Else}
        LogEx::Write "Java not found"
		IfSilent End
        MessageBox MB_OK "Απαιτείται Java 1.6 ή νεότερη έκδοση για να βελτιωθεί η απεικόνιση κειμένου με το enhanced DAISY 3. Μπορείτε να αναβαθμίσετε το σύστημά σας oποιαδήποτε στιγμή μετά την εγκατάσταση του AMIS κατεβάζοντας και εγκαθιστώντας την Java από την http://www.java.com ."
		Goto End
    ${EndIf}
End:
SectionEnd

;******
; Create shortcuts and icons
;*
Section -AdditionalIcons
    WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "http://daisy.org/amis"
    CreateShortCut "$SMPROGRAMS\AMIS\Ιστοθέση.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
    CreateShortCut "$SMPROGRAMS\AMIS\Απεκαντάσταση.lnk" "$INSTDIR\Uninstall-AMIS.exe"
    CreateShortCut "$SMPROGRAMS\AMIS\Βοήθεια (Greek).lnk" "$SETTINGS_DIR\lang\${LANGID}\help\${HELPFILE_NAME}"
    CreateShortCut "$SMPROGRAMS\AMIS\Συμπτύξεις πληκτρολογίου (Greek).lnk" "$SETTINGS_DIR\lang\${LANGID}\shortcuts\amiskeys.html"
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
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "3.1.3"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "http://daisy.org/amis"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "DAISY Consortium"
    WriteRegStr HKLM "${COMPATIBILITY_REG_KEY}" "$INSTDIR\AMIS.exe" "~ WIN7RTM" 

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
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Είστε σίγουροι ότι θέλετε να αφαιρέσετε πλήρως το $(^Name) και όλα τα συστατικά του;" IDYES End
    Abort
End:
FunctionEnd

;******
; uninstall complete
;**
Function un.onUninstSuccess
    HideWindow
	IfSilent End
    MessageBox MB_ICONINFORMATION|MB_OK "Η εφαρμογή $(^Name) αφαιρέθηκε με επιτυχία από τον υπολογιστή σας."
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

	Delete "$SMPROGRAMS\AMIS\Επιπρόσθετα\*"
	Delete "$SMPROGRAMS\AMIS\*"

    RMDir "$SMPROGRAMS\AMIS\Επιπρόσθετα\"
    RMDir "$SMPROGRAMS\AMIS\"

    SetAutoClose true
SectionEnd

;******
;uninstall the langpacks
;**
Section -un.CopyLangpack

    ; uninstall eng-US
    Delete "$SETTINGS_DIR\lang\eng-US\help\*"
    Delete "$SETTINGS_DIR\lang\eng-US\help\img\*"
    Delete "$SETTINGS_DIR\lang\eng-US\audio\*"

    RMDir "$SETTINGS_DIR\lang\eng-US\help\img"
    RMDir "$SETTINGS_DIR\lang\eng-US\help"
    RMDir "$SETTINGS_DIR\lang\eng-US\audio"

    Delete "$SETTINGS_DIR\lang\eng-US\shortcuts\*"
    RMDir "$SETTINGS_DIR\lang\eng-US\shortcuts"

    Delete "$SETTINGS_DIR\lang\eng-US\*"
    RMDir "$SETTINGS_DIR\lang\eng-US"

    ; uninstall custom language

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

    Delete "$SETTINGS_DIR\lang\readme.txt"
    RMDir "$SETTINGS_DIR\lang"
SectionEnd

Section -un.RemoveHistoryBookmarks
	IfSilent Remove
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Θέλετε να αφαιρέσετε τους σελιδοδείκτες (bookmarks) και το ιστορικό?" IDYES Remove IDNO End
Remove:
    Delete "$SETTINGS_DIR\amisHistory.xml"
    Delete "$SETTINGS_DIR\bmk\*"
    RMDir "$SETTINGS_DIR\bmk"
    ; the settings dir should now be empty
    RMDir "$SETTINGS_DIR"
    RMDir "$APPDATA\AMIS\"
End:
SectionEnd
