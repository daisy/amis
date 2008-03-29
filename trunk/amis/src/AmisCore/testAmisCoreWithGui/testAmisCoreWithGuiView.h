// testAmisCoreWithGuiView.h : interface of the CtestAmisCoreWithGuiView class
//


#pragma once


class CtestAmisCoreWithGuiView : public CView
{
protected: // create from serialization only
	CtestAmisCoreWithGuiView();
	DECLARE_DYNCREATE(CtestAmisCoreWithGuiView)

// Attributes
public:
	CtestAmisCoreWithGuiDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CtestAmisCoreWithGuiView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in testAmisCoreWithGuiView.cpp
inline CtestAmisCoreWithGuiDoc* CtestAmisCoreWithGuiView::GetDocument() const
   { return reinterpret_cast<CtestAmisCoreWithGuiDoc*>(m_pDocument); }
#endif

