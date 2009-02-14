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
//Amis error object
#ifndef ERROR_H
#define ERROR_H

//SYSTEM INCLUDES
#include <string>

//PROJECT INCLUDES
#include "AmisCore.h"

using namespace std;

namespace amis
{
//!Error Codes
typedef signed int ErrorCode;
static ErrorCode OK = 1;
static ErrorCode UNDEFINED_ERROR = -100;
static ErrorCode AT_BEGINNING = -200;
static ErrorCode AT_END = -300;
static ErrorCode NOT_FOUND = -400;
static ErrorCode NOT_SUPPORTED = -500;
static ErrorCode PARSE_ERROR = -600;
static ErrorCode NOT_INITIALIZED = -700;

//!Error object: encapsulates a code, message, and source module name
class Error
{

public:
	//!default constructor
	Error();
	//!destructor
	~Error();
	Error(amis::ErrorCode, string);

	//!set the error code
	void setCode(amis::ErrorCode);
	//!set the error message
	void setMessage(string);
	
	//!get the error code
	amis::ErrorCode getCode();
	//!get the error message
	string getMessage();
	//!reset the error code to OK and the message to ""
	void clear();
	
private:
	//!the error code
	amis::ErrorCode mCode;
	//!the error message
	string mMessage;
};
}
#endif
