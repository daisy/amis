// testAmisCoreWithGuiDoc.cpp : implementation of the CtestAmisCoreWithGuiDoc class
//

#include "stdafx.h"
#include "testAmisCoreWithGui.h"

#include "testAmisCoreWithGuiDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CtestAmisCoreWithGuiDoc

IMPLEMENT_DYNCREATE(CtestAmisCoreWithGuiDoc, CDocument)

BEGIN_MESSAGE_MAP(CtestAmisCoreWithGuiDoc, CDocument)
END_MESSAGE_MAP()


// CtestAmisCoreWithGuiDoc construction/destruction

CtestAmisCoreWithGuiDoc::CtestAmisCoreWithGuiDoc()
{
	// add one-time construction code here

}

CtestAmisCoreWithGuiDoc::~CtestAmisCoreWithGuiDoc()
{
}

BOOL CtestAmisCoreWithGuiDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CtestAmisCoreWithGuiDoc serialization

void CtestAmisCoreWithGuiDoc::Serialize(CArchive& ar)
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


// CtestAmisCoreWithGuiDoc diagnostics

#ifdef _DEBUG
void CtestAmisCoreWithGuiDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CtestAmisCoreWithGuiDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CtestAmisCoreWithGuiDoc commands
