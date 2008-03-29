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

//SYSTEM INCLUDES
#include <iostream>

//PROJECT INCLUDES
#include "dtb/smil/SmilEngineConstants.h"
#include "dtb/smil/Node.h"
#include "dtb/smil/ParNode.h"
#include "dtb/smil/SmilTree.h"
#include "dtb/smil/SeqNode.h"

using namespace std;

//--------------------------------------------------
//Default constructor
//--------------------------------------------------
amis::dtb::smil::SeqNode::SeqNode()
{
	mActiveChildIndex = 0;
}

//--------------------------------------------------
//Destructor
//--------------------------------------------------
amis::dtb::smil::SeqNode::~SeqNode()
{
	//empty function

}

//--------------------------------------------------
/*!
	invoke play on the active child node and pass to it a pointer to the 
	SmilMediaGroup which is being passed around the tree.

  	@param[out] pPlayGroup
		The object pointed to by this parameter will have added to it
		additional playback data

	@param[in] pPlayGroup
		Pointer to an initialized object which represents all the nodes in 
		this playback cluster
*/
//--------------------------------------------------
void amis::dtb::smil::SeqNode::play(SmilMediaGroup* pPlayGroup)
{
	
	//remember this Id information because it helps us find the last good position
	if (getElementId().compare("") != 0)
	{
		getSmilTreePtr()->setCurrentId(getElementId());
	}

	//check if this node could be escaped, then make note of this
	if (getSkipOption().compare("") != 0)
	{
		getSmilTreePtr()->setCouldEscape(true);
		getSmilTreePtr()->setPotentialEscapeNode(this);
	}//end escape check

	//the output is media reference data gotten by 
	//invoking the play function of the active child
	getChild(mActiveChildIndex)->play(pPlayGroup);
}

//--------------------------------------------------
//! this function forces the active child index to a value
//--------------------------------------------------
void amis::dtb::smil::SeqNode::setChildIndex(int index)
{
	mActiveChildIndex = index;
}
int amis::dtb::smil::SeqNode::getChildIndex()
{
	return mActiveChildIndex;
}

//--------------------------------------------------
/*!
	this function increments the active child index on the 
	innermost seq in the subtree that has this seq node as its root
*/
//--------------------------------------------------
amis::Error amis::dtb::smil::SeqNode::setNext()
{
	/*!
		@note TRICKY\n
			this function is tricky; edit at your own risk
	*/

	//local variables
	int i;
	bool b_found = false;
	int len = NumChildren();


	//for-loop starts at the current child node and proceeds to the last child
	for (i = mActiveChildIndex; i<len; i++)
	{
		//if this child is a seq or par, and we're not skipping it
		if (getChild(i)->getCategoryOfNode() == TIME_CONTAINER)
		{
			if(getSmilTreePtr()->mustSkipOrEscapeNode(getChild(i)) == false)
			{
				//if it's the same seq or par we called last time
				if (i == mActiveChildIndex)
				{
					//invoke setNext on this current seq or par child node
					if(((TimeContainerNode*)getChild(i))->setNext().getCode() == amis::OK)
					{
						//set the active child index and break from the loop with bFound = true
						mActiveChildIndex = i;
						b_found = true;
						break;
					} //end if setNext did not return LIST_END

				} //end if this child is the same seq or par from last time

				//else, this is a different seq or par child than what we last visited
				else
				{
					//invoke setFirst on this new seq or par node
					if (((TimeContainerNode*)getChild(i))->setFirst().getCode() == amis::OK)
					{
						//set the active child index and break from the loop with bFound = true
						mActiveChildIndex = i;
						b_found = true;
						break;	
					}
				} //end else, this is a different seq or par child than what we last visited
		
			}//end if node is not to be skipped

		}//end if node is a seq or par

		//else, this node is a content node
		else
		{
			//if this is not the same content node we played last time
			if (mActiveChildIndex != i)
			{
				//set the active child index and break from the loop with bFound = true
				mActiveChildIndex = i;
				b_found = true;
				break;
			} //end if this child is not what we played last time

		}//end else this node is a content node
			
	} //end for loop through this Seq Node's children

	//if a match was found, then return OK
	if (b_found == true)
	{
		return amis::Error(amis::OK, "");
	}
	//else, a match was not found and this Seq Node has reached its limit
	else
	{
		return amis::Error(amis::AT_END, "end of sequence");
	}
} //end amis::dtb::smil::SeqNode::setNext() function

