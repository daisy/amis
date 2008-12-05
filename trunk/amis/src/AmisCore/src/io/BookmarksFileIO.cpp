/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2008  DAISY Consortium

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

#include "io/BookmarksFileIO.h"
#include "util/FilePathTools.h"
#include "util/xercesutils.h"
#include <iostream>

#if defined(AMIS_PLATFORM_WINDOWS)
//for creating directories on windows
#include "io.h"
#include "direct.h"
#endif

//XERCES INCLUDES
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/HandlerBase.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>

using namespace std;

amis::io::BookmarksFileIO::BookmarksFileIO()
{
	this->mpBookmarks = NULL;
	this->mpCurrentNote = NULL;
	this->mpCurrentPosData = NULL;
	this->mpDoc = NULL;
	this->mpTitle = NULL;
	this->mTempChars.erase();
	this->mTempWChars.erase();
}

amis::io::BookmarksFileIO::~BookmarksFileIO()
{
}

amis::dtb::BookmarkSet* amis::io::BookmarksFileIO::getBookmarkSet()
{
	return mpBookmarks;
}

bool amis::io::BookmarksFileIO::readFromFile(const ambulant::net::url* filepath)
{
	mpBookmarks = new amis::dtb::BookmarkSet();
	mb_flagGetChars = false;
	
	this->mpCurrentNote = NULL;
	this->mpCurrentPosData = NULL;
	this->mpDoc = NULL;
	this->mpTitle = NULL;
	this->mTempChars.erase();
	this->mTempWChars.erase();

	mElementStack.clear();

	if (!this->parseFile(filepath))
	{
		delete mpBookmarks;
		return false;
	}
	else
	{
		mpBookmarks->setFilepath(&this->mFilepath);
		return true;
	}
}


//--------------------------------------------------
//xerces start element event
//--------------------------------------------------
void amis::io::BookmarksFileIO::startElement(const   XMLCh* const    uri,
				const   XMLCh* const    localname,
				const   XMLCh* const    qname,
				const   Attributes&	attributes)
{
	char* element_name;
	element_name = XMLString::transcode(qname);

	if (strcmp(element_name, "title") == 0)
	{
		mpTitle = new amis::MediaGroup();
	}
	else if(strcmp(element_name, "text") == 0)
	{
		//if we are processing a title or note
		if (mElementStack.back().compare("title") == 0)
		{
			mb_flagGetChars = true;
			amis::TextNode* p_text = new amis::TextNode();
			mpTitle->setText(p_text);
		}
		else if(mElementStack.back().compare("note") == 0)
		{
			mb_flagGetChars = true;
			amis::TextNode* p_text = new amis::TextNode();
			mpCurrentNote->setText(p_text);
		}
	}
	else if(strcmp(element_name, "audio") == 0)
	{
		//if we are processing a title or a note
		if (mElementStack.back().compare("title") == 0 ||
			mElementStack.back().compare("note") == 0)
		{
			amis::AudioNode* p_audio = new amis::AudioNode();
			string src;
			string clipBegin;
			string clipEnd;

			src.assign(SimpleAttrs::get("src", &attributes));
			p_audio->setPath(src);

			clipBegin.assign(SimpleAttrs::get("clipBegin", &attributes));
			p_audio->setClipBegin(clipBegin);

			clipEnd.assign(SimpleAttrs::get("clipEnd", &attributes));
			p_audio->setClipEnd(clipEnd);

			if (mElementStack.back().compare("title") == 0)
				mpTitle->addAudioClip(p_audio);
			else if (mElementStack.back().compare("note") == 0)
				mpCurrentNote->addAudioClip(p_audio);
		}
	}
	else if(strcmp(element_name, "uid") == 0)
	{
		mb_flagGetChars = true;
	}
	else if(strcmp(element_name, "lastmark") == 0)
	{
		mpCurrentPosData = new amis::dtb::PositionData;
		mpCurrentPosData->mbHasCharOffset = false;
		mpCurrentPosData->mbHasTimeOffset = false;
	}

	else if(strcmp(element_name, "ncxRef") == 0)
	{
		mb_flagGetChars = true;
	}
	else if(strcmp(element_name, "uri") == 0)
	{
		mb_flagGetChars = true;
	}
	else if(strcmp(element_name, "timeOffset") == 0)
	{
		mb_flagGetChars = true;
	}

	else if(strcmp(element_name, "charOffset") == 0)
	{
		mb_flagGetChars = true;
	}

	else if(strcmp(element_name, "note") == 0)
	{
		mpCurrentNote = new amis::MediaGroup();
	}
	else if (strcmp(element_name, "bookmark") == 0)
	{
		mpCurrentPosMark = new amis::dtb::Bookmark;
		mpCurrentPosMark->mbHasNote = false;
		mpCurrentPosData = new amis::dtb::PositionData;
		mpCurrentPosData->mbHasCharOffset = false;
		mpCurrentPosData->mbHasTimeOffset = false;
		
	}
	else if(strcmp(element_name, "hilite") == 0)
	{
		mpCurrentPosMark = new amis::dtb::Hilite;
		mpCurrentPosMark->mbHasNote = false;
	}
	else if(strcmp(element_name, "hiliteStart") == 0)
	{
		mpCurrentPosData = new amis::dtb::PositionData;
		mpCurrentPosData->mbHasCharOffset = false;
		mpCurrentPosData->mbHasTimeOffset = false;
	}
	else if(strcmp(element_name, "hiliteEnd") == 0)
	{
		mpCurrentPosData = new amis::dtb::PositionData;
		mpCurrentPosData->mbHasCharOffset = false;
		mpCurrentPosData->mbHasTimeOffset = false;
	}

	string tmpstr;
	tmpstr.assign(element_name);
	mElementStack.push_back(tmpstr);
	XMLString::release(&element_name);
}

