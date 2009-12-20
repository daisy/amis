//NOTE from marisa 23 june 07: I don't think this file is used anymore
/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amis.sf.net

Copyright (C) 2004-2006  DAISY for All Project

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

//SYSTEM INCLUDES
#include <string>
#include <iostream>
#include <cctype>
#include <algorithm>

//PROJECT INCLUDES
#include "util/FilePathTools.h"

#include "Error.h"
#include "Media.h"

#include "SmilEngine.h"

//For making windows DLLs
#include <windows.h>


//--------------------------------------------------
//! This function is for making windows DLLs
//--------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


using namespace std;


SmilEngine* SmilEngine::pinstance = 0;

SmilEngine* SmilEngine::Instance()
{
	if (pinstance == 0)  // is it the first call?
    {  
      pinstance = new SmilEngine; // create sole instance
    }
    return pinstance; // address of sole instance
}

void SmilEngine::DestroyInstance()
{
	delete pinstance;
}

//--------------------------------------------------
//Default constructor
//--------------------------------------------------
SmilEngine::SmilEngine()
{
	//initialize member variables

	//pointers are NULL
	mpSpine = NULL;
	mpSmilTree = NULL;

	//:TEMP:
//	mpSearchText = NULL;

	//Daisy version is undefined (no book loaded right now)
	mDaisyVersion = NO_VERSION_SET;

	//we are not at the end of a Smil Tree
	mbEndOfTree = false;
	mbLoadId = false;

	mSpineBuildStatus = amis::NOT_INITIALIZED;
	mSmilTreeBuildStatus = amis::NOT_INITIALIZED;

}

//--------------------------------------------------
/*!
	the destructor closes the book
*/
//--------------------------------------------------
SmilEngine::~SmilEngine()
{
	closeBook();
	mBookFile = "";
	mIdTarget = "";
	mLastPosition = "";

}

//--------------------------------------------------
/*!
	open a book from a full path to: "ncc.html", "master.smil", or "*.opf"
*/
//--------------------------------------------------
amis::AmisError SmilEngine::openBook(string filePath, SmilMediaGroup* pMedia)
{

	amis::AmisError err;
	string file_ext = amis::FilePathTools::getExtension(filePath);
	string smilpath;

	//reset
	mBookFile = "";
	mIdTarget = "";
	mLastPosition = "";

	//convert the string to lower case before doing a comparison
	 std::transform(file_ext.begin(), file_ext.end(), 
		 file_ext.begin(), (int(*)(int))tolower);
	
	if (file_ext.compare(EXT_OPF) == 0)
	{
		mDaisyVersion = DAISY3;
	}
	else
	{
		mDaisyVersion = DAISY202;
	}

	//prepare for a new book to be loaded
	closeBook();

	//set the daisy version 
	mSmilTreeBuilder.setDaisyVersion(mDaisyVersion);

	//make a new spine for this book
	mpSpine = new Spine();
	
	//make a new text search object
	//:TEMP:
	//mpSearchText = new SearchDaisy();

	//give filepath to spine builder class and request that it builds the spine
	//the spine will be stored in our variable "mpSpine", which we initialized here
	err = mSpineBuilder.createSpine(mpSpine, filePath);

	mSpineBuildStatus = err.getCode();

	if (mSpineBuildStatus == amis::OK)
	{
		/*
		//initialize the text search with a list of smil files
		smilpath = FilePathTools::getAsLocalFilePath(mpSpine->getFirstFile());

		//:TEMP:
		//mpSearchText->addSmilPath(filepath);
	
		//this loop is for the search text module initialization
		while (mpSpine->getStatus()!=LIST_END)
		{
			smilpath = FilePathTools::getAsLocalFilePath(mpSpine->getNextFile());
			//:TEMP:
			//mpSearchText->addSmilPath(filepath);
		}

		*/

		//get the first file from the spine
		string smil_file = mpSpine->getFirstFile();

		//set end of tree to false
		mbEndOfTree = false;

		err = createTreeFromFile(smil_file, pMedia);
	
	}
	else
	{
		delete mpSpine;
		mpSpine = NULL;
		mSpineBuildStatus = amis::NOT_INITIALIZED;
	}

	return err;
	
}

