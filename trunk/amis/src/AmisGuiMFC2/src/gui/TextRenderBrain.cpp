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
#include "afxmt.h"

#include "Preferences.h"
#include "gui/TextRenderBrain.h"
#include "gui/MainWndParts.h"
#include "util/FilePathTools.h"
#include "gui/AmisApp.h"
#include "util/Color.h"
#include "util/Log.h"
#include <algorithm>
#include "DtbWithHooks.h"
#include "dtb\Dtb.h"
#include <fstream>
#include "../resource.h"

using namespace amis::gui;

TextRenderBrain* TextRenderBrain::pinstance = 0;

TextRenderBrain* TextRenderBrain::Instance()
{
	if (pinstance == 0)  pinstance = new TextRenderBrain;
    return pinstance;
}

void TextRenderBrain::DestroyInstance()
{
	delete pinstance;
}

TextRenderBrain::TextRenderBrain()
{
	mFontSize = 0;
	mbWaitForDocumentLoad = false;
	mpPreviousElm = NULL;
	mbStyleOn = false;
	mCurrentStyleIdx = -1;
	VariantInit(&mUnhighlightedFG);
	VariantInit(&mUnhighlightedBG);

	mCurrentUrl = ambulant::net::url();

	mpFontCss = NULL;
	mpStyleCss = NULL;
	mpAmisCss = NULL;
}

TextRenderBrain::~TextRenderBrain()
{
	//TODO: should delete variants
}

void TextRenderBrain::gotoUriTarget(amis::TextNode* pText)
{
	gotoUriTarget(pText->getSrc());
}

//TODO: make sure this path comparison works for Japanese too
void TextRenderBrain::gotoUriTarget(std::string urlstr)
 {
	USES_CONVERSION;
	CString cstr_url = A2T(urlstr.c_str());
	CString msg;
	
	ambulant::net::url url = ambulant::net::url::from_url(urlstr);
	string text_elm_id = url.get_ref();
	
	//make sure our document still exists; otherwise force a reload by setting mCurrentUrl to ""
	if (MainWndParts::Instance()->mpHtmlView->GetHtmlDocument() == NULL) 
	{
		this->mCurrentUrl = ambulant::net::url();
		mTextElmId = "";
	}

	bool is_same_doc = url.same_document(mCurrentUrl);
	// If it's identical ignore it (the previous call may still be waiting for the webDocumentComplete callback)
	if (is_same_doc && text_elm_id == mTextElmId) 
	{
		msg.Format(_T("^^^^^^^^^^ gotoUriTarget(%s): same document, same element\n"), cstr_url);
		TRACE(_T("%s"), msg);
		return;
	}
	
	// If it's a different document: load it.
	if (!is_same_doc)
	{	
		msg.Format(_T("^^^^^^^^^^ gotoUriTarget(%s): Loading HTML document\n"), cstr_url);
		TRACE(_T("%s"), msg);
		//save the current ID
		mTextElmId = text_elm_id;

		string temp_url = url.get_url().c_str();
		temp_url = amis::util::FilePathTools::clearTarget(temp_url);
		CString load_url(temp_url.c_str());
		mCurrentUrl = url.get_document();

		if (mbWaitForDocumentLoad) TRACE(_T("Warning: already waiting for a document to load\n"));

		mbWaitForDocumentLoad = true;
		MainWndParts::Instance()->mpHtmlView->Navigate2(load_url, NULL, NULL);
		//now wait for webDocumentComplete
	}
	else
	{	// If it's not a different document: scroll and highlight only.
		//however, we could still be waiting for the document to load
		//this is a weird case but it does happen -- the renderer receives two requests for different
		//nodes in the same document.  it tries to load the second while still waiting for the documentcomplete
		//event from the first.
		if (mbWaitForDocumentLoad == true)
		{
			msg.Format(_T("^^^^^^^^^^ gotoUriTarget(%s): still waiting for the document to load\n"), cstr_url);
			TRACE(_T("%s"), msg);
			//just update the ID and continue to wait
			this->mTextElmId = text_elm_id;
		}
		else
		{
			msg.Format(_T("^^^^^^^^^^ gotoUriTarget(%s): document already loaded\n"), cstr_url);
			TRACE(_T("%s"), msg);
			//if we are already at this element, don't highlight it
			//sometimes multiple events happen for a single text element
			//like:
			//<par>
			//<text id ="abc"/>
			//<seq>
			//	<audio src="clip1.wav"/>
			//	<audio src="clip2.wav"/>
			//</seq></par>
			if (text_elm_id == mTextElmId) return;
			//save the current ID
			mTextElmId = text_elm_id;
			showElementAtId(mTextElmId);
		}
	}
}


