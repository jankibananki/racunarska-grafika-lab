
// IND_19728View.cpp : implementation of the CIND19728View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "IND_19728.h"
#endif

#include "IND_19728Doc.h"
#include "IND_19728View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIND19728View

IMPLEMENT_DYNCREATE(CIND19728View, CView)

BEGIN_MESSAGE_MAP(CIND19728View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CIND19728View construction/destruction

CIND19728View::CIND19728View() noexcept
{
	// TODO: add construction code here

}

CIND19728View::~CIND19728View()
{
}

BOOL CIND19728View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CIND19728View drawing

void CIND19728View::OnDraw(CDC* /*pDC*/)
{
	CIND19728Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CIND19728View printing

BOOL CIND19728View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CIND19728View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CIND19728View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CIND19728View diagnostics

#ifdef _DEBUG
void CIND19728View::AssertValid() const
{
	CView::AssertValid();
}

void CIND19728View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CIND19728Doc* CIND19728View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIND19728Doc)));
	return (CIND19728Doc*)m_pDocument;
}
#endif //_DEBUG


// CIND19728View message handlers
