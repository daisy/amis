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
#include "AmisCore.h"
#if defined(AMIS_COMPILER_MSVC)
//windows header containing charset conversion macros
#include <afxpriv.h>
#endif

#include "dtb/Dtb.h"
#include "io/NccFileReader.h"
#include "io/SmilFileReader.h"
#include "io/BookmarksFileIO.h"
#include "util/FilePathTools.h"
#include "io/TextSearch.h"
#include "io/OpfFileReader.h"
#include "io/NcxFileReader.h"
#include "util/Log.h"
#include "util/FilePathTools.h"
#include <fstream>

#include "ambulant/common/plugin_engine.h"

//constructor
amis::dtb::Dtb::Dtb(string appPath)
{
	mThreadYielder = NULL;
	mpFiles = NULL;
	mpSmilTree = NULL;
	mpNavModel = NULL;
	mpAuthor = NULL;
	mpBookmarks = NULL;
	mpMetadata = NULL;
	mpSpine = NULL;
	mpTitle = NULL;
	mpTextSmilMap = NULL;
	mpCustomTests = NULL;
	mpFileSearcher = NULL;
	mUid.erase();
	mDaisyVersion = UNSUPPORTED;
	mpCallbackForPreprocessingBookKey = NULL;
	mAppPath = appPath;
}
//destructor
amis::dtb::Dtb::~Dtb()
{
	cleanUpObjects();
}

//delete objects
//we are purposely not deleting mpFileSearcher
//we are also purposely not deleting mpCallbackForPreprocessingBookKey
void amis::dtb::Dtb::cleanUpObjects()
{
	mbIsProtected = false;

	if (mpFiles != NULL)
		delete mpFiles;
	if (mpSmilTree != NULL)
		delete mpSmilTree;
	if (mpNavModel != NULL)
		delete mpNavModel;
	if (mpMetadata != NULL)
		delete mpMetadata;
	if (mpTitle != NULL)
	{
		mpTitle->destroyContents();
		delete mpTitle;
	}
	if (mpAuthor != NULL)
	{
		mpAuthor->destroyContents();
		delete mpAuthor;
	}
	if (mpBookmarks != NULL)
		delete mpBookmarks;
	if (mpSpine != NULL)
		delete mpSpine;
	if (mpTextSmilMap != NULL)
		delete mpTextSmilMap;
	if (mpCustomTests != NULL)
		delete mpCustomTests;

	mpFiles = NULL;
	mpSmilTree = NULL;
	mpNavModel = NULL;
	mpAuthor = NULL;
	mpBookmarks = NULL;
	mpMetadata = NULL;
	mpSpine = NULL;
	mpTitle = NULL;
	mpTextSmilMap = NULL;
	mpCustomTests = NULL;
	mpHistory = NULL;
	mUid.erase();
	mDaisyVersion = UNSUPPORTED;

}
amis::MediaGroup* amis::dtb::Dtb::getTitle()
{
	return mpTitle;
}
amis::MediaGroup* amis::dtb::Dtb::getAuthor()
{
	return mpAuthor;
}
amis::dtb::Spine* amis::dtb::Dtb::getSpine()
{
	return mpSpine;
}
amis::dtb::Metadata* amis::dtb::Dtb::getMetadata()
{
	return mpMetadata;
}
amis::dtb::nav::NavModel* amis::dtb::Dtb::getNavModel()
{
	return mpNavModel;
}
amis::dtb::DtbFileSet* amis::dtb::Dtb::getFileSet()
{
	return mpFiles;
}
amis::dtb::smil::SmilTree* amis::dtb::Dtb::getSmilTree()
{
#ifdef WITH_EXTERNAL_SMIL_PLAYER
	return NULL;
#else
	return mpSmilTree;
#endif
}
amis::dtb::CustomTestSet* amis::dtb::Dtb::getCustomTestSet()
{
	return mpCustomTests;
}
void amis::dtb::Dtb::setFileSearcher(amis::util::SearchForFiles* fileSearcher)
{
	mpFileSearcher = fileSearcher;
}
amis::util::SearchForFiles* amis::dtb::Dtb::getFileSearcher()
{
	return mpFileSearcher;
}

//--------------------------------------------------
//wrapper for opening files
//--------------------------------------------------
bool amis::dtb::Dtb::open(string filepath, string bookmarksDirectory, amis::BookList* history)
{
	ambulant::net::url fileUrl = amis::util::makeUrlFromString(filepath);
	ambulant::net::url bookmarksDirUrl = amis::util::makeUrlFromString(bookmarksDirectory);
	return open(&fileUrl, &bookmarksDirUrl, history);
}
void amis::dtb::Dtb::setThreadYielder(ThreadYielder * ty)
{
	mThreadYielder = ty;
}

