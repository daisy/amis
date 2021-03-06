/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

Copyright (c) 2004 DAISY Consortium

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


#include <algorithm>
#include <string>
#include <cassert>


#include "stdafx.h"
#include "comutil.h"

#include "gui/AmisApp.h"
#include "gui/HtmlDoc.h"
#include "gui/HtmlView.h"
#include "gui/MainWndParts.h"
#include "gui/MmDoc.h"
#include "gui/TextRenderBrain.h"
#include "DtbWithHooks.h"
#include "util/Log.h"
#include "Preferences.h"
#include "util/FilePathTools.h"

#define WITH_HTML_WIDGET
#define AM_DBG if (0)

typedef amis::gui::CAmisHtmlView HtmlView;

using namespace std;
using namespace amis::gui;

// Messages for communicating between worker threads and main (GUI) thread, which
// is the only one that should do html-widget calls.
#define WM_MY_NAVIGATESTRING (WM_USER+1)
#define WM_MY_HIGHLIGHTURITARGET (WM_USER+2)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef WITH_PROTECTED_BOOK_SUPPORT
// XXXJack
// Grmpf: GUIDs don't get defined. Copied here (from PdtbBridge.h)
// for the time being. Even the procedure outlined in Q130869 does
// not seem to work:-(
#include <initguid.h>
/*
DEFINE_GUID(CLSID_CPdtbBridge,
			0xF326B6FD,0x40F5,0x44D7,0xA8,0x7A,0xE6,0x90,0x50,0x0C,0x27,0xF1);
DEFINE_GUID(IID_PdtbBridge,
			0x659B0CA3,0x0746,0x4050,0x85,0x05,0x9C,0x13,0x2A,0x3B,0xBC,0xC5);
*/
#endif

extern CWnd*  topView;

IMPLEMENT_DYNCREATE(CAmisHtmlView, CHtmlView)

BEGIN_MESSAGE_MAP(CAmisHtmlView, CHtmlView)
	ON_WM_CREATE()
	ON_MESSAGE(WM_MY_HIGHLIGHTURITARGET, OnHighlightUrlTarget)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_WM_SIZE()
END_MESSAGE_MAP()

CAmisHtmlView::CAmisHtmlView()
{
	mpLoaderBridge = NULL;
}

CAmisHtmlView::CAmisHtmlView(const RECT& rect, CWnd* parent)
{
	mpLoaderBridge = NULL;

	Create(NULL,_T("HtmlWidget"),WS_VISIBLE,rect,parent,AFX_IDW_PANE_FIRST);
}

CAmisHtmlView::~CAmisHtmlView()
{
#ifdef WITH_PROTECTED_BOOK_SUPPORT
	if (mpLoaderBridge) 
	{
		mpLoaderBridge->Release();
		mpLoaderBridge = NULL;
	}
#endif
}

BOOL CAmisHtmlView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CHtmlView::PreCreateWindow(cs);
}

void CAmisHtmlView::OnDraw(CDC* pDC)
{
	CAmisHtmlDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

#ifdef _DEBUG
void CAmisHtmlView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CAmisHtmlView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}


CAmisHtmlDoc* CAmisHtmlView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAmisHtmlDoc)));
	return (CAmisHtmlDoc*)m_pDocument;
}

#endif //_DEBUG

int CAmisHtmlView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CHtmlView::OnCreate(lpCreateStruct) == -1) return -1;
	
	MainWndParts::Instance()->mpHtmlView = this;
	this->ModifyStyle(0, WS_CLIPCHILDREN);
	return 0;
}

void CAmisHtmlView::OnInitialUpdate() 
{
	this->SetSilent(TRUE);
}

void CAmisHtmlView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CHtmlView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

//this signals the completion of a URL load request
void CAmisHtmlView::OnDocumentComplete(LPCTSTR lpszURL) 
{
	CHtmlView::OnDocumentComplete(lpszURL);
	CString url = lpszURL;
	if (url.Compare(_T("about:blank")) != 0) TextRenderBrain::Instance()->webDocumentComplete();
	else TRACE(L"%s", _T("AMIS ignoring document complete for about:blank\n"));
}

