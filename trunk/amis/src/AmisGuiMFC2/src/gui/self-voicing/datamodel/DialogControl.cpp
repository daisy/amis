/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

Copyright (c) 2004 DAISY Consortium

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


#include "gui/self-voicing/datamodel/DialogControl.h"
#include "gui/self-voicing/datamodel/Switch.h"
#include <fstream>

using namespace amis::gui::spoken;

DialogControl::DialogControl()
{
	setUiItemType(CONTROL);
	
	mMnemonic = NULL;
}

DialogControl::~DialogControl()
{
	if (mMnemonic) delete  mMnemonic;
}

void DialogControl::setMnemonic(Label* mnemonic)
{
	this->mMnemonic = mnemonic;
}
Label* DialogControl::getMnemonic()
{
	return mMnemonic;
}
std::string DialogControl::getWidgetCategories()
{
	return mWidgetCategories;
}

void DialogControl::setWidgetCategories(std::string widgetCategories)
{
	mWidgetCategories.assign(widgetCategories);
}

