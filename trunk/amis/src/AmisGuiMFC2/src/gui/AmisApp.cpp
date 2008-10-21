/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amis.sf.net

Copyright (C) 2004-2007  DAISY for All Project

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

#include "util/RegOcx.h"
#include "../resource.h"
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
END_MESSAGE_MAP()



CAmisApp::CAmisApp()
{
	mpHistory = NULL;
}

//THE APPLICATION!
CAmisApp theApp;

BOOL CAmisApp::InitInstance()
{
	USES_CONVERSION;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (hr == S_FALSE) CoUninitialize();
	assert(hr == S_OK);

	//this says "use the registry instead of ini files" (for windows-specific app preferences).  we use it for UAKs 
	SetRegistryKey(_T("Amis"));

	RegisterOCX();

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();
	CWinApp::InitInstance();
	mbBookIsOpen = false;
	mbIsWaitingToLoad = false;
	mAppPath = "";

	//first read the preferences
	initializePathsAndFiles();
	//then start logging!  
	amis::util::Log::Instance()->startLog(this->getAppSettingsPath() + "amisLog.txt");
	amis::util::Log::Instance()->enable(Preferences::Instance()->getIsLoggingEnabled());
	amis::util::Log::Instance()->setLevel(Preferences::Instance()->getLogLevel());
	Preferences::Instance()->logAllPreferences();
	initializeSelfVoicing();
	
	//our initial language preference
	mLanguagePreference = Preferences::Instance()->getUiLangId();

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
	
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	//it turns out that this step is rather important even if you're not really interested in 
	//the command line because mfc processes dynamic view creation somewhere in here.
	//also: 
	//this will stupidly invoke seemingly any available OnOpenDocument handler if it thinks your
	//command parameters want to open a document. 
	//our workaround is the mbShouldIgnoreOpenDoc event flag
	mbShouldIgnoreOpenDocEvent = true;
	//save the value in cmdInfo
	CString cmd_file_name = cmdInfo.m_strFileName;
	bool b_open_from_cmdline = false;
	ambulant::net::url book_to_open;
	mbIsPlayingHelpBook = false;
	if (Preferences::Instance()->getIsFirstTime() == true)
	{
		book_to_open = findHelpBook();
		Preferences::Instance()->setIsFirstTime(false);
	}
	if (cmd_file_name.GetLength() > 0)
	{
		b_open_from_cmdline = true;
		if (cmd_file_name.Find(_T(":")) != 1)
		{
			//override the cmd info data; otherwise MFC will do something clever with this URL and crash
			cmdInfo.m_strFileName.Empty();
			cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
			book_to_open = ambulant::net::url::from_url(T2A(cmd_file_name));
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
	//set the callback function to handle book key registration
	amis::dtb::DtbWithHooks::Instance()->setCallbackForPreprocessingBookKey(registerBookKeyFile);

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
		if (mpHistory->getLastRead() != NULL && bmk_io.readFromFile(&mpHistory->getLastRead()->mBmkPath))
		{
			amis::dtb::BookmarkSet* p_bmks = bmk_io.getBookmarkSet();
			p_bmks->setLastmark(NULL);
			bmk_io.writeToFile(&mpHistory->getLastRead()->mBmkPath, p_bmks);
			delete p_bmks;
		}
	}

	amis::gui::CAmisApp::emitMessage("ready");

	//open a book if we decided to either open the command line parameter or last-read book
	if (!book_to_open.is_empty_path()) openBook(&book_to_open);
	
	return TRUE;
}

int CAmisApp::ExitInstance() 
{
	amis::util::Log* p_log = amis::util::Log::Instance();
  
	p_log->writeTrace("Starting to EXIT", "CAmisApp::ExitInstance");
	TRACE("\nStarting to EXIT\n\n");

	Preferences::Instance()->setWasExitClean(true);
	Preferences::Instance()->setUiLangId(mLanguagePreference);

	PreferencesFileIO prefs_io;	
	prefs_io.writeToFile(Preferences::Instance()->getSourceUrl()->get_file(), Preferences::Instance());
	
	amis::util::Log::Instance()->writeMessage("Exiting", "CAmisApp::ExitInstance");
	if (m_hMDIMenu != NULL) FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)FreeResource(m_hMDIAccel);

	amis::dtb::DtbWithHooks::Instance()->DestroyInstance();
	amis::Preferences::Instance()->DestroyInstance();
	amis::gui::sidebar::AmisSidebarLoader::Instance()->DestroyInstance();
	amis::gui::TextRenderBrain::Instance()->DestroyInstance();
	amis::gui::MenuManip::Instance()->DestroyInstance();
	amis::gui::MainWndParts::Instance()->DestroyInstance();
	AudioSequencePlayer::Instance()->DestroyInstance();
	DataTree::Instance()->DestroyInstance();
	if (mpHistory != NULL) delete mpHistory;

	CoUninitialize();

	amis::util::Log::Instance()->writeTrace("AMIS EXIT done.", "CAmisApp::ExitInstance");
	amis::util::Log::Instance()->endLog();
	amis::util::Log::Instance()->DestroyInstance();
	
	UnregisterOCX();

	TRACE("\nEXIT.\n\n");

	Sleep(1000);

	return CWinApp::ExitInstance();
}

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
		TRACE(_T("app data directory not found"));
		return "";
	}
}
void CAmisApp::initializePathsAndFiles()
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
	string prefs_path = "amisPrefs.xml";
	//the preferences file is writeable and lives in a common app settings directory
	prefs_path = amis::util::FilePathTools::goRelativePath(settings_dir, prefs_path);
	prefs_io.readFromFile(prefs_path);
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
	prefs->m_prefer_ffmpeg = amis::Preferences::Instance()->getPreferFFMpeg();
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
const ambulant::net::url& CAmisApp::getBookURL() const
{
	return this->mURL;
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
	MainWndParts::Instance()->mpMainFrame->updateToolbarState
				(MainWndParts::Instance()->mpBasicToolbar);
	MainWndParts::Instance()->mpMainFrame->updateToolbarState
				(MainWndParts::Instance()->mpDefaultToolbar);

	bool b_a_book_was_open = false;
	//close the open book
	if (mbBookIsOpen == true) 
	{
		b_a_book_was_open = true;
		OnFileClose();
	}

	if (!filename->is_empty_path()) 
	{
	if (amis::dtb::DtbWithHooks::Instance()->open
			(filename, amis::Preferences::Instance()->getUserBmkDir(), saveInHistory))
		{
			//update the status in the title bar
			MainWndParts::Instance()->updateTitleBar(MainWndParts::TITLEBAR_APPNAME, CString(L"AMIS"));
			MainWndParts::Instance()->updateTitleViewMode();
			MainWndParts::Instance()->updateTitleSelfVoicing(amis::Preferences::Instance()->getIsSelfVoicing());

			mbBookIsOpen = true;
			
			setIsWaiting(false);

			amis::dtb::DtbWithHooks::Instance()->startReading(true);
			
			MainWndParts::Instance()->mpMainFrame->updateToolbarState
				(MainWndParts::Instance()->mpBasicToolbar);
			MainWndParts::Instance()->mpMainFrame->updateToolbarState
				(MainWndParts::Instance()->mpDefaultToolbar);

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
				setIsWaiting(false);
				CString temp;
				temp.LoadStringW(IDS_ERROR_OPENING);
				if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
				{
					AudioSequencePlayer::playPromptFromStringId("generalBookError");
				}
				generalBookErrorMsgBox(temp);
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
	OnFileClose();
	
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
	string log_msg = "Setting section depth to " + level;
	amis::util::Log::Instance()->writeMessage(log_msg, "CAmisApp::OnNavShowSectionDepth");
	amis::gui::MainWndParts::Instance()->mpSidebar->m_wndDlg.expandSections(level);
	amis::gui::MenuManip::Instance()->setSectionDepthCheckmark(id);
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
		mbBookIsOpen = false;
		//the order matters here
		amis::dtb::DtbWithHooks::Instance()->DestroyInstance();
		MainWndParts::Instance()->mpMmView->OnDestroy();
		amis::gui::MainWndParts::Instance()->mpSidebar->m_wndDlg.clearAll();
		amis::gui::MenuManip::Instance()->clearBookmarks();
		//update the status in the title bar
		amis::gui::MainWndParts::Instance()->updateTitleBar(amis::gui::MainWndParts::TITLEBAR_BOOKTITLE, _T(""));
		
		amis::gui::CAmisApp::emitMessage("ready");

		if (mbIsPlayingHelpBook == true)
		{
			mbIsPlayingHelpBook = false;
			//load the last-read book
			if (mpHistory->getLastRead() != NULL)
				openBook(&mpHistory->getLastRead()->mPath);
		}
	}
}

#define VOLUME_RATIO 1.8

void CAmisApp::OnVolumeUpBOOK()
{
	amis::util::Log::Instance()->writeMessage("Volume increase BOOK", "CAmisApp::OnVolumeUp");
	ambulant::gui::dx::change_global_level(VOLUME_RATIO);
}

void CAmisApp::OnVolumeDownBOOK()
{
	amis::util::Log::Instance()->writeMessage("Volume decrease BOOK", "CAmisApp::OnVolumeDown");
    ambulant::gui::dx::change_global_level(1.0/VOLUME_RATIO);
}

void CAmisApp::OnVolumeUpUI()
{
	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		amis::util::Log::Instance()->writeMessage("Volume increase UI", "CAmisApp::OnVolumeUp");
		ambulantX::gui::dx::audio_playerX::change_global_level(VOLUME_RATIO);

		AudioSequence* seq	= new AudioSequence();
		AudioSequencePlayer::playPromptFromUiId(ID_AMIS_INCREASE_VOLUME, seq, false);
	}
}