BOOL CAmisHtmlView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// create the view window itself
	m_pCreateContext = pContext;
	if (!CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd,  nID, pContext))
		return FALSE;

	RECT rectClient;
	GetClientRect(&rectClient);

	// create the control window
	// AFX_IDW_PANE_FIRST is a safe but arbitrary ID
	if (!m_wndBrowser.CreateControl(CLSID_WebBrowser, lpszWindowName,
			WS_VISIBLE | WS_CHILD , rectClient, this, AFX_IDW_PANE_FIRST))
	{
		DestroyWindow();
		return FALSE;
	}
	
	LPUNKNOWN lpUnk = m_wndBrowser.GetControlUnknown();
	HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &m_pBrowserApp);
	if (!SUCCEEDED(hr))
	{
		m_pBrowserApp = NULL;
		m_wndBrowser.DestroyWindow();
		DestroyWindow();
		return FALSE;
	}

#if 1
	// XXXJack: See if this helps the first call to NavigateString succeed
	this->Navigate2(_T("about:blank"), 0, NULL, NULL, NULL, 0);
#endif

	return TRUE;
}

//this stops the control from responding to keyboard events (like ctrl-n for new window)
BOOL CAmisHtmlView::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN) 
	{
		CWnd* p_parent = this->GetTopLevelParent();
		p_parent->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
	}
  
	return CHtmlView::PreTranslateMessage(pMsg);
}