//--------------------------------------------------
/*!
	this function deletes the spine and smil tree that a book comprises
	it also deletes the skip options
*/
//--------------------------------------------------
void SmilEngine::closeBook()
{
	if (mSpineBuildStatus != amis::NOT_INITIALIZED)
	{
		delete mpSpine;
		mpSpine = NULL;
	}

	if (mSmilTreeBuildStatus != amis::NOT_INITIALIZED)
	{
		delete mpSmilTree;
		mpSmilTree = NULL;
	}

	clearAllSkipOptions();

	//:TEMP:
	/*
	if (mpSearchText != NULL)
	{
		delete mpSearchText;
		mpSearchText = NULL;
	}
	*/

	mSpineBuildStatus = amis::NOT_INITIALIZED;
	mSmilTreeBuildStatus = amis::NOT_INITIALIZED;

}

/*!
	build a tree from file and go to the first, last, or specified (by id) node
*/
amis::AmisError SmilEngine::createTreeFromFile(string filepath, SmilMediaGroup* pMedia)
{
	//local variables
//	unsigned int i;
//	vector<amis::CustomTest*> tree_options;

	if (mpSmilTree != NULL)
	{
		delete mpSmilTree;
		mpSmilTree = new SmilTree();
	}

	//make a new smil tree for this book
	mpSmilTree = new SmilTree();

	//build a smil tree from the selected Smil file
	amis::AmisError err = mSmilTreeBuilder.createSmilTree(mpSmilTree, filepath);

	mSmilTreeBuildStatus = err.getCode();

	if (mSmilTreeBuildStatus != amis::OK)
	{
		return err;
	}
	else
	{	
		//refresh the smil tree with the list of global skip options
	
		//loop through all global skip options and update the Smil Tree with the user's preference
		
		//add each skip option to the smil tree
		//the skip options came from outside the smil engine
		mpSmilTree->setSkipOptionList(&mSkipOptions);

		//are we going to a specific element ID?
		if (mbLoadId == true)
		{
			mbLoadId = false;
			err = mpSmilTree->goToId(mIdTarget, pMedia);
		}

		else
		{
			//if we should NOT go to the end of the tree (if we are entering the tree from the top)
			if (mbEndOfTree == false)
			{
				err = mpSmilTree->goFirst(pMedia);
			}
			//else, if we should go to the end of the tree (if we are entering the tree from the end)
			else
			{
				err = mpSmilTree->goLast(pMedia);
			}
		}
		
		return err;

	}
}



//--------------------------------------------------
/*!
	go to the next node in the smil tree.
	if the next node is not found (for ex. if it is being skipped or we are at the
	end of the tree), load the next smil file and try again.
	repeat until found or end of book occurs

	@param[out] pMedia
		playback data delivered as a response to the request is stored here

	@param[in] pMedia
		points to an initialized object
*/
//--------------------------------------------------
amis::AmisError SmilEngine::next(SmilMediaGroup* pMedia)
{
	amis::AmisError err;

	if (mSpineBuildStatus != amis::OK ||
		mSmilTreeBuildStatus != amis::OK)
	{
		err.setCode(amis::NOT_INITIALIZED);
		err.setMessage(MSG_BOOK_NOT_OPEN);
	}
	else
	{
		err = mpSmilTree->goNext(pMedia);
		
		//need a loop here
		//case:
		//skippable option set to not render
		//entire file is skippable
		//when you enter that file, it comes back as LIST_END
		//need to go to the next file and try again
		while (err.getCode() == amis::AT_END)
		{
			//spine-next
			string smil_path;

			//get the next smil file in the spine
			smil_path = mpSpine->getNextFile();

			//if this getNextFile call was successful
			if (mpSpine->getStatus() != amis::AT_END)
			{
				mbEndOfTree = false;
				err = createTreeFromFile(smil_path, pMedia);
				recordPosition();
			}
			else
			{
				err.setCode(amis::AT_END);
				err.setMessage(MSG_END_OF_BOOK);
				break;
			}
		}
		if (err.getCode() == amis::OK)
		{
			//here we are sending out node playback data, so record our current position
			recordPosition();
		}

	}

	err.setSourceModuleName(amis::module_SmilEngine);
	return err;
}

