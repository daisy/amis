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
#ifndef TEXTSEARCH_H
#define TEXTSEARCH_H

#include <string>
#include <vector>
#include <xercesc/sax2/DefaultHandler.hpp>
#include "io/XercesSaxParseBase.h"

XERCES_CPP_NAMESPACE_USE

XERCES_CPP_NAMESPACE_BEGIN
class Attributes;
XERCES_CPP_NAMESPACE_END

using namespace std;

namespace amis
{
namespace io
{
class TextSearch : public XercesSaxParseBase
{
public:
	TextSearch(){}
	~TextSearch(){}
	
	//return the SMIL#frag for the text string
	//params: search string, text filename
	string searchForText(wstring, const ambulant::net::url*);
	//params: search string, starting id in text file, text filename
	string searchForTextForwardFromId(wstring, string, const ambulant::net::url*);
	//params: search string, starting id in text file, text filename
	string searchForTextBackwardsFromId(wstring, string, const ambulant::net::url*);
	
	//!xerces start element event
	void startElement(const   XMLCh* const    uri,
			const   XMLCh* const    localname,
			const   XMLCh* const    qname,
			const   Attributes&	attributes);
	//!xerces end element event
    void endElement(const XMLCh* const uri,
		const XMLCh* const localname,
		const XMLCh* const qname);
	void characters(const XMLCh *const chars, const unsigned int length);
private:
	string readFromFile(const ambulant::net::url*);
	wstring mSearchString;
	void searchInText();
	string mFound;
	string mId;
	string mBoundaryId;
	bool mbFlag_Analyze;
	bool mbFlag_GetChars;
	bool mbFlag_Boundary;
	int mDir;
	//we need stacks because elements may be nested
	vector<wstring> mCharDataStack;
	vector<string> mIdStack;
};
}
}
#endif