void CAmisHtmlView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, 
									  LPCTSTR lpszTargetFrameName,
									  CByteArray& baPostedData, 
									  LPCTSTR lpszHeaders, 
									  BOOL* pbCancel) 
{
	USES_CONVERSION;
	
	// lpszURL can be either a URL or a filename (sigh)
	// Guess: if there's a : in the string and it is not in second position
	// this is a URL.
	string urlOrFile = T2A(lpszURL);

	//strip the amisie protocol if it's there
	if (urlOrFile.substr(0, 7) == "amisie:")
		urlOrFile = urlOrFile.replace(0, 7, "");

	ambulant::net::url thisUrl;
	string::size_type colonPos = urlOrFile.find(':');
	if (colonPos == string::npos || colonPos == 1) 
	{
		thisUrl = ambulant::net::url::from_filename(urlOrFile, true);
	} 
	else if (urlOrFile.substr(0, 5) == "file:") 
	{
		// Stupid hack: urlOrFile can be mixed-case. Sigh.
		ambulant::net::url tmpurl = ambulant::net::url::from_url(urlOrFile);
		assert(tmpurl.is_local_file());
		std::string tmpfile = tmpurl.get_file();
		std::string ref = amis::util::FilePathTools::getTarget(urlOrFile);
		if (ref != "")
			tmpfile += "#" + ref;
		thisUrl = ambulant::net::url::from_filename(tmpfile, true);
	} 
	else 
	{
		thisUrl = ambulant::net::url::from_url(urlOrFile);
	}
	
	ambulant::net::url thisDir = thisUrl.get_base();
	//we can't just use the book dir because it comes with a trailing slash .. so use the OPF or NCC instead
	amis::dtb::DtbFileSet* p_files = amis::dtb::DtbWithHooks::Instance()->getFileSet();
	if (urlOrFile == "about:blank")
	{
		CHtmlView::OnBeforeNavigate2(lpszURL, nFlags,	lpszTargetFrameName, baPostedData, lpszHeaders, pbCancel);
		return;
	}
	if (p_files == NULL)
	{
		*pbCancel = TRUE;
		return;
	}
	ambulant::net::url bookDir;
	if (p_files->getOpfFilepath() != NULL && p_files->getOpfFilepath()->is_empty_path() == false)
		bookDir = *p_files->getOpfFilepath();
	else
		bookDir = *p_files->getNavFilepath();
	bookDir = bookDir.get_base();
	CString cstr_url(lpszURL);
	
	bool is_same_dir = thisDir.same_document(bookDir);
	//bool is_blank = !cstr_url.Compare(_T("about:blank"));
	bool is_pdtb = amis::dtb::DtbWithHooks::Instance()->isProtected();
	bool has_http = false;
	if (thisUrl.get_protocol() == "http") has_http = true;
	//if either:
	//1. book NOT protected, link NOT part of book, link NOT to about:blank
	//2. book IS protected, link IS web
	//then launch the link externally
	amis::dtb::DtbWithHooks* dtbobj = amis::dtb::DtbWithHooks::Instance();
	if (((!is_pdtb && !is_same_dir/* && !is_blank*/) || (is_pdtb && has_http)) && 
		dtbobj->getDaisyVersion() != amis::dtb::DAISY_2005)
	{
		string log_msg = "Launching external link externally: " + thisUrl.get_url();
		amis::util::Log::Instance()->writeTrace(log_msg);
		//this should open the link in the default browser
		ShellExecute(NULL, _T("open"), lpszURL, NULL, NULL, SW_SHOWNORMAL);
		*pbCancel = TRUE;
	}
	//else this path lies within our current book directory
	else
	{
		//if it's a smil file, load it via AMIS
		if (thisUrl.guesstype() == "application/smil")
		{
			*pbCancel = TRUE;
			string log_msg = "Loading smil file link: " + thisUrl.get_url();
			amis::util::Log::Instance()->writeMessage("Loading smil file via hyperlink", &thisUrl, 
				"CAmisHtmlView::OnBeforeNavigate2");
			TRACE(L"%s", "AMIS following HTML hyperlink\n");
			amis::dtb::DtbWithHooks::Instance()->loadSmilFromUrl(&thisUrl);
		}
		//else it's probably a text-file loading request that we sent ourselves
		else
		{
			CHtmlView::OnBeforeNavigate2(lpszURL, nFlags,	lpszTargetFrameName, baPostedData, lpszHeaders, pbCancel);
		}
	}
}
LPARAM CAmisHtmlView::OnHighlightUrlTarget(WPARAM wParam, LPARAM lParam)
{
	assert(wParam == 0);
	std::string *url = (std::string *)lParam;
	std::string newurl; // = *url;
	newurl.assign(*url);
	
	//make sure we aren't processing the same document
	ambulant::net::url amb_url = ambulant::net::url::from_url(newurl);
	if (!amb_url.same_document(TextRenderBrain::Instance()->getCurrentUrl()))
	{
		//the catch here: if you build AMIS without protected book support, you don't get
		//nice-looking transformed dtbook
#ifdef WITH_PROTECTED_BOOK_SUPPORT
		amis::UrlList* text_urls = amis::dtb::DtbWithHooks::Instance()->getFileSet()->getTextFiles();
		string url_no_target = amis::util::FilePathTools::clearTarget(*url);

		//The DTBook file was transformed in the background
		//it should be loaded if all of these are true: 
		//this file request is our DTBook file
		//this is a DAISY 2005 book
		//the DTBook transformation was successful
		bool should_load_transformed_dtbook = 
			amis::util::FilePathTools::urlListContains(url_no_target, text_urls)
			&& 
			(amis::dtb::DtbWithHooks::Instance()->getDaisyVersion() == amis::dtb::DAISY_2005)
			&&
			amis::dtb::DtbWithHooks::Instance()->wasDtbookTransformed();

		//Protected book files OR DTBook files require manual loading through IE
		if (amis::dtb::DtbWithHooks::Instance()->isProtected() || 
			should_load_transformed_dtbook)
		{
			// Also, we need to inform the PdtbPluggableProtocol about
			// our datasource factory. This is done with the PdtbBridge
			// object, which remembers the factory until disposed of.
			if (mpLoaderBridge == NULL) 
			{
				ambulant::net::datasource_factory *df = 
					MainWndParts::Instance()->mpMmView->getDatasourceFactory();
				assert(df);

				HRESULT hres = CoCreateInstance(CLSID_CPdtbBridge, 0, CLSCTX_INPROC_SERVER, IID_PdtbBridge, (void **)&mpLoaderBridge);

#ifdef _DEBUG
	assert(SUCCEEDED(hres));
#endif
				if (SUCCEEDED(hres)) 
				{
					mpLoaderBridge->SetDatasourceFactory((DATASOURCEFACTORYPTR)df);

					if (amis::dtb::DtbWithHooks::Instance()->getDaisyVersion() == amis::dtb::DAISY_2005)
					{
						mpLoaderBridge->SetDtbookProcessing(true);
					}
				}
			}
			
			// Prepend amisie: to the URL. This will change the protocol,
			// and the PdtbPlugin PluggableProtocol COM object has registered
			// that protocol. It will then be used to get the data, and it will
			// do the right thing wrt. decoding.
			if (newurl.substr(0, 7) != "amisie:")
				newurl = "amisie:" + newurl;
		} 
		else 
		{
			if (mpLoaderBridge) 
			{
				mpLoaderBridge->Release();
				mpLoaderBridge = NULL;
			}
		}
#endif
	}
	TextRenderBrain::Instance()->gotoUriTarget(newurl);
	delete url;
	amis::util::Log::Instance()->writeTrace("End function", "HtmlView::OnHighlightUrlTarget");
	return 0;
}

