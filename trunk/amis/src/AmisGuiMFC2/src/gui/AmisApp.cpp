/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2004-2009  DAISY Consortium

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "stdafx.h"
#include <winbase.h>

//#include "util/RegOcx.h"
#include "AmisCore.h"
#include "util/FilePathTools.h"
#include "Preferences.h"
#include "io/BookListFileIO.h"
#include "io/PreferencesFileIO.h"
#include "io/DiscInfoReader.h"
#include "io/DistInfoReader.h"
#include "io/BookmarksFileIO.h"
#include "util/SearchForFilesMFC.h"
#include "util/Log.h"
#include "pdtb.h"
#include "dtb/nav/NavModel.h"
#include "DtbWithHooks.h"

#include "gui/AmisApp.h"
#include "gui/MainFrm.h"
#include "gui/ChildFrm.h"
#include "gui/MainWndParts.h"
#include "gui/MmDoc.h"
#include "gui/MmView.h"
#include "gui/MenuManip.h"
#include "gui/TextRenderBrain.h"
#include "gui/sidebar/AmisSidebar.h"
#include "gui/sidebar/AmisSidebarLoader.h"
#include "gui/dialogs/AmisFileDialog.h"
#include "gui/dialogs/AboutDialog.h"
#include "gui/dialogs/OpenUrlDialog.h"
#include "gui/dialogs/SkipDialog.h"
#include "gui/dialogs/GoToPageDialog.h"
#include "gui/dialogs/SearchForBooksDialog.h"
#include "gui/dialogs/MultipleBooksOnVolumeDialog.h"
#include "gui/dialogs/PreferencesDialog.h"
#include "gui/dialogs/PublicationSummaryDialog.h"
#include "gui/dialogs/TextStyleDialog.h"
#include "gui/dialogs/FindInTextDialog.h"

#include "gui/self-voicing/directshow/dx_audio_player.h"

//using namespace amis::audio;

#include "gui/self-voicing/TTSPlayer.h"
//using namespace amis::tts;

#include "ambulant/gui/dx/dx_audio.h"
//#include "ambulant/gui/dx/dx_audio_player.h"

#include "util/CmdLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#if _DEBUG
//get Visual Leak Detector version 1.0 here: http://dmoulding.googlepages.com/downloads
//#include <vld.h>
#endif

#if defined(AMIS_PLATFORM_WINDOWS)
//for creating directories on windows
#include "io.h"
#include "direct.h"
#endif

using namespace amis::gui;

BEGIN_MESSAGE_MAP(CAmisApp, CWinApp)
	ON_COMMAND(ID_AMIS_OPEN_FILE, OnFileOpen)
	ON_COMMAND(ID_AMIS_PLAYPAUSE, OnPlayPause)
	ON_COMMAND(ID_AMIS_NEXT_PHRASE, OnNavNextPhrase)
	ON_COMMAND(ID_AMIS_PREVIOUS_PHRASE, OnNavPrevPhrase)
	ON_COMMAND(ID_AMIS_EXIT, OnFileExit)
	ON_COMMAND(ID_AMIS_NEXT_SECTION, OnNavNextSection)
	ON_COMMAND(ID_AMIS_PREVIOUS_SECTION, OnNavPreviousSection)
	ON_COMMAND_RANGE(AMIS_RECENT_BOOK_BASE_ID, AMIS_RECENT_BOOK_BASE_ID + AMIS_MAX_ANYTHING, OnFileRecentBook)
	ON_COMMAND_RANGE(AMIS_SECTION_DEPTH_BASE_ID, AMIS_SECTION_DEPTH_BASE_ID + AMIS_MAX_ANYTHING, OnNavShowSectionDepth)
	ON_COMMAND(ID_AMIS_INCREASE_SECTION_DEPTH, OnIncreaseSectionDepth)
	ON_COMMAND(ID_AMIS_DECREASE_SECTION_DEPTH, OnDecreaseSectionDepth)    
	ON_COMMAND(ID_AMIS_NEXT_PAGE, OnNavNextPage)
	ON_COMMAND(ID_AMIS_PREVIOUS_PAGE, OnNavPrevPage)
	ON_COMMAND(ID_AMIS_CLOSE_BOOK, OnFileClose)
	ON_COMMAND(ID_AMIS_ABOUT, OnAbout)
	ON_COMMAND(ID_AMIS_TOGGLE_SIDEBAR, OnViewSidebarOnoff)
	ON_COMMAND(ID_AMIS_ESCAPE, OnPlayEscape)
	ON_COMMAND(ID_AMIS_OPEN_URL, OnFileOpenFromUrl)
	ON_COMMAND(ID_AMIS_SHOW_READING_OPTIONS, OnSkipInfo)
	ON_COMMAND(ID_AMIS_ADD_BOOKMARK, OnAddBookmark)
	ON_COMMAND_RANGE(AMIS_BOOKMARKS_BASE_ID, AMIS_BOOKMARKS_BASE_ID + AMIS_MAX_ANYTHING, OnSelectBookmark)
	ON_COMMAND(ID_AMIS_GOTO_PAGE, OnGoToPage)
	ON_COMMAND(ID_AMIS_BIGGER_FONT, OnIncreaseFontSize)
	ON_COMMAND(ID_AMIS_SMALLER_FONT, OnDecreaseFontSize)
	ON_COMMAND(ID_AMIS_SEARCH_LOCAL_DRIVES, OnSearchForBooks)
	ON_COMMAND(ID_AMIS_LOAD_CD, OnLoadCd)
	ON_COMMAND(ID_AMIS_RESET_SPEED, OnSpeedNormal)
	ON_COMMAND(ID_AMIS_SLOWER, OnSpeedDown)
	ON_COMMAND(ID_AMIS_FASTER, OnSpeedUp)
	ON_COMMAND(ID_AMIS_INCREASE_VOLUME, OnVolumeUp)
	ON_COMMAND(ID_AMIS_DECREASE_VOLUME, OnVolumeDown)
	ON_COMMAND(ID_AMIS_INCREASE_UI_VOLUME, OnVolumeUpUI)
	ON_COMMAND(ID_AMIS_DECREASE_UI_VOLUME, OnVolumeDownUI)
	ON_COMMAND(ID_AMIS_INCREASE_CONTENT_VOLUME, OnVolumeUpBOOK)
	ON_COMMAND(ID_AMIS_DECREASE_CONTENT_VOLUME, OnVolumeDownBOOK)
	ON_COMMAND(ID_AMIS_SHOW_PREFERENCES, OnPreferences)
	ON_COMMAND(ID_AMIS_SHOW_PUBLICATION_SUMMARY, OnPublicationSummary)
	ON_COMMAND(ID_AMIS_TOGGLE_VIEW, OnToggleView)
	ON_COMMAND(ID_AMIS_NO_PAGE_STYLES, OnClearPageStyle)
	ON_COMMAND_RANGE(AMIS_PAGE_STYLE_BASE_ID, AMIS_PAGE_STYLE_BASE_ID + AMIS_MAX_ANYTHING, OnApplyPageStyle)
	ON_COMMAND(ID_AMIS_NEXT_PAGE_STYLE, OnNextPageStyle)
	ON_COMMAND(ID_AMIS_SHOW_TEXTSTYLE, OnShowTextStyle)
	ON_COMMAND_RANGE(AMIS_VIEW_MENU_BASE_ID, AMIS_VIEW_MENU_BASE_ID + AMIS_MAX_ANYTHING, OnChangeTab)
	ON_COMMAND(ID_AMIS_FIND_IN_TEXT, OnShowFindInText)
	ON_COMMAND(ID_AMIS_FIND_NEXT_IN_TEXT, OnFindNextInText)
	ON_COMMAND(ID_AMIS_FIND_PREVIOUS_IN_TEXT, OnFindPreviousInText)
	ON_COMMAND(ID_AMIS_FOCUS_ON_SIDEBAR, OnFocusOnSidebar)
	ON_COMMAND(ID_AMIS_TOGGLE_AUDIO_SELFVOICING_PLAYBACK, OnToggleSelfVoicingAudio)
	ON_COMMAND(ID_AMIS_FOCUS_ON_TEXT, OnFocusOnText)
	ON_COMMAND(ID_AMIS_RESET_HIGHLIGHT_COLORS, OnResetHighlightColors)
	ON_COMMAND(ID_AMIS_TOGGLE_AUDIO_CONTENT_PLAYBACK, OnToggleContentAudio)
	ON_COMMAND(ID_AMIS_SHOW_HELP_CONTENTS, OnShowHelpContents)
	ON_COMMAND(ID_AMIS_SHOW_KEYBOARD_SHORTCUTS, OnShowKeyboardShortcuts)
	ON_COMMAND(ID_AMIS_DECREASE_TTS_VOLUME, OnVolumeDownTTS)
	ON_COMMAND(ID_AMIS_INCREASE_TTS_VOLUME, OnVolumeUpTTS)
END_MESSAGE_MAP()

CAmisApp::CAmisApp()
{
	mpHistory = NULL;
}

//THE APPLICATION!
CAmisApp theApp;
int get_ie_version();

