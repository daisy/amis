/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

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

#ifndef CONTENTNODE_H
#define CONTENTNODE_H

//PROJECT INCLUDES
#include "Media.h"
#include "dtb/smil/Node.h"
#include "dtb/smil/SmilMediaGroup.h"
#include "dtb/smil/SmilVisitor.h"

//! ContentNode represents any media reference in a SMIL file
/*!
the media data itself is represented by an amis::MediaNode member variable.
at runtime, this variable created as a text, audio, or image node
*/
namespace amis
{
namespace dtb
{
namespace smil
{
class ContentNode : public Node
{
public:
	ContentNode();
	~ContentNode();

	NodeType getCategoryOfNode();
	NodeType getTypeOfNode();

	//!create the media node as a new audio node
	void createNewAudio();
	//!create the media node as a new image node
	void createNewImage();
	//!create the media node as a new text node
	void createNewText();
	//!play this node
	void play(SmilMediaGroup*);
	void acceptDepthFirst(SmilVisitor*);
	void acceptDepthFirstOnActivePath(SmilVisitor*);

	//!return the media node, which contains all data required for rendering
	amis::MediaNode* getMediaNode();
	
private:
	//!media node data
	amis::MediaNode* mpMediaData;
	//!have we allocated mpMediaData as a specific type of node?
	bool mbAllocatedMediaNode;
	//!node type (content node in this case)
	NodeType mNodeType;
};
}
}
}
#endif
