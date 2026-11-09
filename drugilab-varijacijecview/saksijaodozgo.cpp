
#include "pch.h"
#include "framework.h"

#ifndef SHARED_HANDLERS
#include "IND_19728.h"
#endif

#include "IND_19728Doc.h"
#include "IND_19728View.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CIND19728View, CView)

BEGIN_MESSAGE_MAP(CIND19728View, CView)
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    ON_WM_CHAR()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CIND19728View::CIND19728View() noexcept
{
}

CIND19728View::~CIND19728View()
{
}

BOOL CIND19728View::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

// Glavno crtanje

void CIND19728View::OnDraw(CDC* pDC)
{
    CIND19728Doc* pDoc = GetDocument();

    ASSERT_VALID(pDoc);

    if (!pDoc)
        return;

    int savedDC = pDC->SaveDC();

    pDC->SetGraphicsMode(GM_ADVANCED);

    DrawFigure(pDC);

    // Mreza 500x500, korak 25
    if (m_showGrid)
    {
        CPen gridPen(
            PS_SOLID, 1, RGB(224, 224, 224));

        CPen* oldPen = pDC->SelectObject(&gridPen);

        for (int x = 0; x <= 500; x += 25)
        {
            pDC->MoveTo(x, 0);
            pDC->LineTo(x, 500);
        }

        for (int y = 0; y <= 500; y += 25)
        {
            pDC->MoveTo(0, y);
            pDC->LineTo(500, y);
        }

        pDC->SelectObject(oldPen);
    }

    pDC->RestoreDC(savedDC);
}

// Translacija

void CIND19728View::Translate(
    CDC* pDC,
    float dX,
    float dY,
    bool rightMultiply)
{
    XFORM xf;

    xf.eM11 = 1.0f;
    xf.eM12 = 0.0f;
    xf.eM21 = 0.0f;
    xf.eM22 = 1.0f;
    xf.eDx = dX;
    xf.eDy = dY;

    pDC->ModifyWorldTransform(
        &xf,
        rightMultiply
            ? MWT_RIGHTMULTIPLY
            : MWT_LEFTMULTIPLY
    );
}

// Skaliranje

void CIND19728View::Scale(
    CDC* pDC,
    float sX,
    float sY,
    bool rightMultiply)
{
    XFORM xf;

    xf.eM11 = sX;
    xf.eM12 = 0.0f;
    xf.eM21 = 0.0f;
    xf.eM22 = sY;
    xf.eDx = 0.0f;
    xf.eDy = 0.0f;

    pDC->ModifyWorldTransform(
        &xf,
        rightMultiply
            ? MWT_RIGHTMULTIPLY
            : MWT_LEFTMULTIPLY
    );
}

// Rotacija - ugao se prosledjuje u radijanima

void CIND19728View::Rotate(
    CDC* pDC,
    float angle,
    bool rightMultiply)
{
    float c = static_cast<float>(std::cos(angle));
    float s = static_cast<float>(std::sin(angle));

    XFORM xf;

    xf.eM11 = c;
    xf.eM12 = s;
    xf.eM21 = -s;
    xf.eM22 = c;
    xf.eDx = 0.0f;
    xf.eDy = 0.0f;

    pDC->ModifyWorldTransform(
        &xf,
        rightMultiply
            ? MWT_RIGHTMULTIPLY
            : MWT_LEFTMULTIPLY
    );
}

// Ucitavanje i crtanje EMF segmenta

void CIND19728View::DrawMetafilePart(
    CDC* pDC,
    CString filename,
    CRect rect)
{
    HENHMETAFILE hEmf = GetEnhMetaFile(filename);

    if (hEmf != NULL)
    {
        pDC->PlayMetaFile(hEmf, &rect);
        DeleteEnhMetaFile(hEmf);
    }
}

// Crtanje jednog segmenta
// light = true za svetli EMF, false za tamni
// sX - sirina, sY - visina
// joint = true - prelazak do sledeceg spoja

