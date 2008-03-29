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
using namespace std;

namespace amis
{
namespace dtb
{
//!Custom test object represents a custom test structure in a DTB
class CustomTest
{
public:
	//!default constructor
	CustomTest();
	//!destructor
	~CustomTest();

	//!set the id
	void setId(const string);
	//!set the user override 
	void setOverride(bool);
	//!set the default state
	void setDefaultState(bool);
	//!set the book struct name
	void setBookStruct(const string);
	//!set the current state
	void setCurrentState(bool);

	//!get the id
	const string getId();
	//!get the user override
	bool getOverride();
	//!get the default state
	bool getDefaultState();
	//!get the book struct name
	const string getBookStruct();
	//!get the current state
	bool getCurrentState();

private:
	//!custom test id
	string mId;
	//!can the user override the default state? (true = encourage, false = discourage)
	bool mbOverride;
	//!default state (true = render, false = skip)
	bool mbDefaultState;
	//!book structure name
	string mBookStruct;
	//!current state (true = render, false = skip)
	bool mbCurrentState;

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
