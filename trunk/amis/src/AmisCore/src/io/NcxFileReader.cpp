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

#include "util/FilePathTools.h"
#include "io/NcxFileReader.h"
#include "dtb/Metadata.h"
#include "dtb/nav/NavTarget.h"
#include "dtb/nav/PageTarget.h"
#include <xercesc/sax2/Attributes.hpp>
#include "util/xercesutils.h"

amis::io::NcxFileReader::NcxFileReader()
{
}

amis::io::NcxFileReader::~NcxFileReader()
{
}

bool amis::io::NcxFileReader::readFromFile(const ambulant::net::url* filepath)
{
	mpCurrentMediaGroup = NULL;
	mpTitle = NULL;
	mpAuthor = NULL;
	return amis::io::NavFileReader::readFromFile(filepath);
}

amis::MediaGroup* amis::io::NcxFileReader::getTitle()
{
	return mpTitle;
}

amis::MediaGroup* amis::io::NcxFileReader::getAuthor()
{
	return mpAuthor;
}

//!xerces start element event
void amis::io::NcxFileReader::startElement(const   XMLCh* const    uri,
		const   XMLCh* const    localname,
		const   XMLCh* const    qname,
		const   Attributes&	attributes)
{
	char *node_name_ = XMLString::transcode(qname);
	string node_name;
	node_name.assign(node_name_);
	XMLString::release(&node_name_);

	if (node_name.compare("navMap") == 0)
	{
		mListType = 0;
	}
	//if this is a heading node
	else if (node_name.compare("navPoint") == 0)
	{ 
		processHeading(&attributes);
	}
	else if(node_name.compare("docTitle") == 0)
	{
		mpTitle = new amis::MediaGroup();
		mpCurrentMediaGroup = mpTitle;
	}
	else if(node_name.compare("docAuthor") == 0)
	{
		mpAuthor = new amis::MediaGroup();	
		mpCurrentMediaGroup = mpAuthor;
	}
	else if(node_name.compare("navLabel") == 0)
	{
		processNavLabel(&attributes);
	}
	else if(node_name.compare("text") == 0)
	{
		processText(&attributes);
	}
	else if(node_name.compare("audio") == 0)
	{
		processAudio(&attributes);
	}
	else if(node_name.compare("navTarget") == 0)
	{
		processNavTarget(&attributes);
	}
	else if(node_name.compare("pageTarget") == 0)
	{
		processPageTarget(&attributes);
	}
	else if(node_name.compare("content") == 0)
	{
		string src;
		src.assign(SimpleAttrs::get("src", &attributes));
		if (mListType == 0)
			mpCurrentNavPoint->setContent(src);
		else if(mListType == 2)
			mpCurrentPageTarget->setContent(src);
		else if(mListType == 1)
			mpCurrentNavTarget->setContent(src);
	}
	else if(node_name.compare("navList") == 0)
	{
		mListType = 1;
		string class_value;
		class_value.assign(SimpleAttrs::get("class", &attributes));
		int idx = mpNavModel->addNavList(class_value);
		amis::dtb::nav::NavList* p_list = mpNavModel->getNavList(idx);
	}
	else if(node_name.compare("pageList") == 0)
	{
		mListType = 2;
		amis::dtb::nav::PageList* p_list = mpNavModel->getPageList();
	}
	else if(node_name.compare("navInfo") == 0)
	{
		mpCurrentMediaGroup = new amis::MediaGroup();
		if (mListType == 0)
			mpNavModel->getNavMap()->setNavInfo(mpCurrentMediaGroup);
		else if (mListType == 2)
			mpNavModel->getPageList()->setNavInfo(mpCurrentMediaGroup);
		else if (mListType == 1)
			mpNavModel->getNavLists()->back()->setNavInfo(mpCurrentMediaGroup);
	}
	else if(node_name.compare("smilCustomTest") == 0)
	{
		processCustomTest(&attributes);
	}
}

//!xerces end element event
void amis::io::NcxFileReader::endElement(const XMLCh* const uri,
	const XMLCh* const localname,
	const XMLCh* const qname)
{
	char* element_name = XMLString::transcode(qname);

	if (strcmp(element_name, "navPoint") == 0)
	{
		mOpenNodes.pop_back();
		mpCurrentNavPoint = NULL;
	}
	else if(strcmp(element_name, "navTarget") == 0)
	{
		mpCurrentNavTarget = NULL;
	}
	else if(strcmp(element_name, "pageTarget") == 0)
	{
		mpCurrentPageTarget = NULL;
	}
	else if(strcmp(element_name, "text") == 0)
	{
		mTempChars.clear();
		mbFlag_GetChars = false;
	}
	
	XMLString::release(&element_name);
}

void amis::io::NcxFileReader::characters(const XMLCh *const chars, const unsigned int length)
{
	if (mbFlag_GetChars == true)
	{
		if (mpCurrentMediaGroup != NULL)
		{
			amis::TextNode* p_text = NULL;
			p_text = mpCurrentMediaGroup->getText();
			if (p_text != NULL)
			{
				mTempChars.append((wchar_t*)chars);
				p_text->setTextString(mTempChars);	
			}
		}
	}
}

