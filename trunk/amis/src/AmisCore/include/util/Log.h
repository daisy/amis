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
#ifndef LOG_H
#define LOG_H

#include <string>
#include <fstream>

#include "Error.h"
using namespace std;

namespace amis
{
namespace util
{
class Log
{
protected:
	Log();
public:
	static Log* Instance();
	void DestroyInstance();
	~Log();

	void startLog(string);
	void writeMessage(string, string, string);
	void writeMessage(string);
	void writeMessage(string, const ambulant::net::url*);
	void writeWarning(string, string, string);
	void writeError(string, string, string);
	void writeMessage(string, const ambulant::net::url*, string, string);
	void writeWarning(string, const ambulant::net::url*, string, string);
	void writeError(string, const ambulant::net::url*, string, string);
	void writeError(amis::Error, string, string);
	void endLog();

private:
	void writeData(string, string, string, string);
	ofstream mFile;
	static Log* pinstance;
	bool mbIsFileOpen;
};
}
}

#endif