void
CAmisHtmlView::smilPlayerDeleted()
{
#ifdef WITH_PROTECTED_BOOK_SUPPORT
	if (mpLoaderBridge) 
	{
		mpLoaderBridge->Release();
		mpLoaderBridge = NULL;
	}
#endif
	//this->Navigate2(_T("about:blank"));
	if (theApp.isBookOpen() == false) TextRenderBrain::Instance()->gotoUriTarget("about:blank");
}


HRESULT CAmisHtmlView::OnFilterDataObject(LPDATAOBJECT pDataObject, LPDATAOBJECT* ppDataObject)
{
	ppDataObject = NULL;
	return S_FALSE;
}

HRESULT CAmisHtmlView::OnShowContextMenu(DWORD dwID, LPPOINT ppt,LPUNKNOWN pcmdtReserved, 
						  LPDISPATCH pdispReserved)
{
	//return S_FALSE;  // Enable the HtmlView popup menu
	return S_OK;	 // Disable the standard HtmlView popup menu
}

void CAmisHtmlView::OnEditCopy() 
{
	// XXXJack: this should intercept Copy, but it does not seem to work??
	ExecFormsCommand(IDM_COPY, NULL, NULL);
	return;
}

void CAmisHtmlView::OnNavigateComplete2(LPCTSTR strURL)
{
	CHtmlView::OnNavigateComplete2(strURL);
}
void CAmisHtmlView::OnNavigateError(
   LPCTSTR lpszURL,
   LPCTSTR lpszFrame,
   DWORD dwError,
   BOOL *pbCancel)
{
}

CString CAmisHtmlView::GetFullName() const
{
    ASSERT(m_pBrowserApp != NULL);

    BSTR bstr;
    m_pBrowserApp->get_FullName(&bstr);
    CString retVal(bstr);

    SysFreeString(bstr); // Added this line to prevent leak.
    return retVal;
}

CString CAmisHtmlView::GetType() const
{
    ASSERT(m_pBrowserApp != NULL);

    BSTR bstr;
    m_pBrowserApp->get_Type(&bstr);

    CString retVal(bstr);

    SysFreeString(bstr); // Added this line to prevent leak.
    return retVal;
}

CString CAmisHtmlView::GetLocationName() const
{
    ASSERT(m_pBrowserApp != NULL);

    BSTR bstr;
    m_pBrowserApp->get_LocationName(&bstr);
    CString retVal(bstr);

    SysFreeString(bstr); // Added this line to prevent leak.
    return retVal;
}

CString CAmisHtmlView::GetLocationURL() const
{
    ASSERT(m_pBrowserApp != NULL);

    BSTR bstr;
    m_pBrowserApp->get_LocationURL(&bstr);
    CString retVal(bstr);

    SysFreeString(bstr); // Added this line to prevent leak.
    return retVal;
}

void CAmisHtmlView::Navigate(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
	LPCTSTR lpszTargetFrameName /* = NULL */ ,
	LPCTSTR lpszHeaders /* = NULL */, LPVOID lpvPostData /* = NULL */,
	DWORD dwPostDataLen /* = 0 */)
{
    CString strURL(lpszURL);
    BSTR bstrURL = strURL.AllocSysString();

    COleSafeArray vPostData;
    if (lpvPostData != NULL)
    {
       if (dwPostDataLen == 0)
          dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);

       vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
    }

    m_pBrowserApp->Navigate(bstrURL, COleVariant((long) dwFlags, VT_I4), COleVariant(lpszTargetFrameName, VT_BSTR),
                            vPostData, COleVariant(lpszHeaders, VT_BSTR));

    SysFreeString(bstrURL); // Added this line to prevent leak.
}


BOOL CAmisHtmlView::LoadFromResource(LPCTSTR lpszResource)
{
    HINSTANCE hInstance = AfxGetResourceHandle();
    ASSERT(hInstance != NULL);

    CString strResourceURL;
    BOOL bRetVal = TRUE;
    LPTSTR lpszModule = new TCHAR[_MAX_PATH];

    if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
    {
       strResourceURL.Format(_T("res://%s/%s"), lpszModule, lpszResource);
       Navigate(strResourceURL, 0, 0, 0);
    }
    else 
		bRetVal = FALSE;

    delete [] lpszModule;
    return bRetVal;
}

