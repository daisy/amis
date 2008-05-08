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

#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
using namespace std;


namespace amis
{
namespace gui
{
namespace spoken
{

class CIniFile
{
public:
	struct Record
	{
		string Comments;
		char Commented;
		string Section;
		string Key;
		string Value;
	};

	enum CommentChar
	{
		Pound = '#',
		SemiColon = ';'
	};

	CIniFile(void);
	virtual ~CIniFile(void);

static bool string2int(const char* digit, int& result);

	static bool AddSection(string SectionName, string FileName);
	static bool CommentRecord(CommentChar cc, string KeyName,string SectionName,string FileName);
	static bool CommentSection(char CommentChar, string SectionName, string FileName);
	static string Content(string FileName);
	static bool Create(string FileName);
	static bool DeleteRecord(string KeyName, string SectionName, string FileName);
	static bool DeleteSection(string SectionName, string FileName);
	static vector<Record> GetRecord(string KeyName, string SectionName, string FileName);
	static vector<Record> GetSection(string SectionName, string FileName);
	static vector<string> GetSectionNames(string FileName);
	static string GetValue(string KeyName, string SectionName, string FileName);
	static bool RecordExists(string KeyName, string SectionName, string FileName);
	static bool RenameSection(string OldSectionName, string NewSectionName, string FileName);
	static bool SectionExists(string SectionName, string FileName);
	static bool SetRecordComments(string Comments, string KeyName, string SectionName, string FileName);
	static bool SetSectionComments(string Comments, string SectionName, string FileName);
	static bool SetValue(string KeyName, string Value, string SectionName, string FileName);
	static bool Sort(string FileName, bool Descending);
	static bool UnCommentRecord(string KeyName,string SectionName,string FileName);
	static bool UnCommentSection(string SectionName, string FileName);

private:
	static vector<Record> GetSections(string FileName);
	static bool Load(string FileName, vector<Record>& content);	
	static bool Save(string FileName, vector<Record>& content);

	struct RecordSectionIs : std::unary_function<Record, bool>
	{
		std::string section_;

		RecordSectionIs(const std::string& section): section_(section){}

		bool operator()( const Record& rec ) const
		{
			return rec.Section == section_;
		}
	};

	struct RecordSectionKeyIs : std::unary_function<Record, bool>
	{
		std::string section_;
		std::string key_;

		RecordSectionKeyIs(const std::string& section, const std::string& key): section_(section),key_(key){}

		bool operator()( const Record& rec ) const
		{
			return ((rec.Section == section_)&&(rec.Key == key_));
		}
	};

	struct AscendingSectionSort
	{
		bool operator()(Record& Start, Record& End)
		{
			return Start.Section < End.Section;
		}
	};

	struct DescendingSectionSort
	{
		bool operator()(Record& Start, Record& End)
		{
			return Start.Section > End.Section;
		}
	};

	struct AscendingRecordSort
	{
		bool operator()(Record& Start, Record& End)
		{
			return Start.Key < End.Key;
		}
	};

	struct DescendingRecordSort
	{
		bool operator()(Record& Start, Record& End)
		{
			return Start.Key > End.Key;
		}
	};
};
}}}