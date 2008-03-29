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
#include "io/TextSearch.h"
#include "util/xercesutils.h"
#include "util/FilePathTools.h"
#include <algorithm>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/HandlerBase.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

//return the text_file#id for the text string
//params: search string, text filepath
string amis::io::TextSearch::searchForText(wstring searchfor, const ambulant::net::url* filepath)
{
	mFound = "";
	mSearchString = searchfor;
	mId = "";
	mBoundaryId = "";
	mDir = 0;
	return readFromFile(filepath);
}

//params: search string, starting id in text file, text filepath
string amis::io::TextSearch::searchForTextForwardFromId(wstring searchfor, string fromId, const ambulant::net::url* filepath)
{
	mFound = "";
	mSearchString = searchfor;
	mId = "";
	mBoundaryId = fromId;
	mDir = 1;
	return readFromFile(filepath);
}

//params: search string, starting id in text file, text filepath
string amis::io::TextSearch::searchForTextBackwardsFromId(wstring searchfor, string fromId, const ambulant::net::url* filepath)
{
	mFound = "";
	mSearchString = searchfor;
	mId = "";
	mBoundaryId = fromId;
	mDir = -1;
	return readFromFile(filepath);
}

string amis::io::TextSearch::readFromFile(const ambulant::net::url* filepath)
{
	mbFlag_Analyze = false;
	mbFlag_Boundary = false;
	mCharDataStack.clear();
	mIdStack.clear();

	//convert the strings to lower case before doing a comparison
	 std::transform(mSearchString.begin(), mSearchString.end(), 
		 mSearchString.begin(), (int(*)(int))tolower);

	if (!this->parseFile(filepath))
	{
		return "";
	}
	else
	{
		if (mFound != "" )
		{
			string ret_val = amis::util::FilePathTools::getFileName(filepath->get_path());
			ret_val = ret_val + "#" + mFound;
		}
		return mFound;
	}
}

void amis::io::TextSearch::startElement(const   XMLCh* const    uri,
				const   XMLCh* const    localname,
				const   XMLCh* const    qname,
				const   Attributes&	attributes)
{
	char* element_name = XMLString::transcode(qname);
	
	//ASSUMING that the body or book tags signify the start of significant
	//content in html, dtbook, and smil
	if (mbFlag_Analyze == false)
	{
		if (strcmp(element_name, "body") == 0 || strcmp(element_name, "book") == 0)
			mbFlag_Analyze = true;
		return;
	}
	mbFlag_GetChars = true;

	mId = SimpleAttrs::get("id", &attributes);
	if (mId != "")	mIdStack.push_back(mId);
	if (mId == mBoundaryId)
	{
		mbFlag_Boundary = true;
		//if we were searching backwards, stop searching
		if (mDir == -1)
		{
			this->mbFlag_Analyze = false;
			this->mbFlag_GetChars = false;
		}
	}
	
	//if we are searching forward, we've found the boundary id, and this is not it
	//then we want to start getting characters
	if (mDir == 1)
	{
		if (mbFlag_Boundary == true && mIdStack.size() > 0 && mIdStack.back() != mBoundaryId)
			mbFlag_GetChars = true;
		else
			mbFlag_GetChars = false;
	}

	XMLString::release(&element_name); 
}

void amis::io::TextSearch::endElement( const XMLCh* const uri,
					const XMLCh* const localname,
					 const XMLCh* const qname)
{
	char* element_name = XMLString::transcode(qname);
	if (mbFlag_Analyze == true)
	{
		if (mId.size() > 0) mIdStack.pop_back();
		mCharDataStack.pop_back();
	}

	if (strcmp(element_name, "body") == 0 ||
		strcmp(element_name, "book") == 0)
	{
		mbFlag_Analyze = false;
	}
	XMLString::release(&element_name);
}


void amis::io::TextSearch::characters(const XMLCh *const chars, const unsigned int length)
{
	if (mbFlag_GetChars == true)
	{
		if (mCharDataStack.size() > 0)
			mCharDataStack.back().append((wchar_t*)chars);
		else
			mCharDataStack.push_back((wchar_t*)chars);

		searchInText();
	}
}

void amis::io::TextSearch::searchInText()
{
	//look in the last stack item for the text

	std::wstring elm_text = mCharDataStack.back();

	//convert the string to lower case before doing a comparison
	 std::transform(elm_text.begin(), elm_text.end(), 
		 elm_text.begin(), (int(*)(int))tolower);

	//found it!
	if (elm_text.find(mSearchString) != std::string::npos)
	{
		if (mDir != -1)
		{
			mbFlag_GetChars = false;
			mbFlag_Analyze = false;
		}

		if (mIdStack.size() > 0)
			mFound = mIdStack.back();
	}
}
