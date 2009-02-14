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
#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include "Media.h"
#include <string>
#include <vector>
#include "ambulant/net/url.h"

using namespace std;

namespace amis
{
namespace dtb
{

//!PositionData class
class PositionData
{
public:
	PositionData();
	~PositionData();
	PositionData* copy();

	string mNcxRef;
	ambulant::net::url mUri;
	string mTimeOffset;
	string mCharOffset;
	bool mbHasTimeOffset;
	bool mbHasCharOffset;
};

//!Generic mark class
class PositionMark
{
public:
	PositionData* mpStart;
	amis::MediaGroup* mpNote;
	bool mbHasNote;

	enum MarkType
	{
		BOOKMARK = 0,
		HILITE = 1
	};

	MarkType mType;
};

//!Bookmark class
class Bookmark : public PositionMark
{
public:
	Bookmark()
	{
		mType = BOOKMARK;
	}
};

//!Hilite class
class Hilite : public PositionMark
{
public:	
	Hilite()
	{
		mType = HILITE;
	}

	PositionData* mpEnd;
};

//!BookmarkSet class
class BookmarkSet
{

public:
	BookmarkSet();
	~BookmarkSet();

	unsigned int getNumberOfItems();

	amis::dtb::PositionMark* getItem(unsigned int);
	amis::dtb::PositionData* getLastmark();
	string getUid();
	amis::MediaGroup* getTitle();

	void addHilite(Hilite*);
	void addBookmark(Bookmark*);

	void deleteItem(unsigned int);

	void print();
	void printPositionData(PositionData*);
	void printMediaGroup(amis::MediaGroup*);

	void setTitle(amis::MediaGroup*);
	void setUid(string);
	void setLastmark(PositionData*);
	void setFilepath(ambulant::net::url*);
	const ambulant::net::url* getFilepath();
	int getIndexOf(PositionMark*);

private:
	amis::MediaGroup* mpTitle;
	string mUid;
	PositionData* mpLastmark;

	//bookmarks and hilites
	vector<PositionMark*> mItems;

	ambulant::net::url mFilepath;
};
}
}
#endif
