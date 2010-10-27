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

#include "util/FilePathTools.h"
#include "dtb/CustomTest.h"
#include "io/NavFileReader.h"

#include <iostream>
#include <fstream>


amis::io::NavFileReader::NavFileReader()
{
	mpNavModel = NULL;
	mpCurrentNavPoint = NULL;
	mpCurrentNavTarget = NULL;
	mpCurrentPageTarget = NULL;
	mpCustomTests = NULL;
}

amis::io::NavFileReader::~NavFileReader()
{
}

amis::dtb::nav::NavModel* amis::io::NavFileReader::getNavModel()
{
	return mpNavModel;
}

amis::dtb::CustomTestSet* amis::io::NavFileReader::getCustomTests()
{
	return mpCustomTests;
}
bool amis::io::NavFileReader::readFromFile(const ambulant::net::url* filepath)
{
	this->mPlayOrderCount = 1;
	this->mbFlag_GetChars = false;
	
	this->mpCurrentNavPoint = NULL;
	this->mpCurrentNavTarget = NULL;
	this->mpCurrentPageTarget = NULL;
	this->mOpenNodes.clear();
	this->mTempChars.erase();

	mpNavModel = new amis::dtb::nav::NavModel();
	mpNavModel->setAreFilenamesLowercase(mbLower);
	mpCustomTests = new amis::dtb::CustomTestSet();

	mOpenNodes.clear();
	mpCurrentNavPoint = NULL;

	//push the root onto the open nodes list
	mOpenNodes.push_back(mpNavModel->getNavMap()->getRoot());

	if (!this->parseFile(filepath))
	{
		delete mpNavModel;
		return false;
	}
	else
	{
		return true;
	}
}

void amis::io::NavFileReader::addCustomTest(string id, bool override, 
								  bool defaultState, string bookStruct)
{
	amis::dtb::CustomTest* p_custom_test = new amis::dtb::CustomTest();
	p_custom_test->setId(id);
	p_custom_test->setOverride(override);
	p_custom_test->setDefaultState(defaultState);
	p_custom_test->setBookStruct(bookStruct);
	p_custom_test->setCurrentState(defaultState);

	//the custom test will only be added if it doesn't already exist
	//if it already exists, the addCustomTest function returns false
	//and we should delete the custom test we just created
	if (!mpCustomTests->addCustomTest(p_custom_test)) delete p_custom_test;
}

void amis::io::NavFileReader::setAreFilenamesLowercase(bool val)
{
	mbLower = val;
}