// testAmisCore.cpp : Defines the entry point for the console application.
//


#ifndef TESTAMISCORE
#define TESTAMISCORE

#include "dtb/Dtb.h"
#include "dtb/nav/NavNode.h"
#include "dtb/nav/NavModel.h"
#include "dtb/nav/PageList.h"
#include "dtb/nav/NavList.h"
#include "dtb/nav/NavMap.h"
#include "util/FilePathTools.h"
#include "util/Log.h"

#include <tchar.h>
#include <iostream>

#include "ambulant/net/url.h"

#if _DEBUG
	#include <vld.h>
#endif

#include "RecentBooks.h"
#endif

void doPhraseNavInputLoop(amis::dtb::Dtb*);
void nextPhrase(amis::dtb::Dtb*);
void previousPhrase(amis::dtb::Dtb*);
void addBookmark(amis::dtb::Dtb*);
void searchText(amis::dtb::Dtb*);
void printContextualInfo(amis::dtb::Dtb*, amis::dtb::smil::SmilMediaGroup*);
void addToRecentBooks(amis::dtb::Dtb*, amis::RecentBooks*);
void printPlayOrderList(amis::dtb::Dtb*);

int _tmain(int argc, _TCHAR* argv[])
{
	amis::util::Log::Instance()->startLog("c:\\devel\\amis\\branches\\amis3\\AmisCore\\testAmisCoreWin32\\amisLog.txt");
	amis::dtb::Dtb* book = new amis::dtb::Dtb();

	//ambulant::net::url book_url = ambulant::net::url::from_filename("c:\\daisybooks\\Mountains_skip\\ncc.html");
	ambulant::net::url book_url = ambulant::net::url::from_filename
		("C:\\daisybooks\\voiceover_guide_zed\\06-SPEECHGEN.OPF");
	ambulant::net::url bookmarks_url = ambulant::net::url::from_filename("c:\\daisybooks\\bmk\\");
	amis::RecentBooks* p_recent_books = new amis::RecentBooks();
	
	if (!book->open(&book_url, &bookmarks_url))
	{
		cout<<"Error opening book"<<endl;
		return 0;
	}
	else
	{
		cout<<"Book opened"<<endl;
		addToRecentBooks(book, p_recent_books);
		//printPlayOrderList(book);
	}

	amis::dtb::smil::SmilMediaGroup* p_data = book->startReading(false);

	if (p_data != NULL)
	{
		p_data->print();
		printContextualInfo(book, p_data);
		p_data->destroyContents();
		delete p_data;
	}

	doPhraseNavInputLoop(book);
	delete book;
	delete p_recent_books;
	amis::util::Log::Instance()->endLog();
	amis::util::Log::Instance()->DestroyInstance();
	return 0;
}
void addToRecentBooks(amis::dtb::Dtb* p_dtb, amis::RecentBooks* p_recent_books)
{
	if (p_recent_books != NULL)
	{
		amis::RecentBookEntry* p_entry = new amis::RecentBookEntry();
		p_entry->mbIsLastRead = true;
		p_entry->mBmkPath = *p_dtb->getFileSet()->getBookmarksFilepath();

		if (p_dtb->getDaisyVersion() == amis::dtb::DAISY_202)
			p_entry->mPath = *p_dtb->getFileSet()->getNavFilepath();
		else if (p_dtb->getDaisyVersion() == amis::dtb::DAISY_2005)
			p_entry->mPath = *p_dtb->getFileSet()->getOpfFilepath();

		p_recent_books->addEntry(p_entry);
	}
}

void doPhraseNavInputLoop(amis::dtb::Dtb* p_dtb)
{
	string cmd;

	while (1)
	{
		cout<<"Type (n)ext, (p)revious, (s)earch, (a)dd bookmark or (q)uit:";
		cin>>cmd;

		if (cmd == "n")
			nextPhrase(p_dtb);
		else if (cmd == "p")
			previousPhrase(p_dtb);
		else if (cmd == "s")
			searchText(p_dtb);
		else if (cmd == "a")
			addBookmark(p_dtb);
		else if (cmd == "q")
			break;
	}
}

