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

//for string transformations
#include <algorithm>

#include "util/SearchForFiles.h"

amis::util::SearchForFiles::SearchForFiles()
{
	mbIsRecursive = true;
	mCriteria.clear();
	mExclusions.clear();
}
amis::util::SearchForFiles::~SearchForFiles()
{
}

void amis::util::SearchForFiles::setRecursiveSearch(bool isRecursive)
{
	mbIsRecursive = isRecursive;
}

bool amis::util::SearchForFiles::isRecursiveSearch()
{
	return mbIsRecursive;
}

/*!
	search criteria is defined as a desired substring of a filename
	so to find all the opf files, just ask for .opf.  not *.opf.
*/
void amis::util::SearchForFiles::addSearchCriteria(std::string criterion)
{
	std::string criterion_lower = criterion;
	//make lower case
	std::transform(criterion_lower.begin(), criterion_lower.end(), criterion_lower.begin(), (int(*)(int))tolower);
	mCriteria.push_back(criterion_lower);
}

amis::StdStringList* amis::util::SearchForFiles::getSearchCriteria()
{
	return &mCriteria;
}
void amis::util::SearchForFiles::addSearchExclusionCriteria(std::string criterion)
{
	std::string criterion_lower = criterion;
	//make lower case
	std::transform(criterion_lower.begin(), criterion_lower.end(), criterion_lower.begin(), (int(*)(int))tolower);
	mExclusions.push_back(criterion_lower);
}

amis::StdStringList* amis::util::SearchForFiles::getSearchExclusionCriteria()
{
	return &mExclusions;
}
void amis::util::SearchForFiles::clearSearchExclusionCriteria()
{
	mExclusions.clear();
}
bool amis::util::SearchForFiles::matchesSearchCriteria(std::string data)
{
	return matchesList(data, mCriteria);
}
bool amis::util::SearchForFiles::matchesSearchExclusionCriteria(std::string data)
{
	//be sure not to return true if the list is empty .. it gives the wrong impression
	if (mExclusions.size() == 0) return false;
	else return matchesList(data, mExclusions);
}

bool amis::util::SearchForFiles::matchesList(std::string data, StdStringList list)
{
	std::string data_lower = data;
	//make lower case
	std::transform(data_lower.begin(), data_lower.end(), data_lower.begin(), (int(*)(int))tolower);
	
	for (int i=0; i<list.size(); i++)
	{
		if (data_lower.find(list[i]) != std::string::npos) return true;
	}
	if (list.size() > 0) return false;
	//if there are no criteria, everything is always a match
	else return true;
}
void amis::util::SearchForFiles::clearSearchCriteria()
{
	mCriteria.clear();
}

//this function should ONLY be called by implementations of SearchForFiles
void amis::util::SearchForFiles::addSearchResult(const ambulant::net::url* searchResult)
{
	mResults.push_back(*searchResult);
}

amis::UrlList* amis::util::SearchForFiles::getSearchResults()
{
	return &mResults;
}

void amis::util::SearchForFiles::clearSearchResults()
{
	mResults.clear();
}

void amis::util::SearchForFiles::clearAll()
{
	clearSearchCriteria();
	clearSearchExclusionCriteria();
	clearSearchResults();
}