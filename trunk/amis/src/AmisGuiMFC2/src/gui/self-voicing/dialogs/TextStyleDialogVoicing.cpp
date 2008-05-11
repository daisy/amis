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

#include "gui/self-voicing/dialogs/textstyledialogvoicing.h"

namespace amis
{
	namespace gui
	{
		namespace dialogs
		{
			TextStyleDialogVoicing::TextStyleDialogVoicing(amis::gui::dialogs::TextStyleDialog * dlg)
			{
				mpDialog = dlg;
			}

			TextStyleDialogVoicing::~TextStyleDialogVoicing(void)
			{
				mpDialog = NULL;
			}

			void TextStyleDialogVoicing::resolvePromptVariables(Prompt* pPrompt)
			{

				USES_CONVERSION;

				PromptVar* p_var = NULL;
				for (int i=0; i<pPrompt->getNumberOfItems(); i++)
				{
					if (pPrompt->getItem(i)->getPromptItemType() == PROMPT_VARIABLE)
					{
						p_var = (PromptVar*)pPrompt->getItem(i);

						if (p_var->getName().compare("FONT_NAME") == 0)
						{
							CString tmp_font;
							CComboBox* p_font_list = (CComboBox*)mpDialog->GetDlgItem(IDC_FONT);
							p_font_list->GetWindowText(tmp_font);

							wstring str;
							str = tmp_font;
							p_var->setContents(str, "");
						} else 
							if (p_var->getName().compare("FOREGROUND_COLOR_NAME") == 0)
							{

								CString tmp_fore;
								CComboBox* p_foreground_list = (CComboBox*)mpDialog->GetDlgItem(IDC_HIGHLIGHTFOREGROUND);
								p_foreground_list->GetWindowText(tmp_fore);

								wstring str;
								str = tmp_fore;
								p_var->setContents(str, "");
							} else 
								if (p_var->getName().compare("BACKGROUND_COLOR_NAME") == 0)
								{

									CString tmp_back;
									CComboBox* p_background_list = (CComboBox*)mpDialog->GetDlgItem(IDC_HIGHLIGHTBACKGROUND);
									p_background_list->GetWindowText(tmp_back);


									wstring str;
									str = tmp_back;
									p_var->setContents(str, "");
								} 
					}
				}
			}

		}
	}
}