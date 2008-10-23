/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2008  DAISY Consortium

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

#include "util/FilePathTools.h"
#include "io/NccFileReader.h"
#include "dtb/Metadata.h"
#include "dtb/nav/NavTarget.h"
#include "dtb/nav/PageTarget.h"
#include <xercesc/sax2/Attributes.hpp>
#include "util/xercesutils.h"

amis::io::NccFileReader::NccFileReader()
{
	mpMetadata = NULL;
	mpCurrentMetaItem = NULL;
}

amis::io::NccFileReader::~NccFileReader()
{
}

amis::dtb::Metadata* amis::io::NccFileReader::getMetadata()
{
	return mpMetadata;
}
bool amis::io::NccFileReader::readFromFile(const ambulant::net::url* filepath)
{
	mbFlag_ProcessMetadataChars = false;
	mpTitle = NULL;
	mpMetadata = new amis::dtb::Metadata();
	return amis::io::NavFileReader::readFromFile(filepath);
}
//!xerces start element event
void amis::io::NccFileReader::startElement(const   XMLCh* const    uri,
		const   XMLCh* const    localname,
		const   XMLCh* const    qname,
		const   Attributes&	attributes)
{
	//get the name of the node from xerces
	char *node_name_ = XMLString::transcode(qname);
	string node_name;
	node_name.assign(node_name_);
	XMLString::release(&node_name_);

	int lvl = isHeading(node_name);
	//if this is a heading node
	if (lvl > 0)
	{
		processHeading(lvl, &attributes);
	}
	else if(node_name.compare("span") == 0)
	{
		processSpan(&attributes);
	}
	else if(node_name.compare("div") == 0)
	{
		processDiv(&attributes);
	}
	else if(node_name.compare("meta") == 0 || 
		node_name.substr(0, 3).compare("dc:") == 0)
	{
		processMetadata(node_name, &attributes);
	}

	//special condition for links due to the nested <a> element
	else if (node_name.compare("a") == 0)
	{
		string href;
		href.assign(SimpleAttrs::get("href", &attributes));
	
		mbFlag_GetChars = true;
		mTempChars.erase();

		if (mListType == 0)
		{
			mpCurrentNavPoint->setContent(href);	
		}
		else if (mListType == 1)
		{
			//nav list
			mpCurrentNavTarget->setContent(href);	
		}
		else if (mListType == 2)
		{
			//page list
			mpCurrentPageTarget->setContent(href);
		}
	}

}

//!xerces end element event
void amis::io::NccFileReader::endElement(const XMLCh* const uri,
	const XMLCh* const localname,
	const XMLCh* const qname)
{
	char* element_name = XMLString::transcode(qname); 
	string tmpstr;
	tmpstr.assign(element_name);
	if (tmpstr.substr(0, 3).compare("dc:") == 0 || tmpstr.compare("meta") == 0)
	{
		if (mbFlag_ProcessMetadataChars == true && mpCurrentMetaItem != NULL)
		{
			//if the meta item has content already, add to it
			//this is because some metadata names might appear more than once, like several authors
			//we don't need to provide atomic navigation for each, so just list them like "A, B, C, ..."
			if (mpCurrentMetaItem->mContent.size() > 0)
			{
				mpCurrentMetaItem->mContent.append(L", ");
				mpCurrentMetaItem->mContent.append(mTempChars);
			}
			else
			{
				mpCurrentMetaItem->mContent.assign(mTempChars);
			}
		}
		mbFlag_ProcessMetadataChars = false;
	}
	
	if (strcmp(element_name, "a") == 0) mbFlag_GetChars = false;
	XMLString::release(&element_name); 
}

