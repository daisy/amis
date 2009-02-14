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

//PROJECT INCLUDES
#include "util/FilePathTools.h"
#include "Error.h"

#include "dtb/smil/SmilTree.h"
#include "dtb/smil/Node.h"
#include "dtb/smil/TimeContainerNode.h"
#include "dtb/smil/ContentNode.h"
#include "dtb/smil/ParNode.h"
#include "dtb/smil/SeqNode.h"
#include "dtb/smil/NodeFactory.h"
#include "io/SmilFileReader.h"
#include "util/xercesutils.h"
using namespace std;

amis::io::SmilFileReader::SmilFileReader()
{
	mpSmilTree = NULL;
}

amis::io::SmilFileReader::~SmilFileReader()
{
}

void amis::io::SmilFileReader::clearOpenNodesList()
{
	int i;
	//remove anything from mOpenNodes NodeList
	amis::dtb::smil::Node* tmpnode = NULL;
	unsigned int sz = mOpenNodes.size();

	for (i = sz-1; i>=0; i--)
	{
		tmpnode = mOpenNodes[i];
		mOpenNodes.pop_back();
		delete tmpnode;	
	}
}

amis::dtb::smil::SmilTree* amis::io::SmilFileReader::getSmilTree()
{
	return mpSmilTree;
}

//--------------------------------------------------
/*!
	Load a SMIL file and start the Xerces SAX2 parser.
*/
//--------------------------------------------------
bool amis::io::SmilFileReader::readFromFile(const ambulant::net::url* filepath)
{
	mpSmilTree = new amis::dtb::smil::SmilTree();

	//reset the link href
	mLinkHref = "";
	
	//set the default value for couldescape
	mpSmilTree->setCouldEscape(false);

	clearOpenNodesList();

	if (!this->parseFile(filepath))
	{
		delete mpSmilTree;
		return false;
	}
	else
	{
		return true;
	}
}


//--------------------------------------------------
/*!
	analyze the element type and collect data to build a node
*/
//--------------------------------------------------
void amis::io::SmilFileReader::startElement(const   XMLCh* const    uri,
				const   XMLCh* const    localname,
				const   XMLCh* const    qname,
				const   Attributes&	attributes)
{
	//local variables
	char* element_name;
	string tmp_string;

	//get the element name as a string
	element_name = XMLString::transcode(qname);

	//if this is a link node, get the href and flag linkOpen
	if(strcmp(element_name, "a") == 0)
	{
		mbLinkOpen = true;
		mLinkHref.assign(SimpleAttrs::get("href", &attributes));
	}

	//else if this is a seq, par, aud, txt, img
	else 
	{
		string elm_name;
		elm_name.assign(element_name);
		processNode(elm_name, &attributes);	
	}

	XMLString::release(&element_name); 

}

