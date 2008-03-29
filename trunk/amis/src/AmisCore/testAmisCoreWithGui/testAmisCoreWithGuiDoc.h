// testAmisCoreWithGuiDoc.h : interface of the CtestAmisCoreWithGuiDoc class
//


#pragma once

class CtestAmisCoreWithGuiDoc : public CDocument
{
protected: // create from serialization only
	CtestAmisCoreWithGuiDoc();
	DECLARE_DYNCREATE(CtestAmisCoreWithGuiDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CtestAmisCoreWithGuiDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


