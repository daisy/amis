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

//FilePathTools
#include <string>
//#include <vector>
#include "util/FilePathTools.h"
#include "AmisCore.h"

//return a string representation of the ambulant URL, for both local and remote files
string amis::util::ambulantUrlToString(const ambulant::net::url* pUrl)
{
	if (pUrl->is_local_file() == true)
		return pUrl->get_file();
	else
		return pUrl->get_url();	
}
string amis::util::ambulantUrlToStringWithRef(const ambulant::net::url* pUrl)
{
	string tmp = ambulantUrlToString(pUrl);
	if (pUrl->get_ref().size() > 0)
		tmp += "#" + pUrl->get_ref();

	return tmp;
}
string amis::util::getFileNameWithRef(const ambulant::net::url* pUrl)
{
	string rel_filename = amis::util::FilePathTools::getFileName(ambulantUrlToString(pUrl));
	string ref = pUrl->get_ref();
	if (ref.size() > 0)
		rel_filename += "#" + ref;
	return rel_filename;
}
//this function will return path1 relative to path2
//however, if path1 is outside of path2, it will just return all of path1
string amis::util::calculateRelativeFilename(const ambulant::net::url* pUrl1, const ambulant::net::url* pUrl2)
{
	string f1 = ambulantUrlToString(&pUrl1->get_base());
	string f2 = ambulantUrlToString(&pUrl2->get_base());
	
	//if f1 is not a subdirectory of f2
	if (f1.size() < f2.size())
	{
		return ambulantUrlToStringWithRef(pUrl1);
	}
	//if the containing folders are the same, just return the filename
	if (f1.compare(f2) == 0)
	{
		return getFileNameWithRef(pUrl1);
	}

	f1 = amis::util::FilePathTools::convertSlashesFwd(f1);
	f2 = amis::util::FilePathTools::convertSlashesFwd(f2);
	int pos = f1.find(f2);
	// if f2 is the base path for f1, then return the rest of f1
	if (pos != string::npos && pos + f2.size() < f1.size() && f1[pos + f2.size()] == '/')
	{
		string rel_filename = f1.substr(pos + f2.size());
		rel_filename = "." + rel_filename + "/" + getFileNameWithRef(pUrl1);
		return rel_filename;

	}
	//else, they are not related
	else
	{
		return ambulantUrlToStringWithRef(pUrl1);
	}
}
//return just the filename and extension
string amis::util::FilePathTools::getFileName(string filepath)
{
	string file_name;
	int pos;
	int substr_len;
	file_name = convertSlashesFwd(filepath);
	pos = file_name.find_last_of("/");

	if (pos!= string::npos && pos < file_name.length() - 1)
	{
		file_name = file_name.substr(pos + 1);
		pos = file_name.find("#");

		//strip the target
		if (pos != string::npos)
		{
			//"filename#target"
			if (pos > 0)
			{
				file_name = file_name.substr(0, pos);
			}
			//#  - mysterious pound sign
			else
			{
				file_name = "";
			}
		}
		
	}
	else
	{
		file_name = "";
	}

	return file_name;
}

//--------------------------------------------------
//--------------------------------------------------
//! return the target
string amis::util::FilePathTools::getTarget(string filepath)
{
	string file_name = filepath;
	string target = "";
	unsigned int pos;
	
	pos = file_name.find("#");

	//strip the target and save
	if (pos != string::npos)
	{
		//"filename#target"
		if (pos > 0 && pos < file_name.length() - 1)
		{
			target = file_name.substr(pos + 1);
		}
		//filename#  - mysterious pound sign
		else
		{
			target = "";
		}
	}

	return target;
}

//--------------------------------------------------
//--------------------------------------------------
//! return the extension
string amis::util::FilePathTools::getExtension(string filepath)
{
	string ext = "";
	string file_name = getFileName(filepath);
	
	unsigned int pos = file_name.find_last_of(".");

	if (pos > 0 && pos < file_name.length() - 1)
	{
		ext = file_name.substr(pos + 1);
	}

	return ext;

}

//!return a string in local path format (windows).  no target is returned.
string amis::util::FilePathTools::getAsLocalFilePath(string filepath)
{

#if defined(AMIS_PLATFORM_WINDOWS)
	//general idea .. strip off any xyz:// and convert the remainder to backslashes
	//also remove the target if exists

	string file_path = filepath;
	int pos;

	pos = file_path.find(":///");

	if (pos != string::npos)
	{
		file_path = file_path.substr(pos+4);
	}

	pos = file_path.find("://");

	if (pos != string::npos)
	{
		file_path = file_path.substr(pos+3);
	}

	pos = file_path.find("#");
	if (pos != string::npos)
	{
		file_path = file_path.substr(0, pos);
	}


	file_path = convertSlashesBkw(file_path);

	return file_path;
#elif defined(AMIS_PLATFORM_LINUX) || defined(AMIS_PLATFORM_OSX)
	//this is just a bad guess
	return filepath;
#else
	//cout<<"ERROR!"<<endl;
	return "";
#endif
}

