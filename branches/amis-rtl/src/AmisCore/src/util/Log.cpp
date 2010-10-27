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
#include "util/Log.h"
#ifdef AMIS_PLATFORM_WINDOWS
#include "time.h"
#else
//TODO: timestamp for other platforms
#endif
using namespace std;

amis::util::Log* amis::util::Log::pinstance = 0;

amis::util::Log* amis::util::Log::Instance()
{
	if (pinstance == 0) pinstance = new amis::util::Log(); 
    return pinstance;
}

amis::util::Log::Log()
{
	mbIsFileOpen = false;
	mbEnabled = true;
	mLevel = amis::util::MEDIUM_LOGGING;
	mCachedMessages.clear();
}
amis::util::Log::~Log()
{
}
void amis::util::Log::DestroyInstance()
{
	delete pinstance;
}

void amis::util::Log::enable(bool value)
{
	mbEnabled = value;

	if (mbEnabled)
	{
		for (int i = 0; i<mCachedMessages.size(); i++)
		{
			writeMessage(mCachedMessages[i], "Cached log message");
		}
		mCachedMessages.clear();
	}
}
void amis::util::Log::setLevel(amis::util::LogLevel value)
{
	mLevel = value;
}
void amis::util::Log::cacheMessage(std::string msg)
{
	mCachedMessages.push_back(msg);
}
void amis::util::Log::startLog(string filename)
{
	if (!mbEnabled) return;

	mFile.open(filename.c_str(), ios::out);
	mbIsFileOpen = true;
	mFile<<"AMIS Log"<<endl;
#ifdef AMIS_PLATFORM_WINDOWS
	char datestr[10];
	char timestr[10];
	_strdate(datestr);
	_strtime(timestr);
	mFile<<datestr<<"\n"<<timestr<<endl;
#else
	//TODO: timestamp for other platforms
#endif
	mFile<<"-------------------------------------"<<endl;	
}

void amis::util::Log::endLog()
{
	if (!mbEnabled) return;

#ifdef AMIS_PLATFORM_WINDOWS
	char datestr[10];
	char timestr[10];
	_strdate(datestr);
	_strtime(timestr);
	mFile<<"End of log\n"<<datestr<<"\n"<<timestr<<endl;
#else
	mFile<<"End of log"<<endl;
	//TODO: timestamp for other platforms
#endif
	mFile.close();
	mbIsFileOpen = false;
}

//ERROR logging functions
void amis::util::Log::writeError(amis::Error error, string origin)
{
	writeError(error.getMessage(), origin);
}
void amis::util::Log::writeError(string msg, string origin)
{
	writeData("ERROR", msg, origin);
}
void amis::util::Log::writeError(string msg, const ambulant::net::url* file, string origin)
{	
	string log_msg = msg;
	if (file != NULL) log_msg += file->get_url();
	else log_msg += "*NULL file name*";
	writeError(log_msg, origin);
}
void amis::util::Log::writeError(string msg, string file, string origin)
{
	string log_msg = msg;
	log_msg += file;
	writeError(log_msg, origin);
}

//WARNING logging functions
void amis::util::Log::writeWarning(string msg, string origin)
{
	writeData("WARNING", msg, origin);
}
void amis::util::Log::writeWarning(string msg, const ambulant::net::url* file, string origin)
{
	string log_msg = msg;
	if (file != NULL) log_msg += file->get_url();
	else log_msg += "*NULL file name*";
	writeWarning(log_msg, origin);
}

//MESSAGE logging functions
void amis::util::Log::writeMessage(string msg, string origin)
{
	if (mLevel < amis::util::MEDIUM_LOGGING) return;
	writeData("MESSAGE", msg, origin);
}
void amis::util::Log::writeMessage(string msg, const ambulant::net::url* file, string origin)
{
	if (mLevel < amis::util::MEDIUM_LOGGING) return;

	string log_msg = msg;
	if (file != NULL) log_msg += file->get_url();
	else log_msg += "*NULL file name*";
	writeMessage(log_msg, origin);
}

//TRACE logging functions
void amis::util::Log::writeTrace(string msg, string origin)
{
	if (mLevel < amis::util::FULL_LOGGING) return;
	writeData("TRACE", msg, origin);
}
void amis::util::Log::writeTrace(string msg, const ambulant::net::url* file, string origin)
{
	if (mLevel < amis::util::FULL_LOGGING) return;
	string log_msg;
	if (file != NULL) log_msg += file->get_url();
	else log_msg += "*NULL file name*";
	writeTrace(log_msg, origin);
}

//generic data-writing functions (all others end up here eventually)
void amis::util::Log::writeData(string type, string msg, string origin)
{
	if (!mbEnabled || !mbIsFileOpen)
	{
		string tmp = type;
		tmp.append(": ");
		tmp.append(msg);
		tmp.append(" [");
		tmp.append(origin);
		tmp.append("]");
		mCachedMessages.push_back(tmp);
		return;
	};
	
	if (type != "") mFile<<type<<": ";
	mFile<<msg; 
	if (origin != "") mFile<<" ["<<origin;
	if (origin != "") mFile<<"]";
	mFile<<endl;
}