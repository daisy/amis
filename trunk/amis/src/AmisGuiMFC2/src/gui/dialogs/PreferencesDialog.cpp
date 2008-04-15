/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amis.sf.net

Copyright (C) 2004-2008  DAISY for All Project

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
#include "gui/dialogs/PreferencesDialog.h"
#include "../resource.h"
#include "AmisCore.h"
#include "Preferences.h"
#include "util/FilePathTools.h"
#include "io/ModuleDescReader.h"
#include "ModuleDescData.h"
#include "ambulant/net/url.h"
#include "gui/self-voicing/TTSPlayer.h"
#include "gui/self-voicing/datamodel/DataTree.h"
#include "gui/MainWndParts.h"

using namespace amis::gui::dialogs;

BEGIN_MESSAGE_MAP(PreferencesDialog, CDialog)
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_ISSELFVOICING, OnBnClickedIsSelfVoicing)
	ON_BN_SETFOCUS(IDC_ISSELFVOICING, OnBnSetfocusIsSelfVoicing)
	ON_CBN_SELCHANGE(IDC_TTSVOICES, OnCbnSelchangeTTSVoices)
	ON_CBN_SETFOCUS(IDC_TTSVOICES, OnCbnSetfocusTTSVoices)
	ON_BN_CLICKED(IDC_STARTINBASICVIEW, OnBnClickedStartInBasicView)
	ON_BN_SETFOCUS(IDC_STARTINBASICVIEW, OnBnSetfocusStartInBasicView)
	ON_BN_CLICKED(IDC_LOADLASTBOOK, OnBnClickedLoadLastBook)
	ON_BN_SETFOCUS(IDC_LOADLASTBOOK, OnBnSetfocusLoadLastBook)
	ON_BN_CLICKED(IDC_HIGHLIGHTTEXT, OnBnClickedHighlightText)
	ON_BN_SETFOCUS(IDC_HIGHLIGHTTEXT, OnBnSetfocusHighlightText)
	ON_CBN_SELCHANGE(IDC_INSTALLEDLANGUAGES, OnCbnSelchangeInstalledLanguages)
	ON_CBN_SETFOCUS(IDC_INSTALLEDLANGUAGES, OnCbnSetfocusInstalledLanguages)
	ON_BN_CLICKED(IDC_DISABLESCREENSAVER, OnBnClickedDisableScreensaver)
	ON_BN_SETFOCUS(IDC_DISABLESCREENSAVER, OnBnSetfocusDisableScreensaver)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void PreferencesDialog::resolvePromptVariables(Prompt* pPrompt) {
	
	
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
					p_button = (CButton*)this->GetDlgItem(IDC_ISSELFVOICING);
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
				} else if (p_var->getParam().compare("HIGHLIGHTTEXT_FEATURE") == 0)
				{
					
					p_button = (CButton*)this->GetDlgItem(IDC_HIGHLIGHTTEXT);
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
				} else if (p_var->getParam().compare("LOAD_LAST_READ_FEATURE") == 0)
				{
					p_button = (CButton*)this->GetDlgItem(IDC_LOADLASTBOOK);
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
				} else if (p_var->getParam().compare("BASIC_VIEW_FEATURE") == 0)
				{
					p_button = (CButton*)this->GetDlgItem(IDC_STARTINBASICVIEW);
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
				}  else if (p_var->getParam().compare("PAUSEONLOSTFOCUS_FEATURE") == 0)
				{
					p_button = (CButton*)this->GetDlgItem(IDC_PAUSEONLOSTFOCUS);
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
				}  else if (p_var->getParam().compare("DISABLESCREENSAVER_FEATURE") == 0)
				{
					p_button = (CButton*)this->GetDlgItem(IDC_DISABLESCREENSAVER);
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
			}else if (p_var->getName().compare("LANGUAGE_NAME") == 0)
			{

				CComboBox* lang_combo = (CComboBox*)GetDlgItem(IDC_INSTALLEDLANGUAGES);
				int sel = lang_combo->GetCurSel();
				amis::StringModuleMap* p_langs = Preferences::Instance()->getInstalledLanguages();
				amis::StringModuleMap::iterator it = p_langs->begin();
				//increment the iterator to go to the same number as the combo selection
				for (int i = 0; i<sel; i++) it++;
				mUiLanguageSelection = it->first;


				wstring str;
				CString cstr(mUiLanguageSelection.c_str());
				str = cstr;
				p_var->setContents(str, "");

				amis::ModuleDescData* p_data = Preferences::Instance()->getCurrentLanguageData();
				if (p_data != NULL || p_data->getLabel() != NULL) {

					amis::MediaGroup* p_media = p_data->getLabel();
					//p_media->getText()->getTextString().c_str();

					p_var->setContents(p_media);

				}


				
			}else if (p_var->getName().compare("TTS_VOICE_NAME") == 0)
			{
				CComboBox* list = (CComboBox*)GetDlgItem(IDC_TTSVOICES);

				int selected = list->GetCurSel();
				mTTSVoiceIndex = selected;

					amis::tts::TTSPlayer::Instance()->ChangeVoice(selected);

					CString cstr;
					list->GetWindowText(cstr);
					wstring str = L"Daniel";
					str = cstr.GetBuffer();
					p_var->setContents(str, "");
				

			}

		}
	}
	AmisDialogBase::resolvePromptVariables(pPrompt);
}

