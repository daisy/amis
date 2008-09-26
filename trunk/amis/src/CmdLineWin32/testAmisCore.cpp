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
#include "BookList.h"
#include "io/TitleAuthorParse.h"

#include <tchar.h>
#include <iostream>

#include "ambulant/net/url.h"

#if _DEBUG
	#include <vld.h>
#endif


//main functions
void doInputLoop();
void nextPhrase();
void previousPhrase();
void addBookmark();
void searchText();
ambulant::net::url chooseBook();
void testTitleAuthorParse();
void readBook();

//helper functions
bool handleInput(string);
void output(amis::dtb::smil::SmilMediaGroup*);
void printContextualInfo(amis::dtb::smil::SmilMediaGroup*);
void printTitleAndAuthorInfo(const ambulant::net::url*);

//a global variable of the book we are reading
amis::dtb::Dtb* gBook;

int _tmain(int argc, _TCHAR* argv[])
{
	//readBook();
	testTitleAuthorParse();
	return 0;
}
void readBook()
{
	amis::util::Log::Instance()->startLog("c:\\devel\\amis\\trunk\\amis\\bin\\amisLog.txt");
	ambulant::net::url bookmarks_url = ambulant::net::url::from_filename("c:\\daisybooks\\bmk\\");
	
	gBook = new amis::dtb::Dtb();
	amis::BookList* p_recent_books = new amis::BookList();
	
	ambulant::net::url book_url = chooseBook();

	if (!gBook->open(&book_url, &bookmarks_url, p_recent_books))
	{
		cout<<"Error opening book"<<endl;
	}
	else
	{
		cout<<"Book opened"<<endl;
	}

	amis::dtb::smil::SmilMediaGroup* p_data = gBook->startReading(false);
	output(p_data);

	doInputLoop();
	delete gBook;
	delete p_recent_books;
	amis::util::Log::Instance()->endLog();
	amis::util::Log::Instance()->DestroyInstance();
}
ambulant::net::url chooseBook()
{
	//some books
	vector<ambulant::net::url> books;	
	books.push_back(ambulant::net::url::from_filename("C:\\daisybooks\\Mountains_skip\\ncc.html"));
	books.push_back(ambulant::net::url::from_filename("C:\\daisybooks\\voiceover_guide_zed\\06-SPEECHGEN.OPF"));
	books.push_back(ambulant::net::url::from_filename("C:\\daisybooks\\j-say-book2\\Ncc.html"));
	//present the list of books
	for (int i = 0; i<books.size(); i++)
		cout<<i+1<<". "<<books[i].get_file()<<endl;
	
	string choice;
	cout<<"Enter a number: ";
	cin>>choice;
	int idx = atoi(choice.c_str());
	return books[idx-1];
}
void doInputLoop()
{
	while (1)
	{
		string cmd;
		cout<<"Type (n)ext, (p)revious, (s)earch, (a)dd bookmark or (q)uit:";
		cin>>cmd;
		if (handleInput(cmd) == false) break;
	}
}
bool handleInput(string cmd)
{
	if (cmd == "n")
		nextPhrase();
	else if (cmd == "p")
		previousPhrase();
	else if (cmd == "s")
		searchText();
	else if (cmd == "a")
		addBookmark();
	else if (cmd == "q")
		return false;

	return true;
}
void nextPhrase()
{
	amis::dtb::smil::SmilMediaGroup* p_data = gBook->nextPhrase();
	output(p_data);
}
void previousPhrase()
{
	amis::dtb::smil::SmilMediaGroup* p_data = gBook->previousPhrase();
	output(p_data);
}
void addBookmark()
{
	amis::MediaGroup* p_note = new amis::MediaGroup();
	amis::TextNode* p_note_text = new amis::TextNode();
	wstring curr_text = L"test bookmark";
	p_note_text->setTextString(curr_text);
	p_note->setText(p_note_text);
	gBook->addBookmark(p_note);
}

