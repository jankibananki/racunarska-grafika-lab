
// IND_19728View.h : interface of the CIND19728View class
//

#pragma once


class CIND19728View : public CView
{
protected: // create from serialization only
	CIND19728View() noexcept;
	DECLARE_DYNCREATE(CIND19728View)

// Attributes
public:
	CIND19728Doc* GetDocument() const;

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
	virtual ~CIND19728View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in IND_19728View.cpp
inline CIND19728Doc* CIND19728View::GetDocument() const
   { return reinterpret_cast<CIND19728Doc*>(m_pDocument); }
#endif

