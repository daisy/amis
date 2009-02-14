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

#ifndef NAVMODEL_H
#define NAVMODEL_H

#include "dtb/nav/NavMap.h"
#include "dtb/nav/NavList.h"
#include "dtb/nav/PageList.h"

#include "Media.h"
#include "dtb/CustomTest.h"
#include "Error.h"
#include "dtb/nav/NavVisitor.h"
#include "AmisCore.h"

namespace amis
{
namespace dtb
{
namespace nav
{
class NavModel
{
public:
	NavModel();
	~NavModel();

	unsigned int getNumberOfNavLists();
	NavList* getNavList(unsigned int);
	NavList* getNavList(string);
	NavListList* getNavLists();
	int addNavList(string);
	bool hasPages();
	PageList* getPageList();
	NavMap* getNavMap();
	void acceptDepthFirst(NavVisitor*);
	NavNode* getNodeForSmilId(string, NavContainer*);
	void setSmilIdNodeMap(NodeRefMap*);
	void testMap();
	NavPoint* previousSection(int);
	NavPoint* nextSection(int);
	NavPoint* previousSection(int, int);
	NavPoint* nextSection(int, int);
	PageTarget* previousPage(int);
	PageTarget* nextPage(int);
	//add a node to the big ordered list.  the node will not necessarily be next in sequence.
	void addToPlayOrderList(NavNode*);
	NavNodeList* getPlayOrderList();
	int getPlayOrderListIndex(int);
	//needed these function because local filenames are lowercased on windows by ambulant::net::url, whereas remote are not
	//we need NavNode::mContent to be handled the same as the rest of the files
	bool getAreFilenamesLowercase();
	void setAreFilenamesLowercase(bool);

private:
	NavMap* mpNavMap;
	PageList* mpPageList;
	NodeRefMap* mpSmilIdNodeMap;
	NavListList mNavLists;
	NavNodeList mPlayOrderList;
	bool mbLower;
};
}
}
}

#endif
