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
#ifndef PARNODE_H
#define PARNODE_H

//PROJECT INCLUDES
#include "dtb/smil/TimeContainerNode.h"
#include "dtb/smil/SmilEngineConstants.h"

//! represents a Par element in a SMIL file
/*!
	A par (as in parallel) time container plays all its children at once
	seq children are played as sequences, starting with the first child
*/
namespace amis
{
namespace dtb
{
namespace smil
{
class ParNode : public TimeContainerNode
{
public:
	//LIFECYCLE
	//!default constructor
	ParNode();
	//!destructor
	~ParNode();

	//ACCESS
	//!get the type of node
	NodeType getTypeOfNode();

	//METHODS
	//!play this node
	void play(SmilMediaGroup*);
	//!go to the next node
	amis::Error setNext();
	//!go to the previous node
	amis::Error setPrevious();
	//!go to the first node
	amis::Error setFirst();
	//!go to the last node
	amis::Error setLast();

	void acceptDepthFirstOnActivePath(SmilVisitor*);
private:
	//!signals received from child nodes
	int mSignalCount;
};
}
}
}
#endif