//--------------------------------------------------
/*!
	This function uses the NodeBuilder to create a node, and then 
	adds the node to the right spot on the tree
*/
//--------------------------------------------------
void amis::io::SmilFileReader::processNode(string elementName, const Attributes* pAttrs)
{
	//see if this is a supported type
	amis::dtb::smil::NodeType node_type = getNodeType(elementName);
	if (node_type == amis::dtb::smil::NONE)
		return;

	//local variables
	amis::dtb::smil::Node* p_node_data;
	amis::dtb::smil::SeqNode* p_root;
	string empty_str = "";
	amis::dtb::smil::Node* p_parent;
	int len;
	string skip_option_name;

	//try to create a node
	p_node_data = amis::dtb::smil::NodeFactory::createNode(node_type, pAttrs);

	//if we successfully created the node
	if (p_node_data != NULL)
	{
		//give the node a pointer to the Smil tree
		p_node_data->setSmilTreePtr(mpSmilTree);

		//if the tree is empty, then we need to add a root
		if (mpSmilTree->isEmpty() == true)
		{
			//if our new node is the first node in the tree and it is not of type SEQ
			if (p_node_data->getTypeOfNode() != SEQ)
			{
				//make a seq node for the root
				p_root = new amis::dtb::smil::SeqNode();
				p_root->setSmilTreePtr(mpSmilTree);
				p_root->setParent(NULL);
				p_root->setSkipOption(empty_str);

				//set the root of the tree equal to this root
				mpSmilTree->setRoot(p_root);

				//add the root to the open nodes list
				mOpenNodes.push_back((amis::dtb::smil::Node* const)p_root);

				//add pNodeData as a child of the root
				p_root->addChild(p_node_data);

				//if this node data is a par, add it to the open nodes list
				//seqs also go on this list, but we have already determined this is not a seq
				if (p_node_data->getTypeOfNode() == PAR)
				{
					mOpenNodes.push_back((amis::dtb::smil::Node* const)p_node_data);
				}
				
			} //end if new node is not a seq

			//else, it is the first node in the tree but it is of type SEQ
			else
			{
				//add our new node as the root of the tree
				mpSmilTree->setRoot((amis::dtb::smil::SeqNode*)p_node_data);

				//put the new node on the open nodes list
				mOpenNodes.push_back((amis::dtb::smil::Node* const)p_node_data);
			}

		} //end if tree is empty

		//else, the tree is not empty
		else
		{
			//get the last item from the open nodes list and set it as the parent for this node
			len = mOpenNodes.size();
			p_parent = mOpenNodes[len-1];

			//record link data for this node
			//only content nodes may be nested beneath a link node
			if (p_node_data->getCategoryOfNode() == CONTENT && 
				mbLinkOpen == true)
			{
				amis::MediaNode* pMediaNode = 
					((amis::dtb::smil::ContentNode*)p_node_data)->getMediaNode();
				pMediaNode->setHref(mLinkHref);
			}

			//add as the next child to the last item of the open nodes list	
			((amis::dtb::smil::TimeContainerNode*)p_parent)->addChild(p_node_data);
			

			//add to the open nodes list if it's a time container (par or seq)
			if (p_node_data->getCategoryOfNode() == TIME_CONTAINER)
			{
				mOpenNodes.push_back((amis::dtb::smil::Node* const)p_node_data);
			}

		} //end 

	} //closing brace for if pNodeData != NULL
}

//--------------------------------------------------
/*!
	close this element so that no additional child nodes are added 
	to it in the Smil Tree
*/
//--------------------------------------------------
void amis::io::SmilFileReader::endElement( const XMLCh* const uri,
					const XMLCh* const localname,
					 const XMLCh* const qname)
{
	//local variable
	char* element_name = XMLString::transcode(qname);

	//if this element is a seq or par, then remove the last item from the openNodes list
	//since the element is being ended, we will not want to add children to it
	if (strcmp(element_name, "seq") == 0 ||
		strcmp(element_name, "par") == 0)
	{
		mOpenNodes.pop_back();
	}

	else if(strcmp(element_name, "a") == 0)
	{
		mbLinkOpen = false;
		mLinkHref= "";
	}
	else
	{
		//empty
	}

	XMLString::release(&element_name);

}


void amis::io::SmilFileReader::characters(const XMLCh *const chars, const unsigned int length)
{
}

//--------------------------------------------------
/*!
	tests node name for a match in our supported nodes list
*/
//--------------------------------------------------
amis::dtb::smil::NodeType amis::io::SmilFileReader::getNodeType(string elementName)
{
	if (elementName.compare("seq") == 0)
		return amis::dtb::smil::SEQ;
	else if (elementName.compare("par") == 0)
		return amis::dtb::smil::PAR;
	else if (elementName.compare("audio") == 0)
		return amis::dtb::smil::AUD;
	else if (elementName.compare("text") == 0)
		return amis::dtb::smil::TXT;
	else if (elementName.compare("image") == 0)
		return amis::dtb::smil::IMG;
	else
		return amis::dtb::smil::NONE;

}
