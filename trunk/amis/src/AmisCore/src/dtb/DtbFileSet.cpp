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

//this is where macros are defined, like the compiler test below
#include "AmisCore.h"

#if defined(AMIS_COMPILER_MSVC)
//windows header containing charset conversion macros
#include <afxpriv.h>
#endif

//for string transformations
#include <algorithm>
#include <iostream>

#include "dtb/DtbFileSet.h"
#include "util/FilePathTools.h"
#include "util/findfilecontainer.h"
#include "util/Log.h"

//DTB fileset utility class
amis::dtb::DtbFileSet::DtbFileSet()
{
}
amis::dtb::DtbFileSet::~DtbFileSet()
{
	mTextFiles.clear();
}

void amis::dtb::DtbFileSet::initWithNccOrOpf(const ambulant::net::url* filepath, amis::util::SearchForFiles* fileSearcher)
{
	ambulant::net::url temp_url = filepath->get_base();
	std::string book_dir = temp_url.get_url();
	book_dir += "/";
	if (temp_url.is_local_file())
		mBookDirectory = ambulant::net::url::from_filename(book_dir);
	else
		mBookDirectory = ambulant::net::url::from_url(book_dir);
	mpFileSearcher = fileSearcher;

	if (isNccFile(filepath))
	{
		this->mNavFilepath = *filepath;
		findTextFiles(fileSearcher);
	}
	else if (isOpfFile(filepath))
	{
		this->mOpfFilepath = *filepath;
		//resource and nav and text filepaths will be retrieved after parsing the OPF file
	}
}

//after the caller has parsed the ncc or opf file, additional data is available
void amis::dtb::DtbFileSet::setAdditionalDataAfterInitialParse(wstring uid, 
															   const ambulant::net::url* navfile, 
															   const ambulant::net::url* resourcefile,
															   amis::BookList* bookHistory)
{
	if (uid.size() > 0)
	{
		amis::BookEntry* p_entry = NULL;
		//try to find the book in the history and grab its bookmarks filepath
		if (bookHistory != NULL)
		{
			p_entry = bookHistory->findByUid(uid);
			if (p_entry != NULL && p_entry->mBmkPath.is_empty_path() == false)
				mBookmarksFilepath = p_entry->mBmkPath;
		}
		//if the bookmarks filepath wasn't found, then calculate it
		//it will be recorded in the history list by the main application
		if (mBookmarksFilepath.is_empty_path() == true)
		{
			std::string thefilename = calculateSafeBookmarksFilename(uid);
			mBookmarksFilepath = ambulant::net::url::from_filename(thefilename);
			mBookmarksFilepath = mBookmarksFilepath.join_to_base(mBookmarksDirectory);
		}
	}
	else
	{
		amis::util::Log::Instance()->writeError("No UID found.  Bookmarking unavailable.", "DtbFileSet::setAdditionalDataAfterInitialParse");
	}
	if (navfile != NULL)
	{
		mNavFilepath = navfile->join_to_base(mBookDirectory);
	}

	if (resourcefile != NULL)
	{
		mResourceFilepath = resourcefile->join_to_base(mBookDirectory);
	}
}

//set the user's bookmarks directory
void amis::dtb::DtbFileSet::setBookmarksDirectory(const ambulant::net::url* bookmarksDirectory)
{
	assert(bookmarksDirectory);
	mBookmarksDirectory = *bookmarksDirectory;
}

//get the root directory of the book
const ambulant::net::url* amis::dtb::DtbFileSet::getBookDirectory()
{
	return &mBookDirectory;
}
//get the path to the navigation (ncc or ncx) file
const ambulant::net::url* amis::dtb::DtbFileSet::getNavFilepath()
{
	return &mNavFilepath;
}
//get the path to the opf file
const ambulant::net::url* amis::dtb::DtbFileSet::getOpfFilepath()
{
	return &mOpfFilepath;
}
//get the path to the resource file
const ambulant::net::url*  amis::dtb::DtbFileSet::getResourceFilepath()
{
	return &mResourceFilepath;
}

//get the complete path to the bookmarks file
const ambulant::net::url* amis::dtb::DtbFileSet::getBookmarksFilepath()
{
	return &mBookmarksFilepath;
}

//these two functions (setSmilFile, getSmilFile) are not used when Ambulant is hooked up
//expected: relative file path
void amis::dtb::DtbFileSet::setSmilFile(const ambulant::net::url* file)
{
	mSmilFilepath = file->join_to_base(mBookDirectory);
}

const ambulant::net::url* amis::dtb::DtbFileSet::getSmilFile()
{
	return &mSmilFilepath;
}


//is this an ncc file?
bool amis::dtb::DtbFileSet::isNccFile(const ambulant::net::url* filepath)
{
	string file_name = amis::util::FilePathTools::getFileName(filepath->get_path());
	//convert the string to lower case before doing a comparison
	std::transform(file_name.begin(), file_name.end(), file_name.begin(), (int(*)(int))tolower);
	
	if (file_name.compare("ncc.html") == 0 ||
		file_name.compare("ncc.pdtb") == 0)
		return true;
	else
		return false;
}

