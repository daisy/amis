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
#ifndef DTB_H
#define DTB_H

#include "AmisCore.h"
#include "dtb/Bookmarks.h"
#include "dtb/nav/NavModel.h"
#include "dtb/Metadata.h"
#include "dtb/Spine.h"
#include "dtb/smil/SmilTree.h"
#include "dtb/smil/SmilMediaGroup.h"
#include "dtb/DtbFileSet.h"
#include "ambulant/net/url.h"
#include "util/SearchForFiles.h"
	
#include "util/ThreadYielder.h"

namespace amis
{
namespace dtb
{
typedef bool (*ProtectedBookKeyHandlerFunctionPtr)(const ambulant::net::url*, const ambulant::net::url*);
enum DaisyVersion {UNSUPPORTED, DAISY_202, DAISY_2005};
class Dtb
{
public:
	Dtb();
	~Dtb();

	void setThreadYielder(ThreadYielder * ty);

	bool open(const ambulant::net::url*, const ambulant::net::url*, amis::BookList*);
	bool open(string, string, amis::BookList*);
	amis::dtb::smil::SmilMediaGroup* startReading(bool);
	amis::dtb::smil::SmilMediaGroup* nextPhrase();
	amis::dtb::smil::SmilMediaGroup* previousPhrase();
	amis::dtb::smil::SmilMediaGroup* loadSmilFromUrl(ambulant::net::url*);
	amis::dtb::Bookmark* addBookmark(amis::MediaGroup*);
	amis::MediaGroup* getTitle();
	amis::MediaGroup* getAuthor();
	amis::dtb::Metadata* getMetadata();
	amis::dtb::Spine* getSpine();
	amis::dtb::nav::NavModel* getNavModel();
	amis::dtb::DtbFileSet* getFileSet();
	wstring getUid();
	amis::dtb::BookmarkSet* getBookmarks();
	amis::dtb::CustomTestSet* getCustomTestSet();
	void setFileSearcher(amis::util::SearchForFiles*);
	amis::util::SearchForFiles* getFileSearcher();
	string searchFullText(wstring, ambulant::net::url, int);
	amis::dtb::smil::SmilTree* getSmilTree();
	string searchFullTextNext();
	string searchFullTextPrevious();
	void setNewLastmark(ambulant::net::url);
	ambulant::net::url calculateUriOfData(amis::dtb::smil::SmilMediaGroup*);
	amis::dtb::nav::NavNode* getCurrentNavNode();
	wstring getLastTextSearchString();

	amis::dtb::DaisyVersion getDaisyVersion();
	bool isProtected();
	void setCallbackForPreprocessingBookKey(ProtectedBookKeyHandlerFunctionPtr);
	bool hasText();

private:
	void saveIndexData(bool check = false);
	void readIndexData();
	bool indexExistsOnDisk();
	
	bool processNcc(const ambulant::net::url*, bool);
	bool processNcx(const ambulant::net::url*, bool);
	bool processOpf(const ambulant::net::url*);
	bool processDaisyResourceFile(const ambulant::net::url*);
	bool processSmil(const ambulant::net::url*);
	void loadBookmarks(const ambulant::net::url*);
	void cleanUpObjects();
#ifndef WITH_EXTERNAL_SMIL_PLAYER
	void setNewLastmark(amis::dtb::smil::SmilMediaGroup*);
#endif
	string searchFullTextRelative(int);
	bool checkForCopyProtection(amis::dtb::Metadata*);
	
private:
	ThreadYielder * mThreadYielder;
	amis::dtb::Metadata* mpMetadata;
	amis::dtb::smil::SmilTree* mpSmilTree;
	amis::dtb::Spine* mpSpine;
	amis::dtb::nav::NavModel* mpNavModel;
	amis::MediaGroup* mpTitle;
	amis::MediaGroup* mpAuthor;
	amis::dtb::BookmarkSet* mpBookmarks;
	amis::dtb::CustomTestSet* mpCustomTests;
	DtbFileSet* mpFiles;
	amis::BookList* mpHistory;
	std::wstring mUid;
	amis::StringMap* mpTextSmilMap;
	wstring mLastSearchString;
	string mLastSearchResult;
	string mLastSearchedTextFile;
	amis::util::SearchForFiles* mpFileSearcher;
	amis::dtb::DaisyVersion mDaisyVersion;
	amis::dtb::nav::NavNode* mpCurrentNavNode;
	bool mbIsProtected;
	ProtectedBookKeyHandlerFunctionPtr mpCallbackForPreprocessingBookKey;
	bool mbCanDecodePdtb;
};
}
}

#endif
