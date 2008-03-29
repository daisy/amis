!include "LogicLib.nsh"

!define SAPI_ONLINE "http://amis.sf.net/download/sapi51redist.msi"

;************************
;see if SAPI is installed
;if it isn't, look for the downloaded file in our local directory
;if not found, give the user options a. download it, b. locate manually, c. ignore it
Function SapiPage

    Var /GLOBAL SAPI_INSTALLER
    Var /GLOBAL INI_VALUE
  
    ;find if SAPI is installed
    ${If} ${FileExists} "$PROGRAMFILES\Common Files\Microsoft\Shared\Speech\sapi.dll"
     ${OrIf} ${FileExists} "$PROGRAMFILES\Common Files\Microsoft Shared\Speech\sapi.dll"
        Goto End 
    ${Else}
        Call showSapiPage
    ${EndIf}
   
End:
FunctionEnd

;**********************
;display the options for getting SAPI
Function ShowSapiPage

  !insertmacro MUI_HEADER_TEXT "Speech API Required" "The required speech library (SAPI 5.1) was not found on your computer."
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "sapipage.ini"

  ;option 1: the user will download sapi
  !insertmacro MUI_INSTALLOPTIONS_READ $INI_VALUE "sapipage.ini" "Field 1" "State"
  ${If} $INI_VALUE == "1"
    Call DownloadSapiAndInstall
  ${EndIf}
  
  ;option 2: the user will point to the sapi installer manually
  !insertmacro MUI_INSTALLOPTIONS_READ $INI_VALUE "sapipage.ini" "Field 2" "State"
  ${If} $INI_VALUE == "1"
    Push $R0
    Call ShowFileBrowse
    Pop $R0
    
    ${If} $R0 == "" 
        Call SkipSapi
    ${Else}
        Call RunSapiSetup
    ${EndIf}
    
    Goto End
  ${EndIf}

  ;option 3: the user will skip installing sapi
  !insertmacro MUI_INSTALLOPTIONS_READ $INI_VALUE "sapipage.ini" "Field 3" "State"
  ${If} $INI_VALUE == "1"
    Call SkipSapi
  ${EndIf}

End:
FunctionEnd

;************************
;let the user pick a file from their hard drive
Function ShowFileBrowse

ShowDlg:
    !insertmacro MUI_HEADER_TEXT "Speech API Setup" "Please find where you have downloaded the SAPI setup program."
    !insertmacro MUI_INSTALLOPTIONS_DISPLAY "filebrowse.ini"
    !insertmacro MUI_INSTALLOPTIONS_READ $INI_VALUE "filebrowse.ini" "Field 1" "State"
    
    ${If} $INI_VALUE == ""
      MessageBox MB_YESNO "No file was selected.  Would you like to browse again?" IDNO End
      Goto ShowDlg
    ${EndIf}
End:     
    Exch $INI_VALUE
FunctionEnd

;*************************
; skip sapi installation
Function SkipSapi
   MessageBox MB_ICONEXCLAMATION "Warning!  Skipping SAPI installation.  If AMIS does not run properly, re-install SAPI after downloading the necessary files from http://amis.sf.net/download/sapi51redist.msi"
FunctionEnd

;***************************
; download sapi and install
Function DownloadSapiAndInstall

   ;local copy will be stored here
   StrCpy $SAPI_INSTALLER "$TEMP\sapi51redist.msi"
   
   !insertmacro MUI_HEADER_TEXT "Speech API Setup" "The required files are being downloaded."
   
    ;10 minute time-out for a 7MB file
    ;users can interrupt the download dialog as well
    inetc::get /TIMEOUT 10000 /BANNER "Downloading speech API files" \
        "${SAPI_ONLINE}" \
        "$TEMP\sapi51redist.msi"
  Pop $0
  StrCmp $0 "OK" dlok
  MessageBox MB_OK|MB_ICONEXCLAMATION \ 
    "Download failed.  Please download SAPI 5.1 manually and install it after completing the AMIS installation process." \ 
    /SD IDOK
  Goto End
dlok:
  Call RunSapiSetup
  Delete "$SAPI_INSTALLER"
  Goto End
  
 End: 
FunctionEnd

;*********************
;run sapi setup
Function RunSapiSetup
  ExecWait 'msiexec /package "$SAPI_INSTALLER"' $0
  StrCmp $0 "0" Success Error
 Success:
    MessageBox MB_OK "The speech API (SAPI 5.1) files were successfully installed"
    Goto End
  Error:
    MessageBox MB_ICONEXCLAMATION "There was an error encountered while attempting to install the speech API (SAPI 5.1) files.  Please download and install manually after completing the AMIS installation."
    Goto End
    
  End:
FunctionEnd