//--------------------------------------------------
/*!
	go to the previous node in the smil tree.
	if the previous node is not found (for ex. if it is being skipped or we are 
	at the beginning of the tree), load the previous smil file and try again.
	repeat until found or beginning of book occurs

	@param[out] pMedia
		playback data delivered as a response to the request is stored here

	@param[in] pMedia
		points to an initialized object
*/
//--------------------------------------------------
amis::AmisError SmilEngine::previous(SmilMediaGroup* pMedia)
{
	amis::AmisError err;

	if (mSpineBuildStatus != amis::OK ||
		mSmilTreeBuildStatus != amis::OK)
	{
		err.setCode(amis::NOT_INITIALIZED);
		err.setMessage(MSG_BOOK_NOT_OPEN);
	}
	else
	{
		err = mpSmilTree->goPrevious(pMedia);

		while (err.getCode() == amis::AT_BEGINNING)
		{
			//spine-next
			string smil_path;

			//get the previous smil file in the spine
			smil_path = mpSpine->getPreviousFile();

			//if this getPreviousFile call was successful
			if (mpSpine->getStatus() != amis::AT_BEGINNING)
			{
				mbEndOfTree = true;
				err = createTreeFromFile(smil_path, pMedia);
				recordPosition();
			}
			else
			{
				err.setCode(amis::AT_BEGINNING);
				err.setMessage(MSG_BEGINNING_OF_BOOK);
				break;
			}
		}
		if (err.getCode() == amis::OK)
		{
			//here we are sending out node playback data, so record our current position
			recordPosition();
		}

	}

	err.setSourceModuleName(amis::module_SmilEngine);
	return err;
}

//--------------------------------------------------
/*!
	go to the first node in the loaded smil tree

	@param[out] pMedia
		playback data delivered as a response to the request is stored here

	@param[in] pMedia
		points to an initialized object
*/
//--------------------------------------------------
amis::AmisError SmilEngine::first(SmilMediaGroup* pMedia)
{
	amis::AmisError err;

	if (mSpineBuildStatus != amis::OK ||
		mSmilTreeBuildStatus != amis::OK)
	{
		err.setCode(amis::NOT_INITIALIZED);
		err.setMessage(MSG_BOOK_NOT_OPEN);
	}
	else
	{
		err = mpSmilTree->goFirst(pMedia);

		//here we are sending out node playback data, so record our current position
		recordPosition();
	}

	err.setSourceModuleName(amis::module_SmilEngine);
	
	return err;
}

//--------------------------------------------------
/*!
	go to the last node in the loaded smil tree

	@param[out] pMedia
		playback data delivered as a response to the request is stored here

	@param[in] pMedia
		points to an initialized object
*/
//--------------------------------------------------
amis::AmisError SmilEngine::last(SmilMediaGroup* pMedia)
{
	amis::AmisError err;

	if (mSpineBuildStatus != amis::OK ||
		mSmilTreeBuildStatus != amis::OK)
	{
		err.setCode(amis::NOT_INITIALIZED);
		err.setMessage(MSG_BOOK_NOT_OPEN);
	}
	else
	{
		err = mpSmilTree->goLast(pMedia);

		//here we are sending out node playback data, so record our current position
		recordPosition();
	}

	err.setSourceModuleName(amis::module_SmilEngine);
	
	return err;
}

