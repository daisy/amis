;Scripts for Amis, version 2.2.1
; JAWS version 6.0+

Include "Amis.jsm"
Include "HjConst.jsh"

Globals 
int FirstTime

Globals
String SNavigationMode,
Int JawsSpeech,
Int TempJawsSpeech,
int PastCursorPositionY,
String STOCMode ;

Void Function AutoStartEvent ()
	if( FirstTime==0) then
		Let FirstTime =1
		let JawsSpeech = 1
		Say (MsgAStart1, 2)
		Say(MsgAStart2, 2)
		Say(MsgAStart3, 2)
		Say(MsgAStart4, 2)
	else
		Say(MsgAstart5, 2)
	EndIf
	If JawsSpeech == 0 Then
		SpeechOff ()
	Else
		SpeechOn ()
	EndIf
EndFunction

Void Function AutoFinishEvent ()
	SpeechOn ()
	Say (MsgAFinish, 3)

EndFunction


Script Play ()
	;SayFormattedMessage (0, MsgPlay, MsgPlay)
	TypeCurrentScriptKey ()
EndScript


Script Pause ()
	;SayFormattedMessage (0, MsgPause, MsgPause)
	TypeCurrentScriptKey ()
EndScript


Script IncreaseVolume ()
	TypeCurrentScriptKey ()
	StopSpeech ()
EndScript


Script VolumeDown ()
	TypeCurrentScriptKey ()
	StopSpeech ()
EndScript


Script FasterNum ()
	SayFormattedMessage (0, MsgFast, MsgFast)
	TypeCurrentScriptKey ()

EndScript


Script Faster ()
	SayFormattedMessage (0, MsgFast, MsgFast)
	TypeCurrentScriptKey ()
EndScript


Script SlowerNum ()
	SayFormattedMessage (0, MsgSlow, MsgSlow)
	TypeCurrentScriptKey ()
EndScript


Script Slower ()
	SayFormattedMessage (0, MsgSlow, MsgSlow)
	TypeCurrentScriptKey ()
EndScript


Script NormalSpeed ()
	SayFormattedMessage (0,MsgNormal, MsgNormal)
	TypeCurrentScriptKey ()
EndScript


Script PreviousSection ()
	If IsPCCursor () 
	&& STOCMode != "ON" 
	&& !UserBufferIsActive () 
	Then
		TypeCurrentScriptKey ()
		StopSpeech ()
	Else
		PriorLine ()

var int CurrentCursorPositionY
let CurrentCursorPositionY = GetCursorRow ()

if CurrentCursorPositionY  == PastCursorPositionY Then
;{Space}
;Delay (10)
;{Ctrl+LeftArrow}
;Delay (30)
;{Ctrl+Space}
;JAWSTopOfFile ()

EndIf
let PastCursorPositionY = CurrentCursorPositionY

		SayLine ()
	EndIf
EndScript


Script NextSection ()
	If IsPCCursor () 
	&& STOCMode != "ON"  
	&& !UserBufferIsActive () 
	Then
		TypeCurrentScriptKey ()
		StopSpeech ()
	Else
		NextLine ()

var int CurrentCursorPositionY
let CurrentCursorPositionY = GetCursorRow ()


if CurrentCursorPositionY  == PastCursorPositionY Then
;{Space}
;Delay (10)
;{Ctrl+RightArrow}
;Delay (30)
;{Ctrl+Space}
;JAWSTopOfFile ()

EndIf
let PastCursorPositionY = CurrentCursorPositionY

		SayLine ()
	EndIf
EndScript


Script PreviousPhrase ()

	If IsPCCursor () 
	&& STOCMode != "ON"    
	&& !UserBufferIsActive () 
	Then
		TypeCurrentScriptKey ()
		StopSpeech ()
	Else
		PriorCharacter ()
		SayCharacter ()
	EndIf
EndScript


Script NextPhrase ()

	If IsPCCursor () 
	&& STOCMode != "ON"   
	&& !UserBufferIsActive () 
	Then
		TypeCurrentScriptKey ()
		StopSpeech ()
	Else
		NextCharacter ()
		SayCharacter ()
	EndIf

EndScript