void CIND19728View::DrawSegment(
    CDC* pDC,
    bool light,
    float sX,
    float sY,
    bool joint)
{
    XFORM xOld;
    pDC->GetWorldTransform(&xOld);

    Scale(pDC, sX, sY, false);

    DrawMetafilePart(
        pDC,
        light
            ? _T("cactus_part_light.emf")
            : _T("cactus_part.emf"),
        CRect(-20, -80, 20, 0)
    );

    // Vracamo skaliranje
    pDC->SetWorldTransform(&xOld);

    // Prelazak na naredni spoj
    if (joint)
    {
        Translate(
            pDC,
            0,
            -75.0f * sY,
            false
        );
    }
}

// Crtanje kruznog spoja

void CIND19728View::DrawJoint(CDC* pDC)
{
    pDC->Ellipse(-10, -10, 10, 10);
}

// Crtanje celog kaktusa

void CIND19728View::DrawFigure(CDC* pDC)
{
    const float DEG = PI / 180.0f;

    XFORM xInitial;
    pDC->GetWorldTransform(&xInitial);

    CPen blackPen(
        PS_SOLID, 1, RGB(0, 0, 0));

    CBrush cactusBrush(RGB(0, 204, 0));

    CPen* oldPen = pDC->SelectObject(&blackPen);
    CBrush* oldBrush = pDC->SelectObject(&cactusBrush);

    // Koren kaktusa - pocetak ispod saksije
    Translate(pDC, 250, 75, false);

    // Kaktus raste nadole, prvi ugao rotira celu figuru
    Rotate(
        pDC,
        PI + m_angleSegment1,
        false
    );

    XFORM xRoot;
    pDC->GetWorldTransform(&xRoot);

    DrawJoint(pDC);

    // Veliki gornji segment
    DrawSegment(
        pDC, false, 1.15f, 1.0f, true
    );

    // Prvi veliki cvor J1
    DrawJoint(pDC);

    XFORM xJ1;
    pDC->GetWorldTransform(&xJ1);

    // Gornja leva grana
    {
        Rotate(
            pDC, 45.0f * DEG, false
        );

        // Tanak dijagonalni segment
        DrawSegment(
            pDC, true, 0.45f, 1.0f, true
        );

        DrawJoint(pDC);

        // Rotirajuci veliki levi segment - LEFT / RIGHT
        XFORM xLeft;
        pDC->GetWorldTransform(&xLeft);

        Rotate(
            pDC,
            m_angleSegment2,
            false
        );

        DrawSegment(
            pDC, true, 1.1f, 0.9f, false
        );

        // Rotacija ne utice na ostale grane
        pDC->SetWorldTransform(&xLeft);

        // Povratak u J1
        pDC->SetWorldTransform(&xJ1);
    }

    // Srednja grana
    {
        DrawSegment(
            pDC, true, 0.32f, 1.0f, true
        );

        DrawJoint(pDC);

        // Drugi veliki cvor J4
        XFORM xJ4;
        pDC->GetWorldTransform(&xJ4);

        // Donja leva grana
        {
            Rotate(
                pDC, 45.0f * DEG, false
            );

            DrawSegment(
                pDC, false, 0.7f, 1.0f, true
            );

            DrawJoint(pDC);

            // Trece racvanje J5
            XFORM xJ5;
            pDC->GetWorldTransform(&xJ5);

            // Horizontalno ulevo
            Rotate(
                pDC, 45.0f * DEG, false
            );

            DrawSegment(
                pDC, false, 0.7f, 0.92f, false
            );

            pDC->SetWorldTransform(&xJ5);

            // Uspravno nadole
            Rotate(
                pDC, -45.0f * DEG, false
            );

            DrawSegment(
                pDC, false, 0.65f, 0.85f, false
            );

            pDC->SetWorldTransform(&xJ5);
            pDC->SetWorldTransform(&xJ4);
        }

        // Donja desna grana
        {
            Rotate(
                pDC, -45.0f * DEG, false
            );

            DrawSegment(
                pDC, false, 0.7f, 1.0f, true
            );

            DrawJoint(pDC);

            // Veliki zavrsni segment
            DrawSegment(
                pDC, false, 1.1f, 0.9f, false
            );

            pDC->SetWorldTransform(&xJ4);
        }

        pDC->SetWorldTransform(&xJ1);
    }

    // Gornja desna grana
    {
        Rotate(
            pDC, -45.0f * DEG, false
        );

        // Svetli dijagonalni spoj
        DrawSegment(
            pDC, true, 0.45f, 1.0f, true
        );

        DrawJoint(pDC);

        // Veliki desni segment
        DrawSegment(
            pDC, false, 1.1f, 0.9f, false
        );

        pDC->SetWorldTransform(&xJ1);
    }

    // Vracanje na pocetnu transformaciju
    pDC->SetWorldTransform(&xRoot);
    pDC->SetWorldTransform(&xInitial);

    // Saksija na vrhu
    CPen potPen(
        PS_SOLID, 1, RGB(160, 95, 0));

    CBrush potBrush(RGB(235, 150, 0));
    CBrush lipBrush(RGB(250, 165, 10));

    pDC->SelectObject(&potPen);
    pDC->SelectObject(&potBrush);

    // Telo obrnute saksije
    CPoint pot[4] =
    {
        CPoint(212, 0),
        CPoint(287, 0),
        CPoint(300, 50),
        CPoint(200, 50)
    };

    pDC->Polygon(pot, 4);

    // Donji rub saksije
    pDC->SelectObject(&lipBrush);
    pDC->Rectangle(
        190, 50, 312, 70
    );

    pDC->SelectObject(oldBrush);
    pDC->SelectObject(oldPen);
}

