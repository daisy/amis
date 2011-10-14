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

#ifndef SMILMEDIAGROUP_H
#define SMILMEDIAGROUP_H

//SYSTEM INCLUDES
#include <string>

//PROJECT INCLUDES
#include "Media.h"

#include "dtb/smil/SmilEngineConstants.h"

using namespace std;

//! SmilMediaGroup represents a group nodes to be played simultaneously
/*!
audio nodes, if more than one exists, are to be played in sequence.  this is a special
case for situations where additional audio data is prepended to a media group, such as
["chapter"] + " 4.3 how to write a daisy player"

the SmilMediaGroup is derived from the more generic amis::MediaGroup class
*/
namespace amis
{
namespace dtb
{
namespace smil
{
class SmilMediaGroup : public amis::MediaGroup
{
public:

	SmilMediaGroup();
	~SmilMediaGroup();

	//!set escapability info
	void setEscape(bool);

	//!get escapability info
	bool getEscape();

private:
	//!escapability on or off (true or false)
	bool mbEscape;

};
}
}
}
#endif
