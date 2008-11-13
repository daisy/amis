;Scripts for Amis, version  3.1 beta
; For AMIS 3.0 Beta
; JAWS version 7.0 and above

Include "Amis.jsm"
Include "HjConst.jsh"

Globals 
int FirstTime

Globals
String SNavigationMode,
Int JawsSpeech,
Int TempJawsSpeech,
int PastCursorPositionY,
String STOCMode,

String AmisDirectory,
String PreferenceHighlightBkColor ,
String g_STitle


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
;	If JawsSpeech == 0 Then
		if IsAmisSelfVoiceOnFromTitle  () == 1 Then
		SayFormattedMessage (3, MsgJawsMuted_l ,  " " )
		SpeechOff ()
let JawsSpeech = 0
	Else
		SpeechOn ()
	Let JawsSpeech = 1
	EndIf

	; activate virtual buffer if in Text only book reading mode
	AmisActivateVirtualCursurSwitch ()

	; Get Amis directory
	let AmisDirectory = GetAppFilePath ()
	let AmisDirectory = StringReplaceSubstrings (AmisDirectory, GetAppFileName (), "")

	; exits text reading modes if new book is open
	If (  IsAMISHasNewBook () == 1 ) Then
		ExitAmisTextReadingModes ()
	EndIf
EndFunction


Function AmisActivateVirtualCursurSwitch ()
	If ( GetControlID (GetFocus ()) == 0 ) Then
		If (     STOCMode == "On" ) Then
			if GetJCFOption (OPT_VIRTUAL_PC_CURSOR)==0  then
				SetDefaultJCFOption (OPT_VIRTUAL_PC_CURSOR, 1 )
			EndIf
		ElIf ( STOCMode == "Off" ) Then
			if GetJCFOption (OPT_VIRTUAL_PC_CURSOR)==1   then
					SetDefaultJCFOption (OPT_VIRTUAL_PC_CURSOR, 0  )
			EndIf
		EndIf
	EndIf


EndFunction


Int Function IsAMISHasNewBook ()
	var String  STitle
	let STitle = GetAppTitle ()

	var Int StringMark
	let StringMark = StringContains (STitle, "Default view mode -")
	If ( StringMark == 0 ) Then
		let StringMark = StringContains (STitle, "Basic view mode -")
	EndIf
	let STitle = stringChopRight (STitle, StringMark)

	If ( g_STitle != STitle ) Then
		let g_STitle = STitle
		Return 1
	Else
		Return 0
	EndIf
EndFunction

Function WindowDestroyedEvent (handle hWindow)
	WindowDestroyedEvent (hWindow)

var string SWindowName 
let SWindowName = GetWindowName (GetRealWindow (GetFocus ())) 

	If (   TempJawsSpeech == 1 
		&&   JawsSpeech == 0 
&&   StringContains (SWindowName, "Open") == 0 )Then
		SpeechOff ()
	EndIf
	ScheduleFunction ("ExitTextModeOnNewBook", 25)
EndFunction


Void Function WindowActivatedEvent (handle hWindow)
WindowActivatedEvent (hWindow)

ScheduleFunction ("UnMuteJawsSpeechOnOpenDialog", 12)
EndFunction


Function UnMuteJawsSpeechOnOpenDialog ()
	If ( JawsSpeech == 0 ) Then
		var string SWindowName 
		let SWindowName = GetWindowName (GetRealWindow (GetFocus ())) 
		If ( InHJDialog () 
		||    StringContains (SWindowName, "Open") != 0 ) Then
			SpeechOn ()
			let TempJawsSpeech = 1
			SayString (SWindowName)
		EndIf
	EndIf



EndFunction

Void Function ExitTextModeOnNewBook ()
	If (  IsAMISHasNewBook () == 1  ) Then
		ExitAmisTextReadingModes ()
	EndIf


EndFunction

Void Function AutoFinishEvent ()
	if GetJCFOption (OPT_VIRTUAL_PC_CURSOR)==0  then
		SetDefaultJCFOption (OPT_VIRTUAL_PC_CURSOR, 1 )
	EndIf

	SpeechOn ()
	if SNavigationMode == "On" Then
ExitAmisFineNavigationMode ()
EndIf
	Say (MsgAFinish, 3)

EndFunction


