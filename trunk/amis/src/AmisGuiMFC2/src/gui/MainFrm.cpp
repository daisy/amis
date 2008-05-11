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
#include "..\..\include\gui\mainfrm.h"

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
	ON_UPDATE_COMMAND_UI(ID_AMIS_NEXT_PAGE, OnUpdateCmdUiNextPage)
	ON_UPDATE_COMMAND_UI(ID_AMIS_PREVIOUS_PAGE, OnUpdateCmdUiPreviousPage)
	ON_UPDATE_COMMAND_UI(ID_AMIS_GOTO_PAGE, OnUpdateCmdUiGoToPage)
	ON_UPDATE_COMMAND_UI(ID_AMIS_ADD_BOOKMARK, OnUpdateCmdUiBookmarks)
	ON_UPDATE_COMMAND_UI(ID_AMIS_FASTER, OnUpdateCmdUiPlayFaster)
	ON_UPDATE_COMMAND_UI(ID_AMIS_CLOSE_BOOK, OnUpdateCmdUiGeneral)
	ON_UPDATE_COMMAND_UI(ID_AMIS_SHOW_READING_OPTIONS, OnUpdateCmdUiInfoSkip)
	ON_UPDATE_COMMAND_UI(ID_AMIS_SLOWER, OnUpdateCmdUiPlaySlower)
	ON_UPDATE_COMMAND_UI(ID_AMIS_FOCUS_ON_TEXT, OnUpdateCmdUiGeneral)
	ON_UPDATE_COMMAND_UI(ID_AMIS_TOGGLE_AUDIO_CONTENT_PLAYBACK, OnUpdateCmdUiGeneral)
	ON_UPDATE_COMMAND_UI(ID_AMIS_NO_PAGE_STYLES, OnUpdateCmdUiPageStyle)
	ON_UPDATE_COMMAND_UI(ID_AMIS_PLAYPAUSE, OnUpdateCmdUiPlayPause)
	ON_UPDATE_COMMAND_UI(ID_AMIS_ESCAPE, OnUpdateCmdUiGeneral)
	ON_UPDATE_COMMAND_UI(ID_AMIS_RESET_SPEED, OnUpdateCmdUiGeneral)
	ON_UPDATE_COMMAND_UI(ID_AMIS_PREVIOUS_SECTION, OnUpdateCmdUiPreviousSection)
	ON_UPDATE_COMMAND_UI(ID_AMIS_NEXT_SECTION, OnUpdateCmdUiNextSection)
	ON_UPDATE_COMMAND_UI(ID_AMIS_PREVIOUS_PHRASE, OnUpdateCmdUiPreviousPhrase)
	ON_UPDATE_COMMAND_UI(ID_AMIS_NEXT_PHRASE, OnUpdateCmdUiNextPhrase)
	ON_UPDATE_COMMAND_UI(ID_AMIS_FIND_IN_TEXT, OnUpdateCmdUiFindInText)
	ON_UPDATE_COMMAND_UI(ID_AMIS_SHOW_PUBLICATION_SUMMARY, OnUpdateCmdUiGeneral)
	ON_UPDATE_COMMAND_UI(ID_AMIS_NEXT_PAGE_STYLE, OnUpdateCmdUiPageStyle)
	ON_UPDATE_COMMAND_UI(ID_AMIS_INCREASE_SECTION_DEPTH, OnUpdateCmdUiGeneral)
	ON_UPDATE_COMMAND_UI(ID_AMIS_DECREASE_SECTION_DEPTH, OnUpdateCmdUiGeneral)
	ON_UPDATE_COMMAND_UI(ID_AMIS_TOGGLE_SIDEBAR, OnUpdateCmdUiCheckSidebar)
	ON_UPDATE_COMMAND_UI_RANGE(AMIS_PAGE_STYLE_BASE_ID, AMIS_PAGE_STYLE_BASE_ID + AMIS_MAX_ANYTHING, OnUpdateCmdUiPageStyle)
	ON_UPDATE_COMMAND_UI(ID_AMIS_SMALLER_FONT, OnUpdateCmdUiSmallerFont)
	ON_UPDATE_COMMAND_UI(ID_AMIS_BIGGER_FONT, OnUpdateCmdUiBiggerFont)
	ON_UPDATE_COMMAND_UI(ID_AMIS_INCREASE_CONTENT_VOLUME, OnUpdateCmdUiGeneral)
	ON_UPDATE_COMMAND_UI(ID_AMIS_DECREASE_CONTENT_VOLUME, OnUpdateCmdUiGeneral)
	ON_UPDATE_COMMAND_UI_RANGE(AMIS_BOOKMARKS_BASE_ID, AMIS_BOOKMARKS_BASE_ID + AMIS_MAX_ANYTHING, OnUpdateCmdUiGeneral)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_SIZE()
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
	delete mMenuVoicing;
}

