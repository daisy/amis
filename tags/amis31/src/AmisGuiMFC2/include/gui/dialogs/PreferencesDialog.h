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
#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include "gui/dialogs/AmisDialogBase.h"
#include "../resource.h"
#include "AmisCore.h"
#include "Media.h"

namespace amis
{
namespace gui
{
namespace dialogs
{
class PreferencesDialog : public AmisDialogBase
{
//friend class amis::gui::spoken::PreferencesDialogVoicing;
friend class PreferencesDialogVoicing;

public:
	void resolvePromptVariables(Prompt*);

	PreferencesDialog(CWnd* pParent = NULL);
	~PreferencesDialog();
	virtual BOOL PreTranslateMessage(MSG*);
	string getUiLangSelection();

	enum { IDD = IDD_PREFERENCES };

	BOOL mbIsSelfVoicing;
	BOOL mbPauseOnLostFocus;
	BOOL mbLoadLastBook;
	BOOL mbStartInBasicView;
	BOOL mbHighlightText;
	BOOL mbDisableScreensaver;
	int	mTTSVoiceIndex;
	string mUiLanguageSelection;

protected:
	afx_msg void OnBnClickedIsSelfVoicing();
	afx_msg void OnBnSetfocusIsSelfVoicing();
	afx_msg void OnCbnSelchangeTTSVoices();
	afx_msg void OnCbnSetfocusTTSVoices();
	afx_msg void OnBnClickedStartInBasicView();
	afx_msg void OnBnSetfocusStartInBasicView();
	afx_msg void OnBnClickedLoadLastBook();
	afx_msg void OnBnSetfocusLoadLastBook();
	afx_msg void OnBnSetfocusHighlightText();
	afx_msg void OnBnClickedHighlightText();
	afx_msg void OnCbnSelchangeInstalledLanguages();
	afx_msg void OnCbnSetfocusInstalledLanguages();
	afx_msg void OnBnClickedDisableScreensaver();
	afx_msg void OnBnSetfocusDisableScreensaver();
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual void DoDataExchange(CDataExchange*);
	DECLARE_MESSAGE_MAP()
private:
	
	void initializeTTSVoiceOption();
	void initializeUiLanguageOption();
	void setCurrentLanguageLabel();
	void findAllLanguagePacks();
};
}
}
}
#endif
