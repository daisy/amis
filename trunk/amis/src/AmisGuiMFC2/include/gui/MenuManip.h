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
#ifndef MENUMANIP_H
#define MENUMANIP_H

#include "dtb/Bookmarks.h"

namespace amis
{
namespace gui
{
class MenuManip
{
protected:
	MenuManip();

public:
	static MenuManip* Instance();
	void DestroyInstance();
	~MenuManip();
	void refreshRecentBooksListMenu();
	void setViewItemCheckmark(bool, UINT);
	void setPauseState(bool pauseState);
	void addBookmark(amis::dtb::PositionMark*);
	void loadBookmarks(amis::dtb::BookmarkSet*);
	void clearBookmarks();
	void setupNavigationOptions();
	void addPageStyles();
	void addNavContainersToViewMenu();
private:
	static MenuManip* pinstance;
};
}
}

#endif