Script PlayPause ()
	;SayFormattedMessage (0, MsgPlay, MsgPlay)
{Space}
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
var Int ActiveCursor
let ActiveCursor = GetActiveCursor ()
	PCCursor ()
	var
	String SCheckViewsResult 
	let SCheckViewsResult = CheckViewsFromTitle ()
	If SCheckViewsResult== "Default"Then

	;	If GetControlID (GetWindowAtPoint (GetCursorCol (), GetCursorRow ())) ==1000 Then
	if GetWindowName (GetCurrentWindow ()) == "tree1" Then
	if SNavigationMode == "On" Then
SayString ( "Side Bar" )
Else
		SayString ("Already in side navigation bar")
EndIf
	Else
		TypeCurrentScriptKey ()
		Delay (2)
		PCCursor ()

		;	If GetControlID (GetWindowAtPoint (GetCursorCol(), GetCursorRow())) ==1000 Then
		if GetWindowName (GetCurrentWindow ()) == "tree1" Then

			SayFormattedMessage (3, SideBar_l, SiDeBar_s)
		EndIf
	EndIf
ExitAmisTextReadingModes ()

	ElIf SCheckViewsResult  == "Basic" Then
		SayFormattedMessage (3, MsgNoSideBar, MsgNoSideBar)
SetActiveCursor (ActiveCursor)

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



String Function CheckViewsFromTitle ()
	var
	String SName
	let SName = GetWindowName (GetAppMainWindow  (GetFocus ()))
	If StringContains (SName, "Basic") Then
		Return "Basic"
	ElIf StringContains (SName, "Default") Then
		Return "Default"
	EndIf

EndFunction



Script ToggleViews ()
	TypeCurrentScriptKey ()
	Delay (1)
	If GetControlID (GetFocus ()) != 2 Then
StopSpeech ()
		SayViews ()
	EndIf
	if SNavigationMode == "On" Then
ExitAmisFineNavigationMode ()
EndIf
EndScript

Script HotKeyHelp ()
	if UserBufferIsActive () then
		UserBufferDeactivate ()
	endif
	SayFormattedMessage (40, MsgVirtualViewer)

	If ( JawsSpeech == 0
	&&    UserBufferIsActive () ) Then
		SpeechOn ()
		let TempJawsSpeech = 1
		SayString ("JAWS Hotkey Help")
		JAWSTopOfFile ()
	EndIf
EndScript

Script FineNavigation ()	
	If JawsSpeech == 0 Then
		let JawsSpeech = 1
		SpeechOn ()
		let TempJawsSpeech = 1
	EndIf

If IsPlaying() == 1 Then
	{Space} ;Pause the book
Delay (3)
EndIf
	;Set background highlight color to default i.e. yellow
;	{Ctrl+Shift+e}
	Delay (3)
	var int ActiveCursor
	let ActiveCursor= GetActiveCursor () ;Saves the status of active cursor
	JAWSCursor ()
	SaveCursor ()
	RouteJAWSToPc ()

	JAWSTopOfFile ()
	SetRestriction (1)
	;find the Highlighted background color on screen
	if FindColors (IgnoreColor,  RGBHexToColor ( GetAmisHighlightBKColor ()) , S_Top, FALSE) == 1   Then
		RouteInvisibleToJAWS ()
		RestoreCursor ()
		SayHighlight (RGBHexToColor ( GetAmisHighlightBKColor ())  )
		let SNavigationMode = "On"
	Else
		SayString ("Not found")
RestoreCursor ()
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
ExitAmisFineNavigationMode ()
	Else
		SayString ("Not in fine Navigation mode")
	EndIf
EndScript


Function ExitAmisFineNavigationMode ()
	SetRestriction (1)
	PCCursor ()
	let SNavigationMode = "Off"
	SayFormattedMessage (3, MsgExitFineNavigation_l, MsgExitFineNavigation_s)

	If TempJawsSpeech == 1 Then
		let JawsSpeech = 0
		SpeechOff ()
		let TempJawsSpeech = 0
	EndIf

EndFunction

Script ToggleJawsSpeech ()
	If JawsSpeech == 0 Then
		let JawsSpeech = 1

		; check if amis self voice is On, if it is on then set it to Off
		if IsAmisSelfVoiceOnFromTitle  () == 1 Then
			{Ctrl+Shift+g}
		EndIf
		SpeechOn ()
		SayFormattedMessage (3, MsgJawsSpeechOn_l, MsgJawsSpeechOn_s)
	Else
		let JawsSpeech = 0
		SayFormattedMessage (3, MsgJawsSpeechOff_l, MsgJawsSpeechOff_s)

		; if amis self voice is Off then set it On
		if IsAmisSelfVoiceOnFromTitle  () == 0 Then
			{Ctrl+Shift+g}
		EndIf

		SpeechOff ()
	EndIF