//--------------------------------------------------
//(SAX Event) close this element
//--------------------------------------------------
void amis::io::BookmarksFileIO::endElement( const XMLCh* const uri,
								   const XMLCh* const localname,
								   const XMLCh* const qname)
{
	char* element_name = XMLString::transcode(qname);

	if (strcmp(element_name, "note") == 0)
	{
		mpCurrentPosMark->mpNote = mpCurrentNote;
		mpCurrentPosMark->mbHasNote = true;
		mpCurrentNote = NULL;
	}
	else if (strcmp(element_name, "uid") == 0)
	{
		mpBookmarks->setUid(mTempChars);
	}
	else if(strcmp(element_name, "uri") == 0)
	{
		mpCurrentPosData->mUri = amis::util::makeUrlFromString(mTempChars);
	}
	else if (strcmp(element_name, "timeOffset") == 0)
	{
		mpCurrentPosData->mTimeOffset = mTempChars;
		mpCurrentPosData->mbHasTimeOffset = true;
	}
	else if(strcmp(element_name, "charOffset") == 0)
	{
		mpCurrentPosData->mCharOffset = mTempChars;
		mpCurrentPosData->mbHasCharOffset = true;
	}
	else if(strcmp(element_name, "ncxRef") == 0)
	{
		mpCurrentPosData->mNcxRef = mTempChars;
	}
	else if (strcmp(element_name, "lastmark") == 0)
	{
		mpBookmarks->setLastmark(mpCurrentPosData);
	}
	else if (strcmp(element_name, "text") == 0)
	{
		//look back to see if we are in an open "note" or "title" tag
		string elmname = mElementStack[mElementStack.size() - 2];
		if (elmname.compare("note") == 0)
		{
			mpCurrentNote->getText()->setTextString(mTempWChars);
		}
		else if (elmname.compare("title") == 0)
		{
			mpTitle->getText()->setTextString(mTempWChars);
		}
	}
	else if (strcmp(element_name, "bookmark") == 0)
	{
		mpCurrentPosMark->mpStart = mpCurrentPosData;
		amis::dtb::Bookmark* p_bookmark = (amis::dtb::Bookmark*)mpCurrentPosMark;
		this->mpBookmarks->addBookmark(p_bookmark);
	}
	else if(strcmp(element_name, "hilite") == 0)
	{
		amis::dtb::Hilite* p_hilite;
		p_hilite = (amis::dtb::Hilite*)mpCurrentPosMark;
		this->mpBookmarks->addHilite(p_hilite);
	}
	else if(strcmp(element_name, "hiliteStart") == 0)
	{
		amis::dtb::Hilite* p_hilite;
		p_hilite = (amis::dtb::Hilite*)mpCurrentPosMark;
		p_hilite->mpStart = mpCurrentPosData;
	}
	else if(strcmp(element_name, "hiliteEnd") == 0)
	{
		amis::dtb::Hilite* p_hilite;
		p_hilite = (amis::dtb::Hilite*)mpCurrentPosMark;
		p_hilite->mpEnd = mpCurrentPosData;
	}
	else if(strcmp(element_name, "title") == 0)
	{
		this->mpBookmarks->setTitle(mpTitle);
	}

	mb_flagGetChars = false;
	mTempChars.erase();
	mTempWChars.erase();

	XMLString::release(&element_name);
	mElementStack.pop_back();
}

