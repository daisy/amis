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

#include <string>
#include <iostream>
#include <algorithm>

#include "util/FilePathTools.h"
#include "dtb/Spine.h"

using namespace std;

amis::dtb::Spine::Spine()
{
	mListIndex = 0;
}

amis::dtb::Spine::~Spine()
{
}


//return the next file in the spine list
const ambulant::net::url* amis::dtb::Spine::getNextFile()
{
	if (mListIndex + 1 < mSpineList.size() && mListIndex+1 >=0)
	{
		mListIndex++;
		return &mSpineList[mListIndex];
	}

	else
	{
		return NULL;
	}
}

//return the previous file in the spine list
const ambulant::net::url* amis::dtb::Spine::getPreviousFile()
{
	if (mListIndex - 1 >= 0 && mListIndex - 1 < mSpineList.size())
	{
		mListIndex--;
		return &mSpineList[mListIndex];
	}
	else
	{
		return NULL;
	}	
}

const ambulant::net::url* amis::dtb::Spine::getFirstFile()
{
	if (mSpineList.size() > 0)
	{
		mListIndex = 0;
		return &mSpineList.front();
	}
	else
	{
		return NULL;
	}
}

const ambulant::net::url* amis::dtb::Spine::getLastFile()
{
	if (mSpineList.size() > 0)
	{
		return &mSpineList.back();
	}
	else
	{
		return NULL;
	}
}


//check to see if a file is present in the spine
bool amis::dtb::Spine::isFilePresent(const ambulant::net::url* filePath)
{
	for (unsigned int i=0; i<mSpineList.size(); i++)
	{
		if (mSpineList[i].same_document((*filePath)))
		{
			return true;
		}
	}
	return false;
}
//if the file doesn't already exist, add it to the spine list
//skipCheck allows to skip the isFilePresent check for very long lists
//filePath should be a full path.
void amis::dtb::Spine::addFile(const ambulant::net::url* filePath, bool skipCheck)
{
	if (filePath->is_empty_path()) return;

	if (skipCheck)
	{
		mSpineList.push_back(filePath->get_document());
	}
	else
	{
		if (isFilePresent(filePath) == false)
		{
			mSpineList.push_back(filePath->get_document());
		}
	}
}

//print the spine
void amis::dtb::Spine::print()
{
	unsigned int i;

	cout<<"Spine"<<endl;
	for (i =0; i<mSpineList.size(); i++)
	{
		cout<<"\t"<<i<<". "<<mSpineList[i].get_url()<<endl;
	}
	cout<<endl;
}


//does the spine contain files?
bool amis::dtb::Spine::isEmpty()
{
	if (mSpineList.size() == 0)
		return true;
	else
		return false;
}

//set the spine at this particular file, if exists
bool amis::dtb::Spine::goToFile(const ambulant::net::url* filePath)
{
	bool b_found = false;
	unsigned int i;
	for (i=0; i<mSpineList.size(); i++)
	{
		if (mSpineList[i].same_document((*filePath)))
		{
			b_found = true;
			break;
		}
	}
	
	if (b_found) this->mListIndex = i;
	return b_found;
}

//get the number of smil files in the publication-
int amis::dtb::Spine::getNumberOfSmilFiles()
{
	return this->mSpineList.size();
}

//get the source path for a smil file (by index)
const ambulant::net::url* amis::dtb::Spine::getSmilFilePath(unsigned int idx)
{
	if (idx >= 0 && idx < mSpineList.size())
	{
		return &mSpineList[idx];
	}
	else
	{
		return NULL;
	}
}
