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
#ifndef TOOLBARIO_H
#define TOOLBARIO_H

#include "io/XercesSaxParseBase.h"
#include "gui/toolbar/Toolbar.h"

#include <string>
#include <vector>


XERCES_CPP_NAMESPACE_USE

XERCES_CPP_NAMESPACE_BEGIN
class Attributes;
XERCES_CPP_NAMESPACE_END

namespace amis
{
namespace io
{
class ToolbarIO : public XercesSaxParseBase
{
public:
	ToolbarIO();
	~ToolbarIO();

	bool readFromFile(string);
	amis::gui::toolbar::ToolbarSettings* getToolbarSettings();

	void startElement(const   XMLCh* const    uri,
		const   XMLCh* const    localname,
		const   XMLCh* const    qname,
		const   Attributes&	attributes);
	void endElement(const XMLCh* const uri,
		const XMLCh* const localname,
		const XMLCh* const qname);
	void characters(const XMLCh *const, const unsigned int){/*this function is not used here*/}

private:
	UINT getIdForCommandString(string);
	amis::gui::toolbar::ToolbarSettings* mpToolbarSettings;
	amis::gui::toolbar::ToolbarItems mItems;
	amis::gui::toolbar::ToolbarToggleButton* mpCurrentToggle;
	bool mbFlagInToggle;
};
}
}
#endif