//--------------------------------------------------
/*!
	print a list of skip options
*/
//--------------------------------------------------
void SmilEngine::printSkipOptions()
{
	unsigned int i;

	cout<<"Smil Engine Skip Options"<<endl;

	for (i=0; i<mSkipOptions.size(); i++)
	{
		cout<<"\tName = "<<mSkipOptions[i]->getId()<<endl;
		cout<<"\tWill play = "<<mSkipOptions[i]->getCurrentState()<<endl;
		cout<<"\tDefault = "<<mSkipOptions[i]->getDefaultState()<<endl;
		cout<<endl;
	}
}
//--------------------------------------------------
/*!
	@note
		call this function before opening a book
*/
//--------------------------------------------------
void SmilEngine::addSkipOption(amis::CustomTest* pOption)
{

	//create a new custom test object and copy the data over
	//local variables
	amis::CustomTest* p_skip_option;

	p_skip_option = new amis::CustomTest();

	p_skip_option->setId(pOption->getId());
	p_skip_option->setCurrentState(pOption->getCurrentState());
	p_skip_option->setBookStruct(pOption->getBookStruct());
	p_skip_option->setDefaultState(pOption->getDefaultState());
	p_skip_option->setOverride(pOption->getOverride());

	//add the new entry to the skipOptions array
	mSkipOptions.push_back (p_skip_option);

}

//--------------------------------------------------
/*!
	returns true if the option was changed, false if not found

	@param[in] id
		id of the custom test which is being changed

	@param[in] newState
		true = will play, false = will skip
*/
//--------------------------------------------------
bool SmilEngine::changeSkipOption(string id, bool newState)
{
	unsigned int i;
	bool b_found;

	b_found = false;
	//for-loop through the skip options array to see if this option already is present
	for (i = 0; i<mSkipOptions.size(); i++)
	{
		//if this option is present
		//if (mSkipOptions[i].mId.compare (option) == 0)
		if (mSkipOptions[i]->getId().compare(id) == 0)
		{
			//set its "willSkip" value equal to the user preference and break from the loop
			//mSkipOptions[i].mbWillSkip = value;
			mSkipOptions[i]->setCurrentState(newState);
			b_found = true;
			break;
		}
	}

	return b_found;
}

//--------------------------------------------------
/*!
	delete objects in the skip options collection
*/
//--------------------------------------------------
void SmilEngine::clearAllSkipOptions()
{
	//delete the skip option vector
	int sz = this->mSkipOptions.size();
	amis::CustomTest* tmp_ptr;

	for (int i=sz-1; i>0; i--)
	{
		tmp_ptr = this->mSkipOptions[i];

		delete tmp_ptr;

		mSkipOptions.pop_back();

	}

	mSkipOptions.clear();
}

//--------------------------------------------------
/*! 
	if the next node outside of the current escapable structure is not found 
	(for ex. if it is being skipped or we are 
	at the end of the tree), load the next smil file and try again.
	repeat until found or end of book occurs

	@param[out] pMedia
		playback data delivered as a response to the request is stored here

	@param[in] pMedia
		points to an initialized object
*/
//--------------------------------------------------
amis::AmisError SmilEngine::escapeCurrent(SmilMediaGroup* pMedia)
{
	amis::AmisError err;

	if (mSpineBuildStatus != amis::OK ||
		mSmilTreeBuildStatus != amis::OK)
	{
		err.setCode(amis::NOT_INITIALIZED);
		err.setMessage(MSG_BOOK_NOT_OPEN);
	}
	else
	{
		err = mpSmilTree->escapeStructure(pMedia);

		while (err.getCode() == amis::AT_END)
		{
			//spine-next
			string smil_path;

			//get the next smil file in the spine
			smil_path = mpSpine->getNextFile();

			//if this getNextFile call was successful
			if (mpSpine->getStatus() != amis::AT_END)
			{
				mbEndOfTree = false;
				err = createTreeFromFile(smil_path, pMedia);
				recordPosition();
			}
			else
			{
				err.setCode(amis::AT_END);
				err.setMessage(MSG_END_OF_BOOK);
				break;
			}
		}
		if (err.getCode() == amis::OK)
		{
			//here we are sending out node playback data, so record our current position
			recordPosition();
		}
	}

	err.setSourceModuleName(amis::module_SmilEngine);
	
	return err;
}

