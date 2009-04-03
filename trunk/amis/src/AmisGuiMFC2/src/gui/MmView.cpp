// This file is part of Ambulant Player, www.ambulantplayer.org.
//
// Copyright (C) 2003-2005 Stichting CWI, 
// Kruislaan 413, 1098 SJ Amsterdam, The Netherlands.
//
// Ambulant Player is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// Ambulant Player is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Ambulant Player; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// MmView.cpp : implementation of the MmView class
//

#include "stdafx.h"
#include "ambulant/gui/dx/html_bridge.h"
#include "gui/MainFrm.h"

#include "gui/MmDoc.h"
#include "gui/MmView.h"
#include "gui/HtmlView.h"
#include "gui/AmisApp.h"
#include "gui/TextRenderBrain.h"
#include "Preferences.h"

#include "io/UrlInputSource.h"

#include "dtb/nav/NavModel.h"
#include "dtb/CustomTest.h"
#include "DtbWithHooks.h"
#include "../resource.h"
#include <fstream>
#include <string>


// DX Player
#include "ambulant/gui/dx/dx_player.h"
#include "ambulant/gui/dx/dx_wmuser.h"

#include "ambulant/common/preferences.h"
#include "ambulant/lib/logger.h"
#include "ambulant/lib/textptr.h"
#include "ambulant/smil2/test_attrs.h"
#include "ambulant/lib/win32/win32_asb.h"
#include "ambulant/net/url.h"
#include "ambulant/lib/string_util.h"
#include "ambulant/version.h"

//AMIS
#include "gui/MainWndParts.h"
#include "util/FilePathTools.h"
#include "DtbWithHooks.h"
#include "gui/sidebar/AmisSidebar.h"

#define WITHOUT_LOG_WINDOW

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace amis::gui;

#ifdef WITHOUT_LOG_WINDOW
const TCHAR log_name[] = TEXT("amlog.txt");

#ifdef _DEBUG
class logwindow_atltrace : public ambulant::lib::ostream 
{
	bool is_open() const {return true;}
	void close() {}
	int write(const unsigned char *buffer, int nbytes) {return write("ostream use of buffer, size not implemented for MFC");}
	int write(const char *cstr) { AtlTrace("%s", cstr); return 0;}
	void write(ambulant::lib::byte_buffer& bb) {write("ostream use of byte_buffer not implemented for MFC");}
	void flush() {}
};
#endif


static std::string get_log_filename() 
{
	TCHAR buf[_MAX_PATH];
	GetModuleFileName(NULL, buf, _MAX_PATH);
	TCHAR *p1 = text_strrchr(buf,'\\');
	if(p1 != NULL) *p1='\0';
	text_strcat(buf, TEXT("\\"));
	text_strcat(buf, log_name);
	return std::string(ambulant::lib::textptr(buf).str());
}
#endif // WITHOUT_LOG_WINDOW

static TCHAR *get_directory(const TCHAR *fn)  
{
	static TCHAR buf[_MAX_PATH];
	buf[0] = 0;
	text_strcat(buf, fn);
	TCHAR *p1 = text_strrchr(buf,'\\');
	if(p1 != NULL) *p1='\0';
	return buf;
}

#ifdef WITHOUT_LOG_WINDOW
std::ofstream 
log_os(get_log_filename().c_str());
#endif

using namespace ambulant;

#ifdef AM_PLAYER_DG
typedef gui::dg::dg_player dg_or_dx_player;
typedef gui::dg::dg_player_callbacks gui_callbacks;
#else 
typedef gui::dx::dx_player dg_or_dx_player;
typedef gui::dx::dx_player_callbacks gui_callbacks;
#endif

// The handle of the single window instance
static HWND s_hwnd;

// A class with callbacks, also instantiated once
class my_player_callbacks : public gui_callbacks 
{
  public:
	HWND new_os_window();
	void destroy_os_window(HWND hwnd);
	html_browser *new_html_browser(int left, int top, int width, int height);
	SIZE get_default_size();
};