void CAmisApp::OnVolumeDownUI()
{
	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		amis::util::Log::Instance()->writeMessage("Volume decrease UI", "CAmisApp::OnVolumeDown");
		ambulantX::gui::dx::audio_playerX::change_global_level(1.0/VOLUME_RATIO);
	
		AudioSequence* seq	= new AudioSequence();
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

	long currentRate = amis::tts::TTSPlayer::Instance()->GetSpeechRate();
	amis::tts::TTSPlayer::Instance()->SetSpeechRate(currentRate+1);

	double rate = ambulant::gui::dx::change_global_rate(0.7);
	ambulantX::gui::dx::audio_playerX::Instance()->set_rate(rate);
	updateSpeedButtons();
  //amis::gui::spoken::AudioSequencePlayer::Instance()->playPromptFromUiId(ID_MENU_PLAY_FASTER, true, NULL);
}

void CAmisApp::OnSpeedDown()
{
	amis::util::Log::Instance()->writeMessage("Speed decrease", "CAmisApp::OnSpeedDown");

	long currentRate = amis::tts::TTSPlayer::Instance()->GetSpeechRate();
	amis::tts::TTSPlayer::Instance()->SetSpeechRate(currentRate-1);

	double rate = ambulant::gui::dx::change_global_rate(-0.7);
	ambulantX::gui::dx::audio_playerX::Instance()->set_rate(rate);
	updateSpeedButtons();
}