BOOL CAmisApp::InitInstance()
{
	USES_CONVERSION;

	// Ensuring that we're ready for COM in this current thread context.
	// Note: this is legal, we are not in DllMain. We will exit COM at the end of ExitInstance()
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (hr == S_FALSE)
	{
		// Cleanup
		CoUninitialize();
	}

#ifdef _DEBUG
	assert(hr == S_OK);
#endif

	string prefs_file = "";
	// Parse command line for standard shell commands, DDE, file open
	amis::util::CmdLine cmdInfo;
	
	ParseCommandLine(cmdInfo);
	prefs_file = cmdInfo.getPrefsFile();

	//this says "use the registry instead of ini files" (for windows-specific app preferences).  we use it for UAKs 
	SetRegistryKey(_T("Amis"));

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();
	CWinApp::InitInstance();
	mbBookIsOpen = false;
	mbIsWaitingToLoad = false;
	mAppPath = "";
	mbOverrideReopen = false;
	mMultivolumeNavId = "";

	//read the preferences from disk
	initializePathsAndFiles(prefs_file);
	
	//our initial language preference
	mLanguagePreference = Preferences::Instance()->getUiLangId();

	mIeVersion = get_ie_version();

	//load the resource dll
	// one of the first things in the init code
	std::string lang_dll = Preferences::Instance()->getUiLangId() + "\\" + 
		Preferences::Instance()->getCurrentLanguageData()->getDllFileName();
	ambulant::net::url localization_dll = ambulant::net::url::from_filename(lang_dll);
	ambulant::net::url base_dir = *Preferences::Instance()->getLangpacksDir();
	
	localization_dll = localization_dll.join_to_base(base_dir);
	
	HINSTANCE hInst_l10n = LoadLibrary(A2CW(localization_dll.get_file().c_str()));
	if (hInst_l10n != NULL)
		AfxSetResourceHandle(hInst_l10n);
	
	AfxEnableControlContainer();
	
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		  IDR_AMISTYPE,
		  RUNTIME_CLASS(amis::gui::MmDoc),
		  RUNTIME_CLASS(amis::gui::CChildFrame),
		  RUNTIME_CLASS(amis::gui::MmView)
		  );

	
	//this instantiates a new document and loads resources associated with
	//the doc template
	//in our case, this opens an HTML view and loads the AMISTYPE menu
	AddDocTemplate(pDocTemplate);

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMDIFrameWnd* pFrame = new CMainFrame;
	if (!pFrame->LoadFrame(IDR_AMISTYPE)) 
	{
		amis::util::Log::Instance()->writeError("Could not load MFC frame IDR_AMISTYPE", "CMainFrame::InitInstance");
		return FALSE;
	}
	m_pMainWnd = pFrame;
	
	
	// Dispatch commands specified on the command line
	//it turns out that this step is rather important even if you're not really interested in 
	//the command line because mfc processes dynamic view creation somewhere in here.
	//also: 
	//this will stupidly invoke seemingly any available OnOpenDocument handler if it thinks your
	//command parameters want to open a document. 
	//our workaround is the mbShouldIgnoreOpenDoc event flag
	mbShouldIgnoreOpenDocEvent = true;
	bool b_open_from_cmdline = false;
	ambulant::net::url book_to_open;
	mbIsPlayingHelpBook = false;
	mbIsPlayingShortcutsBook = false;
	mbMultivolumeFlag = false;
    //save the value in cmdInfo
	CString cmd_file_name = cmdInfo.m_strFileName;
	
	if (cmd_file_name.GetLength() > 0)
	{
		b_open_from_cmdline = true;
		if (cmd_file_name.Find(_T(":")) != 1)
		{
			//override the cmd info data; otherwise MFC will do something clever with this URL and crash
			cmdInfo.m_strFileName.Empty();
			cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
			book_to_open = amis::util::makeUrlFromString(T2A(cmd_file_name));
		}
		//else it's a local file and there shouldn't be any problems
		else
		{
			book_to_open = ambulant::net::url::from_filename(T2A(cmd_file_name));
		}
		amis::util::Log::Instance()->writeMessage("Command line parameter: ", &book_to_open, "CAmisApp::InitInstance");
	}
	if (!ProcessShellCommand(cmdInfo)) 
	{
		amis::util::Log::Instance()->writeError("Error processing shell command info", "CAmisApp::InitInstance");
		return FALSE;
	}
	mbShouldIgnoreOpenDocEvent = false;
	//done with command line stuff .. phew

	if (!Preferences::Instance()->getMustAvoidTTS())
	{
		amis::tts::TTSPlayer::InstanceOne();
		amis::tts::TTSPlayer::InstanceTwo();

		//set some volumes
		amis::tts::TTSPlayer::InstanceOne()->setVolume(Preferences::Instance()->getTTSVolumePct());
		amis::tts::TTSPlayer::InstanceTwo()->setVolume(Preferences::Instance()->getTTSVolumePct());
	}

	double audio_vol = 100;
	double prefVolumeLevel = (double)Preferences::Instance()->getAudioVolumePct();
	if (prefVolumeLevel >= 0 && prefVolumeLevel <= 100) 
		audio_vol = prefVolumeLevel;

	ambulant::gui::dx::set_global_level(audio_vol/100);
  
	if (!Preferences::Instance()->getMustAvoidDirectX())
	{
		ambulantX::gui::dx::audio_playerX::Instance()->set_volume(audio_vol);
	}

	//then start logging!  
	amis::util::Log::Instance()->startLog(this->getAppSettingsPath() + "amisLog.txt");
#if _DEBUG
	amis::util::Log::Instance()->enable(true);
	amis::util::Log::Instance()->setLevel(amis::util::FULL_LOGGING);
#else
	amis::util::Log::Instance()->enable(Preferences::Instance()->getIsLoggingEnabled());
	amis::util::Log::Instance()->setLevel(Preferences::Instance()->getLogLevel());
#endif
	Preferences::Instance()->logPreferences();

	initializeSelfVoicing();
	
	if (Preferences::Instance()->getIsFirstTime() == true)
	{
		book_to_open = findBookInLangpackSubdir("./help");
		Preferences::Instance()->setIsFirstTime(false);
		amis::io::PreferencesFileIO prefs_io;
		prefs_io.writeToFile(Preferences::Instance()->getSourceUrl()->get_file(), Preferences::Instance());
	}

	mbShouldNotRenderAudio = false;
	
	// try to load shared MDI menus and accelerator table
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_AMISTYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_AMISTYPE));
	
	MainWndParts::Instance()->mpMainFrame->SetIcon(AfxGetApp()->LoadIcon(IDI_AMIS), TRUE);
	amis::gui::MenuManip::Instance()->refreshRecentBooksListMenu();
	amis::gui::MenuManip::Instance()->addPageStyles();
	// The main window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pFrame->UpdateWindow();

	//load user keys into the ambulant plugin engine
	addUserKeysToAmbulantPluginEngine();
	
	if (Preferences::Instance()->getStartInBasicView() == true)
		MainWndParts::Instance()->basicView();

	//if we're not opening from the command line, try loading the last-read book
	if (b_open_from_cmdline == false 
		&& Preferences::Instance()->getLoadLastBook() == true 
		&& mbWasLastExitClean == true)
	{
		if (mpHistory->getLastRead() != NULL)
			book_to_open = mpHistory->getLastRead()->mPath;
	}

	//clear the lastmark of our last read book if we didn't exit cleanly last time
	//otherwise, the next time the book loads, AMIS will probably crash again.
	if (mbWasLastExitClean == false)
	{
		amis::io::BookmarksFileIO bmk_io;
		if (mpHistory->getLastRead() != NULL && mpHistory->getLastRead()->mBmkPath.is_empty_path() == false 
			&& bmk_io.readFromFile(&mpHistory->getLastRead()->mBmkPath))
		{
			amis::dtb::BookmarkSet* p_bmks = bmk_io.getBookmarkSet();
			p_bmks->setLastmark(NULL);
			bmk_io.writeToFile(&mpHistory->getLastRead()->mBmkPath, p_bmks);
			delete p_bmks;
		}
	}

	MainWndParts::Instance()->updateTitleBar(MainWndParts::TITLEBAR_APPNAME, CString(L"AMIS"));
	MainWndParts::Instance()->updateTitleSelfVoicing(amis::Preferences::Instance()->getIsSelfVoicing());
	MainWndParts::Instance()->updateTitleViewMode();
	MainWndParts::Instance()->updateTitleBar(MainWndParts::TITLEBAR_PLAYSTATE, CString(L"-"));

	if (!Preferences::Instance()->getMustAvoidTTS())
		amis::tts::TTSPlayer::InstanceTwo()->setCallback((sendMessageCallbackFn)amis::dtb::DtbWithHooks::ttsTwoDone);

	amis::gui::CAmisApp::emitMessage("ready");
	
	//example of how to test for RTL
	if (Preferences::Instance()->getCurrentLanguageData()->getModuleType() == amis::ModuleDescData::LANGPACK_RTL)
	{
		AfxMessageBox(_T("RTL"));
	}

	//open a book if we decided to either open the command line parameter or last-read book
	if (!book_to_open.is_empty_path()) openBook(&book_to_open);
	
	return TRUE;
}

int CAmisApp::ExitInstance() 
{
	amis::util::Log* p_log = amis::util::Log::Instance();
  
	p_log->writeTrace("Starting to EXIT", "CAmisApp::ExitInstance");
	TRACE(L"%s", "\nStarting to EXIT\n\n");

	if (!Preferences::Instance()->getMustAvoidTTS())
	{
		int tts_vol = amis::tts::TTSPlayer::InstanceOne()->getVolume();
		Preferences::Instance()->setTTSVolumePct(tts_vol);
	}
	
	AudioSequencePlayer::Instance()->Stop();

	// 0-1 range
	double audio_vol = ambulant::gui::dx::change_global_level(1.0);
	
	if (audio_vol < 0) audio_vol = 0;
	if (audio_vol > 1) audio_vol = 1;

	int audio_vol_pct = audio_vol * 100;

	if (!Preferences::Instance()->getMustAvoidDirectX())
	{
		// 0-100 range
		double currentLevel = ambulantX::gui::dx::audio_playerX::Instance()->get_volume();
		#if _DEBUG
			assert(audio_vol_pct == currentLevel);
		#endif
	}

	Preferences::Instance()->setAudioVolumePct(audio_vol_pct);

	Preferences::Instance()->setWasExitClean(true);
	Preferences::Instance()->setUiLangId(mLanguagePreference);

	PreferencesFileIO prefs_io;	
	prefs_io.writeToFile(Preferences::Instance()->getSourceUrl()->get_file(), Preferences::Instance());
	
	amis::util::Log::Instance()->writeMessage("Exiting", "CAmisApp::ExitInstance");
	if (m_hMDIMenu != NULL) FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)FreeResource(m_hMDIAccel);

	if (!Preferences::Instance()->getMustAvoidTTS())
		amis::tts::TTSPlayer::DestroyInstanceTwo();

	amis::dtb::DtbWithHooks::Instance()->DestroyInstance();
	amis::Preferences::Instance()->DestroyInstance();
	amis::gui::sidebar::AmisSidebarLoader::Instance()->DestroyInstance();
	amis::gui::TextRenderBrain::Instance()->DestroyInstance();
	amis::gui::MenuManip::Instance()->DestroyInstance();
	amis::gui::MainWndParts::Instance()->DestroyInstance();
	
	if (!Preferences::Instance()->getMustAvoidTTS() ||
		!Preferences::Instance()->getMustAvoidDirectX())
	{
		AudioSequencePlayer::Instance()->DestroyInstance();
	}

	DataTree::Instance()->DestroyInstance();
	if (mpHistory != NULL) delete mpHistory;

	amis::util::Log::Instance()->writeTrace("AMIS EXIT done.", "CAmisApp::ExitInstance");
	amis::util::Log::Instance()->endLog();
	amis::util::Log::Instance()->DestroyInstance();
	
	
