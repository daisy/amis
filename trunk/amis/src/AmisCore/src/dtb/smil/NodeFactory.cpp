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

#include <string>
#include <iostream>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include "util/FilePathTools.h"
#include "Media.h"
#include "dtb/smil/Node.h"
#include "dtb/smil/TimeContainerNode.h"
#include "dtb/smil/ContentNode.h"
#include "dtb/smil/ParNode.h"
#include "dtb/smil/SeqNode.h"
#include "dtb/smil/NodeFactory.h"
#include "util/xercesutils.h"

using namespace std;


amis::dtb::smil::NodeFactory::NodeFactory()
{
}

amis::dtb::smil::NodeFactory::~NodeFactory()
{
	//Empty
}

//--------------------------------------------------
/*!
	Create a node from XML element data.

	Return NULL if the element is not one of six types, 
	otherwise return a pointer to the newly created node
*/
//--------------------------------------------------
amis::dtb::smil::Node* amis::dtb::smil::NodeFactory::createNode(NodeType type, const Attributes*  pAttrs)
{
	//return pNewNode from this function
	Node* p_new_node = NULL;

	if	(type == SEQ)
	{
		p_new_node = createSeqNode(pAttrs);
	}
	else if (type == PAR)
	{
		p_new_node = createParNode(pAttrs);
	}
	else if (type == AUD)
	{
		p_new_node = createAudioNode(pAttrs);	
	}
	else if (type == TXT)
	{
		p_new_node = createTextNode(pAttrs);
	}
	else if (type == IMG)
	{
		p_new_node = createImageNode(pAttrs);
	}
	//return a pointer to the newly created node
	return p_new_node;
}

//--------------------------------------------------
//	create a seq node
//--------------------------------------------------
amis::dtb::smil::Node* amis::dtb::smil::NodeFactory::createSeqNode(const Attributes* pAttrs)
{
	//temporarily save the attribute value in this variable
	string attribute_value;

	//pointer to a new seq node
	SeqNode* p_seq = new SeqNode();

	string id;
	id.assign(SimpleAttrs::get("id", pAttrs));
	p_seq->setElementId(id);

	string custom_test;
	custom_test.assign(SimpleAttrs::get("customTest", pAttrs));
	p_seq->setSkipOption(custom_test);

	//return the newly created Seq node
	return p_seq;
}

//--------------------------------------------------
//	create a par node
//--------------------------------------------------
amis::dtb::smil::Node* amis::dtb::smil::NodeFactory::createParNode(const Attributes* pAttrs)
{
	//pointer to a new par node
	ParNode* p_par = new ParNode();

	string id;
	id.assign(SimpleAttrs::get("id", pAttrs));
	p_par->setElementId(id);

	string sys_required;
	sys_required.assign(SimpleAttrs::get("system_required", pAttrs));
	if (sys_required != "")
	{
		//change this value a bit so that sidebar-on = sidebar
		//daisy 2.02 values in the NCC will appear as, for ex, "sidebar"
		//in the SMIL, they appear as "sidebar-on"
		//in AMIS, transform this "sidebar-on" statement to a customTest class
		//with id=sidebar, defaultState = true
		sys_required.replace(sys_required.find("-on"),1, "");

		p_par->setSkipOption(sys_required);
	}

	//else did not find system-required attribute
	else
	{
		string customTest;
		customTest.assign(SimpleAttrs::get("customTest", pAttrs));
		if (customTest != "")
			p_par->setSkipOption(customTest);
	}

	//return the newly created Par node
	return (Node*)p_par;
}

//--------------------------------------------------
//create an audio node
//--------------------------------------------------
amis::dtb::smil::Node* amis::dtb::smil::NodeFactory::createAudioNode(const Attributes* pAttrs)
{
	//create a new audio node
	ContentNode* p_audio = new ContentNode();
	p_audio->createNewAudio();

	amis::AudioNode* p_audioMedia = (amis::AudioNode*)p_audio->getMediaNode();

	string id;
	id.assign(SimpleAttrs::get("id", pAttrs));
	p_audioMedia->setId(id);
	p_audio->setElementId(id);

	string src;
	src.assign(SimpleAttrs::get("src", pAttrs));
	p_audioMedia->setPath(src);

	string clipBegin;
	clipBegin.assign(SimpleAttrs::get("clip-begin", pAttrs));
	if (clipBegin == "")
	{
		clipBegin.assign(SimpleAttrs::get("clipBegin", pAttrs));
	}

	p_audioMedia->setClipBegin(clipBegin);
	
	string clipEnd;
	clipEnd.assign(SimpleAttrs::get("clip-end", pAttrs));
	if (clipEnd == "")
	{
		clipEnd.assign(SimpleAttrs::get("clipEnd", pAttrs));
	}
	p_audioMedia->setClipEnd(clipEnd);

	//return a pointer to the new audio node
	return (Node*)p_audio;
}

//--------------------------------------------------
//create a text node
//--------------------------------------------------
amis::dtb::smil::Node* amis::dtb::smil::NodeFactory::createTextNode(const Attributes* pAttrs)
{
	//create a new text node
	ContentNode* p_text = new ContentNode();
	p_text->createNewText();

	amis::TextNode* p_textMedia = (amis::TextNode*)p_text->getMediaNode();

	string id;
	id.assign(SimpleAttrs::get("id", pAttrs));
	p_textMedia->setId(id);
	p_text->setElementId(id);
	
	//get and save the content src
	string src;
	src.assign(SimpleAttrs::get("src", pAttrs));
	p_textMedia->setPath(src);

	//return a pointer to the new text node
	return (Node*)p_text;
}

//--------------------------------------------------
//create an image node
//--------------------------------------------------
amis::dtb::smil::Node* amis::dtb::smil::NodeFactory::createImageNode(const Attributes* pAttrs)
{
	//create a new image node
	ContentNode* p_image = new ContentNode();
	p_image->createNewImage();

	amis::ImageNode* p_imageMedia = (amis::ImageNode*)p_image->getMediaNode();

	string id;
	id.assign(SimpleAttrs::get("id", pAttrs));
	p_imageMedia->setId(id);
	p_image->setElementId(id);
	
	//get and save the content src
	string src;
	src.assign(SimpleAttrs::get("src", pAttrs));
	p_imageMedia->setPath(src);

	//return the pointer to a new image node
	return (Node*)p_image;
}
