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
#ifndef AMISSIDEBARLOADER_H
#define AMISSIDEBARLOADER_H

#include "dtb/nav/NavModel.h"
#include "dtb/nav/NavVisitor.h"
#include "gui/sidebar/AmisSidebar.h"
#include "dtb/nav/PageList.h"
#include "dtb/nav/NavList.h"

namespace amis
{
namespace gui
{
namespace sidebar
{
//this class is in charge of loading data into the sidebar
//basically, it's the glue between the data model and the widget
class AmisSidebarLoader : public amis::dtb::nav::NavVisitor
{
protected:
	AmisSidebarLoader();

public:
	static AmisSidebarLoader* Instance();
	void DestroyInstance();
	~AmisSidebarLoader();

	void loadNavigationData(amis::dtb::nav::NavModel*, amis::gui::sidebar::CAmisSidebar*);
	bool preVisit(amis::dtb::nav::NavNode*);

private:
	void loadNavList(amis::dtb::nav::NavList*, CListCtrl*);
	std::wstring getTextLabel(amis::dtb::nav::NavNode*);
	std::wstring getTextLabel(amis::dtb::nav::NavContainer*);

	amis::gui::sidebar::CAmisSidebar* mpSidebar;
	vector<HTREEITEM> mWidgetNodes;

private:
	static AmisSidebarLoader* pinstance;
};
}
}
}
#endif