IHTMLStyleSheet* CAmisHtmlView::applyStylesheet(const ambulant::net::url* stylesheet)
{
	USES_CONVERSION;

	if (stylesheet == NULL) return NULL;

	// First we need to get a pointer to the DOM
	IDispatch *pDisp = GetHtmlDocument();
	if (pDisp == NULL) return NULL;
	IHTMLDocument2 *pDoc;
	HRESULT res;
	res = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc);
	if (!SUCCEEDED(res)) return NULL;
	if (pDoc == NULL) return NULL;

	BSTR css_path_bstr;
	CString css_path_cstr = A2T(stylesheet->get_file().c_str());
	css_path_bstr = css_path_cstr.AllocSysString();
	
	string log_msg = "Applying stylesheet: " + stylesheet->get_url();
	amis::util::Log::Instance()->writeMessage("Applying stylesheet", stylesheet, "CAmisHtmlView::applyStylesheet");
	
	IHTMLStyleSheet* pStyleSheet;
	string log_msg2 = "About to call create stylesheet";
	amis::util::Log::Instance()->writeTrace(log_msg2, "CAmisHtmlView::applyStylesheet");
	
	res = pDoc->createStyleSheet(css_path_bstr, 0, &pStyleSheet);
	string log_msg3 = "Created stylesheet: done";
	amis::util::Log::Instance()->writeTrace(log_msg3, "CAmisHtmlView::applyStylesheet");
	SysFreeString(css_path_bstr);
	
	long len = stylesheetCount();
	
	forceResizeHack();

	string log_msg4 = "About to exit function";
	amis::util::Log::Instance()->writeTrace(log_msg4, "CAmisHtmlView::applyStylesheet");
	return pStyleSheet;
}

void CAmisHtmlView::removeStylesheet(IHTMLStyleSheet* pStyleSheet)
{
	if (!pStyleSheet) return;
	
	amis::util::Log::Instance()->writeMessage("Removing stylesheet", "CAmisHtmlView::removeStylesheet");
	
	pStyleSheet->put_disabled(VARIANT_TRUE);
	
#if 0
	// this should remove the stylesheet from the document... 
	// unfortunately, IHTMLStyleSheet doesn't seem to support the IHTMLDOMNode interface, though, so this code doesn't work
	// leaving this code here in case I figure it out someday
	IHTMLDOMNode* pNode;
	HRESULT hr = pStyleSheet->QueryInterface(IID_IHTMLDOMNode, (void**)&pNode);
	if (S_OK(hr))
	{
		IHTMLDOMNode* pParent;
		IHTMLDOMNode* pRemoved;
		if (pNode->get_parentNode(&pParent) == S_OK) {
			pParent->removeChild(pNode, pRemovedNode);
		}
		pParent->Release();
		pNode->Release();
		pRemovedNode->Release();
	}
#endif

	pStyleSheet->Release();
	pStyleSheet = NULL;
	
	forceResizeHack();
}

// returns the number of stylesheets currently applied to the document
// note that there shouldn't ever be more than 3 (amis.css, font, and page contrast)
long CAmisHtmlView::stylesheetCount()
{
	// First we need to get a pointer to the DOM
	IDispatch *pDisp = GetHtmlDocument();
	if (pDisp == NULL) return 0;
	IHTMLDocument2 *pDoc;
	HRESULT res;
	res = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc);
	if (!SUCCEEDED(res)) return 0;
	if (pDoc == NULL) return 0;

	IHTMLStyleSheetsCollection* pStyleSheetsCollection;
	pDoc->get_styleSheets(&pStyleSheetsCollection);
	long length;
	pStyleSheetsCollection->get_length(&length);
	return length;
}

BOOL CAmisHtmlView::LoadFromResource(UINT nRes)
{
    HINSTANCE hInstance = AfxGetResourceHandle();
    ASSERT(hInstance != NULL);

    CString strResourceURL;
    BOOL bRetVal = TRUE;
    LPTSTR lpszModule = new TCHAR[_MAX_PATH];

   if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
    {
       strResourceURL.Format(_T("res://%s/%d"), lpszModule, nRes);
       Navigate(strResourceURL, 0, 0, 0);
    }
    else
       bRetVal = FALSE;

    delete [] lpszModule;
    return bRetVal;
}

