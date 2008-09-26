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

#include "io/XercesSaxParseBase.h"
#include "util/FilePathTools.h"
#include "io/UrlInputSource.h"
#include <iostream>
#include "util/Log.h"

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
#include <xercesc/framework/LocalFileInputSource.hpp>
using namespace std;

amis::io::XercesSaxParseBase::XercesSaxParseBase()
{
	mError.clear();
}
amis::io::XercesSaxParseBase::~XercesSaxParseBase()
{
}
const ambulant::net::url* amis::io::XercesSaxParseBase::getFilepath()
{
	return &mFilepath;
}

bool amis::io::XercesSaxParseBase::parseFile(const ambulant::net::url* filepath)
{
	SAX2XMLReader* parser;
	mFilepath = *filepath;
	amis::util::Log::Instance()->writeMessage("Parsing file: ", filepath, "XercesSaxParseBase::parseFile");
	//try-catch block for Xerces platform utilities
	try
	{
		XMLPlatformUtils::Initialize();
	}
	
	catch (const XMLException& toCatch)
	{
		mError.setCode(amis::PARSE_ERROR);
		char *msg = XMLString::transcode(toCatch.getMessage());
		string str;
		str.assign(msg);	
		mError.setMessage(str);
		XMLString::release(&msg);
		amis::util::Log::Instance()->writeError(mError, "XercesSaxParseBase::parseFile");
		return false;
	}
		
	//assuming we've made it this far, create a new parser
	parser = XMLReaderFactory::createXMLReader();

	//set these parser features to turn off DTD loading and validation
	parser->setFeature(XMLUni::fgXercesLoadExternalDTD, false);
	parser->setFeature(XMLUni::fgSAX2CoreValidation, false);
	
	amis::io::UrlInputSource* p_input_source = amis::io::NewUrlInputSource(mFilepath);
	if (p_input_source == NULL)
	{
		mError.setCode(amis::PARSE_ERROR);
		string str;
		str.assign("Could not create input source for " + mFilepath.get_url());	
		mError.setMessage(str);
		delete p_input_source;
		delete parser;
		XMLPlatformUtils::Terminate();
		amis::util::Log::Instance()->writeError(mError, "XercesSaxParseBase::parseFile");
		return false;
	}

	//try-catch block for Xerces parsing
	try 
	{
		//give the sax parser pointers to our content and error handler object
		parser->setContentHandler(this);
		parser->setErrorHandler(this);
		//parser begins parsing; expect SAX Events soon
		parser->parse((*p_input_source));
	}
	
	catch (const XMLException& toCatch)
	{	
		mError.setCode(amis::PARSE_ERROR);
		char *msg = XMLString::transcode(toCatch.getMessage());
		string str;
		str.assign(msg);	
		mError.setMessage(str);
		XMLString::release(&msg);
		delete p_input_source;
		delete parser;
		XMLPlatformUtils::Terminate();
		amis::util::Log::Instance()->writeError(mError, "XercesSaxParseBase::parseFile");
		return false;
	}
	
	delete p_input_source;
	delete parser;

	try
	{
		XMLPlatformUtils::Terminate();
	}
	catch (...)//const XMLException& toCatch)
	{
		amis::util::Log::Instance()->writeWarning("Exception while terminating XMLPlatformUtils", 
			"XercesSaxParseBase::parseFile");
	}

	if (mError.getCode() != amis::OK) return false;
	amis::util::Log::Instance()->writeMessage("Done parsing.", "XercesSaxParseBase::parseFile");
	return true;

}
amis::Error amis::io::XercesSaxParseBase::getError()
{
	return mError;
}

void amis::io::XercesSaxParseBase::warning(const SAXParseException& e)
{
	char* xerces_msg = XMLString::transcode(e.getMessage());
	long line = e.getLineNumber();
	char ch_line[10];
	itoa(line, ch_line, 10);
	string msg = "Parse warning: \n\t" + mFilepath.get_url() + "\n\tline: " + ch_line +  "\n\t" + xerces_msg;
	amis::util::Log::Instance()->writeWarning(msg, "XercesSaxParseBase::error");
	XMLString::release(&xerces_msg);
}

void amis::io::XercesSaxParseBase::error(const SAXParseException& e)
{
	char* xerces_msg = XMLString::transcode(e.getMessage());
	long line = e.getLineNumber();
	char ch_line[10];
	itoa(line, ch_line, 10);
	string msg = "Parse error: \n\t" + mFilepath.get_url() + "\n\tline: " + ch_line +  "\n\t" + xerces_msg;
	amis::util::Log::Instance()->writeError(msg, "XercesSaxParseBase::error");
	XMLString::release(&xerces_msg);
}

void amis::io::XercesSaxParseBase::fatalError(const SAXParseException& e)
{	
	char* xerces_msg = XMLString::transcode(e.getMessage());
	long line = e.getLineNumber();
	char ch_line[10];
	itoa(line, ch_line, 10);
	string msg = "Parse fatal error: \n\t" + mFilepath.get_url() + "\n\tline: " + ch_line +  "\n\t" + xerces_msg;
	amis::util::Log::Instance()->writeError(msg, "XercesSaxParseBase::error");
	mError.setCode(amis::PARSE_ERROR);
	mError.setMessage(msg);
	XMLString::release(&xerces_msg);
}
