/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

Copyright (c) 2004 DAISY Consortium

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

//PROJECT INCLUDES
#include "dtb/smil/SmilMediaGroup.h"

using namespace std;

//--------------------------------------------------
//default constructor
//--------------------------------------------------
amis::dtb::smil::SmilMediaGroup::SmilMediaGroup()
{
}

//--------------------------------------------------
//destructor
//--------------------------------------------------
amis::dtb::smil::SmilMediaGroup::~SmilMediaGroup()
{
}
//--------------------------------------------------
//set escape
//--------------------------------------------------
void amis::dtb::smil::SmilMediaGroup::setEscape(bool escape)
{
	mbEscape = escape;
}

//--------------------------------------------------
//get escapability
//--------------------------------------------------
bool amis::dtb::smil::SmilMediaGroup::getEscape()
{
	return mbEscape;
}
