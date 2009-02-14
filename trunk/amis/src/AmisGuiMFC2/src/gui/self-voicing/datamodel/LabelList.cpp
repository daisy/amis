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

#include "gui/self-voicing/datamodel/LabelList.h"
#include <fstream>

using namespace amis::gui::spoken;

LabelList::LabelList()
{
	mLabels.empty();
}
LabelList::~LabelList()
{
	int sz = mLabels.size();
	Label* p_label = NULL;

	for (int i=sz-1; i>=0; i--)
	{
		p_label = mLabels[i];
		mLabels.pop_back();

		if (p_label!=NULL)
			delete p_label;

		p_label = NULL;
	}
}

int LabelList::getNumberOfLabels()
{
	return mLabels.size();
}

Label* LabelList::getLabel(int idx)
{
	if (idx >= 0 && idx < mLabels.size())
	{
		return mLabels[idx];
	}
	else
	{
		return NULL;
	}
}

bool LabelList::doesListRepeat()
{
	return mbRepeat;
}

void LabelList::setRepeat(bool repeat)
{
	mbRepeat = repeat;
}

void LabelList::addLabel(Label* pLabel)
{
	mLabels.push_back(pLabel);
}
