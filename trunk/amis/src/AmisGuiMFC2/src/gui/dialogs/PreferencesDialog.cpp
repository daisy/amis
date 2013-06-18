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
#include "gui/AmisApp.h"
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

#include "gui/self-voicing/dialogs/PreferencesDialogVoicing.h"

using namespace amis::gui::dialogs;
using namespace amis::gui::spoken;

amis::gui::dialogs::PreferencesDialogVoicing * mpPreferencesDialogVoicing = NULL;

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

void PreferencesDialog::resolvePromptVariables(Prompt* pPrompt)
{
	mpPreferencesDialogVoicing->resolvePromptVariables(pPrompt);
	AmisDialogBase::resolvePromptVariables(pPrompt);
}

PreferencesDialog::PreferencesDialog(CWnd* pParent /*=NULL*/)
	: AmisDialogBase(PreferencesDialog::IDD)
{
	if (Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mpPreferencesDialogVoicing = new amis::gui::dialogs::PreferencesDialogVoicing(this);
	}
	else
	{
		mpPreferencesDialogVoicing = NULL;
	}
}

PreferencesDialog::~PreferencesDialog()
{
	if (mpPreferencesDialogVoicing != NULL)
	{
		delete mpPreferencesDialogVoicing;
		mpPreferencesDialogVoicing = NULL;
	}
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
	//disable self-voicing if we have no TTS and no DirectX
	if (Preferences::Instance()->getMustAvoidTTS() && Preferences::Instance()->getMustAvoidDirectX()) 
	{ 
		CButton* p_self_voicing = (CButton*)this->GetDlgItem(IDC_ISSELFVOICING); 
		p_self_voicing->EnableWindow(0); 
	} 
	//disable the TTS stuff if we're running without TTS
	if (Preferences::Instance()->getMustAvoidTTS())
	{
		CComboBox* p_tts_list = (CComboBox*)this->GetDlgItem(IDC_TTSVOICES); 
		p_tts_list->EnableWindow(0); 
		
		CStatic* p_label = (CStatic*)this->GetDlgItem(IDC_SELTTSLABEL); 
		p_label->EnableWindow(0); 
	}

	mbIsSelfVoicing = Preferences::Instance()->getIsSelfVoicing();
	mbPauseOnLostFocus = Preferences::Instance()->getPauseOnLostFocus();
	mbLoadLastBook = Preferences::Instance()->getLoadLastBook();
	mbStartInBasicView = Preferences::Instance()->getStartInBasicView();
	mbHighlightText = Preferences::Instance()->getHighlightText();
	mUiLanguageSelection = Preferences::Instance()->getUiLangId();
	mbDisableScreensaver = Preferences::Instance()->getDisableScreensaver();
	
	CDialog::OnInitDialog();
	this->setFontOnAllControls();

	initializeTTSVoiceOption();

	initializeUiLanguageOption();
	
	return TRUE;
}

void PreferencesDialog::initializeTTSVoiceOption()
{
	if (Preferences::Instance()->getMustAvoidTTS()) return;

	// Make sure to initialize the voice (just in case TTSPlayer has been unused until now)
	amis::tts::TTSPlayer::InstanceOne()->SetVoice(Preferences::Instance()->getTTSVoiceIndex());
	amis::tts::TTSPlayer::InstanceTwo()->SetVoice(Preferences::Instance()->getTTSVoiceIndex());

	mTTSVoiceIndex = 0;
	//fill the combo box for the tts voices, and return an index to the voice we are currently using
	//note that both instances of the tts player are assumed to use the same voice, so this code need only
	//refer to InstanceOne
	int index = amis::tts::TTSPlayer::InstanceTwo()->initVoiceList(this->GetSafeHwnd());
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
	if (Preferences::Instance()->getIsSelfVoicing() == false) 
		return CDialog::PreTranslateMessage(pMsg);

	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		CWnd* cwnd = this->GetFocus();
		if (cwnd) 
		{
			mCommonPreTranslateMessageHandler->handle
				(this, pMsg, (cwnd == NULL ? -1 : cwnd->GetDlgCtrlID()), false, true);
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
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
	if (Preferences::Instance()->getMustAvoidTTS()) return;

	CComboBox* list = (CComboBox*)GetDlgItem(IDC_TTSVOICES);
	if (list == NULL) return;

	int selected = list->GetCurSel();
	mTTSVoiceIndex = selected;

	if (Preferences::Instance()->getIsSelfVoicing() == true)
		mpPreferencesDialogVoicing->playNoVerboseList();

	//Bypassing the default behaviour to uses less verbosity
	//OnBnClickedIsSelfVoicing();
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

	if (Preferences::Instance()->getIsSelfVoicing() == true)
		mpPreferencesDialogVoicing->playNoVerboseList();

	//Bypassing the default behaviour to uses less verbosity
	//OnBnClickedIsSelfVoicing();
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