//--------------------------------------------------
//open an NCC or OPF file
//--------------------------------------------------
bool amis::dtb::Dtb::open(const ambulant::net::url* fileUrl, 
						  const ambulant::net::url* bookmarksDirectory, 
						  amis::BookList* history)
{
	cleanUpObjects();
	mpHistory = history;
	string log_msg = "Opening book: " + fileUrl->get_url();
	amis::util::Log::Instance()->writeMessage(log_msg, "Dtb::open"); 

	mpFiles = NULL;
	mpFiles = new amis::dtb::DtbFileSet();
	mpFiles->initWithNccOrOpf(fileUrl, mpFileSearcher);
	mpFiles->setBookmarksDirectory(bookmarksDirectory);
	
	mpCurrentNavNode = NULL;
	if (DtbFileSet::isNccFile(fileUrl))
	{
		amis::util::Log::Instance()->writeMessage("This is a DAISY 2.02 book", "Dtb::open");	
		mDaisyVersion = DAISY_202;
		if (mThreadYielder != 0) mThreadYielder->peekAndPump();
		if (!processNcc(mpFiles->getNavFilepath(), fileUrl->is_local_file()))
		{
			return false;
		}
	}
	else if (DtbFileSet::isOpfFile(fileUrl))
	{
		if (mThreadYielder != 0) mThreadYielder->peekAndPump();
		if (!processOpf(mpFiles->getOpfFilepath()))
		{
			return false;
		}
		mDaisyVersion = DAISY_2005;
		amis::util::Log::Instance()->writeMessage("This is a Daisy 2005 book", "Dtb::open");
		if (!processNcx(mpFiles->getNavFilepath(), fileUrl->is_local_file())) return false;
		//if (!processDaisyResourceFile(mpFiles->getResourceFilepath())) return false;
	}
	else
	{ 
		mDaisyVersion = UNSUPPORTED;
		amis::util::Log::Instance()->writeWarning("The type of book could not be determined", 
			"Dtb::open");
		return false;
	}
	
	loadBookmarks(mpFiles->getBookmarksFilepath());
	amis::util::Log::Instance()->writeMessage("Opened book successfully", "Dtb::open");
	
	return true;
}

//--------------------------------------------------
//start reading the book
//--------------------------------------------------
amis::dtb::smil::SmilMediaGroup* amis::dtb::Dtb::startReading(bool loadLastmarkIfExists)
{
#ifdef WITH_EXTERNAL_SMIL_PLAYER
	return NULL;
#else
	amis::dtb::smil::SmilMediaGroup* p_smil_media = NULL;

	if (loadLastmarkIfExists == false || mpBookmarks == NULL || mpBookmarks->getLastmark() == NULL)
	{
		mpFiles->setSmilFile(mpSpine->getFirstFile());
		if (processSmil(mpFiles->getSmilFile()))
		{
			p_smil_media = mpSmilTree->firstPhrase();
			setNewLastmark(p_smil_media);
		}
	}
	else
	{
		if (mpBookmarks->getLastmark() != NULL)
		{
			ambulant::net::url* smilfile = &mpBookmarks->getLastmark()->mUri;
			string id = smilfile->get_ref();
			mpFiles->setSmilFile(smilfile);
		
			if (processSmil(mpFiles->getSmilFile()))
			{
				p_smil_media = mpSmilTree->goToId(id);
				setNewLastmark(p_smil_media);
			}
		}
	}
	return p_smil_media;
#endif
}

