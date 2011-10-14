/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

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

#ifndef MULTIBOOKSDIALOG_VOICING_H
#define MULTIBOOKSDIALOG_VOICING_H

#include "gui/dialogs/MultipleBooksOnVolumeDialog.h";

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
			class MultipleBooksOnVolumeDialogVoicing
			{
			private:
				amis::gui::dialogs::MultipleBooksOnVolumeDialog * mpDialog;

			public:
				MultipleBooksOnVolumeDialogVoicing(amis::gui::dialogs::MultipleBooksOnVolumeDialog * dlg);
				~MultipleBooksOnVolumeDialogVoicing(void);
				
				void OnSelchangeFilelist();
				void resolvePromptVariables(Prompt* pPrompt);
			};
		}
	}
}

#endif //MULTIBOOKSDIALOG_VOICING_H