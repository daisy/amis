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

#include "io/BookListFileIO.h"
#include "util/FilePathTools.h"
#include "util/xercesutils.h"
#include <iostream>

#if defined(AMIS_COMPILER_MSVC)
//for creating directories on windows
#include "io.h"
#include "direct.h"
//todo: something for linux
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


amis::io::BookListFileIO::BookListFileIO()
{
	mpBookList = NULL;
}

amis::io::BookListFileIO::~BookListFileIO()
{
}

amis::BookList* amis::io::BookListFileIO::getBookList()
{
	return mpBookList;
}

bool amis::io::BookListFileIO::readFromFile(const ambulant::net::url* filepath)
{
	mpCurrentEntry = NULL;
	if (!this->parseFile(filepath))
	{
		delete mpBookList;
		return false;
	}
	return true;
}

void amis::io::BookListFileIO::startElement(const   XMLCh* const    uri,
				const   XMLCh* const    localname,
				const   XMLCh* const    qname,
				const   Attributes&	attributes)
{
	char* element_name;
	element_name = XMLString::transcode(qname);

	if (strcmp(element_name, "bookList") == 0)
	{
		mpBookList = new amis::BookList();
		mpBookList->setFilepath(this->getFilepath());
		wstring tmp_wstr;
		tmp_wstr.assign(SimpleAttrs::getw("name", &attributes));
		mpBookList->setName(tmp_wstr);
	}
	else if (strcmp(element_name, "bookEntry") == 0)
	{
		mpCurrentEntry = new amis::BookEntry();
		mpBookList->addEntry(mpCurrentEntry);

		string tmpstr;
		tmpstr.assign(SimpleAttrs::get("isLastRead", &attributes));
		if (tmpstr.compare("yes") == 0) 
			mpCurrentEntry->mbIsLastRead = true;
		else 
			mpCurrentEntry->mbIsLastRead = false;

		wstring tmp_wstr;
		tmp_wstr.assign(SimpleAttrs::getw("uid", &attributes));
		mpCurrentEntry->mUid = tmp_wstr;

		tmpstr.assign(SimpleAttrs::get("href", &attributes));
		mpCurrentEntry->mPath = amis::util::makeUrlFromString(tmpstr);
	}
	else if(strcmp(element_name, "bookmarks") == 0)
	{
		if (mpCurrentEntry != NULL)
		{
			string tmpstr;
			tmpstr.assign(SimpleAttrs::get("href", &attributes));
			mpCurrentEntry->mBmkPath = amis::util::makeUrlFromString(tmpstr);
		}
	}
	else if (strcmp(element_name, "title") == 0)
	{
		if (mpCurrentEntry != NULL)
		{
			string src;
			string clipbegin;
			string clipend;
			src.assign(SimpleAttrs::get("src", &attributes));
			clipbegin.assign(SimpleAttrs::get("clipBegin", &attributes));
			clipend.assign(SimpleAttrs::get("clipEnd", &attributes));
			mpCurrentEntry->setTitleAudio(src, clipbegin, clipend);
			mbFlagGetChars = true;
		}
	}
	XMLString::release(&element_name);
}
void amis::io::BookListFileIO::endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
{
	char* element_name = XMLString::transcode(qname);

	if (strcmp(element_name, "title") == 0)
	{
		this->mpCurrentEntry->setTitleText(mTempWChars);
		mbFlagGetChars = false;
	}
	mTempWChars.erase();

	XMLString::release(&element_name);
}
void amis::io::BookListFileIO::characters(const XMLCh *const chars, const unsigned int length)
{
	if (mbFlagGetChars == true) mTempWChars.append((wchar_t*)chars);
}