//--------------------------------------------------
//process an NCC file
//--------------------------------------------------
bool amis::dtb::Dtb::processNcc(const ambulant::net::url* filepath, bool isLocal)
{
	amis::io::NccFileReader ncc_file_reader;
	amis::dtb::nav::BuildSpineVisitor spine_visitor;
	//TODO: this might just apply for windows
	ncc_file_reader.setAreFilenamesLowercase(isLocal);
	if (!ncc_file_reader.readFromFile(filepath)) 
	{
		amis::util::Log::Instance()->writeError("Could not read NCC file!", "Dtb::processNcc");
		return false;
	}
	mpNavModel = ncc_file_reader.getNavModel();
	mpMetadata = ncc_file_reader.getMetadata();
	mpCustomTests = ncc_file_reader.getCustomTests();
	amis::dtb::nav::NavPoint* p_title = ncc_file_reader.getTitle();

	//if this NCC file turned out to be a protected book, then we need to get custom tests and a nav model
	//from an encrypted file
	mbCanDecodePdtb = false;
	if (checkForCopyProtection(mpMetadata))
	{
		amis::util::Log::Instance()->writeMessage("This is a protected book", "Dtb::processNcc");
		mbIsProtected = true;
		if (mpCallbackForPreprocessingBookKey != NULL) 
		{
			if (mpCallbackForPreprocessingBookKey(
				getFileSet()->getProtectedNavFilepath(),
				getFileSet()->getProtectedBookKeyFilepath()
				) == true)
			{
				//reparse the ncc.pdtb file
				if (!ncc_file_reader.readFromFile(getFileSet()->getProtectedNavFilepath()))
				{
					mbCanDecodePdtb = false;
					return false;
				}
				else
				{
					mbCanDecodePdtb = true;
					if (mpNavModel) delete mpNavModel;
					if (mpCustomTests) delete mpCustomTests;
					mpNavModel = ncc_file_reader.getNavModel();
					mpCustomTests = ncc_file_reader.getCustomTests();
					p_title = ncc_file_reader.getTitle();
				}
			}
		}
		else
		{
			/* if: 1. no function registered to handle protected books or
					2. that function failed
			then: playback of NCC.html continues and it will say "this book is protected... "
			*/
			mbCanDecodePdtb = false;
			amis::util::Log::Instance()->writeError("Protected book could not be read", 
				"Dtb::processNcc");
		}
	}

	//the spine visitor also makes the smil file paths absolute
	mpSpine = spine_visitor.getSpine(mpNavModel, this->getFileSet()->getBookDirectory());

	//fill in the text for the title and the author
	mUid = getUid();
	mpTitle = new amis::MediaGroup();
	amis::TextNode* p_title_text = new amis::TextNode();
	p_title_text->setTextString(mpMetadata->getMetadataContent("dc:title"));
	mpTitle->setText(p_title_text);
	
	mpAuthor = new amis::MediaGroup();
	amis::TextNode* p_author_text = new amis::TextNode();
	p_author_text->setTextString(mpMetadata->getMetadataContent("dc:creator"));
	mpAuthor->setText(p_author_text);

	mpFiles->setAdditionalDataAfterInitialParse(mUid, NULL, NULL, mpHistory);
	amis::dtb::nav::ResolveSmilDataVisitor resolve_smil_visitor;
	
	if (mThreadYielder != 0) mThreadYielder->peekAndPump();

	resolve_smil_visitor.setThreadYielder(mThreadYielder);
	
	if (!indexExistsOnDisk())
	{
		resolve_smil_visitor.resolve(mpNavModel, mpSpine, true);
		this->mpTextSmilMap = resolve_smil_visitor.getSmilTextMap();	
		if (mCacheIndex) saveIndexData();
	}
	else
	{
		//try to read the saved index data.  if it couldn't be read, then re-calculate it.
		if (!readIndexData())
		{
			amis::util::Log::Instance()->writeWarning("Could not read index file", "Dtb::processNcc");
			resolve_smil_visitor.resolve(mpNavModel, mpSpine, true);
			this->mpTextSmilMap = resolve_smil_visitor.getSmilTextMap();	
			if (mCacheIndex) saveIndexData();
		}
	}

	//wait until after the smil data is parsed to set the title audio (otherwise it's not available)
	if (p_title != NULL && p_title->getLabel() != NULL && p_title->getLabel()->hasAudio())
		mpTitle->addAudioClip(p_title->getLabel()->getAudio(0)->clone());

	return true;
}

//--------------------------------------------------
//process an NCX file
//--------------------------------------------------
bool amis::dtb::Dtb::processNcx(const ambulant::net::url* filepath, bool isLocal)
{
	amis::io::NcxFileReader ncx_file_reader;
	ncx_file_reader.setAreFilenamesLowercase(isLocal);
	if (!ncx_file_reader.readFromFile(filepath)) return false;

	mpNavModel = ncx_file_reader.getNavModel();
	mpCustomTests = ncx_file_reader.getCustomTests();
	
	mpTitle = ncx_file_reader.getTitle();
	mpAuthor = ncx_file_reader.getAuthor();
	 //note that this step takes a very long time if the book is large, because
	//it involves walking the nav model and also opening all the SMIL files
	amis::dtb::nav::ResolveSmilDataVisitor resolve_smil_visitor;
	
	if (mThreadYielder != 0) mThreadYielder->peekAndPump();

	resolve_smil_visitor.setThreadYielder(mThreadYielder);
	
	if (!indexExistsOnDisk())
	{
		resolve_smil_visitor.resolve(mpNavModel, mpSpine, false);
		this->mpTextSmilMap = resolve_smil_visitor.getSmilTextMap();
		if (mCacheIndex) saveIndexData();
	}
	else
	{
		//try to read the saved index data.  if it couldn't be read, then re-calculate it.
		if (!readIndexData())
		{
			amis::util::Log::Instance()->writeWarning("Could not read index file", "Dtb::processNcc");
			resolve_smil_visitor.resolve(mpNavModel, mpSpine, true);
			this->mpTextSmilMap = resolve_smil_visitor.getSmilTextMap();	
			if (mCacheIndex) saveIndexData();
		}
	}

	return true;
}

