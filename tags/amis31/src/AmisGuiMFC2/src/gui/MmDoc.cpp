/*
 * 
 * This file is part of Ambulant Player, www.ambulantplayer.org.
 * 
 * Copyright (C) 2003 Stiching CWI, 
 * Kruislaan 413, 1098 SJ Amsterdam, The Netherlands.
 * 
 * Ambulant Player is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Ambulant Player is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Ambulant Player; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * In addition, as a special exception, if you link Ambulant Player with
 * other files to produce an executable, this library does not by itself
 * cause the resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other reason why
 * the executable file might be covered by the GNU General Public License.
 * 
 * As a special exception, the copyright holders of Ambulant Player give
 * you permission to link Ambulant Player with independent modules that
 * communicate with Ambulant Player solely through the region and renderer
 * interfaces, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting combined work under
 * terms of your choice, provided that every copy of the combined work is
 * accompanied by a complete copy of the source code of Ambulant Player
 * (the version of Ambulant Player used to produce the combined work),
 * being distributed under the terms of the GNU General Public License plus
 * this exception.  An independent module is a module which is not derived
 * from or based on Ambulant Player.
 * 
 * Note that people who make modified versions of Ambulant Player are not
 * obligated to grant this special exception for their modified versions;
 * it is their choice whether to do so.  The GNU General Public License
 * gives permission to release a modified version without this exception;
 * this exception also makes it possible to release a modified version
 * which carries forward this exception. 
 * 
 */

// MmDoc.cpp : implementation of the MmDoc class
//

#include "stdafx.h"

#include "gui/MmDoc.h"
#include "gui/MmView.h"
#include "gui/MainWndParts.h"
#include "gui/AmisApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace amis::gui;

// MmDoc

IMPLEMENT_DYNCREATE(MmDoc, CDocument)

BEGIN_MESSAGE_MAP(MmDoc, CDocument)
END_MESSAGE_MAP()


// MmDoc construction/destruction

MmDoc::MmDoc()
{
	ASSERT(MainWndParts::Instance()->mpMmDoc == NULL);
	MainWndParts::Instance()->mpMmDoc = this;
	m_autostart = false;
}

MmDoc::~MmDoc()
{
}

BOOL MmDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument()) return FALSE;
	return TRUE;
}




// MmDoc serialization

void MmDoc::Serialize(CArchive& ar)
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


// MmDoc diagnostics

#ifdef _DEBUG
void MmDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void MmDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// MmDoc commands

BOOL MmDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	//med 21 June 07
	//this is a hack to get around the fact that windows will automatically invoke this
	//OnOpenDocument handler when it does command line processing (even though I wish it wouldn't)
	if (theApp.shouldIgnoreOpenDocEvent() == true) return TRUE;

	POSITION pos = GetFirstViewPosition();
	if(pos != NULL) 
	{
		CView* pView = GetNextView(pos);
		ASSERT_VALID(pView);
		((MmView*)pView)->SetMMDocument(lpszPathName, m_autostart);
	}
	return TRUE;
}

void MmDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) 
{
	if(_tcsstr(lpszPathName, _T("://")) != 0) 
	{
		// seems a url
		m_strPathName = "URL";
		m_bEmbedded = FALSE;
		SetTitle(_T("URL"));
	} 
	else 
	{
		CDocument::SetPathName(lpszPathName, bAddToMRU);
	}
}

void MmDoc::StartPlayback() 
{
	POSITION pos = GetFirstViewPosition();
	if(pos != NULL) 
	{
		CView* pView = GetNextView(pos);
		ASSERT_VALID(pView);
		((MmView*)pView)->OnFilePlay();
	}
}
