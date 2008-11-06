/*
 * This file is part of Ambulant Player, www.ambulantplayer.org.
 *
 * Copyright (C) 2003-2005 Stichting CWI, 
 * Kruislaan 413, 1098 SJ Amsterdam, The Netherlands.
 *
 * Ambulant Player is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * Ambulant Player is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Ambulant Player; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

// MmView.h : interface of the MmView class
//

#pragma once

#include "ambulant/common/player.h"
#include "ambulant/gui/dx/dx_player.h"
#include "MmDoc.h"
#include <string>

namespace amis
{
namespace gui
{
class CLogWindow;

class MmView : public CView, public ambulant::common::player_feedback
{
protected: // create from serialization only
	MmView();
	DECLARE_DYNCREATE(MmView)

// Attributes
public:
	MmDoc* GetDocument() const;
	
// Operations
public:
	void SetMMDocument(LPCTSTR lpszPathName, bool autostart);
	void skipOptionsChanged();
	void ScheduleReplaceDoc(std::string& url);
	ambulant::net::datasource_factory *getDatasourceFactory();
	bool expectingAudio() const { return m_expecting_audio; }
	UINT_PTR m_timer_id;
	CString m_curDocFilename;
	CString m_curFilter;
	UINT m_cursor_id;
	bool m_autoplay;
	CString m_welcomeDocFilename;
#ifndef WITHOUT_LOG_WINDOW
	CLogWindow *m_logwindow;
#endif
	const ambulant::lib::node *m_recent_media_node;
	const ambulant::lib::node *m_recent_par_node;
	const ambulant::lib::node *m_recent_audio_node;
	bool m_expecting_audio;
	bool m_previous_in_progress;
	bool m_saw_audio;
// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~MmView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();
	virtual void OnInitialUpdate();
	afx_msg void OnFilePlay();
	afx_msg void OnUpdateFilePlay(CCmdUI *pCmdUI);
	bool isPlaying() const;
	afx_msg void OnFilePause();
	afx_msg void OnUpdateFilePause(CCmdUI *pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFocusAdvance();
	afx_msg void OnFocusActivate();
	afx_msg LRESULT OnSetClientRect(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	afx_msg LRESULT OnReplaceDoc(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// Daisy local navigation support
	void prevPhrase();
	void nextPhrase();
	void lastPhrase();
	void escapeCurrent();
	
	//returns smilfile#id (updated every time the id changes)
	const ambulant::net::url* getCurrentUrl();

	//pasted in by Marisa from AMIS' last copy of MmView.h
private:
	ambulant::gui::dx::dx_player *CreatePlayerInstance(const ambulant::net::url& u);
	ambulant::net::url m_current_url;
public:
	// player_feedback interface:
	void document_loaded(ambulant::lib::document *doc);
	void document_started();
	void document_stopped();
	void node_started(const ambulant::lib::node *n);
	void node_stopped(const ambulant::lib::node *n);
	void node_focussed(const ambulant::lib::node *n);
};

#ifndef _DEBUG  // debug version in MmView.cpp
inline MmDoc* MmView::GetDocument() const
   { return reinterpret_cast<MmDoc*>(m_pDocument); }
#endif
}
}