EndScript


Int Function IsAmisSelfVoiceOnFromTitle ()
	var
	int CheckStatus 
	var
	String SName
	let SName = GetWindowName (GetAppMainWindow  (GetFocus ()))
	If StringContains (SName, "Not self-voicing") Then
let CheckStatus  = 0 
	ElIf StringContains (SName, "Self-voicing") Then
let CheckStatus  = 1 
EndIf
	Return CheckStatus




EndFunction


Int Function IsPlaying ()
	var
	int CheckStatus 
	var
	String SName
	let SName = GetWindowName (GetAppMainWindow  (GetFocus ()))
	If StringContains (SName, "Paused") Then
let CheckStatus  = 0 
	ElIf StringContains (SName, "Playing") Then
let CheckStatus  = 1 
Else
let CheckStatus  = 2
EndIf
	Return CheckStatus

EndFunction


Script ReadAsTextOnly ()
If (SNavigationMode == "On"  ) Then
ExitAmisFineNavigationMode ()
EndIf

	If GetControlID (GetWindowAtPoint (GetCursorCol (), GetCursorRow ())) ==1000 
	&& CheckViewsFromTitle () == "Default"
	Then
		PCCursor ()
		If  IsPlaying () == 1  Then
			{Space}
		EndIf
	Delay (1)

	var String StringHeading  
	var Int TreeLevel
	let StringHeading  = GetObjectName (1)
	let TreeLevel = GetTreeViewLevel ()

let StringHeading = stringStripAllBlanks (StringHeading)
SayFormattedMessage (3, MsgMoveToTextWindow_l, MsgMoveToTextWindow_s)
SpeechOff ()
{Control+T}

	Delay (2)2
	PCCursor ()
	if GetJCFOption (OPT_VIRTUAL_PC_CURSOR)==0  then
		SetDefaultJCFOption (OPT_VIRTUAL_PC_CURSOR, 1 )
	EndIf

	If GetControlID (GetFocus ()) == 0 Then 
		Refresh (1)
Delay (2)
		var int Count
		let Count = 0

		PerformScript MoveToFirstHeading()

		var Int TotalHeadingCount
		let TotalHeadingCount = GetHeadingCount (0)

		While (  IsTargetHtmlHeading( StringHeading )  == 0
		&&  Count <= TotalHeadingCount )
			PerformScript MoveToNextHeading()
;MoveToHeading (s_Top, TreeLevel+1 )
;			Delay (1)
			let Count = Count + 1
		EndWhile
		If  IsPlaying () == 1  Then
			{Space}
		EndIf
		SpeechOn ()
		Delay (1)
		SayLine ()
	EndIf
Else
;SayFormattedMessage (3, MsgNotInSidebar_l, MsgNotInSidebar_s)
SayFormattedMessage (3, MsgMoveToTextWindow_s, MsgMoveToTextWindow_s)
	if GetJCFOption (OPT_VIRTUAL_PC_CURSOR)==0  then
		SetDefaultJCFOption (OPT_VIRTUAL_PC_CURSOR, 1 )
Delay(4)
	EndIf
NextLine ()
SayLine ()
EndIf
		let STOCMode = "ON"
EndScript



Script TextOnlyModeExit ()

If STOCMode == "ON" Then
ExitAmisTextOnlyReadingMode ()
EndIf

EndScript


Function ExitAmisTextOnlyReadingMode ()
let STOCMode  = "OFF"
Refresh ()
	if GetJCFOption (OPT_VIRTUAL_PC_CURSOR)==1  then
		SetDefaultJCFOption (OPT_VIRTUAL_PC_CURSOR, 0 )
		StopSpeech ()
	EndIf

SayString ("Exiting Read text only book mode")



EndFunction

Void Function ExitAmisTextReadingModes ()
	If STOCMode == "ON" Then
		ExitAmisTextOnlyReadingMode ()
	EndIf

		if SNavigationMode == "On" Then
		ExitAmisFineNavigationMode ()
	EndIf


EndFunction

