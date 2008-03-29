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
#ifndef PAGELIST_H
#define PAGELIST_H

#include "dtb/nav/PageTarget.h"
#include "dtb/nav/NavList.h"

namespace amis
{
namespace dtb
{
namespace nav
{
class PageList : public NavList
{
public:
	PageList();
	~PageList();

	int countPagesInRange(int, int);
	PageTarget* findPage(wstring);
	//override
	void addNode(NavTarget*);
	void print();

};
}
}
}
#endif