// Obrada tastature

void CIND19728View::OnChar(
    UINT nChar,
    UINT nRepCnt,
    UINT nFlags)
{
    // Jedan pritisak = 30 stepeni
    const float STEP = 30.0f * PI / 180.0f;

    switch (nChar)
    {
        // Ukljucivanje i iskljucivanje mreze
        case 'g':
        case 'G':
            m_showGrid = !m_showGrid;
            break;

        // LEFT - rotacija jednog levog segmenta
        case 'a':
        case 'A':
            m_angleSegment2 -= STEP;
            break;

        // RIGHT - suprotan smer
        case 'd':
        case 'D':
            m_angleSegment2 += STEP;
            break;

        // LEFT 2 - rotacija celog kaktusa
        case 'q':
        case 'Q':
            m_angleSegment1 -= STEP;
            break;

        // RIGHT 2 - suprotan smer
        case 'e':
        case 'E':
            m_angleSegment1 += STEP;
            break;

        // Vracanje u pocetni polozaj
        case 'r':
        case 'R':
            m_angleSegment1 = 0.0f;
            m_angleSegment2 = 0.0f;
            break;

        default:
            CView::OnChar(
                nChar,
                nRepCnt,
                nFlags
            );
            return;
    }

    Invalidate(FALSE);
}

// Bojenje pozadine

BOOL CIND19728View::OnEraseBkgnd(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);

    CBrush lightBlueBrush(
        RGB(135, 207, 235)
    );

    pDC->FillRect(
        &rect,
        &lightBlueBrush
    );

    return TRUE;
}

// Stampanje

BOOL CIND19728View::OnPreparePrinting(
    CPrintInfo* pInfo)
{
    return DoPreparePrinting(pInfo);
}

void CIND19728View::OnBeginPrinting(
    CDC* /*pDC*/,
    CPrintInfo* /*pInfo*/)
{
}

void CIND19728View::OnEndPrinting(
    CDC* /*pDC*/,
    CPrintInfo* /*pInfo*/)
{
}

// Dijagnostika

#ifdef _DEBUG

void CIND19728View::AssertValid() const
{
    CView::AssertValid();
}

void CIND19728View::Dump(
    CDumpContext& dc) const
{
    CView::Dump(dc);
}

CIND19728Doc* CIND19728View::GetDocument() const
{
    ASSERT(
        m_pDocument->IsKindOf(
            RUNTIME_CLASS(CIND19728Doc)
        )
    );

    return (CIND19728Doc*)m_pDocument;
}

#endif