void amis::io::NccFileReader::characters(const XMLCh *const chars, const unsigned int length)
{
	if (mbFlag_GetChars == true)
	{
		mTempChars.append((wchar_t*)chars);
		//process metadata characters at endElement because the value of mContent might need to be appended
		if (mbFlag_ProcessMetadataChars == false)
		{
			amis::MediaGroup* p_media_label;

			if (mListType == 0)
				p_media_label = mpCurrentNavPoint->getLabel();
			else if(mListType == 1)
				p_media_label = mpCurrentNavTarget->getLabel();
			else if(mListType == 2)
				p_media_label = mpCurrentPageTarget->getLabel();
			
			amis::TextNode* p_text = new amis::TextNode();
			p_text->setTextString(mTempChars);
			p_media_label->setText(p_text);
		
			//At this point, we could set other text properties that we don't use (RTL, lang, etc)
			//However: we don't use them, so ...
		}
	}
}

void amis::io::NccFileReader::processHeading(int level, const Attributes* pAttrs)
{
	//check and see if we can close an open node
	unsigned int len = mOpenNodes.size();
	if (len != 0)
	{
		amis::dtb::nav::NavPoint* p_tmp = mOpenNodes[len-1];

		//if this heading is lesser/equal to the previous one
		int diff = p_tmp->getLevel() - level + 1;
		if (diff > 0)
		{
			for (int i=0; i<diff; i++)
			{
				//then the previous one(s) will not be accepting children
				mOpenNodes.pop_back();
			}
		}
	}

	mListType = 0;

	mpCurrentNavPoint = new amis::dtb::nav::NavPoint(); 

	string classname;
	string id;
	
	classname.assign(SimpleAttrs::get("class", pAttrs));
	mpCurrentNavPoint->setClass(classname);

	id.assign(SimpleAttrs::get("id", pAttrs)); 
	mpCurrentNavPoint->setId(id);

	mpCurrentNavPoint->setLevel(level);

	if (classname == "title")
		mpTitle = mpCurrentNavPoint;

	//set label (gather text and retrieve audio clip) are done during <a> tag processing
	amis::MediaGroup* p_media_label = new amis::MediaGroup();
	mpCurrentNavPoint->setLabel(p_media_label);

	//set play order
	mpCurrentNavPoint->setPlayOrder(mPlayOrderCount);
	mPlayOrderCount++;

	//get the last item from the open nodes list and set it as the parent for this node
	amis::dtb::nav::NavPoint* p_parent = mOpenNodes.back();
	p_parent->addChild(mpCurrentNavPoint);
	mOpenNodes.push_back(mpCurrentNavPoint);
}

void amis::io::NccFileReader::processSpan(const Attributes* pAttrs)
{
	string classname;
	string id;

	classname.assign(SimpleAttrs::get("class", pAttrs));
	id.assign(SimpleAttrs::get("id", pAttrs));
	if (classname.compare("page-front") == 0 ||
		classname.compare("page-normal") == 0 ||
		classname.compare("page-special") == 0)
	{

		mListType = 2;

		classname = classname.substr(5);
		amis::dtb::nav::PageTarget* p_page = new amis::dtb::nav::PageTarget();
		p_page->setClass(classname);
		p_page->setPlayOrder(mPlayOrderCount);
		p_page->setId(id);
		
		if (classname.compare("front") == 0)
			p_page->setType(amis::dtb::nav::PageTarget::PAGE_FRONT);
		else if(classname.compare("normal") == 0)
			p_page->setType(amis::dtb::nav::PageTarget::PAGE_NORMAL);
		else if(classname.compare("special") == 0)
			p_page->setType(amis::dtb::nav::PageTarget::PAGE_SPECIAL);

		mPlayOrderCount++;

		amis::MediaGroup* p_media_label = new amis::MediaGroup();
		p_page->setLabel(p_media_label);
		
		amis::dtb::nav::PageList* p_page_list;
		p_page_list = mpNavModel->getPageList();

		p_page_list->addNode(p_page);
		mpCurrentPageTarget = p_page;		
		addCustomTest("pagenumber", true, true, "");
	}

	else if(classname.compare("sidebar") == 0 ||
		classname.compare("optional-prodnote") == 0 ||
		classname.compare("noteref") == 0 ||
		classname.compare("group") == 0)
	{
		mListType = 1;

		amis::dtb::nav::NavTarget* p_navt = new amis::dtb::nav::NavTarget();

		p_navt->setClass(classname);
		p_navt->setPlayOrder(mPlayOrderCount);
		p_navt->setId(id);
		
		mPlayOrderCount++;

		amis::MediaGroup* p_media_label = new amis::MediaGroup();
		p_navt->setLabel(p_media_label);

		amis::dtb::nav::NavList* p_nav_list;
		p_nav_list = mpNavModel->getNavList(classname);

		if (p_nav_list == NULL)
		{
			int newidx = mpNavModel->addNavList(classname);
			p_nav_list = mpNavModel->getNavList(newidx);
		}

		p_nav_list->addNode(p_navt);
		mpCurrentNavTarget = p_navt;

		//see if we should add a custom test to the list
		string custom_test;

		if (classname.compare("sidebar") == 0)
			custom_test = "sidebar";
		else if (classname.compare("optional-prodnote") == 0)
			custom_test = "prodnote";
		else if(classname.compare("noteref") == 0)
			custom_test = "footnote";
		else
			custom_test = "";

		if (custom_test.compare("") != 0)
			addCustomTest(custom_test, true, true, "");
	}
}

