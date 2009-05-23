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

#include "gui/self-voicing/dialogs/searchforbooksdialogvoicing.h"

#include "BookList.h"

namespace amis
{
	namespace gui
	{
		namespace dialogs
		{
			SearchForBooksDialogVoicing::SearchForBooksDialogVoicing(amis::gui::dialogs::SearchForBooksDialog * dlg)
			{
				mpDialog = dlg;
			}

			SearchForBooksDialogVoicing::~SearchForBooksDialogVoicing(void)
			{
				mpDialog = NULL;
			}

			void SearchForBooksDialogVoicing::OnSelchangeFilelist() 
			{
				CListBox* p_filelist = NULL;
				p_filelist = (CListBox*)mpDialog->GetDlgItem(IDC_FILESFOUND);

				int sel = p_filelist->GetCurSel();
				if (sel >= 0)
				{ 
					amis::UrlList* p_search_results = mpDialog->mSearcher.getSearchResults();
					if (sel > -1 && sel < p_search_results->size())
					{

						Dialog* p_dlg = DataTree::Instance()->findDialog(IDD_SEARCHDAISY);

						if (p_dlg != NULL)
						{
							DialogControl* p_ctrl = p_dlg->findControl(IDC_FILESFOUND);
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

						AudioSequence * seq = new AudioSequence();

						if (mpDialog->mpBooks != NULL)
						{
							amis::BookEntry * book = mpDialog->mpBooks->getEntry(sel);
							amis::AudioNode * audio = book->getTitleAudio();
							std::wstring str = book->getTitleText();
							if (audio != NULL)
							{
								seq->append(audio->clone(), str.c_str());
							}
							else
							{
								seq->append(str.c_str());
							}
						}

						AudioSequencePlayer::Instance()->Play(seq);
						*/
					}
				}
			}
			void SearchForBooksDialogVoicing::resolvePromptVariables(Prompt* pPrompt) 
			{
				PromptVar* p_var = NULL;
				PromptItem* promptNotAvailable = DataTree::Instance()->findPromptItem("not_available");

				for (int i=0; i<pPrompt->getNumberOfItems(); i++)
				{
					if (pPrompt->getItem(i)->getPromptItemType() == PROMPT_VARIABLE)
					{
						p_var = (PromptVar*)pPrompt->getItem(i);

						if (p_var->getName().compare("NUM_FILES_FOUND") == 0)
						{
							CString value;
							value.Format(_T("%d"), mpDialog->mFilesFound);
							wstring str;
							str = value;
							p_var->setContents(str, "");
						}
						else
						if (p_var->getName().compare("BOOK_TITLE") == 0)
						{
							CListBox* p_filelist = NULL;
							p_filelist = (CListBox*)mpDialog->GetDlgItem(IDC_FILESFOUND);

							int sel = p_filelist->GetCurSel();
							if (sel >= 0)
							{ 
								amis::UrlList* p_search_results = mpDialog->mSearcher.getSearchResults();
								if (sel > -1 && sel < p_search_results->size())
								{
									if (mpDialog->mpBooks != NULL)
									{
										amis::BookEntry * book = mpDialog->mpBooks->getEntry(sel);
										amis::AudioNode * audio = book->getTitleAudio();
										std::wstring str = book->getTitleText();

										p_var->setContents(str, (audio == NULL ? NULL : audio->clone()));
									}
								}
							}
						}
					}
				}
			}
			void SearchForBooksDialogVoicing::announceStatus(CString msg, SearchStatus status)
			{
				if (status == SEARCHING) 
				{
					AudioSequencePlayer::Instance()->playDialogControlFromUiIds(IDD_SEARCHDAISY, IDC_SEARCHING, mpDialog, true, "WhileSearching");
				} 
				else if (status == NONE_FOUND) 
				{
					AudioSequencePlayer::Instance()->playDialogControlFromUiIds(IDD_SEARCHDAISY, IDC_SEARCHING, mpDialog, true, "SearchCompleteNoFilesFound");
				} 
				else if (status == ONE_FOUND || status == MANY_FOUND) 
				{
					CListBox* p_filelist = NULL;
					p_filelist = (CListBox*)mpDialog->GetDlgItem(IDC_FILESFOUND);
					p_filelist->SetCurSel(0);

					AudioSequence * seq = NULL;
					if (status == ONE_FOUND) 
					{
						seq = AudioSequencePlayer::Instance()->playDialogControlFromUiIds(IDD_SEARCHDAISY, IDC_SEARCHING, mpDialog, false, "SearchCompleteOneFileFound");
					} 
					else 
					{
						seq = AudioSequencePlayer::Instance()->playDialogControlFromUiIds(IDD_SEARCHDAISY, IDC_SEARCHING, mpDialog, false, "SearchCompleteFilesFound");
					}

					AudioSequence * seq2 = AudioSequencePlayer::Instance()->playDialogControlFromUiIds(IDD_SEARCHDAISY, IDC_FILESFOUND, mpDialog, false, "default");
					seq->appendAll(seq2);
					delete seq2;

					int sel = p_filelist->GetCurSel();
					if (sel >= 0) 
					{ 
						amis::UrlList* p_search_results = mpDialog->mSearcher.getSearchResults();
						if (sel > -1 && sel < p_search_results->size()) 
						{
							//mLoadBookOnDialogClose = (*p_search_results)[sel];	
							CString str;
							p_filelist->GetText(sel, str);
							seq->append(str);
						}
					}
					AudioSequencePlayer::Instance()->Play(seq);

				} 
				else if (status == STOPPED) 
				{
					AudioSequencePlayer::Instance()->playDialogControlFromUiIds(IDD_SEARCHDAISY, IDC_SEARCHING, mpDialog, true, "SearchAborted");
				} 
			}
		}
	}
}