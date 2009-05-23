/*******************************************************************/
//////////////////////////////////////////////////////////////////////
//  Description:
//				The constructor can be called with a global function
//				pointer.  In which case, the class will find the files
//				and pass each file that is found to the global function
//				Method 2:
//				The constructor can be called with OUT a global function.
//				In which case, the class will simply find all files with 
//				the correct criteria, and store the path and file name.
//				The developer can then use either the PerformFunctionOnListedFiles
//				or the PerformClassFunctionOnListedFiles method.
//				The first function takes a global function pointer, and
//				the second function takes a class member function pointer.
//				Both functions will iterate through the file name list
//				and pass each file name to the function pointer.
//				
//  Usage:
//  Example 1:
//	FindFilesInPath My_FindFilesInPath("C:\\temp_dir\\", "*.*");
//	My_FindFilesInPath.PerformFunctionOnListedFiles(CheckForExtraNoChar);
//
//  Example 2:
//	FindFilesInPath MyFindFilesInPath("M:\\BinSearch\\", "*.c*");
//	for(std::vector<std::string>::const_iterator i = MyFindFilesInPath.c_FileList.begin();
//		i != MyFindFilesInPath.c_FileList.end();++i)
//	{
//			cout << i->c_str() << endl;
//	}
// 
//////////////////////////////////////////////////////////////////////


/*******************************************************************
//		*** COPYRIGHT NOTICE ****
//
//	Copyright 2004. David Maisonave (david@axter.com)
//	
//	This code is free for use under the following conditions:
//
//		You may not claim authorship of this code.
//		You may not sell or distrubute this code without author's permission.
//		You can use this code for your applications as you see fit, but this code may not be sold in any form
//		to others for use in their applications.
//		This copyright notice may not be removed.
//
//
//  No guarantees or warranties are expressed or implied. 
//	This code may contain bugs.
*******************************************************************/
//  Please let me know of any bugs, bug fixes, or enhancements made to this code.
//  If you have ideas for this, and/or tips or admonitions, I would be glad to hear them.
//
//////////////////////////////////////////////////////////////////////


#if !defined(FindFilesInPath_H__2CC53D08_5CE6_11D4_B205_0001024219CD__INCLUDED_)
#define FindFilesInPath_H__2CC53D08_5CE6_11D4_B205_0001024219CD__INCLUDED_

#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>


class FindFilesInPath
{
public:
	typedef void (*FUNC_PTR_type)(const std::string &FullFileName);

	FindFilesInPath(std::string StartingPath, const std::string &FileTypes, FUNC_PTR_type fptr = NULL, bool SearchSubDir = true)
		:m_FileTypes(FileTypes), m_FuncPtr(fptr), c_FileList(m_FileList)
	{
		FindInDir(StartingPath, SearchSubDir);
	}
	virtual void PerformFunctionOnListedFiles(FUNC_PTR_type fptr)
	{
		const int Count = m_FileList.size();
		for (int i = 0;i < Count;++i)
		{
			fptr( m_FileList[i]);
		}
	}
	template<typename T, typename F>
		void PerformClassFunctionOnListedFiles(T &SrcObj, F func)
	{
		const int Count = m_FileList.size();
		for (int i = 0;i < Count;++i)
		{
			(SrcObj.*func)(m_FileList[i]);
		}
	}
protected:
	virtual void FindInDir(const std::string &StartingPath, bool SearchSubDir = true)
	{
		std::string Command = 
#ifdef WIN32
			((SearchSubDir)?std::string("dir /B /S "):std::string("dir /B ")) + StartingPath + m_FileTypes;
#define popen _popen
#define pclose _pclose
#else
		(SearchSubDir)?(std::string("find ") + StartingPath + " -name '" + m_FileTypes + "' -print"):(std::string("ls -1 ") + StartingPath + m_FileTypes);
#endif
		FILE   *fp = popen(Command.c_str(), "r");
		if (!fp) return;
		char psBuffer[1024];
		while( !feof( fp ) )
		{
			if( fgets(psBuffer, sizeof(psBuffer), fp ) != NULL )
			{
				if (psBuffer[strlen(psBuffer)-1] == '\n')
					psBuffer[strlen(psBuffer)-1] = 0;
				m_FileList.push_back(psBuffer);
				if (m_FuncPtr) m_FuncPtr(psBuffer);
			}
		}
		pclose(fp);
	}
private:
	FUNC_PTR_type m_FuncPtr;
	std::vector<std::string> m_FileList;
public:
	const std::string m_FileTypes;
	const std::vector<std::string> &c_FileList;
};

#endif // !defined(FindFilesInPath_H__2CC53D08_5CE6_11D4_B205_0001024219CD__INCLUDED_)