void TextRenderBrain::webDocumentComplete()
{
	if (mbWaitForDocumentLoad)
	{
		//this is the user's css file
		mpAmisCss = MainWndParts::Instance()->mpHtmlView->applyStylesheet
			(Preferences::Instance()->getAmisCssFile());
		
		MainWndParts::Instance()->mpHtmlView->RedrawWindow();
		MainWndParts::Instance()->mpMainFrame->RecalcLayout();
		mbWaitForDocumentLoad = false;

		//refresh font size and contrast settings for the newly loaded page
		if (mFontSize > 0) setFontSize(mFontSize);
		if (mbStyleOn) applyPageStyle(mCurrentStyleIdx);

		showElementAtId(mTextElmId);
	}
}

void TextRenderBrain::saveElementText(IHTMLElement* pElm)
{
	USES_CONVERSION;

	if (!pElm) return;

	BSTR tag_name;
	pElm->get_tagName(&tag_name);
	string str_tag_name = OLE2A(tag_name);
	
	//convert the string to lower case before doing a comparison
	std::transform(str_tag_name.begin(), str_tag_name.end(), str_tag_name.begin(), (int(*)(int))tolower);
	
	if (str_tag_name.compare("img") == 0)
	{
		const char* c_attr_alt = "alt";
		BSTR attr_alt;
		attr_alt = A2BSTR(c_attr_alt);
		VARIANT attr_val;
		VariantInit(&attr_val);
		attr_val.vt = VT_BSTR;

		pElm->getAttribute(attr_alt, 0, &attr_val);

		CString temp_cstr(OLE2CW(attr_val.bstrVal));
		LPCTSTR tmp;
		tmp = (LPCTSTR)temp_cstr;
		mCurrentElmText.assign ((LPCTSTR)temp_cstr);
		SysFreeString(attr_val.bstrVal);
	}
	else
	{
		//save the element inner text
		BSTR inner_text;
		pElm->get_innerText(&inner_text);

		CString temp_cstr(OLE2CW(inner_text));
		LPCTSTR tmp;
		tmp = (LPCTSTR)temp_cstr;
		mCurrentElmText.assign ((LPCTSTR)temp_cstr);
		SysFreeString(inner_text);
	}
}

