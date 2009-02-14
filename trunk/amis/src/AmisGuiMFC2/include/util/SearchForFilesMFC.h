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
#ifndef SEARCHFORFILESMFC_H
#define SEARCHFORFILESMFC_H

#include "stdafx.h"
#include <string>
#include "util/SearchForFiles.h"

namespace amis
{
namespace util
{
class SearchForFilesMFC : public amis::util::SearchForFiles
{
public:
	SearchForFilesMFC();
	~SearchForFilesMFC();

	//the parameter is a folder path, with or without a trailing slash, e.g. "c:\daisybooks"
	int startSearch(std::string);
	void stopSearch();
	int startSearchOnCdRom();

private:
	int recursiveSearch(LPCTSTR);
	bool mbShouldStop;
};
}
}
#endif