//--------------------------------------------------
/*!
	this function decrements the active child index on the innermost seq 
	in the subtree that has this seq node as its root
*/
//--------------------------------------------------
amis::Error amis::dtb::smil::SeqNode::setPrevious()
{
	/*!
		@note TRICKY\n
			this function is tricky; edit at your own risk
	*/

	//local variables
	int i;
	bool b_found = false;
	amis::Error return_value;

	//for-loop from the current active child down to the first child
	for (i = mActiveChildIndex; i >= 0; i--)
	{
		//if this child is a seq or par
		if (getChild(i)->getCategoryOfNode() == TIME_CONTAINER)
		{
			//if we shouldn't skip it
			if(getSmilTreePtr()->mustSkipOrEscapeNode(getChild(i)) == false)
			{
				//if it's the same seq or par we called last time
				if (i == mActiveChildIndex)
				{
					//invoke setNext on this current seq or par child node				
					return_value = ((TimeContainerNode*)getChild(i))->setPrevious();

					//if the setNext call didn't hit the beginning of a list
					if (return_value.getCode() != amis::AT_BEGINNING)
					{
						//set the active child index and break from the loop with bFound = true
						mActiveChildIndex = i;
						b_found = true;
						break;
					} //end if setNext did not return LIST_END

				} //end if this child is the same seq or par from last time

				//else, this is a different seq or par child than what we last visited
				else
				{
					//invoke setLast on this new seq or par node
					if (((TimeContainerNode*)getChild(i))->setLast().getCode() == amis::OK)
					{
						//set the active child index and break from the loop with bFound = true
						mActiveChildIndex = i;
						b_found = true;
						break;
					}
				} //end else, this is a different seq or par child than what we last visited
			
			}//end if this is not to be skipped

		}//end if node is a seq or par

		//else, this node is a content node
		else
		{
			//if this is not the same content node we played last time
			if (mActiveChildIndex != i)
			{
				//set the active child index and break from the loop with bFound = true
				mActiveChildIndex = i;
				b_found = true;
				break;
			} //end if this child is not what we played last time

		}//end else this node is a content node
			
	} //end for loop through this Seq Node's children

	//if a match was found, then return OK
	if (b_found == true)
	{
		return amis::Error(amis::OK, "");
	}
	//else, a match was not found and this Seq Node has reached its limit
	else
	{
		return amis::Error(amis::AT_BEGINNING, "beginning of sequence");
	}
} //end amis::dtb::smil::SeqNode::setPrevious() function

//--------------------------------------------------
/*!
	this function sets the active child index to be the first non-skipped child
*/
//--------------------------------------------------
amis::Error amis::dtb::smil::SeqNode::setFirst()
{
	//local variables
	int i;
	int len = NumChildren();
	amis::Error return_value;
	return_value.setCode(amis::NOT_FOUND);

	//for-loop from the first to last child
	for (i = 0; i<len; i++)
	{	
		//if a par or seq and not being skipped
		if (getChild(i)->getCategoryOfNode() == TIME_CONTAINER)
		{
			if (getSmilTreePtr()->mustSkipOrEscapeNode(getChild(i)) == false)
			{
				if (((TimeContainerNode*)getChild(i))->setFirst().getCode() == amis::OK)
				{
					//set active child index 
					mActiveChildIndex = i;
					return_value.setCode(amis::OK);
					break;
				}
			}//end if this is not to be skipped

		}
		else
		{
			//set active child index
			mActiveChildIndex = i;
			return_value.setCode(amis::OK);
			break;
		}

	} //end for loop

	return return_value;

} //end of amis::dtb::smil::SeqNode::setFirst() function

//--------------------------------------------------
/*!
	this function sets the active child index to be the last non-skipped child
*/
//--------------------------------------------------
amis::Error amis::dtb::smil::SeqNode::setLast()
{
	//local variables
	int i;
	int len = NumChildren();
	amis::Error return_value;
	return_value.setCode(amis::NOT_FOUND);

	//for-loop, from last to first child
	for (i = len - 1; i >= 0; i--)
	{
		//if this child is a seq or par, invoke its setLast() function
			
		if (getChild(i)->getCategoryOfNode() == TIME_CONTAINER)
		{
			if(getSmilTreePtr()->mustSkipOrEscapeNode(getChild(i)) == false)
			{
				if (((TimeContainerNode*)getChild(i))->setLast().getCode() == amis::OK)
				{
					//set active child index
					mActiveChildIndex = i;
					return_value.setCode(amis::OK);
					break;
				}

			}//end if this is not to be skipped

		} //end if this child is a seq or par

		else
		{
			//set active child index
			mActiveChildIndex = i;
			return_value.setCode(amis::OK);
			break;
		}

	} //end for loop

	return return_value;

} //end amis::dtb::smil::SeqNode::setLast() function

//--------------------------------------------------
/*!
	this node is of type SEQ
*/
//--------------------------------------------------
NodeType amis::dtb::smil::SeqNode::getTypeOfNode()
{
	return SEQ;
}
void amis::dtb::smil::SeqNode::acceptDepthFirstOnActivePath(SmilVisitor* v)
{
	if (v->preVisit(this))
	{
		//read the children
		if (this->getChild(mActiveChildIndex) != NULL)
		{
			this->getChild(mActiveChildIndex)->acceptDepthFirst(v);
		}
	}
}
