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

#include "gui/self-voicing/dialogs/publicationsummarydialogvoicing.h"
#include "DtbWithHooks.h"
#include "dtb/Dtb.h"

namespace amis
{
	namespace gui
	{
		namespace dialogs
		{
			PublicationSummaryDialogVoicing::PublicationSummaryDialogVoicing(amis::gui::dialogs::PublicationSummaryDialog * dlg)
			{
				mpDialog = dlg;
			}

			PublicationSummaryDialogVoicing::~PublicationSummaryDialogVoicing(void)
			{
				mpDialog = NULL;
			}

			void PublicationSummaryDialogVoicing::OnLvnItemchangedSummary(NMHDR *pNMHDR, LRESULT *pResult)
			{
				LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
				if (pNMLV->iItem != -1)
				{
					if (pNMLV->uNewState == 3)
					{
						//CListCtrl* p_ctrl_list = (CListCtrl*)mpDialog->GetDlgItem(IDC_SUMMARYLIST);
						//int i = p_ctrl_list->GetSelectionMark(); // Should = pNMLV->iItem
						int i = pNMLV->iItem;

						Dialog* p_dlg = DataTree::Instance()->findDialog(IDD_PUBLICATIONSUMMARY);

						if (p_dlg != NULL)
						{
							DialogControl* p_ctrl = p_dlg->findControl(IDC_SUMMARYLIST);
							if (p_ctrl != NULL)
							{
								LabelList* p_list = p_ctrl->getLabelList();
								if (p_list != NULL)
								{
									Prompt* p_prompt = p_list->getLabel(i)->getPrompt();
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
					}
				}
			}
			void PublicationSummaryDialogVoicing::resolvePromptVariables(Prompt* pPrompt) 
			{

				PromptVar* p_var = NULL;
				PromptItem* promptNotAvailable = DataTree::Instance()->findPromptItem("not_available");

				for (int i=0; i<pPrompt->getNumberOfItems(); i++)
				{
					if (pPrompt->getItem(i)->getPromptItemType() == PROMPT_VARIABLE)
					{
						p_var = (PromptVar*)pPrompt->getItem(i);

						if (p_var->getName().compare("META_TITLE") == 0)
						{
							//amis::dtb::DtbWithHooks::Instance()->getTitle()->
							amis::AudioNode * node = mpDialog->mpTitle->getAudio(0);
							//xxxxx resolve full path
							//join_to_base
							//AudioSequencePlayer::Instance()->computeExpandedSrc(node, langID, url)

							wstring str;
							str = mpDialog->computeTitle();

							if (node == NULL) {
								if (str.length() != 0)
								{
									p_var->setContents(str, "");
								}
								else if (promptNotAvailable != NULL)
								{
									p_var->setContents(promptNotAvailable->getContents()->clone());
								}
							}
							else
							{
								p_var->setContents(str, node->clone());
							}
						}
						else if (p_var->getName().compare("META_CREATOR") == 0)
						{
							wstring str;
							str = mpDialog->computeAuthor();
							if (str.length() != 0)
							{
								p_var->setContents(str, "");
							}
							else if (promptNotAvailable != NULL)
							{
								p_var->setContents(promptNotAvailable->getContents()->clone());
							}
						} 

						else if (p_var->getName().compare("META_PUBLISHER") == 0)
						{
							if (mpDialog->mPublisher.length() != 0)
							{
								p_var->setContents(mpDialog->mPublisher, "");
							}
							else if (promptNotAvailable != NULL)
							{
								p_var->setContents(promptNotAvailable->getContents()->clone());
							}
						} 
						else if (p_var->getName().compare("META_DESC") == 0)
						{
							if (mpDialog->mDescription.length() != 0)
							{
								p_var->setContents(mpDialog->mDescription, "");
							}
							else if (promptNotAvailable != NULL)
							{
								p_var->setContents(promptNotAvailable->getContents()->clone());
							}
						} 
						else if (p_var->getName().compare("META_NARRATOR") == 0)
						{
							if (mpDialog->mNarrator.length() != 0)
							{
								p_var->setContents(mpDialog->mNarrator, "");
							}
							else if (promptNotAvailable != NULL)
							{
								p_var->setContents(promptNotAvailable->getContents()->clone());
							}
						} 
						else if (p_var->getName().compare("META_TOTALTIME") == 0)
						{
							if (mpDialog->mTotalTime.length() != 0)
							{
								p_var->setContents(mpDialog->mTotalTime, "");
							}
							else if (promptNotAvailable != NULL)
							{
								p_var->setContents(promptNotAvailable->getContents()->clone());
							}
						} 
						else if (p_var->getName().compare("META_NUMPAGES") == 0)
						{
							CString value;
							value.Format(_T("%d"), mpDialog->mTotalNumPages);
							wstring str;
							str = value;
							p_var->setContents(str, "");
						} 
						else if (p_var->getName().compare("META_DATE") == 0)
						{
							if (mpDialog->mDate.length() != 0)
							{
								p_var->setContents(mpDialog->mDate, "");
							}
							else if (promptNotAvailable != NULL)
							{
								p_var->setContents(promptNotAvailable->getContents()->clone());
							}
						} 
						else if (p_var->getName().compare("META_FORMAT") == 0)
						{
							if (mpDialog->mFormat.length() != 0)
							{
								p_var->setContents(mpDialog->mFormat, "");
							}
							else if (promptNotAvailable != NULL)
							{
								p_var->setContents(promptNotAvailable->getContents()->clone());
							}
						} 

						else if (p_var->getName().compare("META_TOCITEMS") == 0)
						{
							amis::dtb::nav::NavModel* p_nav = mpDialog->mpBook->getNavModel();
							int max = p_nav->getNavMap()->getNumberOfNavPoints();
							CString value;
							value.Format(_T("%d"), max);
							wstring str;
							str = value;
							p_var->setContents(str, "");
							//p_var->setContents(promptNotAvailable->getContents()->clone());
						} 
						else if (p_var->getName().compare("META_DEPTH") == 0)
						{
							amis::dtb::nav::NavModel* p_nav = mpDialog->mpBook->getNavModel();
							int max = p_nav->getNavMap()->getMaxDepth();
							CString value;
							value.Format(_T("%d"), max);
							wstring str;
							str = value;
							p_var->setContents(str, "");
							//p_var->setContents(promptNotAvailable->getContents()->clone());
						} 
						else if (p_var->getName().compare("TOC_CATEGORIES") == 0)
						{
							if (mpDialog->mNavigableItems.length() != 0)
							{

								p_var->setContents(mpDialog->mNavigableItems, "");

								/*
								amis::dtb::nav::NavModel* p_nav = mpBook->getNavModel();
								if (p_nav->getNavMap()->getLabel() != NULL)
								{
								amis::AudioNode * node = p_nav->getNavMap()->getLabel()->getAudio(0);
								p_var->setContents(p_nav->getNavMap()->getLabel()->getText()->getTextString(), node);
								}
								else
								{
								p_var->setContents(mNavigableItems, "");
								}
								*/
							}
							else if (promptNotAvailable != NULL)
							{
								p_var->setContents(promptNotAvailable->getContents()->clone());
							}

							/*
							//TODO: get the full multimedia data (should be available directly from the nav data model)

							amis::dtb::nav::NavModel* p_nav = mpBook->getNavModel();
							if (p_nav->getNavMap()->getLabel() != NULL)
							{
							mNavigableItems = p_nav->getNavMap()->getLabel()->getText()->getTextString();
							if (p_nav->hasPages())
							{	
							mNavigableItems.append(L", ");
							mNavigableItems.append(p_nav->getPageList()->getLabel()->getText()->getTextString());
							}
							}
							int sz = p_nav->getNumberOfNavLists();
							for (int i=0; i<sz; i++)
							{
							mNavigableItems.append(L", ");
							if (p_nav->getNavList(i)->getLabel() != NULL)
							mNavigableItems.append(p_nav->getNavList(i)->getLabel()->getText()->getTextString());
							} */
						} 

						else if (p_var->getName().compare("CURRENT_PAGE") == 0)
						{
							if (mpDialog->mpCurrentPage != NULL) {
								amis::AudioNode * node = mpDialog->mpCurrentPage->getAudio(0);
								p_var->setContents(mpDialog->mpCurrentPage->getText()->getTextString(), (node != NULL ? node->clone() : NULL));
							} else if (promptNotAvailable != NULL)
							{
								p_var->setContents(promptNotAvailable->getContents()->clone());
							}
						} 
						/*else if (p_var->getName().compare("CURRENT_TIME") == 0)
						{
						p_var->setContents(promptNotAvailable->getContents()->clone());
						} */

						else if (p_var->getName().compare("CURRENT_SECTION_TITLE") == 0)
						{
							if (mpDialog->mpSectionName != NULL)
							{
								amis::AudioNode * node = mpDialog->mpSectionName->getAudio(0);
								p_var->setContents(mpDialog->mpSectionName->getText()->getTextString(), (node != NULL ? node->clone() : NULL));
							} else if (promptNotAvailable != NULL)
							{
								p_var->setContents(promptNotAvailable->getContents()->clone());
							}
						} 

						else if (p_var->getName().compare("CURRENT_SECTION_NUM_SUBSECTIONS") == 0)
						{
							CString value;
							value.Format(_T("%d"), mpDialog->mNumSubsections);
							wstring str;
							str = value;
							p_var->setContents(str, "");
						} 

						else if (p_var->getName().compare("CURRENT_SECTION_NUM_PAGES") == 0)
						{
							CString value;
							value.Format(_T("%d"), mpDialog->mNumPagesForSection);
							wstring str;
							str = value;
							p_var->setContents(str, "");
						} 
					}
				}
			}
		}
	}
}