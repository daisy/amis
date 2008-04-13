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
#include "gui/MainWndParts.h"
#include "gui/MenuManip.h"
#include "gui/AmisApp.h"
#include "DtbWithHooks.h"

using namespace amis::gui;

MainWndParts* MainWndParts::pinstance = 0;

MainWndParts* MainWndParts::Instance()
{
	if (pinstance == 0) pinstance = new MainWndParts; 
    return pinstance;
}
void MainWndParts::DestroyInstance()
{
	delete pinstance;
}


MainWndParts::MainWndParts()
{
	mpSidebar = NULL;
	mpMainFrame = NULL;
	mpBasicToolbar = NULL;
	mpDefaultToolbar = NULL;
	mpBasicRebar = NULL;
	mpDefaultRebar = NULL;
	mpHtmlView = NULL;
	mpStatusBar = NULL;
	mpMmDoc = NULL;
	mpMmView = NULL;

	mbSidebarVisible = true;
	mbBasicView = false;
}

MainWndParts::~MainWndParts()
{
}

void MainWndParts::toggleViewMode()
{
	if (mbBasicView == false) basicView();		
	else defaultView();
}


void MainWndParts::toggleSidebar()
{
	mbSidebarVisible = !mbSidebarVisible;
	MenuManip::Instance()->setViewItemCheckmark(mbSidebarVisible, ID_AMIS_TOGGLE_SIDEBAR);

	//if the window is hidden, show it
	if (!mpSidebar->ShowWindow(SW_SHOWNA)) mpMainFrame->ShowControlBar(mpSidebar, TRUE, TRUE);
	//else, hide it
	else mpMainFrame->ShowControlBar(mpSidebar, FALSE, TRUE);
	
	mpMainFrame->RecalcLayout();
}

void MainWndParts::basicView()
{
	if (this->mbSidebarVisible == true) toggleSidebar();
	
	MainWndParts::Instance()->updateTitleViewMode();
	// TODO: change the window title text to say "basic view mode"
	mpMainFrame->ShowControlBar(mpStatusBar, FALSE, TRUE);
	mpMainFrame->ShowControlBar(mpDefaultRebar, FALSE, TRUE);
	mpMainFrame->ShowControlBar(mpBasicRebar, TRUE, TRUE);
	mpMainFrame->SetMenu(NULL);
	mpMainFrame->RecalcLayout();
	mbBasicView = true;	
}

void MainWndParts::defaultView()
{
	CMenu* p_menu  = NULL;
	MainWndParts::Instance()->updateTitleViewMode();	
	mpMainFrame->restoreMenu();
	if (theApp.isBookOpen())
	{
		//update the menus
		MenuManip::Instance()->addNavContainersToViewMenu();
		MenuManip::Instance()->loadBookmarks(amis::dtb::DtbWithHooks::Instance()->getBookmarks());
		MenuManip::Instance()->addPageStyles();
		MenuManip::Instance()->setupNavigationOptions();
		MenuManip::Instance()->refreshRecentBooksListMenu();
	}

	mpMainFrame->ShowControlBar(mpStatusBar, TRUE, TRUE);
	mpMainFrame->ShowControlBar(mpDefaultRebar, TRUE, TRUE);
	mpMainFrame->ShowControlBar(mpBasicRebar, FALSE, TRUE);
	mpMainFrame->RecalcLayout();
	mbBasicView = false;
	//TODO: make this smarter: only turn the sidebar on if it had been on before
	if (this->mbSidebarVisible == false) toggleSidebar();
}

bool MainWndParts::isBasicView()
{
	return mbBasicView;
}


void MainWndParts::updateTitleViewMode()
{
	updateTitleBar(TITLEBAR_VIEWMODE, (isBasicView() ? L"Basic view mode" : L"Default view mode"));
}

void MainWndParts::updateTitleSelfVoicing(bool selfVoiced)
{
	updateTitleBar(TITLEBAR_SELFVOICING, (selfVoiced ? L"Self-voicing" : L"Not self-voicing"));
}

void MainWndParts::updateTitlePlayState(bool playing)
{
	updateTitleBar(TITLEBAR_PLAYSTATE, (playing ? L"Playing..." : L"Paused."));
}

void MainWndParts::updateTitleBar(TitleBarFieldID field, CString str) 
{
	CString title;
	switch (field) 
	{
		case TITLEBAR_APPNAME: {
			mStrTITLEBAR_APPNAME = str;
			break;
							   }
		case TITLEBAR_BOOKTITLE: {
			mStrTITLEBAR_BOOKTITLE = str;
			break;
							   }
		case TITLEBAR_VIEWMODE: {
			mStrTITLEBAR_VIEWMODE = str;
			break;
							   }
		case TITLEBAR_SELFVOICING: {
			mStrTITLEBAR_SELFVOICING = str;
			break;
							   }
		case TITLEBAR_PLAYSTATE: {
			mStrTITLEBAR_PLAYSTATE = str;
			break;
							   }
	}
	title.Append(mStrTITLEBAR_APPNAME);
	title.Append(L" \"");
	if (mStrTITLEBAR_BOOKTITLE.IsEmpty())
	{
		title.Append(L"NO TITLE");
	}
	else
	{
		title.Append(mStrTITLEBAR_BOOKTITLE);
	}
	title.Append(L"\" ");
	title.Append(mStrTITLEBAR_VIEWMODE);
	title.Append(L" - ");
	title.Append(mStrTITLEBAR_SELFVOICING);
	title.Append(L" - ");
	if (mStrTITLEBAR_PLAYSTATE.IsEmpty())
	{
		title.Append(L"Playing?");
	}
	else
	{
		title.Append(mStrTITLEBAR_PLAYSTATE);
	}

	mpMainFrame->SetTitle(L"");
	AfxGetApp()->m_pMainWnd->SetWindowText(L"");

	mpMainFrame->SetTitle(title);
	AfxGetApp()->m_pMainWnd->SetWindowText(title);
}