my_player_callbacks s_player_callbacks;

HWND my_player_callbacks::new_os_window() 
{
	// Return the handle of the single instance for now
	// This means paint bits of the new window
	// to the single instance
	return s_hwnd;
}

void my_player_callbacks::destroy_os_window(HWND hwnd) 
{
	// none for now; keep the single instance
}

html_browser* my_player_callbacks::new_html_browser(int left, int top, int width, int height)
{
	return ::new_html_browser(left, top, width, height);
}

SIZE my_player_callbacks::get_default_size() 
{
	SIZE size;
	//size.cx = 800;
	//size.cy = 600;
	CRect rect;
	MainWndParts::Instance()->mpMmView->GetClientRect(&rect);
	size.cx = rect.Width();
	size.cy = rect.Height();
	return size;
}


static dg_or_dx_player* create_player_instance(const net::url& u, common::player_feedback *feedback) 
{
	dg_or_dx_player *rv = new dg_or_dx_player(s_player_callbacks, feedback, u);
	amis::io::UrlInputSource::SetDatasourceFactory(rv->get_datasource_factory());
	return rv;
}

static dg_or_dx_player *player = 0;
static bool needs_done_redraw = false;

CWnd* topView = NULL;

// MmView
IMPLEMENT_DYNCREATE(MmView, CView)
BEGIN_MESSAGE_MAP(MmView, CView)
	ON_WM_DESTROY()
	ON_COMMAND(ID_AMBULANT_PLAY, OnFilePlay)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
	ON_MESSAGE(WM_SET_CLIENT_RECT, OnSetClientRect)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_REPLACE_DOC, OnReplaceDoc)
	ON_WM_SIZE()
	ON_MESSAGE(WM_MY_GOTO_ID, OnGotoId)
END_MESSAGE_MAP()

MmView::MmView()
{
#ifndef WITHOUT_LOG_WINDOW
	m_logwindow(NULL)
#endif // WITHOUT_LOG_WINDOW

	m_timer_id = 0;
	m_cursor_id = 0;
	m_autoplay = true;
	m_recent_media_node = NULL;
	m_recent_par_node = NULL;
	m_recent_audio_node = NULL;
	m_expecting_audio = false;
	m_previous_in_progress = false;

#ifdef WITHOUT_LOG_WINDOW
#ifdef _DEBUG
	lib::logger::get_logger()->set_ostream(new logwindow_atltrace());
#else
	lib::logger::get_logger()->set_std_ostream(log_os);
#endif
#else
	lib::logger::get_logger()->set_ostream(new logwindow_ostream());
#endif // WITHOUT_LOG_WINDOW
	lib::logger::get_logger()->debug(gettext("Ambulant Player: compile time version %s, runtime version %s"), AMBULANT_VERSION, ambulant::get_version());
	lib::logger::get_logger()->debug(gettext("Ambulant Player: built on %s for Windows/MFC"), __DATE__);
#if ENABLE_NLS
	lib::logger::get_logger()->debug(gettext("Ambulant Player: localization enabled (english)"));
#endif
#ifdef AMBULANT_USE_DLL
	lib::logger::get_logger()->debug(gettext("Ambulant Player: using AmbulantPlayer in DLL"));
#endif
#ifdef AM_PLAYER_DG
	lib::logger::get_logger()->debug("Ambulant Player: using DG Player");
#else
	lib::logger::get_logger()->debug("Ambulant Player: using DX Player");
#endif
	topView = this;
	MainWndParts::Instance()->mpMmView = this;
}

MmView::~MmView()
{
	topView = NULL;
	MainWndParts::Instance()->mpMmView = NULL;
	ambulant::gui::dx::dx_player::cleanup();
}

BOOL MmView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPCHILDREN; // reduce flicker
	return CView::PreCreateWindow(cs);
}

