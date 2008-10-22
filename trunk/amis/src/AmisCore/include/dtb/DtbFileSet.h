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
#ifndef DTBFILESET_H
#define DTBFILESET_H

#include <string>
#include "AmisCore.h"
#include "ambulant/net/url.h"
#include "util/SearchForFiles.h"
#include "BookList.h"

using namespace std;

namespace amis
{
namespace dtb
{
class DtbFileSet
{
public:
	DtbFileSet();
	~DtbFileSet();
	//!initialize with NCC or OPF filepath
	void initWithNccOrOpf(const ambulant::net::url*, amis::util::SearchForFiles*);
	//utility functions
	static bool isNccFile(const ambulant::net::url*);
	static bool isOpfFile(const ambulant::net::url*);
	//set additional information
	void setBookmarksDirectory(const ambulant::net::url*);
	void setAdditionalDataAfterInitialParse(wstring, 
		const ambulant::net::url*, 
		const ambulant::net::url*,
		amis::BookList*);
	//get data
	const ambulant::net::url* getBookDirectory();
	const ambulant::net::url* getNavFilepath();
	const ambulant::net::url* getOpfFilepath();
	const ambulant::net::url* getResourceFilepath();
	const ambulant::net::url* getBookmarksFilepath();
	void setSmilFile(const ambulant::net::url*);
	const ambulant::net::url* getSmilFile();
	void print();
	void setTextFile(const ambulant::net::url*);
	amis::UrlList* getTextFiles();
	
	void setProtectedNavFilepath(const ambulant::net::url*);
	const ambulant::net::url* getProtectedNavFilepath();
	void setProtectedBookKeyFilepath(const ambulant::net::url*);
	const ambulant::net::url* getProtectedBookKeyFilepath();

private:
	string calculateSafeBookmarksFilename(wstring);
	void findTextFiles(amis::util::SearchForFiles*);

	ambulant::net::url mNavFilepath;
	ambulant::net::url mBookDirectory;
	ambulant::net::url mResourceFilepath;
	ambulant::net::url mOpfFilepath;
	ambulant::net::url mBookmarksDirectory;
	ambulant::net::url mBookmarksFilepath;
	ambulant::net::url mSmilFilepath;
	ambulant::net::url mProtectedNavFilepath;
	ambulant::net::url mProtectedBookKeyFilepath;
	amis::util::SearchForFiles* mpFileSearcher;
	UrlList mTextFiles;
};
}
}
#endif
