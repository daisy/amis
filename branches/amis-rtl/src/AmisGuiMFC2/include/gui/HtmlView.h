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
#ifndef HTMLVIEW_H
#define HTMLVIEW_H

#include <afxhtml.h>
#include "HtmlDoc.h"
#include "ambulant/gui/dx/html_bridge.h"

// #define this to load html through the AmbulantPdtbIE plugin for protected books
#define HTML_LOAD_AMBULANT_PDTB

#include "PdtbBridge.h"

namespace amis
{
namespace gui
{
class CAmisHtmlView : public CHtmlView
{
protected:
	CAmisHtmlView();
	DECLARE_DYNCREATE(CAmisHtmlView)

public:
	CAmisHtmlView(const RECT&, CWnd*);	
	virtual ~CAmisHtmlView();

	CAmisHtmlDoc* GetDocument();
	//returns a pointer to the stylesheet
	IHTMLStyleSheet* applyStylesheet(const ambulant::net::url*);
	void removeStylesheet(IHTMLStyleSheet*);

	virtual void OnDraw(CDC*);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT&);
	virtual void OnInitialUpdate();
	virtual void OnDocumentComplete(LPCTSTR);
	virtual BOOL Create(LPCTSTR, LPCTSTR, DWORD, const RECT&, CWnd*, UINT, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG*);
	virtual void OnBeforeNavigate2(LPCTSTR, DWORD, LPCTSTR, CByteArray&, LPCTSTR, BOOL*);
	virtual void OnNavigateComplete2(LPCTSTR);
	virtual void CAmisHtmlView::OnNavigateError(LPCTSTR, LPCTSTR, DWORD, BOOL*);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnActivateView(BOOL, CView*, CView*);
	virtual HRESULT OnFilterDataObject(LPDATAOBJECT, LPDATAOBJECT*);
	virtual HRESULT OnShowContextMenu(DWORD, LPPOINT, LPUNKNOWN, LPDISPATCH);
	virtual void OnEditCopy();
	afx_msg int OnCreate(LPCREATESTRUCT);
	DECLARE_MESSAGE_MAP()
private:
	void forceResizeHack();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext&) const;
#endif

	
	
public:
	
//from: http://support.microsoft.com/default.aspx?scid=kb;EN-US;Q241750
//new code
// Operations
public:
    CString GetFullName() const;
    CString GetType() const;
    CString GetLocationName() const;
    CString GetLocationURL() const;

    void Navigate(LPCTSTR lpszURL, DWORD dwFlags = 0 ,
                  LPCTSTR lpszTargetFrameName = NULL ,
                  LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,
                  DWORD dwPostDataLen = 0);
	LPARAM OnHighlightUrlTarget(WPARAM wParam, LPARAM lParam);
	void smilPlayerDeleted();
	IPdtbBridge *mpLoaderBridge;

//Add the following functions. This action prevents the leak because the
//CHtmlView versions of LoadFromResource() call CHtmlView::Navigate().
//These need to be defined because CHtmlView::Navigate() is not virtual.
    BOOL LoadFromResource(LPCTSTR lpszResource);
    BOOL LoadFromResource(UINT nRes);
//end new code
};

#ifndef _DEBUG  // debug version in AmisHtmlView.cpp

inline CAmisHtmlDoc* CAmisHtmlView::GetDocument()
   { return (CAmisHtmlDoc*)m_pDocument; }

#endif

class html_browser_imp : public html_browser {
public:
	html_browser_imp(int left, int top, int width, int height);
	~html_browser_imp();
	void goto_url(std::string url, ambulant::net::datasource_factory *df);
	void show();
	void hide();
	void redraw();
	bool uses_screen_reader();
private:
	static CAmisHtmlView *s_browser;
};

inline html_browser *new_html_browser(int left, int top, int width, int height)
{
	return new html_browser_imp(left, top, width, height);
}
}
}
#endif