void MmView::OnDraw(CDC* pDC)
{
	MmDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;

	// add draw code for native data here
	if(player) player->redraw(m_hWnd, pDC->m_hDC);
}


// MmView diagnostics
#ifdef _DEBUG
void MmView::AssertValid() const
{
	CView::AssertValid();
}

void MmView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

MmDoc* MmView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(MmDoc)));
	return (MmDoc*)m_pDocument;
}
#endif //_DEBUG


// MmView message handlers
int MmView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1) return -1;

	m_timer_id = SetTimer(1, 500, 0);
	
	// Set static handle
	s_hwnd = GetSafeHwnd();
	ModifyStyle(0, WS_CLIPCHILDREN); // reduce flicker
	
	//this line forces a window size on startup -- otherwise we are in full screen mode
	//PostMessage(WM_SET_CLIENT_RECT, common::default_layout_width, ambulant::common::default_layout_height);
	return 0;
}

void MmView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
}

void MmView::OnDestroy()
{
	if(player) 
	{
		CAmisHtmlView *hv = MainWndParts::Instance()->mpHtmlView;
		if (hv) hv->smilPlayerDeleted();
		player->stop();
		delete player;
		player = 0;
		m_recent_media_node = NULL;
		m_current_url = ambulant::net::url();
	}
	if(m_timer_id) KillTimer(m_timer_id);
	CView::OnDestroy();
}

void MmView::ScheduleReplaceDoc(std::string& urlstr) 
{
	//std::string url(urlstr);
	PostMessage(WM_REPLACE_DOC, 1, (LPARAM)(new std::string(urlstr)));
}

void MmView::SetMMDocument(LPCTSTR lpszPathName, bool autostart) 
{
	USES_CONVERSION;
	dg_or_dx_player *dummy = player;
	player = 0;
	m_recent_media_node = NULL;
	if(dummy) 
	{
		CAmisHtmlView *hv = MainWndParts::Instance()->mpHtmlView;
		if (hv) hv->smilPlayerDeleted();
		dummy->stop();
		delete dummy;
	}
	// Heuristic check to see whether it's a URL:
	// - Contains a \, or
	// - second char is :, or
	// - contains neither : nor /
	bool is_local_filename = false;

	if (_tcschr(lpszPathName, _T('\\'))) is_local_filename = true;
	if (lpszPathName[0] && lpszPathName[1] == _T(':')) is_local_filename = true;
	if (_tcschr(lpszPathName, _T(':')) == NULL && _tcschr(lpszPathName, _T('/')) == NULL) is_local_filename = true;

	net::url u;
	TCHAR path[_MAX_PATH];
	if (is_local_filename) 
	{
		TCHAR *pFilePart = 0;	
		GetFullPathName(lpszPathName, MAX_PATH, path, &pFilePart);
		u = net::url::from_filename(T2CA(path));
	} 
	else 
	{
		_tcscpy(path, lpszPathName);
		u = net::url::from_url(T2CA(path));
	}
	
	if (!u.is_absolute()) 
	{
		lib::logger::get_logger()->error("Cannot play from non-absolute pathname: %s", lpszPathName);
		return;
	}

	CAmisApp* pApp = (CAmisApp *) AfxGetApp(); 
	pApp->setIsWaiting(true);
	dummy = create_player_instance(u, this);
	m_curDocFilename = u.get_url().c_str();
	player = dummy;
	skipOptionsChanged();

	if (m_previous_in_progress) 
	{
		// Goto last node
		lastPhrase();
		m_previous_in_progress = false;
	}
	if(autostart || m_autoplay)	
		PostMessage(WM_COMMAND, ID_AMBULANT_PLAY);
}

