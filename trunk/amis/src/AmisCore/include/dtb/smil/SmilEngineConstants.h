/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

Copyright (C) 2004-2006  DAISY for All Project

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

#ifndef SMILENGINECONSTANTS_H
#define SMILENGINECONSTANTS_H

//SYSTEM INCLUDES
#include <string>
#include <vector>
#include "dtb/CustomTest.h"

using namespace std;

//filetypes
#define FILETYPE_NCC			100
#define FILETYPE_OPF			120
#define FILETYPE_SMIL			140

//book types
#define NO_VERSION_SET			300
#define DAISY202				320
#define DAISY3					340

//file type definitions
#define MASTER_SMIL				"master.smil"
#define EXT_OPF					"opf"
#define NCC_HTML				"ncc.html"

//DAISY tag string defs
#define TAG_A					"a"
#define TAG_PAR					"par"
#define	TAG_IMG					"img"
#define	TAG_AUD					"audio"
#define	TAG_TXT					"text"
#define	TAG_SEQ					"seq"
#define TAG_META				"meta"
#define TAG_REGION				"region"
#define TAG_CUSTOM_TEST			"customTest"
#define TAG_MANIFEST			"manifest"
#define TAG_SPINE				"spine"
#define TAG_ITEM				"item"
#define TAG_ITEMREF				"itemref"
#define TAG_REF					"ref"

//DAISY attribute string defs
#define ATTR_HREF				"href"
#define ATTR_SRC				"src"
#define ATTR_CLIPBEGIN			"clipBegin"
#define ATTR_CLIPEND			"clipEnd"
#define ATTR_CLIP_BEGIN			"clip-begin"
#define ATTR_CLIP_END			"clip-end"
#define ATTR_ID					"id"
#define ATTR_SYSTEM_REQUIRED	"system-required"
#define ATTR_DEFAULT_STATE		"defaultState"
#define ATTR_OVERRIDE			"override"
#define ATTR_CUSTOM_TEST		"customTest"
#define ATTR_REGION				"region"
#define ATTR_TYPE				"type"
#define ATTR_IDREF				"idref"
#define ATTR_MEDIA_TYPE			"media-type"

//DAISY attribute value string defs
#define VAL_TRUE				"true"
#define VAL_FALSE				"false"
#define VAL_VISIBLE				"visible"
#define VAL_HIDDEN				"hidden"
#define VAL_APP_SMIL			"application/smil"

//return messages
#define MSG_BOOK_NOT_OPEN		"Book_Not_Open"
#define MSG_BEGINNING_OF_BOOK	"Beginning_Of_Book"
#define MSG_END_OF_BOOK			"End_Of_Book"


//! NodeType enumeration
enum NodeType
{
	NONE = 0,
	SEQ = 1, 
	PAR = 2, 
	AUD = 3, 
	TXT = 4, 
	IMG = 5,
	A = 6,
	CONTENT = 7,
	TIME_CONTAINER = 8
};


//! content region structure for layout regions
struct ContentRegionData
{
	string mId;
	string mXmlString;
};
//end of file
#endif