//--------------------------------------------------
//--------------------------------------------------
//! clear the target from the filepath
string amis::util::FilePathTools::clearTarget(string filepath)
{
	string file_path = filepath;
	int pos;
	
	pos = file_path.find("#");

	//strip the target and discard
	if (pos != string::npos)
	{
		//"filename#target"
		if (pos > 0)
		{
			file_path = file_path.substr(0, pos);
		}
	
	}

	return file_path;
}

//--------------------------------------------------
//--------------------------------------------------
//!navigate based on a relative path beginning with ./, ../, or /
string amis::util::FilePathTools::goRelativePath(string filepath, string relativePath)
{

	string file_path;
	string rel_path;
	string rel_instr;
	string path_append;
	
	int pos;
	int i;
	int len;

	file_path = convertSlashesFwd(filepath);
	rel_path = convertSlashesFwd(relativePath);

	pos = rel_path.find(":/");
	if (pos != string::npos)
	{
		//the relative path is not relative, it contains a drive name
		return rel_path;
	}
	
	//strip any file name off the file path along with the last slash
	pos = file_path.find_last_of("/");
	if (pos != string::npos)
	{
		file_path = file_path.substr(0, pos);
	}


	//parse the relative path slashes into an array
	len = rel_path.length();
	for (i=0; i<len; i++)
	{
		pos = rel_path.find("/");
		if (pos != string::npos)// && pos < rel_path.length() - 1)
		{
			//rel_path_array.push_back(rel_path.substr(0, pos));
			rel_instr = rel_path.substr(0, pos + 1);
			rel_path = rel_path.substr(pos + 1);
			len-=pos;
		}
		else
		{
			path_append = rel_path;
			break;
		}

		//new code
		if (rel_instr.compare("/") == 0)
		{
			//do nothing
		}
		else if (rel_instr.compare("./")==0)
		{
			//do nothing
		}
		else if(rel_instr.compare("../")==0)
		{
			//go up a folder
			pos = file_path.find_last_of("/");

			if (pos != string::npos)
			{
				file_path = file_path.substr(0, pos);
			}
		}
		else
		{
			//make sure there is a slash somewhere between the two strings
			if (file_path[file_path.length() - 1] != '/' &&
				rel_instr.length() > 0 &&
				rel_instr[0] != '/')
			{
				file_path += "/";
			}
			file_path += rel_instr;
		}
	}

	//make sure there is a slash somewhere between the two strings
	if (file_path.length() > 0 &&
		file_path[file_path.length() - 1] != '/' &&
		path_append.length() > 0 &&
		path_append[0] != '/')
	{
		file_path += "/";
	}
	//special case to ensure getting c:/ instead of c:
	if (file_path.length() > 0 &&
		file_path[file_path.length() - 1] == ':')
	{
		file_path +='/';
	}

	file_path.append(path_append);

	return file_path;
	
}

//--------------------------------------------------
//--------------------------------------------------
string amis::util::FilePathTools::convertSlashesFwd(string filepath)
{
	string file_path = filepath;
	unsigned int i;

	//make all the slashes go forward
	for (i=0; i<file_path.length(); i++)
	{
		if (file_path.substr(i, 1).compare("\\")==0)
		{
			file_path.replace(i, 1, "/");
		}
	}

	return file_path;
}

//--------------------------------------------------
//--------------------------------------------------
string amis::util::FilePathTools::convertSlashesBkw(string filepath)
{
	string file_path = filepath;
	unsigned int i;

	//make all the slashes go forward
	for (i=0; i<file_path.length(); i++)
	{
		if (file_path.substr(i, 1).compare("/")==0)
		{
			file_path.replace(i, 1, "\\");
		}
	}

	return file_path;
}

string amis::util::FilePathTools::getParentDirectory(string filepath)
{
	string file_path = convertSlashesFwd(filepath);
	string new_file_path = "";

	//if filepath is just a path of folders (ends in a slash), remove the slash
	//doesn't work with gcc: if (file_path.end() == "/")
	if (file_path[file_path.length() - 1] == '/')
	{
		new_file_path = file_path.substr(0, file_path.length() - 1);
	}

	//strip any file/folder name off the file path
	int pos = file_path.find_last_of("/");
	if (pos != string::npos)
	{
		new_file_path = file_path.substr(0, pos);
	}

	return new_file_path;
}
string amis::util::FilePathTools::getAsUrlPath(string filepath)
{
	//general idea .. strip off any xyz:// and convert the remainder to backslashes
	//also remove the target if exists

	string file_path = filepath;
	int pos;

	pos = file_path.find("://");

	if (pos != string::npos)
	{
		return filepath;
	}

	string prepend = "file:///";

	filepath = convertSlashesFwd(filepath);

	filepath = prepend + filepath;

	return filepath;

}
