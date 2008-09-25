/*
AmisCommon: common system objects and utility routines

Copyright (C) 2004  DAISY for All Project

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

//SYSTEM INCLUDES
#include <string>
#include <iostream>
#include <cctype>
#include <algorithm>

//PROJECT INCLUDES
#include "FilePathTools.h"
#include "TitleAuthorParse.h"
#include "OpfItemExtract.h"
#include "SmilAudioExtract.h"
#include "trim.h"

using namespace std;




//--------------------------------------------------
//constructor
//--------------------------------------------------
amis::TitleAuthorParse::TitleAuthorParse()
{
	mb_flagTheyHaveTitleInfo = false;
	mb_flagTheyHaveAuthorInfo = false;

	mpTitleInfo = NULL;
	mpAuthorInfo = NULL;
}


//--------------------------------------------------
//destructor
//!destroy the media group objects if no one else has taken ownership of them
//--------------------------------------------------
amis::TitleAuthorParse::~TitleAuthorParse()
{
	if (mb_flagTheyHaveTitleInfo == false &&
		mpTitleInfo != NULL)
	{
		mpTitleInfo->destroyContents();

		delete mpTitleInfo;
	}

	if (mb_flagTheyHaveAuthorInfo == false &&
		mpAuthorInfo != NULL)
	{
		mpAuthorInfo->destroyContents();

		delete mpAuthorInfo;
	}
}

//--------------------------------------------------
//return the file path
//--------------------------------------------------
string amis::TitleAuthorParse::getFilePath()
{
	return this->mFilePath;
}

//!open a file (ncc, ncx, or opf)
bool amis::TitleAuthorParse::openFile(string filepath)
{
	//local variables
	const char* cp_file;
	string tmp_string;
	SAX2XMLReader* parser;

	mb_flagFinished = false;
	mb_flagGetChars = false;
	mb_flagDocAuthor = false;
	mb_flagDocTitle = false;
	
	mFilePath = amis::FilePathTools::getAsLocalFilePath(filepath);
	mFilePath = amis::FilePathTools::clearTarget(filepath);

	string file_name = amis::FilePathTools::getFileName(filepath);
	string file_ext = amis::FilePathTools::getExtension(filepath);

	//convert the string to lower case before doing a comparison
	 std::transform(file_name.begin(), file_name.end(), 
		 file_name.begin(), (int(*)(int))tolower);

	 //convert the string to lower case before doing a comparison
	 std::transform(file_ext.begin(), file_ext.end(), 
		 file_ext.begin(), (int(*)(int))tolower);

	 if (file_name.compare(FILENAME_NCC) == 0)
	 {
		 mFileType = NCC;
	 }
	 
	 if (file_ext.compare(FILE_EXT_OPF) == 0)
	 {
		 OpfItemExtract opf_parse;
		 mFilePath = opf_parse.getItemHref(mFilePath, ID_NCX);

		 mFileType = NCX;
		 
	 }

	 if (file_ext.compare(FILE_EXT_NCX) == 0)
	 {
		 mFileType = NCX;
	 }

	//do a SAX parse of this new file
	cp_file = mFilePath.c_str();
	
	//try-catch block for Xerces platform utilities
	try
	{
		XMLPlatformUtils::Initialize();
	}
	
	catch (...)//const XMLException& toCatch)
	{
		return false;
	}
		
	//assuming we've made it this far, create a new parser
	parser = XMLReaderFactory::createXMLReader();
	
	//set these parser features to turn off DTD loading and validation
	parser->setFeature(XMLUni::fgXercesLoadExternalDTD, false);
	parser->setFeature(XMLUni::fgSAX2CoreValidation, false);
	
	
	//try-catch block for Xerces parsing
	try 
	{
		//give the sax parser pointers to our content and error handler object
		parser->setContentHandler(this);
		parser->setErrorHandler(this);
		
		//parser begins fileparse.  expect SAX Events soon.  
		parser->parse(cp_file);
	}
	
	catch (...)//const XMLException& toCatch)
	{	
		delete parser;
		//terminate the platform utilities
		XMLPlatformUtils::Terminate();
		
		return false;
	}
	
	//we're done with the parser so delete it and terminate the XML platform utilities
	delete parser;
	XMLPlatformUtils::Terminate();

	//if this was an NCC file, resolve the smil href to get the title audio data
	
	if (mFileType == NCC)
	{
		if (mHref != "")
		{
			SmilAudioExtract* smil_extract = new SmilAudioExtract();;
			mHref = amis::FilePathTools::goRelativePath(mFilePath, mHref);
			amis::AudioNode* p_audio = smil_extract->getAudioAtId(mHref);
			if (p_audio != NULL)
			{
				mpTitleInfo->addAudioClip(p_audio);
			}
			delete smil_extract;
		}
	}
	return true;
}


//--------------------------------------------------
//get the author info
//--------------------------------------------------
amis::MediaGroup* amis::TitleAuthorParse::getAuthorInfo()
{
	mb_flagTheyHaveAuthorInfo = true;
	return mpAuthorInfo;
}

//--------------------------------------------------
//get the title info
//--------------------------------------------------
amis::MediaGroup* amis::TitleAuthorParse::getTitleInfo()
{
	mb_flagTheyHaveTitleInfo = true;
	return mpTitleInfo;
}

//--------------------------------------------------
//SAX METHODS
//--------------------------------------------------

//--------------------------------------------------
//xerces start element event
//--------------------------------------------------
void amis::TitleAuthorParse::startElement(const   XMLCh* const    uri,
				const   XMLCh* const    localname,
				const   XMLCh* const    qname,
				const   Attributes&	attributes)
{
	char* element_name;
		
	//get the element name as a string
	element_name = XMLString::transcode(qname); //DANIEL MEMORY LEAK VERIFIED

	mpAttributes = &attributes;

	if (mb_flagFinished == false)
	{
		//for ncc's we are looking for <h1 class="title"...>
		if (mFileType == NCC)
		{
			//if this is a heading one element
			if (strcmp(element_name, TAG_H1) == 0)
			{
				string classval = getAttributeValue(ATTR_CLASS);

				//if the class="title"
				if (classval == ATTRVAL_TITLE)
				{
					mpTitleInfo = new amis::MediaGroup();

					mb_flagGetChars = true;
					mTempChars.erase();

					mb_flagGetHref = true;
					mChardataElm = TAG_H1;
				}
			}

			//if this is a link element and we are looking to get an href value
			else if(mb_flagGetHref == true &&
				strcmp(element_name, TAG_A) == 0)
			{
				mHref = getAttributeValue(ATTR_HREF);
				mb_flagGetHref = false;
			}

			else
			{
				//empty
			}
		}

		//for ncx's we are looking for docTitle and docAuthor elements
		else if(mFileType == NCX)
		{
			if (strcmp(element_name, TAG_DOCAUTHOR) == 0)
			{
				mpAuthorInfo = new amis::MediaGroup();
				mb_flagDocAuthor = true;
			}

			else if(strcmp(element_name, TAG_DOCTITLE) == 0)
			{
				mpTitleInfo = new amis::MediaGroup();
				mb_flagDocTitle = true;
			}

			else if((mb_flagDocTitle == true || mb_flagDocAuthor == true)
				&& strcmp(element_name, TAG_TEXT) == 0)
			{
				mb_flagGetChars = true;
				mTempChars.erase();
				mChardataElm = TAG_TEXT;
			}

			else if((mb_flagDocTitle == true || mb_flagDocAuthor == true)
				&& strcmp(element_name, TAG_AUDIO) == 0)
			{
				string src = getAttributeValue(ATTR_SRC);
				string clip_begin = getAttributeValue(ATTR_CLIPBEGIN);
				string clip_end = getAttributeValue(ATTR_CLIPEND);

				src = amis::FilePathTools::goRelativePath(mFilePath, src);
				amis::AudioNode* p_audio;
				p_audio = new amis::AudioNode();
				p_audio->setSrc(src);
				p_audio->setClipBegin(clip_begin);
				p_audio->setClipEnd(clip_end);

				amis::MediaGroup* p_current;

				if (mb_flagDocTitle == true)
				{
					p_current = mpTitleInfo;
				}
				else if(mb_flagDocAuthor == true)
				{
					p_current = mpAuthorInfo;
				}
				else
				{
					//empty
				}

				p_current->addAudioClip(p_audio);


			}

			else
			{
				//empty
			}
		}

		
	}

	XMLString::release(&element_name); //DANIEL MEMORY LEAK VERIFIED
}

//--------------------------------------------------
//(SAX Event) close this element so that no additional child nodes are added to it in the Smil Tree
//--------------------------------------------------
void amis::TitleAuthorParse::endElement( const XMLCh* const uri,
								   const XMLCh* const localname,
								   const XMLCh* const qname)
{
	//local variable
	char* element_name = XMLString::transcode(qname); //DANIEL MEMORY LEAK VERIFIED
	
	//if we are ending the tag we wanted to get character data for
	if (mb_flagGetChars == true &&
		strcmp(element_name, mChardataElm.c_str()) == 0)
	{
		mb_flagGetChars = false;
	}
	
	else if(mb_flagDocAuthor == true &&
		strcmp(element_name, TAG_DOCAUTHOR) == 0)
	{
		mb_flagDocAuthor = false;
	}

	else if(mb_flagDocTitle == true &&
		strcmp(element_name, TAG_DOCTITLE) == 0)
	{
		mb_flagDocTitle = false;
	}
	else
	{
		//empty
	}
	XMLString::release(&element_name); //DANIEL MEMORY LEAK VERIFIED
	
}

//--------------------------------------------------
//xerces error event
//--------------------------------------------------
void amis::TitleAuthorParse::error(const SAXParseException& e)
{
}

//--------------------------------------------------
//xerces fatal error event
//--------------------------------------------------
void amis::TitleAuthorParse::fatalError(const SAXParseException& e)
{
}

//--------------------------------------------------
//xerces warning event
//--------------------------------------------------
void amis::TitleAuthorParse::warning(const SAXParseException& e)
{
}

//--------------------------------------------------
//xerces character data event
//--------------------------------------------------
void amis::TitleAuthorParse::characters(const XMLCh *const chars, const unsigned int length)
{
	if (mb_flagGetChars == true)
	{
		
		mTempChars.append((wchar_t*)chars);
		
		amis::MediaGroup* p_current_media = NULL;

		if (mFileType == NCX)
		{
			if (mb_flagDocTitle == true)
			{
				p_current_media = mpTitleInfo;
			}
			else if(mb_flagDocAuthor == true)
			{
				p_current_media = mpAuthorInfo;
			}
			else
			{
				//empty
			}
			
		}
		else // if (mFileType == NCC)
		{	
			p_current_media = mpTitleInfo;
		}

		if (p_current_media != NULL)
		{
			if (p_current_media->hasText() == false)
			{
				amis::TextNode* p_text;
				p_text = new amis::TextNode();
				p_text->setTextString(mTempChars);
				p_current_media->setText(p_text);
			}
			else
			{
				p_current_media->getText()->setTextString(trim(mTempChars));

			}
		
		}

	}//end if mb_flagGetChars = true
}


//---------------------------------
//utility function
//---------------------------------
string amis::TitleAuthorParse::getAttributeValue(string attributeName)
{
	//initialize local strings
	char* current_attribute_name;
	char* attribute_value;
	string return_value = "";


	//save the attributes list length
	int len = mpAttributes->getLength();

	//for-loop through the attributes list until we find a match
	for (int i = 0; i<len; i++)
	{
		current_attribute_name = XMLString::transcode(mpAttributes->getQName(i)); //DANIEL MEMORY LEAK VERIFIED

		//comparison if statement
		if (strcmp(current_attribute_name, attributeName.c_str()) == 0)
		{
			//a match has been found, so save it and break from the loop
			attribute_value = XMLString::transcode(mpAttributes->getValue(i)); //DANIEL MEMORY LEAK VERIFIED
		
			return_value.assign(attribute_value);
			XMLString::release(&attribute_value); //DANIEL MEMORY LEAK VERIFIED
		XMLString::release(&current_attribute_name); //DANIEL MEMORY LEAK VERIFIED
			break;
		}
		XMLString::release(&current_attribute_name); //DANIEL MEMORY LEAK VERIFIED
	} //end for-loop

	
	//return the value of the requested attribute
	//if the attribute does not exist, this function returns an empty string
	return return_value;
}
