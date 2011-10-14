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
//INCLUDES
#include "Error.h"
using namespace std;



amis::Error::Error()
{
	clear();
}
amis::Error::Error(amis::ErrorCode code, string message)
{
	setCode(code);
	setMessage(message);
}
amis::Error::~Error()
{
}

void amis::Error::clear()
{
	setCode(amis::OK);
	mMessage.clear();
}

void amis::Error::setCode(amis::ErrorCode code)
{
	mCode = code;
}

void amis::Error::setMessage(string message)
{
	mMessage.assign(message);
}

amis::ErrorCode amis::Error::getCode()
{
	return mCode;
}

string amis::Error::getMessage()
{
	return mMessage;
}