void amis::io::NccFileReader::processDiv(const Attributes* pAttrs)
{
	mListType = 1;

	amis::dtb::nav::NavTarget* p_navt = new amis::dtb::nav::NavTarget();

	string classname;
	classname.assign(SimpleAttrs::get("class", pAttrs));
	p_navt->setClass(classname);
	p_navt->setPlayOrder(mPlayOrderCount);
	mPlayOrderCount++;
	amis::MediaGroup* p_media_label = new amis::MediaGroup();
	p_navt->setLabel(p_media_label);

	amis::dtb::nav::NavList* p_nav_list;
	p_nav_list = mpNavModel->getNavList(classname);

	if (p_nav_list == NULL)
	{
		int newidx = mpNavModel->addNavList(classname);
		p_nav_list = mpNavModel->getNavList(newidx);
	}
	p_nav_list->addNode(p_navt);
	mpCurrentNavTarget = p_navt;
}

void amis::io::NccFileReader::processMetadata(string tagname, const Attributes* pAttrs)
{
	amis::dtb::MetaItem* p_meta_item = NULL;
	
	//it's a dublin core
	if (tagname.substr(0, 3).compare("dc:") == 0)
	{
		mbFlag_GetChars = true;
		mbFlag_ProcessMetadataChars = true;
		//account for dc:creator already existing - we might just append content to it
		if (tagname == "dc:creator" && mpMetadata->getMetadata("dc:creator") != NULL)
		{
			p_meta_item = mpMetadata->getMetadata("dc:creator");	
		}
		else
		{
			p_meta_item = new amis::dtb::MetaItem();
			p_meta_item->mName.assign(tagname);
		}
	}
	//it's just a plain "meta"
	else 
	{
		p_meta_item = new amis::dtb::MetaItem();
		mbFlag_GetChars = false;
		string meta_name = SimpleAttrs::get("name", pAttrs);
		wstring meta_content = SimpleAttrs::getw("content", pAttrs);
		p_meta_item->mName.assign(meta_name);
		p_meta_item->mContent = meta_content;
	}
	mpCurrentMetaItem = p_meta_item;
	mpMetadata->addMetadata(p_meta_item);
}

//return 1...6 indicating the heading level; otherwise return 0
int amis::io::NccFileReader::isHeading(string nodeName)
{
	//check type
	if (nodeName.compare("h1") == 0 || 
		nodeName.compare("h2") == 0 ||
		nodeName.compare("h3") == 0 ||
		nodeName.compare("h4") == 0 ||
		nodeName.compare("h5") == 0 ||
		nodeName.compare("h6") == 0)
	{
		string str_level = nodeName.substr(1);

		const char* c_level = str_level.c_str();
		int i_level = atoi(c_level);
	
		return i_level;
	}
	else
	{
		return 0;
	}
}
amis::dtb::nav::NavPoint* amis::io::NccFileReader::getTitle()
{
	return mpTitle;
}