Script MoveToTextWindow ()
	TypeCurrentScriptKey ()
	Delay (2)
	if GetJCFOption (OPT_VIRTUAL_PC_CURSOR)==1  then
			SetDefaultJCFOption (OPT_VIRTUAL_PC_CURSOR, 0 )
			StopSpeech ()
	EndIf
	SayFormattedMessage (3, MsgMoveToMainWindow_l, MsgMoveToMainWindow_s)
	ExitAmisTextReadingModes ()
EndScript


Int Function IsTargetHtmlHeading (string HeadingName)
var String CurrentHeadingName
let CurrentHeadingName = GetCurrentHeading ()
let CurrentHeadingName  = stringStripAllBlanks (CurrentHeadingName )

var String PrefColor

If ( CurrentHeadingName == HeadingName ) Then
If ( IsAmisTextHighlighted ()  == 1 ) Then
Return 1
Else
; check if next line under heading is highlighted
NextLine ()

let PrefColor = GetAmisHighlightBKColor ()
If ( PrefColor != "" ) Then
let PreferenceHighlightBkColor  = PrefColor
EndIf

If ( GetColorBackground () == RGBHexToColor (PreferenceHighlightBkColor  )) Then
Return 1
EndIf
; Check if this line was null  with next sentence
PriorLine ()
NextSentence ()
If ( GetColorBackground () == RGBHexToColor (PreferenceHighlightBkColor  )) Then
Return 1
EndIf

EndIf
EndIf
Return 0

EndFunction


Int Function IsAmisTextHighlighted ()
var String ElementInfo
let ElementInfo = GetElementDescription (1, 0)
;SayString (ElementInfo)


var String MatchString

var String BkColor
let BkColor = GetAmisHighlightBKColor () 

If ( BkColor!= "" ) Then
let PreferenceHighlightBkColor = BkColor
EndIf

let MatchString = "BACKGROUND-COLOR: " + "#" +  StringLower ( PreferenceHighlightBkColor )
	
;UserBufferClear ()
;UserBufferActivate (1)
;UserBufferAddText (MatchString)

If ( StringContains ( ElementInfo , MatchString) != 0 ) Then
;SayString ("Highlighted")
Return 1
Else
Return 0
EndIf


EndFunction


String Function GetAmisHighlightBKColor ()


var String Color

let Color = GetAmisPreferenceValue ("highlight-bg")
;let Color = StringReplaceChars (Color, "#", "")
If StringContains (Color, "#") Then
let Color = stringChopLeft (Color, 1)
EndIf
Return Color

EndFunction

String Function GetAmisPreferenceValue ( String Element)
var String FileString
var String PreferenceValue
let FileString = GetFileTextInString (AmisDirectory + "settings\\config\\amisPrefs.xml")

var Int StartIndex
var Int EndIndex
let StartIndex = StringContains ( FileString , Element)
let PreferenceValue = stringChopLeft (FileString, StartIndex)
let EndIndex = StringContains (PreferenceValue, "/>")
let PreferenceValue = StringLeft (PreferenceValue, EndIndex-2)

let StartIndex = StringContains (PreferenceValue, "value=")
let PreferenceValue = stringChopLeft (PreferenceValue, StartIndex+ 6  )

Return PreferenceValue
EndFunction

; file operation functions
Object Function ObjectCreate (String SObjectName)
 Var Object OReturnObject
;Object Null

let OReturnObject  = CreateObjectEx (SObjectName, 1 )

 If !OReturnObject  Then
let OReturnObject = CreateObjectEx (SObjectName, 0)
 EndIf
 If !OReturnObject Then
let OReturnObject = GetObject (SObjectName)
 EndIf
 Return OReturnObject

 EndFunction

String Function GetFileTextInString (string SFile)
 Var
Object OSystem,
Object OFile,
String SFileString

let OSystem  = ObjectCreate("Scripting.FilesystemObject")
let OFile =OSystem .OpenTextFile( SFile , 1, 0)

let SFileString = OFile.ReadAll()

OFile.close()
var  Object Null
let OFile = null
let OSystem = null
;SayString ( SFileString)
 Return SFileString
 EndFunction



Script SayTitleHook ()
If ( JawsSpeech == 0 ) Then
	SpeechOn ()
	PerformScript SayWindowTitle()
	SayFormattedMessage (3, MsgJawsMuted_l ,MsgJawsMuted_l)
	SpeechOff ()
Else
	PerformScript SayWindowTitle()
EndIf


EndScript
