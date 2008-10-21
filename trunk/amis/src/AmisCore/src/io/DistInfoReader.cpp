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

//xerces includes
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/HandlerBase.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
//end xerces includes

#include "util/FilePathTools.h"
#include "io/DistInfoReader.h"
#include "util/xercesutils.h"

#include <iostream>
#include <fstream>

/*
Dist info files:
http://www.daisy.org/z3986/2005/Z3986-2005.html#DistElem

Right now, this reader supports extracting a book list from the file.
Next will be to add support for multiple volumes and the associated messages.
*/

amis::io::DistInfoReader::DistInfoReader()
{
	mpBookList = NULL;
}

amis::io::DistInfoReader::~DistInfoReader()
{
}

amis::BookList* amis::io::DistInfoReader::getBookList()
{
	return mpBookList;
}
bool amis::io::DistInfoReader::readFromFile(const ambulant::net::url* filepath)
{
	mbFlagGetChars = false;
	mpBookList = NULL;
	mTempWChars.erase();
	mbFlagProcessingTitle = false;
	mpBookList = new amis::BookList();
	
	if (!this->parseFile(filepath))
	{
		delete mpBookList;
		return false;
	}
	else
	{
		return true;
	}
}

void amis::io::DistInfoReader::startElement(const   XMLCh* const    uri,
				const   XMLCh* const    localname,
				const   XMLCh* const    qname,
				const   Attributes&	attributes)
{
	char* element_name;
	element_name = XMLString::transcode(qname);

	if (strcmp(element_name, "distInfo") == 0)
	{
		mpBookList = new amis::BookList();
		mpBookList->setFilepath(this->getFilepath());
	}
	else if (strcmp(element_name, "book") == 0)
	{
		mpCurrentEntry = new amis::BookEntry();
		mpBookList->addEntry(mpCurrentEntry);

		wstring tmp_wstr;
		tmp_wstr.assign(SimpleAttrs::getw("uid", &attributes));
		mpCurrentEntry->mUid = tmp_wstr;

		string tmpstr;
		tmpstr.assign(SimpleAttrs::get("pkgRef", &attributes));
		ambulant::net::url filepath = ambulant::net::url::from_filename(tmpstr);
		mpCurrentEntry->mPath = filepath.join_to_base(*this->getFilepath());
	}
	else if (strcmp(element_name, "docTitle") == 0)
	{
		mbFlagProcessingTitle = true;
	}
	else if (strcmp(element_name, "text") == 0)
	{
		if (mbFlagProcessingTitle == true) mbFlagGetChars = true;
	}
	else if (strcmp(element_name, "audio") == 0)
	{
		string src;
		string clipbegin;
		string clipend;
		src.assign(SimpleAttrs::get("src", &attributes));
		ambulant::net::url url_src = ambulant::net::url::from_filename(src);
		url_src = url_src.join_to_base(*this->getFilepath());
		src.assign(url_src.get_file());
		
		clipbegin.assign(SimpleAttrs::get("clipBegin", &attributes));
		clipend.assign(SimpleAttrs::get("clipEnd", &attributes));
		if (mpCurrentEntry != NULL) mpCurrentEntry->setTitleAudio(src, clipbegin, clipend);
	}
	
	XMLString::release(&element_name);
}
void amis::io::DistInfoReader::endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
{
	char* element_name = XMLString::transcode(qname);

	if (strcmp(element_name, "text") == 0)
	{
		if (mbFlagProcessingTitle == true &&
			mpCurrentEntry != NULL)
			this->mpCurrentEntry->setTitleText(mTempWChars);
		mbFlagGetChars = false;
	}
	else if (strcmp(element_name, "docTitle") == 0) 
	{
		mbFlagProcessingTitle = false;
	}
	mTempWChars.erase();

	XMLString::release(&element_name);
}
void amis::io::DistInfoReader::characters(const XMLCh *const chars, const unsigned int length)
{
	if (mbFlagGetChars == true) mTempWChars.append((wchar_t*)chars);
}