#ifdef _DEBUG

	//This was moved to the uninstaller
	//UnregisterOCX();

#endif

	CoUninitialize();

	TRACE(L"%s", "\nEXIT.\n\n");

	Sleep(1000);

	return CWinApp::ExitInstance();
}

//this returns the application path with a trailing slash
std::string CAmisApp::getAppPath()
{
	USES_CONVERSION;
	if (mAppPath == "")
	{
		TCHAR szBuffer[256];
		GetModuleFileName(NULL, szBuffer, 256);
		CString cstr_app_path = szBuffer;
		int pos = cstr_app_path.ReverseFind('\\');
		if (pos >= 0) cstr_app_path = cstr_app_path.Mid(0, pos + 1);
		mAppPath = W2CA(cstr_app_path);
	}
	return mAppPath;
}
//returned with trailing slash
std::string CAmisApp::getAppSettingsPath()
{
	USES_CONVERSION;
	TCHAR szPath[MAX_PATH];

	HRESULT success = SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath);
	//just for testing
	if (success == S_OK)
	{
		//we will have created appdata\AMIS\settings during installation
		PathAppend(szPath, TEXT("AMIS"));
		PathAppend(szPath, TEXT("settings"));
		std::string app_settings_path = W2CA(szPath);
		app_settings_path.append("\\");
		return app_settings_path;
	}
	else
	{
		TRACE(L"%s", _T("app data directory not found"));
		return "";
	}
}
void CAmisApp::initializePathsAndFiles(string preferencesFile)
{
	//call this twice
	//first time to make sure we want to use plugins 
	//(although the prefer_ffmpeg option will just be the default amis::Preferences value because
	//the amisPrefs.xml file hasn't been read yet (that requires using an Ambulant data source
	//which initializes the plugin engine, so we want to make sure it initializes with at least
	//some of our values))
	initializeAmbulantPreferences();

	string settings_dir = getAppSettingsPath();

	//read the preferences and mark the was-exit-clean flag as false
	amis::io::PreferencesFileIO prefs_io;
	string prefs_path;
	if (preferencesFile == "")
	{
		prefs_path= "amisPrefs.xml";
		//the preferences file is writeable and lives in a common app settings directory
		prefs_path = amis::util::FilePathTools::goRelativePath(settings_dir, prefs_path);
	}
	else
	{
		//just using ambulant url to find out if this path is absolute
		ambulant::net::url prefs_url = ambulant::net::url::from_filename(preferencesFile);
		//the preferences file can be expressed relative to the settings directory, or as an absolute path
		if (prefs_url.is_absolute()) prefs_path = preferencesFile;
		else prefs_path = amis::util::FilePathTools::goRelativePath(settings_dir, preferencesFile);
	}
	bool could_read_prefs = prefs_io.readFromFile(prefs_path);
	if (!could_read_prefs)
	{
		amis::util::Log::Instance()->writeError("Could not read prefs", prefs_path, 
			"AmisApp::initializePathsAndFiles");
	}
	mbWasLastExitClean = Preferences::Instance()->getWasExitClean();
	Preferences::Instance()->setWasExitClean(false);
	prefs_io.writeToFile(prefs_path, Preferences::Instance());
	
	//read the recent books list
	string history_path = "amisHistory.xml";
	//the history file is writeable and lives in a common app settings directory
	history_path = amis::util::FilePathTools::goRelativePath(settings_dir, history_path);
	ambulant::net::url the_path = ambulant::net::url::from_filename(history_path);
	amis::io::BookListFileIO history_io;
	if (history_io.readFromFile(&the_path)) mpHistory = history_io.getBookList();
	else mpHistory = NULL;

	//call this again to set any preferences that were dependent on our own amis preferences file
	initializeAmbulantPreferences();
}

void CAmisApp::initializeAmbulantPreferences()
{
	ambulant::common::preferences *prefs = ambulant::common::preferences::get_preferences();
	prefs->m_welcome_seen = true;
	prefs->m_validation_scheme = "never";
	prefs->m_parser_id = "xerces";
	prefs->m_use_plugins = true;
	prefs->m_plugin_dir = "";
	prefs->m_prefer_ffmpeg = amis::Preferences::Instance()->getAmbulantPrefersFFMpeg();
	prefs->m_dynamic_content_control = true;
}

void CAmisApp::initializeSelfVoicing()
{
	DataReader new_data_reader;
	DataTree* p_new_data_tree;
	p_new_data_tree = DataTree::Instance();

	//a lang module file points to the moduleDescData.xml in the top-level directory of every langpack
	amis::ModuleDescData* lang_data = amis::Preferences::Instance()->getCurrentLanguageData();
	bool b_try_default_langpack_instead = false;
	if (lang_data == NULL)
	{
		b_try_default_langpack_instead = true;
		amis::util::Log::Instance()->writeError("No language pack data found", "AmisApp::initializeSelfVoicing");
	}
	else
	{
		const ambulant::net::url* lang_module_file = amis::Preferences::Instance()->getCurrentLanguageData()->getXmlFileName();
		ambulant::net::url lang_xml_file = ambulant::net::url::from_filename("amisAccessibleUi.xml");
		lang_xml_file = lang_xml_file.join_to_base(*lang_module_file);
		new_data_reader.setAppPath(getAppSettingsPath());
		amis::ErrorCode did_it_work = new_data_reader.readFile(lang_xml_file.get_file(), p_new_data_tree);
		
		if (did_it_work != amis::OK) b_try_default_langpack_instead = true;
	}
	//if the chosen langpack could not be read, then try to load the default language pack 
	//exit if that doesn't work.
	if (b_try_default_langpack_instead == true)
	{
		if (Preferences::Instance()->getUiLangId() != "eng-US")
		{
			amis::util::Log::Instance()->writeWarning("Forcing load of default language pack", "AmisApp::initializeSelfVoicing");
			Preferences::Instance()->setUiLangId("eng-US");
			//save the file .. why not
			PreferencesFileIO prefs_io;	
			prefs_io.writeToFile(Preferences::Instance()->getSourceUrl()->get_file(), Preferences::Instance());
			//try again with the new preferences setting
			initializeSelfVoicing();
		}
		else
		{
			amis::util::Log::Instance()->writeError("Default (eng-US) language pack could not be loaded.  Exiting.", "AmisApp::initializeSelfVoicing");
			this->ExitInstance();
		}
	}
}

amis::BookList* CAmisApp::getHistory()
{
	return mpHistory;
}
bool CAmisApp::isBookOpen()
{
	return mbBookIsOpen;
}
bool CAmisApp::shouldIgnoreOpenDocEvent()
{
	return mbShouldIgnoreOpenDocEvent;
}

