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

#include "dtb/nav/NavTarget.h"
#include "dtb/nav/NavList.h"

amis::dtb::nav::NavTarget::NavTarget()
{
	this->mTypeOfNode = NavNode::NAV_TARGET;
}

amis::dtb::nav::NavTarget::~NavTarget()
{
}

int amis::dtb::nav::NavTarget::getIndex()
{
	return mIndex;
}

void amis::dtb::nav::NavTarget::setIndex(int idx)
{
	mIndex = idx;
}

void amis::dtb::nav::NavTarget::acceptDepthFirst(amis::dtb::nav::NavVisitor* v)
{
	if (v->preVisit(this))
	{
		NavList* p_list = (NavList*)this->getNavContainer();
		//read the next node after this one
		if (mIndex < p_list->getLength() - 1)
			p_list->getAtIndex(mIndex + 1)->acceptDepthFirst(v);
	}
}
