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

#include "dtb/Bookmarks.h"
#include "util/FilePathTools.h"
#include <iostream>
using namespace std;

amis::dtb::BookmarkSet::BookmarkSet()
{
	mpLastmark = NULL;
	mpTitle = NULL;
	this->mUid.erase();
	this->mItems.clear();
}

amis::dtb::BookmarkSet::~BookmarkSet()
{
	if (mpTitle != NULL)
	{
		mpTitle->destroyContents();
		delete mpTitle;
	}

	if (mpLastmark != NULL)
	{
		delete mpLastmark;
	}

	int i;
	int sz;
	Bookmark* p_temp_bmk;
	Hilite* p_temp_hilite;
	MediaGroup* p_note;
	PositionData* p_pos;

	sz = mItems.size() - 1;

	for (i = sz; i>=0; i--)
	{
		if (mItems[i]->mType == PositionMark::BOOKMARK)
		{
			p_temp_bmk = (Bookmark*)mItems[i];
			mItems.pop_back();
			p_pos = p_temp_bmk->mpStart;
			delete p_pos;

			if (p_temp_bmk->mbHasNote == true)
			{
				p_note = p_temp_bmk->mpNote;
				p_note->destroyContents();
				delete p_note;
			}

			delete p_temp_bmk;
		}
		
		else
		{
			p_temp_hilite = (Hilite*)mItems[i];
			mItems.pop_back();
			p_pos = p_temp_hilite->mpStart;
			delete p_pos;

			p_pos = p_temp_hilite->mpEnd;
			delete p_pos;

			if (p_temp_hilite->mbHasNote == true)
			{
				p_note = p_temp_hilite->mpNote;
				p_note->destroyContents();
				delete p_note;
			}
			delete p_temp_hilite;
		}
	}

}

unsigned int amis::dtb::BookmarkSet::getNumberOfItems()
{
	return mItems.size();
}

const ambulant::net::url* amis::dtb::BookmarkSet::getFilepath()
{
	return &mFilepath;
}

void amis::dtb::BookmarkSet::setFilepath(ambulant::net::url* filepath)
{
	mFilepath = *filepath;
}

amis::dtb::PositionMark* amis::dtb::BookmarkSet::getItem(unsigned int idx)
{
	if (idx >= 0 && idx < mItems.size())
		return mItems[idx];
	else
		return NULL;
}

amis::dtb::PositionData::PositionData()
{
	mNcxRef = "";
	mTimeOffset = "";
	mCharOffset = "";
	mbHasTimeOffset = false;
	mbHasCharOffset = false;
}

amis::dtb::PositionData::~PositionData()
{
}
amis::dtb::PositionData* amis::dtb::PositionData::copy()
{	
	PositionData* p_data = new PositionData();
	p_data->mNcxRef.assign(this->mNcxRef);
	//how to safely copy ambulant urls otherwise?
	p_data->mUri = ambulant::net::url::from_url(this->mUri.get_url());
	p_data->mTimeOffset.assign(this->mTimeOffset);
	p_data->mCharOffset.assign(this->mCharOffset);
	p_data->mbHasTimeOffset = this->mbHasTimeOffset;
	p_data->mbHasCharOffset = this->mbHasCharOffset;
	return p_data;
}
amis::dtb::PositionData* amis::dtb::BookmarkSet::getLastmark()
{
	return mpLastmark;
}

string amis::dtb::BookmarkSet::getUid()
{
	return mUid;
}

amis::MediaGroup* amis::dtb::BookmarkSet::getTitle()
{
	return mpTitle;
}

int amis::dtb::BookmarkSet::getIndexOf(PositionMark* pMark)
{
	for (int i = 0; i<mItems.size(); i++)
	{
		if (mItems[i] == pMark) return i;
	}
	return -1;
}

void amis::dtb::BookmarkSet::addHilite(Hilite* pHilite)
{
	mItems.push_back(pHilite);
}

void amis::dtb::BookmarkSet::addBookmark(Bookmark* pBookmark)
{
	mItems.push_back(pBookmark);
}
void amis::dtb::BookmarkSet::deleteItem(unsigned int idx)
{
	if (mItems.size() == 0 || idx > mItems.size()-1) return;
	vector<amis::dtb::PositionMark*>::iterator it;
	it = mItems.begin();
	for (int i = 0; i<idx; i++) it++;
	mItems.erase(it);
}

void amis::dtb::BookmarkSet::setTitle(amis::MediaGroup* pData)
{
	mpTitle = pData;
}

void amis::dtb::BookmarkSet::setUid(string uid)
{
	mUid = uid;
}

void amis::dtb::BookmarkSet::setLastmark(PositionData* pData)
{
	if (mpLastmark != NULL) delete mpLastmark;
	mpLastmark = pData;
}

void amis::dtb::BookmarkSet::print()
{
	cout<<"TITLE: "<<endl;
	printMediaGroup(mpTitle);
	cout<<endl;

	cout<<"UID: "<<mUid<<endl<<endl;

	if (mpLastmark != NULL)
	{
		cout<<"LASTMARK: "<<endl;
		printPositionData(mpLastmark);
		cout<<endl;
	}
		
	unsigned int i;
	Bookmark* p_bmk;
	Hilite* p_hilite;

	for (i=0; i<mItems.size(); i++)
	{
		if (mItems[i]->mType == PositionMark::BOOKMARK)
		{
			cout<<"BOOKMARK: "<<endl;
		
			p_bmk = (Bookmark*)mItems[i];

			printPositionData(p_bmk->mpStart);
			if (p_bmk->mbHasNote == true)
			{
				printMediaGroup(p_bmk->mpNote);
			}
			cout<<endl;
		}
		else
		{
			p_hilite = (Hilite*)mItems[i];

			cout<<"HILITE: "<<endl;
			cout<<"start - "<<endl;
			printPositionData(p_hilite->mpStart);
			cout<<"end - "<<endl;
			printPositionData(p_hilite->mpEnd);

			if (p_hilite->mbHasNote == true)
			{
				printMediaGroup(p_hilite->mpNote);
			}
			cout<<endl;
		}
	}

}

void amis::dtb::BookmarkSet::printPositionData(PositionData* pData)
{
	if (pData == NULL)
	{
		cout<<"\tBAD POSITION DATA"<<endl;
		return;
	}

	cout<<"\tNcxRef: "<<pData->mNcxRef<<endl;
	cout<<"\tUri: "<<pData->mUri.get_url()<<endl;
	if (pData->mbHasCharOffset == true)
	{
		cout<<"\tCharOffset: "<<pData->mCharOffset<<endl;
	}
	if (mpLastmark->mbHasTimeOffset == true)
	{
		cout<<"\tTimeOffset: "<<pData->mTimeOffset<<endl;
	}
}

void amis::dtb::BookmarkSet::printMediaGroup(amis::MediaGroup* pData)
{
	if (pData == NULL)
	{
		cout<<"\tBAD MEDIA DATA"<<endl;
		return;
	}

	if (pData->hasText() == true)
	{
		amis::TextNode* p_text = pData->getText();

		if (p_text != NULL)
		{
			wstring text_content = p_text->getTextString();
			cout<<"\t";
			wcout<<text_content<<endl;
		}
	}
	if (pData->hasAudio() == true)
	{
		cout<<"\t"<<pData->getAudio(0)->getSrc()<<endl;
	}

}
