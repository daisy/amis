/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amis.sf.net

Copyright (C) 2004-2007  DAISY for All Project

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
#include "BookList.h"
#include "util/FilePathTools.h"
#include <iostream>
using namespace std;

amis::BookEntry::BookEntry()
{
	mbIsLastRead= false;
	mpTitle = NULL;
}

amis::BookEntry::~BookEntry()
{
	if (mpTitle != NULL) delete mpTitle;
}

void amis::BookEntry::setTitleText(wstring text)
{
	if (mpTitle == NULL) mpTitle = new amis::MediaGroup();
	if (mpTitle->getText() == NULL)
	{
		amis::TextNode* p_text = new amis::TextNode();
		mpTitle->setText(p_text);
	}
	mpTitle->getText()->setTextString(text);
}
void amis::BookEntry::setTitleAudio(string src, string clipbegin, string clipend)
{
	if (mpTitle == NULL) mpTitle = new amis::MediaGroup();
	if (mpTitle->getNumberOfAudioClips() == 0)
	{
		amis::AudioNode* p_audio = new amis::AudioNode();
		mpTitle->addAudioClip(p_audio);
	}
	//this should be a full path
	mpTitle->getAudio(0)->setSrc(src);
	mpTitle->getAudio(0)->setSrcExpanded(src);
	mpTitle->getAudio(0)->setClipBegin(clipbegin);
	mpTitle->getAudio(0)->setClipEnd(clipend);
}
wstring amis::BookEntry::getTitleText()
{
	if (mpTitle != NULL && mpTitle->getText() != NULL)
	{
		return mpTitle->getText()->getTextString();
	}
	else
	{
		//maybe there's a better way to return an empty wide string?  
		//returning NULL doesn't work.
		wstring empty_string;
		empty_string.erase();
		return empty_string;
	}
}
amis::AudioNode* amis::BookEntry::getTitleAudio()
{
	if (mpTitle != NULL && mpTitle->getNumberOfAudioClips() > 0)
		return mpTitle->getAudio(0);
	else
		return NULL;
}
amis::BookList::BookList()
{
	mItems.clear();
}
amis::BookList::~BookList()
{
	cleanUpVector();
}
void amis::BookList::cleanUpVector()
{
	int sz = mItems.size();
	amis::BookEntry* p_tmp;
	for (int i = sz-1; i>=0; i--)
	{
		p_tmp = mItems[i];
		mItems.pop_back();
		if (p_tmp != NULL) delete p_tmp;
	}
}
int amis::BookList::getNumberOfEntries()
{
	return mItems.size();
}
void amis::BookList::setName(std::wstring name)
{
	mName = name;
}
std::wstring amis::BookList::getName()
{
	return mName;
}
amis::BookEntry* amis::BookList::getEntry(unsigned int idx)
{
	if (idx >= 0 && idx < mItems.size())
		return mItems[idx];
	else
		return NULL;
}

void amis::BookList::addEntry(amis::BookEntry* pEntry)
{
	int n_duplicate = -1;
	//first check for duplicates
	if (mItems.size() > 0)
	{
		for (int i=0; i<mItems.size(); i++)
		{
			if (mItems[i]->mUid.compare(pEntry->mUid) == 0 && mItems[i]->mUid.size() > 0)
			{
				n_duplicate = i;
				break;
			}
		}

		//if we found a duplicate, delete it from its original position 
		//and add our new (identical) entry at the end of the list
		if (n_duplicate > -1)
			deleteEntry(n_duplicate);
	}

	//add the new entry
	mItems.push_back(pEntry);

	//only one book can be the last read book!
	if (pEntry->mbIsLastRead == true)
	{
		amis::BookEntry* p_tmp = NULL;
		for (unsigned int i = 0; i<mItems.size() - 1; i++)
		{
			p_tmp = mItems[i];
			p_tmp->mbIsLastRead = false;
		}
	}
}
void amis::BookList::deleteEntry(unsigned int idx)
{
	std::vector<amis::BookEntry*>::iterator it;

	if (idx < mItems.size() && idx >= 0)
	{
		it = mItems.begin() + idx;
		mItems.erase(it);
	}
}

amis::BookEntry* amis::BookList::getLastRead()
{
	if (mItems.empty() == true) return NULL;

	amis::BookEntry* p_tmp = NULL;
	for (unsigned int i = 0; i<mItems.size(); i++)
	{
		p_tmp = mItems[i];
		if (p_tmp->mbIsLastRead == true) break;
	}
	return p_tmp;
}

void amis::BookList::setFilepath(const ambulant::net::url* pFilepath)
{
	mFilepath = *pFilepath;
}

const ambulant::net::url* amis::BookList::getFilepath()
{
	return &mFilepath;
}

amis::BookEntry* amis::BookList::findByUid(wstring uid)
{
	for (int i=0; i<mItems.size(); i++)
	{
		if (mItems[i]->mUid.compare(uid) == 0 && uid.size() > 0)
		{
			return mItems[i];
		}
	}
	return NULL;
}