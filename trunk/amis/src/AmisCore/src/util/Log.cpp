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
}
amis::util::Log::~Log()
{
}
void amis::util::Log::DestroyInstance()
{
	delete pinstance;
}

void amis::util::Log::startLog(string filename)
{
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
void amis::util::Log::writeError(amis::Error error, string origin, string library)
{
	writeError(error.getMessage(), origin, library);
}
void amis::util::Log::writeError(string msg, string origin, string library)
{
	writeData("ERROR", msg, origin, library);
}
void amis::util::Log::writeWarning(string msg, string origin, string library)
{
	writeData("WARNING", msg, origin, library);
}
void amis::util::Log::writeMessage(string msg, string origin, string library)
{
	writeData("", msg, origin, library);
}
void amis::util::Log::writeMessage(string msg, const ambulant::net::url* file, string origin, string library)
{
	string log_msg = msg;
	if (file != NULL) log_msg += file->get_url();
	else log_msg += "*NULL file name*";
	writeMessage(log_msg, origin, library);
}
void amis::util::Log::writeWarning(string msg, const ambulant::net::url* file, string origin, string library)
{
	string log_msg = msg;
	if (file != NULL) log_msg += file->get_url();
	else log_msg += "*NULL file name*";
	writeWarning(log_msg, origin, library);
}
void amis::util::Log::writeError(string msg, const ambulant::net::url* file, string origin, string library)
{
	string log_msg = msg;
	if (file != NULL) log_msg += file->get_url();
	else log_msg += "*NULL file name*";
	writeError(log_msg, origin, library);
}
void amis::util::Log::writeMessage(string msg)
{
	writeData("", msg, "", "");
}
void amis::util::Log::writeMessage(string msg, const ambulant::net::url* file)
{
	writeMessage("", file, "", "");
}
void amis::util::Log::writeData(string type, string msg, string origin, string library)
{
	if (!mbIsFileOpen) return;

	if (type != "") mFile<<type<<": ";
	mFile<<msg; 
	if (origin != "") mFile<<" ["<<origin;
	if (library != "") mFile<<" ("<<library<<") ";
	if (origin != "") mFile<<"]";
	mFile<<endl;
}