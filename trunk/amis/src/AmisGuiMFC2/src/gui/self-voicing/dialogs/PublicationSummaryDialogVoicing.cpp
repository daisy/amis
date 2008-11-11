/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2008  DAISY Consortium

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

#include "gui/self-voicing/dialogs/skipdialogvoicing.h"

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

				int itemsCount = pPrompt->getNumberOfItems();
				for (int iItem=0; iItem<itemsCount; iItem++)
				{
					if (pPrompt->getItem(iItem)->getPromptItemType() == PROMPT_VARIABLE)
					{
						p_var = (PromptVar*)pPrompt->getItem(iItem);

						if (p_var->getName().compare("META_TITLE") == 0)
						{
							amis:MediaGroup * group = mpDialog->mpTitle; 
							amis::AudioNode * node = (group == 0 ? 0 : group->getAudio(0));

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
							amis::MediaGroup * group = mpDialog->mpBook->getAuthor(); 
							amis::AudioNode * node = (group == 0 ? 0 : group->getAudio(0));

							wstring str;
							str = mpDialog->computeAuthor();
							
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
								USES_CONVERSION;

								amis::dtb::nav::NavModel* p_nav = mpDialog->mpBook->getNavModel();
								if (p_nav->getNavMap()->getLabel() != NULL)
								{		
									amis::gui::spoken::PromptItem * p_item = new amis::gui::spoken::PromptItem();
									p_item->setId(p_var->getName());

									/*
									pPrompt->swapItem(iItem, p_item);
									delete p_var;
									p_var = NULL;
									*/

									std::wstring str = p_nav->getNavMap()->getLabel()->getText()->getTextString();

									if (p_nav->getNavMap()->getLabel()->hasAudio())
									{
										amis::AudioNode * node = p_nav->getNavMap()->getLabel()->getAudio(0);
										p_item->setContents(str, node->clone());
									}
									else
									{
										std::string str_ = T2A(str.c_str());
										PromptItem* prompt = SkipDialogVoicing::getPromptItemForReadingOptionName(str_);
										if (prompt != NULL)
										{
											p_item->setContents(prompt->getContents()->clone());
										}
										else
										{
											p_item->setContents(str, "");
										}
									}

									p_var->appendSubItem(p_item);

									if (p_nav->hasPages())
									{
										amis::gui::spoken::PromptItem * p_item2 = new amis::gui::spoken::PromptItem();
										
										/*
										pPrompt->insertItem(iItem+1, p_item2);
										itemsCount ++;
										iItem ++;
										*/

										std::wstring str = p_nav->getPageList()->getLabel()->getText()->getTextString();

										if (p_nav->getPageList()->getLabel()->hasAudio())
										{
											amis::AudioNode * node = p_nav->getPageList()->getLabel()->getAudio(0);
											p_item2->setContents(str, node->clone());
										}
										else
										{
											std::string str_ = T2A(str.c_str());
											PromptItem* prompt = SkipDialogVoicing::getPromptItemForReadingOptionName(str_);
											if (prompt != NULL)
											{
												p_item2->setContents(prompt->getContents()->clone());
											}
											else
											{
												p_item2->setContents(str, "");
											}
										}
										
										p_var->appendSubItem(p_item2);
									}
								}
								int sz = p_nav->getNumberOfNavLists();
								for (int i=0; i<sz; i++)
								{
									amis::gui::spoken::PromptItem * p_item3 = new amis::gui::spoken::PromptItem();
									
									/*
									pPrompt->insertItem(iItem+1, p_item3);
									itemsCount ++;
									iItem ++;
									*/

									if (p_nav->getNavList(i)->getLabel() != NULL)
									{	
										std::wstring str = p_nav->getNavList(i)->getLabel()->getText()->getTextString();

										if (p_nav->getNavList(i)->getLabel()->hasAudio())
										{
											amis::AudioNode * node = p_nav->getNavList(i)->getLabel()->getAudio(0);
											p_item3->setContents(str, node->clone());
										}
										else
										{
											std::string str_ = T2A(str.c_str());
											PromptItem* prompt = SkipDialogVoicing::getPromptItemForReadingOptionName(str_);
											if (prompt != NULL)
											{
												p_item3->setContents(prompt->getContents()->clone());
											}
											else
											{
												p_item3->setContents(str, "");
											}
										}
									}
									
									p_var->appendSubItem(p_item3);
								}
							}
							else if (promptNotAvailable != NULL)
							{
								p_var->setContents(promptNotAvailable->getContents()->clone());
							}
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