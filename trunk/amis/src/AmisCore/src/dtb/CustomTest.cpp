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

#include "dtb/CustomTest.h"
#include <iostream>

using namespace std;

amis::dtb::CustomTest::CustomTest()
{
	mId="";
	mbOverride = true;
	mbDefaultState = false;
	mBookStruct = "";
	mbCurrentState = false;
}
amis::dtb::CustomTest::~CustomTest()
{
	mId = "";
	mBookStruct = "";
}
void amis::dtb::CustomTest::setId(const string id)
{
	mId.assign(id);
}
void amis::dtb::CustomTest::setOverride(bool override)
{
	mbOverride = override;
}
void amis::dtb::CustomTest::setDefaultState(bool defaultState)
{
	mbDefaultState = defaultState;
}
void amis::dtb::CustomTest::setBookStruct(const string bookStruct)
{
	mBookStruct.assign(bookStruct);
}
void amis::dtb::CustomTest::setCurrentState(bool currentState)
{
	mbCurrentState = currentState;
}
const string amis::dtb::CustomTest::getId()
{
	return mId;
}
bool amis::dtb::CustomTest::getOverride()
{
	return mbOverride;
}
bool amis::dtb::CustomTest::getDefaultState()
{
	return mbDefaultState;
}
const string amis::dtb::CustomTest::getBookStruct()
{
	return mBookStruct;
}
bool amis::dtb::CustomTest::getCurrentState()
{
	return mbCurrentState;
}
amis::dtb::CustomTestSet::CustomTestSet()
{
}
amis::dtb::CustomTestSet::~CustomTestSet()
{
	int sz = mList.size();
	for (int i = sz-1; i>=0; i--)
	{
		CustomTest* p_temp = mList[i];
		mList.pop_back();
		delete p_temp;
	}
}
//only add if it doesn't already exist
bool amis::dtb::CustomTestSet::addCustomTest(CustomTest* pCustomTest)
{
	bool b_found = false;
	for (int i = 0; i<mList.size(); i++)
	{
		CustomTest* p_temp = mList[i];
		if (pCustomTest->getId().compare(p_temp->getId()) == 0)
		{
			b_found = true;
			break;
		}
	}
	if (b_found == false) mList.push_back(pCustomTest);
	return !b_found;
}
amis::dtb::CustomTest* amis::dtb::CustomTestSet::getCustomTest(int idx)
{
	if (idx >= 0 && idx < mList.size())
		return mList[idx];
	else
		return NULL;
}

int amis::dtb::CustomTestSet::getLength()
{
	return mList.size();
}
