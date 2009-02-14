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

#include "util/Log.h"
#include "io/SmilAudioExtract.h"
#include "util/FilePathTools.h"
#include "util/xercesutils.h"
#include "Media.h"


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

amis::io::SmilAudioExtract::SmilAudioExtract()
{	
}

amis::io::SmilAudioExtract::~SmilAudioExtract()
{
}

amis::AudioNode* amis::io::SmilAudioExtract::getAudioAtId(const ambulant::net::url* filepath)
{	
	mbFlagInPar = false;
	mbFlagFoundId = false;
	mbIsDataExtracted = false;
	mpAudioInfo = NULL;
	mId = filepath->get_ref();
	this->parseFile(filepath);
	
	return mpAudioInfo;
}

void amis::io::SmilAudioExtract::startElement(const   XMLCh* const    uri,
									 const   XMLCh* const    localname,
									 const   XMLCh* const    qname,
									 const   Attributes&	attributes)
{
	if (mbIsDataExtracted == true) return;

	char* element_name = XMLString::transcode(qname);
	
	//if the id has not been found yet and this is a par tag
	if (mbFlagFoundId == false && strcmp(element_name, "par") == 0) 
	{
		mbFlagInPar = true;
		string id = SimpleAttrs::get("id", &attributes);
		//when we find the ID, flag the system to stop looking. 
		//it will stop processing elements past the close of this par.
		if (id.compare(mId) == 0)	
			mbFlagFoundId = true;
	}

	//if we are in a par and this is an audio tag
	else if (mbFlagInPar == true && strcmp(element_name, "audio") == 0)
	{
		if (mpAudioInfo == NULL)
		{
			mpAudioInfo = new amis::AudioNode();
			//record the audio data, it might be useful if this is the par we're looking for
			mpAudioInfo->setClipBegin(SimpleAttrs::get("clip-begin", &attributes));
			mpAudioInfo->setClipEnd(SimpleAttrs::get("clip-end", &attributes));
			string src = SimpleAttrs::get("src", &attributes);
			src = amis::util::FilePathTools::goRelativePath(this->getFilepath()->get_file(), src);
			mpAudioInfo->setPath(src);
		}
	}

	//if we are in a par and this is a text tag
	else if (mbFlagInPar == true && strcmp(element_name, "text") == 0)
	{
		//when we find the ID (could be on the text although not usually),
		//flag the system to stop searching after the close of this text node's
		//parent par.
		string id = SimpleAttrs::get("id", &attributes);
		if (id.compare(mId) == 0)	
			mbFlagFoundId = true;
	}
	XMLString::release(&element_name);	
}

void amis::io::SmilAudioExtract::endElement( const XMLCh* const uri,
								   const XMLCh* const localname,
								   const XMLCh* const qname)
{
	char* element_name = XMLString::transcode(qname);
	
	//if this element is a par, then create all data collected since the open-par tag
	//and add it to the audio list
	if (strcmp(element_name, "par") == 0)
	{
		mbFlagInPar = false;
		//if we found the id, set a flag to not process any more data
		if (mbFlagFoundId == true)
			mbIsDataExtracted = true;
	}
	
	XMLString::release(&element_name);
}