//--------------------------------------------------
//xerces character data event
//--------------------------------------------------
void amis::io::BookmarksFileIO::characters(const XMLCh *const chars, const unsigned int length)
{
	if (mb_flagGetChars == true)
	{
		int idx = mElementStack.size() - 2;
		if (mElementStack[idx].compare("note") == 0 ||
			mElementStack[idx].compare("title") == 0)
		{
			mTempWChars.append((wchar_t*)chars);
		}
		else
		{
			if (mTempChars == "")
			{
				char* chardata = XMLString::transcode(chars);
				mTempChars.assign(chardata);
				XMLString::release(&chardata);
			}
		}
	}//end if mb_flagGetChars = true
}

bool amis::io::BookmarksFileIO::writeToFile(const ambulant::net::url* filepath, amis::dtb::BookmarkSet* pBmks)
{
	// Xerces-specific stuff: try-catch block to initialize the XML platform utilities
	// Initialize the XML4C2 system.
    try
    {
        XMLPlatformUtils::Initialize();
    }

	// catch the error
    catch(...)//const XMLException& toCatch)
    {
        return false;
    }
	// end of try-catch block

	if (pBmks == NULL)
		return false;

	mpBookmarks = pBmks;

	unsigned int i;

	//the DOM implementation pointer (this is part of how Xerces does things)
	DOMImplementation* pImpl;

	//a pointer to the Document Writer.  This will save the documet to file.
	DOMWriter* pWriter;

	//a DOMNode pointer - need this later for writing output
	DOMNode* pDocNode;

	//document output
	 XMLFormatTarget* pFormTarget; 

	//-------------
	// initializing the document
	//------------
	//You have to use the DOM Implementation class to get a pointer to the DOM object.  It's part of how Xerces works.
	pImpl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

	//Now create a document using the DOM implementation's createDocument method 
    mpDoc = pImpl->createDocument(
				X("http://amisproject.org"),     // root element namespace URI.
				X("bookmarkSet"),				// root element name
                0);								// document type object (DTD).

	//start adding data to the DOM
	writeTitle(mpBookmarks->getTitle());
	writeUid(mpBookmarks->getUid());
	writeLastmark(mpBookmarks->getLastmark());

	amis::dtb::PositionMark* p_pos;
	amis::dtb::Bookmark* p_bmk;
	amis::dtb::Hilite* p_hilite;

	for (i=0; i<mpBookmarks->getNumberOfItems(); i++)
	{	
		p_pos = mpBookmarks->getItem(i);

		if (p_pos->mType == amis::dtb::PositionMark::BOOKMARK)
		{
			p_bmk = (amis::dtb::Bookmark*)p_pos;
			writeBookmark(p_bmk);
		}
		else
		{
			p_hilite = (amis::dtb::Hilite*)p_pos;
			writeHilite(p_hilite);
		}
	}

	//initialize the DOM Writer
	pWriter = ((DOMImplementationLS*)pImpl)->createDOMWriter();

	ambulant::net::url base = filepath->get_base();
	//make sure the path to the file exists
	string dir = filepath->get_base().get_file();
	if (access(dir.c_str(), 0) == -1)
	{
#if defined(AMIS_PLATFORM_WINDOWS)
		_mkdir(dir.c_str());
#else
		//todo: do something for linux
#endif
	}


	//initialize the output stream and give it the name of our output file
	pFormTarget = new LocalFileFormatTarget(filepath->get_file().c_str());

	//get the document as a node pointer
	pDocNode = (DOMNode*)mpDoc;

	//process the document through the writer
	//the writer sends output to the form target (which was configured to use our output file) 
	pWriter->writeNode(pFormTarget, *pDocNode);
	
	//delete some pointers
	delete pFormTarget;
	delete pWriter;
	delete mpDoc;

	//terminate the XML platform utilities (part of how Xerces works)
    XMLPlatformUtils::Terminate();

	return true;
}

