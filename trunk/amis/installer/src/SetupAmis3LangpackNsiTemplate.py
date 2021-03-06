#!/usr/bin/env python




##################################################
## DEPENDENCIES
import sys
import os
import os.path
try:
    import builtins as builtin
except ImportError:
    import __builtin__ as builtin
from os.path import getmtime, exists
import time
import types
from Cheetah.Version import MinCompatibleVersion as RequiredCheetahVersion
from Cheetah.Version import MinCompatibleVersionTuple as RequiredCheetahVersionTuple
from Cheetah.Template import Template
from Cheetah.DummyTransaction import *
from Cheetah.NameMapper import NotFound, valueForName, valueFromSearchList, valueFromFrameOrSearchList
from Cheetah.CacheRegion import CacheRegion
import Cheetah.Filters as Filters
import Cheetah.ErrorCatchers as ErrorCatchers

##################################################
## MODULE CONSTANTS
VFFSL=valueFromFrameOrSearchList
VFSL=valueFromSearchList
VFN=valueForName
currentTime=time.time
__CHEETAH_version__ = '2.4.4'
__CHEETAH_versionTuple__ = (2, 4, 4, 'development', 0)
__CHEETAH_genTime__ = 1371701421.394979
__CHEETAH_genTimestamp__ = 'Wed Jun 19 21:10:21 2013'
__CHEETAH_src__ = 'SetupAmis3LangpackNsiTemplate.tmpl'
__CHEETAH_srcLastModified__ = 'Wed Jun 19 21:10:16 2013'
__CHEETAH_docstring__ = 'Autogenerated by Cheetah: The Python-Powered Template Engine'

if __CHEETAH_versionTuple__ < RequiredCheetahVersionTuple:
    raise AssertionError(
      'This template was compiled with Cheetah version'
      ' %s. Templates compiled before version %s must be recompiled.'%(
         __CHEETAH_version__, RequiredCheetahVersion))

##################################################
## CLASSES