void MmView::skipOptionsChanged()
{
	/*
	amis::dtb::nav::NavModel* p_nav_model = NULL;//amis::dtb::Dtb::getNavModel();
	assert(p_nav_model);
	int i;
	for (i=0; i<p_nav_model->getNumberOfCustomTests(); i++)
	{
		amis::dtb::CustomTest *p_test = p_nav_model->getCustomTest(i);

		//add the suffix "-on" to the system required attribute
		//but!! only if the book is DAISY 202
		//DAISY 3 books don't have this weirdness
		std::string test_id = p_test->getId();
		bool test_state = p_test->getCurrentState();
		TRACE("skipOptionsChanged: %s %d\n", test_id.c_str(), (int)test_state);
		// XXXJack: communicate to player
		// player->set_system_test(test_id + "-on", test_state);
		// player->set_custom_test(test_id, test_state);
	}
*/

}

void MmView::OnFilePlay()
{
	/*DBG*/static int in_here = 0;
	/*DBG*/assert(!in_here);
	/*DBG*/in_here++;
	if(player) 
	{
		setPauseState(false);
		player->play();
		needs_done_redraw = true;
		//InvalidateRect(NULL); REMOVED, otherwise crashes.
	}
	/*DBG*/in_here = 0;
}

bool MmView::isPlaying() const
{
	return player && player->is_play_active();
}

void MmView::OnUpdateFilePlay(CCmdUI *pCmdUI)
{	
	pCmdUI->Enable((int)(player && player->is_play_enabled()));
	pCmdUI->SetCheck((int)(player && player->is_play_active()));
}

void MmView::OnFilePause()
{
	if(player)
	{
		setPauseState(true);
		player->pause();
	}
}

void MmView::OnUpdateFilePause(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((int)(player && player->is_pause_enabled()));
	pCmdUI->SetCheck((int)(player && player->is_pause_active()));
}

void MmView::OnTimer(UINT nIDEvent)
{
	// XXXX Jack: This seems a very funny way to get a final redraw...
	CView::OnTimer(nIDEvent);
	if(player && needs_done_redraw && player->is_stop_active()) 
	{
		player->on_done();
		InvalidateRect(NULL);
		PostMessage(WM_INITMENUPOPUP,0, 0); 
		needs_done_redraw = false;
	}
}


void MmView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(player) player->on_click(point.x, point.y, GetSafeHwnd());
	CView::OnLButtonDown(nFlags, point);
}

void MmView::OnMouseMove(UINT nFlags, CPoint point)
{
	#ifdef AMIS_LATER
	if(player) 
	{
		int new_cursor_id = player->get_cursor(point.x, point.y, GetSafeHwnd());
		if(new_cursor_id>0) EnableToolTips(TRUE);
		else CancelToolTips();
		if(new_cursor_id != m_cursor_id) 
		{
			HCURSOR new_cursor = 0;
			if(new_cursor_id == 0) 
			{
				new_cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW); 
			} 
			else 
			{
				new_cursor = AfxGetApp()->LoadCursor(IDC_CURSOR_HAND); 
			}
			SetClassLongPtr(GetSafeHwnd(), GCLP_HCURSOR, HandleToLong(new_cursor));
			m_cursor_id = new_cursor_id;
		}
	}
	#endif
	CView::OnMouseMove(nFlags, point);
}

void MmView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(player) player->on_char(nChar);
	CView::OnChar(nChar, nRepCnt, nFlags);
}

void MmView::OnFocusAdvance()
{
	if (player) player->on_focus_advance();
}

void MmView::OnFocusActivate()
{
	if (player) player->on_focus_activate();
}

LPARAM MmView::OnSetClientRect(WPARAM wParam, LPARAM lParam) 
{
	CFrameWnd *mainWnd = (CFrameWnd*) AfxGetMainWnd();
	
	POINT pt = {0, 0}; // margins
	
	CRect rc1;
	mainWnd->GetWindowRect(&rc1);
	
	CRect rc2;
	GetWindowRect(&rc2);
	int dx = rc1.Width() - rc2.Width();
	int dy = rc1.Height() - rc2.Height();
	
	CSize size(int(wParam) + (2*pt.x + 4) + dx, int(lParam) + (2*pt.y+4) + dy);
	
	UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE;
	mainWnd->SetWindowPos(&wndTop, 0, 0, size.cx, size.cy, flags);
	return 0;
}

