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
#ifndef LOG_H
#define LOG_H

#include <string>
#include <fstream>

#include "Error.h"
#include "AmisCore.h"
using namespace std;

namespace amis
{
namespace util
{
//LOW = errors and warnings only
//MEDIUM = errors, warnings, messages
//FULL = errors, warnings, messages, traces
enum LogLevel {LOW_LOGGING, MEDIUM_LOGGING, FULL_LOGGING};

class Log
{
protected:
	Log();
public:
	static Log* Instance();
	void DestroyInstance();
	~Log();

	void startLog(string);
	
	void cacheMessage(string);
	//messages are for logging reporting information
	void writeMessage(string msg, string origin);
	void writeMessage(string msg, const ambulant::net::url* path, string origin);
	
	//warnings are non-fatal errors
	void writeWarning(string msg, string origin);
	void writeWarning(string msg, const ambulant::net::url* path, string origin);
	
	//errors are fatal, or at least really bad
	void writeError(string msg, string origin);
	void writeError(string msg, const ambulant::net::url* path, string origin);
	void writeError(amis::Error err, string origin);
	void writeError(string msg, string file, string origin);

	//traces are for anything else
	void writeTrace(string msg, string origin="");
	void writeTrace(string msg, const ambulant::net::url* path, string origin="");

	void endLog();
	void enable(bool);
	void setLevel(LogLevel);
private:
	void writeData(string type, string msg, string origin);
	ofstream mFile;
	static Log* pinstance;
	bool mbIsFileOpen;
	bool mbEnabled;
	LogLevel mLevel;
	amis::StdStringList mCachedMessages;
};
}
}

#endif