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
#include "stdafx.h"
#include "util/Color.h"
#include <algorithm>

using namespace amis::util;

Color::Color()
{
	//this should be the list of color names supported by IE
	mColorHexMap["antiquewhite"]="#FAEBD7"; 	 
	mColorHexMap["aqua"]= "#00FFFF"; 	 
	mColorHexMap["aquamarine"]="#7FFFD4";
	mColorHexMap["azure"]="#F0FFFF";
	mColorHexMap["beige"]="#F5F5DC"; 	 
	mColorHexMap["bisque"]="#FFE4C4";	 
	mColorHexMap["black"]="#000000";	 
	mColorHexMap["blanchedalmond"]="#FFEBCD"; 	 
	mColorHexMap["blue"]="#0000FF";
	mColorHexMap["blueviolet"]="#8A2BE2"; 	 
	mColorHexMap["brown"]="#A52A2A";
	mColorHexMap["burlywood"]="#DEB887"; 	 
	mColorHexMap["cadetblue"]="#5F9EA0"; 	 
	mColorHexMap["chartreuse"]="#7FFF00"; 	 
	mColorHexMap["chocolate"]="#D2691E"; 	 
	mColorHexMap["coral"]="#FF7F50"; 	 
	mColorHexMap["cornflowerblue"]="#6495ED"; 	 
	mColorHexMap["cornsilk"]="#FFF8DC"; 	 
	mColorHexMap["crimson"]="#DC143C"; 	 
	mColorHexMap["cyan"]="#00FFFF"; 	 
	mColorHexMap["darkblue"]="#00008B"; 	 
	mColorHexMap["darkcyan"]="#008B8B"; 	 
	mColorHexMap["darkgoldenrod"]="#B8860B"; 	 
	mColorHexMap["darkgray"]="#A9A9A9"; 	 
	mColorHexMap["darkgrey"]="#A9A9A9"; 	 
	mColorHexMap["darkgreen"]="#006400"; 	 
	mColorHexMap["darkkhaki"]="#BDB76B"; 	 
	mColorHexMap["darkmagenta"]="#8B008B"; 	 
	mColorHexMap["darkolivegreen"]="#556B2F"; 	 
	mColorHexMap["darkorange"]="#FF8C00";	 
	mColorHexMap["darkorchid"]="#9932CC"; 	 
	mColorHexMap["darkred"]="#8B0000"; 	 
	mColorHexMap["darksalmon"]="#E9967A"; 	 
	mColorHexMap["darkseagreen"]="#8FBC8F"; 	 
	mColorHexMap["darkslateblue"]="#483D8B"; 	 
	mColorHexMap["darkslategray"]="#2F4F4F";  
	mColorHexMap["darkslategrey"]="#2F4F4F"; 	 
	mColorHexMap["darkturquoise"]="#00CED1"; 	 
	mColorHexMap["darkviolet"]="#9400D3"; 	 
	mColorHexMap["deeppink"]="#FF1493";  
	mColorHexMap["deepskyblue"]="#00BFFF"; 	 
	mColorHexMap["dimgray"]="#696969"; 	 
	mColorHexMap["dimgrey"]="#696969"; 	 
	mColorHexMap["dodgerblue"]="#1E90FF"; 	 
	mColorHexMap["firebrick"]="#B22222"; 	 
	mColorHexMap["floralwhite"]="#FFFAF0"; 	 
	mColorHexMap["forestgreen"]="#228B22"; 	 
	mColorHexMap["fuchsia"]="#FF00FF"; 	 
	mColorHexMap["gainsboro"]="#DCDCDC"; 	 
	mColorHexMap["ghostwhite"]="#F8F8FF"; 	 
	mColorHexMap["gold"]="#FFD700"; 	 
	mColorHexMap["goldenrod"]="#DAA520"; 	 
	mColorHexMap["gray"]="#808080"; 	 
	mColorHexMap["grey"]="#808080"; 	 
	mColorHexMap["green"]="#008000"; 	 
	mColorHexMap["greenyellow"]="#ADFF2F"; 	 
	mColorHexMap["honeydew"]="#F0FFF0"; 	 
	mColorHexMap["hotpink"]="#FF69B4"; 	 
	mColorHexMap["indianred"]="#CD5C5C"; 	 
	mColorHexMap["indigo"]="#4B0082"; 	 
	mColorHexMap["ivory"]="#FFFFF0"; 	 
	mColorHexMap["khaki"]="#F0E68C"; 	 
	mColorHexMap["lavender"]="#E6E6FA"; 	 
	mColorHexMap["lavenderblush"]="#FFF0F5"; 	 
	mColorHexMap["lawngreen"]="#7CFC00";  
	mColorHexMap["lemonchiffon"]="#FFFACD"; 	 
	mColorHexMap["lightblue"]="#ADD8E6"; 	 
	mColorHexMap["lightcoral"]="#F08080"; 	 
	mColorHexMap["lightcyan"]="#E0FFFF"; 	 
	mColorHexMap["lightgoldenrodyellow"]="#FAFAD2"; 	 
	mColorHexMap["lightgray"]="#D3D3D3"; 	 
	mColorHexMap["lightgrey"]="#D3D3D3"; 	 
	mColorHexMap["lightgreen"]="#90EE90"; 	 
	mColorHexMap["lightpink"]="#FFB6C1"; 	 
	mColorHexMap["lightsalmon"]="#FFA07A"; 	 
	mColorHexMap["lightseagreen"]="#20B2AA"; 	 
	mColorHexMap["lightskyblue"]="#87CEFA"; 	 
	mColorHexMap["lightslategray"]="#778899"; 	 
	mColorHexMap["lightslategrey"]="#778899"; 	 
	mColorHexMap["lightsteelblue"]="#B0C4DE"; 	 
	mColorHexMap["lightYellow"]="#FFFFE0"; 	 
	mColorHexMap["lime"]="#00FF00"; 	 
	mColorHexMap["limegreen"]="#32CD32"; 	 
	mColorHexMap["linen"]="#FAF0E6"; 	 
	mColorHexMap["magenta"]="#FF00FF"; 	 
	mColorHexMap["maroon"]="#800000"; 	 
	mColorHexMap["mediumaquamarine"]="#66CDAA"; 	 
	mColorHexMap["mediumblue"]="#0000CD";	 
	mColorHexMap["mediumorchid"]="#BA55D3"; 	 
	mColorHexMap["mediumpurple"]="#9370D8"; 	 
	mColorHexMap["mediumseagreen"]="#3CB371"; 	 
	mColorHexMap["mediumslateblue"]="#7B68EE"; 	 
	mColorHexMap["mediumspringgreen"]="#00FA9A"; 	 
	mColorHexMap["mediumturquoise"]="#48D1CC"; 	 
	mColorHexMap["mediumvioletred"]="#C71585"; 	 
	mColorHexMap["midnightblue"]="#191970"; 	 
	mColorHexMap["mintcream"]="#F5FFFA"; 	 
	mColorHexMap["mistyrose"]="#FFE4E1"; 	 
	mColorHexMap["moccasin"]="#FFE4B5"; 	 
	mColorHexMap["navajowhite"]="#FFDEAD"; 	 
	mColorHexMap["navy"]="#000080"; 	 
	mColorHexMap["oldlace"]="#FDF5E6"; 	 
	mColorHexMap["olive"]="#808000"; 	 
	mColorHexMap["olivedrab"]="#6B8E23"; 	 
	mColorHexMap["orange"]="#FFA500"; 	 
	mColorHexMap["orangered"]="#FF4500"; 	 
	mColorHexMap["orchid"]="#DA70D6"; 	 
	mColorHexMap["palegoldenrod"]="#EEE8AA"; 	 
	mColorHexMap["palegreen"]="#98FB98"; 	 
	mColorHexMap["paleturquoise"]="#AFEEEE"; 	 
	mColorHexMap["palevioletred"]="#D87093"; 	 
	mColorHexMap["papayawhip"]="#FFEFD5"; 	 
	mColorHexMap["peachpuff"]="#FFDAB9"; 	 
	mColorHexMap["peru"]="#CD853F"; 	 
	mColorHexMap["pink"]="#FFC0CB"; 	 
	mColorHexMap["plum"]="#DDA0DD"; 	 
	mColorHexMap["powderblue"]="#B0E0E6"; 	 
	mColorHexMap["purple"]="#800080"; 	 
	mColorHexMap["red"]="#FF0000"; 	 
	mColorHexMap["rosybrown"]="#BC8F8F"; 	 
	mColorHexMap["royalblue"]="#4169E1"; 	 
	mColorHexMap["saddlebrown"]="#8B4513"; 	 
	mColorHexMap["salmon"]="#FA8072"; 	 
	mColorHexMap["sandybrown"]="#F4A460"; 	 
	mColorHexMap["seagreen"]="#2E8B57"; 	 
	mColorHexMap["seashell"]="#FFF5EE"; 	 
	mColorHexMap["sienna"]="#A0522D";
	mColorHexMap["silver"]="#C0C0C0"; 
	mColorHexMap["skyblue"]="#87CEEB"; 	 
	mColorHexMap["slateblue"]="#6A5ACD"; 	 
	mColorHexMap["slategray"]="#708090"; 	 
	mColorHexMap["slategrey"]="#708090"; 	 
	mColorHexMap["snow"]="#FFFAFA"; 	 
	mColorHexMap["springgreen"]="#00FF7F"; 	 
	mColorHexMap["steelblue"]="#4682B4"; 	 
	mColorHexMap["tan"]="#D2B48C"; 	 
	mColorHexMap["teal"]="#008080"; 	 
	mColorHexMap["thistle"]="#D8BFD8"; 	 
	mColorHexMap["tomato"]="#FF6347"; 	 
	mColorHexMap["turquoise"]="#40E0D0"; 	 
	mColorHexMap["violet"]="#EE82EE"; 	 
	mColorHexMap["wheat"]="#F5DEB3"; 	 
	mColorHexMap["white"]="#FFFFFF"; 	 
	mColorHexMap["whitesmoke"]="#F5F5F5"; 	 
	mColorHexMap["yellow"]="#FFFF00";
	mColorHexMap["yellowgreen"]="#9ACD32";
}
//this expects a HEX value
Color::Color(std::string value)
{
	set(value);
}
Color::Color(COLORREF value)
{
	set(value);
}

