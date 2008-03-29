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

#ifndef COLOR_H
#define COLOR_H

#include <string>
#include "AmisCore.h"

namespace amis
{
namespace util
{
class Color
{
private:
	COLORREF mColor;
	std::string mColorHtml;
public:
	Color();
	//needs a hex value
	Color(std::string);
	Color(COLORREF);
	void set(std::string);
	void set(COLORREF);
	void setWithHtmlName(std::string);
	COLORREF getAsColorRef();
	std::string getAsHtmlHexColor();
	std::string getHexValueForNamedColor(std::string);
	amis::StringMap* getColorHexMap();
private:
	static COLORREF htmlToRgb(std::string);
	static int _httoi(const TCHAR *value);
	static std::string getColorAsHtmlValue(COLORREF);
	amis::StringMap mColorHexMap;

};
}
}
#endif