//put the element in view and optionally highlight it
void TextRenderBrain::showElementAtId(string elmId)
{
	USES_CONVERSION;
	CString cstr_elm_id = A2T(elmId.c_str());
	CString msg;
	//get the previous (currently highlighted) text element ID
	string prev_text_elm_id = "";
	if (mpPreviousElm)
	{
		const char* c_attr_id = "id";
		BSTR attr_id;
		attr_id = A2BSTR(c_attr_id);
		VARIANT attr_val;
		VariantInit(&attr_val);
		attr_val.vt = VT_BSTR;
		mpPreviousElm->getAttribute(attr_id, 0, &attr_val);
		CString temp_cstr(OLE2CW(attr_val.bstrVal));
		LPCTSTR tmp;
		tmp = (LPCTSTR)temp_cstr;
		prev_text_elm_id = T2A(tmp);
		SysFreeString(attr_val.bstrVal);
	}
	else
	{
		msg.Format(_T("^^^^^^^^^^ showElementAtId(%s): No previous element.\n"), cstr_elm_id);
		TRACE(_T("%s"), msg);
	}
	if (prev_text_elm_id == mTextElmId)
	{
		msg.Format(_T("^^^^^^^^^^ showElementAtId(%s): Warning: we should have already highlighted this element.\n"), cstr_elm_id);
		TRACE(_T("%s"), msg);
	}
	else
	{
		msg.Format(_T("^^^^^^^^^^ showElementAtId(%s): This is a new element that I have never seen before in my life.\n"), cstr_elm_id);
		TRACE(_T("%s"), msg);
	}
	
	if (elmId.size() == 0) return; 
	IHTMLElement* p_elm = NULL;
	p_elm = GetElementFromId(elmId, 0);
	if (p_elm == NULL) return;

	//save the element text
	//this is just a convenient place for this function call, since it minimizes DOM searching	
	saveElementText(p_elm);

	//unhighlight the previous element
	unHighlightPreviousElement();
	//set background/foreground colors for the element
	if (Preferences::Instance()->getHighlightText() == true) setHighlightColors(p_elm);

	//scroll into view
	if (isElementInView(p_elm) == false)
	{
		VARIANT v_bool;
		v_bool.vt = VT_BOOL;
		v_bool.boolVal = TRUE;

		p_elm->scrollIntoView(v_bool);
	}
	mpPreviousElm = p_elm;
	
}
//reset the highlight from the previously highlighted element
void TextRenderBrain::unHighlightPreviousElement()
{
	if (mpPreviousElm != NULL && mUnhighlightedBG.vt != VT_EMPTY && mUnhighlightedFG.vt != VT_EMPTY)
	{
		VARIANT copy_last_bg;
		VARIANT copy_last_fg;
		VariantInit(&copy_last_bg);
		VariantInit(&copy_last_fg);
		copy_last_bg.vt = VT_BSTR;
		copy_last_fg.vt = VT_BSTR;
		copy_last_bg.bstrVal = mUnhighlightedBG.bstrVal;
		copy_last_fg.bstrVal = mUnhighlightedFG.bstrVal;

		IHTMLStyle* p_last_style = NULL;
		mpPreviousElm->get_style(&p_last_style);
		//set the properties
		p_last_style->put_backgroundColor(copy_last_bg);
		p_last_style->put_color(copy_last_fg);
	}
}
//set the highlight colors on an element
void TextRenderBrain::setHighlightColors(IHTMLElement* pElm)
{
	USES_CONVERSION;
	
	if (pElm == NULL) return;

	IHTMLStyle* p_style = NULL;
	//save the current style info before changing it
	pElm->get_style(&p_style);
	p_style->get_backgroundColor(&mUnhighlightedBG);
	p_style->get_color(&mUnhighlightedFG);
	
	//variables for the highlight colors
	string str_text_fg, str_text_bg;
	VARIANT var_bg;
	VARIANT var_fg;

	VariantInit(&var_bg);
	VariantInit(&var_fg);
		
	//get as strings from the preferences for highlight values
	str_text_fg = amis::Preferences::Instance()->getHighlightFGColor().getAsHtmlHexColor();
	str_text_bg = amis::Preferences::Instance()->getHighlightBGColor().getAsHtmlHexColor();
		
	//convert to chars
	const char* textfg_color = str_text_fg.c_str();
	const char* textbg_color = str_text_bg.c_str();

	//convert to bstr/variant 
	var_fg.vt = VT_BSTR;
	var_fg.bstrVal = A2BSTR(textfg_color);
	var_bg.vt = VT_BSTR;
	var_bg.bstrVal = A2BSTR(textbg_color);

	//set the properties
	//this puts the preferred highlighting colors into pStyle
	p_style->put_backgroundColor(var_bg);
	p_style->put_color(var_fg);
}
int TextRenderBrain::getCurrentCustomStyleIndex()
{
	return mCurrentStyleIdx;
}
void TextRenderBrain::applyPageStyle(int idx)
{
	clearPageStyle();
	amis::util::Log::Instance()->writeMessage("Applying custom style", "TextRenderBrain::applyPageStyle", "AmisGuiMFC2");
	mbStyleOn = true;
	amis::UrlList* list = Preferences::Instance()->getCustomCssFiles();
	ambulant::net::url url = (*list)[idx];
	mCurrentStyleIdx = idx;
	mpStyleCss = MainWndParts::Instance()->mpHtmlView->applyStylesheet(&url);
}