void CAmisApp::OnSpeedNormal()
{
	amis::util::Log::Instance()->writeMessage("Speed reset to normal", "CAmisApp::OnSpeedNormal");
	
    amis::tts::TTSPlayer::Instance()->SetSpeechRate(0);

	ambulant::gui::dx::set_global_rate(1.0);
	ambulantX::gui::dx::audio_playerX::Instance()->set_rate(1.0);
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
		Preferences::Instance()->setTTSVoiceIndex(prefs.mTTSVoiceIndex);
		Preferences::Instance()->setIsSelfVoicing(prefs.mbIsSelfVoicing);
		Preferences::Instance()->setPauseOnLostFocus(prefs.mbPauseOnLostFocus);
		Preferences::Instance()->setStartInBasicView(prefs.mbStartInBasicView);
		Preferences::Instance()->setLoadLastBook(prefs.mbLoadLastBook);
		Preferences::Instance()->setDisableScreensaver(prefs.mbDisableScreensaver);
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
		Preferences::Instance()->logUserControllablePreferences();
	}
	else 
	{
		amis::util::Log::Instance()->writeMessage("Dialog cancelled", "CAmisApp::OnPreferences");	
		// make sure to restore the original voice (which may have been changed in the preference dialog)
		amis::tts::TTSPlayer::Instance()->ChangeVoice(Preferences::Instance()->getTTSVoiceIndex());
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
	int idx;
	idx = id - AMIS_PAGE_STYLE_BASE_ID;
	TextRenderBrain::Instance()->applyPageStyle(idx);
}
void CAmisApp::OnNextPageStyle()
{
	TextRenderBrain::Instance()->applyNextPageStyle();
}
void CAmisApp::OnShowTextStyle()
{
	amis::gui::dialogs::TextStyleDialog dlg;
	if (dlg.do_modal() == IDOK)
	{
		TextRenderBrain::Instance()->redoPageColors();
		MainWndParts::Instance()->mpSidebar->m_wndDlg.setFontName(Preferences::Instance()->getSidebarFontName());
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
			ambulant::net::url smil_url = ambulant::net::url::from_url(result);
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
	string result;
	result = amis::dtb::DtbWithHooks::Instance()->searchFullTextNext();
	if (result.size() > 0)
	{		
		ambulant::net::url smil_url = ambulant::net::url::from_url(result);
		amis::dtb::DtbWithHooks::Instance()->loadSmilFromUrl(&smil_url);
	}
}
void CAmisApp::OnFindPreviousInText()
{
	string result;
	result = amis::dtb::DtbWithHooks::Instance()->searchFullTextPrevious();
	if (result.size() > 0)
	{
		ambulant::net::url smil_url = ambulant::net::url::from_url(result);
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
	//this isn't "mute" so much as "stop rendering audio nodes"
}
void CAmisApp::OnShowHelpContents()
{
	const ambulant::net::url url = findHelpBook();
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
/***************************************************
* (MED) I moved these functions out of the menu handler area
*****************************************************/

/**
 * 'pauseState' function parameter:
 * - FALSE => state is set to "PLAYING", the "PAUSE" action button is therefore shown
 * - TRUE  => state is set to "PAUSED", the "PLAY" action button is therefore shown
 */
void CAmisApp::setPauseState(bool pauseState)
{
	std::wstring str2 = AudioSequencePlayer::getTextForPromptFromStringId((pauseState ? "paused" : "playing"));

	if (str2.length() > 0)
		MainWndParts::Instance()->mpMmView->SetStatusLine(str2.c_str());

	amis::gui::MenuManip::Instance()->setPauseState(pauseState);
	MainWndParts::Instance()->updateTitlePlayState(!pauseState);
	
	MainWndParts::Instance()->mpDefaultToolbar->togglePlayPause(pauseState);
	MainWndParts::Instance()->mpBasicToolbar->togglePlayPause(pauseState);
}

void CAmisApp::OnPlayPause()
{
	if (MainWndParts::Instance()->mpMainFrame->mbWasPlayingWhenLostFocus) 
		MainWndParts::Instance()->mpMainFrame->mbWasPlayingWhenLostFocus = false;

	MmView *view = MainWndParts::Instance()->mpMmView;
	assert(view); // XXXJack: or what to do if view == NULL? Skip?
	if (view->isPlaying() == true)
	{
		amis::util::Log::Instance()->writeMessage("Pausing", "CAmisApp::OnPlayPause");

		if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
		{
			//AudioSequence* seq	= new AudioSequence();
			//AudioSequencePlayer::playPromptFromUiId(ID_AMIS_PLAYPAUSE, seq, false);
			//
			//AudioSequencePlayer::playPromptFromStringId("paused");
			//
			amis::gui::CAmisApp::emitMessage("paused");

		}

		view->OnFilePause(); // this calls CAmisApp::setPauseState(), see above
	}
	else
	{
		amis::util::Log::Instance()->writeMessage("Playing", "CAmisApp::OnPlayPause");

		view->OnFilePlay(); // this calls CAmisApp::setPauseState(), see above
	}
}

std::wstring CAmisApp::pauseBookAndEmitMessage(std::string msgID)
{
	MmView *view = MainWndParts::Instance()->mpMmView;
	bool b_was_playing = amis::Preferences::Instance()->getIsSelfVoicing() == true && view->isPlaying();

	if (b_was_playing == true)
	{
		view->OnFilePause();
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
		MainWndParts::Instance()->mpMmView->SetStatusLine(str2.c_str());
	else
		int debug = 1;

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
	bool b_was_playing = view->isPlaying();
	
	if (b_was_playing == true)
	{
		view->OnFilePause();
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
			view->OnFilePlay();
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
	double rate = ambulantX::gui::dx::audio_playerX::Instance()->get_rate();
	if (rate < AMIS_MAX_AUDIO_RATE) return true;
	else return false;

}
bool CAmisApp::canDecreasePlaybackSpeed()
{
	double rate = ambulantX::gui::dx::audio_playerX::Instance()->get_rate();
	if (rate <= 1) return false;
	else return true;
}

ambulant::net::url CAmisApp::findHelpBook()
{
	amis::util::SearchForFilesMFC searcher;
	//search the langpack/help directory for opf or ncc file
	searcher.clearAll();
	searcher.addSearchCriteria("ncc.htm");
	searcher.addSearchCriteria(".opf");
	//sometimes I see these temp files on my drive .. excluding them just to be safe
	searcher.addSearchExclusionCriteria("_ncc.html");
	searcher.setRecursiveSearch(false);

	ambulant::net::url help_dir = ambulant::net::url::from_filename("./help");
	help_dir = help_dir.join_to_base(*amis::Preferences::Instance()->getCurrentLanguageData()->getXmlFileName());

	int files_found = searcher.startSearch(help_dir.get_file());
	if (files_found > 0)
	{
		amis::UrlList* p_url_list = searcher.getSearchResults();
		return (*p_url_list)[0];
	}
	else
	{
		return ambulant::net::url::from_filename("");
	}
}

