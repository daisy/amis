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
#ifndef TEXTRENDERBRAIN_H
#define TEXTRENDERBRAIN_H

#include <string>
//#include <comutil.h>	//for _bstr_t_

#include "../../AmisCore/include/Media.h"

#include "mshtml.h"

#include "gui/HtmlView.h"

using namespace std;

namespace amis
{
namespace gui
{
class TextRenderBrain
{
protected: 
	TextRenderBrain();

public:
	static TextRenderBrain* Instance();
	void DestroyInstance();
	~TextRenderBrain();

	bool canIncreaseFontSize();
	bool canDecreaseFontSize();
	void resetFontSize();
	void increaseFontSize();
	void decreaseFontSize();

	void gotoUriTarget(amis::TextNode*);
	void gotoUriTarget(std::string url);
	
	void applyPageStyle(int);
	void clearPageStyle();
	void applyNextPageStyle();
	int getCurrentCustomStyleIndex();

	void redoPageColors();
	wstring getCurrentText();
	void webDocumentComplete();
	ambulant::net::url getCurrentUrl();
private:
	void saveElementText(IHTMLElement*);
	void setFontSize(int);
	void showElementAtId(string);
	void unHighlightPreviousElement();
	void setHighlightColors(IHTMLElement*);
	void redoHighlightColors();
	bool isElementInView(IHTMLElement*);
	IHTMLElement* GetElementFromId(string id, const GUID *iid);
	IHTMLDocument2* GetDoc();

	int mFontSize;
	bool mbWaitForDocumentLoad;
	ambulant::net::url mCurrentUrl;
	string mTextElmId;
	wstring mCurrentElmText;
	IHTMLElement* mpPreviousElm;
	VARIANT mUnhighlightedBG;
	VARIANT mUnhighlightedFG;
	IHTMLStyleSheet* mpFontCss;
	IHTMLStyleSheet* mpStyleCss;
	IHTMLStyleSheet* mpAmisCss;
	int mCurrentStyleIdx;
	bool mbStyleOn;

	//part of a workaround to stop highlighting of text nodes
	//this highlighting happens sometimes to nodes that should be skipped
	//the node_started/node_stopped events are over before the highlighting commands get sent
	//i don't know where they come from..
	//it only happens when we try to load the lastmark of a document (url#frag; ambulant not started playing yet)
	ambulant::net::url mTextSrcToWaitFor;
	
private:
	static TextRenderBrain* pinstance;

};
}
}
#endif