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
//NodeBuilder

//node creation happens quickly (no file reading going on) so no need to mess with events
//just return a new node pointer.

#ifndef NODEFACTORY_H
#define NODEFACTORY_H

//Xerces requires this
XERCES_CPP_NAMESPACE_USE

XERCES_CPP_NAMESPACE_BEGIN
class Attributes;
XERCES_CPP_NAMESPACE_END

#include "dtb/smil/Node.h"

//! creates a Node based on XML element data
/*!
	The XML element data comes from the SmilEngineBuilder, which uses a Xerces SAX
	parse to read in the file.
	The NodeBuilder is called to create an audio, image, text, seq, or par node
*/
namespace amis
{
namespace dtb
{
namespace smil
{
//! NodeType enumeration
enum NodeType
{
	NONE = 0,
	SEQ = 1, 
	PAR = 2, 
	AUD = 3, 
	TXT = 4, 
	IMG = 5,
	A = 6,
	CONTENT = 7,
	TIME_CONTAINER = 8
};

class NodeFactory
{
public:
	NodeFactory();
	~NodeFactory();

	//!create a new node
	static Node* createNode(NodeType, const Attributes*);

private:
	static Node* createAudioNode(const Attributes*);
	static Node* createTextNode(const Attributes*);
	static Node* createImageNode(const Attributes*);
	static Node* createParNode(const Attributes*);
	static Node* createSeqNode(const Attributes*);

};
}
}
}
#endif