//this function is used by all functions here that lead to a book being opened
bool CAmisApp::openBook(const ambulant::net::url* filename, bool saveInHistory)
{
	setIsWaiting(true);
	MainWndParts::Instance()->mpMainFrame->PostMessageW(WM_MY_UPDATE_TOOLBAR_STATE,
		(WPARAM)MainWndParts::Instance()->mpBasicToolbar);
	MainWndParts::Instance()->mpMainFrame->PostMessageW(WM_MY_UPDATE_TOOLBAR_STATE,
		(WPARAM)MainWndParts::Instance()->mpDefaultToolbar);

	
	bool b_a_book_was_open = false;
	//close the open book
	if (mbBookIsOpen == true) 
	{
		b_a_book_was_open = true;
		//if we were reading the help or shortcuts book, be sure to override
		//OnFileClose's behavior of reloading the last-read book
		//because we are actually trying to open another book
		mbOverrideReopen = mbIsPlayingHelpBook || mbIsPlayingShortcutsBook;
		OnFileClose();
	}

	mbOverrideReopen = false;
	//set the callback function to handle book key registration
	amis::dtb::DtbWithHooks::Instance()->setCallbackForPreprocessingBookKey(registerBookKeyFile);

	if (!filename->is_empty_path()) 
	{
	if (amis::dtb::DtbWithHooks::Instance()->open
			(filename, amis::Preferences::Instance()->getUserBmkDir(), saveInHistory))
		{
			//update the status in the title bar
			MainWndParts::Instance()->updateTitleViewMode();
			mbBookIsOpen = true;
			setIsWaiting(false);
			MainWndParts::Instance()->mpMainFrame->PostMessageW(WM_MY_UPDATE_TOOLBAR_STATE, 
				(WPARAM)MainWndParts::Instance()->mpBasicToolbar);
			MainWndParts::Instance()->mpMainFrame->PostMessageW(WM_MY_UPDATE_TOOLBAR_STATE,
				(WPARAM)MainWndParts::Instance()->mpDefaultToolbar);
			
			int max = amis::dtb::DtbWithHooks::Instance()->getNavModel()->getNavMap()->getMaxDepth();
			amis::gui::MenuManip::Instance()->setSectionDepthCheckmark(max + AMIS_SECTION_DEPTH_BASE_ID);

			if (mbMultivolumeFlag && 
				amis::dtb::DtbWithHooks::Instance()->getUid() == mMultivolumeUid)
			{
				mbMultivolumeFlag = false;
				if (!mMultivolumePosition.is_empty_path())
				{
					ambulant::net::url to_load = mMultivolumePosition;
					//need to clear this variable once it's been used
					ambulant::net::url dummy;
					mMultivolumePosition = dummy;
					amis::dtb::DtbWithHooks::Instance()->startReadingMultivolumePosition(&to_load);
					
				}
				else if (mMultivolumeNavId != "")
				{
					amis::dtb::nav::NavNode* p_node = NULL;
					p_node = amis::dtb::DtbWithHooks::Instance()->
						getNavModel()->getNavNode(mMultivolumeNavId);
					mMultivolumeNavId = "";
					amis::dtb::DtbWithHooks::Instance()->startReadingMultivolumePosition(p_node);
				}
				else
				{
					amis::dtb::DtbWithHooks::Instance()->startReading(true);
				}
			}
			else
			{
				amis::dtb::DtbWithHooks::Instance()->startReading(true);
			}
			return true;
		}
		else
		{
			if (b_a_book_was_open)
			{
				CString temp;
				temp.LoadStringW(IDS_ERROR_OPENING_RELOADING);
				if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
				{
					AudioSequencePlayer::playPromptFromStringId("generalBookErrorReloading");
				}
				setIsWaiting(false);
				generalBookErrorMsgBox(temp);
				openLastReadBook();
				return false;
			}
			else
			{
				//update the status in the title bar
				MainWndParts::Instance()->updateTitleBar(MainWndParts::TITLEBAR_BOOKTITLE, CString(L""));
				MainWndParts::Instance()->updateTitleBar(MainWndParts::TITLEBAR_PLAYSTATE, CString(L""));

				setIsWaiting(false);
				CString temp;
				temp.LoadStringW(IDS_ERROR_OPENING);
				if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
				{
					AudioSequencePlayer::playPromptFromStringId("generalBookError");
				}
				generalBookErrorMsgBox(temp);
				MainWndParts::Instance()->mpMainFrame->PostMessageW(WM_MY_UPDATE_TOOLBAR_STATE,
					(WPARAM)MainWndParts::Instance()->mpBasicToolbar);
				MainWndParts::Instance()->mpMainFrame->PostMessageW(WM_MY_UPDATE_TOOLBAR_STATE,
					(WPARAM)MainWndParts::Instance()->mpDefaultToolbar);

				return false;
			}
		}
		return true;
	}
	else //empty file path
	{
		setIsWaiting(false);
		return false;
	}
}
void CAmisApp::openLastReadBook()
{
	//close any open book
	if (mbBookIsOpen) OnFileClose();
	
	if (mpHistory->getLastRead())
		openBook(&mpHistory->getLastRead()->mPath);
}
void CAmisApp::setIsWaiting(bool value)
{
	if (value == true)
	{
		mbIsWaitingToLoad = true;
		BeginWaitCursor();
	}
	else
	{
		mbIsWaitingToLoad = false;
		EndWaitCursor();
	}
}
bool CAmisApp::getIsWaiting()
{
	return mbIsWaitingToLoad;
}
bool CAmisApp::getShouldNotRenderAudio()
{
	return mbShouldNotRenderAudio;
}

/*******************************
* start of the menu handlers
********************************/
void CAmisApp::OnFileOpen() 
{
	amis::util::Log::Instance()->writeMessage("Showing file open dialog", "CAmisApp::OnFileOpen");
	amis::gui::dialogs::AmisFileDialog dlg(amis::gui::dialogs::AmisFileDialog::getFileOpenFilter());

	string filename = dlg.showOpen();
	if (filename == "")
	{
		amis::util::Log::Instance()->writeMessage("Dialog cancelled", "CAmisApp::OnFileOpen");
		return;
	}

	//the user is able to open UAK files through AMIS
	if (isUserKeyFile(filename))
	{
		importUserKeysIntoRegistry(filename);
	}
	else
	{
		ambulant::net::url book_path = ambulant::net::url::from_filename(filename);
		openBook(&book_path);
	}
}

void CAmisApp::OnNavPrevPhrase()
{
	amis::util::Log::Instance()->writeMessage("Previous phrase", "CAmisApp::OnNavPrevPhrase");
	amis::dtb::DtbWithHooks::Instance()->previousPhrase();
}

void CAmisApp::OnNavNextPhrase()
{
	amis::util::Log::Instance()->writeMessage("Next phrase", "CAmisApp::OnNavNextPhrase");
	amis::dtb::DtbWithHooks::Instance()->nextPhrase();
}
void CAmisApp::OnFileExit() 
{
	amis::util::Log::Instance()->writeMessage("Exiting", "CAmisApp::OnFileExit");
	CWnd * ptr = AfxGetMainWnd();
	ptr->SendMessage(WM_CLOSE);
}
void CAmisApp::OnFileRecentBook(UINT id)
{
	if (mpHistory == NULL) return;

	UINT selection = id - AMIS_RECENT_BOOK_BASE_ID;
	string filename = "";

	amis::BookEntry* p_book = NULL;
	p_book = mpHistory->getEntry(selection);
	string log_msg = "Loading recent book " + p_book->mPath.get_url();
	amis::util::Log::Instance()->writeMessage("Loading recent book", &p_book->mPath, "CAmisApp::OnFileRecentBook");
	openBook(&p_book->mPath);
}

void CAmisApp::OnNavNextSection() 
{
	amis::util::Log::Instance()->writeMessage("Next section", "CAmisApp::OnNavNextSection");
	amis::dtb::DtbWithHooks::Instance()->nextSection();
}

void CAmisApp::OnNavPreviousSection()
{
	amis::util::Log::Instance()->writeMessage("Previous section", "CAmisApp::OnNavPreviousSection");
	amis::dtb::DtbWithHooks::Instance()->previousSection();
}
void CAmisApp::OnNavShowSectionDepth(UINT id)
{
	int level = id - AMIS_SECTION_DEPTH_BASE_ID;
	setSectionDepth(level);
}
void CAmisApp::setSectionDepth(int level)
{
	int max = amis::dtb::DtbWithHooks::Instance()->getNavModel()->getNavMap()->getMaxDepth();
	if (level < 1)
		level = 1;
	else if (level > max)
		level = max; 

	char ch[3];
	itoa(level, ch, 10);
	string log_msg = "Setting section depth to ";
	log_msg.append(ch);
	amis::util::Log::Instance()->writeMessage(log_msg, "CAmisApp::setSectionDepth");
	amis::gui::MainWndParts::Instance()->mpSidebar->m_wndDlg.expandSections(level);
	amis::gui::MenuManip::Instance()->setSectionDepthCheckmark(level + AMIS_SECTION_DEPTH_BASE_ID);
}
void CAmisApp::OnIncreaseSectionDepth()
{
    int curr = amis::gui::MainWndParts::Instance()->mpSidebar->m_wndDlg.getExposedDepth();
    int max = amis::dtb::DtbWithHooks::Instance()->getNavModel()->getNavMap()->getMaxDepth();
    curr++;
    if (curr < 1)
        curr = 1;
    else if (curr > max)
        curr = max;
    
    setSectionDepth(curr);
}
void CAmisApp::OnDecreaseSectionDepth()
{
    int curr = amis::gui::MainWndParts::Instance()->mpSidebar->m_wndDlg.getExposedDepth();
    int max = amis::dtb::DtbWithHooks::Instance()->getNavModel()->getNavMap()->getMaxDepth();
    
    curr--;
    if (curr < 1)
        curr = 1;
    else if (curr > max)
        curr = max;
    
    setSectionDepth(curr);
    
}

void CAmisApp::OnNavNextPage()
{
	amis::util::Log::Instance()->writeMessage("Next page", "CAmisApp::OnNavNextPage");
	amis::dtb::DtbWithHooks::Instance()->nextPage();
}

void CAmisApp::OnNavPrevPage()
{
	amis::util::Log::Instance()->writeMessage("Previous page", "CAmisApp::OnNavPrevPage");
	amis::dtb::DtbWithHooks::Instance()->previousPage();
}

void CAmisApp::OnFileClose()
{
	amis::util::Log::Instance()->writeMessage("Closing book", "CAmisApp::OnFileClose");
	if (mbBookIsOpen)
	{
		//stop the book TTS, otherwise it could continue for a while
		if (!Preferences::Instance()->getMustAvoidTTS() &&
			amis::tts::TTSPlayer::InstanceTwo()->IsPlaying())
		{
			amis::tts::TTSPlayer::InstanceTwo()->Stop();
		}

		mbBookIsOpen = false;
		//the order matters here
		amis::dtb::DtbWithHooks::Instance()->DestroyInstance();
		MainWndParts::Instance()->mpMmView->OnDestroy();
		amis::gui::MainWndParts::Instance()->mpSidebar->m_wndDlg.clearAll();
		amis::gui::MenuManip::Instance()->clearBookmarks();
		//there is no longer a last-read book
		mpHistory->clearLastRead();
		amis::io::BookListFileIO io;
		io.writeToFile(mpHistory->getFilepath(), mpHistory);
		
		//update the status in the title bar
		amis::gui::MainWndParts::Instance()->updateTitleBar(amis::gui::MainWndParts::TITLEBAR_BOOKTITLE, _T(""));
		MainWndParts::Instance()->mpMainFrame->PostMessageW(WM_MY_UPDATE_TOOLBAR_STATE,
			(WPARAM)MainWndParts::Instance()->mpBasicToolbar);
		MainWndParts::Instance()->mpMainFrame->PostMessageW(WM_MY_UPDATE_TOOLBAR_STATE,
			(WPARAM)MainWndParts::Instance()->mpDefaultToolbar);		
		amis::gui::CAmisApp::emitMessage("ready");

		if ((mbIsPlayingHelpBook || mbIsPlayingShortcutsBook) && !mbOverrideReopen)
		{
			mbIsPlayingHelpBook = false;
			mbIsPlayingShortcutsBook = false;
			//load the last-read book
			if (mpHistory->getLastRead() != NULL)
				openBook(&mpHistory->getLastRead()->mPath);
		}
		
	}
}

