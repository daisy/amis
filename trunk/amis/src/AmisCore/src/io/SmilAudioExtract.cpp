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

//SYSTEM INCLUDES
#include <string>
#include <iostream>
#include <vector>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/HandlerBase.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

//PROJECT INCLUDES
#include "FilePathTools.h"
#include "SmilAudioExtract.h"

using namespace std;


//--------------------------------------------------
//constructor
//--------------------------------------------------
amis::SmilAudioExtract::SmilAudioExtract()
{
	
}

//--------------------------------------------------
//destructor
//--------------------------------------------------
amis::SmilAudioExtract::~SmilAudioExtract()
{
}

//--------------------------------------------------
/*!
	@param[in] filepath 
		the full path to a smil file with an id target on the end
*/
//--------------------------------------------------
amis::AudioNode* amis::SmilAudioExtract::getAudioAtId(string filepath)
{	
	//local variables
	const char* cp_file;
	string tmp_string;
	SAX2XMLReader* parser;
	
	mb_flagInPar = false;
	mb_flagFoundId = false;
	mb_flagFinished = false;

	mpAudioInfo = NULL;

	mId = amis::FilePathTools::getTarget(filepath);
	
	mFilePath = amis::FilePathTools::getAsLocalFilePath(filepath);
	mFilePath = amis::FilePathTools::clearTarget(filepath);

	//do a SAX parse of this new file
	
	cp_file = mFilePath.c_str();
	
	//try-catch block for Xerces platform utilities
	try
	{
		XMLPlatformUtils::Initialize();
	}
	
	catch (...)//const XMLException& toCatch)
	{
		//return empty object
		return mpAudioInfo;
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
		
		//return empty object
		return mpAudioInfo;
	}
	
	//we're done with the parser so delete it and terminate the XML platform utilities
	delete parser;
	XMLPlatformUtils::Terminate();
	
	return mpAudioInfo;
}

//SAX METHODS
//--------------------------------------------------
//! (SAX Event) analyze the element type and collect data to build a node
//--------------------------------------------------
void amis::SmilAudioExtract::startElement(const   XMLCh* const    uri,
									 const   XMLCh* const    localname,
									 const   XMLCh* const    qname,
									 const   Attributes&	attributes)
{
	if (mb_flagFinished == true)
	{
		return;
	}

	//local variables
	char* element_name;
	string tmp_string;

	int len = attributes.getLength();
	
	//get the element name as a string
	element_name = XMLString::transcode(qname); //DANIEL MEMORY LEAK VERIFIED

	mpAttributes = &attributes;

	//if the id has not been found yet and this is a par tag
	if (mb_flagFoundId == false && 
		strcmp(element_name, SMIL_TAG_PAR) == 0) 
	{
		mb_flagInPar = true;

		tmp_string = getAttributeValue(SMIL_ATTR_ID);
		//when we find the ID, flag the system to stop looking. 
		//it will stop processing elements past the close of this par.
		if (tmp_string.compare(mId) == 0)
		{
			mb_flagFoundId = true;
		}
	}

	//if we are in a par and this is an audio tag
	else if (mb_flagInPar == true &&
		strcmp(element_name, SMIL_TAG_AUDIO) == 0)
	{
		if (mpAudioInfo == NULL)
		{
			mpAudioInfo = new amis::AudioNode();

			//record the audio data, it might be useful if this is the par we're looking for
			mpAudioInfo->setClipBegin(getAttributeValue(SMIL_ATTR_CLIPBEGIN));
			mpAudioInfo->setClipEnd(getAttributeValue(SMIL_ATTR_CLIPEND));

			tmp_string = getAttributeValue(SMIL_ATTR_SRC);
			tmp_string = amis::FilePathTools::goRelativePath(mFilePath, tmp_string);
			mpAudioInfo->setSrc(tmp_string);
		}
	}

	//if we are in a par and this is a text tag
	else if (mb_flagInPar == true && 
		strcmp(element_name, SMIL_TAG_TEXT) == 0)
	{
		//when we find the ID (could be on the text although not usually),
		//flag the system to stop searching after the close of this text node's
		//parent par.
		tmp_string = getAttributeValue(SMIL_ATTR_ID);
		if (tmp_string.compare(mId) == 0)
		{
			mb_flagFoundId = true;
		}
	}

	else
	{
		//empty
	}

	
	XMLString::release(&element_name); //DANIEL MEMORY LEAK VERIFIED
	
} //end SmilTreeBuilder::startElement function


//--------------------------------------------------
//! (SAX Event) close this element so that no additional child nodes are added to it in the Smil Tree
//--------------------------------------------------
void amis::SmilAudioExtract::endElement( const XMLCh* const uri,
								   const XMLCh* const localname,
								   const XMLCh* const qname)
{
	//local variable
	char* element_name = XMLString::transcode(qname); //DANIEL MEMORY LEAK VERIFIED
	
	//if this element is a par, then create all data collected since the open-par tag
	//and add it to the audio list
	if (strcmp(element_name, SMIL_TAG_PAR) == 0)
	{
		mb_flagInPar = false;

		//if we found the id, set a flag to not process any more data
		if (mb_flagFoundId == true)
		{
			mb_flagFinished = true;
		}
	}
	
	XMLString::release(&element_name); //DANIEL MEMORY LEAK VERIFIED
	
}


//--------------------------------------------------
//! (SAX Event) error
//--------------------------------------------------
void amis::SmilAudioExtract::error(const SAXParseException& e)
{
	//	cerr<<"error"<<endl;
}

//--------------------------------------------------
//! (SAX Event) fatal error
//--------------------------------------------------
void amis::SmilAudioExtract::fatalError(const SAXParseException& e)
{
	//	cerr<<"fatal error"<<endl;
	
}

//--------------------------------------------------
//! (SAX Event) warning
//--------------------------------------------------
void amis::SmilAudioExtract::warning(const SAXParseException& e)
{
	//	cerr<<"warning"<<endl;
}


//---------------------------------
//utility function
//---------------------------------
string amis::SmilAudioExtract::getAttributeValue(string attributeName)
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
