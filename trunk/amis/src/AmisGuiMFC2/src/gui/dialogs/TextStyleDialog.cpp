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

#include "stdafx.h"

#include "gui/dialogs/TextStyleDialog.h"
//#include "..\..\..\include\gui\dialogs\textstyledialog.h"

#include "../resource.h"
#include "AmisCore.h"
#include "util/Color.h"
#include "Preferences.h"

#include "gui/self-voicing/dialogs/TextStyleDialogVoicing.h"

using namespace amis::gui::dialogs;

using namespace amis::gui::spoken;

amis::gui::dialogs::TextStyleDialogVoicing * mpTextStyleDialogVoicing = NULL;

BEGIN_MESSAGE_MAP(TextStyleDialog, CDialog)
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_CBN_SELCHANGE(IDC_FONT, OnCbnSelchangeFont)
	ON_CBN_SETFOCUS(IDC_FONT, OnCbnSetfocusFont)
	ON_CBN_SELCHANGE(IDC_HIGHLIGHTFOREGROUND, OnCbnSelchangeHighlightForeground)
	ON_CBN_SETFOCUS(IDC_HIGHLIGHTFOREGROUND, OnCbnSetfocusHighlightForeground)
	ON_CBN_SELCHANGE(IDC_HIGHLIGHTBACKGROUND, OnCbnSelchangeHighlightBackground)
	ON_CBN_SETFOCUS(IDC_HIGHLIGHTBACKGROUND, OnCbnSetfocusHighlightBackground)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void TextStyleDialog::resolvePromptVariables(Prompt* pPrompt)
{
	mpTextStyleDialogVoicing->resolvePromptVariables(pPrompt);
	AmisDialogBase::resolvePromptVariables(pPrompt);
}
void TextStyleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
BOOL CALLBACK EnumFontCallback (LPLOGFONT lplf, LPTEXTMETRIC lptm, DWORD dwType, LPARAM lpData)	
{	
	USES_CONVERSION;
	CComboBox* p_font_list = (CComboBox*)lpData;
	int index = p_font_list->AddString(lplf->lfFaceName);
	std::string font = T2A(lplf->lfFaceName);
	if (font.compare(amis::Preferences::Instance()->getAppFontName()) == 0)
	{
		p_font_list->SetCurSel(index);
	}
	return TRUE;
}

TextStyleDialog::TextStyleDialog(CWnd* pParent /*=NULL*/)
	: AmisDialogBase(TextStyleDialog::IDD)
{
	if (Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mpTextStyleDialogVoicing = new amis::gui::dialogs::TextStyleDialogVoicing(this);
	}
}
TextStyleDialog::~TextStyleDialog()
{
	if (mpTextStyleDialogVoicing != NULL)
	{
		delete mpTextStyleDialogVoicing;
		mpTextStyleDialogVoicing = NULL;
	}
}
BOOL TextStyleDialog::OnInitDialog() 
{	
	CDialog::OnInitDialog();
	this->setFontOnAllControls();
	initializeCombos();
	
	return TRUE;
}
void TextStyleDialog::initializeCombos()
{
	USES_CONVERSION;

	CComboBox* p_foreground_list = (CComboBox*)GetDlgItem(IDC_HIGHLIGHTFOREGROUND);
	CComboBox* p_background_list = (CComboBox*)GetDlgItem(IDC_HIGHLIGHTBACKGROUND);

	//workaround: color map is not static
	util::Color dummy_color;
	amis::StringMap* p_all_colors = dummy_color.getColorHexMap(); 
	
	//the current values all come from Preferences
	int foreground_sel = -1;
	int background_sel = -1;
	int i = 0;
	amis::StringMap::iterator it = p_all_colors->begin();
	std::string highlight_bg = Preferences::Instance()->getHighlightBGColor().getAsHtmlHexColor();
	std::string highlight_fg = Preferences::Instance()->getHighlightFGColor().getAsHtmlHexColor();
	while (it != p_all_colors->end())
	{
		CString label = A2T(it->first.c_str());
		p_foreground_list->AddString(label);
		p_background_list->AddString(label);
		if (it->second == highlight_bg)
			background_sel = i;
		if (it->second == highlight_fg)
			foreground_sel = i;
		i++;
		it++;
	}
	p_foreground_list->SetCurSel(foreground_sel);
	p_background_list->SetCurSel(background_sel);
	
	CClientDC dc(this);	
	EnumFonts (dc, 0,(FONTENUMPROC)EnumFontCallback,(LPARAM)this->GetDlgItem(IDC_FONT));
}

void TextStyleDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Let the window do its default painting...
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );
	//call the base class on_paint function
	on_paint();
}

BOOL TextStyleDialog::PreTranslateMessage(MSG* pMsg)
{
	return AmisDialogBase::PreTranslateMessage(pMsg);
}

//widget event handlers
void TextStyleDialog::OnCbnSelchangeFont()
{
	CWnd* cwnd = this->GetFocus();
	CComboBox* p = (CComboBox*)GetDlgItem(IDC_FONT);
	int sel = p->GetCurSel();
TRACE("SEL: %d", sel);
	CString tmp;
	p->GetWindowText(tmp);
	p->GetLBText(sel, tmp);
TRACE(tmp);
	triggerVirtualKeyStroke(p);
}
void TextStyleDialog::OnCbnSetfocusFont()
{
	OnCbnSelchangeFont();
}
void TextStyleDialog::OnCbnSelchangeHighlightForeground()
{
	CWnd* cwnd = this->GetFocus();
	CComboBox* p = (CComboBox*)GetDlgItem(IDC_HIGHLIGHTFOREGROUND);
	triggerVirtualKeyStroke(p);
}
void TextStyleDialog::OnCbnSetfocusHighlightForeground()
{
	OnCbnSelchangeHighlightForeground();
}
void TextStyleDialog::OnCbnSelchangeHighlightBackground()
{
	CWnd* cwnd = this->GetFocus();
	CComboBox* p = (CComboBox*)GetDlgItem(IDC_HIGHLIGHTBACKGROUND);
	triggerVirtualKeyStroke(p);
}
void TextStyleDialog::OnCbnSetfocusHighlightBackground()
{
	OnCbnSelchangeHighlightBackground();
}
void amis::gui::dialogs::TextStyleDialog::OnOK()
{
	USES_CONVERSION;
	CString tmp;
	CComboBox* p_font_list = (CComboBox*)GetDlgItem(IDC_FONT);
	p_font_list->GetWindowText(tmp);
	Preferences::Instance()->setAppFontName(T2A(tmp));
	
	CComboBox* p_background_list = (CComboBox*)GetDlgItem(IDC_HIGHLIGHTBACKGROUND);
	p_background_list->GetWindowText(tmp);
	if (tmp == "") tmp = "yellow";
	amis::util::Color bg_clr;
	bg_clr.setWithHtmlName(T2A(tmp));
	Preferences::Instance()->setHighlightBGColor(bg_clr);
	
	CComboBox* p_foreground_list = (CComboBox*)GetDlgItem(IDC_HIGHLIGHTFOREGROUND);
	p_foreground_list->GetWindowText(tmp);
	if (tmp == "") tmp = "black";
	amis::util::Color fg_clr;
	fg_clr.setWithHtmlName(T2A(tmp));
	Preferences::Instance()->setHighlightFGColor(fg_clr);

	AmisDialogBase::OnOK();
}
