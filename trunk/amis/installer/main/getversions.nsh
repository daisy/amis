;**************************
;get the directx version
;**************************
Function GetDXVersion
    Push $0
    Push $1

    ReadRegStr $0 HKLM "Software\Microsoft\DirectX" "Version"
    IfErrors noDirectX

    StrCpy $1 $0 2 5    ; get the minor version
    StrCpy $0 $0 2 2    ; get the major version
    IntOp $0 $0 * 100   ; $0 = major * 100 + minor
    IntOp $0 $0 + $1
    Goto done

noDirectX:
    StrCpy $0 0

done:
    Pop $1
    Exch $0
FunctionEnd


; GetWindowsVersion
;
; Based on Yazno's function, http://yazno.tripod.com/powerpimpit/
; Updated by Joost Verburg
;
; Returns on top of stack
;
; Windows Version (95, 98, ME, NT x.x, 2000, XP, 2003, Vista)
; or
; '' (Unknown Windows Version)
;
; Usage:
;   Call GetWindowsVersion
;   Pop $R0
;   ; at this point $R0 is "NT 4.0" or whatnot
 
Function GetWindowsVersion
    Push $R0
    Push $R1
    StrCpy $R0 "XP"
    Goto lbl_done
    
    ClearErrors

    ReadRegStr $R0 HKLM \
    "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion

    IfErrors 0 lbl_winnt

    ; we are not NT
    ReadRegStr $R0 HKLM \
    "SOFTWARE\Microsoft\Windows\CurrentVersion" VersionNumber

    StrCpy $R1 $R0 1
    StrCmp $R1 '4' 0 lbl_error

    StrCpy $R1 $R0 3

    StrCmp $R1 '4.0' lbl_win32_95
    StrCmp $R1 '4.9' lbl_win32_ME lbl_win32_98

lbl_win32_95:
    StrCpy $R0 '95'
    Goto lbl_done

lbl_win32_98:
    StrCpy $R0 '98'
    Goto lbl_done

lbl_win32_ME:
    StrCpy $R0 'ME'
    Goto lbl_done

lbl_winnt:

    StrCpy $R1 $R0 1

    StrCmp $R1 '3' lbl_winnt_x
    StrCmp $R1 '4' lbl_winnt_x

    StrCpy $R1 $R0 3

    StrCmp $R1 '5.0' lbl_winnt_2000
    StrCmp $R1 '5.1' lbl_winnt_XP
    StrCmp $R1 '5.2' lbl_winnt_2003
    StrCmp $R1 '6.0' lbl_winnt_vista lbl_error

lbl_winnt_x:
    StrCpy $R0 "NT $R0" 6
    Goto lbl_done

lbl_winnt_2000:
    Strcpy $R0 '2000'
    Goto lbl_done

lbl_winnt_XP:
    Strcpy $R0 'XP'
    Goto lbl_done

lbl_winnt_2003:
    Strcpy $R0 '2003'
    Goto lbl_done

lbl_winnt_vista:
    Strcpy $R0 'Vista'
    Goto lbl_done

lbl_error:
    Strcpy $R0 ''

lbl_done:
    Pop $R1
    Exch $R0

FunctionEnd