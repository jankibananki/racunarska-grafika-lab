
// RacunarskaGrafikaLab1-PripremaView.h : interface of the CRacunarskaGrafikaLab1PripremaView class
//

#pragma once


class CRacunarskaGrafikaLab1PripremaView : public CView
{
protected: // create from serialization only
	CRacunarskaGrafikaLab1PripremaView() noexcept;
	DECLARE_DYNCREATE(CRacunarskaGrafikaLab1PripremaView)

// Attributes
public:
	CRacunarskaGrafikaLab1PripremaDoc* GetDocument() const;
	bool m_showGrid = true;

// Operations
protected:
	void DrawPolygon(CDC* pDC, CPoint* points, int n);
	void DrawRegularPolygon(CDC* pDC, int cx, int cy, int r, int n, float rotAngle);
	void DrawGrid(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

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
	virtual ~CRacunarskaGrafikaLab1PripremaView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in RacunarskaGrafikaLab1-PripremaView.cpp
inline CRacunarskaGrafikaLab1PripremaDoc* CRacunarskaGrafikaLab1PripremaView::GetDocument() const
   { return reinterpret_cast<CRacunarskaGrafikaLab1PripremaDoc*>(m_pDocument); }
#endif