//--------------------------------------------------
//process an OPF file
//--------------------------------------------------
bool amis::dtb::Dtb::processOpf(const ambulant::net::url* filepath)
{
	#if defined(AMIS_COMPILER_MSVC)
		//windows utility for converting wstring to string
		USES_CONVERSION;
	#endif

	amis::io::OpfFileReader opf_file_reader;
	if (!opf_file_reader.readFromFile(filepath)) return false;

	mpSpine = opf_file_reader.getSpine();
	mpMetadata = opf_file_reader.getMetadata();
	
	//only support DAISY 2005 books
	amis::dtb::MetaItem* p_item = mpMetadata->getMetadata("dc:Format");
	std::string str_content = "";
	#if defined(AMIS_COMPILER_MSVC)
		str_content.assign(T2A(p_item->mContent.c_str()));
	#else
		//TODO: convert string on other platforms
	#endif
	if (p_item == NULL || str_content != "ANSI/NISO Z39.86-2005")
	{
		std::string msg = "This format is unsupported: ";
		if (p_item == NULL) msg += "dc:Format not found";
		else msg += str_content;
		amis::util::Log::Instance()->writeError(msg, "Dtb::processOpf");
		mDaisyVersion = UNSUPPORTED;
		return false;
	}
	mUid = getUid();
	
	/*mpTitle = new amis::MediaGroup();
	amis::TextNode* p_title_text = new amis::TextNode();
	p_title_text->setTextString(mpMetadata->getMetadataContent("dc:Title"));
	mpTitle->setText(p_title_text);

	mpAuthor = new amis::MediaGroup();
	amis::TextNode* p_author_text = new amis::TextNode();
	p_author_text->setTextString(mpMetadata->getMetadataContent("dc:Creator"));
	mpAuthor->setText(p_author_text);*/

	const ambulant::net::url* navfile = opf_file_reader.getNavFilename();
	const ambulant::net::url* resfile = opf_file_reader.getResourceFilename();
	const ambulant::net::url* txtfile = opf_file_reader.getTextFilename();
	mUid = getUid();
	
	mpFiles->setAdditionalDataAfterInitialParse(mUid, navfile, resfile, mpHistory);
	mpFiles->setTextFile(txtfile);
	return true;
}
//--------------------------------------------------
//process a resource file
//--------------------------------------------------
bool amis::dtb::Dtb::processDaisyResourceFile(const ambulant::net::url* filepath)
{
	return false;
}

//--------------------------------------------------
//get the UID for the currently open book
//--------------------------------------------------
wstring amis::dtb::Dtb::getUid()
{
	if (mpMetadata == NULL)
		return 0;

	wstring w_uid = mpMetadata->getMetadataContent("dc:Identifier");
	if (w_uid.size() == 0)
		w_uid = mpMetadata->getMetadataContent("dc:identifier");

	if (w_uid.size() == 0)
		w_uid = mpMetadata->getMetadataContent("ncc:identifier");

	return w_uid;
}


//--------------------------------------------------
//get the bookmark set for the currently open book
//--------------------------------------------------
amis::dtb::BookmarkSet* amis::dtb::Dtb::getBookmarks()
{
	return mpBookmarks;
}