void amis::io::BookmarksFileIO::writeTitle(amis::MediaGroup* pTitle)
{
	if (pTitle == NULL)
	{
		return;
	}
		
	//create the title
    DOMElement* p_title_elm = mpDoc->createElement(X("title"));
	writeMediaGroup(pTitle, p_title_elm);
	DOMElement* p_root = mpDoc->getDocumentElement();
	p_root->appendChild(p_title_elm);
}

void amis::io::BookmarksFileIO::writeUid(string uid)
{
	DOMElement* p_uid_elm = mpDoc->createElement(X("uid"));
	DOMText* p_text = mpDoc->createTextNode(X(uid.c_str()));
	p_uid_elm->appendChild(p_text);
	
	DOMElement* p_root = mpDoc->getDocumentElement();
	p_root->appendChild(p_uid_elm);
}

void amis::io::BookmarksFileIO::writeLastmark(amis::dtb::PositionData* pLastmark)
{
	if (pLastmark == NULL)
		return;

	DOMElement* p_lastmark_elm = mpDoc->createElement(X("lastmark"));
	writePositionData(pLastmark, p_lastmark_elm);
	DOMElement* p_root = mpDoc->getDocumentElement();
	p_root->appendChild(p_lastmark_elm);
}

void amis::io::BookmarksFileIO::writeHilite(amis::dtb::Hilite* pHilite)
{
	DOMElement* p_hilite_elm = mpDoc->createElement(X("hilite"));
	DOMElement* p_hilite_start_elm = mpDoc->createElement(X("hiliteStart"));
	DOMElement* p_hilite_end_elm = mpDoc->createElement(X("hiliteEnd"));

	amis::dtb::PositionData* p_start = pHilite->mpStart;
	amis::dtb::PositionData* p_end = pHilite->mpEnd;

	writePositionData(p_start, p_hilite_start_elm);
	writePositionData(p_end, p_hilite_end_elm);

	p_hilite_elm->appendChild(p_hilite_start_elm);
	p_hilite_elm->appendChild(p_hilite_end_elm);

	if (pHilite->mbHasNote == true)
	{
		writeNote(pHilite->mpNote, p_hilite_elm);
	}

	DOMElement* p_root = mpDoc->getDocumentElement();
	p_root->appendChild(p_hilite_elm);
}

void amis::io::BookmarksFileIO::writeBookmark(amis::dtb::Bookmark* pBookmark)
{
	DOMElement* p_bookmark_elm = mpDoc->createElement(X("bookmark"));
	amis::dtb::PositionData* p_pos = pBookmark->mpStart;
	writePositionData(p_pos, p_bookmark_elm);

	if (pBookmark->mbHasNote == true)
		writeNote(pBookmark->mpNote, p_bookmark_elm);

	DOMElement* p_root = mpDoc->getDocumentElement();
	p_root->appendChild(p_bookmark_elm);
}

