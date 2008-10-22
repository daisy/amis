/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (C) 2008  DAISY Consortium

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
#ifndef NAVLIST_H
#define NAVLIST_H

#include "dtb/nav/NavTarget.h"
#include "dtb/nav/NavContainer.h"
#include "dtb/nav/NavVisitor.h"

namespace amis
{
namespace dtb
{
namespace nav
{
class NavList : public NavContainer
{
public:
	NavList();
	~NavList();
	void addNode(NavTarget*);
	int getLength();

	NavTarget* getAtIndex(int);
	NavNode* goToContentRef(string);
	NavNode* goToId(string);
	NavNode* goToPlayOrder(int);

	void print(bool printheader = true);

	void acceptDepthFirst(NavVisitor*);

private:
	vector<NavTarget*> mNodes;
};
}
}
}
#endif
