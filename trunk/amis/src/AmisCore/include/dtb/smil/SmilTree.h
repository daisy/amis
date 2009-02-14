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


#ifndef SMILTREE_H
#define SMILTREE_H

//SYSTEM INCLUDES
#include <string>

//PROJECT INCLUDES
#include "Error.h"
#include "dtb/CustomTest.h"

#include "dtb/smil/SmilMediaGroup.h"
#include "dtb/smil/SmilEngineConstants.h"
#include "dtb/smil/Node.h"
#include "dtb/smil/SeqNode.h"
#include "dtb/smil/SmilVisitor.h"

using namespace std;

namespace amis
{
namespace dtb
{
namespace smil
{
//! The Smil Tree is a tree structure representing the contents of one SMIL file.
/*!
	The tree's root node is a seq node and the root's children are par, seq, audio,
	text, and image nodes.

   Playback of a Smil Tree object can begin at any node in the tree. Next and 
   Previous commands traverse items contained within the most local sequence, 
   relative to the current playback position.
*/
class SmilTree
{
public:
	SmilTree();
	~SmilTree();
	
	void setRoot(SeqNode*);
	Node* getRoot();

	void setSmilFilePath(string);
	string getSmilFilePath();

	string getCurrentId();
	void setCurrentId(string);

	string getRecentTextId();
	void setRecentTextId(string);

	bool getCouldEscape();
	void setCouldEscape(bool);

	void setCustomTestSet(CustomTestSet*);

	//!identify a node as being escapable
	void setPotentialEscapeNode(Node*);

	//!print the tree
	void print();

	//!go to the first group of parallel elements
	SmilMediaGroup* firstPhrase();
	//!go to the last group of parallel elements
	SmilMediaGroup* lastPhrase();
	//!go to the next group of parallel elements
	SmilMediaGroup* nextPhrase();
	//!go to the previous group of parallel elements
	SmilMediaGroup* previousPhrase();
	//!go to a specific id
	SmilMediaGroup* goToId(string);
	//!escape current structure
	SmilMediaGroup* escapeCurrentStructure();
	
	//!is the tree empty?
	bool isEmpty();
	//!is this node to be skipped or escaped?
	bool mustSkipOrEscapeNode(Node*);
	
	amis::Error getError();

	void acceptDepthFirst(SmilVisitor*);
	void acceptDepthFirstOnActivePath(SmilVisitor*);

private:
	//!print a node
	void printNode(Node*, int);
	//!check the status of the tree index, calculated from the seq nodes "active" child pointers
	bool isAtBeginning(Node*);
	bool isAtEnd(Node*);

	//!root of the tree
	SeqNode* mpRoot;
	//!current escape node if exists
	TimeContainerNode* mpEscapeNode;

	//!skippable options list
	CustomTestSet* mpSkipOptions;

	//!path to smil file
	string mFilepath;

	//!did the user request an escape?
	bool mbEscapeRequested;
	//!is the current set of parallel nodes escapable?
	bool mbCouldEscape;

	//!current node id
	string mCurrentId;
	//!recent text id
	string mRecentTextId;

	amis::Error mError;
};
}
}
}
#endif
