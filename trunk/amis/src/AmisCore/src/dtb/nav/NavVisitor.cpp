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
#include "dtb/nav/NavVisitor.h"
#include "dtb/nav/NavModel.h"
#include "io/QuickDataSmilFileReader.h"
#include "util/FilePathTools.h"
#include <iostream>
#include <fstream>

/**************************************/
/*BUILD SPINE VISITOR
/**************************************/
amis::dtb::Spine* amis::dtb::nav::BuildSpineVisitor::getSpine(amis::dtb::nav::NavModel* model, const ambulant::net::url* pBookDirectory)
{
	mpBookDirectory = pBookDirectory;
	mpSpine = new amis::dtb::Spine();
	model->acceptDepthFirst(this);
	return mpSpine;
}

//this gets called each time a node is visited
bool amis::dtb::nav::BuildSpineVisitor::preVisit(NavNode* n)
{
	if (n->getContent() == "") return true;

	//make a full path out of n->getContent()
	ambulant::net::url smil_url = ambulant::net::url::from_url(n->getContent());
	ambulant::net::url full_smil_url = smil_url.join_to_base(*mpBookDirectory);

	mpSpine->addFile(&full_smil_url, false);
	return true;
}


/**************************************/
/*RESOLVE SMIL DATA VISITOR          
/**************************************/
amis::StringMap* amis::dtb::nav::ResolveSmilDataVisitor::getSmilTextMap()
{
	return mpTextMap;
}

//this will match up file.smil#id lists to each NavPoint and PageTarget
//it will also index the text in the SMIL files (since it's parsing them already)
void amis::dtb::nav::ResolveSmilDataVisitor::resolve(amis::dtb::nav::NavModel* pModel, amis::dtb::Spine* pSpine, bool resolveNavNodeAudioData)
{
	if (pSpine == NULL)
		return;

	mpPreviousNavNode = NULL;
	//this is the map of NavNodes to smil ids (one id to many nodes)
	//an id in section 1.1 is in section 1.1 and not in section 1
	//an id in any section may also be referenced by a page node
	mpMap = new NodeRefMap;

	//a map of text ids and the smil files they occur in
	mpTextMap = new StringMap;

	unsigned int i = 0;
	//build a giant list of smil files and their element IDs
	for (int i = 0; i<pSpine->getNumberOfSmilFiles(); i++)
	{
		NavNodeList* p_nav_nodes = NULL;
		//find out which nav nodes refer to this smil file
		//then we'll send the list to the smil parser, and the nodes will
		//have their audio data filled in (this data isn't built into the NCC files)
		if (resolveNavNodeAudioData == true)
		{
			//who = which nav nodes
			WhoRefersToThisSmilFile who_visitor;
			p_nav_nodes = who_visitor.findOut(pModel, pSpine->getSmilFilePath(i));
		}
		const ambulant::net::url* smilfile = pSpine->getSmilFilePath(i);
		amis::io::QuickDataSmilFileReader quick_reader;
		//get all the ids and text src's from this smil file.  also fill in the audio data for the nav nodes.
		//big id list is a list of all SMIL element IDs
		//text map is a map of the smilfile name and the text src in it
		//p_nav_nodes is a list of pointers to nav nodes that refer to this smil file
		quick_reader.readFromFile(smilfile, &mBigIdList, mpTextMap, p_nav_nodes);

		//this deletes a pointer to a list of pointers, not the nodes themselves
		if (p_nav_nodes != NULL)
			delete p_nav_nodes;
	}

	
	//first, resolve the node-smil ranges for the nav map
	if (pModel->getNavMap() != NULL)
	{
		mpPreviousNavNode = NULL;
		pModel->getNavMap()->acceptDepthFirst(this);
		//grab data for the last one
		preVisit(NULL);
	}
	
	//then, resolve the node-smil ranges for the page list
	if (pModel->hasPages())
	{
		mpPreviousNavNode = NULL;
		pModel->getPageList()->acceptDepthFirst(this);
		//grab data for the last one
		preVisit(NULL);
	}
	//then, resolve for the nav lists
	for (i = 0; i<pModel->getNumberOfNavLists(); i++)
	{
		mpPreviousNavNode = NULL;
		pModel->getNavList(i)->acceptDepthFirst(this);
		//ranges aren't yet supported for nav targets, so there's no need to call preVisit(NULL)
	}

	pModel->setSmilIdNodeMap(mpMap);
	
}
//then assign each NavNode a list of file#id values
//a NavPoint's range is from its contentHref until the next NavPoint's contentHref
//a PageTarget's range is from its contentHref until the next PageTarget's contentHref
//what is hard to tell: ranges for nav list items (so for now just give them one file#id
//when we have dtbook support, this info can come from smilref attributes
bool amis::dtb::nav::ResolveSmilDataVisitor::preVisit(NavNode* pNode)
{
	//ranges aren't supported in nav targets yet
	if (pNode != NULL && pNode->getTypeOfNode() == NavNode::NAV_TARGET)
	{
		addToMap(pNode->getContent(), pNode);
	}
	else
	{
		if (mpPreviousNavNode != NULL)
		{
			string c1 = mpPreviousNavNode->getContent();
			string c2;
			if (pNode == NULL) c2 = "";
			else c2 = pNode->getContent();

			//this will be the range for the previous nav point
			StdStringList range;
			range = getRange(c1, c2);
			
			for (int i = 0; i<range.size(); i++)
			{
				addToMap(range[i], mpPreviousNavNode);
			}
		}
		mpPreviousNavNode = pNode;
	}

	return true;
}
amis::StdStringList amis::dtb::nav::ResolveSmilDataVisitor::getRange(string a, string b)
{
	StdStringList::iterator it_a;
	StdStringList::iterator it_b;
	bool b_found = false;
	
	//the starting point cannot be empty
	if (a == "")
	{
		StdStringList empty_list;
		return empty_list;
	}

	//look for the start of the range
	for (it_a = mBigIdList.begin(); it_a != mBigIdList.end(); it_a++)
	{
		if ((*it_a).compare(a) == 0)
		{
			//if the end-value is empty, then automatically go to the end of the list
			if (b == "")
			{
				it_b = mBigIdList.end();
				b_found = true;
				break;
			}
			else
			{
				//look for the end of the range
				for (it_b = it_a; it_b < mBigIdList.end(); it_b++)
				{
					if ((*it_b).compare(b) == 0)
					{
						b_found = true;
						break;
					}
				}
			}
		}
		if (b_found == true) break;
	}


	StdStringList return_value;
	if (b_found == true)
		return_value.assign(it_a, it_b);
	
	return return_value;
}