//--------------------------------------------------
//load the bookmarks for this book
//--------------------------------------------------
void amis::dtb::Dtb::loadBookmarks(const ambulant::net::url* filepath)
{
	if (filepath == NULL || filepath->is_empty_path())
	{
		mpBookmarks = NULL;
		return;
	}

	amis::io::BookmarksFileIO bmk_file_io;
	if (!bmk_file_io.readFromFile(filepath))
		mpBookmarks = new amis::dtb::BookmarkSet();
	else
		mpBookmarks = bmk_file_io.getBookmarkSet();
}
//--------------------------------------------------
//process a smil file
//--------------------------------------------------
bool amis::dtb::Dtb::processSmil(const ambulant::net::url* filepath)
{
#ifdef WITH_EXTERNAL_SMIL_PLAYER
	return true;
#else
	amis::io::SmilFileReader smil_reader;
	if (!smil_reader.readFromFile(filepath)) return false;
	mpSmilTree = smil_reader.getSmilTree();
	mpSmilTree->setCustomTestSet(mpCustomTests);
	return true;
#endif
}
//--------------------------------------------------
//go to the next phrase
//--------------------------------------------------
amis::dtb::smil::SmilMediaGroup* amis::dtb::Dtb::nextPhrase()
{
#ifdef WITH_EXTERNAL_SMIL_PLAYER
	return NULL;
#else
	if (mpSmilTree == NULL) return NULL;

	amis::dtb::smil::SmilMediaGroup* p_smil_media = NULL;
	p_smil_media = mpSmilTree->nextPhrase();

	if (p_smil_media == NULL)
	{
		if (mpSmilTree->getError().getCode() == amis::AT_END)
		{
			const ambulant::net::url* p_file = NULL;
			p_file = mpSpine->getNextFile();
			if (p_file != NULL)
			{
				mpFiles->setSmilFile(p_file);
				if (processSmil(mpFiles->getSmilFile()))
				{
					p_smil_media = mpSmilTree->firstPhrase();
				}
			}
			else
			{
				//cout<<"** end of book **"<<endl;
			}
		}
	}
	return p_smil_media;
#endif
}
//--------------------------------------------------
//go to the previous phrase
//--------------------------------------------------
amis::dtb::smil::SmilMediaGroup* amis::dtb::Dtb::previousPhrase()
{
#ifdef WITH_EXTERNAL_SMIL_PLAYER
	return NULL;
#else
	if (mpSmilTree == NULL) return NULL;

	amis::dtb::smil::SmilMediaGroup* p_smil_media = NULL;
	p_smil_media = mpSmilTree->previousPhrase();

	if (p_smil_media == NULL)
	{
		if (mpSmilTree->getError().getCode() == amis::AT_BEGINNING)
		{
			const ambulant::net::url* p_file = NULL;
			p_file =  mpSpine->getPreviousFile();
			if (p_file != NULL)
			{
				mpFiles->setSmilFile(p_file);
				if (processSmil(mpFiles->getSmilFile()))
				{
					p_smil_media = mpSmilTree->lastPhrase();
				}
			}
		}
		else
		{
			//cout<<"** beginning of book **"<<endl;
		}
	}
	return p_smil_media;
#endif
}

//this function is only used when we aren't using an external smil player (e.g. ambulant)
#ifndef WITH_EXTERNAL_SMIL_PLAYER
void amis::dtb::Dtb::setNewLastmark(amis::dtb::smil::SmilMediaGroup* pMediaGroup)
{
	if (pMediaGroup == NULL)
		return;
	if (mpBookmarks == NULL)
		return;
	ambulant::net::url data_uri = calculateUriOfData(pMediaGroup);
	setNewLastmark(data_uri);
}
#endif
//this function does more than it seems .. it needs a better name
//it turns out that this method signature is friendlier for ambulant than the other setNewLastmark function
void amis::dtb::Dtb::setNewLastmark(ambulant::net::url positionUri)
{
	amis::util::Log::Instance()->writeTrace("start function", "Dtb::setNewLastmark");
	if (mpBookmarks == NULL) return;
	
	//if we couldn't decode a pdtb and are presumably playing the copyright notice, don't set a lastmark
	//it confuses amis later on if the pdtb is opened after the user loads the key
	if (isProtected() == true && mbCanDecodePdtb == false)
		return;
	
	amis::dtb::PositionData* p_pos_data = new amis::dtb::PositionData();
	p_pos_data->mUri = positionUri;

	if (mpNavModel != NULL)
	{
		if (mpNavModel->getNavMap() != NULL)
		{
			amis::dtb::nav::NavNode* p_node = NULL;
			std::string smil_file_name = amis::util::calculateRelativeFilename
				(&p_pos_data->mUri, this->getFileSet()->getNavFilepath());
			p_node = mpNavModel->getNodeForSmilId(smil_file_name, mpNavModel->getNavMap());
			if (p_node != NULL) 
				p_pos_data->mNcxRef = p_node->getId();
			
			mpCurrentNavNode = p_node;
			
			//the current nav node could be a page target or nav point, depending on which has the greater
			//play order value.  default to nav point.
 			if (mpNavModel->hasPages() == true)
			{
				p_node = mpNavModel->getNodeForSmilId(smil_file_name, mpNavModel->getPageList());
				if (p_node != NULL && mpCurrentNavNode != NULL &&
					mpCurrentNavNode->getPlayOrder() < p_node->getPlayOrder())
				{
					mpCurrentNavNode = p_node;
				}
				else if (p_node != NULL && mpCurrentNavNode == NULL)
				{
					mpCurrentNavNode = p_node;
				}
			}
		}
	}

	mpBookmarks->setLastmark(p_pos_data);
	amis::io::BookmarksFileIO bmk_file_io;
	bmk_file_io.writeToFile(mpFiles->getBookmarksFilepath(), mpBookmarks);

	amis::util::Log::Instance()->writeTrace("end function", "Dtb::setNewLastmark");
	
}

