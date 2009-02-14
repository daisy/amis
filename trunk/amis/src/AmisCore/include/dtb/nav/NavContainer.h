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

#ifndef NAVCONTAINER_H
#define NAVCONTAINER_H

#include "Media.h"
#include "dtb/nav/NavNode.h"
#include "dtb/nav/NavVisitor.h"

namespace amis
{
namespace dtb
{
namespace nav
{

class NavModel;
class NavContainer
{
public:
	NavContainer();
	virtual ~NavContainer() = 0;
	
	virtual NavNode* goToPlayOrder(int) = 0;
	virtual NavNode* goToContentRef(string) = 0;
	virtual NavNode* goToId(string) = 0;

	void setLabel(amis::MediaGroup*);
	amis::MediaGroup* getLabel();

	void setNavInfo(amis::MediaGroup*);
	amis::MediaGroup* getNavInfo();
	
	void setId(string);
	string getId();

	void setNavModel(NavModel*);
	NavModel* getNavModel();

	virtual void acceptDepthFirst(NavVisitor*) = 0;

protected:
	NavModel* mpNavModel;

private:
	amis::MediaGroup* mpLabel;
	amis::MediaGroup* mpNavInfo;
	string mId;


};
}
}
}

#endif