void MmView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	// On resize, if we have a single child, we assume it is the
	// HTML View and we resize it to our complete size.
	// NOTE: This only works for true Daisy books, where the HTML
	// view occupies the complete SMIL layout. If ever more complicated
	// layout is used this code will break.
	long childCount = GetWindowedChildCount();
	if (childCount == 1) {
		CWnd *child = GetWindow(GW_CHILD);
		assert(child);
		RECT rect;
		rect.top = 0;
		rect.left = 0;
		rect.right = cx;
		rect.bottom = cy;
		child->MoveWindow(&rect);
	}
}
LPARAM MmView::OnReplaceDoc(WPARAM wParam, LPARAM lParam) 
{
	USES_CONVERSION;
	if(lParam == 0) return 0;
	std::string *purlstr = (std::string *)lParam;
	SetMMDocument(A2CT(purlstr->c_str()), wParam?true:false);
	delete purlstr;
	return 0;
}

BOOL MmView::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	if(!player) return false;
	
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	UINT_PTR nID = pNMHDR->idFrom;
	CString strTipText;
	if(pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND)) 
	{
		std::string str = player->get_pointed_node_str();
		strTipText.Format(_T("Anchor: %s"), str.c_str());
		if(pNMHDR->code == TTN_NEEDTEXTA)
			strncpy(pTTTA->szText, str.c_str(), sizeof(pTTTA->szText));
		else
			::MultiByteToWideChar( CP_ACP , 0, str.c_str(), -1, pTTTW->szText, sizeof(pTTTW->szText) );
		*pResult = 0;
		return TRUE;
	}
	return FALSE;
}

INT_PTR MmView::OnToolHitTest(CPoint point, TOOLINFO* pTI) const 
{
	if(player) 
	{
		int new_cursor_id = player->get_cursor(point.x, point.y, GetSafeHwnd());
		if(new_cursor_id > 0) 
		{
			INT_PTR nHit = 1; // node id
			if (pTI != NULL) 
			{
				pTI->hwnd = m_hWnd;
				pTI->uId = (UINT_PTR)m_hWnd; // hWndChild;
				pTI->uFlags |= TTF_IDISHWND;
				pTI->lpszText = LPSTR_TEXTCALLBACK;
				pTI->uFlags |= TTF_NOTBUTTON; //|TTF_CENTERTIP;
			}
			return nHit;
		}
	} 
	return -1;  // not found
}

// Helper routine: find "phrase" for a given node: the most recent
// ancestor that is the child of a <seq> or <body>.
static const ambulant::lib::node* _curPhrase(const ambulant::lib::node *n)
{
	assert(n);
	const ambulant::lib::node *parent = n->up();
	while (parent) 
	{
		std::string tag = parent->get_local_name();
		if (tag == "seq" || tag == "body") 
		{
			return n;
		}
		n = parent;
		parent = n->up();
	}
	return NULL;
}

void MmView::nextPhrase()
{
	if (!m_recent_media_node) 
	{
		TRACE("nextPhrase: m_recent_media_node==NULL, nowhere to go.\n");	
		return;
	}
	TRACE("nextPhrase: m_recent_media_node=%s\n", m_recent_media_node->get_sig().c_str());
	// Find the nearest ancestor that is a child of a <seq>
	const ambulant::lib::node *phrase = _curPhrase(m_recent_media_node);
	const ambulant::lib::node *next;
	do 
	{
		if (!phrase) 
		{
			TRACE("nextPhrase: no current phrase");
			assert(player);
			player->stop();
			setPauseState(true);
			return;
		}
		next = phrase->next();
		if (!next) 
		{
			// We're apparently at the end of the parent <seq>. Up one level.
			const ambulant::lib::node *parent = phrase->up();
			if (!parent) 
			{
				// Need to go to next SMIL document.
				TRACE("nextPhrase: top-of-tree, stop playback\n");
				assert(player);
				player->stop();
				setPauseState(true);
				return;
			}
			phrase = _curPhrase(parent);
		}
		else
		{
			smil2::test_attrs ta(next);
			if (!ta.selected()) 
			{
				phrase = next;
				next=NULL;
			}
		}
	} 
	while(!next);
	
	TRACE("nextPhrase: goto_node(%s)\n", next->get_sig().c_str());
	if (isPlaying() == false)
		OnFilePlay();
	player->goto_node(next);
}