string amis::dtb::Dtb::searchFullText(wstring search, ambulant::net::url currentTextUrl, int dir)
{
	amis::io::TextSearch text_search;
	UrlList* p_text_files = mpFiles->getTextFiles();
	UrlList::iterator it;
	string result = "";
	mLastSearchString.assign(search);
	string file_name = amis::util::FilePathTools::getFileName(currentTextUrl.get_url());
	string elm_id = currentTextUrl.get_ref();
	mLastSearchedTextFile = file_name + "#" + elm_id;
	return searchFullTextRelative(dir);
}
string amis::dtb::Dtb::searchFullTextNext()
{
	return searchFullTextRelative(1);
}

string amis::dtb::Dtb::searchFullTextPrevious()
{
	return searchFullTextRelative(-1);
}
wstring amis::dtb::Dtb::getLastTextSearchString()
{
	return mLastSearchString;
}
string amis::dtb::Dtb::searchFullTextRelative(int dir)
{
	UrlList* p_text_files = mpFiles->getTextFiles();
	amis::io::TextSearch text_search;
	UrlList::iterator it;
	string result = "";
	string last_file_name = amis::util::FilePathTools::clearTarget(mLastSearchedTextFile);
	string last_id = amis::util::FilePathTools::getTarget(mLastSearchedTextFile);

	bool b_filefound = false;
	for (it = p_text_files->begin(); it != p_text_files->end(); ++it)
	{
		//start at the file we left off at last time
		if (b_filefound == false)
		{
			string it_filename = amis::util::FilePathTools::getFileName(it->get_url());
			if (last_file_name == it_filename) b_filefound = true;
		}
		if (b_filefound == true)
		{
			if (dir == 1)
				result = text_search.searchForTextForwardFromId(mLastSearchString, last_id, &(*it));
			else if (dir == -1)
				result = text_search.searchForTextBackwardsFromId(mLastSearchString, last_id, &(*it));

			if (result != "")
				break;
		}
	}

	//we should now have the text id of the item
	if (result != "")
	{
		mLastSearchedTextFile  = amis::util::FilePathTools::getFileName(it->get_path());
		mLastSearchedTextFile += "#" + result;
		mLastSearchResult = (*mpTextSmilMap)[mLastSearchedTextFile];
		//now we have just the filename#id, so we can compare it to our hash map
		return mLastSearchResult;
	}

	mLastSearchResult = "";
	return "";
}

//input: filename#target
amis::dtb::smil::SmilMediaGroup* amis::dtb::Dtb::loadSmilFromUrl(ambulant::net::url* filepath)
{
#ifdef WITH_EXTERNAL_SMIL_PLAYER
	return NULL;
#else
	assert(filepath);
	ambulant::net::url smilfile = filepath->join_to_base(*this->getFileSet()->getBookDirectory());

	if (!mpFiles->getSmilFile()->same_document(smilfile))
	{
		mpSpine->goToFile(filepath);
		mpFiles->setSmilFile(&smilfile);
		if (!processSmil(mpFiles->getSmilFile()))
			return NULL;
	}
	//else, the document is already loaded in the smil tree
	return mpSmilTree->goToId(filepath->get_ref());
#endif
}

ambulant::net::url amis::dtb::Dtb::calculateUriOfData(amis::dtb::smil::SmilMediaGroup* pData)
{
	assert(pData);

	std::string id = pData->getId();
	ambulant::net::url smil_filename = (*mpFiles->getSmilFile());
	const ambulant::net::url* book_dir = mpFiles->getBookDirectory();
	
	ambulant::net::url fullpath = smil_filename.join_to_base(*book_dir);
	fullpath = fullpath.add_fragment(id);
	return fullpath;
}

amis::dtb::DaisyVersion amis::dtb::Dtb::getDaisyVersion()
{
	return mDaisyVersion;
}

//add a bookmark at the current position
//use the supplied note if one is given
amis::dtb::Bookmark* amis::dtb::Dtb::addBookmark(amis::MediaGroup* pNote)
{
	if (mpBookmarks == NULL) return NULL;

	amis::dtb::Bookmark* p_bmk = new amis::dtb::Bookmark();
	amis::dtb::PositionData* p_pos = mpBookmarks->getLastmark()->copy();
	
	amis::util::Log::Instance()->writeMessage("Adding bookmark", &p_pos->mUri, 
		"Dtb::addBookmark");

	p_bmk->mType = amis::dtb::Bookmark::BOOKMARK;
	p_bmk->mpStart = p_pos;
	p_bmk->mpNote = pNote;
	if (pNote != NULL) p_bmk->mbHasNote = true;

	mpBookmarks->addBookmark(p_bmk);

	amis::io::BookmarksFileIO bmk_file_io;
	bmk_file_io.writeToFile(mpFiles->getBookmarksFilepath(), mpBookmarks);
	
	return p_bmk;
}

