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
#ifndef SEARCHFORFILES_H
#define SEARCHFORFILES_H

#include <string>
#include <vector>
#include "AmisCore.h"
#include "Media.h"
#include "ambulant/net/url.h"

namespace amis
{
namespace util
{

//will we need this struct?  it is leftover.
//if so, does it belong here?
struct BookInfo
{
	amis::MediaGroup* mpTitle;
	string mFilePath;
};

//this class is mostly virtual
//it needs platform-specific implementations
//one example is the MFC implementation in AMIS for Windows
class SearchForFiles
{
public:
	SearchForFiles();
	virtual ~SearchForFiles()=0;

	virtual int startSearch(std::string)=0;
	
	void setRecursiveSearch(bool);
	bool isRecursiveSearch();

	//clears search results, search criteria, and search-exclusion criteria
	void clearAll();

	void addSearchCriteria(std::string);
	amis::StdStringList* getSearchCriteria();
	void clearSearchCriteria();
	bool matchesSearchCriteria(std::string);

	void addSearchExclusionCriteria(std::string);
	amis::StdStringList* getSearchExclusionCriteria();
	void clearSearchExclusionCriteria();
	bool matchesSearchExclusionCriteria(std::string);

	//this function should ONLY be called by implementations of SearchForFiles
	void addSearchResult(const ambulant::net::url*);

	amis::UrlList* getSearchResults();
	void clearSearchResults();

private:
	bool matchesList(std::string, amis::StdStringList);
	bool mbIsRecursive;
	amis::StdStringList mCriteria;
	amis::StdStringList mExclusions;
	amis::UrlList mResults;
};
}
}
#endif