void MmView::prevPhrase()
{
	if (!m_recent_media_node) 
	{
		TRACE("prevPhrase: m_recent_media_node==NULL, nowhere to go.\n");
		amis::util::Log::Instance()->writeTrace("prevPhrase: nowhere to go");
		return;
	}
	// Find the nearest ancestor that is a child of a <seq>
	const ambulant::lib::node *phrase = _curPhrase(m_recent_media_node);
	const ambulant::lib::node *prev;
	do 
	{
top_of_do_while_loop:
		if (!phrase) 
		{
			TRACE("prevPhrase: no current phrase\n");
			amis::util::Log::Instance()->writeTrace("prevPhrase: no current phrase");
			m_previous_in_progress = true;
			assert(player);
			player->stop();
			setPauseState(true);
			return;
		}
		prev = phrase->previous();
		if (!prev) 
		{
			// We're apparently at the beginning of the parent <seq>. Up one level.
			const ambulant::lib::node *parent = phrase->up();
			if (!parent) 
			{
				// Need to go to previous SMIL document.
				TRACE("prevPhrase: top-of-tree, stop playback, handle in upper layer\n");
				amis::util::Log::Instance()->writeTrace("prevPhrase: top of tree");
				m_previous_in_progress = true;
				assert(player);
				player->stop();
				setPauseState(true);
				return;
			}
			phrase = _curPhrase(parent);
		}
		else
		{
			smil2::test_attrs ta(prev);
			if (!ta.selected()) 
			{
				phrase = prev;
				prev=NULL;
			}
		}
	} 
	while(!prev);

	// Now we need to find the last child of prev. Go down, and find
	// the phrase for that node.
	const ambulant::lib::node *ch = prev->get_last_child();
	while (ch) 
	{
		prev = ch;
		ch = prev->get_last_child();
	}
	prev = _curPhrase(prev);
	smil2::test_attrs ta(prev);
	if (!ta.selected()) 
	{
		phrase = prev;
		prev=NULL;
		goto top_of_do_while_loop;
	}
	TRACE("prevPhrase: goto_node(%s)\n", prev->get_sig().c_str());
	string msg = "prevPhrase: goto node ";
	msg.append(prev->get_sig().c_str());
	amis::util::Log::Instance()->writeTrace(msg);

	if (isPlaying() == false)
		OnFilePlay();
	player->goto_node(prev);
}

void MmView::lastPhrase()
{
	// We need to find the last child of the document. Go down, and find
	// the phrase for that node.
	ambulant::lib::document *doc = player->get_document();
	assert(doc);
	const ambulant::lib::node *root = doc->get_root();
	assert(root);
	const ambulant::lib::node *ch = root->get_last_child();
	const ambulant::lib::node *prev;
	assert(ch);
	while (ch) 
	{
		prev = ch;
		ch = prev->get_last_child();
	}
	prev = _curPhrase(prev);
	smil2::test_attrs ta(prev);
	if (!ta.selected()) 
	{ 
		m_recent_media_node = prev->down();
		assert(m_recent_media_node);
		prevPhrase(); 
		return;
	}
	TRACE("lastPhrase: goto_node(%s)\n", prev->get_sig().c_str());
	player->goto_node(prev);
}