//is this an opf file?
bool amis::dtb::DtbFileSet::isOpfFile(const ambulant::net::url* filepath)
{
	string file_ext = amis::util::FilePathTools::getExtension(filepath->get_path());
	 //convert the string to lower case before doing a comparison
	 std::transform(file_ext.begin(), file_ext.end(), file_ext.begin(), (int(*)(int))tolower);

	 if (file_ext.compare("opf") == 0)
		 return true;
	 else
		 return false;
}

string amis::dtb::DtbFileSet::calculateSafeBookmarksFilename(wstring uid)
{
#if defined(AMIS_COMPILER_MSVC)
	//windows utility for converting wstring to string
	USES_CONVERSION;
#endif
	string proposed_filename;
#if defined(AMIS_COMPILER_MSVC)
	proposed_filename.assign(T2A(uid.c_str()));
#elif defined(AMIS_COMPILER_GCC)
	//TODO: something for other platforms
	proposed_filename.assign("the_bookmark_file");
#endif
	
	string safe_filename;
	//make sure the bookmark filename contains only ASCII A-Z, 1-9 characters and hyphens
	for (unsigned int i=0; i<proposed_filename.length(); i++)
	{
		int c = proposed_filename[i];
		/*the safe values of ASCII characters are:
		32 (space)
		45 (hyphen)
		46 (period)
		95 (underscore)
		48 through 57 (0 through 9)
		65 through 90 (A through Z)
		97 through 122 (a through z)
		*/
		if (c == 32 || c == 45 || c == 46 || c == 95 
			|| (c >= 48 && c <= 57)
			|| (c >= 65 && c <= 90) 
			|| (c >=97 && c<=122))
		{
			safe_filename += proposed_filename[i];
		}
		else
		{
			//use the character code directly (or an underscore)
			char ch[3];
			itoa(c, ch, 10);
			safe_filename.append(ch);
		}
	}
	
	safe_filename.append(".bmk");
	return safe_filename;
}

void amis::dtb::DtbFileSet::print()
{
	cout<<"Files are:"<<endl;
	cout<<"\t Nav = *"<<mNavFilepath.get_url()<<"*"<<endl;
	cout<<"\t Book dir = *"<<mBookDirectory.get_url()<<"*"<<endl;
	cout<<"\t Res file = *"<<mResourceFilepath.get_url()<<"*"<<endl;
	cout<<"\t Opf file = *"<<mOpfFilepath.get_url()<<"*"<<endl;
	cout<<"\t Bmk dir = *"<<mBookmarksDirectory.get_url()<<"*"<<endl;
	cout<<"\t Bmk file = *"<<mBookmarksFilepath.get_url()<<"*"<<endl;
	cout<<endl;
}

/*one limitation to this function is that if there are multiple text files,
their search order is determined alphabetically
to fix this, we would need to look at all the SMIL file text references and sort out the order
there is a map of <text-src, text-id> in the Dtb class, but i don't know if it is 
stored in the order it was created,
and i don't know how to get at the key (text-src) by itself (maybe map_iter.first)?

*/
void amis::dtb::DtbFileSet::findTextFiles(amis::util::SearchForFiles* fileSearcher)
{
	if (fileSearcher == NULL) return;
	//searching won't work on network-based books yet
	if (mBookDirectory.is_local_file() == false) return;

	mTextFiles.clear();

	//if we are not dealing with an OPF, then we have to look for the text files ourself
	if (this->mOpfFilepath.is_empty_path())
	{
		//we want to find the HTML files that are not NCC files
		//mpFileSearcher->addSearchCriteria(".html");
		mpFileSearcher->addSearchCriteria(".htm");
		mpFileSearcher->addSearchExclusionCriteria("ncc.html");
		mpFileSearcher->addSearchExclusionCriteria("._");
		mpFileSearcher->addSearchExclusionCriteria(".svn-base");
		mpFileSearcher->addSearchExclusionCriteria(".opf");
		mpFileSearcher->setRecursiveSearch(true);
		mpFileSearcher->startSearch(mBookDirectory.get_file());
	
		mTextFiles = (*(mpFileSearcher->getSearchResults()));
		
		//if we didn't find any text files, then use the nav file as our text file
		if (mTextFiles.size() == 0) mTextFiles.push_back(this->mNavFilepath);
	}
}

void amis::dtb::DtbFileSet::setTextFile(const ambulant::net::url* file)
{
	mTextFiles.clear();
	if (!file->is_empty_path())
	{
		ambulant::net::url textfile = file->join_to_base(mBookDirectory);
		mTextFiles.push_back(textfile);
	}
}

amis::UrlList* amis::dtb::DtbFileSet::getTextFiles()
{
	return &mTextFiles;
}

//this should be a full path (inconsistent, I know, but at least documented)
void amis::dtb::DtbFileSet::setProtectedNavFilepath(const ambulant::net::url* file)
{
	mProtectedNavFilepath = *file;
}

const ambulant::net::url* amis::dtb::DtbFileSet::getProtectedNavFilepath()
{
	return &mProtectedNavFilepath;
}

//this should be a full path
void amis::dtb::DtbFileSet::setProtectedBookKeyFilepath(const ambulant::net::url* file)
{
	mProtectedBookKeyFilepath = *file;
}
const ambulant::net::url* amis::dtb::DtbFileSet::getProtectedBookKeyFilepath()
{
	return &mProtectedBookKeyFilepath;
}
