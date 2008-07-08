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
#ifndef MAINFRM_H
#define MAINFRM_H

#include "gui/sidebar/AmisSidebar.h"
#include "gui/toolbar/Toolbar.h"
#include "gui/self-voicing/PreTranslateMessageHandler.h"
#pragma once

namespace amis
{
namespace gui
{
class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
	friend class MenuVoicing;

public:
	CMainFrame();
	PreTranslateMessageHandler* mCommonPreTranslateMessageHandler;
	
	bool mbWasPlayingWhenLostFocus;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	BOOL OnSetCursor(CWnd*, UINT, UINT);
	virtual ~CMainFrame();
	void restoreMenu();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CStatusBar  mStatusBar;
	amis::gui::sidebar::cdxCDynamicBarT<amis::gui::sidebar::CAmisSidebar>	mSidebar;
	amis::gui::toolbar::Toolbar mDefaultToolbar;
	amis::gui::toolbar::Toolbar mBasicToolbar;
	CReBar mDefaultRebar;
	CReBar mBasicRebar;
	CMenu mSavedMenu;

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnMenuSelect( UINT nItemID, UINT nFlags, HMENU hSysMenu );
	//afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) ;
	//afx_msg LRESULT OnMenuChar( UINT nItemID, UINT nFlags, CMenu* menu );
	afx_msg void OnEnterMenuLoop(BOOL bIsTrackPopupMenu);
	afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized ); 
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//these update the menu items and toolbar buttons for the mapped command
	afx_msg void OnUpdateCmdUiGoToPage(CCmdUI*);
	afx_msg void OnUpdateCmdUiAvailableWhenBookIsOpen(CCmdUI*);
	afx_msg void OnUpdateCmdUiAlmostAlwaysAvailable(CCmdUI*);
	afx_msg void OnUpdateCmdUiInfoSkip(CCmdUI*);
	afx_msg void OnUpdateCmdUiPlaySlower(CCmdUI*);
	afx_msg void OnUpdateCmdUiPlayFaster(CCmdUI*);
	afx_msg void OnUpdateCmdUiCheckSidebar(CCmdUI*);
	afx_msg void OnUpdateCmdUiFindInText(CCmdUI*);
	afx_msg void OnUpdateCmdUiPlayPause(CCmdUI*);
	afx_msg void OnUpdateCmdUiPageStyle(CCmdUI*);
	afx_msg void OnUpdateCmdUiBiggerFont(CCmdUI*);
	afx_msg void OnUpdateCmdUiSmallerFont(CCmdUI*);
	afx_msg void OnUpdateCmdUiBookmarks(CCmdUI*);
	afx_msg void OnUpdateCmdUiPreviousSection(CCmdUI*);
	afx_msg void OnUpdateCmdUiNextSection(CCmdUI*);
	afx_msg void OnUpdateCmdUiPreviousPhrase(CCmdUI*);
	afx_msg void OnUpdateCmdUiNextPhrase(CCmdUI*);
	afx_msg void OnUpdateCmdUiPreviousPage(CCmdUI*);
	afx_msg void OnUpdateCmdUiNextPage(CCmdUI*);
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
public:
	void updateToolbarState(toolbar::Toolbar*);
private:
	void updateUiCommandState(CCmdUI*, bool);
public:
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
}
}
#endif

