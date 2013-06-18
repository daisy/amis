/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

Copyright (c) 2004 DAISY Consortium

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

#include "dtb/nav/NavContainer.h"
#include "dtb/nav/NavModel.h"

amis::dtb::nav::NavContainer::NavContainer()
{
	mpLabel = NULL;
}

amis::dtb::nav::NavContainer::~NavContainer()
{
	if (mpLabel != NULL)
	{
		mpLabel->destroyContents();
		delete mpLabel;
	}
}

void amis::dtb::nav::NavContainer::setNavModel(NavModel* pModel)
{
	mpNavModel = pModel;
}
amis::dtb::nav::NavModel* amis::dtb::nav::NavContainer::getNavModel()
{
	return mpNavModel;
}
void amis::dtb::nav::NavContainer::setLabel(amis::MediaGroup* pLabel)
{
	this->mpLabel = pLabel;
}

amis::MediaGroup* amis::dtb::nav::NavContainer::getLabel()
{
	return mpLabel;
}

void amis::dtb::nav::NavContainer::setNavInfo(amis::MediaGroup* pNavInfo)
{
	mpNavInfo = pNavInfo;
}

amis::MediaGroup* amis::dtb::nav::NavContainer::getNavInfo()
{
	return mpNavInfo;
}

void amis::dtb::nav::NavContainer::setId(string id)
{
	mId = id;
}

string amis::dtb::nav::NavContainer::getId()
{
	return mId;
}