//this expects a HEX value
void Color::set(std::string value)
{
	mColorHtml.assign(value);
	mColor = htmlToRgb(value);
}

void Color::setWithHtmlName(std::string value)
{
	mColorHtml = mColorHexMap[value];
	mColor = htmlToRgb(value);
}
void Color::set(COLORREF value)
{
	mColor = value;
	mColorHtml.assign(getColorAsHtmlValue(value));
}

COLORREF Color::getAsColorRef()
{
	return mColor;
}

std::string Color::getAsHtmlHexColor()
{
	return mColorHtml;
}


COLORREF Color::htmlToRgb(std::string html_color)
{
	std::string clr = html_color;
	
	std::wstring wred, wgreen, wblue;

	USES_CONVERSION;

	if (clr.size() < 6)
	{
		return RGB(0,0,0);
	}

	//chop off the leading # sign
	if (clr.substr(0, 1).compare("#") == 0) clr = clr.substr(1);

	wred = A2CW(clr.substr(0, 2).c_str());
	wgreen = A2CW(clr.substr(2, 2).c_str());
	wblue = A2CW(clr.substr(4, 2).c_str());


	int iRed = _httoi(wred.c_str());
	int iGreen = _httoi(wgreen.c_str());
	int iBlue = _httoi(wblue.c_str());

	return RGB(iRed, iGreen, iBlue);
}

