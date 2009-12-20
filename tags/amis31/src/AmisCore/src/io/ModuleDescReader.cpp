/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

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

#include "io/ModuleDescReader.h"
#include "util/FilePathTools.h"
#include "util/xercesutils.h"
#include <iostream>


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

amis::io::ModuleDescReader::ModuleDescReader()
{
}

amis::io::ModuleDescReader::~ModuleDescReader()
{
}

amis::ModuleDescData* amis::io::ModuleDescReader::getModuleDescData()
{
	return mpData;
}

bool amis::io::ModuleDescReader::readFromFile(const ambulant::net::url* filepath)
{
	mpData = new amis::ModuleDescData();
	mb_flagGetChars = false;

	if (!this->parseFile(filepath))
	{
		delete mpData;
		return false;
	}
	else
	{
		mpData->setXmlFileName(this->mFilepath);
		return true;
	}
}


//--------------------------------------------------
//xerces start element event
//--------------------------------------------------
void amis::io::ModuleDescReader::startElement(const   XMLCh* const    uri,
				const   XMLCh* const    localname,
				const   XMLCh* const    qname,
				const   Attributes&	attributes)
{
	char* element_name;
	element_name = XMLString::transcode(qname);

	if (strcmp(element_name, "moduleDesc") == 0)
	{
		string id;
		id.assign(SimpleAttrs::get("id", &attributes));
		this->mpData->setId(id);

		string filename;
		filename.assign(SimpleAttrs::get("filename", &attributes));
		this->mpData->setDllFileName(filename);

		string type;
		type.assign(SimpleAttrs::get("type", &attributes));
		if (type.compare("plugin") == 0)
			this->mpData->setModuleType(amis::ModuleDescData::PLUGIN);
		else if (type.compare("langpack") == 0)
			this->mpData->setModuleType(amis::ModuleDescData::LANGPACK);
		
		string enabled;
		enabled.assign(SimpleAttrs::get("enabled", &attributes));
		if (enabled.compare("yes") == 0)
			this->mpData->setIsEnabled(true);
		else
			this->mpData->setIsEnabled(false);
	}
	else if(strcmp(element_name, "label") == 0)
	{
		mpTempLabel = new amis::MediaGroup();
	}
	else if(strcmp(element_name, "audio") == 0)
	{
		amis::AudioNode* p_audio = new amis::AudioNode();

		string src;
		src.assign(SimpleAttrs::get("src", &attributes));
		ambulant::net::url url_src = ambulant::net::url::from_filename(src);
		url_src = url_src.join_to_base(*this->getFilepath());
		p_audio->setPath(url_src.get_file());
	
		string clipBegin;
		clipBegin.assign(SimpleAttrs::get("clipBegin", &attributes));
		p_audio->setClipBegin(clipBegin);

		string clipEnd;
		clipEnd.assign(SimpleAttrs::get("clipEnd", &attributes));
		p_audio->setClipEnd(clipEnd);

		mpTempLabel->addAudioClip(p_audio);
	}
	else if(strcmp(element_name, "text") == 0)
	{
		amis::TextNode* p_text;
		p_text = new amis::TextNode();
		mpTempLabel->setText(p_text);
		mb_flagGetChars = true;
	}
	else if(strcmp(element_name, "image") == 0)
	{
		amis::ImageNode* p_img = new amis::ImageNode();
		
		string src;
		src.assign(SimpleAttrs::get("src", &attributes));
		p_img->setPath(src);

		mpTempLabel->setImage(p_img);
	}

	XMLString::release(&element_name);
}

//--------------------------------------------------
//(SAX Event) close this element
//--------------------------------------------------
void amis::io::ModuleDescReader::endElement( const XMLCh* const uri,
								   const XMLCh* const localname,
								   const XMLCh* const qname)
{
	char* element_name = XMLString::transcode(qname); 

	if (strcmp(element_name, "label") == 0)
	{
		this->mpData->setLabel(mpTempLabel);
	}
	else if (strcmp(element_name, "text") == 0)
	{
		amis::TextNode* p_text;
		p_text = this->mpTempLabel->getText();
		p_text->setTextString(mTempWChars);

	}

	mb_flagGetChars = false;
	mTempWChars.erase();

	XMLString::release(&element_name); 
}

//--------------------------------------------------
//xerces character data event
//--------------------------------------------------
void amis::io::ModuleDescReader::characters(const XMLCh* const chars, const XMLSize_t length)
{
	if (mb_flagGetChars == true)
	{	
		mTempWChars.append((wchar_t*)chars);	
	}
}