void searchText()
{
	wstring search;
	cout<<"Enter text to search for: "<<endl;
	wcin>>search;

	string result = gBook->searchFullText(search, gBook->getBookmarks()->getLastmark()->mUri, 1);
	if (result == "")
	{
		cout<<"Not found!"<<endl;
		return;
	}

	amis::dtb::smil::SmilMediaGroup* p_data = gBook->loadSmilFromUrl(&ambulant::net::url::from_filename(result));
	output(p_data);

	while(1)
	{
		string cmd;
		cout<<"Search for same text again?  (f)orward, (r)everse, or (q)uit searching "<<endl;
		cin>>cmd;
		string result = "";
		if (cmd == "f")
			result = gBook->searchFullTextNext();
		else if (cmd == "r")
			result = gBook->searchFullTextPrevious();
		else if (cmd == "q")
			break;
		if (result != "")
		{
			amis::dtb::smil::SmilMediaGroup* p_data = gBook->loadSmilFromUrl(&ambulant::net::url::from_filename(result));
			output(p_data);
		}
		else
		{
			cout<<"Not found!"<<endl;
		}	
	}
}

void printContextualInfo(amis::dtb::smil::SmilMediaGroup* pData)
{
	if (gBook->getNavModel() != NULL)
	{
		amis::dtb::nav::NavModel* p_nav = gBook->getNavModel();
		amis::dtb::nav::NavContainer* p_container = NULL;
		amis::dtb::nav::NavNode* p_node = NULL;
		amis::dtb::DtbFileSet* p_files = gBook->getFileSet();
		const ambulant::net::url* smilfilepath = p_files->getSmilFile();
		string smilfile = smilfilepath->get_file();
		smilfile = amis::util::FilePathTools::getFileName(smilfile);

		if (p_nav->getNavMap() != NULL)
		{
			p_container = p_nav->getNavMap();
			string smilref = smilfile + "#" + pData->getId();
			p_node = p_nav->getNodeForSmilId(smilref, p_container);
			if (p_node != NULL)
				wcout<<L"Section: "<<p_node->getLabel()->getText()->getTextString()<<endl;
		}
	
		if (p_nav->getPageList() != NULL)
		{		
			p_container = p_nav->getPageList();
			string smilref = smilfile + "#" + pData->getId();
			p_node = p_nav->getNodeForSmilId(smilref, p_container);
			if (p_node != NULL)
				wcout<<L"Page: "<<p_node->getLabel()->getText()->getTextString()<<endl;
		}

		for (int i = 0; i<p_nav->getNumberOfNavLists(); i++)
		{
			string smilref = smilfile + "#" + pData->getId();
			p_node = p_nav->getNodeForSmilId(smilref, p_nav->getNavList(i));
			if (p_node != NULL)
				wcout<<L"Special item: "<<p_node->getLabel()->getText()->getTextString()<<endl;
		}
	}
}

void output(amis::dtb::smil::SmilMediaGroup* pData)
{
	if (pData != NULL)
	{
		pData->print();
		printContextualInfo(pData);
		delete pData;
	}
}

void testTitleAuthorParse()
{
	amis::util::Log::Instance()->enable(true);
	amis::util::Log::Instance()->setLevel(amis::util::FULL_LOGGING);
	amis::util::Log::Instance()->startLog("C:\\devel\\amis\\amisLogTestCore.log");

	ambulant::net::url url_202 = ambulant::net::url::from_filename("C:\\daisybooks\\Mountains_skip\\ncc.html");
	ambulant::net::url url_zed = ambulant::net::url::from_filename("C:\\daisybooks\\voiceover_guide_zed\\06-SPEECHGEN.OPF");
	
	printTitleAndAuthorInfo(&url_202);
	printTitleAndAuthorInfo(&url_zed);	

	amis::util::Log::Instance()->endLog();
	amis::util::Log::Instance()->DestroyInstance();
}

void printTitleAndAuthorInfo(const ambulant::net::url* book)
{
	amis::io::TitleAuthorParse parser;
	if (parser.readFromFile(book) == true)
	{
		amis::MediaGroup* p_data = NULL;
		p_data = parser.getAuthorInfo();
		if (p_data != NULL)
		{
			cout<<"Author: "<<endl;
			p_data->print();
			delete p_data;
		}
		p_data = parser.getTitleInfo();
		if (p_data != NULL)
		{
			cout<<"Title: "<<endl;
			p_data->print();
			delete p_data;
		}
	}
	else
	{
		cout<<"Could not parse file! "<<book->get_file()<<endl;
	}
}

#endif