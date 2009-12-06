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

#include "dtb/Metadata.h"

#include <string>
#include <iostream>
#include <algorithm>
#include "util/FilePathTools.h"

using namespace std;

amis::dtb::Metadata::Metadata()
{
	clearVector();
}

amis::dtb::Metadata::~Metadata()
{
	clearVector();
}

//This is case-insensitive
amis::dtb::MetaItem* amis::dtb::Metadata::getMetadata(string metaname)
{
	for (int i = 0; i<mMetaList.size(); i++)
	{
		//convert to lowercase first
		std::string name1 = metaname;
		std::string name2 = mMetaList[i]->mName;
		std::transform(name1.begin(), name1.end(), name1.begin(), (int(*)(int))tolower);
		std::transform(name2.begin(), name2.end(), name2.begin(), (int(*)(int))tolower);
		
		if (name1.compare(name2) == 0)
		{
			return mMetaList[i];
		}
	}
	return NULL;
}

wstring amis::dtb::Metadata::getMetadataContent(string metaname)
{
	wstring return_value;
	return_value.erase();
	MetaItem* p_item = getMetadata(metaname);
	if (p_item != NULL) return_value = p_item->mContent;
	return return_value;
}

void amis::dtb::Metadata::addMetadata(MetaItem* pNewItem)
{
	mMetaList.push_back(pNewItem);
}

//delete all metaitem objects in the list of metadata
void amis::dtb::Metadata::clearVector()
{
	unsigned int sz = mMetaList.size();
	MetaItem* tmp_ptr;

	for (int i=sz-1; i>=0; i--)
	{
		tmp_ptr = NULL;
		tmp_ptr = mMetaList[i];
		mMetaList.pop_back();
		delete tmp_ptr;
	}
	mMetaList.clear();
}
