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