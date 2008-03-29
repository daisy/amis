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

#include "stdafx.h"
#include "gui/AmisApp.h"

#include "gui/HtmlDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace amis::gui;

IMPLEMENT_DYNCREATE(CAmisHtmlDoc, CDocument)

BEGIN_MESSAGE_MAP(CAmisHtmlDoc, CDocument)
END_MESSAGE_MAP()

CAmisHtmlDoc::CAmisHtmlDoc()
{
}

CAmisHtmlDoc::~CAmisHtmlDoc()
{
}

BOOL CAmisHtmlDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument()) return FALSE;
	// add reinitialization code here (SDI documents will reuse this document)
	return TRUE;
}

void CAmisHtmlDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// add storing code here
	}
	else
	{
		// add loading code here
	}
}

#ifdef _DEBUG
void CAmisHtmlDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAmisHtmlDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
