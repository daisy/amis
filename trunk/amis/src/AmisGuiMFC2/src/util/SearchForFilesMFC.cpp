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
#include "stdafx.h"
#include "util/SearchForFiles.h"
#include "util/SearchForFilesMFC.h"
#include "ambulant/net/url.h"
#include "util/VolMaster.h"
#include "gui/MainWndParts.h"
#include "util/Log.h"

#include <stdio.h>
#include <sstream>
#include <sys/types.h>

using namespace amis::util;

SearchForFilesMFC::SearchForFilesMFC()
{
	mThreadYielder = NULL;
	mbShouldStop = false;
}

SearchForFilesMFC::~SearchForFilesMFC()
{
	clearSearchResults();
}

int SearchForFilesMFC::startSearch(std::string searchPath)
{
	USES_CONVERSION;
	mbShouldStop = false;
	std::string search_path = searchPath;

	clearSearchResults();
	//make sure it has a trailing slash
	if (search_path[search_path.length() - 1] != '\\') search_path += "\\";
	search_path += "*.*";
	amis::util::Log::Instance()->writeTrace("Start search");
	//call the search routine, which returns the number of files found

	return recursiveSearch(A2T(search_path.c_str()));
}

int SearchForFilesMFC::startSearchOnCdRom()
{
	USES_CONVERSION;
	//find the cdrom drive letter
	amis::util::CVolumeMaster vm;
	CStringArray arDrives;
	int n = vm.GetLogicalDriveStrings(arDrives);
	CString cd_drive;
	int files_found = 0;

	for (int i=0; i<n; i++)
	{
		if (GetDriveType(arDrives.GetAt(i)) == DRIVE_CDROM)
		{
			cd_drive = arDrives.GetAt(i);
			break;
		}
	}

	if (cd_drive.IsEmpty() == FALSE)
	{
		files_found = startSearch(T2A(cd_drive));
	}
	
	return files_found;
}

void SearchForFilesMFC::stopSearch()
{
	mbShouldStop = true;
}

void SearchForFilesMFC::setThreadYielder(ThreadYielder * ty)
{
	mThreadYielder = ty;
}

int SearchForFilesMFC::recursiveSearch(LPCTSTR path)
{
	USES_CONVERSION;

	CString cstr_path = path;
	if (cstr_path.GetLength() == 0) return 0;
	int files_found = 0;

#ifdef USE_LOW_LEVEL_DIRECTORY_SCANNING_METHOD

	HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;
	wstring spec = cstr_path;

	if (mThreadYielder != 0) mThreadYielder->peekAndPump();

	hFind = FindFirstFile(spec.c_str(), &ffd);

	if (mThreadYielder != 0) mThreadYielder->peekAndPump();

    if (hFind == INVALID_HANDLE_VALUE)  {
        return 0;
    }

	cstr_path.Replace(_T("\\*.*"), _T(""));

	do
	{
		if (mThreadYielder != 0) mThreadYielder->peekAndPump();

		// received a stop signal
		if (mbShouldStop == true) break;

        if (wcscmp(ffd.cFileName, L".") != 0 && 
            wcscmp(ffd.cFileName, L"..") != 0)
		{
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (isRecursiveSearch())
				{
					CString str = ffd.cFileName;
					// this folder, which exists on vista, makes the search routine crash
					if (str.Find(_T("winsxs")) != -1)
						continue;
					//this is the recycle bin, which should also not be searched (causes a crash)
					else if (str.Find(_T("recycler")) != -1)
						continue;
					else
						files_found += recursiveSearch(cstr_path + L"\\" + ffd.cFileName + _T("\\*.*"));
				}
            }
            else
			{
				std::string str_filename = T2A(ffd.cFileName);
				if (matchesSearchCriteria(str_filename) && !matchesSearchExclusionCriteria(str_filename)) 
				{
					if (mThreadYielder != 0) mThreadYielder->peekAndPump();

					files_found++;
					CString file_path = cstr_path + L"\\" + ffd.cFileName;
					std::string file_path_string = T2A(file_path);
					
					//we assume that the search returns local files
					ambulant::net::url file_path_url = ambulant::net::url::from_filename(file_path_string);
					addSearchResult(&file_path_url);
				}
            }
        }

		if (mThreadYielder != 0) mThreadYielder->peekAndPump();
    }
	while (FindNextFile(hFind, &ffd) != 0);

	if (mThreadYielder != 0) mThreadYielder->peekAndPump();

	if (GetLastError() != ERROR_NO_MORE_FILES) {
        FindClose(hFind);
        return 0;
    }

    FindClose(hFind);
    hFind = INVALID_HANDLE_VALUE;

#else

	CFileFind finder;
	BOOL b_is_working = finder.FindFile(cstr_path);

	if (mThreadYielder != 0) mThreadYielder->peekAndPump();

	while (b_is_working)
	{
		if (mThreadYielder != 0) mThreadYielder->peekAndPump();

		b_is_working = finder.FindNextFile();

		if (mThreadYielder != 0) mThreadYielder->peekAndPump();

		// received a stop signal
		if (mbShouldStop == true) break;

		if (finder.IsDots()) continue;

		// if it's a directory, recursively search it
		if (finder.IsDirectory())
		{
			if (isRecursiveSearch())
			{
				CString str = finder.GetFilePath();
				// this folder, which exists on vista, makes the search routine crash
				if (str.Find(_T("winsxs")) != -1)
					continue;
				//this is the recycle bin, which should also not be searched (causes a crash)
				else if (str.Find(_T("recycler")) != -1)
					continue;
				else
					files_found += recursiveSearch(str + _T("\\*.*"));
			}
		}
		else
		{
			CString filename;
			filename = finder.GetFileName();
			std::string str_filename = T2A(filename);
			if (matchesSearchCriteria(str_filename) && !matchesSearchExclusionCriteria(str_filename)) 
			{
				files_found++;
				//save the file path
				CString file_path = finder.GetFilePath();
				std::string file_path_string = T2A(file_path);
				
				//we assume that the search returns local files
				ambulant::net::url file_path_url = ambulant::net::url::from_filename(file_path_string);
				addSearchResult(&file_path_url);
			}
			
			if (mThreadYielder != 0) mThreadYielder->peekAndPump();
		}
	}
#endif

	amis::UrlList* results = getSearchResults();
	return results->size();
}

