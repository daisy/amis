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

#include "ambulant/common/plugin_engine.h"

//constructor
amis::dtb::Dtb::Dtb()
{
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
	ambulant::net::url fileUrl = ambulant::net::url::from_url(filepath);
	ambulant::net::url bookmarksDirUrl = ambulant::net::url::from_url(bookmarksDirectory);
	return open(&fileUrl, &bookmarksDirUrl, history);
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
	amis::util::Log::Instance()->writeMessage(log_msg, "Dtb::open", "AmisCore"); 

	mpFiles = NULL;
	mpFiles = new amis::dtb::DtbFileSet();
	mpFiles->initWithNccOrOpf(fileUrl, mpFileSearcher);
	mpFiles->setBookmarksDirectory(bookmarksDirectory);
	
	mpCurrentNavNode = NULL;
	if (DtbFileSet::isNccFile(fileUrl))
	{
		amis::util::Log::Instance()->writeMessage("This is a DAISY 2.02 book", "Dtb::open", "AmisCore");	
		mDaisyVersion = DAISY_202;
		if (!processNcc(mpFiles->getNavFilepath())) 
		{
			return false;
		}
	}
	else if (DtbFileSet::isOpfFile(fileUrl))
	{
		if (!processOpf(mpFiles->getOpfFilepath())) 
		{
			amis::util::Log::Instance()->writeMessage("xx process opf failed");
			return false;
		}
		mDaisyVersion = DAISY_2005;
		amis::util::Log::Instance()->writeMessage("This is a Daisy 2005 book", "Dtb::open", "AmisCore");
		if (!processNcx(mpFiles->getNavFilepath())) return false;
		//if (!processDaisyResourceFile(mpFiles->getResourceFilepath())) return false;
	}
	else
	{ 
		mDaisyVersion = UNSUPPORTED;
		amis::util::Log::Instance()->writeWarning("The type of book could not be determined", 
			"Dtb::open", "AmisCore");
		return false;
	}
	loadBookmarks(mpFiles->getBookmarksFilepath());
	amis::util::Log::Instance()->writeMessage("Opened book successfully", "Dtb::open", "AmisCore");
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
bool amis::dtb::Dtb::processNcc(const ambulant::net::url* filepath)
{
	amis::io::NccFileReader ncc_file_reader;
	amis::dtb::nav::BuildSpineVisitor spine_visitor;

	if (!ncc_file_reader.readFromFile(filepath)) 
	{
		amis::util::Log::Instance()->writeError("Could not read NCC file!", "Dtb::processNcc", "AmisCore");
		return false;
	}
	mpNavModel = ncc_file_reader.getNavModel();
	mpMetadata = ncc_file_reader.getMetadata();
	mpCustomTests = ncc_file_reader.getCustomTests();

	//if this NCC file turned out to be a protected book, then we need to get custom tests and a nav model
	//from an encrypted file
	if (checkForCopyProtection(mpMetadata))
	{
		amis::util::Log::Instance()->writeMessage("This is a protected book", "Dtb::processNcc", "AmisCore");
		mbIsProtected = true;
		if (mpCallbackForPreprocessingBookKey != NULL) 
		{
			if (mpCallbackForPreprocessingBookKey(
				getFileSet()->getProtectedNavFilepath(),
				getFileSet()->getProtectedBookKeyFilepath()
				) == true)
			{
				//reparse the ncc.pdtb file
				if (!ncc_file_reader.readFromFile(getFileSet()->getProtectedNavFilepath())) return false;

				if (mpNavModel) delete mpNavModel;
				if (mpCustomTests) delete mpCustomTests;
				mpNavModel = ncc_file_reader.getNavModel();
				mpCustomTests = ncc_file_reader.getCustomTests();
			}
		}
		else
		{
			/* if: 1. no function registered to handle protected books or
					2. that function failed
			then: playback of NCC.html continues and it will say "this book is protected... "
			*/
			amis::util::Log::Instance()->writeError("Protected book could not be read", 
				"Dtb::processNcc", "AmisCore");
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

	//TODO: get the audio for the title (author audio not available in Daisy 202)

	mpFiles->setAdditionalDataAfterInitialParse(mUid, NULL, NULL, mpHistory);
	amis::dtb::nav::ResolveSmilDataVisitor resolve_smil_visitor;
	resolve_smil_visitor.resolve(mpNavModel, mpSpine, true);
	this->mpTextSmilMap = resolve_smil_visitor.getSmilTextMap();

	return true;
}

//--------------------------------------------------
//process an NCX file
//--------------------------------------------------
bool amis::dtb::Dtb::processNcx(const ambulant::net::url* filepath)
{
	amis::io::NcxFileReader ncx_file_reader;
	if (!ncx_file_reader.readFromFile(filepath)) return false;

	mpNavModel = ncx_file_reader.getNavModel();
	mpCustomTests = ncx_file_reader.getCustomTests();

	amis::dtb::nav::ResolveSmilDataVisitor resolve_smil_visitor;

	//note that this step takes a very long time if the book is large, because
	//it involves walking the nav model and also opening all the SMIL files
	resolve_smil_visitor.resolve(mpNavModel, mpSpine, false);
	this->mpTextSmilMap = resolve_smil_visitor.getSmilTextMap();
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
	if (0) // (p_item == NULL || str_content != "ANSI/NISO Z39.86-2005")
	{
		std::string msg = "This format is unsupported: ";
		if (p_item == NULL) msg += "dc:Format not found";
		else msg += str_content;
		amis::util::Log::Instance()->writeError(msg, "Dtb::processOpf", "AmisCore");
		mDaisyVersion = UNSUPPORTED;
		return false;
	}
	//fill in the text for the title and the author
	mUid = getUid();
	mpTitle = new amis::MediaGroup();
	amis::TextNode* p_title_text = new amis::TextNode();
	p_title_text->setTextString(mpMetadata->getMetadataContent("dc:Title"));
	mpTitle->setText(p_title_text);

	mpAuthor = new amis::MediaGroup();
	amis::TextNode* p_author_text = new amis::TextNode();
	p_author_text->setTextString(mpMetadata->getMetadataContent("dc:Creator"));
	mpAuthor->setText(p_author_text);

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
//--------------------------------------------------
//set the lastmark
//--------------------------------------------------
void amis::dtb::Dtb::setNewLastmark(amis::dtb::smil::SmilMediaGroup* pMediaGroup)
{
	if (pMediaGroup == NULL)
		return;
	if (mpBookmarks == NULL)
		return;

	ambulant::net::url data_uri = calculateUriOfData(pMediaGroup);
	setNewLastmark(data_uri);
}

//this function does more than it seems .. it needs a better name
//it turns out that this method signature is friendlier for ambulant than the other setNewLastmark function
void amis::dtb::Dtb::setNewLastmark(ambulant::net::url positionUri)
{
	if (mpBookmarks == NULL) return;

	amis::dtb::PositionData* p_pos_data = new amis::dtb::PositionData();
	p_pos_data->mUri = positionUri;

	if (mpNavModel != NULL)
	{
		if (mpNavModel->getNavMap() != NULL)
		{
			amis::dtb::nav::NavNode* p_node = NULL;
			std::string uri_ref = p_pos_data->mUri.get_ref();
			//TODO: make this work for arbitrarily nested SMIL files (don't assume ncx and smil in same dir)
			std::string smil_file_name = amis::util::FilePathTools::getFileName(p_pos_data->mUri.get_url());
			smil_file_name += "#" + uri_ref;
			p_node = mpNavModel->getNodeForSmilId(smil_file_name, mpNavModel->getNavMap());
			if (p_node != NULL) 
			{
				mpCurrentNavNode = p_node;
				p_pos_data->mNcxRef = p_node->getId();
			}
			//the current nav node could be a page target or nav point, depending on which has the greater
			//play order value.  default to nav point.
 			if (mpNavModel->hasPages() == true)
			{
				p_node = mpNavModel->getNodeForSmilId(smil_file_name, mpNavModel->getPageList());
				if (p_node != NULL && mpCurrentNavNode->getPlayOrder() < p_node->getPlayOrder())
				{
					mpCurrentNavNode = p_node;
				}
			}
		}
	}

	mpBookmarks->setLastmark(p_pos_data);
	amis::io::BookmarksFileIO bmk_file_io;
	bmk_file_io.writeToFile(mpFiles->getBookmarksFilepath(), mpBookmarks);
}

string amis::dtb::Dtb::searchFullText(wstring search)
{
	amis::io::TextSearch text_search;
	UrlList* p_text_files = mpFiles->getTextFiles();
	UrlList::iterator it;
	string result = "";
	mLastSearchString.assign(search);

	for (it = p_text_files->begin(); it != p_text_files->end(); ++it)
	{
		result = text_search.searchForText(search, &(*it));
		if (result != "")
			break;
	}

	//we should now have the text id of the item
	if (result != "")
	{
		mLastSearchedTextFile  = amis::util::FilePathTools::getFileName(it->get_path());
		mLastSearchedTextFile += "#" + result;
		mLastSearchResult = (*mpTextSmilMap)[mLastSearchedTextFile.c_str()];
		//now we have just the filename#id, so we can compare it to our hash map
		return mLastSearchResult;
	}

	mLastSearchResult = "";
	return "";
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

string amis::dtb::Dtb::searchFullTextRelative(int dir)
{
	amis::io::TextSearch text_search;
	UrlList* p_text_files = mpFiles->getTextFiles();
	UrlList::iterator it;
	string result = "";
	string last_file_name = amis::util::FilePathTools::clearTarget(mLastSearchedTextFile );
	string last_id = amis::util::FilePathTools::getTarget(mLastSearchedTextFile);

	bool b_filefound = false;
	for (it = p_text_files->begin(); it != p_text_files->end(); ++it)
	{
		//start at the file we left off at last time
		if (b_filefound == false)
		{
			string it_filename = amis::util::FilePathTools::getFileName(it->get_path());
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
		"Dtb::addBookmark", "AmisCore");

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
		"Dtb::setCallbackForPreprocessingBookKey", "AmisCore");
	else
		amis::util::Log::Instance()->writeMessage("Setting handler for protected books", 
		"Dtb::setCallbackForPreprocessingBookKey", "AmisCore");
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