void TextRenderBrain::applyNextPageStyle()
{
	amis::UrlList* p_list = Preferences::Instance()->getCustomCssFiles();
	int sz = p_list->size();
	if (mCurrentStyleIdx >= -1 && 
		mCurrentStyleIdx < sz-1)
	{
		int tmp = mCurrentStyleIdx;
		clearPageStyle();
		mCurrentStyleIdx = tmp + 1;
		applyPageStyle(mCurrentStyleIdx);
	}
	else
	{
		clearPageStyle();
	}
}

void TextRenderBrain::clearPageStyle()
{
	amis::util::Log::Instance()->writeMessage("Clearing page style", "TextRenderBrain::clearPageStyle", "AmisGuiMFC2");
	mbStyleOn = false;
	mCurrentStyleIdx = -1;
	MainWndParts::Instance()->mpHtmlView->removeStylesheet(mpStyleCss);
}

void TextRenderBrain::increaseFontSize()
{
	if (canIncreaseFontSize())
	{
		amis::util::Log::Instance()->writeMessage("Font size increased", "TextRenderBrain::increaseFontSize", "AmisGuiMFC2");
		setFontSize(mFontSize + 1);
	}
}
void TextRenderBrain::decreaseFontSize()
{
	if (canDecreaseFontSize())
	{
		amis::util::Log::Instance()->writeMessage("Font size decreased", "TextRenderBrain::decreaseFontSize", "AmisGuiMFC2");
		setFontSize(mFontSize - 1);
	}
}
bool TextRenderBrain::canIncreaseFontSize()
{
	if (mFontSize < Preferences::Instance()->getFontsizeCssFiles()->size())
		return true;
	else
		return false;
}
bool TextRenderBrain::canDecreaseFontSize()
{
	if (mFontSize > 0)
		return true;
	else
		return false;
}
void TextRenderBrain::resetFontSize()
{
	amis::util::Log::Instance()->writeMessage("Font size reset", "TextRenderBrain::resetFontSize", "AmisGuiMFC2");
	setFontSize(0);
}
//set the font size
//0 = reset
//1...n = index into a file in the css/font directory (presumably the files are in alpha order and each
//represents a larger size)
void TextRenderBrain::setFontSize(int fontsz)
{
	if (fontsz > Preferences::Instance()->getFontsizeCssFiles()->size() || fontsz < 1) fontsz = 0;
	mFontSize = fontsz;

	//remove the old stylesheet
	MainWndParts::Instance()->mpHtmlView->removeStylesheet(mpFontCss);
	
	//apply the size change to the sidebar
	double d_fontsz = fontsz;
	double d_max = Preferences::Instance()->getFontsizeCssFiles()->size();
	double pct = d_fontsz/d_max;
	MainWndParts::Instance()->mpSidebar->m_wndDlg.setFontSize(pct);

	ambulant::net::url* css_url = NULL;
	if (fontsz > 0 && fontsz <= Preferences::Instance()->getFontsizeCssFiles()->size())
	{
		css_url = &(*Preferences::Instance()->getFontsizeCssFiles())[fontsz - 1];
		mpFontCss = MainWndParts::Instance()->mpHtmlView->applyStylesheet(css_url);
	}
	else
	{
		mpFontCss = NULL;
	}
	if (isElementInView(mpPreviousElm) == false)
	{
		VARIANT v_bool;
		v_bool.vt = VT_BOOL;
		v_bool.boolVal = TRUE;
		mpPreviousElm->scrollIntoView(v_bool);		
	}	
}