void nextPhrase(amis::dtb::Dtb* p_dtb)
{
	amis::dtb::smil::SmilMediaGroup* p_data = p_dtb->nextPhrase();
	if (p_data != NULL)
	{
		p_data->print();
		printContextualInfo(p_dtb, p_data);
		delete p_data;
	}

}
void previousPhrase(amis::dtb::Dtb* p_dtb)
{
	amis::dtb::smil::SmilMediaGroup* p_data = p_dtb->previousPhrase();
	if (p_data != NULL)
	{
		p_data->print();
		printContextualInfo(p_dtb, p_data);
		delete p_data;
	}
}
void addBookmark(amis::dtb::Dtb* p_dtb)
{
	amis::MediaGroup* p_note = new amis::MediaGroup();
	amis::TextNode* p_note_text = new amis::TextNode();
	
	wstring curr_text = L"test bookmark";
	p_note_text->setTextString(curr_text);
	p_note->setText(p_note_text);

	//TODO: add an audio node (need to find out the information from Ambulant)
	///add the audio clip of the current position to the bookmark's note's audio field

	p_dtb->addBookmark(p_note);
}

void searchText(amis::dtb::Dtb* p_dtb)
{
	wstring search;
	cout<<"Enter text to search for: "<<endl;
	wcin>>search;

	string result = p_dtb->searchFullText(search);
	if (result == "")
	{
		cout<<"Not found!"<<endl;
		return;
	}

	amis::dtb::smil::SmilMediaGroup* p_data = p_dtb->loadSmilFromUrl(&ambulant::net::url::from_filename(result));
	if (p_data != NULL)
	{
		p_data->print();
		printContextualInfo(p_dtb, p_data);
		delete p_data;
	}

	string cmd;
	do
	{
		cout<<"Search for same text again?  (f)orward or (r)everse direction, or (q)uit"<<endl;
		cin>>cmd;
		string result = "";
		if (cmd == "f")
			result = p_dtb->searchFullTextNext();
		else if (cmd == "r")
			result = p_dtb->searchFullTextPrevious();

		if (result != "")
		{
			amis::dtb::smil::SmilMediaGroup* p_data = p_dtb->loadSmilFromUrl(&ambulant::net::url::from_filename(result));
			if (p_data != NULL)
			{
				p_data->print();
				printContextualInfo(p_dtb, p_data);
				delete p_data;
			}
		}
		else
			cout<<"Not found!"<<endl;
	
	}
	while (cmd != "q");
}

void printContextualInfo(amis::dtb::Dtb* pDtb, amis::dtb::smil::SmilMediaGroup* pData)
{
	if (pDtb->getNavModel() != NULL)
	{
		amis::dtb::nav::NavModel* p_nav = pDtb->getNavModel();
		amis::dtb::nav::NavContainer* p_container = NULL;
		amis::dtb::nav::NavNode* p_node = NULL;
		amis::dtb::DtbFileSet* p_files = pDtb->getFileSet();
		const ambulant::net::url* smilfilepath = p_files->getSmilFile();
		string smilfile = smilfilepath->get_file();
		smilfile = amis::util::FilePathTools::getFileName(smilfile);

		if (p_nav->getNavMap() != NULL)
		{
			p_container = p_nav->getNavMap();
			cout<<"Section: ";
			string smilref = smilfile + "#" + pData->getId();
			p_node = p_nav->getNodeForSmilId(smilref, p_container);
			if (p_node != NULL)
				wcout<<p_node->getLabel()->getText()->getTextString()<<endl;
			else
				cout<<"not found!"<<endl;
		}
	
		if (p_nav->getPageList() != NULL)
		{		
			p_container = p_nav->getPageList();
			cout<<"Page: ";
			string smilref = smilfile + "#" + pData->getId();
			p_node = p_nav->getNodeForSmilId(smilref, p_container);
			if (p_node != NULL)
				wcout<<p_node->getLabel()->getText()->getTextString()<<endl;
			else
				cout<<"not found!"<<endl;
		}

		for (int i = 0; i<p_nav->getNumberOfNavLists(); i++)
		{
			string smilref = smilfile + "#" + pData->getId();
			p_node = p_nav->getNodeForSmilId(smilref, p_nav->getNavList(i));
			cout<<"Special Item: ";
			if (p_node != NULL)
				wcout<<p_node->getLabel()->getText()->getTextString()<<endl;
			else
				cout<<"not found or not referenced"<<endl;
		}
	}
}

void printPlayOrderList(amis::dtb::Dtb* p_dtb)
{
	vector<amis::dtb::nav::NavNode*> list = p_dtb->getNavModel()->getPlayOrderList();

	char intstr[5];
	for (int i = 0; i<list.size(); i++)
	{
		string i_str = itoa(i, intstr, 5);
		string msg = "#" + i_str;
		msg += " = ";
		if (list[i] == NULL) msg += "NULL -- WARNING!";
		else msg += list[i]->getId(); 
		amis::util::Log::Instance()->writeMessage(msg);
	}
}