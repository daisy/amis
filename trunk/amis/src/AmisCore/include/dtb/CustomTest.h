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
#ifndef CUSTOMTEST_H
#define CUSTOMTEST_H

#include <string>
#include <vector>
#include "Media.h"

using namespace std;

namespace amis
{
namespace dtb
{
//!Custom test object represents a custom test structure in a DTB
class CustomTest
{
public:
	CustomTest();
	~CustomTest();

	void setId(const string);
	void setOverride(bool);
	void setDefaultState(bool);
	void setBookStruct(const string);
	void setCurrentState(bool);
	void setLabel(amis::MediaGroup*);

	const string getId();
	bool getOverride();
	bool getDefaultState();
	const string getBookStruct();
	bool getCurrentState();
	amis::MediaGroup* getLabel();

private:
	string mId;
	bool mbOverride;
	bool mbDefaultState;
	string mBookStruct;
	bool mbCurrentState;
	amis::MediaGroup* mpLabel;
};	

class CustomTestSet
{
public:
	CustomTestSet();
	~CustomTestSet();

	bool addCustomTest(CustomTest*);
	CustomTest* getCustomTest(int);
	int getLength();

private:
	vector <CustomTest*> mList;
};

}
}
#endif
