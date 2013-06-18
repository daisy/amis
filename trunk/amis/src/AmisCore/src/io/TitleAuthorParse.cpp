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

#include "util/Log.h"
#include "io/TitleAuthorParse.h"
#include "util/FilePathTools.h"
#include "util/xercesutils.h"
#include "Media.h"
#include "dtb/DtbFileSet.h"
#include "io/OpfFileReader.h"
#include "io/SmilAudioExtract.h"



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


using namespace std;

amis::io::TitleAuthorParse::TitleAuthorParse()
{
	mbTransferredTitleData = false;
	mbTransferredAuthorData = false;
	mpTitleInfo = NULL;
	mpAuthorInfo = NULL;
}

amis::io::TitleAuthorParse::~TitleAuthorParse()
{
	if (mbTransferredTitleData == false && mpTitleInfo != NULL)
	{
		mpTitleInfo->destroyContents();
		delete mpTitleInfo;
	}

	if (mbTransferredAuthorData == false &&	mpAuthorInfo != NULL)
	{
		mpAuthorInfo->destroyContents();
		delete mpAuthorInfo;
	}
}

//this function takes an OPF or NCC filepath and starts the parsing
bool amis::io::TitleAuthorParse::readFromFile(const ambulant::net::url* filepath)
{
	
	amis::util::Log::Instance()->writeMessage("Starting title and author data extraction", filepath, 
		"TitleAuthorParse::readFromFile");
	
	//clear all variables
	mbFlagGetChars = false;
	mSmilHref = "";
	mbIsDataExtracted = false;
	mbFlagGetSmilHref = false;
	mbFlagProcessingAuthor = false;
	mbFlagProcessingTitle = false;
	mTempChars.erase();
	mChardataElm = "";
	mbTransferredTitleData = false;
	mbTransferredAuthorData = false;

	ambulant::net::url file_to_parse;

	if (amis::dtb::DtbFileSet::isNccFile(filepath) == true)
	{
		file_to_parse = *filepath;
		mFileType = NCC;
	}
	//for OPF files, we have to read them and find out the NCX filepath, and use that with our parser
	else if (amis::dtb::DtbFileSet::isOpfFile(filepath) == true)
	{
		OpfFileReader opf_parse;
		if (opf_parse.readFromFile(filepath) == true)
		{
			file_to_parse = *opf_parse.getNavFilename();
			mFileType = NCX;
		}
		else
		{
			amis::util::Log::Instance()->writeWarning("Could not open OPF file", filepath, "TitleAuthorParse::readFromFile");
			return false;
		}
	}
	else
	{
		amis::util::Log::Instance()->writeWarning("Unrecognized file type", filepath, "TitleAuthorParse::readFromFile");
		return false;
	}

	//start the parsing.. expect events soon
	if (!this->parseFile(&file_to_parse)) 
	{
		amis::util::Log::Instance()->writeWarning("Parse failed", filepath, "TitleAuthorParse::readFromFile");
		return false;
	}
	
	//if this was an NCC file, resolve the smil href to get the title audio data
	if (mFileType == NCC)
	{
		if (mSmilHref != "")
		{
			amis::AudioNode* p_audio = NULL;
			ambulant::net::url smil_href = amis::util::makeUrlFromString(mSmilHref);
			smil_href = smil_href.join_to_base(*this->getFilepath());
			//quickly parse the SMIL file and grab the audio closest to this ID
			SmilAudioExtract smil_audio_extract;
			p_audio = smil_audio_extract.getAudioAtId(&smil_href);
			if (p_audio != NULL)
				mpTitleInfo->addAudioClip(p_audio);
			else
				amis::util::Log::Instance()->writeWarning("No audio available in SMIL", &smil_href, 
					"TitleAuthorParse::readFromFile");
		}
	}

	return true;
}

amis::MediaGroup* amis::io::TitleAuthorParse::getAuthorInfo()
{
	mbTransferredAuthorData = true;
	return mpAuthorInfo;
}

