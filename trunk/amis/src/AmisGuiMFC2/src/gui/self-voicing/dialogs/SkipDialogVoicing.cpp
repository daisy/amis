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

#include "gui/self-voicing/dialogs/skipdialogvoicing.h"

#include "gui/AmisApp.h"

namespace amis
{
	namespace gui
	{
		namespace dialogs
		{
			SkipDialogVoicing::SkipDialogVoicing(amis::gui::dialogs::SkipDialog * dlg)
			{
				mpDialog = dlg;
			}

			SkipDialogVoicing::~SkipDialogVoicing(void)
			{
				mpDialog = NULL;
			}

			void SkipDialogVoicing::resolvePromptVariables(Prompt* pPrompt) 
			{
				PromptVar* p_var = NULL;
				PromptItem* promptNotAvailable = DataTree::Instance()->findPromptItem("not_available");

				for (int i=0; i<pPrompt->getNumberOfItems(); i++)
				{
					if (pPrompt->getItem(i)->getPromptItemType() == PROMPT_VARIABLE)
					{
						p_var = (PromptVar*)pPrompt->getItem(i);

						if (p_var->getName().compare("SKIPPABLE_ITEM") == 0)
						{
							int i = mpDialog->m_ListSkips.GetCurSel();
							if (i < 0)
							{
								i = 0;
							}

							amis::dtb::CustomTest* p_custom_test = (amis::dtb::CustomTest*)mpDialog->m_ListSkips.GetItemData(i);

							if (p_custom_test != NULL)
							{
								string item_id = p_custom_test->getId();
								amis::gui::spoken::PromptItem* pi = SkipDialogVoicing::getPromptItemForReadingOptionName(item_id);
								if (pi != NULL)
								{
									p_var->setContents(pi->getContents()->clone());
								}
								else
								{
									p_var->setContents(LPCTSTR(item_id.c_str()), "");
								}
							}
						}
						else if (p_var->getName().compare("WILL_SKIP_OR_NOT") == 0)
						{
							//SKIPPABLE_ITEM_ID

							int i = mpDialog->m_ListSkips.GetCurSel();
							if (i < 0)
							{
								i = 0;
							}

							amis::dtb::CustomTest* p_custom_test = (amis::dtb::CustomTest*)mpDialog->m_ListSkips.GetItemData(i);

							if (p_custom_test != NULL)
							{
								string strSelect;

								//false = skip this item
								if (mpDialog->m_ListSkips.GetCheck(i) == 1) //p_custom_test->getCurrentState() == false)
								{
									strSelect = "item_selected";
								}
								else
								{
									strSelect = "item_deselected";
								}

								amis::gui::spoken::PromptItem* pi = NULL;

								pi = amis::gui::spoken::DataTree::Instance()->findPromptItem(strSelect);
								if (pi != NULL)
								{
									p_var->setContents(pi->getContents()->clone());
								}
							}
						}
					}
				}
			}

			amis::gui::spoken::PromptItem* SkipDialogVoicing::getPromptItemForReadingOptionName(string item_id)
			{
				amis::gui::spoken::PromptItem* pi = NULL;
				std::string str = amis::gui::CAmisApp::getPromptIDFromSideBarName(item_id);
				if (str.length() != 0)
					pi = amis::gui::spoken::DataTree::Instance()->findPromptItem(str);
				return pi;
			}

			void SkipDialogVoicing::appendSeqListContent(AudioSequence* seq, amis::dtb::CustomTest* p_custom_test, int i)
			{
				string item_id = p_custom_test->getId();
				amis::gui::spoken::PromptItem* pi = getPromptItemForReadingOptionName(item_id);
				if (pi != NULL)
				{
					AudioSequencePlayer::fillSequenceContents(seq, pi);
				}
				else
				{
					seq->append(item_id.c_str());
				}

				string strSelect;

				if (p_custom_test != NULL)
				{
					//false = skip this item
					if (mpDialog->m_ListSkips.GetCheck(i) == 1) //p_custom_test->getCurrentState() == false)
					{
						strSelect = "item_selected";
					}
					else
					{
						strSelect = "item_deselected";
					}
				}

				pi = amis::gui::spoken::DataTree::Instance()->findPromptItem("thisItemIsCurrently");
				if (pi != NULL)
				{
					AudioSequencePlayer::fillSequenceContents(seq, pi);
				}
				pi = amis::gui::spoken::DataTree::Instance()->findPromptItem(strSelect);
				if (pi != NULL)
				{
					AudioSequencePlayer::fillSequenceContents(seq, pi);
				}
			}

			void SkipDialogVoicing::OnSelchangeSkips() 
			{
				int i = mpDialog->m_ListSkips.GetCurSel();

				amis::dtb::CustomTest* p_test = (amis::dtb::CustomTest*)mpDialog->m_ListSkips.GetItemData(i);

				AudioSequence* seq = new AudioSequence;
				appendSeqListContent(seq, p_test, i);
				amis::gui::spoken::PromptItem * pi = amis::gui::spoken::DataTree::Instance()->findPromptItem("spaceToToggle"); 
				if (pi != NULL)
				{
					AudioSequencePlayer::fillSequenceContents(seq, pi);
				}

				AudioSequencePlayer::Instance()->Play(seq);
			}
		}
	}
}