//set the function that deals with protected books
//in our case (amis-ambulant), the callback will register the key file with the decryption plugin
//I decided to do this instead of linking the plugin to AmisCore directly 
//the callback here will get invoked during processNcc
void amis::dtb::Dtb::setCallbackForPreprocessingBookKey(ProtectedBookKeyHandlerFunctionPtr pFunction)
{
	if (pFunction == NULL)
		amis::util::Log::Instance()->writeWarning("Setting NULL handler for protected books",
		"Dtb::setCallbackForPreprocessingBookKey");
	else
		amis::util::Log::Instance()->writeMessage("Setting handler for protected books", 
		"Dtb::setCallbackForPreprocessingBookKey");
	mpCallbackForPreprocessingBookKey = pFunction;
}


//this function looks at the metadata and decides if the book is a protected book
//if so, the book key and "real" ncc file path are saved
bool amis::dtb::Dtb::checkForCopyProtection(amis::dtb::Metadata* pMetadata)
{
#if defined(AMIS_COMPILER_MSVC)
	//windows utility for converting wstring to string
	USES_CONVERSION;
#endif
	if (!pMetadata) return false;

	//metadata uses wide strings
	std::wstring w_version = pMetadata->getMetadataContent("prod:pdtb-version");
	std::wstring w_key = pMetadata->getMetadataContent("prod:pdtb-bookKey");
	std::wstring w_ncc = pMetadata->getMetadataContent("prod:pdtb-nccFile");

#if defined(AMIS_COMPILER_MSVC)
	std::string s_version = T2A(w_version.c_str());
	std::string s_key = T2A(w_key.c_str());
	std::string s_ncc = T2A(w_ncc.c_str());
#else
	//TODO convert the strings on other platforms
#endif

	if (s_version == "1.0.0" && s_key != "" && s_ncc != "")
	{
		ambulant::net::url keyfile = ambulant::net::url::from_filename(s_key);
		ambulant::net::url ncc = ambulant::net::url::from_filename(s_ncc);
		getFileSet()->setProtectedBookKeyFilepath(&keyfile.join_to_base(*getFileSet()->getBookDirectory()));
		getFileSet()->setProtectedNavFilepath(&ncc.join_to_base(*getFileSet()->getBookDirectory()));
		return true;
	}
	else return false;
}

//just in case this info is useful to anyone
bool amis::dtb::Dtb::isProtected()
{
	return mbIsProtected;
}

amis::dtb::nav::NavNode* amis::dtb::Dtb::getCurrentNavNode()
{
	return mpCurrentNavNode;
}

//does this book contain text files (main body content; not the ncc/ncx)
bool amis::dtb::Dtb::hasText()
{
	if (mpFiles->getTextFiles()->size() > 0)
		return true;
	else
		return false;
}


//save the smil id - nav node data
//(one id, many nav nodes)
void amis::dtb::Dtb::saveIndexData()
{	
#ifdef AMIS_COMPILER_MSVC
	USES_CONVERSION;
#else
	//TODO: something for other platforms
#endif
	//calculate this filepath from the bookmark filepath - it's unique
	string filepath = this->getFileSet()->getBookmarksFilepath()->get_url();
	filepath = amis::util::FilePathTools::getAsLocalFilePath(filepath);
	filepath.replace(filepath.find(".bmk"), 4, ".idx");
	string temp_filepath = filepath + ".tmp";

	ofstream f;
	f.open(temp_filepath.c_str(), ios::out);
	
	amis::dtb::nav::NodeRefMap* p_map = this->getNavModel()->getSmilIdNodeMap();
	
	amis::dtb::nav::NodeRefMap::iterator it;
	for (it = p_map->begin(); it != p_map->end(); it++)
	{
		f<<it->first<<endl;
		
		amis::dtb::nav::NavNodeList* p_list = it->second;
		for (int i=0; i<p_list->size(); i++)
		{
			amis::dtb::nav::NavNode* p_n = (*p_list)[i];
			f<<p_n->getId()<<endl;
		}
		f<<"*"<<endl;
	}
	f<<"**"<<endl;

	amis::StringMap::iterator it2;
	for (it2=mpTextSmilMap->begin(); it2 != mpTextSmilMap->end(); it2++)
	{
		f<<it2->first<<endl;
		f<<it2->second<<endl;
	}
	f.close();
	amis::util::Log::Instance()->writeTrace("created " + temp_filepath);
	//now compress the file
#ifdef AMIS_COMPILER_MSVC
	string exe = mAppPath + "lzop.exe";
	string params = "-o \"" + filepath + "\" \"" + temp_filepath + "\"";
	
	SHELLEXECUTEINFO sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.nShow = SW_HIDE;
    sei.lpVerb = _T("open");
    sei.lpFile = A2T(exe.c_str());
	sei.lpParameters = A2T(params.c_str());
	ShellExecuteEx(&sei);
	::WaitForSingleObject(sei.hProcess, INFINITE);
	::CloseHandle(sei.hProcess);

	if (ShellExecuteEx(&sei) == FALSE)
		amis::util::Log::Instance()->writeTrace("error compressing " + temp_filepath);
	else
		amis::util::Log::Instance()->writeTrace("compressed " + filepath);

	remove(temp_filepath.c_str());
#else
	//TODO: something for other platforms
#endif
}

