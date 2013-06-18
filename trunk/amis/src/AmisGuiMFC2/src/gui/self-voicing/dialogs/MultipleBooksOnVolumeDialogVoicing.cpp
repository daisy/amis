/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

Copyright (c) 2004 DAISY Consortium

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
#include "gui/self-voicing/dialogs/MultipleBooksOnVolumeDialogvoicing.h"

namespace amis
{
	namespace gui
	{
		namespace dialogs
		{
			MultipleBooksOnVolumeDialogVoicing::MultipleBooksOnVolumeDialogVoicing(amis::gui::dialogs::MultipleBooksOnVolumeDialog * dlg)
			{
				mpDialog = dlg;
			}

			MultipleBooksOnVolumeDialogVoicing::~MultipleBooksOnVolumeDialogVoicing(void)
			{
				mpDialog = NULL;
			}
			
			void MultipleBooksOnVolumeDialogVoicing::OnSelchangeFilelist() 
			{
				CListBox* p_filelist = NULL;
				p_filelist = (CListBox*)mpDialog->GetDlgItem(IDC_BOOKLIST);

				int sel = p_filelist->GetCurSel();
				if (sel >= 0)
				{ 
					if (sel > -1 && sel < mpDialog->mpBookList->getNumberOfEntries())
					{
						Dialog* p_dlg = DataTree::Instance()->findDialog(IDD_CDSEARCH);

						if (p_dlg != NULL)
						{
							DialogControl* p_ctrl = p_dlg->findControl(IDC_BOOKLIST);
							if (p_ctrl != NULL)
							{
								LabelList* p_list = p_ctrl->getLabelList();
								if (p_list != NULL)
								{
									Prompt* p_prompt = p_list->getLabel(0)->getPrompt();
									if (p_prompt != NULL)
									{
										AudioSequence* seq = new AudioSequence();
										AudioSequencePlayer::Instance()->fillSequencePrompt(seq, p_prompt, mpDialog);
										if (seq->IsEmpty()) 
										{
											delete seq;
										}
										else
										{ 
											AudioSequencePlayer::Instance()->Play(seq);
										}
									}
								}
							}
						}

						/*
						// WORKING VERISON, BUT PYPASSES AmisAccessibleUi.xml (keep as a reference)

						CString title = mpDialog->mpBookList->getEntry(sel)->getTitleText().c_str();
						amis::AudioNode* node = mpDialog->mpBookList->getEntry(sel)->getTitleAudio();

						AudioSequence * seq = new AudioSequence();
						if (node != NULL)
							seq->append(node->clone(), title);
						else
							seq->append(title);
						AudioSequencePlayer::Instance()->Play(seq);
						*/
					}
				}
			}

			void MultipleBooksOnVolumeDialogVoicing::resolvePromptVariables(Prompt* pPrompt) 
			{
				PromptVar* p_var = NULL;
				PromptItem* promptNotAvailable = DataTree::Instance()->findPromptItem("not_available");

				for (int i=0; i<pPrompt->getNumberOfItems(); i++)
				{
					if (pPrompt->getItem(i)->getPromptItemType() == PROMPT_VARIABLE)
					{
						p_var = (PromptVar*)pPrompt->getItem(i);

						if (p_var->getName().compare("BOOK_TITLE") == 0)
						{
							CListBox* p_filelist = NULL;
							p_filelist = (CListBox*)mpDialog->GetDlgItem(IDC_BOOKLIST);

							int sel = p_filelist->GetCurSel();
							if (sel >= 0)
							{ 
								if (sel > -1 && sel < mpDialog->mpBookList->getNumberOfEntries())
								{
									CString title = mpDialog->mpBookList->getEntry(sel)->getTitleText().c_str();
									amis::AudioNode* node = mpDialog->mpBookList->getEntry(sel)->getTitleAudio();
									wstring str;
									str = title;
									p_var->setContents(str, (node == NULL ? NULL : node->clone()));
								}
							}
						}
					}
				}
			}
		}
	}
}