void amis::io::BookmarksFileIO::writePositionData(amis::dtb::PositionData* pData, 
											  DOMElement* pParent)
{
	std::string ncx_ref = "";
	if (pData->mNcxRef != "") ncx_ref = pData->mNcxRef;
	
	DOMElement* p_ncx_ref = mpDoc->createElement(X("ncxRef"));
	DOMText* p_ncx_ref_text = mpDoc->createTextNode(X(ncx_ref.c_str()));	
	p_ncx_ref->appendChild(p_ncx_ref_text);
	pParent->appendChild(p_ncx_ref);

	DOMElement* p_uri = mpDoc->createElement(X("uri"));
	DOMText* p_uri_text = mpDoc->createTextNode(X(pData->mUri.get_url().c_str()));
	p_uri->appendChild(p_uri_text);
	pParent->appendChild(p_uri);

	if (pData->mbHasTimeOffset == true)
	{
		DOMElement* p_time_offset = mpDoc->createElement(X("timeOffset"));
		DOMText* p_time_offset_text = mpDoc->createTextNode(X(pData->mTimeOffset.c_str()));
		p_time_offset->appendChild(p_time_offset_text);
		pParent->appendChild(p_time_offset);
	}

	if (pData->mbHasCharOffset == true)
	{
		DOMElement* p_char_offset = mpDoc->createElement(X("charOffset"));
		DOMText* p_char_offset_text = mpDoc->createTextNode(X(pData->mCharOffset.c_str()));
		p_char_offset->appendChild(p_char_offset_text);
		pParent->appendChild(p_char_offset);
	}
}

void amis::io::BookmarksFileIO::writeMediaGroup(amis::MediaGroup* pMedia, DOMElement* pParent)
{	
	if (pMedia == NULL || pParent == NULL)
		return;

	if (pMedia->hasText() == true)
	{
		DOMElement* p_text_elm = mpDoc->createElement(X("text"));
		wstring text_content = pMedia->getText()->getTextString();
#if defined(AMIS_COMPILER_GCC)
//note!! i don't promise that there aren't bugs here
		int len = text_content.length();
		char* temp = new char[len+1];
		wcstombs(temp, text_content.c_str(), len+1);
		XMLCh* xmlch_temp = XMLString::transcode(temp);
		DOMText* p_text_content = mpDoc->createTextNode(xmlch_temp);
		XMLString::release(&xmlch_temp);
		delete temp;
#elif defined(AMIS_COMPILER_MSVC)
		DOMText* p_text_content = mpDoc->createTextNode(text_content.c_str());
#endif
		p_text_elm->appendChild(p_text_content);
		pParent->appendChild(p_text_elm);
	}

	if (pMedia->hasAudio() == true)
	{
		DOMElement* p_audio_elm = mpDoc->createElement(X("audio"));
		amis::AudioNode* p_audio_obj = pMedia->getAudio(0);

		//add src, clipBegin, and clipEnd attributes to "audio"
		p_audio_elm->setAttribute(X("src"), 
			X(p_audio_obj->getPath().c_str()));

		p_audio_elm->setAttribute(X("clipBegin"), 
			X(p_audio_obj->getClipBegin().c_str()));

		p_audio_elm->setAttribute(X("clipEnd"), 
			X(p_audio_obj->getClipEnd().c_str()));

		pParent->appendChild(p_audio_elm);
	}
}

void amis::io::BookmarksFileIO::writeNote(amis::MediaGroup* pNote, DOMElement* pParent)
{
	DOMElement* p_note_elm = mpDoc->createElement(X("note"));
	writeMediaGroup(pNote, p_note_elm);
	pParent->appendChild(p_note_elm);
}