//--------------------------------------------------
/*!
	go to a specified position in the book.

	@param[out] pMedia
		playback data delivered as a response to the request is stored here

	@param[in] positionUri
		string of the position to load.  stated as a full path to the node, as in 
		c:\book\file.smil#target

	@param[in] pMedia
		points to an initialized object
*/
//--------------------------------------------------
amis::AmisError SmilEngine::loadPosition(string positionUri, SmilMediaGroup* pMedia)
{
	amis::AmisError err;
	string err_msg;
	err_msg = "*" + positionUri + "* was not found.";

	//the bookmark file
	string position = positionUri;

	//save the target
	mIdTarget = amis::FilePathTools::getTarget(positionUri);

	//clear the target from the file
	positionUri = amis::FilePathTools::clearTarget(positionUri);

	//is this bookmark file the same as the currently open smil file? 
	if (mpSmilTree->getSmilFilePath().compare(positionUri) == 0)
	{
		//go to the target Id in the current smil tree
		err = mpSmilTree->goToId(mIdTarget, pMedia);

		//if this new position isn't valid then restore the last one
		if (err.getCode() != amis::OK)
		{
			err = loadPosition(mLastPosition, pMedia);

			//there isn't a serious error but let the user know their request 
			//was invalid and that's why we didn't load it
			err.setMessage(err_msg);
		}
		else
		{
			recordPosition();
		}
	}

	//else, the filepaths are not the same, so we should build a new smil tree
	else if (mpSpine->goToFile(positionUri) == true)
	{
		//we are not at the end of the tree
		mbEndOfTree = false;
		mbLoadId = true;

		err = createTreeFromFile(positionUri, pMedia);

		//position not found is the likely error here
		//so load our last good position
		if (err.getCode() != amis::OK)
		{
			err = loadPosition(mLastPosition, pMedia);
			//there isn't a serious error but let the user know their request 
			//was invalid and that's why we didn't load it
			err.setMessage(err_msg);
		}
		else
		{
			recordPosition();
		}

	}

	//else, this Smil file is not found
	//don't even try to change position
	else
	{
		err.setCode(amis::NOT_FOUND);
		err.setMessage(err_msg);
	}

	err.setSourceModuleName(amis::module_SmilEngine);

	return err;

}

//--------------------------------------------------
//record our current position.
/*!
	@pre
		this position was just delivered as output data
*/
//--------------------------------------------------
void SmilEngine::recordPosition()
{
	//remember the last position so we can load it again if gotoId fails
	mLastPosition = mpSmilTree->getSmilFilePath();
	mLastPosition += "#" + mpSmilTree->getCurrentId();
}


//--------------------------------------------------
//print the tree
/*!
	this function is useful in debugging
*/
//--------------------------------------------------
void SmilEngine::printTree()
{
	if (mpSmilTree != NULL)
	{
		mpSmilTree->print();
	}
}

//--------------------------------------------------
//get the source path for the currently open smil file
//--------------------------------------------------
string SmilEngine::getSmilSourcePath()
{
	if (mpSmilTree != NULL)
	{
		return this->mpSmilTree->getSmilFilePath();
	}
	else
	{
		return "";
	}
}

//--------------------------------------------------
//get the number of smil files in the publication
//--------------------------------------------------
int SmilEngine::getNumberOfSmilFiles()
{
	return mpSpine->getNumberOfSmilFiles();
}

//--------------------------------------------------
//get the source path for a smil file (by index)
//--------------------------------------------------
string SmilEngine::getSmilFilePath(int idx)
{
	return mpSpine->getSmilFilePath(idx);
}

