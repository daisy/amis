JFW Script File                                                           $&  �    autostartevent  $  firsttime         
          &  firsttime        &  jawsspeech      Welcome to AMIS window         say        for a help list of Jaws specific commands press  insert h          say       		 for help for AMIS commands press F1        say        AMIS is ready          say           AMIS Window is active          say       $  jawsspeech        
          speechoff              speechon             \     autofinishevent      speechon           Leaving AMIS           say       4     $play        typecurrentscriptkey          4     $pause       typecurrentscriptkey          T     $increasevolume      typecurrentscriptkey            stopspeech        P     $volumedown      typecurrentscriptkey            stopspeech        x     $fasternum            Faster   Faster    sayformattedmessage         typecurrentscriptkey          t     $faster           Faster   Faster    sayformattedmessage         typecurrentscriptkey          x     $slowernum            Slower   Slower    sayformattedmessage         typecurrentscriptkey          t     $slower           Slower   Slower    sayformattedmessage         typecurrentscriptkey          �     $normalspeed              Normal Speed     Normal Speed      sayformattedmessage         typecurrentscriptkey          P    $previoussection         ispccursor  # P $  stocmode     ON  
  
  # x      userbufferisactive    
          typecurrentscriptkey            stopspeech             priorline           getcursorrow    '   %   $  pastcursorpositiony 
        %   &  pastcursorpositiony      sayline          L    $nextsection         ispccursor  # L $  stocmode     ON  
  
  # t      userbufferisactive    
          typecurrentscriptkey            stopspeech             nextline            getcursorrow    '   %   $  pastcursorpositiony 
        %   &  pastcursorpositiony      sayline          �     $previousphrase      ispccursor  # L $  stocmode     ON  
  
  # t      userbufferisactive    
          typecurrentscriptkey            stopspeech             priorcharacter          saycharacter             �     $nextphrase      ispccursor  # H $  stocmode     ON  
  
  # p      userbufferisactive    
          typecurrentscriptkey            stopspeech             nextcharacter           saycharacter             �    $gotosidebar         pccursor            checkviewsfromtitle '   %    Default 
             getcurrentwindow      getwindowname    tree1   
         Already in side navigation bar    saystring              typecurrentscriptkey                 delay           pccursor               getcurrentwindow      getwindowname    tree1   
              Move to side bar     Side Bar      sayformattedmessage             %    Basic   
             		 There is no side bar in basic view  		 There is no side bar in basic view    sayformattedmessage          �     $prevpage        ispccursor  # H $  stocmode     ON  
  
  # p      userbufferisactive    
          typecurrentscriptkey            stopspeech             priorword           sayword          �     $nextpage        ispccursor  # H $  stocmode     ON  
  
  # p      userbufferisactive    
          typecurrentscriptkey            stopspeech             nextword            sayword          ,     $sayview         sayviews          t    sayviews         checkviewsfromtitle  Default 
             		 Now you are in default view mode     Default View Mode     sayformattedmessage            checkviewsfromtitle  Basic   
              Now you are in basic view mode with no side bar.     Basic view mode   sayformattedmessage            restorecursor         l    checkviews       savecursor          pccursor                  mutesynthesizertoggle          Alt   typekey      pause           getword '       Esc   typekey      pause                 mutesynthesizertoggle      %    File    
      Default    	       Basic      	           restorecursor         �     checkviewsfromtitle            getfocus      getappmainwindow      getwindowname   '      %    basic     stringcontains      Basic      	       Default    	         �     $toggleviews         typecurrentscriptkey                 delay              getfocus      getcontrolid         
          sayviews             �    $hotkeyhelp      userbufferisactive          userbufferdeactivate               (   The keyboard shortcuts are:
Play/Pause %KeyFor(Play)
Faster Rate %KeyFor(Faster)
Faster Rate using NumPad %KeyFor(FasterNum)
Slower Rate %KeyFor(Slower)
Slower Rate using NumPad %KeyFor(SlowerNum)
Normal speed rate %KeyFor(NormalSpeed)
Decrease Volume %KeyFor(VolumeDown)
Increase Volume %KeyFor(IncreaseVolume)
Go to next phrase %KeyFor(NextPhrase)
Go to previous phrase %KeyFor(PreviousPhrase)
Go to Next Section %KeyFor(NextSection)
Go to previous section %KeyFor(PreviousSection)
Go to Next Page %KeyFor(NextPage)
Go to previous Page %KeyFor(PrevPage)
Toggle views between default view and basic view mode %KeyFor(ToggleViews)
Say Current view %KeyFor(SayView)
Go to side bar displaying topics tree view %KeyFor(GoToSideBar)
Fine Navigation %KeyFor(FineNavigation)
Fine Navigation Exit %KeyFor(FineNavigationExit)
Toggle Jaws Speech%KeyFor(ToggleJawsSpeech)
Read text Only Book%KeyFor(ReadAsTextOnly )
Exit text Only Book mode%KeyFor(TextOnlyModeExit )

Pres	s ESC to close this message.
Press %keyFor(HotKeyHelp) for JAWS hot key help   sayformattedmessage       �    $finenavigation $  jawsspeech        
          &  jawsspeech       speechon            &  tempjawsspeech         Ctrl+Shift+e      typekey           delay           getactivecursor '        jawscursor          savecursor          routejawstopc           jawstopoffile                setrestriction             �  ��                findcolors       
          routeinvisibletojaws            restorecursor           ��    sayhighlight        On  &  snavigationmode        Not found     saystring         %     setactivecursor          �    sayhighlight            invisiblecursor              setrestriction           '       getcolorbackground  %   
  # � %    
   
  
          priorline      %       
  '   d         getcolorbackground  %   
          sayline             '       getcolorbackground  %   
  # `%       
  
          nextword       %       
  '   $        sayline          �    $finenavigationexit $  snavigationmode  On  
               setrestriction          pccursor        Off &  snavigationmode          Exiting Fine Navigation Mode     Exiting fine navigation   sayformattedmessage    $  tempjawsspeech       
           &  jawsspeech       speechoff            &  tempjawsspeech            Not in fine Navigation mode   saystring            �    $togglejawsspeech   $  jawsspeech        
          &  jawsspeech       isamisselfvoiceon        
         Ctrl+Shift+g      typekey         speechon                Jaws Speech for Amis, on     on    sayformattedmessage             &  jawsspeech           Jaws Speech for Amis, off    off   sayformattedmessage         isamisselfvoiceon         
         Ctrl+Shift+g      typekey         speechoff            ,    isamisselfvoiceon        speechoff          Alt+f     typekey           delay          p     typekey           delay           tabkey               delay           controlischecked    '       Esc   typekey      speechon       %      	      �    $readastextonly            getcursorcol         getcursorrow      getwindowatpoint      getcontrolid      �  
  # �      checkviewsfromtitle  Default 
  
          pccursor            getline '             delay                 getfocus      getrealwindow     movetowindow               getfocus      getrealwindow   '     %          finddescendantwindow    '     %    movetowindow                 delay           leftmousebutton         pccursor                  getcursorcol         getcursorrow      getwindowatpoint      getcontrolid      �  
           '       jawstopoffile           speechoff           getcurrentheading   %   
  # %    �  
  
          $movetonextheading            delay      %       
  '   �       Space     typekey      speechon                 delay           sayline     ON  &  stocmode             �     $textonlymodeexit   $  stocmode     ON  
      OFF &  stocmode         refresh       		 Exiting Read text only book mode      saystring          Ctrl+n    typekey       <     $selectallhook       typecurrentscriptkey          