Script GoToSideBar ()

	PCCursor ()
	var
	String SCheckViewsResult 
	let SCheckViewsResult = CheckViews ()
	If SCheckViewsResult== "Default"Then

	;	If GetControlID (GetWindowAtPoint (GetCursorCol (), GetCursorRow ())) ==1000 Then
	if GetWindowName (GetCurrentWindow ()) == "tree1" Then

	SayString ("Already in side navigation bar")
	Else
		TypeCurrentScriptKey ()
		Delay (2)
		PCCursor ()

		;	If GetControlID (GetWindowAtPoint (GetCursorCol(), GetCursorRow())) ==1000 Then
		if GetWindowName (GetCurrentWindow ()) == "tree1" Then

			SayFormattedMessage (3, SideBar_l, SiDeBar_s)
		EndIf
	EndIf
	ElIf SCheckViewsResult  == "Basic" Then
		SayFormattedMessage (3, MsgNoSideBar, MsgNoSideBar)

EndIf
EndScript


Script PrevPage ()
	;	If IsPCCursor () && !IsVirtualPCCursor ()  Then

	If IsPcCursor () 
	&& STOCMode != "ON"   
	&& !UserBufferIsActive () 
	Then
		TypeCurrentScriptKey ()
		StopSpeech ()
	Else
		PriorWord ()
		SayWord ()
	EndIf
EndScript


Script NextPage ()

	If IsPcCursor () 
	&& STOCMode != "ON"   
	&& 	 !UserBufferIsActive () 
 Then
		TypeCurrentScriptKey ()
		StopSpeech ()
	Else
		NextWord ()
		SayWord ()
	EndIf
EndScript


Script sayview ()
	SayViews ()

EndScript

Void Function SayViews ()

	If CheckViewsFromTitle () == "Default" Then
		SayFormattedMessage (3, MsgDefault_l, MsgDefault_s)
	ElIf CheckViewsFromTitle () ==  "Basic" Then
		SayFormattedMessage (3, MsgBasic_l, MsgBasic_s)
	EndIf
	RestoreCursor ()

EndFunction



String Function CheckViews ()
	;Check the Type of menu bar
	SaveCursor ()
	PCCursor ()
	MuteSynthesizerToggle (0)
	{Alt}
	Pause ()
	var
	String Stemp
	let sTemp = GetWord ()
	{Esc}
Pause ()
	MuteSynthesizerToggle (0)
	If sTemp == "File" Then
Return "Default"
	Else 
Return "Basic"
	EndIf
	RestoreCursor ()




EndFunction



String Function CheckViewsFromTitle ()
var
String SName
let SName = GetWindowName (GetAppMainWindow  (GetFocus ()))
If StringContains (SName, "basic") Then
Return "Basic"
Else
Return "Default"
EndIf



EndFunction


Script ToggleViews ()
	TypeCurrentScriptKey ()
	Delay (3)
	If GetControlID (GetFocus ()) != 2 Then
		SayViews ()
	EndIf
EndScript

Script HotKeyHelp ()
	if UserBufferIsActive () then
		UserBufferDeactivate ()
	endif
	SayFormattedMessage (40, MsgVirtualViewer)

EndScript

Script FineNavigation ()	
	If JawsSpeech == 0 Then
		let JawsSpeech = 1
		SpeechOn ()
		let TempJawsSpeech = 1
	EndIf

	;	{Ctrl+Space} ;Pause the book

	;Set background highlight color to default i.e. yellow
	{Ctrl+Shift+e}
	Delay (3)
		var int ActiveCursor
	let ActiveCursor= GetActiveCursor () ;Saves the status of active cursor
	JAWSCursor ()
	SaveCursor ()
	RouteJAWSToPc ()

	JAWSTopOfFile ()
	SetRestriction (1)
	;find the yellow background color on screen
	if FindColors (IgnoreColor, 65535, S_Top, FALSE) == 1 Then
		RouteInvisibleToJAWS ()
		RestoreCursor ()
		SayHighlight (65535)
let SNavigationMode = "On"
	Else
		SayString ("Not found")
		SetActiveCursor (ActiveCursor)
	EndIf
EndScript

Void Function SayHighlight (int BackColor)
	InvisibleCursor ()
	SetRestriction (3)
	;SaveCursor ()

	var int i
	;finds the first character from where highlight color begins
	let i = 0 
	While ( GetColorBackground () == BackColor  && i <10 )
		PriorLine ()
		let i= i+1
	EndWhile
	If  GetColorBackground () ==  BackColor Then
		SayLine ()
	Else
		let i = 0
	While (GetColorBackground () !=  BackColor && i < 30)
		NextWord ()
		let i =i+1
	EndWhile
	;NextLine ()
	SayLine ()
	EndIf