void amis::dtb::nav::ResolveSmilDataVisitor::addToMap(string key, amis::dtb::nav::NavNode* pNode)
{
	//the key might already exist
	if ((*mpMap)[key] != NULL)
	{
		NavNodeList* node_list = (*mpMap)[key];
		node_list->push_back(pNode);
	}
	//if it doesn't, then add it
	else
	{
		NavNodeList* node_list = new NavNodeList;
		node_list->push_back(pNode);
		(*mpMap)[key] = node_list;
	}
}

void amis::dtb::nav::ResolveSmilDataVisitor::printMap(bool toFile)
{
	// Save the original stdout buffer.
	std::streambuf * sobuf = cout.rdbuf();
	std::ofstream fileout;
	if (toFile == true)
	{
		fileout.open("map_printout.txt");
		std::streambuf * filebuf = fileout.rdbuf();
		cout.rdbuf(filebuf);
	}

	if (mBigIdList.size() == (*mpMap).size())
		cout<<"Sizes are equal"<<endl;
	else
		cout<<"ERROR! Sizes ARE NOT EQUAL!"<<endl;

	for (int i = 0; i<mBigIdList.size(); i++)
	{
		cout<<"Key: "<<mBigIdList[i]<<":"<<endl;
		NavNodeList* node_list = (*mpMap)[mBigIdList[i]];
		if (node_list == NULL) continue;
		for (int j = 0; j<node_list->size(); j++)
		{
			cout<<"\t";
			if ((*node_list)[j]->getTypeOfNode() == NavNode::NAV_POINT)
				cout<<"Nav Point ";
			else if ((*node_list)[j]->getTypeOfNode() == NavNode::PAGE_TARGET)
				cout<<"Page Target ";
			else if ((*node_list)[j]->getTypeOfNode() == NavNode::NAV_TARGET)
				cout<<"Nav Target ";

			cout<<(*node_list)[j]->getId()<<endl;
		}
	}

	if (toFile == true)
	{
		fileout.close();
		cout.rdbuf(sobuf);
	}
		
}

/***********************************************/
/*Which nav nodes refer to this smil file?
/***********************************************/
amis::dtb::nav::NavNodeList* amis::dtb::nav::WhoRefersToThisSmilFile::findOut(amis::dtb::nav::NavModel* pModel, const ambulant::net::url* filename)
{
	if (mpNodes != NULL)
		delete mpNodes;
	
	mpNodes = new NavNodeList;
	mFilename = *filename;
	pModel->acceptDepthFirst(this);

	return mpNodes;
}

bool amis::dtb::nav::WhoRefersToThisSmilFile::preVisit(NavNode* pNode)
{
	ambulant::net::url content = ambulant::net::url::from_url(pNode->getContent());
	if (mFilename.same_document(content))
	{
		mpNodes->push_back(pNode);
	}
	return true;
}
/************************************************/
/* visit the node with class="title"*/
/************************************************/
amis::MediaGroup* amis::dtb::nav::VisitTheTitle::getTheTitle(NavModel* pModel)
{
	if (pModel == NULL) return NULL;
	if (pModel->getNavMap() == NULL) return NULL;

	mpTitle = NULL;
	pModel->getNavMap()->acceptDepthFirst(this);

	return mpTitle;
}

bool amis::dtb::nav::VisitTheTitle::preVisit(NavNode* pNode)
{
	if (pNode->getClass() == "title")
	{
		mpTitle = new amis::MediaGroup();
		if (pNode->getLabel()->hasText() == true)
			mpTitle->setText((amis::TextNode*)pNode->getLabel()->getText()->clone());
		if (pNode->getLabel()->getNumberOfAudioClips() > 0)
			mpTitle->addAudioClip((amis::AudioNode*)pNode->getLabel()->getAudio(0)->clone());

		return false;
	}
	else
	{
		return true;
	}
}
