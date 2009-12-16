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
#include "gui/AmisApp.h"

#include "gui/MainFrm.h"
#include "gui/MainWndParts.h"
#include "Preferences.h"
#include "DtbWithHooks.h"
#include "gui/MenuManip.h"
#include "gui/TextRenderBrain.h"

#include "gui/self-voicing/directshow/dx_audio_player.h"

#include "io/BookmarksFileIO.h"
#include "io/ToolbarIO.h"
#include "util/Log.h"

#include "gui/self-voicing/dialogs/MenuVoicing.h"

#ifdef _DEBUG
#define	new	DEBUG_NEW
#undef THIS_FILE
static char	THIS_FILE[]	= __FILE__;
#endif

using namespace amis::gui;


IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	//ON_MESSAGE(WM_MESSAGETOAMIS, OnMessageFromPlugin)
	ON_WM_INITMENUPOPUP()
	ON_WM_MENUSELECT()
	//ON_WM_MENUCHAR()
	ON_WM_ENTERMENULOOP()
	//ON_MESSAGE(WM_MENUSELECT,	OnMenuSelect)
	ON_WM_INITMENU()
	ON_WM_ACTIVATE()
	ON_WM_ENTERMENULOOP()
	ON_WM_EXITMENULOOP()
	//ON_WM_KEYDOWN()
	ON_WM_SYSCOMMAND()
	ON_WM_SETCURSOR()
	ON_UPDATE_COMMAND_UI(ID_AMIS_NEXT_PAGE, OnUpdateCmdUiNextPage)
	ON_UPDATE_COMMAND_UI(ID_AMIS_PREVIOUS_PAGE, OnUpdateCmdUiPreviousPage)
	ON_UPDATE_COMMAND_UI(ID_AMIS_GOTO_PAGE, OnUpdateCmdUiGoToPage)
	ON_UPDATE_COMMAND_UI(ID_AMIS_ADD_BOOKMARK, OnUpdateCmdUiBookmarks)
	ON_UPDATE_COMMAND_UI(ID_AMIS_FASTER, OnUpdateCmdUiPlayFaster)
	ON_UPDATE_COMMAND_UI(ID_AMIS_CLOSE_BOOK, OnUpdateCmdUiAvailableWhenBookIsOpen)
	ON_UPDATE_COMMAND_UI(ID_AMIS_SHOW_READING_OPTIONS, OnUpdateCmdUiInfoSkip)
	ON_UPDATE_COMMAND_UI(ID_AMIS_SLOWER, OnUpdateCmdUiPlaySlower)
	ON_UPDATE_COMMAND_UI(ID_AMIS_FOCUS_ON_TEXT, OnUpdateCmdUiAvailableWhenBookIsOpen)
	ON_UPDATE_COMMAND_UI(ID_AMIS_TOGGLE_AUDIO_CONTENT_PLAYBACK, OnUpdateCmdUiAvailableWhenBookIsOpen)
	ON_UPDATE_COMMAND_UI(ID_AMIS_NO_PAGE_STYLES, OnUpdateCmdUiPageStyle)
	ON_UPDATE_COMMAND_UI(ID_AMIS_PLAYPAUSE, OnUpdateCmdUiPlayPause)
	ON_UPDATE_COMMAND_UI(ID_AMIS_ESCAPE, OnUpdateCmdUiAvailableWhenBookIsOpen)
	ON_UPDATE_COMMAND_UI(ID_AMIS_RESET_SPEED, OnUpdateCmdUiAvailableWhenBookIsOpen)
	ON_UPDATE_COMMAND_UI(ID_AMIS_PREVIOUS_SECTION, OnUpdateCmdUiPreviousSection)
	ON_UPDATE_COMMAND_UI(ID_AMIS_NEXT_SECTION, OnUpdateCmdUiNextSection)
	ON_UPDATE_COMMAND_UI(ID_AMIS_PREVIOUS_PHRASE, OnUpdateCmdUiPreviousPhrase)
	ON_UPDATE_COMMAND_UI(ID_AMIS_NEXT_PHRASE, OnUpdateCmdUiNextPhrase)
	ON_UPDATE_COMMAND_UI(ID_AMIS_FIND_IN_TEXT, OnUpdateCmdUiFindInText)
	ON_UPDATE_COMMAND_UI(ID_AMIS_SHOW_PUBLICATION_SUMMARY, OnUpdateCmdUiAvailableWhenBookIsOpen)
	ON_UPDATE_COMMAND_UI(ID_AMIS_NEXT_PAGE_STYLE, OnUpdateCmdUiPageStyle)
	ON_UPDATE_COMMAND_UI(ID_AMIS_TOGGLE_SIDEBAR, OnUpdateCmdUiCheckSidebar)
	ON_UPDATE_COMMAND_UI_RANGE(AMIS_PAGE_STYLE_BASE_ID, AMIS_PAGE_STYLE_BASE_ID + AMIS_MAX_ANYTHING, OnUpdateCmdUiPageStyle)
	ON_UPDATE_COMMAND_UI(ID_AMIS_SMALLER_FONT, OnUpdateCmdUiSmallerFont)
	ON_UPDATE_COMMAND_UI(ID_AMIS_BIGGER_FONT, OnUpdateCmdUiBiggerFont)
	ON_UPDATE_COMMAND_UI(ID_AMIS_INCREASE_CONTENT_VOLUME, OnUpdateCmdUiAvailableWhenBookIsOpen)
	ON_UPDATE_COMMAND_UI(ID_AMIS_DECREASE_CONTENT_VOLUME, OnUpdateCmdUiAvailableWhenBookIsOpen)
	ON_UPDATE_COMMAND_UI_RANGE(AMIS_BOOKMARKS_BASE_ID, AMIS_BOOKMARKS_BASE_ID + AMIS_MAX_ANYTHING, OnUpdateCmdUiAvailableWhenBookIsOpen)
	ON_UPDATE_COMMAND_UI(ID_AMIS_OPEN_FILE, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_UPDATE_COMMAND_UI(ID_AMIS_EXIT, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_UPDATE_COMMAND_UI_RANGE(AMIS_RECENT_BOOK_BASE_ID, AMIS_RECENT_BOOK_BASE_ID + AMIS_MAX_ANYTHING, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_UPDATE_COMMAND_UI_RANGE(AMIS_SECTION_DEPTH_BASE_ID, AMIS_SECTION_DEPTH_BASE_ID + AMIS_MAX_ANYTHING, OnUpdateCmdUiAvailableWhenBookIsOpen)
	ON_UPDATE_COMMAND_UI(ID_AMIS_ABOUT, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_UPDATE_COMMAND_UI(ID_AMIS_OPEN_URL, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_UPDATE_COMMAND_UI(ID_AMIS_SEARCH_LOCAL_DRIVES, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_UPDATE_COMMAND_UI(ID_AMIS_LOAD_CD, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_UPDATE_COMMAND_UI(ID_AMIS_INCREASE_VOLUME, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_UPDATE_COMMAND_UI(ID_AMIS_DECREASE_VOLUME, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_UPDATE_COMMAND_UI(ID_AMIS_SHOW_PREFERENCES, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_UPDATE_COMMAND_UI(ID_AMIS_TOGGLE_VIEW, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_UPDATE_COMMAND_UI(ID_AMIS_SHOW_TEXTSTYLE, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_UPDATE_COMMAND_UI_RANGE(AMIS_VIEW_MENU_BASE_ID, AMIS_VIEW_MENU_BASE_ID + AMIS_MAX_ANYTHING, OnUpdateCmdUiAvailableWhenBookIsOpen)
	ON_UPDATE_COMMAND_UI(ID_AMIS_SHOW_HELP_CONTENTS, OnUpdateCmdUiAlmostAlwaysAvailable)

	
	//commands that we don't use in the menus, but that might be available on a toolbar
	ON_UPDATE_COMMAND_UI(ID_AMIS_INCREASE_SECTION_DEPTH, OnUpdateCmdUiAvailableWhenBookIsOpen)
	ON_UPDATE_COMMAND_UI(ID_AMIS_DECREASE_SECTION_DEPTH, OnUpdateCmdUiAvailableWhenBookIsOpen)
	ON_UPDATE_COMMAND_UI(ID_AMIS_FIND_NEXT_IN_TEXT, OnUpdateCmdUiFindInText)
	ON_UPDATE_COMMAND_UI(ID_AMIS_FIND_PREVIOUS_IN_TEXT, OnUpdateCmdUiFindInText)
	ON_UPDATE_COMMAND_UI(ID_AMIS_INCREASE_UI_VOLUME, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_UPDATE_COMMAND_UI(ID_AMIS_DECREASE_UI_VOLUME, OnUpdateCmdUiAlmostAlwaysAvailable)	
	ON_UPDATE_COMMAND_UI(ID_AMIS_FOCUS_ON_SIDEBAR, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_UPDATE_COMMAND_UI(ID_AMIS_TOGGLE_AUDIO_SELFVOICING_PLAYBACK, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_UPDATE_COMMAND_UI(ID_AMIS_RESET_HIGHLIGHT_COLORS, OnUpdateCmdUiAlmostAlwaysAvailable)
	ON_MESSAGE(WM_MY_UPDATE_TOOLBAR_STATE, OnUpdateToolbarState)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_MESSAGE(WM_MY_SET_PAUSE_STATE, OnSetPauseState)
	ON_MESSAGE(WM_MY_LOAD_NAV_NODE, OnLoadNavNode)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,		// status line indicator
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

using namespace amis::gui::spoken;

amis::gui::MenuVoicing* mMenuVoicing;

CMainFrame::CMainFrame()
{	
	mbWasPlayingWhenLostFocus =	false;	
	mSavedMenu.LoadMenu(IDR_AMISTYPE);
	mCommonPreTranslateMessageHandler =	new	PreTranslateMessageHandler(-1);
	mMenuVoicing =	new	amis::gui::MenuVoicing(this);
}
CMainFrame::~CMainFrame()
{
	delete mCommonPreTranslateMessageHandler;
	mCommonPreTranslateMessageHandler = NULL;

	delete mMenuVoicing;
	mMenuVoicing = NULL;
}

void CMainFrame::OnClose() 
{
	// That is a hack to remove a weird bug (crash after ExitInstance() when a self-voicing dialog was previously open)
	if (!Preferences::Instance()->getMustAvoidTTS() &&
		!Preferences::Instance()->getMustAvoidDirectX())
	{
		AudioSequencePlayer::Instance()->Stop();
	}
	CMDIFrameWnd::OnClose();
}
BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mCommonPreTranslateMessageHandler->handleCommand(wParam);
	}
	
	return CMDIFrameWnd::OnCommand(wParam, lParam);
}
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1) return -1;
	
	CString temp;
	temp.LoadStringW(IDS_NAVIGATION);
	if (!mSidebar.Create(temp, this, CSize(0, 0),
		TRUE /*bHasGripper*/, AFX_IDW_CONTROLBAR_FIRST + 32))
	{
		amis::util::Log::Instance()->writeError("Failed to create sidebar", "CMainFrame::OnCreate");
		return -1;
	}
	mSidebar.SetBarStyle(mSidebar.GetBarStyle()	| CBRS_TOOLTIPS |	CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	mSidebar.EnableDocking(CBRS_ALIGN_ANY);

	//create the status bar
	if (!mStatusBar.Create(this) 
		|| !mStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		amis::util::Log::Instance()->writeError("Failed to create status bar", "CMainFrame::OnCreate");
		return -1;
	}

	//Toolbar data
	CRect rect;
	GetWindowRect(rect);
	amis::io::ToolbarIO toolbar_reader;
	ambulant::net::url toolbar_file;
	amis::gui::toolbar::ToolbarSettings* p_settings = NULL;
	DWORD align;
	//create the default toolbar
	amis::util::Log::Instance()->writeMessage("Loading default toolbar", "CMainFrame::OnCreate");
	toolbar_file= ambulant::net::url::from_filename("defaultToolbar.xml");
	toolbar_file = toolbar_file.join_to_base(ambulant::net::url::from_filename(theApp.getAppSettingsPath()));
	toolbar_reader.readFromFile(toolbar_file.get_file());
	p_settings = toolbar_reader.getToolbarSettings();
	mDefaultToolbar.setSettings(p_settings);
	if (!mDefaultToolbar.Create(WS_CHILD | WS_VISIBLE |	TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, 
		rect, this,	mSidebar.GetDlgCtrlID()	+ 1))
	{
		amis::util::Log::Instance()->writeError("Failed to create default toolbar", "CMainFrame::OnCreate");
		return -1;
	}
	mDefaultToolbar.EnableToolTips(TRUE);
	updateToolbarState(&mDefaultToolbar);
	
	//create the default rebar (toolbar container)
	if (p_settings->getPlacement() == amis::gui::toolbar::TOP) align = CBRS_TOP;
	else align = CBRS_BOTTOM;
	if (mDefaultRebar.Create(this, 0, WS_CHILD | WS_VISIBLE	| WS_CLIPSIBLINGS |
		WS_CLIPCHILDREN	| align, AFX_IDW_REBAR) == FALSE)
	{
		amis::util::Log::Instance()->writeError("Failed to create default rebar", "CMainFrame::OnCreate");
		return -1;
	}

	mDefaultRebar.AddBar(&mDefaultToolbar);	

	//create the basic toolbar
	amis::util::Log::Instance()->writeMessage("Loading basic toolbar", "CMainFrame::OnCreate");
	toolbar_file = ambulant::net::url::from_filename("basicToolbar.xml");
	toolbar_file = toolbar_file.join_to_base(ambulant::net::url::from_filename(theApp.getAppSettingsPath()));
	toolbar_reader.readFromFile(toolbar_file.get_file());
	p_settings = toolbar_reader.getToolbarSettings();
	mBasicToolbar.setSettings(p_settings);
	if (!mBasicToolbar.Create(WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, 
		rect, this,	mSidebar.GetDlgCtrlID()	+ 2))
	{
		amis::util::Log::Instance()->writeError("Failed to create basic toolbar", "CMainFrame::OnCreate");
		return -1;
	}
	mBasicToolbar.EnableToolTips(TRUE);
	updateToolbarState(&mBasicToolbar);

	//create the basic rebar (toolbar container)
	if (p_settings->getPlacement() == amis::gui::toolbar::TOP) align = CBRS_TOP;
	else align = CBRS_BOTTOM;
	if (mBasicRebar.Create(this, RBS_BANDBORDERS, WS_CHILD | WS_VISIBLE	| WS_CLIPSIBLINGS |
		WS_CLIPCHILDREN	| align, AFX_IDW_REBAR) == FALSE)
	{
		amis::util::Log::Instance()->writeError("Failed to create basic rebar", "CMainFrame::OnCreate");
		return -1;
	}
	mBasicRebar.AddBar(&mBasicToolbar);
	mBasicRebar.ShowWindow(SW_HIDE);
	
	// Delete these three lines if you don't want the toolbar to be dockable
	EnableDocking(CBRS_ALIGN_ANY);
#ifdef _SCB_REPLACE_MINIFRAME
	m_pFloatingFrameClass =	RUNTIME_CLASS(CSCBMiniDockFrameWnd);
#endif

	DockControlBar(&mSidebar, AFX_IDW_DOCKBAR_LEFT);

	MainWndParts::Instance()->mpSidebar	= &mSidebar;
	MainWndParts::Instance()->mpStatusBar =	&mStatusBar;
	MainWndParts::Instance()->mpMainFrame =	this;
	MainWndParts::Instance()->mpDefaultToolbar = &mDefaultToolbar;
	MainWndParts::Instance()->mpBasicToolbar = &mBasicToolbar;
	MainWndParts::Instance()->mpBasicRebar = &mBasicRebar;
	MainWndParts::Instance()->mpDefaultRebar = &mDefaultRebar;
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CMDIFrameWnd::PreCreateWindow(cs)) return FALSE;

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE | WS_THICKFRAME 
		| WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;
	return TRUE;
}
//reload the original menu
void CMainFrame::restoreMenu()
{
	this->SetMenu(&mSavedMenu);
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}
void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}
#endif

BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (theApp.getIsWaiting() == true)
	{
		RestoreWaitCursor();
		return TRUE;
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CMainFrame::OnMenuSelect( UINT	nItemID, UINT nFlags, HMENU	hSysMenu )
{	
	amis::util::Log* p_log = amis::util::Log::Instance();
  
	p_log->writeTrace("CMainFrame::OnMenuSelect()");
	TRACE("\nCMainFrame::OnMenuSelect()\n");
	CMDIFrameWnd::OnMenuSelect(nItemID, nFlags, hSysMenu);

	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mMenuVoicing->OnMenuSelect(nItemID, nFlags, hSysMenu);
	}
}

void CMainFrame::OnInitMenuPopup(CMenu*	pPopupMenu,	UINT nIndex, BOOL bSysMenu)	
{
	amis::util::Log* p_log = amis::util::Log::Instance();

	p_log->writeTrace("CMainFrame::OnInitMenuPopup()");
	TRACE("\nCMainFrame::OnInitMenuPopup()\n");

	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mMenuVoicing->OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	}

	CMDIFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
}

void CMainFrame::OnInitMenu(CMenu* pMenu)
{
	amis::util::Log* p_log = amis::util::Log::Instance();
	p_log->writeTrace("CMainFrame::OnInitMenu()");
	TRACE("\nCMainFrame::OnInitMenu()\n");
	CMDIFrameWnd::OnInitMenu(pMenu);
}


void CMainFrame::OnEnterMenuLoop(BOOL bIsTrackPopupMenu)
{
	CMDIFrameWnd::OnEnterMenuLoop(bIsTrackPopupMenu);

	bool b_BookIsPlaying = amis::dtb::DtbWithHooks::Instance()->isPlaying();
	mbWasPlayingWhenLostFocus =	b_BookIsPlaying;
	if (mbWasPlayingWhenLostFocus)
		amis::dtb::DtbWithHooks::Instance()->pause();
}
void CMainFrame::OnExitMenuLoop(BOOL bIsTrackPopupMenu)
{
	CMDIFrameWnd::OnExitMenuLoop(bIsTrackPopupMenu);

	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mMenuVoicing->OnExitMenuLoop(bIsTrackPopupMenu);
	}

	if (mbWasPlayingWhenLostFocus)
	{
		amis::dtb::DtbWithHooks::Instance()->resume();
	}
	mbWasPlayingWhenLostFocus =	false;

}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (amis::Preferences::Instance()->getIsSelfVoicing() == true &&
		(pMsg->message == WM_KEYDOWN	|| pMsg->message ==	WM_KEYUP))
	{

		if (pMsg->message == WM_KEYUP)
		{
			if (pMsg->wParam == VK_ESCAPE) {
				mMenuVoicing->resetLastOpenPopupMenu();
			}
		}

		CWnd* cwnd = this->GetFocus();
		if (cwnd) mCommonPreTranslateMessageHandler->handle(NULL, pMsg, (cwnd	== NULL	? -1 : cwnd->GetDlgCtrlID()));
	}
	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnActivate( UINT nState, CWnd*	pWndOther, BOOL	bMinimized )
{
	CWnd::OnActivate( nState, pWndOther, bMinimized );
	//TODO: focus lost	detection doesn't work when	a modal	dialog is used on top of the main application window.
	//	The	consequence	is that	the	book is	stopped	anyway because modal dialogs stop book audio by	default,
	//	and	the	dialog-related speech doesn't stop.
	if (Preferences::Instance()->getPauseOnLostFocus() == true)
	{
		if	(nState ==	WA_INACTIVE)
		{
			if (! AfxGetMainWnd()->IsChild(pWndOther))
			{
				bool b_BookIsPlaying = amis::dtb::DtbWithHooks::Instance()->isPlaying();
				mbWasPlayingWhenLostFocus =	b_BookIsPlaying;

				if (mbWasPlayingWhenLostFocus)
				{
					amis::dtb::DtbWithHooks::Instance()->pause();
					
					if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
					{
						AudioSequencePlayer::Instance()->Stop();
					}
				}
			}
		}
		else
		{ //WA_ACTIVE
			if (mbWasPlayingWhenLostFocus)
			{
				amis::dtb::DtbWithHooks::Instance()->resume();
			}
			mbWasPlayingWhenLostFocus =	false;
		}
	}
}
//stop the screensaver from starting
void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(Preferences::Instance()->getDisableScreensaver() == true 
		&& MainWndParts::Instance()->mpMmView->isPlaying() == true
		&& (nID & 0xFFF0) == SC_SCREENSAVE)
	{
		amis::util::Log::Instance()->writeMessage("Preventing screensaver from starting", "CMainFrame");
		return;
	}
	CMDIFrameWnd::OnSysCommand(nID, lParam);
}
//---------------------------------------------
//beginning of menu command updater functions
//---------------------------------------------
void CMainFrame::OnUpdateCmdUiGoToPage(CCmdUI* pCmdUi)
{
	if (theApp.isBookOpen() && amis::dtb::DtbWithHooks::Instance()->getNavModel()->hasPages())
		updateUiCommandState(pCmdUi, true);
	else
		updateUiCommandState(pCmdUi, false);
}

void CMainFrame::OnUpdateCmdUiAvailableWhenBookIsOpen(CCmdUI* pCmdUi)
{
	updateUiCommandState(pCmdUi, theApp.isBookOpen());
}
void CMainFrame::OnUpdateCmdUiAlmostAlwaysAvailable(CCmdUI* pCmdUi)
{
	updateUiCommandState(pCmdUi, true);
}
void CMainFrame::OnUpdateCmdUiInfoSkip(CCmdUI* pCmdUi)
{
	if (theApp.isBookOpen() && amis::dtb::DtbWithHooks::Instance()->getCustomTestSet() != NULL 
		&& amis::dtb::DtbWithHooks::Instance()->getCustomTestSet()->getLength() > 0)
		updateUiCommandState(pCmdUi, true);
	else
		updateUiCommandState(pCmdUi, false);
}
void CMainFrame::OnUpdateCmdUiPlaySlower(CCmdUI* pCmdUi)
{
	if (theApp.canDecreasePlaybackSpeed())
		updateUiCommandState(pCmdUi, true);
	else 
		updateUiCommandState(pCmdUi, false);
}
void CMainFrame::OnUpdateCmdUiPlayFaster(CCmdUI* pCmdUi)
{
	if (theApp.canIncreasePlaybackSpeed())
		updateUiCommandState(pCmdUi, true);
	else 
		updateUiCommandState(pCmdUi, false);

}
void CMainFrame::OnUpdateCmdUiCheckSidebar(CCmdUI* pCmdUi)
{
	if (MainWndParts::Instance()->mpSidebar->IsVisible() ==	TRUE)
		pCmdUi->SetCheck(1);
	else
		pCmdUi->SetCheck(0);

	updateUiCommandState(pCmdUi, true);
}
void CMainFrame::OnUpdateCmdUiFindInText(CCmdUI* pCmdUi)
{
	updateUiCommandState(pCmdUi, theApp.isBookOpen());
}
void CMainFrame::OnUpdateCmdUiPlayPause(CCmdUI* pCmdUi)
{
	if (theApp.isBookOpen())
	{
		MmView *p_view = MainWndParts::Instance()->mpMmView;
		if (p_view == NULL) return;
		bool b_is_playing = p_view->isPlaying();
		MainWndParts::Instance()->mpDefaultToolbar->togglePlayPause(!b_is_playing);
		MainWndParts::Instance()->mpBasicToolbar->togglePlayPause(!b_is_playing);
		MenuManip::Instance()->setPauseState(!b_is_playing);
		updateUiCommandState(pCmdUi, true);
	}
	else
		updateUiCommandState(pCmdUi, false);
}
void amis::gui::CMainFrame::OnUpdateCmdUiPageStyle(CCmdUI* pCmdUi)
{
	if (TextRenderBrain::Instance()->canApplyPageStyle())
	{
		int curr = TextRenderBrain::Instance()->getCurrentCustomStyleIndex();		
		if (curr == -1)
		{
			if (pCmdUi->m_nID == ID_AMIS_NO_PAGE_STYLES) pCmdUi->SetCheck(1);
			else pCmdUi->SetCheck(0);
		}
		else
		{
			if (curr == pCmdUi->m_nID - AMIS_PAGE_STYLE_BASE_ID) pCmdUi->SetCheck(1);
			else pCmdUi->SetCheck(0);
		}
		updateUiCommandState(pCmdUi, true);
	}
	else
	{
		updateUiCommandState(pCmdUi, false);
	}
}
void amis::gui::CMainFrame::OnUpdateCmdUiSmallerFont(CCmdUI* pCmdUi)
{
	if (theApp.isBookOpen() && TextRenderBrain::Instance()->canDecreaseFontSize()) 
		updateUiCommandState(pCmdUi, true);
	else
		updateUiCommandState(pCmdUi, false);
}
void amis::gui::CMainFrame::OnUpdateCmdUiBiggerFont(CCmdUI* pCmdUi)
{
	if (theApp.isBookOpen() && TextRenderBrain::Instance()->canIncreaseFontSize())
		updateUiCommandState(pCmdUi, true);
	else
		updateUiCommandState(pCmdUi, false);
}
void amis::gui::CMainFrame::OnUpdateCmdUiBookmarks(CCmdUI* pCmdUi)
{
	if (theApp.isBookOpen() && amis::dtb::DtbWithHooks::Instance()->getBookmarks() != NULL)
		updateUiCommandState(pCmdUi, true);
	else
		updateUiCommandState(pCmdUi, false);
}
void amis::gui::CMainFrame::OnUpdateCmdUiPreviousPhrase(CCmdUI* pCmdUi)
{
	if (theApp.isBookOpen() && amis::dtb::DtbWithHooks::Instance()->canGoToPreviousPhrase())
		updateUiCommandState(pCmdUi, true);
	else
		updateUiCommandState(pCmdUi, false);
}
void amis::gui::CMainFrame::OnUpdateCmdUiNextPhrase(CCmdUI* pCmdUi)
{
	if (theApp.isBookOpen() && amis::dtb::DtbWithHooks::Instance()->canGoToNextPhrase())
		updateUiCommandState(pCmdUi, true);
	else
		updateUiCommandState(pCmdUi, false);
}
void amis::gui::CMainFrame::OnUpdateCmdUiPreviousSection(CCmdUI* pCmdUi)
{
	if (theApp.isBookOpen() && amis::dtb::DtbWithHooks::Instance()->canGoToPreviousSection())
		updateUiCommandState(pCmdUi, true);
	else
		updateUiCommandState(pCmdUi, false);
}
void amis::gui::CMainFrame::OnUpdateCmdUiNextSection(CCmdUI* pCmdUi)
{
	if (theApp.isBookOpen() && amis::dtb::DtbWithHooks::Instance()->canGoToNextSection())
		updateUiCommandState(pCmdUi, true);
	else
		updateUiCommandState(pCmdUi, false);
}
void amis::gui::CMainFrame::OnUpdateCmdUiPreviousPage(CCmdUI* pCmdUi)
{
	if (theApp.isBookOpen() && amis::dtb::DtbWithHooks::Instance()->canGoToPreviousPage())
		updateUiCommandState(pCmdUi, true);
	else
		updateUiCommandState(pCmdUi, false);
}
void amis::gui::CMainFrame::OnUpdateCmdUiNextPage(CCmdUI* pCmdUi)
{
	if (theApp.isBookOpen() && amis::dtb::DtbWithHooks::Instance()->canGoToNextPage())
		updateUiCommandState(pCmdUi, true);
	else
		updateUiCommandState(pCmdUi, false);
}
//the ON_UPDATE_COMMAND_UI messages end up here because it takes care of the toolbars too
void amis::gui::CMainFrame::updateUiCommandState(CCmdUI* pCmdUi, bool value)
{
	bool enable = value;
	//don't do anything if we're waiting for a book to load
	if (theApp.isBookOpen() == false && theApp.getIsWaiting() == true) enable = false;

	pCmdUi->Enable(enable);
	mDefaultToolbar.enable(pCmdUi->m_nID, enable);
	mBasicToolbar.enable(pCmdUi->m_nID, enable);
}

LPARAM amis::gui::CMainFrame::OnUpdateToolbarState(WPARAM wParam, LPARAM lParam)
{
	toolbar::Toolbar* p_tb = (toolbar::Toolbar*)wParam;
	updateToolbarState(p_tb);
	return 0;
}
//enable or disable the commands that only work when a book is open
//note that although this function should be unnecessary when we have OnUpdateCommandUi,
//we need it anyway.  i suspect that our custom toolbars bypass the effect of OnUpdateCommandUi
void amis::gui::CMainFrame::updateToolbarState(toolbar::Toolbar* pToolbar)
{
	bool b_is_book_open = theApp.isBookOpen();
	amis::dtb::DtbWithHooks* p_dtb = amis::dtb::DtbWithHooks::Instance();

	//if we're waiting for a book to open
	if (b_is_book_open == false && theApp.getIsWaiting() == true)
	{
		pToolbar->enableAll(false);
		return;
	}
	else
	{
		//this just to turn something back on in case it isn't covered by the code below
		pToolbar->enableAll(true);
	}
	//audio rate
		pToolbar->enable(ID_AMIS_SLOWER, theApp.canDecreasePlaybackSpeed());
		pToolbar->enable(ID_AMIS_FASTER, theApp.canIncreasePlaybackSpeed());
	
	//navigation
	if (b_is_book_open)
	{
		if (p_dtb->getNavModel() != NULL && p_dtb->getNavModel()->hasPages())
		{
			pToolbar->enable(ID_AMIS_NEXT_PAGE, p_dtb->canGoToNextPage());
			pToolbar->enable(ID_AMIS_PREVIOUS_PAGE, p_dtb->canGoToPreviousPage());
			pToolbar->enable(ID_AMIS_GOTO_PAGE, true);
		}
		pToolbar->enable(ID_AMIS_PREVIOUS_SECTION, p_dtb->canGoToPreviousSection());
		pToolbar->enable(ID_AMIS_NEXT_SECTION, p_dtb->canGoToNextSection());
	
		pToolbar->enable(ID_AMIS_PREVIOUS_PHRASE, p_dtb->canGoToPreviousPhrase());
		pToolbar->enable(ID_AMIS_NEXT_PHRASE, p_dtb->canGoToNextPhrase());
	}
	else
	{
		pToolbar->enable(ID_AMIS_NEXT_PAGE, false);
		pToolbar->enable(ID_AMIS_PREVIOUS_PAGE, false);
		pToolbar->enable(ID_AMIS_GOTO_PAGE, false);
		pToolbar->enable(ID_AMIS_PREVIOUS_SECTION, false);
		pToolbar->enable(ID_AMIS_NEXT_SECTION, false);
		pToolbar->enable(ID_AMIS_PREVIOUS_PHRASE, false);
		pToolbar->enable(ID_AMIS_NEXT_PHRASE, false);
	}

	//font sizes
	if (b_is_book_open)
	{
		pToolbar->enable(ID_AMIS_BIGGER_FONT, TextRenderBrain::Instance()->canIncreaseFontSize());
		pToolbar->enable(ID_AMIS_SMALLER_FONT, TextRenderBrain::Instance()->canDecreaseFontSize());
	}
	else
	{
		pToolbar->enable(ID_AMIS_BIGGER_FONT, false);
		pToolbar->enable(ID_AMIS_SMALLER_FONT, false);
	}

	//reading options (aka "skippability")
	if (b_is_book_open && p_dtb->getCustomTestSet() != NULL && p_dtb->getCustomTestSet()->getLength() > 0)
		pToolbar->enable(ID_AMIS_SHOW_READING_OPTIONS, true);
	else
		pToolbar->enable(ID_AMIS_SHOW_READING_OPTIONS, false);

	if (b_is_book_open && p_dtb->getBookmarks() != NULL)
		pToolbar->enable(ID_AMIS_ADD_BOOKMARK, true);
	else
		pToolbar->enable(ID_AMIS_ADD_BOOKMARK, false);

	//general stuff
	pToolbar->enable(ID_AMIS_CLOSE_BOOK, b_is_book_open);
	pToolbar->enable(ID_AMIS_FOCUS_ON_TEXT, b_is_book_open);
	pToolbar->enable(ID_AMIS_TOGGLE_AUDIO_CONTENT_PLAYBACK, b_is_book_open);
	pToolbar->enable(ID_AMIS_NO_PAGE_STYLES, b_is_book_open);
	pToolbar->enable(ID_AMIS_PLAYPAUSE, b_is_book_open);
	pToolbar->enable(ID_AMIS_ESCAPE, b_is_book_open);
	pToolbar->enable(ID_AMIS_RESET_SPEED, b_is_book_open);
	pToolbar->enable(ID_AMIS_SHOW_PUBLICATION_SUMMARY, b_is_book_open);
	pToolbar->enable(ID_AMIS_NEXT_PAGE_STYLE, TextRenderBrain::Instance()->canApplyPageStyle());

	pToolbar->enable(ID_AMIS_INCREASE_SECTION_DEPTH, b_is_book_open);
	pToolbar->enable(ID_AMIS_DECREASE_SECTION_DEPTH, b_is_book_open);

	pToolbar->enable(ID_AMIS_FIND_IN_TEXT, b_is_book_open);
}
void amis::gui::CMainFrame::RecalcLayout(BOOL bNotify)
{
	TRACE(_T("CMainFrame: Recalc Layout\n"));

	if (MainWndParts::Instance()->mpBasicToolbar != NULL)
	{
		//center the buttons on	a basic	toolbar
		CRect rect;
		GetClientRect(&rect);
		mBasicToolbar.SetIndent (
			(rect.Width() - (mBasicToolbar.GetButtonCount() * LOWORD(mBasicToolbar.GetButtonSize())))	
			/ 2);
	}
	CMDIFrameWnd::RecalcLayout(bNotify);
}

void amis::gui::CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	TRACE(_T("CMainFrame: OnSize\n"));
	CMDIFrameWnd::OnSize(nType, cx, cy);
}

/**
 * 'pauseState' function parameter:
 * - FALSE => state is set to "PLAYING", the "PAUSE" action button is therefore shown
 * - TRUE  => state is set to "PAUSED", the "PLAY" action button is therefore shown
 */
LPARAM amis::gui::CMainFrame::OnSetPauseState(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("on set pause state\n"));
	bool val = (bool)wParam;
	setPauseState(val);
	return 0;
}
void amis::gui::CMainFrame::setPauseState(bool pauseState)
{
	std::wstring str2 = AudioSequencePlayer::getTextForPromptFromStringId((pauseState ? "paused" : "playing"));

	if (str2.length() > 0)
	{
		MainWndParts::Instance()->setStatusText(str2);
	}

	amis::gui::MenuManip::Instance()->setPauseState(pauseState);
	MainWndParts::Instance()->updateTitlePlayState(!pauseState);
	
	MainWndParts::Instance()->mpDefaultToolbar->togglePlayPause(pauseState);
	MainWndParts::Instance()->mpBasicToolbar->togglePlayPause(pauseState);
}

LPARAM amis::gui::CMainFrame::OnLoadNavNode(WPARAM wParam, LPARAM lParam)
{
	amis::dtb::DtbWithHooks::Instance()->loadNavNode((amis::dtb::nav::NavNode*)wParam);
	return 0;
}
