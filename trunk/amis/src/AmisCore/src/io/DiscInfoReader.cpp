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
#include "io/DiscInfoReader.h"
#include "util/xercesutils.h"
#include "io/TitleAuthorParse.h"
#include "Media.h"

#include <iostream>
#include <fstream>

/*
Disc info files are XHTML with this in the body:
<body>
    <a href="./book1/ncc.html">Economics</a>
    <a href="./book2/ncc.html">Ecology</a>
</body>
*/

amis::io::DiscInfoReader::DiscInfoReader()
{
	mpBookList = NULL;
	
}

amis::io::DiscInfoReader::~DiscInfoReader()
{
}

amis::BookList* amis::io::DiscInfoReader::getBookList()
{
	return mpBookList;
}
bool amis::io::DiscInfoReader::readFromFile(const ambulant::net::url* filepath)
{
	mbFlagGetChars = false;
	mpBookList = NULL;
	mTempWChars.erase();

	mpBookList = new amis::BookList();
	
	if (!this->parseFile(filepath))
	{
		delete mpBookList;
		return false;
	}
	else
	{
		resolveAudio();
		return true;
	}
}

void amis::io::DiscInfoReader::startElement(const   XMLCh* const    uri,
				const   XMLCh* const    localname,
				const   XMLCh* const    qname,
				const   Attributes&	attributes)
{
	char* element_name;
	element_name = XMLString::transcode(qname);

	if (strcmp(element_name, "a") == 0)
	{
		mpCurrentEntry = new amis::BookEntry();
		mpBookList->addEntry(mpCurrentEntry);
		string tmpstr;
		tmpstr.assign(SimpleAttrs::get("href", &attributes));
		ambulant::net::url filepath = ambulant::net::url::from_filename(tmpstr);
		mpCurrentEntry->mPath = filepath.join_to_base(*this->getFilepath());
		mbFlagGetChars = true;
	}
	
	XMLString::release(&element_name);
}
void amis::io::DiscInfoReader::endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
{
	char* element_name = XMLString::transcode(qname);

	if (strcmp(element_name, "a") == 0)
	{
		this->mpCurrentEntry->setTitleText(mTempWChars);
		mbFlagGetChars = false;
	}
	mTempWChars.erase();

	XMLString::release(&element_name);
}
void amis::io::DiscInfoReader::characters(const XMLCh* const chars, const XMLSize_t length)
{
	if (mbFlagGetChars == true) mTempWChars.append((wchar_t*)chars);
}
void amis::io::DiscInfoReader::resolveAudio()
{
	amis::io::TitleAuthorParse title_parser;

	for (int i=0; i<mpBookList->getNumberOfEntries(); i++)
	{
		amis::BookEntry* p_entry = mpBookList->getEntry(i);
		title_parser.readFromFile(&p_entry->mPath);
		amis::MediaGroup* p_title_media = NULL;
		p_title_media = title_parser.getTitleInfo();
		if (p_title_media != NULL)
		{
			if (p_title_media->hasAudio())
			{
				amis::AudioNode* p_aud = p_title_media->getAudio(0);
				p_entry->setTitleAudio(p_aud->getPath(), p_aud->getClipBegin(), p_aud->getClipEnd());
			}
			delete p_title_media;
		}
	}
}