;Scripts for Amis, version 1.0
; JAWS version 6.0

Include "Amis.jsm"
Include "HjConst.jsh"

Globals 
int FirstTime

Globals
String SNavigationMode

Globals
Int JawsSpeech

Globals
Int TempJawsSpeech

Globals
int PastCursorPositionY

Globals
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
	If IsPCCursor () && STOCMode != "ON" Then
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
	If IsPCCursor () && STOCMode != "ON"  Then
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

	If IsPCCursor () && STOCMode != "ON"    Then
		TypeCurrentScriptKey ()
		StopSpeech ()
	Else
		PriorCharacter ()
		SayCharacter ()
	EndIf
EndScript


Script NextPhrase ()

	If IsPCCursor () && STOCMode != "ON"   Then
		TypeCurrentScriptKey ()
		StopSpeech ()
	Else
		NextCharacter ()
		SayCharacter ()
	EndIf

EndScript


Script GoToSideBar ()
	Pause ()
PCCursor ()

	If GetControlID (GetWindowAtPoint (GetCursorCol (), GetCursorRow ())) ==1000 Then
SayString ("Already in side navigation bar")
Else
	TypeCurrentScriptKey ()
Delay (10)
PCCursor ()

	If GetControlID (GetWindowAtPoint (GetCursorCol(), GetCursorRow())) ==1000 Then
		SayFormattedMessage (3, SideBar_l, SiDeBar_s)
	Else
		SayFormattedMessage (3, MsgNoSideBar, MsgNoSideBar)
	EndIf
EndIf
EndScript


Script PrevPage ()
;	If IsPCCursor () && !IsVirtualPCCursor ()  Then

	If IsPcCursor () && STOCMode != "ON"   Then
		TypeCurrentScriptKey ()
		StopSpeech ()
	Else
		PriorWord ()
		SayWord ()
	EndIf
EndScript


Script NextPage ()

	If IsPcCursor () && STOCMode != "ON"   Then
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
	If CheckViews () == "Default" Then
		SayFormattedMessage (3, MsgDefault_l, MsgDefault_s)
	Else
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


Script ToggleViews ()
	TypeCurrentScriptKey ()
	Delay (1)
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

	{Ctrl+Space} ;Pause the book
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
	SetRestriction (1)
	PCCursor ()
	SayFormattedMessage (3, MsgExitFineNavigation_l, MsgExitFineNavigation_s)

	If TempJawsSpeech == 1 Then
		let JawsSpeech = 0
		SpeechOff ()
		let TempJawsSpeech = 0
	EndIf
EndScript


Script ToggleJawsSpeech ()
	If JawsSpeech == 0 Then
		let JawsSpeech = 1
		SpeechOn ()
		SayFormattedMessage (3, MsgJawsSpeechOn_l, MsgJawsSpeechOn_s)
	Else
		let JawsSpeech = 0
		SayFormattedMessage (3, MsgJawsSpeechOff_l, MsgJawsSpeechOff_s)
		SpeechOff ()
	EndIF


EndScript


Script ReadAsTextOnly ()
	PCCursor ()
	var String StrintHeading  
	let StrintHeading  = GetLine ()
	;SayString (ST)
	{Ctrl+Space}
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