bool amis::io::BookListFileIO::writeToFile(const ambulant::net::url* filepath, amis::BookList* pFile)
{
	if (filepath != NULL && filepath->is_local_file() == false) return false;

	// Initialize the XML4C2 system.
    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch(...)//const XMLException& toCatch)
    {
        return false;
    }
	
	if (pFile == NULL) return false;
	if (!pFile->getFilepath()->is_local_file()) return false;

	std::string local_file_name;
	//use the specified filepath unless it is unavailable
	if (filepath != NULL) 
		local_file_name = amis::util::FilePathTools::getAsLocalFilePath(filepath->get_file());
	else 
		local_file_name = amis::util::FilePathTools::getAsLocalFilePath(pFile->getFilepath()->get_file());
	
	mpBookList = pFile;

	int i;
	//the DOM implementation pointer (this is part of how Xerces does things)
	DOMImplementation* pImpl;
	//a pointer to the Document Writer.  This will save the document to file.
	DOMWriter* pWriter;

	//a DOMNode pointer - need this later for writing output
	DOMNode* pDocNode;

	//document output
	XMLFormatTarget* pFormTarget; 

	//You have to use the DOM Implementation class to get a pointer to the DOM object.  It's part of how Xerces works.
	pImpl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

	//Now create a document using the DOM implementation's createDocument method 
    mpDoc = pImpl->createDocument(
				X("http://amisproject.org"),     // root element namespace URI.
				X("bookList"),					// root element name
                0);								// document type object (DTD).


	DOMElement* p_root = mpDoc->getDocumentElement();
	//add the root element attributes
	if (mpBookList->getName().size() > 0)
		p_root->setAttribute(X("name"), mpBookList->getName().c_str());
	
	//start adding data to the DOM
	amis::BookEntry* p_entry;
	for (i=0; i<mpBookList->getNumberOfEntries(); i++)
	{
		p_entry = mpBookList->getEntry(i);
		writeBookEntry(p_entry);
	}


	//initialize the DOM Writer
	pWriter = ((DOMImplementationLS*)pImpl)->createDOMWriter();

	//initialize the output stream and give it the name of our output file
	//if no filepath was specified, use the one associated with the book list object
	if (filepath->is_empty_path())
	{
		//make sure the path to the file exists
		string dir = amis::util::FilePathTools::getParentDirectory(local_file_name);
		if (access(dir.c_str(), 0) == -1)
		{
#if defined(AMIS_COMPILER_MSVC)
			_mkdir(dir.c_str());
#elif defined(AMIS_COMPILER_GCC)
			//todo: something that creates directories on other platforms
#endif
		}
		pFormTarget = new LocalFileFormatTarget(local_file_name.c_str());
	}
	//otherwise save to the specified location
	else
	{
		//make sure the path to the file exists
		string dir = amis::util::FilePathTools::getParentDirectory(local_file_name);
		if (access(dir.c_str(), 0) == -1)
		{
#if defined(AMIS_COMPILER_MSVC)
			_mkdir(dir.c_str());
#elif defined(AMIS_COMPILER_GCC)
			//todo: something to make directories on other platforms
#endif
		}
		pFormTarget = new LocalFileFormatTarget(local_file_name.c_str());
	}

	//get the document as a node pointer
	pDocNode = (DOMNode*)mpDoc;

	//process the document through the writer
	//the writer sends output to the form target (which was configured to use our output file) 
	pWriter->writeNode(pFormTarget, *pDocNode);
	
	//delete some pointers
	delete pFormTarget;
	delete pWriter;
	delete mpDoc;

	//terminate the XML platform utilities 
    XMLPlatformUtils::Terminate();
	return true;
}

void amis::io::BookListFileIO::writeBookEntry(amis::BookEntry* pEntry)
{
	if (pEntry == NULL) return;
		
	//create the title
	DOMElement* p_book_elm = mpDoc->createElement(X("bookEntry"));
	DOMElement* p_bmk_elm = NULL;
	DOMElement* p_title_elm = mpDoc->createElement(X("title"));
	DOMText* p_text_content = mpDoc->createTextNode(pEntry->getTitleText().c_str());
	p_title_elm->appendChild(p_text_content);
	amis::AudioNode* p_audio = pEntry->getTitleAudio();
	if (p_audio != NULL)
	{
		p_title_elm->setAttribute(X("src"), X(p_audio->getPath().c_str()));
		p_title_elm->setAttribute(X("clipBegin"), X(p_audio->getClipBegin().c_str()));
		p_title_elm->setAttribute(X("clipEnd"), X(p_audio->getClipEnd().c_str()));
	}
	p_book_elm->appendChild(p_title_elm);

	if (pEntry->mBmkPath.is_empty_path() == false)
	{
		p_bmk_elm = mpDoc->createElement(X("bookmarks"));
		p_bmk_elm->setAttribute(X("href"), X(pEntry->mBmkPath.get_url().c_str()));
	}
	if (pEntry->mbIsLastRead == true)
		p_book_elm->setAttribute(X("isLastRead"), X("yes"));
	
	p_book_elm->setAttribute(X("uid"), pEntry->mUid.c_str());
	p_book_elm->setAttribute(X("href"), X(pEntry->mPath.get_url().c_str()));

	if (p_bmk_elm != NULL) p_book_elm->appendChild(p_bmk_elm);

	DOMElement* p_root = mpDoc->getDocumentElement();
	p_root->appendChild(p_book_elm);
}