PreferencesDialog::PreferencesDialog(CWnd* pParent /*=NULL*/)
	: AmisDialogBase(PreferencesDialog::IDD)
{
}

PreferencesDialog::~PreferencesDialog()
{
}

string PreferencesDialog::getUiLangSelection()
{
	return mUiLanguageSelection;
}
void PreferencesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_ISSELFVOICING, mbIsSelfVoicing);
	DDX_Check(pDX, IDC_PAUSEONLOSTFOCUS, mbPauseOnLostFocus);
	DDX_Check(pDX, IDC_LOADLASTBOOK, mbLoadLastBook);
	DDX_Check(pDX, IDC_STARTINBASICVIEW, mbStartInBasicView);
	DDX_Check(pDX, IDC_HIGHLIGHTTEXT, mbHighlightText);
	DDX_Check(pDX, IDC_DISABLESCREENSAVER, mbDisableScreensaver);
}
BOOL PreferencesDialog::OnInitDialog() 
{	
	
	mbIsSelfVoicing = Preferences::Instance()->getIsSelfVoicing();
	mbPauseOnLostFocus = Preferences::Instance()->getPauseOnLostFocus();
	mbLoadLastBook = Preferences::Instance()->getLoadLastBook();
	mbStartInBasicView = Preferences::Instance()->getStartInBasicView();
	mbHighlightText = Preferences::Instance()->getHighlightText();
	mUiLanguageSelection = Preferences::Instance()->getUiLangId();
	mbDisableScreensaver = Preferences::Instance()->getDisableScreensaver();

	initializeTTSVoiceOption();
	initializeUiLanguageOption();
	
	return CDialog::OnInitDialog();
}

void PreferencesDialog::initializeTTSVoiceOption()
{
	// Make sure to initialize the voice (just in case TTSPlayer has been unused until now)
	amis::tts::TTSPlayer::Instance()->ChangeVoice(Preferences::Instance()->getTTSVoiceIndex());

	mTTSVoiceIndex = 0;
	//fill the combo box for the tts voices, and return an index to the voice we are currently using
	int index = amis::tts::TTSPlayer::Instance()->initVoiceList(this->GetSafeHwnd());
	if (index != -1) 
	{
		CComboBox* list = (CComboBox*)GetDlgItem(IDC_TTSVOICES);
		list->SetCurSel(index);
		mTTSVoiceIndex = index;
	}
}
void PreferencesDialog::initializeUiLanguageOption()
{
	//fill the combo with language names
	CComboBox* list = (CComboBox*)GetDlgItem(IDC_INSTALLEDLANGUAGES);
	amis::StringModuleMap* p_langs = Preferences::Instance()->getInstalledLanguages();
	amis::StringModuleMap::iterator it = p_langs->begin();
	int sel = -1;
	int count = 0;
	while (it != p_langs->end())
	{
		amis::ModuleDescData* p_data = it->second;
		CString label = p_data->getLabel()->getText()->getTextString().c_str();
		list->AddString(label);
		//if this happens to be our currently selected language, mark its position
		string id = p_data->getId();
		if (id == mUiLanguageSelection) sel = count;
		it++;
		count++;
	}
	list->SetCurSel(sel);
	setCurrentLanguageLabel();
}

