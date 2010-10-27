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

#ifndef NAVNODE_H
#define NAVNODE_H

#include "Media.h"
#include "dtb/nav/NavVisitor.h"

namespace amis
{
namespace dtb
{
namespace nav
{
class NavModel;

class NavNode
{
public:
	enum TypeOfNode
	{
		NAV_POINT = 0,
		NAV_TARGET = 1,
		PAGE_TARGET = 2
	};

	NavNode();
	virtual ~NavNode() = 0;

	TypeOfNode getTypeOfNode();

	amis::MediaGroup* getLabel();
	string getContent();
	string getId();
	int getPlayOrder();
	string getClass();
	void print(int);
	void setLabel(amis::MediaGroup*);
	void setContent(string);
	void setId(string);
	void setPlayOrder(int);
	void setClass(string);
	void setNavModel(NavModel*);
	NavContainer* getNavContainer();
	void setNavContainer(NavContainer*);
	virtual void acceptDepthFirst(NavVisitor*) = 0;
	string getRel();
	void setRel(string);

protected:
	NavModel* mpNavModel;
	TypeOfNode mTypeOfNode;
	NavContainer* mpNavContainer;

private:
	amis::MediaGroup* mpLabel;
	string mContent;
	string mId;
	int mPlayOrder;
	string mClass;
	string mRel;	

};
}
}
}
#endif