// increment for the 0-1 volume level range
#define DX_VOLUME_LEVEL_STEP 0.1

void CAmisApp::OnVolumeUpBOOK()
{
	//if (isBookOpen() == false) return;

	amis::util::Log::Instance()->writeMessage("Volume increase BOOK", "CAmisApp::OnVolumeUp");
	
	// 0-1 range
	double currentLevel = ambulant::gui::dx::change_global_level(1.0);
	currentLevel += DX_VOLUME_LEVEL_STEP;
	if (currentLevel > 1) currentLevel = 1;
	if (currentLevel <= 0) currentLevel = 0.01;
	ambulant::gui::dx::set_global_level(currentLevel);

	if (!Preferences::Instance()->getMustAvoidTTS())
		amis::tts::TTSPlayer::InstanceTwo()->IncreaseVolume();
}

void CAmisApp::OnVolumeDownBOOK()
{
	//if (isBookOpen() == false) return;
	
	amis::util::Log::Instance()->writeMessage("Volume decrease BOOK", "CAmisApp::OnVolumeDown");
    
	// 0-1 range
	double currentLevel = ambulant::gui::dx::change_global_level(1.0);
	currentLevel -= DX_VOLUME_LEVEL_STEP;
	if (currentLevel > 1) currentLevel = 1;
	if (currentLevel <= 0) currentLevel = 0.01;
	ambulant::gui::dx::set_global_level(currentLevel);

	if (!Preferences::Instance()->getMustAvoidTTS())
		amis::tts::TTSPlayer::InstanceTwo()->DecreaseVolume();
}

void CAmisApp::OnVolumeDownTTS()
{
	amis::util::Log::Instance()->writeMessage("Volume decrease TTS", "CAmisApp::OnVolumeDown");
	if (!Preferences::Instance()->getMustAvoidTTS())
	{
		amis::tts::TTSPlayer::InstanceOne()->DecreaseVolume();
		amis::tts::TTSPlayer::InstanceTwo()->DecreaseVolume();
		
		if (amis::Preferences::Instance()->getIsSelfVoicing() && !Preferences::Instance()->getMustAvoidTTS())
		{	
			AudioSequence* seq	= new AudioSequence();
			std::wstring str = AudioSequencePlayer::getMenuCaption(ID_AMIS_DECREASE_VOLUME);

			std::string volume_level_str;
			char volume_level_char[5] = "";
			int vol = amis::tts::TTSPlayer::InstanceOne()->getVolume();
			itoa(vol, volume_level_char, 10);
			volume_level_str.assign(volume_level_char);
			seq->append(CString(volume_level_str.c_str())+L"%");

			if (str.size() > 0)
				seq->append(str.c_str());

			AudioSequencePlayer::playPromptFromUiId(ID_AMIS_DECREASE_VOLUME, seq, false);

		/*
			std::wstring str = AudioSequencePlayer::getMenuCaption(ID_AMIS_DECREASE_VOLUME);
			if (str.size() > 0)
				amis::tts::TTSPlayer::InstanceOne()->Play(str.c_str());*/
		}
	}
}
void CAmisApp::OnVolumeUpTTS()
{
	amis::util::Log::Instance()->writeMessage("Volume increase TTS", "CAmisApp::OnVolumeUp");
	if (!Preferences::Instance()->getMustAvoidTTS())
	{
		amis::tts::TTSPlayer::InstanceOne()->IncreaseVolume();
		amis::tts::TTSPlayer::InstanceTwo()->IncreaseVolume();

		if (amis::Preferences::Instance()->getIsSelfVoicing() && !Preferences::Instance()->getMustAvoidTTS())
		{	
			AudioSequence* seq	= new AudioSequence();
			std::wstring str = AudioSequencePlayer::getMenuCaption(ID_AMIS_INCREASE_VOLUME);

			std::string volume_level_str;
			char volume_level_char[5] = "";
			int vol = amis::tts::TTSPlayer::InstanceOne()->getVolume();
			itoa(vol, volume_level_char, 10);
			volume_level_str.assign(volume_level_char);
			seq->append(CString(volume_level_str.c_str())+L"%");

			if (str.size() > 0)
				seq->append(str.c_str());

			AudioSequencePlayer::playPromptFromUiId(ID_AMIS_INCREASE_VOLUME, seq, false);

		/*
			std::wstring str = AudioSequencePlayer::getMenuCaption(ID_AMIS_INCREASE_VOLUME);
			if (str.size() > 0)
				amis::tts::TTSPlayer::InstanceOne()->Play(str.c_str());*/
		}
	}
}
void CAmisApp::OnVolumeUpUI()
{
	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		amis::util::Log::Instance()->writeMessage("Volume increase UI", "CAmisApp::OnVolumeUp");

		if (!Preferences::Instance()->getMustAvoidTTS())
			amis::tts::TTSPlayer::InstanceOne()->IncreaseVolume();
		
		if (!Preferences::Instance()->getMustAvoidDirectX())
		{
			// 0-100 range
			double currentLevel = ambulantX::gui::dx::audio_playerX::Instance()->get_volume();
			currentLevel += (DX_VOLUME_LEVEL_STEP * 100);
			if (currentLevel > 100) currentLevel = 100;
			if (currentLevel <= 0) currentLevel = 1;
			ambulantX::gui::dx::audio_playerX::Instance()->set_volume(currentLevel);
		}

		AudioSequence* seq	= new AudioSequence();
		std::wstring str = AudioSequencePlayer::getMenuCaption(ID_AMIS_INCREASE_VOLUME);

		if (!Preferences::Instance()->getMustAvoidTTS())
		{
			std::string volume_level_str;
			char volume_level_char[5] = "";
			int vol = amis::tts::TTSPlayer::InstanceOne()->getVolume();
			itoa(vol, volume_level_char, 10);
			volume_level_str.assign(volume_level_char);
			seq->append(CString(volume_level_str.c_str())+L"%");

			if (str.size() > 0)
				seq->append(str.c_str());

			if (!Preferences::Instance()->getMustAvoidDirectX())
			{
				long volume = ambulantX::gui::dx::audio_playerX::Instance()->get_volume();
				itoa(volume, volume_level_char, 10);
				volume_level_str.assign(volume_level_char);
				seq->append(CString(volume_level_str.c_str())+L"%");
			}
		}

		AudioSequencePlayer::playPromptFromUiId(ID_AMIS_INCREASE_VOLUME, seq, false);
	}
}

void CAmisApp::OnVolumeDownUI()
{
	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		amis::util::Log::Instance()->writeMessage("Volume decrease UI", "CAmisApp::OnVolumeDown");

		if (!Preferences::Instance()->getMustAvoidTTS())
			amis::tts::TTSPlayer::InstanceOne()->DecreaseVolume();

		if (!Preferences::Instance()->getMustAvoidDirectX())
		{
			// 0-100 range
			double currentLevel = ambulantX::gui::dx::audio_playerX::Instance()->get_volume();
			currentLevel -= (DX_VOLUME_LEVEL_STEP * 100);
			if (currentLevel > 100) currentLevel = 100;
			if (currentLevel <= 0) currentLevel = 1;
			ambulantX::gui::dx::audio_playerX::Instance()->set_volume(currentLevel);
		}
	
		AudioSequence* seq	= new AudioSequence();
		std::wstring str = AudioSequencePlayer::getMenuCaption(ID_AMIS_DECREASE_VOLUME);

		if (!Preferences::Instance()->getMustAvoidTTS())
		{
			std::string volume_level_str;
			char volume_level_char[5] = "";
			int vol = amis::tts::TTSPlayer::InstanceOne()->getVolume();
			itoa(vol, volume_level_char, 10);
			volume_level_str.assign(volume_level_char);
			seq->append(CString(volume_level_str.c_str())+L"%");

			if (str.size() > 0)
				seq->append(str.c_str());

			if (!Preferences::Instance()->getMustAvoidDirectX())
			{
				long volume = ambulantX::gui::dx::audio_playerX::Instance()->get_volume();
				itoa(volume, volume_level_char, 10);
				volume_level_str.assign(volume_level_char);
				seq->append(CString(volume_level_str.c_str())+L"%");
			}
		}

		AudioSequencePlayer::playPromptFromUiId(ID_AMIS_DECREASE_VOLUME, seq, false);
	}
}

void CAmisApp::OnVolumeUp()
{
	OnVolumeUpUI();
	OnVolumeUpBOOK();
}

void CAmisApp::OnVolumeDown()
{
	OnVolumeDownUI();
	OnVolumeDownBOOK();
}

void CAmisApp::OnSpeedUp()
{
	amis::util::Log::Instance()->writeMessage("Speed increase", "CAmisApp::OnSpeedUp");

	if (!Preferences::Instance()->getMustAvoidTTS())
	{
		long currentRate = amis::tts::TTSPlayer::InstanceOne()->GetSpeechRate();
		amis::tts::TTSPlayer::InstanceOne()->SetSpeechRate(currentRate+1);
		amis::tts::TTSPlayer::InstanceTwo()->SetSpeechRate(currentRate+1);
	}

	amis::util::Log::Instance()->writeTrace("before ambulant increase rate");
	int rate = ambulant::gui::dx::change_global_rate(1);
	char ch_rate[15];
	sprintf(ch_rate, "Rate = %d", rate);
	amis::util::Log::Instance()->writeTrace(ch_rate);

	if (!Preferences::Instance()->getMustAvoidDirectX())
		ambulantX::gui::dx::audio_playerX::Instance()->set_rate(rate);

	updateSpeedButtons();
}