//This just appends the name of the current language to the dialog label
//TODO: what we really need to do is resolve the prompt and build the string
//"You are currently using AMIS in <LANGUAGE>"
void PreferencesDialog::setCurrentLanguageLabel()
{
	amis::ModuleDescData* p_data = Preferences::Instance()->getCurrentLanguageData();
	if (p_data == NULL || p_data->getLabel() == NULL) return;
	amis::MediaGroup* p_media = p_data->getLabel();
	CStatic* dlg_label = (CStatic*)this->GetDlgItem(IDC_CURRLANG);
	CString label;
	dlg_label->GetWindowText(label);
	label.Append(p_media->getText()->getTextString().c_str());
	dlg_label->SetWindowText(label);
}

void PreferencesDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Let the window do its default painting...
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );
	//call the base class on_paint function
	on_paint();
}


BOOL PreferencesDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		CWnd* p_wnd = this->GetFocus();
		if (p_wnd)
		{
			int id = p_wnd->GetDlgCtrlID();
			//inexplicably, the default behavior for pressing enter in a dialog is to close it.
			//we're overriding this here.
			if (pMsg->wParam == VK_RETURN)
			{
				//do nothing
			}	
		}
	}
	return AmisDialogBase::PreTranslateMessage(pMsg);
}


//widget event handlers
void PreferencesDialog::OnBnClickedIsSelfVoicing()
{
	CWnd* cwnd = this->GetFocus();
	triggerVirtualKeyStroke(cwnd);
}
void PreferencesDialog::OnBnSetfocusIsSelfVoicing()
{
}
void PreferencesDialog::OnCbnSelchangeTTSVoices()
{
	CComboBox* list = (CComboBox*)GetDlgItem(IDC_TTSVOICES);

	int selected = list->GetCurSel();
	mTTSVoiceIndex = selected;

	OnBnClickedIsSelfVoicing();
}
void PreferencesDialog::OnCbnSetfocusTTSVoices()
{
	OnBnClickedIsSelfVoicing();
}
void PreferencesDialog::OnBnClickedStartInBasicView()
{
	OnBnClickedIsSelfVoicing();
}
void PreferencesDialog::OnBnSetfocusStartInBasicView()
{
}
void PreferencesDialog::OnBnClickedLoadLastBook()
{
	OnBnClickedIsSelfVoicing();
}
void PreferencesDialog::OnBnSetfocusLoadLastBook()
{
}
void PreferencesDialog::OnBnSetfocusHighlightText()
{
}
void PreferencesDialog::OnBnClickedHighlightText()
{
	OnBnClickedIsSelfVoicing();
}
void PreferencesDialog::OnCbnSelchangeInstalledLanguages()
{
	CComboBox* lang_combo = (CComboBox*)GetDlgItem(IDC_INSTALLEDLANGUAGES);
	int sel = lang_combo->GetCurSel();
	amis::StringModuleMap* p_langs = Preferences::Instance()->getInstalledLanguages();
	amis::StringModuleMap::iterator it = p_langs->begin();
	//increment the iterator to go to the same number as the combo selection
	for (int i = 0; i<sel; i++) it++;
	mUiLanguageSelection = it->first;

	OnBnClickedIsSelfVoicing();
}
void PreferencesDialog::OnCbnSetfocusInstalledLanguages()
{
	OnBnClickedIsSelfVoicing();
}
void PreferencesDialog::OnBnSetfocusDisableScreensaver()
{
}
void PreferencesDialog::OnBnClickedDisableScreensaver()
{
	OnBnClickedIsSelfVoicing();
}