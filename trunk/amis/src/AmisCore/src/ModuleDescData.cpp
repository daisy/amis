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
#include "ModuleDescData.h"


amis::ModuleDescData::ModuleDescData()
{
	mpLabel = NULL;
	mType = NONE;
}

amis::ModuleDescData::~ModuleDescData()
{
	if (this->mpLabel != NULL)
	{
		mpLabel->destroyContents();
		delete mpLabel;
	}
}

amis::MediaGroup* amis::ModuleDescData::getLabel()
{
	return mpLabel;
}

string amis::ModuleDescData::getId()
{
	return mId;
}

amis::ModuleDescData::ModuleType amis::ModuleDescData::getModuleType()
{
	return mType;
}

string amis::ModuleDescData::getDllFileName()
{
	return mDllFileName;
}

const ambulant::net::url* amis::ModuleDescData::getXmlFileName()
{
	return &mXmlFileName;
}

bool amis::ModuleDescData::isEnabled()
{
	return this->mEnabled;
}

void amis::ModuleDescData::setLabel(amis::MediaGroup* pLabel)
{
	mpLabel = pLabel;
}

void amis::ModuleDescData::setId(string id)
{
	mId = id;
}

void amis::ModuleDescData::setModuleType(amis::ModuleDescData::ModuleType moduleType)
{
	mType = moduleType;
}

void amis::ModuleDescData::setDllFileName(string filename)
{
	mDllFileName = filename;
}

void amis::ModuleDescData::setXmlFileName(ambulant::net::url filename)
{
	mXmlFileName = filename;
}

void amis::ModuleDescData::setIsEnabled(bool bVal)
{
	this->mEnabled = bVal;
}
