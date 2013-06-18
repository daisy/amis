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

#include "gui/self-voicing/dialogs/preferencesdialogvoicing.h"

#include "gui/self-voicing/TTSPlayer.h"
#include "Preferences.h"

namespace amis
{
	namespace gui
	{
		namespace dialogs
		{
			PreferencesDialogVoicing::PreferencesDialogVoicing(amis::gui::dialogs::PreferencesDialog * dlg)
			{
				mpDialog = dlg;
			}

			PreferencesDialogVoicing::~PreferencesDialogVoicing(void)
			{
				mpDialog = NULL;
			}

			void PreferencesDialogVoicing::playNoVerboseList()
			{	
				CWnd* cwnd = mpDialog->GetFocus();

				AudioSequence * mSeq = AudioSequencePlayer::Instance()->playDialogControlFromUiIds(PreferencesDialog::IDD, (cwnd == NULL ? -1 : cwnd->GetDlgCtrlID()), mpDialog, false, "default", false);
				if (mSeq != NULL)
				{
					if (mSeq->GetCount() == 0)
					{
						delete mSeq;
					}
					else
					{
						AudioSequencePlayer::Instance()->Play(mSeq);
					}
				}
			}

			void PreferencesDialogVoicing::resolvePromptVariables(Prompt* pPrompt) 
			{
				CButton* p_button = NULL;

				PromptItem* promptNotAvailable = DataTree::Instance()->findPromptItem("not_available");

				PromptItem* promptSelected = DataTree::Instance()->findPromptItem("item_selected");
				PromptItem* promptDeSelected = DataTree::Instance()->findPromptItem("item_deselected");

				PromptVar* p_var = NULL;
				for (int i=0; i<pPrompt->getNumberOfItems(); i++)
				{
					if (pPrompt->getItem(i)->getPromptItemType() == PROMPT_VARIABLE)
					{
						p_var = (PromptVar*)pPrompt->getItem(i);

						if (p_var->getName().compare("ON_OR_OFF") == 0)
						{
							if (p_var->getParam().compare("SELF_VOICING_FEATURE") == 0)
							{
								p_button = (CButton*)mpDialog->GetDlgItem(IDC_ISSELFVOICING);
								if (p_button != NULL)
								{
									if (p_button->GetCheck() == 1)
									{
										p_var->setContents(promptSelected->getContents()->clone());
									}
									else
									{
										p_var->setContents(promptDeSelected->getContents()->clone());
									}
								}
							}
							else if (p_var->getParam().compare("HIGHLIGHTTEXT_FEATURE") == 0)
							{

								p_button = (CButton*)mpDialog->GetDlgItem(IDC_HIGHLIGHTTEXT);
								if (p_button != NULL)
								{
									if (p_button->GetCheck() == 1)
									{
										p_var->setContents(promptSelected->getContents()->clone());
									}
									else
									{
										p_var->setContents(promptDeSelected->getContents()->clone());
									}
								}
							}
							else if (p_var->getParam().compare("LOAD_LAST_READ_FEATURE") == 0)
							{
								p_button = (CButton*)mpDialog->GetDlgItem(IDC_LOADLASTBOOK);
								if (p_button != NULL)
								{
									if (p_button->GetCheck() == 1)
									{
										p_var->setContents(promptSelected->getContents()->clone());
									}
									else
									{
										p_var->setContents(promptDeSelected->getContents()->clone());
									}
								}
							}
							else if (p_var->getParam().compare("BASIC_VIEW_FEATURE") == 0)
							{
								p_button = (CButton*)mpDialog->GetDlgItem(IDC_STARTINBASICVIEW);
								if (p_button != NULL)
								{
									if (p_button->GetCheck() == 1)
									{
										p_var->setContents(promptSelected->getContents()->clone());
									}
									else
									{
										p_var->setContents(promptDeSelected->getContents()->clone());
									}
								}
							}
							else if (p_var->getParam().compare("PAUSEONLOSTFOCUS_FEATURE") == 0)
							{
								p_button = (CButton*)mpDialog->GetDlgItem(IDC_PAUSEONLOSTFOCUS);
								if (p_button != NULL)
								{
									if (p_button->GetCheck() == 1)
									{
										p_var->setContents(promptSelected->getContents()->clone());
									}
									else
									{
										p_var->setContents(promptDeSelected->getContents()->clone());
									}
								}
							}
							else if (p_var->getParam().compare("DISABLESCREENSAVER_FEATURE") == 0)
							{
								p_button = (CButton*)mpDialog->GetDlgItem(IDC_DISABLESCREENSAVER);
								if (p_button != NULL)
								{
									if (p_button->GetCheck() == 1)
									{
										p_var->setContents(promptSelected->getContents()->clone());
									}
									else
									{
										p_var->setContents(promptDeSelected->getContents()->clone());
									}
								}
							}
						}
						else if (p_var->getName().compare("LANGUAGE_NAME") == 0)
						{

							CComboBox* lang_combo = (CComboBox*)mpDialog->GetDlgItem(IDC_INSTALLEDLANGUAGES);
							int sel = lang_combo->GetCurSel();
							amis::StringModuleMap* p_langs = Preferences::Instance()->getInstalledLanguages();
							amis::StringModuleMap::iterator it = p_langs->begin();
							//increment the iterator to go to the same number as the combo selection
							for (int i = 0; i<sel; i++) it++;
							mpDialog->mUiLanguageSelection = it->first;

							wstring str;
							CString cstr(mpDialog->mUiLanguageSelection.c_str());
							str = cstr;
							p_var->setContents(str, "");

							amis::ModuleDescData* p_data = Preferences::Instance()->getLanguageData(mpDialog->mUiLanguageSelection);

							if (p_data != NULL || p_data->getLabel() != NULL)
							{
								amis::MediaGroup* p_media = p_data->getLabel();
								//p_media->getText()->getTextString().c_str();

								// No need to clone here (MediaGroup memebers will be cloned inside the setContents method).
								p_var->setContents(p_media);
							}
						}
						else if (p_var->getName().compare("TTS_VOICE_NAME") == 0)
						{
							CComboBox* list = (CComboBox*)mpDialog->GetDlgItem(IDC_TTSVOICES);

							int selected = list->GetCurSel();
							mpDialog->mTTSVoiceIndex = selected;

							if (!Preferences::Instance()->getMustAvoidTTS())
							{
								amis::tts::TTSPlayer::InstanceOne()->SetVoice(selected);
								amis::tts::TTSPlayer::InstanceTwo()->SetVoice(selected);
							}
							
							CString cstr;
							list->GetWindowText(cstr);
							wstring str = L"Daniel";
							str = cstr.GetBuffer();
							p_var->setContents(str, "");
						}
					}
				}
			}
		}
	}
}