//taken from http://www.wischik.com/lu/programmer/webform.html
IHTMLElement* TextRenderBrain::GetElementFromId(string id_str, const GUID *iid)
{
	const char* id = NULL;
	IHTMLElement *ret=0; 
	IHTMLElementCollection* doc_all = NULL;
	IHTMLDocument2 *doc = NULL; 
	HRESULT hr;

	if (id_str.size() == 0) return 0;
	
	id = id_str.c_str();
	doc = GetDoc();

	if (doc==0) return 0;
	hr = doc->get_all(&doc_all);      // this is like doing document.all
  
	if (hr == S_OK)
	{ 
		IDispatch* disp = NULL;
		VARIANT vid;
		VARIANT v0; 
		unsigned int len;
		wchar_t* ws = NULL;

		VariantInit(&v0);
		len=strlen(id);
		vid.vt=VT_BSTR;
    
		ws = new wchar_t[len+1];
		MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED, id, -1, ws, len+1);
		vid.bstrVal=ws;

		hr = doc_all->item(vid,v0,&disp);       // this is like doing document.all["messages"]
		delete[] ws;
    
		if (hr == S_OK && disp != 0)
		{ 
			IHTMLElement* element = NULL;

			if (iid == 0) hr = disp->QueryInterface(IID_IHTMLElement,(void **)&element);
			else hr = disp->QueryInterface(*iid,(void**)&element);
      
			if (hr == S_OK) ret = element; // it's the caller's responsibility to release it
			disp->Release();
		}
		doc_all->Release();
	}
	doc->Release();
	return ret;
}

//taken and modified slightly from http://www.wischik.com/lu/programmer/webform.html
IHTMLDocument2* TextRenderBrain::GetDoc()
{ 
	IDispatch *dispatch = NULL;  
	dispatch = MainWndParts::Instance()->mpHtmlView->GetHtmlDocument();
	IHTMLDocument2 *doc = NULL;  

	if (dispatch)
	{
		dispatch->QueryInterface(IID_IHTMLDocument2,(void**)&doc);
		dispatch->Release();
	}
	return doc;
}

wstring TextRenderBrain::getCurrentText()
{
	return mCurrentElmText;
}
ambulant::net::url TextRenderBrain::getCurrentUrl()
{
	return mCurrentUrl.add_fragment(mTextElmId);
}

void TextRenderBrain::redoPageColors()
{
	if (Preferences::Instance()->getHighlightText() == true)
		redoHighlightColors();
	else
		unHighlightPreviousElement();
}
//this is called to refresh the page colors
//the colors might have changed but the element cursor hasn't moved yet
//so without a call to this function, the old color settings would persist
void TextRenderBrain::redoHighlightColors()
{
	USES_CONVERSION;
	IHTMLStyle* pStyle = NULL;
	if (mpPreviousElm != NULL)
	{		
		//get a pointer to the style
		mpPreviousElm->get_style(&pStyle);
				
		//variables for the colors
		string str_text_fg, str_text_bg;
		VARIANT var_bg;
		VARIANT var_fg;
		VariantInit(&var_bg);
		VariantInit(&var_fg);
		
		//get as strings
		str_text_fg = amis::Preferences::Instance()->getHighlightFGColor().getAsHtmlHexColor();
		str_text_bg = amis::Preferences::Instance()->getHighlightBGColor().getAsHtmlHexColor();
		
		//convert to chars
		const char* textfg_color = str_text_fg.c_str();
		const char* textbg_color = str_text_bg.c_str();

		//convert to bstr/variant 
		var_fg.vt = VT_BSTR;
		var_fg.bstrVal = A2BSTR(textfg_color);
		var_bg.vt = VT_BSTR;
		var_bg.bstrVal = A2BSTR(textbg_color);

		//set the properties
		pStyle->put_backgroundColor(var_bg);
		pStyle->put_color(var_fg);
	}
}

