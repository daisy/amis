// testAmisCoreLinux.cpp : Defines the entry point for the console application.
//

#include "dtb/Dtb.h"
#include "dtb/nav/NavNode.h"
#include "dtb/nav/NavModel.h"
#include "dtb/nav/PageList.h"
#include "dtb/nav/NavList.h"
#include "dtb/nav/NavMap.h"
#include "util/FilePathTools.h"
#include <iostream>

void doPhraseNavInputLoop(amis::dtb::Dtb*);
void nextPhrase(amis::dtb::Dtb*);
void previousPhrase(amis::dtb::Dtb*);
void searchText(amis::dtb::Dtb*);
void printContextualInfo(amis::dtb::Dtb*, amis::dtb::smil::SmilMediaGroup*);

int main(int argc, char* argv[])
{
	string home = getenv("HOME");
	string book_path = "/Users/marisa/daisybooks/Mountains_skip/ncc.html";
	string bmk_path = home + "/bmk";

	amis::dtb::Dtb* book = new amis::dtb::Dtb();
	if (!book->open(book_path, bmk_path))
		cout<<"Error opening book"<<endl;
	else
		cout<<"Book opened"<<endl;

	amis::dtb::smil::SmilMediaGroup* p_data = book->startReading(false);

	if (p_data != NULL)
	{
		p_data->print();
		printContextualInfo(book, p_data);
		delete p_data;
	}
	else
	{
		cout<<"smil data is NULL!"<<endl;
	}

	doPhraseNavInputLoop(book);
	delete book;
	return 0;
}

void doPhraseNavInputLoop(amis::dtb::Dtb* p_dtb)
{
	string cmd;

	while (1)
	{
		cout<<"Type (n)ext, (p)previous, (s)earch or (q)uit:";
		cin>>cmd;

		if (cmd == "n")
			nextPhrase(p_dtb);
		else if (cmd == "p")
			previousPhrase(p_dtb);
		else if (cmd == "s")
			searchText(p_dtb);
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

	//temporarily disabled amis::dtb::smil::SmilMediaGroup* p_data = p_dtb->loadSmilFromUrl(result);
/*	if (p_data != NULL)
	{
		p_data->print();
		printContextualInfo(p_dtb, p_data);
		delete p_data;
	}
*/
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
			//temporarily disabled amis::dtb::smil::SmilMediaGroup* p_data = p_dtb->loadSmilFromUrl(result);
/*			if (p_data != NULL)
			{
				p_data->print();
				printContextualInfo(p_dtb, p_data);
				delete p_data;
			}
*/		}
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
		string smilfile = p_files->getSmilFile()->get_url();
	//	smilfile = amis::util::FilePathTools::clearTarget(smilfile);
	//	smilfile = amis::util::FilePathTools::getFileName(smilfile);

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
	else
	{
		cout<<"Nav model is NULL!"<<endl;
	}
}