void MmView::escapeCurrent()
{
	if (!m_recent_media_node) return;
	// We implement escaping the current structure by searching up the tree
	// until the first escapable node (designated by having a customTest) and
	// then doing nextPhrase on that one. If there's no escapable structure
	// we degenerate into nextPhrase.
	// XXXJack need to check these semantics with Marisa.
	const ambulant::lib::node *n = m_recent_media_node;
	while (n && n->get_attribute("customTest") == NULL
			&& n->get_attribute("systemTest") == NULL
			&& n->get_attribute("system-test") == NULL)
		n = n->up();
	if (n)
		m_recent_media_node = n;
	nextPhrase();
}

ambulant::net::datasource_factory* MmView::getDatasourceFactory()
{
	if (!player) return NULL;
	return player->get_datasource_factory();
}

void MmView::document_loaded(ambulant::lib::document *doc)
{
	TRACE(_T("MmView::document_loaded()\n"));
	// Hack: some FNB Daisy SMIL documents have <seq dur="0.000s">
	// that we want to fix.
#define AMBULANT_SEEK_WORKAROUND
#ifdef AMBULANT_SEEK_WORKAROUND
	// Hack number 2: Moreover, Ambulant has a bug in that it cannot currently
	// (June 2007) seek correctly if a document contains a <seq> with an explicit
	// duration which has descendants that are continuous nodes with an implicit
	// duration. Until this is fixed we simply zap all durations on <seq> nodes.
#endif
	assert(doc);
	lib::node* root = doc->get_root(false);
	assert(root);
	lib::node *body = root->get_first_child("body");
	assert(body);
	lib::node *s = body->down();
	while (s) 
	{
		const char *dur_str = s->get_attribute("dur");
#ifdef AMBULANT_SEEK_WORKAROUND
		if (dur_str)
#else
		if (dur_str && strcmp(dur_str, "0.000s") == 0)
#endif
			s->del_attribute("dur");
		s = s->next();
	}
	// Second thing to fix: replace system-required and systemRequired
	// with systemComponent.
	lib::node::iterator it;
	lib::node::iterator end = root->end();
	for (it=root->begin(); it!=end; it++) 
	{
		if ((*it).first) 
		{ // Only do something on inward iteration
			lib::node *n = (*it).second;
			//we are fiddling with things here...setting systemComponent only works (*should work*) because we are
			//messing with something
			const char *teststr = n->get_attribute("system-required");
			if (teststr) 
			{
				n->set_attribute("systemComponent", teststr);
				n->del_attribute("system-required");
			}
		}
	}
	//ambulant::smil2::test_attrs::set_current_system_component_value("pagenumber-on", true);
}

void MmView::document_started()
{
	TRACE(_T("MmView::document_started()\n"));
	setPauseState(false);	
	CAmisApp* pApp = (CAmisApp *) AfxGetApp(); 
	pApp->setIsWaiting(false);
	amis::dtb::DtbWithHooks::Instance()->getFileSet()->setSmilFile(&player->get_url());
}

void MmView::document_stopped()
{
	TRACE(_T("MmView::document_stopped()\n"));
	CAmisApp* pApp = (CAmisApp *) AfxGetApp(); 
	pApp->setIsWaiting(false);
	m_recent_media_node = NULL;
	this->m_recent_par_node = NULL;
	this->m_saw_audio = false;
	this->m_expecting_audio = false;
	if (!player) 
	{
		// If player is NULL we're actually in the process of stopping
		// this document on purpose, so don't load the next one.
		return;
	}

	setPauseState(true);	
	if (m_previous_in_progress) 
	{
		// This is step one in "goto previous sentence" across a smil boundary:
		// we load the previous SMIL document. Step 2 will be done in SetMMDocument
		// after the document has been loaded.
		if (amis::dtb::DtbWithHooks::isNull() == false) 
			amis::dtb::DtbWithHooks::Instance()->previousSmilDocument();
	} 
	else 
	{
		if (amis::dtb::DtbWithHooks::isNull() == false) 
			amis::dtb::DtbWithHooks::Instance()->nextSmilDocument();
	}
}

