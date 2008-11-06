/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2008  DAISY Consortium

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
#ifndef MAINWNDPARTS_H
#define MAINWNDPARTS_H

#include "gui/MainFrm.h"

#include "gui/sidebar/AmisSidebar.h"
#include "gui/toolbar/Toolbar.h"
#include "HtmlView.h"
#include "MmDoc.h"
#include "MmView.h"
#include "util/ThreadYielder.h"

using namespace amis::gui;

namespace amis
{
namespace gui
{
	class MainWndParts : public ThreadYielder
{
protected:
	MainWndParts();

public:
	static MainWndParts* Instance();
	void DestroyInstance();
	~MainWndParts();

	void toggleSidebar();
	void basicView();
	void defaultView();
	void toggleViewMode();
	bool isBasicView();

	void peekAndPump();
	void setStatusText(wstring);
public:
	amis::gui::sidebar::cdxCDynamicBarT<amis::gui::sidebar::CAmisSidebar>* mpSidebar;
	CMainFrame* mpMainFrame;
	amis::gui::toolbar::Toolbar* mpBasicToolbar;
	amis::gui::toolbar::Toolbar* mpDefaultToolbar;
	CReBar* mpBasicRebar;
	CReBar* mpDefaultRebar;
	CAmisHtmlView* mpHtmlView;
	MmDoc* mpMmDoc;
	MmView* mpMmView;
	CStatusBar* mpStatusBar;
	
private:
	bool mbBasicView;
	CString mMainWindowTitle;
	bool mbSidebarWasVisible;

private:
	static MainWndParts* pinstance;

public:

	enum TitleBarFieldID{TITLEBAR_APPNAME, TITLEBAR_BOOKTITLE, TITLEBAR_VIEWMODE, TITLEBAR_SELFVOICING, TITLEBAR_PLAYSTATE};
	void updateTitleBar(TitleBarFieldID field, CString str);

	void updateTitleViewMode();
	void updateTitleSelfVoicing(bool selfVoiced);
	void updateTitlePlayState(bool playing);

private:

	CString mStrTITLEBAR_APPNAME;
	CString mStrTITLEBAR_VIEWMODE;
	CString mStrTITLEBAR_SELFVOICING;
	CString mStrTITLEBAR_PLAYSTATE;
	CString mStrTITLEBAR_BOOKTITLE;

};
}
}
#endif