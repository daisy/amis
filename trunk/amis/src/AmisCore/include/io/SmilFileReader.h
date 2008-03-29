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

#ifndef SMILFILEREADER_H
#define SMILFILEREADER_H

#include <string>
#include <xercesc/sax2/DefaultHandler.hpp>

#include "Error.h"
#include "io/XercesSaxParseBase.h"
#include "dtb/smil/NodeFactory.h"

XERCES_CPP_NAMESPACE_USE

XERCES_CPP_NAMESPACE_BEGIN
class Attributes;
XERCES_CPP_NAMESPACE_END

using namespace std;

class SmilTree;

//! The Smil Tree Builder parses a SMIL file and builds a tree
/*!
	The input files must be valid SMIL documents. Xerces SAX 2 is the parsing engine. 
	The Smil Tree Builder acts as a Xerces Content and Error handler object.
*/
namespace amis
{
namespace io
{
class SmilFileReader : public XercesSaxParseBase 
{
public:
	SmilFileReader();
	//!destructor
	~SmilFileReader();

	//!main method to create a smil tree from a filepath
	bool readFromFile(const ambulant::net::url*);

	amis::dtb::smil::SmilTree* getSmilTree();

	//SAX METHODS
	//!xerces start element event
	void startElement(const   XMLCh* const    uri,
			const   XMLCh* const    localname,
			const   XMLCh* const    qname,
			const   Attributes&	attributes);
	//!xerces end element event
    void endElement(const XMLCh* const uri,
		const XMLCh* const localname,
		const XMLCh* const qname);
	void characters(const XMLCh *const, const unsigned int);

private:
	//!see if the node name is interesting to us
	amis::dtb::smil::NodeType getNodeType(string);

	//!process a node in the smil body
	void processNode(string, const Attributes*);
	//helper function
	void clearOpenNodesList();

	amis::dtb::smil::SmilTree* mpSmilTree;

	//!list of open nodes in the tree (nodes to which children can be added)
	vector<amis::dtb::smil::Node*> mOpenNodes;

	//!is there a link element open?
	bool mbLinkOpen;
	//!the link href
	string mLinkHref;
};
}
}

#endif
