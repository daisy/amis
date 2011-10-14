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

//tree structure 

#ifndef NAVMAP_H
#define NAVMAP_H

#include <vector>

#include "dtb/nav/NavContainer.h"
#include "dtb/nav/NavPoint.h"
#include "dtb/nav/NavVisitor.h"

using namespace std;

namespace amis
{
namespace dtb
{
namespace nav
{
class NavMap : public NavContainer
{
public:
	NavMap();
	~NavMap();
	
	NavPoint* getRoot();

	//test print function
	void print();
	void printNode(NavPoint*, int);

	bool isEmpty();

	NavNode* goToPlayOrder(int);
	NavNode* goToContentRef(string);
	NavNode* goToId(string);
	
	int getNumberOfNavPoints();

	int getMaxDepth();

	void acceptDepthFirst(NavVisitor*);

protected:	
	void recordNewDepth(int);
	void setRoot(NavPoint*);


private:

	NavPoint* mpRoot;
	int mMaxDepth;

	friend class NavPoint;
	friend class NavModel;
	

};
}
}
}
#endif