void amis::io::NcxFileReader::processHeading(const Attributes* pAttrs)
{
	mpCurrentNavPoint = new amis::dtb::nav::NavPoint(); 

	int lvl = mOpenNodes.size();
	mpCurrentNavPoint->setLevel(lvl);
	
	string class_value;
	class_value.assign(SimpleAttrs::get("class", pAttrs));
	mpCurrentNavPoint->setClass(class_value);

	string id;
	id.assign(SimpleAttrs::get("id", pAttrs));
	mpCurrentNavPoint->setId(id);

	string play_order;
	play_order.assign(SimpleAttrs::get("playOrder", pAttrs));
	int i_play_order = atoi(play_order.c_str());
	mpCurrentNavPoint->setPlayOrder(i_play_order);

	//get the last item from the open nodes list and set it as the parent for this node
	int len = mOpenNodes.size();
	amis::dtb::nav::NavPoint* p_parent = mOpenNodes[len-1];

	p_parent->addChild(mpCurrentNavPoint);
	mOpenNodes.push_back(mpCurrentNavPoint);
}

void amis::io::NcxFileReader::processNavLabel(const Attributes* pAttrs)
{
	mpCurrentMediaGroup = new amis::MediaGroup();
	if (mpCurrentNavPoint != NULL)
	{
		mpCurrentNavPoint->setLabel(mpCurrentMediaGroup);
	}
	else if (mpCurrentNavTarget != NULL)
	{
		mpCurrentNavTarget->setLabel(mpCurrentMediaGroup);
	}
	else if (mpCurrentPageTarget != NULL)
	{
		mpCurrentPageTarget->setLabel(mpCurrentMediaGroup);
	}
	else
	{
		if (mListType == 0)
			mpNavModel->getNavMap()->setLabel(mpCurrentMediaGroup);
		else if (mListType == 2)
			mpNavModel->getPageList()->setLabel(mpCurrentMediaGroup);
		else if(mListType == 1)
			mpNavModel->getNavLists()->back()->setLabel(mpCurrentMediaGroup);
	}
}

void amis::io::NcxFileReader::processText(const Attributes* pAttrs)
{
	if (mpCurrentMediaGroup != NULL)
	{
		amis::TextNode* p_text = new amis::TextNode();
		mTempChars.clear();
		mpCurrentMediaGroup->setText(p_text);
		this->mbFlag_GetChars = true;
	}
}
void amis::io::NcxFileReader::processAudio(const Attributes* pAttrs)
{
	if (mpCurrentMediaGroup != NULL)
	{
		amis::AudioNode* p_audio = new amis::AudioNode();
		
		string src;
		src.assign(SimpleAttrs::get("src", pAttrs));
		p_audio->setSrc(src);

		string clip_begin;
		clip_begin.assign(SimpleAttrs::get("clipBegin", pAttrs));
		p_audio->setClipBegin(clip_begin);

		string clip_end;
		clip_end.assign(SimpleAttrs::get("clipEnd", pAttrs));
		p_audio->setClipEnd(clip_end);

		mpCurrentMediaGroup->addAudioClip(p_audio);
	}
}

void amis::io::NcxFileReader::processNavTarget(const Attributes* pAttrs)
{
	mpCurrentNavTarget = new amis::dtb::nav::NavTarget();
	
	string class_value;
	class_value.assign(SimpleAttrs::get("class", pAttrs));
	mpCurrentNavTarget->setClass(class_value);

	string id;
	id.assign(SimpleAttrs::get("id", pAttrs));
	mpCurrentNavTarget->setId(id);

	string play_order;
	play_order.assign(SimpleAttrs::get("playOrder", pAttrs));
	int i_play_order = atoi(play_order.c_str());
	mpCurrentNavTarget->setPlayOrder(i_play_order);
	
	amis::dtb::nav::NavList* p_nav_list = NULL;
	int num = mpNavModel->getNumberOfNavLists();
	p_nav_list = mpNavModel->getNavList(num-1);

	if (p_nav_list != NULL)
		p_nav_list->addNode(mpCurrentNavTarget);
	else
		delete mpCurrentNavTarget;
}

void amis::io::NcxFileReader::processPageTarget(const Attributes* pAttrs)
{
	amis::dtb::nav::PageList* p_page_list;
	p_page_list = mpNavModel->getPageList();

	amis::dtb::nav::PageTarget* p_page = new amis::dtb::nav::PageTarget();

	string class_value;
	class_value.assign(SimpleAttrs::get("class", pAttrs));
	p_page->setClass(class_value);

	string id;
	id.assign(SimpleAttrs::get("id", pAttrs));
	p_page->setId(id);

	string play_order;
	play_order.assign(SimpleAttrs::get("playOrder", pAttrs));
	int i_play_order = atoi(play_order.c_str());
	p_page->setPlayOrder(i_play_order);	
	p_page_list->addNode(p_page);

	mpCurrentPageTarget = p_page;
}

void amis::io::NcxFileReader::processCustomTest(const Attributes* pAttrs)
{
	string id;
	id.assign(SimpleAttrs::get("id", pAttrs));

	string book_struct;
	book_struct.assign(SimpleAttrs::get("bookStruct", pAttrs));

	addCustomTest(id, true, true, book_struct);
}