//this function looks at the element's coordinates and the coordinates of the section of the
//document which is being displayed, and decides if the element is visible or not
bool TextRenderBrain::isElementInView(IHTMLElement* pElm)
{
	if (pElm == NULL) return false;

	//the IHTMLELEMENT measurements
	long elm_width = 0;
	long elm_height = 0;
	long elm_top = 0;
	long elm_left = 0;

	//if any measurements go wrong, return false
	if (pElm->get_offsetWidth(&elm_width) != S_OK) return false;
	if (pElm->get_offsetHeight(&elm_height) != S_OK) return false;
	if (pElm->get_offsetTop(&elm_top) != S_OK) return false;
	if (pElm->get_offsetLeft(&elm_left) != S_OK) return false;

	HRESULT hr;

	// get the document dispatch from browser
	IDispatch *pDisp = MainWndParts::Instance()->mpHtmlView->GetHtmlDocument();
	ASSERT( pDisp ); //if NULL, we failed

	IHTMLDocument2 *pDocument = NULL;
	hr = pDisp->QueryInterface( IID_IHTMLDocument2, (void**)&pDocument );
	ASSERT( SUCCEEDED( hr ) );
	ASSERT( pDocument );

	IHTMLElement *pBody = NULL;
	hr = pDocument->get_body( &pBody );
	ASSERT( SUCCEEDED( hr ) );
	ASSERT( pBody );

	// Attempt by Jack, following
	// <http://groups.google.com/group/microsoft.public.inetsdk.programming.webbrowser_ctl/browse_frm/thread/fa3452e1bbb6bcf5>
	// Depending on whether the HTML document has a doctype the scroll position can be stored
	// in one of two locations, we add the two to get what we want.
	IHTMLDocument3 *pDoc3 = NULL;
	IHTMLElement *pElem = NULL;
	IHTMLElement2 *pElem2 = NULL;
	hr = pDocument->QueryInterface(IID_IHTMLDocument3,(void**)&pDoc3);
	if (SUCCEEDED(hr)) 
	{
		hr = pDoc3->get_documentElement(&pElem);
		if (SUCCEEDED(hr)) 
		{
			hr = pElem->QueryInterface(IID_IHTMLElement2, (void**)&pElem2);
		}
	}
	IHTMLTextContainer *pContainer = NULL;
	hr = pBody->QueryInterface(IID_IHTMLTextContainer,(void**)&pContainer);
	ASSERT(SUCCEEDED(hr));
	ASSERT( pContainer );

	CPoint scroll_pos, scroll_pos2;
	pContainer->get_scrollTop(&scroll_pos.y);
	pContainer->get_scrollLeft(&scroll_pos.x);
	if (pElem2) 
	{
		pElem2->get_scrollTop(&scroll_pos2.y);
		pElem2->get_scrollLeft(&scroll_pos2.x);
		scroll_pos.y += scroll_pos2.y;
		scroll_pos.x += scroll_pos2.x;
	}

	if (pElem2) pElem2->Release();
	if (pElem) pElem->Release();
	if (pDoc3) pDoc3->Release();
	pContainer->Release();
	pBody->Release();
	pDocument->Release();
	pDisp->Release();

	//get the html view's width and height
	long view_width = 0;
	long view_height = 0;
	view_width = MainWndParts::Instance()->mpHtmlView->GetWidth();
	view_height = MainWndParts::Instance()->mpHtmlView->GetHeight();

	int top_visible = scroll_pos.y;
	int bottom_visible = scroll_pos.y + view_height;
	int left_visible = scroll_pos.x;
	int right_visible = scroll_pos.x + view_width;

	int elm_bottom = elm_top + elm_height;
	int elm_right = elm_left + elm_width;

/*	CString element_data;
	element_data.Format(_T("*****\nElement \ttop = %d \tbottom = %d \tleft = %d \tright = %d \n****\n"), 
		elm_top, elm_bottom, elm_left, elm_right);
	CString screen_data;
	screen_data.Format(_T("******\nScreen \ttop = %d \tbottom = %d \tleft = %d \tright = %d \n****\n"), 
		top_visible, bottom_visible, left_visible, right_visible);
	TRACE(screen_data);
	TRACE(element_data);	
*/
	if (elm_top < top_visible || elm_bottom > bottom_visible) return false;
	if (elm_left < left_visible || elm_right > right_visible) return false;

	//otherwise we assume it's in view
	return true;
}

