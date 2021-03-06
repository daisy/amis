/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

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
#ifndef MULTIPLEBOOKSONVOLUMEDIALOG_H
#define MULTIPLEBOOKSONVOLUMEDIALOG_H

#include "gui/dialogs/AmisDialogBase.h"
#include "../resource.h"
#include "ambulant/net/url.h"
#include "AmisCore.h"
#include "BookList.h"

namespace amis
{
namespace gui
{
namespace dialogs
{
class MultipleBooksOnVolumeDialog : public AmisDialogBase
{
	friend class MultipleBooksOnVolumeDialogVoicing;

public:
	void resolvePromptVariables(Prompt*);

	//the pBookList parameter has a default value only because the compiler requires it
	MultipleBooksOnVolumeDialog(CWnd* pParent = NULL, amis::BookList* pBookList = NULL);
	~MultipleBooksOnVolumeDialog();
	ambulant::net::url getBookToLoad();
	enum { IDD = IDD_CDSEARCH };
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkFilelist();
	afx_msg void OnOpenbook();
	afx_msg void OnPaint();
	afx_msg void OnSelchangeFilelist();
	DECLARE_MESSAGE_MAP()

private:
	void populateListControl();
	void loadBook();

	ambulant::net::url mLoadBookOnDialogClose;
	amis::BookList* mpBookList;
};
}
}
}
#endif
