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

#ifndef AMISCORE_H
#define AMISCORE_H

#include <string>
using namespace std;

#include <vector>
#include "ambulant/net/url.h"

#if defined(AMIS_COMPILER_MSVC)
//this hash_map include is MSVC-specific
#include <hash_map>
#elif defined(AMIS_COMPILER_GCC)
#include <ext/hash_map>
#endif

#if defined(AMIS_COMPILER_GCC)
namespace __gnu_cxx
{
	template<> struct hash< std::string >
	{
		size_t operator()( const std::string& x ) const	
		{
			return hash< const char* >()( x.c_str() );
		}
	};
}
#endif

namespace amis
{
	class MediaNode;
	class TextNode;
	class ImageNode;
	class AudioNode;
	class MediaGroup;
	class Error;
	class BookEntry;
	class BookList;
	class ModuleDescData;
	class SearchDTBText;
	class SearchDTBParser;	

	typedef vector<string> StdStringList;
	typedef vector<ambulant::net::url> UrlList;
	
#if defined(AMIS_COMPILER_MSVC)

	//putting hash_map in stdext:: is MSVC-specific
	typedef stdext::hash_map<std::string, std::string> StringMap;
	typedef stdext::hash_map<std::string, ambulant::net::url> UrlMap;
	typedef stdext::hash_map<std::string, bool> BooleanMap;
	typedef stdext::hash_map<std::string, amis::ModuleDescData*> StringModuleMap;
	typedef stdext::hash_map<const ambulant::net::url*, amis::StdStringList>UrlPtrToStringListMap;

#elif defined(AMIS_COMPILER_GCC)
	typedef __gnu_cxx::hash_map<std::string, std::string> StringMap;
	typedef __gnu_cxx::hash_map<std::string, ambulant::net::url> UrlMap;
	typedef __gnu_cxx::hash_map<std::string, bool> BooleanMap;
	typedef __gnu_cxx::hash_map<std::string, amis::ModuleDescData*> StringModuleMap;
	typedef __gnu_cxx::hash_map<std::string, amis::StdStringList>StringToStringsMap;

#endif

	namespace dtb
	{
		class Dtb;
		class DtbIndex;
		class CustomTest;
		class Metadata;
		class MetaItem;
	
		namespace nav
		{
			class NavContainer;
			class NavList;
			class NavMap;
			class NavModel;
			class NavNode;
			class NavPoint;
			class NavTarget;
			class PageList;
			class PageTarget;
			typedef std::vector<NavNode*> NavNodeList;
			
#if defined(AMIS_COMPILER_MSVC)
			//putting hash_map in stdext:: is MSVC-specific
			typedef stdext::hash_map<std::string, NavNodeList*> NodeRefMap;
#elif defined(AMIS_COMPILER_GCC)
			typedef __gnu_cxx::hash_map<std::string, NavNodeList*> NodeRefMap;
#endif
			typedef std::vector<NavList*> NavListList;

		}
		namespace smil
		{
			class ContentNode;
			class Node;
			class NodeBuilder;
			class ParNode;
			class SeqNode;
			class SmilEngine;
			class SmilMediaGroup;
			class SmilTree;
			class SmilTreeBuilder;
			class TimeContainerNode;
		}
	}
	namespace io
	{	
		class BookListFileIO;
		class BookmarksFileIO;
		class ModuleDescReader;
		class NccFileReader;
		class NavFileReader;
		class NcxFileReader;
		class OpfFileReader;
		class QuickDataSmilFileReader;
		class SmilFileReader;
		class TextSearch;
		class XercesSaxParseBase;
		class UrlInputSource;
	}
	namespace util
	{
		class FilePathTools;
		class SmilAudioExtract;
		class Log;
	}
}

#endif