void CAmisApp::OnSpeedDown()
{
	amis::util::Log::Instance()->writeMessage("Speed decrease", "CAmisApp::OnSpeedDown");

	if (!Preferences::Instance()->getMustAvoidTTS())
	{
		long currentRate = amis::tts::TTSPlayer::InstanceOne()->GetSpeechRate();
		amis::tts::TTSPlayer::InstanceOne()->SetSpeechRate(currentRate-1);
		amis::tts::TTSPlayer::InstanceTwo()->SetSpeechRate(currentRate-1);
	}

	double rate = ambulant::gui::dx::change_global_rate(-1);
	char ch_rate[15];
	sprintf(ch_rate, "Rate = %d", rate);
	amis::util::Log::Instance()->writeTrace(ch_rate);
	
	if (!Preferences::Instance()->getMustAvoidDirectX())
		ambulantX::gui::dx::audio_playerX::Instance()->set_rate(rate);
	updateSpeedButtons();
}

void CAmisApp::OnSpeedNormal()
{
	amis::util::Log::Instance()->writeMessage("Speed reset to normal", "CAmisApp::OnSpeedNormal");
	
	if (!Preferences::Instance()->getMustAvoidTTS())
	{
		amis::tts::TTSPlayer::InstanceOne()->SetSpeechRate(0);
		amis::tts::TTSPlayer::InstanceTwo()->SetSpeechRate(0);
	}

	if (!Preferences::Instance()->getMustAvoidDirectX())
		ambulantX::gui::dx::audio_playerX::Instance()->set_rate(1.0);
	
	ambulant::gui::dx::set_global_rate(1.0);
	updateSpeedButtons();
}


void CAmisApp::OnAbout()
{
	amis::util::Log::Instance()->writeMessage("Showing about box", "CAmisApp::OnAbout");
	amis::gui::dialogs::AboutDialog about;
	about.do_modal();
}

void CAmisApp::OnViewSidebarOnoff() 
{
	amis::util::Log::Instance()->writeMessage("Toggle sidebar", "CAmisApp::OnViewSidebarOnOff");
	MainWndParts::Instance()->toggleSidebar();
}

void CAmisApp::OnPlayEscape()
{
	amis::util::Log::Instance()->writeMessage("Escape", "CAmisApp::OnPlayEscape");
	MmView *view = MainWndParts::Instance()->mpMmView;
	assert(view); 
	view->escapeCurrent();
}

void CAmisApp::OnFileOpenFromUrl()
{
	USES_CONVERSION;

	amis::util::Log::Instance()->writeMessage("Showing open from URL dialog", "CAmisApp::OnFileOpenFromUrl");
	amis::gui::dialogs::OpenUrlDialog dlg;
	string str_url = "";

	if (dlg.do_modal() == IDOK) 
	{
		str_url = T2A(dlg.getUrl());
		ambulant::net::url book_url = ambulant::net::url::from_url(str_url);
		openBook(&book_url);
	}
	else
		amis::util::Log::Instance()->writeMessage("Dialog cancelled", "CAmisApp::OnFileOpenFromUrl");
}

void CAmisApp::OnSkipInfo()
{
	amis::util::Log::Instance()->writeMessage("Showing reading options dialog (skippability)", "CAmisApp::OnSkipInfo");
	amis::gui::dialogs::SkipDialog skip_dialog;
	skip_dialog.initializeData(amis::dtb::DtbWithHooks::Instance()->getCustomTestSet());
	if (skip_dialog.do_modal() == IDOK)
	{
		amis::dtb::DtbWithHooks::Instance()->updateCustomTestStates();
	}
}

void CAmisApp::OnAddBookmark()
{
	amis::util::Log::Instance()->writeMessage("Adding bookmark", "CAmisApp::OnAddBookmark");
	amis::dtb::Bookmark* p_bmk = NULL;
	p_bmk = amis::dtb::DtbWithHooks::Instance()->addBookmark();
	if (p_bmk)
	{
		MenuManip::Instance()->addBookmark(p_bmk);
		pauseBookAndEmitMessage("done");
	}
}

void CAmisApp::OnSelectBookmark(UINT id)
{
	int idx;
	idx = id - AMIS_BOOKMARKS_BASE_ID;
	amis::util::Log::Instance()->writeMessage("Loading bookmark", "CAmisApp::OnSelectBookmark");
	amis::dtb::DtbWithHooks::Instance()->loadBookmark(idx);
}

void CAmisApp::OnGoToPage()
{
	amis::gui::dialogs::GoToPageDialog dialog;
	amis::util::Log::Instance()->writeMessage("Showing go to page dialog", "CAmisApp::OnGoToPage");
	if (dialog.do_modal() == IDOK)
	{
		wstring page_number = dialog.getPageNumber();
		amis::dtb::nav::NavModel* p_nav_model = NULL;
		p_nav_model = amis::dtb::DtbWithHooks::Instance()->getNavModel();
		if (p_nav_model->hasPages() && p_nav_model->getPageList() != NULL)
		{
			amis::dtb::nav::PageTarget* p_page = NULL;
			p_page = p_nav_model->getPageList()->findPage(page_number);
			if (p_page) amis::dtb::DtbWithHooks::Instance()->loadNavNode(p_page);
		}
	}
	else
		amis::util::Log::Instance()->writeMessage("Dialog cancelled", "CAmisApp::OnGoToPage");
}
void CAmisApp::OnIncreaseFontSize()
{
	amis::util::Log::Instance()->writeMessage("Increase font size", "CAmisApp::OnIncreaseFontSize");
	TextRenderBrain::Instance()->increaseFontSize();
	updateFontSizeButtons();
}
void CAmisApp::OnDecreaseFontSize()
{
	amis::util::Log::Instance()->writeMessage("Reset font size", "CAmisApp::OnResetFontSize");
	TextRenderBrain::Instance()->decreaseFontSize();
	updateFontSizeButtons();
}
void CAmisApp::OnSearchForBooks()
{
	amis::util::Log::Instance()->writeMessage("Showing search for books dialog", "CAmisApp::OnSearchForBooks");
	dialogs::SearchForBooksDialog search;
	if (search.do_modal() == IDOK)
	{
		ambulant::net::url book_url = search.getBookToLoad();
		this->openBook(&book_url);
	}
	else
		amis::util::Log::Instance()->writeMessage("Dialog cancelled", "CAmisApp::OnSearchForBooks");
}
void CAmisApp::OnLoadCd()
{
	amis::util::Log::Instance()->writeMessage("Loading from CD", "CAmisApp::OnLoadCd");
	amis::util::SearchForFilesMFC searcher;
	//prepare the search tool
	//first pass: ncc or opf at the root level
	searcher.clearAll();
	searcher.addSearchCriteria("ncc.htm");
	searcher.addSearchCriteria(".opf");
	//sometimes I see these temp files on my drive .. excluding them just to be safe
	searcher.addSearchExclusionCriteria("_ncc.html");
	searcher.setRecursiveSearch(false);
	int files_found = searcher.startSearchOnCdRom();
	const ambulant::net::url* single_book = NULL;
	
	//there were no ncc or opf files at the root, so we need to make a
	//second pass and look for disc info or dist info file at the root level
	if (files_found == 0) 
	{
		searcher.clearAll();
		searcher.addSearchCriteria("discinfo.html");
		searcher.addSearchCriteria("distInfo.dinf");
		searcher.setRecursiveSearch(false);
		files_found = searcher.startSearchOnCdRom();
		
		//there should be at most one discinfo or distinfo file
		if (files_found == 1) 
		{	
			amis::UrlList* p_results = searcher.getSearchResults();
			string filename = (*p_results)[0].get_file();
			amis::BookList* p_list = NULL;
			if (amis::util::FilePathTools::getExtension(filename) == "html")
			{
				amis::io::DiscInfoReader reader;
				reader.readFromFile(&(*p_results)[0]);
				p_list = reader.getBookList();
			}
			else
			{
				amis::io::DistInfoReader reader;
				reader.readFromFile(&(*p_results)[0]);
				p_list = reader.getBookList();
			}
			
			//if, for some reason, the distribution info file had only one listing, open it directly
			if (p_list->getNumberOfEntries() == 1)
			{
				single_book = &p_list->getEntry(0)->mPath;
			}
			//show an error.  corrupt discinfo files can lead to having nothing to display.
			else if (p_list->getNumberOfEntries() == 0)
			{
				string msg = "No entries found on discinfo.html";
				amis::util::Log::Instance()->writeError(msg, "CAmisApp::OnLoadCd()");
				amis::util::Log::Instance()->writeWarning("No DAISY books on CD-ROM", "CAmisApp::OnLoadCd");
				CString temp;
				temp.LoadStringW(IDS_NO_BOOKS_ON_CD);
				if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
				{
					AudioSequencePlayer::playPromptFromStringId("noBooksFoundOnCd");
				}
				generalBookErrorMsgBox(temp);
			}
			//otherwise, show the user a dialog so they can select which book to open
			else
			{
				amis::util::Log::Instance()->writeMessage("Multiple books found on CD-ROM", "CAmisApp::OnLoadCd");
				dialogs::MultipleBooksOnVolumeDialog dlg(NULL, p_list);
				amis::util::Log::Instance()->writeMessage("Showing multiple books on volume dialog", "CAmisApp::OnLoadCd");
				if (dlg.do_modal() == IDOK)
					openBook(&dlg.getBookToLoad());
				else
					amis::util::Log::Instance()->writeMessage("Dialog cancelled", "CAmisApp::OnLoadCd");
			}
		}
		else //files_found == 0
		{
			amis::util::Log::Instance()->writeWarning("No DAISY books on CD-ROM", "CAmisApp::OnLoadCd");
			CString temp;
			temp.LoadStringW(IDS_NO_BOOKS_ON_CD);
			if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
			{
				AudioSequencePlayer::playPromptFromStringId("noBooksFoundOnCd");
			}
			generalBookErrorMsgBox(temp);
		}
	}
	else //one ncc or opf file was found at the root
	{
		amis::UrlList* p_results = searcher.getSearchResults();
		single_book = &((*p_results)[0]);
	}
	
	//open a single book
	if (single_book != NULL)
	{
		amis::util::Log::Instance()->writeMessage("Found one book on CD; opening it", "CAmisApp::OnLoadCd");
		//open the first and only URL found
		openBook(single_book);
	}
}