//function taken from http://www.codeproject.com/string/hexstrtoint.asp
int Color::_httoi(const TCHAR *value)
{

  struct CHexMap
  {
    TCHAR chr;
    int value;
  };
  const int HexMapL = 16;
  CHexMap HexMap[HexMapL] =
  {
    {'0', 0}, {'1', 1},
    {'2', 2}, {'3', 3},
    {'4', 4}, {'5', 5},
    {'6', 6}, {'7', 7},
    {'8', 8}, {'9', 9},
    {'A', 10}, {'B', 11},
    {'C', 12}, {'D', 13},
    {'E', 14}, {'F', 15}
  };

 TCHAR *mstr = _tcsupr(_tcsdup(value));
 TCHAR *s = mstr;

  int result = 0;

  if (*s == '0' && *(s + 1) == 'X')
  {
	  s += 2;
  }

  bool firsttime = true;

  while (*s != '\0')
  {
    bool found = false;
    for (int i = 0; i < HexMapL; i++)
    {
      if (*s == HexMap[i].chr)
      {
         if (!firsttime) result <<= 4;
        result |= HexMap[i].value;
        found = true;
        break;
      }
    }
    if (!found) break;
    s++;
    firsttime = false;
  }
  free(mstr);
  return result;
}
std::string Color::getColorAsHtmlValue(COLORREF color_ref)
{
	USES_CONVERSION;
	std::string ret_val;

	BYTE rVal = GetRValue(color_ref);
    BYTE gVal = GetGValue(color_ref);
    BYTE bVal = GetBValue(color_ref);

	CString cstr;
	cstr.Format(_T("#%02X%02X%02X"), rVal, gVal, bVal);

	ret_val = T2A(cstr);
	return ret_val;
}

//input: a color name
//output: #RRGGBB
std::string Color::getHexValueForNamedColor(std::string colorName)
{
	std::string color = colorName;
	//make lower case
	std::transform(color.begin(), color.end(), color.begin(), (int(*)(int))tolower);
	return mColorHexMap[color];
}

amis::StringMap* Color::getColorHexMap()
{
	return &mColorHexMap;
}