bool amis::dtb::Dtb::readIndexData()
{
#ifdef AMIS_COMPILER_MSVC
	USES_CONVERSION;
#else
	//TODO: something on other platforms
#endif
	//calculate this filepath from the bookmark filepath - it's unique
	string filepath = this->getFileSet()->getBookmarksFilepath()->get_url();
	filepath = amis::util::FilePathTools::getAsLocalFilePath(filepath);
	filepath.replace(filepath.find(".bmk"), 4, ".idx");
	string temp_filepath = filepath + ".tmp";

	//now decompress the file
#ifdef AMIS_COMPILER_MSVC
	string exe = mAppPath + "lzop.exe";
	string params = "-d -o \"" + temp_filepath + "\" \"" + filepath + "\"";
	
	SHELLEXECUTEINFO sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.nShow = SW_HIDE;
    sei.lpVerb = _T("open");
    sei.lpFile = A2T(exe.c_str());
	sei.lpParameters = A2T(params.c_str());
	ShellExecuteEx(&sei);
	::WaitForSingleObject(sei.hProcess, INFINITE);
	::CloseHandle(sei.hProcess);
#else
	//TODO: something for other platforms
#endif
	amis::dtb::nav::NodeRefMap* p_smil_node_map = new amis::dtb::nav::NodeRefMap;
	amis::StringMap* p_text_smil_map = new amis::StringMap;
	//1 = smil_node, 2 = text_smil
	int which_map = 1;

	ifstream f;
	f.open(temp_filepath.c_str(), ios::in);
	//if the IDX file couldn't be read, return false
	if (f.fail()) return false;
	bool first_in_set = true;
	string map_key = "";
	amis::dtb::nav::NavNodeList* p_curr_node_list = NULL;

	while (!f.eof())
	{
		string s = "";
		getline(f, s);
		if (s == "") continue;
		//the delimiter for the two sections (one section per map)
		if (s == "**")
		{
			which_map = 2;
			continue;
		}
		//processing smil strings and nodes
		if (which_map == 1)
		{	
			//if this is not the delimiter between data entries
			if (s != "*")
			{
				if (first_in_set == true)
				{
					map_key = s;
					p_curr_node_list = new amis::dtb::nav::NavNodeList();
					first_in_set = false;
				}
				else
				{
					amis::dtb::nav::NavNode* p_n = mpNavModel->getNavNode(s);
					//amis::dtb::nav::NavNode* p_n = NULL;
					if (p_n != NULL) 
					{
						p_curr_node_list->push_back(p_n);
					}
				}
				continue;
			}
			else
			{
				(*p_smil_node_map)[map_key] = p_curr_node_list;
				first_in_set = true;
				continue;
			}
			

		}
		//processing text ids and smil strings
		else
		{
			//read in another line
			string s2 = "";
			getline(f, s2);
			if (s2 != "") (*p_text_smil_map)[s] = s2;
		}
	}

	mpNavModel->setSmilIdNodeMap(p_smil_node_map);
	mpTextSmilMap = p_text_smil_map;

	f.close();

#ifdef AMIS_COMPILER_MSVC
	remove(temp_filepath.c_str());
#else
	//TODO: something on other platforms.  Is "remove" a standard stdio function?
#endif
	return true;
}

bool amis::dtb::Dtb::indexExistsOnDisk()
{
	//calculate this filepath from the bookmark filepath - it's unique
	string filepath = this->getFileSet()->getBookmarksFilepath()->get_url();
	filepath = amis::util::FilePathTools::getAsLocalFilePath(filepath);
	filepath.replace(filepath.find(".bmk"), 4, ".idx");

	ifstream f;
	f.open(filepath.c_str(), ios::in);
	f.close();
	if (f.fail()) return false;
	else return true;
}

//turn on or off index caching
void amis::dtb::Dtb::setCacheIndex(bool shouldCache)
{
	mCacheIndex = shouldCache;
}