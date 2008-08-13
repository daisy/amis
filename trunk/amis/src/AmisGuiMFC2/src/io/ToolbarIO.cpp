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

#include "io/ToolbarIO.h"

#include "util/FilePathTools.h"
#include "util/xercesutils.h"
#include <iostream>
#include "gui/AmisApp.h"

#include "gui/self-voicing/datamodel/IniFile.h"

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

using namespace amis::io;

ToolbarIO::ToolbarIO()
{
}
ToolbarIO::~ToolbarIO()
{
}
amis::gui::toolbar::ToolbarSettings* ToolbarIO::getToolbarSettings()
{
	return mpToolbarSettings;
}

//this file is local! and the address does not look like a URL.
bool ToolbarIO::readFromFile(string filepath)
{
	mbFlagInToggle = false;
	mpCurrentToggle = NULL;
	int sz = mItems.size();
	for (int i = sz - 1; i>=0; i--)
	{
		amis::gui::toolbar::ToolbarItem* p_item = mItems[i];
		mItems.pop_back();
		delete p_item;
	}
	mpToolbarSettings = NULL;
	ambulant::net::url tmp_url = ambulant::net::url::from_filename(filepath);
	bool ret = parseFile(&tmp_url);
	return ret;
}

//--------------
//xerces' start element function
void ToolbarIO::startElement(const   XMLCh* const    uri,
		const   XMLCh* const    localname,
		const   XMLCh* const    qname,
		const   Attributes&	attributes)
{
	char* node_name = XMLString::transcode(qname); 
	
	if (strcmp(node_name, "toolbar") == 0)
	{
		mpToolbarSettings = new amis::gui::toolbar::ToolbarSettings();
		string view = SimpleAttrs::get("view", &attributes);
		string placement = SimpleAttrs::get("placement", &attributes);
		string button_height = SimpleAttrs::get("buttonHeight", &attributes);
		string button_width = SimpleAttrs::get("buttonWidth", &attributes);
		int height, width;
		height = atoi(button_height.c_str());
		width = atoi(button_width.c_str());
		mpToolbarSettings->setButtonResolution(width, height);

		if (placement == "bottom") mpToolbarSettings->setPlacement(amis::gui::toolbar::BOTTOM);
		else if (placement == "left") mpToolbarSettings->setPlacement(amis::gui::toolbar::LEFT);
		else if (placement == "right") mpToolbarSettings->setPlacement(amis::gui::toolbar::RIGHT);
		else mpToolbarSettings->setPlacement(amis::gui::toolbar::TOP);

		if (view == "basic") mpToolbarSettings->setView(amis::gui::toolbar::BASIC);
		else mpToolbarSettings->setView(amis::gui::toolbar::DEFAULT);
	}
	else if (strcmp(node_name, "button") == 0)
	{
		amis::gui::toolbar::ToolbarButton* p_button = new amis::gui::toolbar::ToolbarButton();
		string image_path = SimpleAttrs::get("src", &attributes);
		string mfcid = SimpleAttrs::get("mfcid", &attributes);
		string id = SimpleAttrs::get("id", &attributes);
		ambulant::net::url image_url = ambulant::net::url::from_filename(image_path);
		const ambulant::net::url* base_file = getFilepath();
		image_url = image_url.join_to_base(*base_file);
		UINT n_mfcid = getIdForCommandString(mfcid);
		p_button->setImageSrc(image_url);
		p_button->setCommandId(n_mfcid);
		p_button->setId(id);
		if (mbFlagInToggle == true)
			mpCurrentToggle->addButton(p_button);
		else
			mpToolbarSettings->addItem(p_button);
	}
	else if (strcmp(node_name, "toggle") == 0)
	{
		mpCurrentToggle = new amis::gui::toolbar::ToolbarToggleButton();
		mbFlagInToggle = true;
		mpToolbarSettings->addItem(mpCurrentToggle);
	}
	else if (strcmp(node_name, "separator") == 0)
	{
		amis::gui::toolbar::ToolbarSeparator* p_separator = new amis::gui::toolbar::ToolbarSeparator();
		mpToolbarSettings->addItem(p_separator);
	}
	XMLString::release(&node_name);
}

void ToolbarIO::endElement(const XMLCh* const uri,
	const XMLCh* const localname,
	const XMLCh* const qname)
{
	char* node_name = XMLString::transcode(qname); 	
	if (strcmp(node_name, "toggle") == 0) mbFlagInToggle = false;
	XMLString::release(&node_name); 
}

UINT ToolbarIO::getIdForCommandString(string mfcid)
{
	int int_value;
	//TODO: formalize the location of this file
	string resourcefile = theApp.getAppSettingsPath() + "/resource.h.ini";
	string string_value = amis::gui::spoken::CIniFile::GetValue(mfcid, "resources", resourcefile);
	CIniFile::string2int(string_value.c_str(), int_value);
	return int_value;
}