void CAmisApp::OnPreferences()
{
	dialogs::PreferencesDialog prefs;
	amis::util::Log::Instance()->writeMessage("Showing preferences dialog", "CAmisApp::OnPreferences");
	if (prefs.do_modal() == IDOK)
	{
		if (!Preferences::Instance()->getMustAvoidTTS())
		{
			Preferences::Instance()->setTTSVoiceIndex(prefs.mTTSVoiceIndex);
		}
		//if either is enabled, we can have self-voicing
		if (!Preferences::Instance()->getMustAvoidTTS() || !Preferences::Instance()->getMustAvoidDirectX())
		{
		  Preferences::Instance()->setIsSelfVoicing(prefs.mbIsSelfVoicing);
		}
		Preferences::Instance()->setPauseOnLostFocus(prefs.mbPauseOnLostFocus);
		Preferences::Instance()->setStartInBasicView(prefs.mbStartInBasicView);
		Preferences::Instance()->setLoadLastBook(prefs.mbLoadLastBook);
		Preferences::Instance()->setDisableScreensaver(prefs.mbDisableScreensaver);

		MainWndParts::Instance()->updateTitleSelfVoicing(Preferences::Instance()->getIsSelfVoicing());

		if (prefs.mbHighlightText != Preferences::Instance()->getHighlightText())
		{
			Preferences::Instance()->setHighlightText(prefs.mbHighlightText);
			TextRenderBrain::Instance()->redoPageColors();
		}
		mLanguagePreference = prefs.mUiLanguageSelection;
		if (mLanguagePreference != Preferences::Instance()->getUiLangId())
		{
			if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
			{
				AudioSequencePlayer::playPromptFromStringId("restartForChanges");
			}
			AfxMessageBox(IDS_PLEASE_RESTART);
		}
		amis::io::PreferencesFileIO prefs_io;
		prefs_io.writeToFile(Preferences::Instance()->getSourceUrl()->get_file(), Preferences::Instance());
		Preferences::Instance()->logPreferences(true);
	}
	else 
	{
		amis::util::Log::Instance()->writeMessage("Dialog cancelled", "CAmisApp::OnPreferences");

		// make sure to restore the original voice (which may have been changed in the preference dialog)
		if (!Preferences::Instance()->getMustAvoidTTS())
		{
			amis::tts::TTSPlayer::InstanceOne()->SetVoice(Preferences::Instance()->getTTSVoiceIndex());
			amis::tts::TTSPlayer::InstanceTwo()->SetVoice(Preferences::Instance()->getTTSVoiceIndex());
		}
	}
}
void CAmisApp::OnPublicationSummary()
{
	dialogs::PublicationSummaryDialog summary;
	amis::util::Log::Instance()->writeMessage("Showing publication summary dialog", "CAmisApp::OnPublicationSummary");
	summary.setBook(amis::dtb::DtbWithHooks::Instance());
	summary.do_modal();
}
void CAmisApp::OnToggleView()
{
	MainWndParts::Instance()->toggleViewMode();
}
void CAmisApp::OnClearPageStyle()
{
	TextRenderBrain::Instance()->clearPageStyle();
}
void CAmisApp::OnApplyPageStyle(UINT id)
{
	if (isBookOpen() == false) return;
	int idx;
	idx = id - AMIS_PAGE_STYLE_BASE_ID;
	TextRenderBrain::Instance()->applyPageStyle(idx);
}
void CAmisApp::OnNextPageStyle()
{
	if (isBookOpen() == false) return;
	TextRenderBrain::Instance()->applyNextPageStyle();
}
void CAmisApp::OnShowTextStyle()
{
	amis::gui::dialogs::TextStyleDialog dlg;
	if (dlg.do_modal() == IDOK)
	{	
		if (isBookOpen())
		{
			TextRenderBrain::Instance()->redoPageColors();
			MainWndParts::Instance()->mpSidebar->m_wndDlg.setFontName(Preferences::Instance()->getAppFontName());
		}
	}
}
//the user selected "sections" or "pages" or a nav list id from the view menu
void CAmisApp::OnChangeTab(UINT id)
{
	int idx;
	idx = id - AMIS_VIEW_MENU_BASE_ID;
	MainWndParts::Instance()->mpSidebar->m_wndDlg.selectTab(idx);
}
void CAmisApp::OnShowFindInText()
{
	if (isBookOpen() == false) return;
	amis::gui::dialogs::FindInTextDialog dlg;
	CString last_val = 
		amis::dtb::DtbWithHooks::Instance()->getLastTextSearchString().c_str();
	dlg.forceSearchValue(last_val);
	
	if (dlg.do_modal() == IDOK)
	{
		CString tmp = dlg.getUserSearchString();
		if (tmp.GetLength() == 0) return;

		amis::gui::CAmisApp::emitMessage("searching_for_text");

		int dir = dlg.getSearchDirection();
		wstring search_string = tmp;
		//the address of the search result
		std::string result = amis::dtb::DtbWithHooks::Instance()->searchFullText(search_string, 
				TextRenderBrain::Instance()->getCurrentUrl(), dir);
		if (result.size() > 0)
		{
			if (Preferences::Instance()->getIsSelfVoicing() == true)
			{
				AudioSequencePlayer::Instance()->waitForSequenceEnd();
			}
			amis::gui::CAmisApp::emitMessage("done");
			if (Preferences::Instance()->getIsSelfVoicing() == true)
			{
				AudioSequencePlayer::Instance()->waitForSequenceEnd();
			}
			ambulant::net::url smil_url = amis::util::makeUrlFromString(result);
			amis::dtb::DtbWithHooks::Instance()->loadSmilFromUrl(&smil_url);
		}
		else
		{	
			if (Preferences::Instance()->getIsSelfVoicing() == true)
			{
				AudioSequencePlayer::Instance()->waitForSequenceEnd();
			}

			amis::gui::CAmisApp::emitMessage("not_found");
			if (Preferences::Instance()->getIsSelfVoicing() == true)
			{
				AudioSequencePlayer::Instance()->waitForSequenceEnd();
			}
		}
	}
}
void CAmisApp::OnFindNextInText()
{
	if (isBookOpen() == false) return;
	string result;
	result = amis::dtb::DtbWithHooks::Instance()->searchFullTextNext();
	if (result.size() > 0)
	{		
		ambulant::net::url smil_url = amis::util::makeUrlFromString(result);
		amis::dtb::DtbWithHooks::Instance()->loadSmilFromUrl(&smil_url);
	}
}
void CAmisApp::OnFindPreviousInText()
{
	if (isBookOpen() == false) return;
	string result;
	result = amis::dtb::DtbWithHooks::Instance()->searchFullTextPrevious();
	if (result.size() > 0)
	{
		ambulant::net::url smil_url = amis::util::makeUrlFromString(result);
		amis::dtb::DtbWithHooks::Instance()->loadSmilFromUrl(&smil_url);
	}
}
void CAmisApp::OnFocusOnSidebar()
{
	MainWndParts::Instance()->mpSidebar->m_wndDlg.setFocusToActiveList();
}
void CAmisApp::OnToggleSelfVoicingAudio()
{
	bool val = Preferences::Instance()->getIsSelfVoicing();
	Preferences::Instance()->setIsSelfVoicing(!val);
	MainWndParts::Instance()->updateTitleSelfVoicing(Preferences::Instance()->getIsSelfVoicing());
}
void CAmisApp::OnFocusOnText()
{	
	if (MainWndParts::Instance()->mpHtmlView && MainWndParts::Instance()->mpHtmlView->m_wndBrowser)
	{
		MainWndParts::Instance()->mpHtmlView->m_wndBrowser.SetFocus();
		amis::gui::CAmisApp::pauseBookAndEmitMessage("textWindowHasFocus");
	}
}
void CAmisApp::OnResetHighlightColors()
{
	if (isBookOpen())
	{
		//reset the highlight colors to their predictable yellow default
		amis::Preferences::Instance()->resetColors();
		//make sure text highlight is on
		amis::Preferences::Instance()->setHighlightText(true);
		//refresh the display
		TextRenderBrain::Instance()->redoPageColors();
	}
}
void CAmisApp::OnToggleContentAudio()
{
	if (isBookOpen() == false) return;

	//this isn't "mute" so much as "stop rendering audio nodes"
	mbShouldNotRenderAudio = !mbShouldNotRenderAudio;
	
	//resume paused audio if we are rendering audio now
	if (mbShouldNotRenderAudio == false)
	{
		if (amis::dtb::DtbWithHooks::Instance()->isPlaying() == false) 
			amis::dtb::DtbWithHooks::Instance()->resume();
	}
	//pause audio if we are stopping audio rendering
	else
	{
		if (amis::dtb::DtbWithHooks::Instance()->isPlaying() == true)
			amis::dtb::DtbWithHooks::Instance()->pause();
	}
}
void CAmisApp::OnShowHelpContents()
{
	const ambulant::net::url url = findBookInLangpackSubdir("./help");
	if (url.is_empty_path() == false)
	{
		if (openBook(&url, false) == true)
			mbIsPlayingHelpBook = true;
	}
	else
	{
		CString temp;
		temp.LoadStringW(IDS_ERROR_OPENING);
		if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
		{
			AudioSequencePlayer::playPromptFromStringId("generalBookError");
		}
		generalBookErrorMsgBox(temp);
	}
}
void CAmisApp::OnShowKeyboardShortcuts()
{
	const ambulant::net::url url = findBookInLangpackSubdir("./shortcuts");
	if (url.is_empty_path() == false)
	{
		if (openBook(&url, false) == true)
			mbIsPlayingShortcutsBook = true;
	}
	else
	{
		CString temp;
		temp.LoadStringW(IDS_ERROR_OPENING);
		if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
		{
			AudioSequencePlayer::playPromptFromStringId("generalBookError");
		}
		generalBookErrorMsgBox(temp);
	}
}
/***************************************************
* (MED) I moved these functions out of the menu handler area
*****************************************************/


