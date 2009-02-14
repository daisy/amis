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
#ifndef TIMECONTAINERNODE_H
#define TIMECONTAINERNODE_H

#include "dtb/smil/Node.h"
#include "dtb/smil/SmilVisitor.h"
#include "Error.h"

//! Represents a seq or a par.
/*!
	A time container node will be a seq or a par.  The purpose of this class is to 
	provide	handling for adding and retrieving child nodes.
 */
namespace amis
{
namespace dtb
{
namespace smil
{
class TimeContainerNode : public Node
{
public:
	TimeContainerNode();
	~TimeContainerNode();

	//!add a child node
	void addChild(Node*);
	//!get the child at the specified index
	Node* getChild(int);

	//!set a skippability option for this time container
	void setSkipOption(string);
	//!get this time container's skippability option
	string getSkipOption();

	//!get the number of children
	int NumChildren();
	//!get the category of this node
	NodeType getCategoryOfNode();

	//!set index at first child
	virtual amis::Error setFirst() = 0;
	//!set index at last child
	virtual amis::Error setLast() = 0;
	//!set index at next child
	virtual amis::Error setNext() = 0;
	//!set index at previous child
	virtual amis::Error setPrevious() = 0;
	//!set index at the child with this id
	bool setAtId(string);
	void acceptDepthFirst(SmilVisitor*);
	virtual void acceptDepthFirstOnActivePath(SmilVisitor*) = 0;

private:
	//!pointer to first child
	Node* mpFirstChild;
	//!number of children
	int mNumChildren;
	//!skippability option name if exists
	string mSkipOptionName;
};
}
}
}
#endif
