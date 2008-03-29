// testAmisCoreWithGuiView.cpp : implementation of the CtestAmisCoreWithGuiView class
//

#include "stdafx.h"
#include "testAmisCoreWithGui.h"

#include "testAmisCoreWithGuiDoc.h"
#include "testAmisCoreWithGuiView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CtestAmisCoreWithGuiView

IMPLEMENT_DYNCREATE(CtestAmisCoreWithGuiView, CView)

BEGIN_MESSAGE_MAP(CtestAmisCoreWithGuiView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CtestAmisCoreWithGuiView construction/destruction

CtestAmisCoreWithGuiView::CtestAmisCoreWithGuiView()
{
	// add construction code here

}

CtestAmisCoreWithGuiView::~CtestAmisCoreWithGuiView()
{
}

BOOL CtestAmisCoreWithGuiView::PreCreateWindow(CREATESTRUCT& cs)
{
	//  Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CtestAmisCoreWithGuiView drawing

void CtestAmisCoreWithGuiView::OnDraw(CDC* /*pDC*/)
{
	CtestAmisCoreWithGuiDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// add draw code for native data here
}


// CtestAmisCoreWithGuiView printing

BOOL CtestAmisCoreWithGuiView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CtestAmisCoreWithGuiView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// add extra initialization before printing
}

void CtestAmisCoreWithGuiView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// add cleanup after printing
}


// CtestAmisCoreWithGuiView diagnostics

#ifdef _DEBUG
void CtestAmisCoreWithGuiView::AssertValid() const
{
	CView::AssertValid();
}

void CtestAmisCoreWithGuiView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CtestAmisCoreWithGuiDoc* CtestAmisCoreWithGuiView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CtestAmisCoreWithGuiDoc)));
	return (CtestAmisCoreWithGuiDoc*)m_pDocument;
}
#endif //_DEBUG


// CtestAmisCoreWithGuiView message handlers
