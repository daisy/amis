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
#ifndef BOOKLIST_H
#define BOOKLIST_H

#include <string>
#include <vector>
#include "ambulant/net/url.h"
#include "Media.h"

using namespace std;

namespace amis
{
class BookEntry
{
public:
	BookEntry();
	~BookEntry();
	void setTitleText(wstring);
	void setTitleAudio(string, string, string);
	wstring getTitleText();
	amis::AudioNode* getTitleAudio();
	void setTitleMedia(amis::MediaGroup*);

	std::wstring mUid;
	ambulant::net::url mPath;
	ambulant::net::url mBmkPath;
	bool mbIsLastRead;

private:
	amis::MediaGroup* mpTitle;
	
};
class BookList
{
public:
	BookList();
	~BookList();	
	int getNumberOfEntries();
	amis::BookEntry* getEntry(unsigned int);
	void addEntry(amis::BookEntry*);
	void deleteEntry(unsigned int);
	amis::BookEntry* getLastRead();
	const ambulant::net::url* getFilepath();
	void setFilepath(const ambulant::net::url*);
	void setName(std::wstring);
	std::wstring getName();
	amis::BookEntry* findByUid(wstring);
	void clearLastRead();
private:
	void cleanUpVector();
	ambulant::net::url mFilepath;
	vector<amis::BookEntry*> mItems;
	std::wstring mName;
};
}
#endif