void MmView::node_started(const ambulant::lib::node* n)
{
	// Note by Jack: this routine makes a mess of unicode, mbcs and such.
	USES_CONVERSION;
	const char *id = n->get_attribute("id");
	ambulant::lib::xml_string tagname = n->get_local_name();
	
	CString msg;
	msg.Format(_T("Node started %s id=\"%s\" src=\"%s\"\n"), 
		A2T(n->get_local_name().c_str()), 
		A2T(n->get_attribute("id")),
		A2T(n->get_attribute("src")));
	TRACE(_T("%s"), msg);
	
	if (tagname == "audio")
	{
		m_recent_audio_node = n;
		if (theApp.getShouldNotRenderAudio() == true) return;
	}

	if (tagname != "par" && tagname != "text" && tagname != "audio" && tagname != "seq") 
		return;

	// Remember this node so we can later use it for local navigation
	// (next, prev, escape).
	if (tagname == "text" || tagname == "audio") m_recent_media_node = n;
	if (tagname == "audio") m_saw_audio = true;
	// Also, if it is a <par> we remember it, so that we can pause at the
	// end of the par if we haven't seen any audio.
	if (tagname == "par") 
	{
		m_saw_audio = false;
		m_recent_par_node = n;
		// If there's a sequence we assume there's gonna be audio in there:
		m_expecting_audio = (n->get_first_child("audio") != NULL || n->get_first_child("seq") != NULL);
	}

	// Finally, if this node has an ID we may want to update the table of contents.
	if (!id || strlen(id) == 0) return;
	
	m_current_url = player->get_url();
	std::string id_string;
	id_string.assign(id);
	m_current_url = m_current_url.add_fragment(id_string);

	//set the lastmark
	amis::dtb::DtbWithHooks::Instance()->setNewLastmark(m_current_url);

	//highlight the sidebar
	//a message is necessary because making a direct call gets caught up in thread locks
	amis::gui::MainWndParts::Instance()->mpSidebar->m_wndDlg.PostMessageW(WM_MY_UPDATE_SIDEBAR_SELECTION);
}	

void MmView::node_stopped(const ambulant::lib::node *n)
{
	USES_CONVERSION;
	
	CString msg;
	msg.Format(_T("^^^^^^^^^^ Node stopped %s id=\"%s\"\n"), A2T(n->get_local_name().c_str()), A2T(n->get_attribute("id")));
	TRACE(_T("%s"), msg);

	//if we are ending a par and we were expecting audio
	if (n->get_local_name() == "par" && m_recent_par_node != NULL && m_saw_audio == false)
	{
		player->pause();
	}
}

void MmView::node_focussed(const ambulant::lib::node *n)
{
	
}

const ambulant::net::url* MmView::getCurrentUrl()
{
	return &m_current_url;
}

//a convenience function for testing different approaches
void MmView::setPauseState(bool val)
{
	amis::gui::MainWndParts::Instance()->mpMainFrame->PostMessageW(WM_MY_SET_PAUSE_STATE, val);
}

void MmView::gotoId(string id)
{
	if (id == "") return;
	TRACE("goto ID =%s\n", id.c_str());
	//start at the top of the document and go until the node with the given ID is found
	ambulant::lib::document* doc = NULL;
	doc = player->get_document();
	if (doc == NULL) return;
	const ambulant::lib::node* n = doc->get_node(id.c_str());
	if (n)
	{
		if (isPlaying() == false)
			OnFilePlay();
		// sometimes it hangs here interminably ...
		player->goto_node(n);
	}
}

LPARAM MmView::OnGotoId(WPARAM wParam, LPARAM lParam)
{
	std::string *id = (std::string *)lParam;
	std::string id2 = *id;
	this->gotoId(id2);
	return 0;
}