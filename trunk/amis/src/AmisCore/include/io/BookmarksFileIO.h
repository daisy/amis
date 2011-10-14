/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

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
#ifndef BOOKMARKSFILEIO_H
#define BOOKMARKSFILEIO_H

#include "dtb/Bookmarks.h"
#include "io/XercesSaxParseBase.h"

#include <string>
#include <vector>

#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

XERCES_CPP_NAMESPACE_USE

XERCES_CPP_NAMESPACE_BEGIN
class Attributes;
XERCES_CPP_NAMESPACE_END

namespace amis
{
namespace io
{
class BookmarksFileIO : public XercesSaxParseBase
{
public:
	BookmarksFileIO();
	~BookmarksFileIO();

	bool readFromFile(const ambulant::net::url*);
	bool writeToFile(const ambulant::net::url*, amis::dtb::BookmarkSet*);

	amis::dtb::BookmarkSet* getBookmarkSet();
	
	//SAX METHODS
	void startElement(const   XMLCh* const    uri,
		const   XMLCh* const    localname,
		const   XMLCh* const    qname,
		const   Attributes&	attributes);
	void endElement(const XMLCh* const uri,
		const XMLCh* const localname,
		const XMLCh* const qname);
	void characters(const XMLCh *const, const XMLSize_t);
	/*end of sax methods*/
private:
	void writeTitle(amis::MediaGroup*);
	void writeUid(string);
	void writeLastmark(amis::dtb::PositionData*);
	void writeHilite(amis::dtb::Hilite*);
	void writeBookmark(amis::dtb::Bookmark*);
	void writePositionData(amis::dtb::PositionData*, DOMElement*);
	void writeMediaGroup(amis::MediaGroup*, DOMElement*);
	void writeNote(amis::MediaGroup*, DOMElement*);
private:
	//useful while parsing
	wstring mTempWChars;
	string mTempChars;
	bool mb_flagGetChars;
	vector<string> mElementStack;
	xercesc_3_0::DOMDocument* mpDoc;
	//the title element
	amis::MediaGroup* mpTitle;
	//the current bookmark or hilite element
	amis::dtb::PositionMark* mpCurrentPosMark;
	//the current note element
	amis::MediaGroup* mpCurrentNote;
	//the current position data
	amis::dtb::PositionData* mpCurrentPosData;
	
	//the bookmarks file object model
	amis::dtb::BookmarkSet* mpBookmarks;

};
}
}

#endif
