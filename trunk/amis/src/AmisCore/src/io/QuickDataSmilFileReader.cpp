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

//SYSTEM INCLUDES
#include <string>
#include <iostream>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/HandlerBase.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

#include "io/QuickDataSmilFileReader.h"
#include "util/xercesutils.h"
#include "util/FilePathTools.h"
#include "dtb/nav/NavNode.h"
#include "dtb/smil/NodeFactory.h"
using namespace std;

amis::io::QuickDataSmilFileReader::QuickDataSmilFileReader()
{
	this->mpList = NULL;
	this->mpNodes =  NULL;
	this->mpStringMap = NULL;
}

amis::io::QuickDataSmilFileReader::~QuickDataSmilFileReader()
{
}

//--------------------------------------------------
/*!
	Load a SMIL file and start the Xerces SAX2 parser.
*/
//--------------------------------------------------
bool amis::io::QuickDataSmilFileReader::readFromFile(const ambulant::net::url* filepath, StdStringList* pList, StringMap* pTextMap, amis::dtb::nav::NavNodeList* pNodes)
{
	mpList = pList;
	mFilename = amis::util::FilePathTools::getFileName(filepath->get_path());
	mpStringMap = pTextMap;
	mpNodes = pNodes;
	mbFlag_GetNextAudio = false;

	if (!this->parseFile(filepath))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void amis::io::QuickDataSmilFileReader::startElement(const   XMLCh* const    uri,
				const   XMLCh* const    localname,
				const   XMLCh* const    qname,
				const   Attributes&	attributes)
{
	char* element_name = XMLString::transcode(qname);
	
	if (isSupported(element_name))
	{
		//save all smil element IDs
		string id;
		id.assign(SimpleAttrs::get("id", &attributes));
		if (id.size() > 0)
		{
			id = mFilename + "#" + id;
			mpList->push_back(id);
		}

		//save the text src/id info for fast indexing later
		if (strcmp(element_name, "text") == 0)
		{
			string src;
			src.assign(SimpleAttrs::get("src", &attributes));
			(*mpStringMap)[src] = mpList->back();
		}

		//if we need to fill in node data while we're in this file
		if (mpNodes != NULL && mpNodes->empty() == false)
		{
			//see if we have a node to get audio data for
			amis::dtb::nav::NavNode* p_node = NULL;
			string search = mFilename + "#" + id;
			p_node = matchesNodeInList(search);
			if (p_node)
			{
				mbFlag_GetNextAudio = true;
			}
			//if we are at an audio node
			if (mbFlag_GetNextAudio == true && strcmp(element_name, "audio") == 0)
			{
				amis::dtb::smil::NodeFactory factory;
				amis::AudioNode* p_audio = (amis::AudioNode*)factory.createNode(amis::dtb::smil::AUD, &attributes);
				if (p_node->getLabel() != NULL)
					p_node->getLabel()->addAudioClip(p_audio);
				else
					delete p_audio;
				mbFlag_GetNextAudio = false;
			}
		}
	}

	XMLString::release(&element_name); 
}

void amis::io::QuickDataSmilFileReader::endElement( const XMLCh* const uri,
					const XMLCh* const localname,
					 const XMLCh* const qname)
{
	//char* element_name = XMLString::transcode(qname);
	//XMLString::release(&element_name);
}


void amis::io::QuickDataSmilFileReader::characters(const XMLCh *const chars, const unsigned int length)
{
}

bool amis::io::QuickDataSmilFileReader::isSupported(string elementName)
{
	if (elementName.compare("seq") == 0 ||
		elementName.compare("par") == 0 ||
		elementName.compare("audio") == 0 ||
		elementName.compare("text") == 0 ||
		elementName.compare("image") == 0)
	{
		return true;
	}
	else
	{
		return false;
	}

}

amis::dtb::nav::NavNode* amis::io::QuickDataSmilFileReader::matchesNodeInList(string content)
{
  bool b_found = false;
  int i = 0;

	for (i = 0; i<mpNodes->size(); i++)
	{
		if ((*mpNodes)[i]->getContent() == content)
		{
			b_found = true;
			break;
		}
	}

	if (b_found) return (*mpNodes)[i];
	else return NULL;
}