void CAmisApp::OnPlayPause()
{
	if (MainWndParts::Instance()->mpMainFrame->mbWasPlayingWhenLostFocus) 
		MainWndParts::Instance()->mpMainFrame->mbWasPlayingWhenLostFocus = false;

	if (amis::dtb::DtbWithHooks::Instance()->isPlaying() == true)
	{
		amis::util::Log::Instance()->writeMessage("Pausing", "CAmisApp::OnPlayPause");

		if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
		{
			amis::gui::CAmisApp::emitMessage("paused");
		}

		amis::dtb::DtbWithHooks::Instance()->pause();
	}
	else
	{
		amis::util::Log::Instance()->writeMessage("Playing", "CAmisApp::OnPlayPause");
		amis::dtb::DtbWithHooks::Instance()->resume();
	}
}


std::wstring CAmisApp::pauseBookAndEmitMessage(std::string msgID1, std::string msgID2)
{
	MmView *view = MainWndParts::Instance()->mpMmView;
	bool b_was_playing = amis::Preferences::Instance()->getIsSelfVoicing() == true 
		&& amis::dtb::DtbWithHooks::Instance()->isPlaying();

	if (b_was_playing == true)
	{
		amis::dtb::DtbWithHooks::Instance()->pause();
	}

	std::wstring str = emitMessage(msgID1);

	if (msgID2.length() != 0)
	{
		if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
		{
			AudioSequencePlayer::Instance()->waitForSequenceEnd();
		}
		str = emitMessage(msgID2);
	}

	if (b_was_playing == true)
	{
		AudioSequencePlayer::Instance()->WaitForEndSeqAndRestartBook();
	}
	return str;
}


std::wstring CAmisApp::pauseBookAndEmitMessage(std::string msgID)
{
	MmView *view = MainWndParts::Instance()->mpMmView;
	bool b_was_playing = amis::Preferences::Instance()->getIsSelfVoicing() == true 
		&& amis::dtb::DtbWithHooks::Instance()->isPlaying();

	if (b_was_playing == true)
	{
		amis::dtb::DtbWithHooks::Instance()->pause();
	}

	std::wstring str = emitMessage(msgID);

	if (b_was_playing == true)
	{
		AudioSequencePlayer::Instance()->WaitForEndSeqAndRestartBook();
	}
	return str;
}

std::wstring CAmisApp::emitMessage(std::string msgID, bool repeat)
{
	std::wstring str2 = AudioSequencePlayer::getTextForPromptFromStringId(msgID);

	if (str2.length() > 0)
	{
		MainWndParts::Instance()->setStatusText(str2);
	}

	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		AudioSequencePlayer::playPromptFromStringId(msgID, repeat);
	}
	
	return str2;
}

bool CAmisApp::beforeModalBox() 
{
	
	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		//AudioSequencePlayer::Instance()->Stop();
	}

	MmView *view = MainWndParts::Instance()->mpMmView;
	bool b_was_playing = amis::dtb::DtbWithHooks::Instance()->isPlaying();
	
	if (b_was_playing == true)
	{
		amis::dtb::DtbWithHooks::Instance()->pause();
	}

	return b_was_playing;
}

void CAmisApp::afterModalBox(bool b_was_playing) 
{
	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		//AudioSequencePlayer::Instance()->Stop();
		AudioSequencePlayer::playPromptFromStringId("dialogClosed");

		if (b_was_playing == true)
		{
			AudioSequencePlayer::Instance()->WaitForEndSeqAndRestartBook();
		}

	}
	else
	{
		if (b_was_playing == true)
		{
			//MainWndParts::Instance()->mpMainFrame->PostMessage(WM_COMMAND, ID_AMBULANT_PLAY);

			MmView *view = MainWndParts::Instance()->mpMmView;
			amis::dtb::DtbWithHooks::Instance()->resume();
		}
	}
}

void CAmisApp::generalBookErrorMsgBox(CString str)
{

	bool b = beforeModalBox();
	
	AfxMessageBox(str);
	afterModalBox(b);
}

void CAmisApp::updateFontSizeButtons()
{
	bool can_increase = TextRenderBrain::Instance()->canIncreaseFontSize();
	bool can_decrease = TextRenderBrain::Instance()->canDecreaseFontSize();
	MainWndParts::Instance()->mpDefaultToolbar->enable(ID_AMIS_BIGGER_FONT, can_increase);
	MainWndParts::Instance()->mpBasicToolbar->enable(ID_AMIS_BIGGER_FONT, can_increase);
	MainWndParts::Instance()->mpDefaultToolbar->enable(ID_AMIS_SMALLER_FONT, can_decrease);
	MainWndParts::Instance()->mpBasicToolbar->enable(ID_AMIS_SMALLER_FONT, can_decrease);
}

void CAmisApp::updateSpeedButtons()
{
	bool can_increase = canIncreasePlaybackSpeed();
	bool can_decrease = canDecreasePlaybackSpeed();
	MainWndParts::Instance()->mpDefaultToolbar->enable(ID_AMIS_FASTER, can_increase);
	MainWndParts::Instance()->mpBasicToolbar->enable(ID_AMIS_FASTER, can_increase);
	MainWndParts::Instance()->mpDefaultToolbar->enable(ID_AMIS_SLOWER, can_decrease);
	MainWndParts::Instance()->mpBasicToolbar->enable(ID_AMIS_SLOWER, can_decrease);
}

bool CAmisApp::canIncreasePlaybackSpeed()
{
	if (!Preferences::Instance()->getMustAvoidDirectX())
	{
		double rate = ambulantX::gui::dx::audio_playerX::Instance()->get_rate();
		if (rate < 10) return true;
		else return false;
	}
	else
	{
		return false;
	}
}
bool CAmisApp::canDecreasePlaybackSpeed()
{
	if (!Preferences::Instance()->getMustAvoidDirectX())
	{
		double rate = ambulantX::gui::dx::audio_playerX::Instance()->get_rate();
		if (rate > -5) return true;
		else return false;
	}
	else
	{
		return false;
	}
}

//return the path to a book in the given subdirectory of the language pack directory
//this is for finding the help and shortcuts books
ambulant::net::url CAmisApp::findBookInLangpackSubdir(std::string dir)
{
	amis::util::SearchForFilesMFC searcher;
	//search the langpack/help directory for opf or ncc file
	searcher.clearAll();
	searcher.addSearchCriteria("ncc.htm");
	searcher.addSearchCriteria(".opf");
	//sometimes I see these temp files on my drive .. excluding them just to be safe
	searcher.addSearchExclusionCriteria("_ncc.html");
	searcher.setRecursiveSearch(false);

	ambulant::net::url help_dir = ambulant::net::url::from_filename(dir);
	help_dir = help_dir.join_to_base(*amis::Preferences::Instance()->getCurrentLanguageData()->getXmlFileName());

	int files_found = searcher.startSearch(help_dir.get_file());
	if (files_found > 0)
	{
		amis::UrlList* p_url_list = searcher.getSearchResults();
		return (*p_url_list)[0];
	}
	else
	{
		ambulant::net::url empty;
		return empty;
	}
}

std::string CAmisApp::getPromptIDFromSideBarName(std::string item_id)
{
	if (item_id.compare("prodnote") == 0 ||
		item_id.compare("optional-prodnote") == 0 || item_id.compare("Producer notes") == 0)
	{
		return "prodnotes";
	}
	else if (item_id.compare("pages") == 0 || item_id.compare("Pages") == 0 || item_id.compare("pagenumber") == 0)
	{
		return "pages";
	}
	else if (item_id.compare("sections") == 0 || item_id.compare("Sections") == 0)
	{
		return "sections";
	}
	else if (item_id.compare("groups") == 0 || item_id.compare("Grouped items") == 0)
	{
		return "groups";
	}
	else if (item_id.compare("sidebar") == 0 || item_id.compare("Sidebars") == 0)
	{
		return "sidebars";
	}
	else if (item_id.compare("footnote") == 0 || item_id.compare("Footnotes") == 0 || item_id.compare("noteref") == 0)
	{
		return "noterefs";
	}
	return "";
}

int CAmisApp::getIeVersion()
{
	return mIeVersion;
}
std::string CAmisApp::getVersion()
{
	return "3.1";
}
std::string CAmisApp::getReleaseDate()
{
	return "2009-12-18";
}
//when the next volume for this book is loaded, then load this URL
void CAmisApp::setMultivolumeLoadPoint(std::wstring uid, const ambulant::net::url* url)
{
	mbMultivolumeFlag = true;
	mMultivolumeUid = uid;
	mMultivolumePosition = *url;
}
//when the next volume for this book is loaded, then load this URL
void CAmisApp::setMultivolumeLoadPoint(std::wstring uid, amis::dtb::nav::NavNode* node)
{
	mbMultivolumeFlag = true;
	mMultivolumeUid = uid;
	mMultivolumeNavId = node->getId();
}
int get_ie_version()
{
	USES_CONVERSION;
	HKEY hKey;
	//"ERROR_SUCCESS" means it worked
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Internet Explorer",
		0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		char buf[255] = {0};
		DWORD type = REG_SZ;
		DWORD size = 255;
		if (RegQueryValueEx(hKey, L"Version", 
			NULL, &type, (LPBYTE)buf, &size) == ERROR_SUCCESS)
		{
			//we only want to read until the "."
			string v = "";
			for (int i = 0; i<255; i++)
			{
				if (buf[i])
				{
					if (buf[i] == '.') break;
					v += buf[i];
				}
			}
			
			RegCloseKey(hKey);
			int version = atoi(v.c_str());
			return version;
		}
		else
		{	
			RegCloseKey(hKey);
		}
	}
	return 0;
}