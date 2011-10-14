/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

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
#include "io/XercesDomWriter.h"

#include "util/xercesutils.h"
#include <iostream>


//XERCES INCLUDES
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

using namespace std;


amis::io::XercesDomWriter::XercesDomWriter()
{
	mpDoc = NULL;
	mpImpl = NULL;
}
amis::io::XercesDomWriter::~XercesDomWriter()
{	
	delete mpDoc;
	XMLPlatformUtils::Terminate();
}
xercesc_3_0::DOMDocument* amis::io::XercesDomWriter::getDocument()
{
	return mpDoc;
}
bool amis::io::XercesDomWriter::initialize(std::string rootElementName)
{
    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch(...)
    {
        return false;
    }
	
	string core = "Core";
	string ns = "http://amisproject.org";
	mpImpl = DOMImplementationRegistry::getDOMImplementation(X(core.c_str()));
	mpDoc = mpImpl->createDocument(X(ns.c_str()), X(rootElementName.c_str()), 0);							

	return true;	
}

//localFileName must contain a valid directory
void amis::io::XercesDomWriter::writeToFile(string localFileName)
{
	DOMLSSerializer* p_writer = ((DOMImplementationLS*)mpImpl)->createLSSerializer();
	DOMLSOutput* p_output = ((DOMImplementationLS*)mpImpl)->createLSOutput(); 
	XMLFormatTarget* p_form_target = new LocalFileFormatTarget(localFileName.c_str());
	p_output->setEncoding(X("utf-8"));
	p_output->setSystemId(X("amis"));
	p_output->setByteStream(p_form_target);
	
	p_writer->write(mpDoc, p_output);
	
	p_output->release();
	p_writer->release();
	delete p_form_target;
//	delete p_output;
//	delete p_writer;
}

//useful function to write any DOM to file, not just the one created by XercesDomWriter
void amis::io::XercesDomWriter::writeToFile(xercesc_3_0::DOMDocument* doc, string localFileName)
{
	string core = "Core";
	mpDoc = doc;
	mpImpl = DOMImplementationRegistry::getDOMImplementation(X(core.c_str()));
	writeToFile(localFileName);
}