amis::MediaGroup* amis::io::TitleAuthorParse::getTitleInfo()
{
	mbTransferredTitleData = true;
	return mpTitleInfo;
}

void amis::io::TitleAuthorParse::startElement(const   XMLCh* const    uri,
				const   XMLCh* const    localname,
				const   XMLCh* const    qname,
				const   Attributes&	attributes)
{
	char* element_name = XMLString::transcode(qname);
	
	if (mbIsDataExtracted == false)
	{
		//for ncc's we are looking for <h1 class="title"...>
		if (mFileType == NCC)
		{
			if (strcmp(element_name, "h1") == 0)
			{
				string classval = SimpleAttrs::get("class", &attributes);
				if (classval == "title")
				{
					mpTitleInfo = new amis::MediaGroup();
					mbFlagGetChars = true;
					mTempChars.erase();
					mbFlagGetSmilHref = true;
					mChardataElm = "h1";
				}
			}
			//if this is a link element and we are looking to get an href value
			else if(mbFlagGetSmilHref == true && strcmp(element_name, "a") == 0)
			{
				mSmilHref = SimpleAttrs::get("href", &attributes);
				mbFlagGetSmilHref = false;
			}
		}

		//for ncx's we are looking for docTitle and docAuthor elements
		else if(mFileType == NCX)
		{
			if (strcmp(element_name, "docAuthor") == 0)
			{
				mpAuthorInfo = new amis::MediaGroup();
				mbFlagProcessingAuthor = true;
			}
			else if(strcmp(element_name, "docTitle") == 0)
			{
				mpTitleInfo = new amis::MediaGroup();
				mbFlagProcessingTitle = true;
			}
			else if(mbFlagProcessingTitle == true || mbFlagProcessingAuthor == true)
			{
				if (strcmp(element_name, "text") == 0)
				{
					mbFlagGetChars = true;
					mTempChars.erase();
					mChardataElm = "text";
				}

				if (strcmp(element_name, "audio") == 0)
				{
					string src = SimpleAttrs::get("src", &attributes);
					string clip_begin = SimpleAttrs::get("clipBegin", &attributes);
					string clip_end = SimpleAttrs::get("clipEnd", &attributes);

					src = amis::util::FilePathTools::goRelativePath(this->getFilepath()->get_file(), src);
					amis::AudioNode* p_audio;
					p_audio = new amis::AudioNode();
					p_audio->setPath(src);
					p_audio->setClipBegin(clip_begin);
					p_audio->setClipEnd(clip_end);

					amis::MediaGroup* p_current;

					if (mbFlagProcessingTitle == true) 
						p_current = mpTitleInfo;
					else if(mbFlagProcessingAuthor == true)
						p_current = mpAuthorInfo;
					
					p_current->addAudioClip(p_audio);
				}
			}
		}
	}
	XMLString::release(&element_name);
}

void amis::io::TitleAuthorParse::endElement( const XMLCh* const uri,
								   const XMLCh* const localname,
								   const XMLCh* const qname)
{
	char* element_name = XMLString::transcode(qname); 

	if (mbFlagGetChars == true && strcmp(element_name, mChardataElm.c_str()) == 0)
		mbFlagGetChars = false;
	
	else if(mbFlagProcessingAuthor == true && strcmp(element_name, "docAuthor") == 0)
		mbFlagProcessingAuthor = false;

	else if(mbFlagProcessingTitle == true && strcmp(element_name, "docTitle") == 0)
		mbFlagProcessingTitle = false;

	XMLString::release(&element_name);	
}


void amis::io::TitleAuthorParse::characters(const XMLCh* const chars, const XMLSize_t length)
{
	if (mbFlagGetChars == true)
	{
		mTempChars.append((wchar_t*)chars);	
		amis::MediaGroup* p_current_media = NULL;
		if (mFileType == NCX)
		{
			if (mbFlagProcessingTitle == true)
				p_current_media = mpTitleInfo;
			else if(mbFlagProcessingAuthor == true)
				p_current_media = mpAuthorInfo;			
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
				p_current_media->getText()->setTextString(mTempChars);
			}
		}
	}
}