void CMainFrame::OnClose() 
{
	// That is a hack to remove a weird bug (crash after ExitInstance() when a self-voicing dialog was previously open)
	AudioSequencePlayer::Instance()->Stop();
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
		amis::util::Log::Instance()->writeError("Failed to create sidebar", "CMainFrame::OnCreate",
			"AmisGuiMFC2");
		return -1;
	}
	mSidebar.SetBarStyle(mSidebar.GetBarStyle()	| CBRS_TOOLTIPS |	CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	mSidebar.EnableDocking(CBRS_ALIGN_ANY);

	//create the status bar
	if (!mStatusBar.Create(this) 
		|| !mStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		amis::util::Log::Instance()->writeError("Failed to create status bar", "CMainFrame::OnCreate",
			"AmisGuiMFC2");
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
	amis::util::Log::Instance()->writeMessage("Loading default toolbar", "CMainFrame::OnCreate", "AmisGuiMFC2");
	toolbar_file= ambulant::net::url::from_filename("./settings/defaultToolbar.xml");
	toolbar_file = toolbar_file.join_to_base(ambulant::net::url::from_filename(theApp.getAppPath()));
	toolbar_reader.readFromFile(toolbar_file.get_file());
	p_settings = toolbar_reader.getToolbarSettings();
	mDefaultToolbar.setSettings(p_settings);
	if (!mDefaultToolbar.Create(WS_CHILD | WS_VISIBLE |	TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, 
		rect, this,	mSidebar.GetDlgCtrlID()	+ 1))
	{
		amis::util::Log::Instance()->writeError("Failed to create default toolbar", "CMainFrame::OnCreate", 
			"AmisGuiMFC2");
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
		amis::util::Log::Instance()->writeError("Failed to create default rebar", "CMainFrame::OnCreate", 
			"AmisGuiMFC2");
		return -1;
	}

	mDefaultRebar.AddBar(&mDefaultToolbar);	

	//create the basic toolbar
	amis::util::Log::Instance()->writeMessage("Loading basic toolbar", "CMainFrame::OnCreate", "AmisGuiMFC2");
	toolbar_file = ambulant::net::url::from_filename("./settings/basicToolbar.xml");
	toolbar_file = toolbar_file.join_to_base(ambulant::net::url::from_filename(theApp.getAppPath()));
	toolbar_reader.readFromFile(toolbar_file.get_file());
	p_settings = toolbar_reader.getToolbarSettings();
	mBasicToolbar.setSettings(p_settings);
	if (!mBasicToolbar.Create(WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, 
		rect, this,	mSidebar.GetDlgCtrlID()	+ 2))
	{
		amis::util::Log::Instance()->writeError("Failed to create basic toolbar", "CMainFrame::OnCreate", 
			"AmisGuiMFC2");
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
		amis::util::Log::Instance()->writeError("Failed to create basic rebar", "CMainFrame::OnCreate", 
			"AmisGuiMFC2");
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


void CMainFrame::OnMenuSelect( UINT	nItemID, UINT nFlags, HMENU	hSysMenu )
{
	
	amis::util::Log* p_log = amis::util::Log::Instance();
  
	p_log->writeMessage("CMainFrame::OnMenuSelect()");
	TRACE("\nCMainFrame::OnMenuSelect()\n");
	CFrameWnd::OnMenuSelect(nItemID, nFlags, hSysMenu);

	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mMenuVoicing->OnMenuSelect(nItemID, nFlags, hSysMenu);
	}
}

void CMainFrame::OnInitMenuPopup(CMenu*	pPopupMenu,	UINT nIndex, BOOL bSysMenu)	
{
	amis::util::Log* p_log = amis::util::Log::Instance();

	p_log->writeMessage("CMainFrame::OnInitMenuPopup()");
	TRACE("\nCMainFrame::OnInitMenuPopup()\n");

	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mMenuVoicing->OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	}

	CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
}

void CMainFrame::OnInitMenu(CMenu* pMenu)
{
	
					amis::util::Log* p_log = amis::util::Log::Instance();
  
					p_log->writeMessage("CMainFrame::OnInitMenu()");
	TRACE("\nCMainFrame::OnInitMenu()\n");
	CMDIFrameWnd::OnInitMenu(pMenu);
}


void CMainFrame::OnEnterMenuLoop(BOOL bIsTrackPopupMenu)
{
	CMDIFrameWnd::OnEnterMenuLoop(bIsTrackPopupMenu);


	/* Nothing to do here.
	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mMenuVoicing->OnEnterMenuLoop(bIsTrackPopupMenu);
	} */

	MmView *view = MainWndParts::Instance()->mpMmView;
	assert(view); // XXXJack: or what to do if view == NULL? Skip?
	if (view==NULL)
	{
		return;
	}
	bool b_BookIsPlaying = view->isPlaying(); // BOOK Audio	Player
	mbWasPlayingWhenLostFocus =	b_BookIsPlaying;
	if (mbWasPlayingWhenLostFocus)
	{
		view->OnFilePause();
	}
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

		MmView *view = MainWndParts::Instance()->mpMmView;
		assert(view); // XXXJack: or what to do if view == NULL? Skip?
		if (view==NULL) {return;}

		view->OnFilePlay();
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
		MmView *view = MainWndParts::Instance()->mpMmView;
		assert(view); // XXXJack: or what to do if view == NULL? Skip?

		if (view=NULL)
		{
			return;
		}
		if	( nState ==	WA_INACTIVE	 )
		{
			if ( ! AfxGetMainWnd()->IsChild(pWndOther) )
			{
				bool b_BookIsPlaying = view->isPlaying(); // BOOK Audio	Player

				//mbWasPlayingWhenLostFocus	= b_BookIsPlaying || b_GuiIsPlaying;
				mbWasPlayingWhenLostFocus =	b_BookIsPlaying;

				if (mbWasPlayingWhenLostFocus)
				{
					view->OnFilePause();

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

				view->OnFilePlay();

				/*if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
				{
					AudioSequencePlayer::Instance()->RepeatLast();
				}*/
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
		amis::util::Log::Instance()->writeMessage("Preventing screensaver from starting", "CMainFrame", "AmisGuiMFC2");
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

void CMainFrame::OnUpdateCmdUiGeneral(CCmdUI* pCmdUi)
{
	updateUiCommandState(pCmdUi, theApp.isBookOpen());
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
}
void CMainFrame::OnUpdateCmdUiFindInText(CCmdUI* pCmdUi)
{
	pCmdUi->Enable(theApp.isBookOpen());
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
	}
	else
		updateUiCommandState(pCmdUi, false);
}
void amis::gui::CMainFrame::OnUpdateCmdUiPageStyle(CCmdUI* pCmdUi)
{
	if (theApp.isBookOpen() && Preferences::Instance()->getCustomCssFiles()->size() > 0)
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
		updateUiCommandState(pCmdUi, false);
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
	pCmdUi->Enable(value);
	mDefaultToolbar.enable(pCmdUi->m_nID, value);
	mBasicToolbar.enable(pCmdUi->m_nID, value);
}

//enable or disable the commands that only work when a book is open
//note that although this function should be unnecessary when we have OnUpdateCommandUi,
//we need it anyway.  i suspect that our custom toolbars bypass the effect of OnUpdateCommandUi
void amis::gui::CMainFrame::updateToolbarState(toolbar::Toolbar* pToolbar)
{
	bool b_is_book_open = theApp.isBookOpen();
	//audio rate
		pToolbar->enable(ID_AMIS_SLOWER, theApp.canDecreasePlaybackSpeed());
		pToolbar->enable(ID_AMIS_FASTER, theApp.canIncreasePlaybackSpeed());
	
	//navigation
	if (b_is_book_open)
	{
		if (amis::dtb::DtbWithHooks::Instance()->getNavModel()->hasPages())
		{
			pToolbar->enable(ID_AMIS_NEXT_PAGE, amis::dtb::DtbWithHooks::Instance()->canGoToNextPage());
			pToolbar->enable(ID_AMIS_PREVIOUS_PAGE, amis::dtb::DtbWithHooks::Instance()->canGoToPreviousPage());
			pToolbar->enable(ID_AMIS_GOTO_PAGE, true);
		}
		pToolbar->enable(ID_AMIS_PREVIOUS_SECTION, amis::dtb::DtbWithHooks::Instance()->canGoToPreviousSection());
		pToolbar->enable(ID_AMIS_NEXT_SECTION, amis::dtb::DtbWithHooks::Instance()->canGoToNextSection());
	
		pToolbar->enable(ID_AMIS_PREVIOUS_PHRASE, amis::dtb::DtbWithHooks::Instance()->canGoToPreviousPhrase());
		pToolbar->enable(ID_AMIS_NEXT_PHRASE, amis::dtb::DtbWithHooks::Instance()->canGoToNextPhrase());
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
	if (b_is_book_open 
		&& amis::dtb::DtbWithHooks::Instance()->getCustomTestSet() != NULL 
		&& amis::dtb::DtbWithHooks::Instance()->getCustomTestSet()->getLength() > 0)
		pToolbar->enable(ID_AMIS_SHOW_READING_OPTIONS, true);
	else
		pToolbar->enable(ID_AMIS_SHOW_READING_OPTIONS, false);

	if (b_is_book_open && amis::dtb::DtbWithHooks::Instance()->getBookmarks() != NULL)
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
	if (b_is_book_open && Preferences::Instance()->getCustomCssFiles()->size() > 0)
		pToolbar->enable(ID_AMIS_NEXT_PAGE_STYLE, true);
	else
		pToolbar->enable(ID_AMIS_NEXT_PAGE_STYLE, false);

	pToolbar->enable(ID_AMIS_INCREASE_SECTION_DEPTH, b_is_book_open);
	pToolbar->enable(ID_AMIS_DECREASE_SECTION_DEPTH, b_is_book_open);

	//pToolbar->enable(ID_AMIS_INCREASE_CONTENT_VOLUME, b_is_book_open);
	//pToolbar->enable(ID_AMIS_DECREASE__CONTENT_VOLUME, b_is_book_open);

	pToolbar->enable(ID_AMIS_FIND_IN_TEXT, b_is_book_open);
}
void amis::gui::CMainFrame::RecalcLayout(BOOL bNotify)
{
	if (MainWndParts::Instance()->mpBasicToolbar == NULL) return;
	//center the buttons on	a basic	toolbar
	CRect rect;
	GetClientRect(&rect);
	mBasicToolbar.SetIndent (
		(rect.Width() - (mBasicToolbar.GetButtonCount() * LOWORD(mBasicToolbar.GetButtonSize())))	
		/ 2);

	CMDIFrameWnd::RecalcLayout(bNotify);
}

void amis::gui::CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIFrameWnd::OnSize(nType, cx, cy);
	if (MainWndParts::Instance()->mpMmView != NULL)
	{
		//CRect sr;
		//CRect r;
		//this->GetWindowRect(&r);
		//MainWndParts::Instance()->mpSidebar->GetWindowRect(&sr);
		MainWndParts::Instance()->mpMmView->OnSize(nType, cx, cy);
	}
}
