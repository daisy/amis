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

#ifndef TEXTSTYLEDIALOG_VOICING_H
#define TEXTSTYLEDIALOG_VOICING_H

#include "gui/dialogs/TextStyleDialog.h";

#include "gui/self-voicing/datamodel/DataTree.h"
#include "gui/self-voicing/datamodel/Dialog.h"
#include "gui/self-voicing/datamodel/Switch.h"
#include "gui/self-voicing/datamodel/Label.h"
#include "gui/self-voicing/Prompt.h"
#include "gui/self-voicing/UiMediaTypes.h"

namespace amis
{
	namespace gui
	{
		namespace dialogs
		{
			class TextStyleDialogVoicing
			{
			private:
				amis::gui::dialogs::TextStyleDialog * mpDialog;

			public:
				TextStyleDialogVoicing(amis::gui::dialogs::TextStyleDialog * dlg);
				~TextStyleDialogVoicing(void);

				void resolvePromptVariables(Prompt* pPrompt);
			};
		}
	}
}

#endif //TEXTSTYLEDIALOG_VOICING_H