class SetupAmis3LangpackNsiTemplate(Template):

    ##################################################
    ## CHEETAH GENERATED METHODS


    def __init__(self, *args, **KWs):

        super(SetupAmis3LangpackNsiTemplate, self).__init__(*args, **KWs)
        if not self._CHEETAH__instanceInitialized:
            cheetahKWArgs = {}
            allowedKWs = 'searchList namespaces filter filtersLib errorCatcher'.split()
            for k,v in KWs.items():
                if k in allowedKWs: cheetahKWArgs[k] = v
            self._initCheetahInstance(**cheetahKWArgs)
        

    def respond(self, trans=None):



        ## CHEETAH: main method generated for this template
        if (not trans and not self._CHEETAH__isBuffering and not callable(self.transaction)):
            trans = self.transaction # is None unless self.awake() was called
        if not trans:
            trans = DummyTransaction()
            _dummyTrans = True
        else: _dummyTrans = False
        write = trans.response().write
        SL = self._CHEETAH__searchList
        _filter = self._CHEETAH__currentFilter
        
        ########################################
        ## START - generated method body
        
        write(u''';******\r
; Usage:\r
; makensis setup-amis3-langpack.nsi\r
;**\r
\r
''')
        #  the idea was to use only python vars, but in some cases, it was easier to copy them into NSIS vars
        #  because the python template compiler (cheetah) has an escape sequence ("\$var") that conflicts
        #  with path names
        #  so "a\b\$c" comes out as "a\b$c"
        #  and there's no elegant way around it
        #  so we have a few custom defines
        write(u''';******\r
; custom defines\r
;**\r
!define LANGID "''')
        _v = VFFSL(SL,"langid",True) # u'$langid' on line 15, col 17
        if _v is not None: write(_filter(_v, rawExpr=u'$langid')) # from line 15, col 17.
        write(u'''"\r
!define PRODUCT_NAME "''')
        _v = VFFSL(SL,"product_name",True) # u'$product_name' on line 16, col 23
        if _v is not None: write(_filter(_v, rawExpr=u'$product_name')) # from line 16, col 23.
        write(u'''"\r
!define HELPFILE_NAME "''')
        _v = VFFSL(SL,"helpfile_name",True) # u'$helpfile_name' on line 17, col 24
        if _v is not None: write(_filter(_v, rawExpr=u'$helpfile_name')) # from line 17, col 24.
        write(u'''"\r
!define LANGNAME "''')
        _v = VFFSL(SL,"langname",True) # u'$langname' on line 18, col 19
        if _v is not None: write(_filter(_v, rawExpr=u'$langname')) # from line 18, col 19.
        write(u'''"\r
\r
!define PRODUCT_DIR_REGKEY "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\AMIS.exe"\r
!define PRODUCT_UNINST_KEY "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall${PRODUCT_NAME}"\r
!define PRODUCT_UNINST_ROOT_KEY "HKLM"\r
\r
;******\r
; pages, components, and script includes\r
;**\r
; required for InstallLib\r
!include "Library.nsh"\r
\r
; MUI 1.67 compatible ------\r
!include "MUI.nsh"\r
\r
; Support for conditional logic (should be in c:\\program files\\nsis\\include by default)\r
!include "LogicLib.nsh"\r
\r
; MUI Settings\r
!define MUI_ABORTWARNING\r
!define MUI_ICON "${NSISDIR}\\Contrib\\Graphics\\Icons\\modern-install.ico"\r
!define MUI_UNICON "${NSISDIR}\\Contrib\\Graphics\\Icons\\modern-uninstall.ico"\r
\r
; Welcome page\r
!insertmacro MUI_PAGE_WELCOME\r
\r
; License page\r
!insertmacro MUI_PAGE_LICENSE "lgpl.txt"\r
\r
; Instfiles page\r
!insertmacro MUI_PAGE_INSTFILES\r
\r
; Finish page\r
!insertmacro MUI_PAGE_FINISH\r
\r
; Uninstaller pages\r
!insertmacro MUI_UNPAGE_INSTFILES\r
\r
; Language files\r
!insertmacro MUI_LANGUAGE "''')
        _v = VFFSL(SL,"nsis_language",True) # u'$nsis_language' on line 57, col 28
        if _v is not None: write(_filter(_v, rawExpr=u'$nsis_language')) # from line 57, col 28.
        write(u'''"\r
\r
; MUI end ------\r
\r
\r
;******\r
; directory, installer exe name, etc\r
;**\r
Name "''')
        _v = VFFSL(SL,"product_name",True) # u'$product_name' on line 65, col 7
        if _v is not None: write(_filter(_v, rawExpr=u'$product_name')) # from line 65, col 7.
        write(u''' ''')
        _v = VFFSL(SL,"product_version",True) # u'$product_version' on line 65, col 21
        if _v is not None: write(_filter(_v, rawExpr=u'$product_version')) # from line 65, col 21.
        write(u'''"\r
;this is the name of the installer that gets created.\r
OutFile "''')
        _v = VFFSL(SL,"build_dir",True) # u'$build_dir' on line 67, col 10
        if _v is not None: write(_filter(_v, rawExpr=u'$build_dir')) # from line 67, col 10.
        write(u'''\\Setup-amis-langpack-${LANGNAME}.exe"\r
\r
;InstallDir "$APPDATA\\AMIS\\settings"\r
\r
ShowInstDetails show\r
ShowUnInstDetails show\r
\r
;******\r
; copy all files, register DLLs, etc\r
;**\r
Section "MainSection" SEC01\r
\r
  ;the settings dir will live here\r
\tVar /GLOBAL SETTINGS_DIR\r
\r
\t;figure out the user\'s application data directory\r
\t;look for the "all users" context\r
\tSetShellVarContext all\r
\tStrCpy $SETTINGS_DIR $APPDATA\\AMIS\\settings\r
\r
  ;copy the langpack root files\r
  SetOutPath "$SETTINGS_DIR\\lang\\${LANGID}"\r
  File "''')
        _v = VFFSL(SL,"appdata_dir",True) # u'$appdata_dir' on line 89, col 9
        if _v is not None: write(_filter(_v, rawExpr=u'$appdata_dir')) # from line 89, col 9.
        write(u'''\\AMIS\\settings\\lang\\${LANGID}\\*"\r
\r
  ;copy the langpack audio\r
  SetOutPath "$SETTINGS_DIR\\lang\\${LANGID}\\audio"\r
  File "''')
        _v = VFFSL(SL,"appdata_dir",True) # u'$appdata_dir' on line 93, col 9
        if _v is not None: write(_filter(_v, rawExpr=u'$appdata_dir')) # from line 93, col 9.
        write(u'''\\AMIS\\settings\\lang\\${LANGID}\\audio\\*"\r
\r
  ;copy the langpack help book files (and images)\r
  SetOutPath "$SETTINGS_DIR\\lang\\${LANGID}\\help"\r
  File "''')
        _v = VFFSL(SL,"appdata_dir",True) # u'$appdata_dir' on line 97, col 9
        if _v is not None: write(_filter(_v, rawExpr=u'$appdata_dir')) # from line 97, col 9.
        write(u'''\\AMIS\\settings\\lang\\${LANGID}\\help\\*"\r
  SetOutPath "$SETTINGS_DIR\\lang\\${LANGID}\\help\\img"\r
  File /nonfatal "''')
        _v = VFFSL(SL,"appdata_dir",True) # u'$appdata_dir' on line 99, col 19
        if _v is not None: write(_filter(_v, rawExpr=u'$appdata_dir')) # from line 99, col 19.
        write(u'''\\AMIS\\settings\\lang\\${LANGID}\\help\\img\\*"\r
  ;copy the langpack shortcut book files\r
  SetOutPath "$SETTINGS_DIR\\lang\\${LANGID}\\shortcuts"\r
  File "''')
        _v = VFFSL(SL,"appdata_dir",True) # u'$appdata_dir' on line 102, col 9
        if _v is not None: write(_filter(_v, rawExpr=u'$appdata_dir')) # from line 102, col 9.
        write(u'''\\AMIS\\settings\\lang\\${LANGID}\\shortcuts\\*"\r
\r
SectionEnd\r
\r
;******\r
; Create shortcuts to the documentation (found in the language pack)\r
;*\r
Section -AdditionalIcons\r
  CreateShortCut "''')
        write(_filter("$SMPROGRAMS\\AMIS\\"))
        _v = VFFSL(SL,"text121",True) # u'$text121' on line 110, col 45
        if _v is not None: write(_filter(_v, rawExpr=u'$text121')) # from line 110, col 45.
        write(u''' (''')
        _v = VFFSL(SL,"langname",True) # u'$langname' on line 110, col 55
        if _v is not None: write(_filter(_v, rawExpr=u'$langname')) # from line 110, col 55.
        write(u''').lnk" "$SETTINGS_DIR\\lang\\${LANGID}\\help\\${HELPFILE_NAME}"\r
  CreateShortCut "''')
        write(_filter("$SMPROGRAMS\\AMIS\\"))
        _v = VFFSL(SL,"text122",True) # u'$text122' on line 111, col 45
        if _v is not None: write(_filter(_v, rawExpr=u'$text122')) # from line 111, col 45.
        write(u''' (''')
        _v = VFFSL(SL,"langname",True) # u'$langname' on line 111, col 55
        if _v is not None: write(_filter(_v, rawExpr=u'$langname')) # from line 111, col 55.
        write(u''').lnk" "$SETTINGS_DIR\\lang\\${LANGID}\\shortcuts\\amiskeys.html"\r
SectionEnd\r
\r
;******\r
; write unintaller and registry strings\r
; check if we need to install the msvc runtimes\r
;**\r
Section -Post\r
  WriteUninstaller "$SETTINGS_DIR\\lang\\${LANGID}\\Uninstall-amis-langpack-${LANGNAME}.exe"\r
\r
SectionEnd\r
\r
;******\r
;NSIS init function\r
;**\r
Function .onInit\r
\r
\t!insertmacro MUI_INSTALLOPTIONS_EXTRACT "..\\filebrowse.ini"\r
\r
\t\t;show the splash screen\r
\t\tFile /oname=$PLUGINSDIR\\splash.bmp "''')
        _v = VFFSL(SL,"logo_dir",True) # u'$logo_dir' on line 131, col 40
        if _v is not None: write(_filter(_v, rawExpr=u'$logo_dir')) # from line 131, col 40.
        write(u'''\\amis.bmp"\r
\t\tsplash::show 1000 $PLUGINSDIR\\splash\r
\r
\t\t; possible values for $0:\r
\t\t; \'1\' if the user closed the splash screen early,\r
\t\t; \'0\' if everything closed normally,\r
\t\t; \'-1\' if some error occurred.\r
\t\tPop $0\r
\r
\tSetShellVarContext all\r
\tStrCpy $SETTINGS_DIR $APPDATA\\AMIS\\settings\r
\r
\t; make sure the target directory exists\r
\tIfFileExists "$SETTINGS_DIR" End Error\r
\r
\tError:\r
\t\tIfSilent +2\r
\t\tMessageBox MB_ICONSTOP "''')
        _v = VFFSL(SL,"text201",True) # u'$text201' on line 148, col 27
        if _v is not None: write(_filter(_v, rawExpr=u'$text201')) # from line 148, col 27.
        write(u'''"\r
\t\tAbort\r
\r
\r
 End:\r
FunctionEnd\r
\r
;******\r
; uninstall init\r
;**\r
Function un.onInit\r
  IfSilent End\r
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "''')
        _v = VFFSL(SL,"text202",True) # u'$text202' on line 160, col 54
        if _v is not None: write(_filter(_v, rawExpr=u'$text202')) # from line 160, col 54.
        write(u'''" IDYES +2\r
  Abort\r
End:\r
FunctionEnd\r
\r
;******\r
; uninstall complete\r
;**\r
Function un.onUninstSuccess\r
  HideWindow\r
  IfSilent End\r
  MessageBox MB_ICONINFORMATION|MB_OK "''')
        _v = VFFSL(SL,"text203",True) # u'$text203' on line 171, col 40
        if _v is not None: write(_filter(_v, rawExpr=u'$text203')) # from line 171, col 40.
        write(u'''"\r
End:\r
FunctionEnd\r
\r
\r
;******\r
;uninstall process\r
;**\r
Section Uninstall\r
\r
\t;figure out the user\'s application data directory\r
\t;look for the "all users" context\r
\tSetShellVarContext all\r
\tStrCpy $SETTINGS_DIR $APPDATA\\AMIS\\settings\r
\r
\tDelete "''')
        write(_filter("$SMPROGRAMS\\AMIS\\"))
        _v = VFFSL(SL,"text121",True) # u'$text121' on line 186, col 36
        if _v is not None: write(_filter(_v, rawExpr=u'$text121')) # from line 186, col 36.
        write(u''' (''')
        _v = VFFSL(SL,"langname",True) # u'$langname' on line 186, col 46
        if _v is not None: write(_filter(_v, rawExpr=u'$langname')) # from line 186, col 46.
        write(u''').lnk"\r
  \tDelete "''')
        write(_filter("$SMPROGRAMS\\AMIS\\"))
        _v = VFFSL(SL,"text122",True) # u'$text122' on line 187, col 38
        if _v is not None: write(_filter(_v, rawExpr=u'$text122')) # from line 187, col 38.
        write(u''' (''')
        _v = VFFSL(SL,"langname",True) # u'$langname' on line 187, col 48
        if _v is not None: write(_filter(_v, rawExpr=u'$langname')) # from line 187, col 48.
        write(u''').lnk"\r
\r
\tDelete "$SETTINGS_DIR\\lang\\${LANGID}\\help\\*"\r
\tDelete "$SETTINGS_DIR\\lang\\${LANGID}\\help\\img\\*"\r
\tDelete "$SETTINGS_DIR\\lang\\${LANGID}\\audio\\*"\r
\r
\tRMDir "$SETTINGS_DIR\\lang\\${LANGID}\\help\\img"\r
\tRMDir "$SETTINGS_DIR\\lang\\${LANGID}\\help"\r
\tRMDir "$SETTINGS_DIR\\lang\\${LANGID}\\audio"\r
\r
\tDelete "$SETTINGS_DIR\\lang\\${LANGID}\\shortcuts\\*"\r
\tRMDir "$SETTINGS_DIR\\lang\\${LANGID}\\shortcuts"\r
\r
\tDelete "$SETTINGS_DIR\\lang\\${LANGID}\\*"\r
\tRMDir "$SETTINGS_DIR\\lang\\${LANGID}"\r
\r
  SetAutoClose true\r
SectionEnd\r
''')
        
        ########################################
        ## END - generated method body
        
        return _dummyTrans and trans.response().getvalue() or ""
        
    ##################################################
    ## CHEETAH GENERATED ATTRIBUTES


    _CHEETAH__instanceInitialized = False

    _CHEETAH_version = __CHEETAH_version__

    _CHEETAH_versionTuple = __CHEETAH_versionTuple__

    _CHEETAH_genTime = __CHEETAH_genTime__

    _CHEETAH_genTimestamp = __CHEETAH_genTimestamp__

    _CHEETAH_src = __CHEETAH_src__

    _CHEETAH_srcLastModified = __CHEETAH_srcLastModified__

    _mainCheetahMethod_for_SetupAmis3LangpackNsiTemplate= 'respond'

## END CLASS DEFINITION

if not hasattr(SetupAmis3LangpackNsiTemplate, '_initCheetahAttributes'):
    templateAPIClass = getattr(SetupAmis3LangpackNsiTemplate, '_CHEETAH_templateClass', Template)
    templateAPIClass._addCheetahPlumbingCodeToClass(SetupAmis3LangpackNsiTemplate)


# CHEETAH was developed by Tavis Rudd and Mike Orr
# with code, advice and input from many other volunteers.
# For more information visit http://www.CheetahTemplate.org/

##################################################
## if run from command line:
if __name__ == '__main__':
    from Cheetah.TemplateCmdLineIface import CmdLineIface
    CmdLineIface(templateObj=SetupAmis3LangpackNsiTemplate()).run()