EndFunction

Script FineNavigationExit ()
	if SNavigationMode == "On" Then
		SetRestriction (1)
		PCCursor ()
		let SNavigationMode = "Off"
		SayFormattedMessage (3, MsgExitFineNavigation_l, MsgExitFineNavigation_s)

		If TempJawsSpeech == 1 Then
			let JawsSpeech = 0
			SpeechOff ()
			let TempJawsSpeech = 0
		EndIf
	Else
		SayString ("Not in fine Navigation mode")
	EndIf
EndScript


Script ToggleJawsSpeech ()
	If JawsSpeech == 0 Then
		let JawsSpeech = 1

		; check if amis self voice is On, if it is on then set it to Off
;		if IsAmisSelfVoiceOn  () == 1 Then
;			{Ctrl+Shift+g}
;		EndIf
		SpeechOn ()
		SayFormattedMessage (3, MsgJawsSpeechOn_l, MsgJawsSpeechOn_s)
	Else
		let JawsSpeech = 0
		SayFormattedMessage (3, MsgJawsSpeechOff_l, MsgJawsSpeechOff_s)

		; if amis self voice is Off then set it On
;		if IsAmisSelfVoiceOn  () == 0 Then
;			{Ctrl+Shift+g}
;		EndIf

		SpeechOff ()
	EndIF
EndScript


Int Function IsAmisSelfVoiceOn ()
	SpeechOff ()
	{Alt+f}
	Delay (3)
	{p}
	Delay (3)
	TabKey ()
	Delay (2)
	var
	int CheckStatus 
	let CheckStatus = ControlIsChecked ()
;	{Esc}
	SpeechOn ()

	Return CheckStatus

EndFunction


Script ReadAsTextOnly ()
If GetControlID (GetWindowAtPoint (GetCursorCol (), GetCursorRow ())) ==1000 
&& CheckViews () == "Default"
Then

	PCCursor ()
	var String StrintHeading  
	let StrintHeading  = GetLine ()
	;SayString (ST)
	;	{Ctrl+Space}
	Delay (5)
	MoveToWindow (GetRealWindow (GetFocus ()))
	;SayInteger (GetRealWindow (GetFocus ()))

	var handle i
	let i = GetRealWindow (GetFocus ())

	var handle HtmlWindowHandle
	let  HtmlWindowHandle = FindDescendantWindow (i, 0)
	;SayInteger (d)
	MoveToWindow ( HtmlWindowHandle )


	Delay (5)
	LeftMouseButton ()

	PCCursor ()
	If GetControlID (GetWindowAtPoint (GetCursorCol (), GetCursorRow ())) ==1000 Then

		var int Count
		let Count = 0

		JAWSTopOfFile ()
		SpeechOff ()

	While (GetCurrentHeading () !=StrintHeading   && Count < 1000 )
		PerformScript MoveToNextHeading()
		Delay (2)
		let Count = Count + 1
	EndWhile
	{Ctrl+Space}
	SpeechOn ()
	Delay (2)
	SayLine ()
	;EndIf
	let STOCMode = "ON"
	;SayString ("done")
EndIf
EndIf
EndScript



Script TextOnlyModeExit ()

If STOCMode == "ON" Then
let STOCMode  = "OFF"
Refresh ()
SayString ("Exiting Read text only book mode")
EndIf


EndScript


Script SelectAllHook ()
TypeCurrentScriptKey ()


EndScript

Script SpeakWindow ()
var
String SName
;let SName = GetWindowName (GetRealWindow (GetFocus ()))
let SName = GetWindowName (GetAppMainWindow  (GetFocus ()))
If StringContains (SName, "basic") Then
SayString ("It is basic")
EndIf

;SayString (SName)
SpeechOff ()
{Alt+f}
Pause ()
Pause ()

{p}
Delay (5)
TabKey ()
Delay (5)
SpeechOn ()
SayInteger (ControlIsChecked () )
if ControlIsChecked () == 1 Then
Delay (10)
SayString ("Check")

EndIf
var
string s

let s = GetWindowName (GetFocus ())
;SayString (s)
if s == "tree1" Then
;SayString ("Done")
EndIf
EndScript
