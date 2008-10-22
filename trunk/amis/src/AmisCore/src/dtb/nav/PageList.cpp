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

#include <iostream>
#include "dtb/nav/PageList.h"
#include "dtb/nav/NavModel.h"
#include "util/FilePathTools.h"

using namespace std;

amis::dtb::nav::PageList::PageList()
{
	//the default id for this container
	setId("pages");
}

amis::dtb::nav::PageList::~PageList()
{
}

//override: add node to list
void amis::dtb::nav::PageList::addNode(NavTarget* pNode)
{
	//only add page targets
	if (pNode->getTypeOfNode() == amis::dtb::nav::NavNode::PAGE_TARGET)
	{
		//remember that the base class takes care of adding this node to the play order list
		amis::dtb::nav::NavList::addNode(pNode);
		pNode->setNavContainer(this);
	}
}


//start and end are playorder values; use end=-1 for the end of a set
int amis::dtb::nav::PageList::countPagesInRange(int start, int end)
{
	int page_count = 0;

	for (unsigned int i=0; i<this->getLength(); i++)
	{
		int play_order = getAtIndex(i)->getPlayOrder();
		if (end != -1 && play_order >= start && play_order < end)
			page_count++;
		else if (end == -1 && play_order >= start)
			page_count++;

	}

	return page_count;
}

//go to a page with the given label
amis::dtb::nav::PageTarget* amis::dtb::nav::PageList::findPage(wstring pageLabel)
{
	amis::MediaGroup* p_label;
	bool b_found = false;

	unsigned int i = 0;
	for (i=0; i<this->getLength(); i++)
	{
		p_label = this->getAtIndex(i)->getLabel();
		if (p_label->getText()->getTextString().compare(pageLabel) == 0)
		{
			b_found = true;
			break;
		}
	}

	if (b_found == true) return (PageTarget*)this->getAtIndex(i);
	else return NULL;
}

void amis::dtb::nav::PageList::print()
{
	cout<<"Page List"<<endl;
	amis::dtb::nav::NavList::print(false);
}