void amis::gui::CAmisHtmlView::OnSize(UINT nType, int cx, int cy)
{
	TRACE(L"%s", _T("CAmisHtmlView: OnSize\n"));
	CHtmlView::OnSize(nType, cx, cy);
}

//added this to help with stylesheet rendering
void amis::gui::CAmisHtmlView::forceResizeHack()
{
	if (MainWndParts::Instance()->mpMainFrame == NULL) return;

	//force a resize to avoid the gap between the CSS area and the HTML page margin
	
	CRect rect;
	//get the current window size
	MainWndParts::Instance()->mpMainFrame->GetWindowRect(&rect);
	//shrink the window a little bit
	//unfortunately, any value smaller than 9 for the offset results in no effect at all
	MainWndParts::Instance()->mpMainFrame->SetWindowPos(&wndTop, 0, 0, rect.Width()-9, rect.Height(), SWP_NOMOVE); 
	//set to the original size
	MainWndParts::Instance()->mpMainFrame->SetWindowPos(&wndTop, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE); 
}

CAmisHtmlView *html_browser_imp::s_browser = NULL;

html_browser_imp::html_browser_imp(int left, int top, int width, int height)
{
	RECT rect;
	rect.left = left;
	rect.top = top;
	rect.right = left + width;
	rect.bottom = top + height;

	
	if ( ! s_browser) 
	{
		// No browser yet. Create one with the right size and position.
		HtmlView* browser = new HtmlView(rect, topView);
		
		assert (browser != NULL);
		s_browser = browser;
	} else 
	{
		// The browser was created earlier. Move and resize it, if needed.
		s_browser->MoveWindow(&rect, 1);
	}
	//AM_DBG lib::logger::get_logger()->debug("html_browser_imp::html_browser_imp(0x%x): LTWH=(%d,%d,%d,%d) m_browser=0x%x", this, left, top, width, height, m_browser);
}

html_browser_imp::~html_browser_imp() 
{
	//AM_DBG lib::logger::get_logger()->debug("html_browser_imp::~html_browser_imp(0x%x)", this);
#if 0
	ShowWindow(s_browser->m_hWnd, SW_HIDE);
#else
	s_browser->PostMessage(WM_SHOWWINDOW, FALSE, 0);
#endif

//KB DestroyWindow(browser->m_hWnd);
//KB delete browser;
//KB s_browser = NULL;
}

void html_browser_imp::goto_url(std::string urlstr, ambulant::net::datasource_factory *df) 
{
	USES_CONVERSION;
	
	std::string *strcopy = new std::string(urlstr);
	MainWndParts::Instance()->mpHtmlView->PostMessage(WM_MY_HIGHLIGHTURITARGET, 0, (LPARAM)strcopy);
	amis::util::Log::Instance()->writeTrace("End function", "html_browser_imp::goto_url");
	//Sleep(1000);
}

void html_browser_imp::hide() 
{
//	AM_DBG lib::logger::get_logger()->debug("html_browser_imp::hide(0x%x)", this);
#if 0
	// JACK hack was to comment this line out: it causes a deadlock between the main
	// thread (holds some GUI lock, wants to kill event_processor thread) and the
	// event_processor (waits for GUI lock here, therefore cannot die).
	ShowWindow(s_browser->m_hWnd, SW_HIDE);
#else
	s_browser->PostMessage(WM_SHOWWINDOW, FALSE, 0);
#endif
	
}

void html_browser_imp::show() 
{
//	AM_DBG lib::logger::get_logger()->debug("html_browser_imp::show(0x%x)", this);
#if 0
	ShowWindow(s_browser->m_hWnd, SW_SHOW);
#else
	s_browser->PostMessage(WM_SHOWWINDOW, TRUE, 0);
#endif
}

void html_browser_imp::redraw() 
{
	TRACE(L"%s", _T("html_browser_imp: redraw\n"));

//	AM_DBG lib::logger::get_logger()->debug("html_browser_imp::redraw(0x%x)", this);
	ShowWindow(s_browser->m_hWnd, SW_SHOWNA);
}

// the "screen reader" could also be our own TTS renderer implementation
//the assumption is that if there is no audio in the book, something else will take over the voicing
bool html_browser_imp::uses_screen_reader()
{
	//if we are explicitly disabling audio rendering
	if (theApp.getShouldNotRenderAudio()) 
		return true;
	//if we can't find any audio to render
	else 
		return !amis::dtb::DtbWithHooks::Instance()->hasAudio();
}
