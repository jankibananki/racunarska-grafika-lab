
// RacunarskaGrafikaLab1-PripremaView.cpp : implementation of the CRacunarskaGrafikaLab1PripremaView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "RacunarskaGrafikaLab1-Priprema.h"
#endif

#include "RacunarskaGrafikaLab1-PripremaDoc.h"
#include "RacunarskaGrafikaLab1-PripremaView.h"
#include <cmath>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRacunarskaGrafikaLab1PripremaView

IMPLEMENT_DYNCREATE(CRacunarskaGrafikaLab1PripremaView, CView)

BEGIN_MESSAGE_MAP(CRacunarskaGrafikaLab1PripremaView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CRacunarskaGrafikaLab1PripremaView construction/destruction

CRacunarskaGrafikaLab1PripremaView::CRacunarskaGrafikaLab1PripremaView() noexcept
{
	// TODO: add construction code here

}

CRacunarskaGrafikaLab1PripremaView::~CRacunarskaGrafikaLab1PripremaView()
{
}

BOOL CRacunarskaGrafikaLab1PripremaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CRacunarskaGrafikaLab1PripremaView drawing

void CRacunarskaGrafikaLab1PripremaView::DrawPolygon(CDC* pDC, CPoint* points, int n) // znaci prosledjuje se kontekst uredjaja preko koga crtamo, pokazivac niza temena, i broj temena
{
	pDC->Polygon(points, n); //->crta i popunjava mnogougao
}

void CRacunarskaGrafikaLab1PripremaView::DrawRegularPolygon(CDC* pDC, int cx, int cy, int r, int n, float rotAngle)
{
	//potrebno je da funkcija sama izracuna koordinate pomocu sinusa i kosinusa
	// NAPOMENA: CPP KORISTI RADIJANE NE STEPENE

	if (n < 3 || r <= 0)
		return; //ne moze da se nacrta mnogougao u ovom slucaju

	const double PI = 3.141592653589793;
	double step = 2 * PI / n; // 360 = 2pi
	double rotation = rotAngle * PI / 180;

	std::vector<CPoint> points(n);

	for (int i = 0; i < n; i++)
	{
		double alpha = i * step + rotation; // rootAngle se koristi za rotaciju, da se zarotira za odredjeni ugao
		int x = (int)round(cx + r * cos(alpha));
		int y = (int)round(cy + r * sin(alpha));

		points[i] = CPoint(x, y);
	}

	DrawPolygon(pDC, points.data(), n);
	//cx i cy su centar mnogougla
	//r je poluprecnik opisane kruznice
	//i angle ugao trenutnog temena
}

void CRacunarskaGrafikaLab1PripremaView::DrawGrid(CDC* pDC)
{
	CPen pen(PS_SOLID, 1, RGB(200, 200, 200));
	CPen* oldPen = pDC->SelectObject(&pen);

	for (int i = 0; i <= 500; i += 25)
	{
		pDC->MoveTo(i, 0);
		pDC->LineTo(i, 500);

		pDC->MoveTo(0, i);
		pDC->LineTo(500, i);
	}

	pDC->SelectObject(oldPen);
}

void CRacunarskaGrafikaLab1PripremaView::OnKeyDown(
	UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'G')
	{
		m_showGrid = !m_showGrid;
		Invalidate();
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CRacunarskaGrafikaLab1PripremaView::OnDraw(CDC* pDC)
{
    CRacunarskaGrafikaLab1PripremaDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // TODO: add draw code for native data here

    //pdc je pokazivac na device context odnosno objekat preko kojeg
    //gdi zna gde i kako da crta 

    /*
    //fje se pozivaju preko pokazivaca
    pDC->MoveTo(100, 100); //"pomera olovku al ne crta nista jos"
    pDC->LineTo(300, 300); //"povlaci liniju od pocetne do zadate tacke"
    pDC->LineTo(100, 300);
    pDC->LineTo(100, 100);*/
    //a da se ne bi crtala svaka linija posebno moze da se koristi poligon

    // Cuvamo pocetno stanje DC-a
    int savedDC = pDC->SaveDC();

    CPen pen(PS_SOLID, 3, RGB(255, 0, 0)); // -> kreiranje olovke: puna linija, debljine tri crvena boja
    CBrush brush1(RGB(255, 255, 255)); // -> kreiranje cetkice: boja zuta
    //CBrush brush(HS_VERTICAL, RGB(0, 0, 255));

    CPen* oldPen = pDC->SelectObject(&pen);
    CBrush* oldBrush = pDC->SelectObject(&brush1); // selektovanje olovke i cetkice - govorimo GDI-u da koristi nase umesto podrazumevanih

    // SPOLJNI KVADRAT - BELA PODLOGA
    pDC->Rectangle(0, 0, 500, 500);

    // LJUBICASTI KVADRAT
    CBrush brush2(RGB(153, 0, 204));
    pDC->SelectObject(&brush2);

    DrawRegularPolygon(pDC, 250, 125, 125, 4, 0);

    // ROZE TROUGAO
    CBrush brush3(RGB(255, 153, 204));
    pDC->SelectObject(&brush3);

    CPoint points1[3] =
    {
        CPoint(0,0),
        CPoint(250,0),
        CPoint(125,125)
    };

    DrawPolygon(pDC, points1, 3);

    // CRVENI TROUGAO
    CBrush brush4(RGB(255, 0, 0));
    pDC->SelectObject(&brush4);

    CPoint points2[3] =
    {
        CPoint(0,0),
        CPoint(250,250),
        CPoint(0,500)
    };

    DrawPolygon(pDC, points2, 3);

    // DONJI BELI TROUGAO SA PLAVOM SRAFUROM
    CBrush brush5(HS_VERTICAL, RGB(0, 0, 255));
    pDC->SelectObject(&brush5);

    pDC->SetBkColor(RGB(255, 255, 255));
    pDC->SetBkMode(OPAQUE);

    CPoint points3[3] =
    {
        CPoint(0,500),
        CPoint(250,250),
        CPoint(500,500)
    };

    DrawPolygon(pDC, points3, 3);

    pDC->SetBkColor(RGB(255, 255, 255));
    pDC->SetBkMode(TRANSPARENT);

    // Osmougao cemo nacrtati na kraju,
    // kada zavrsimo sve obojene figure.

    // ZUTI TROUGAO
    CBrush brush6(RGB(255, 255, 0));
    pDC->SelectObject(&brush6);

    CPoint points4[3] =
    {
        CPoint(250,250),
        CPoint(375,125),
        CPoint(375,375)
    };

    DrawPolygon(pDC, points4, 3);

    pDC->SetBkColor(RGB(255, 255, 255));
    pDC->SetBkMode(TRANSPARENT);

    // Petougao cemo takodje nacrtati na kraju.

    // ZELENI TROUGAO
    CBrush brush7(RGB(29, 180, 29));
    pDC->SelectObject(&brush7);

    CPoint points5[3] =
    {
        CPoint(250,0),
        CPoint(500,0),
        CPoint(500,250)
    };

    DrawPolygon(pDC, points5, 3);

    // NARANDZASTI POLIGON
    CBrush brush8(RGB(255, 140, 0));
    pDC->SelectObject(&brush8);

    CPoint points6[4] =
    {
        CPoint(375,125),
        CPoint(500,250),
        CPoint(500,500),
        CPoint(375,375)
    };

    DrawPolygon(pDC, points6, 4);


    //pDC->Polygon(points, 3);
    //DrawPolygon(pDC,points, 4);
    //pDC->SetBkColor(RGB(255, 255, 255));
    //pDC->SetBkMode(OPAQUE);

    //DrawRegularPolygon(pDC, 250, 250, 150, 6, 0);

    // GRID PREKO OBOJENIH FIGURA
    if (m_showGrid)
        DrawGrid(pDC);

    // MALI PRAVILNI MNOGOUGLOVI
    // NULL_BRUSH = samo kontura, bez popunjavanja
    pDC->SelectStockObject(NULL_BRUSH);

    // Sedmougao u roze delu
    DrawRegularPolygon(pDC, 125, 55, 27, 7, -90);

    // Sestougao u zelenom delu
    DrawRegularPolygon(pDC, 425, 75, 37, 6, 0);

    // Petougao u zutom delu
    DrawRegularPolygon(pDC, 325, 250, 25, 5, -90);

    // Osmougao u donjem belom delu
    DrawRegularPolygon(pDC, 250, 400, 55, 8, -90);

    // SPOLJNI CRVENI OKVIR
    pDC->Rectangle(0, 0, 500, 500);

    // Vracamo prethodno stanje DC-a
    pDC->RestoreDC(savedDC);
}


// CRacunarskaGrafikaLab1PripremaView printing

BOOL CRacunarskaGrafikaLab1PripremaView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CRacunarskaGrafikaLab1PripremaView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CRacunarskaGrafikaLab1PripremaView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CRacunarskaGrafikaLab1PripremaView diagnostics

#ifdef _DEBUG
void CRacunarskaGrafikaLab1PripremaView::AssertValid() const
{
	CView::AssertValid();
}

void CRacunarskaGrafikaLab1PripremaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRacunarskaGrafikaLab1PripremaDoc* CRacunarskaGrafikaLab1PripremaView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRacunarskaGrafikaLab1PripremaDoc)));
	return (CRacunarskaGrafikaLab1PripremaDoc*)m_pDocument;
}
#endif //_DEBUG


// CRacunarskaGrafikaLab1PripremaView message handlers

/*Za nepravilne figure :
CPoint points[] = {
	CPoint(0, 0),
	CPoint(250, 250),
	CPoint(0, 500)
};
DrawPolygon(pDC, points, 3);


Za pravilne :
DrawRegularPolygon(pDC, 250, 125, 125, 4, 0);*/