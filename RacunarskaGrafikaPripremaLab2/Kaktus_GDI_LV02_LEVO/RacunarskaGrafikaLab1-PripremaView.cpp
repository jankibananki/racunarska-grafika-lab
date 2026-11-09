
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

// Rotacija - ugao u radijanima

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

// Crtanje EMF fajla

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

    // Vracanje skaliranja
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

// Crtanje kaktusa sa saksijom levo

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

    // Koren kaktusa - saksija levo
    Translate(pDC, 75, 250, false);

    // Okretanje osnovnog segmenta udesno
    Rotate(pDC, PI / 2.0f, false);

    DrawJoint(pDC);

    // Prvi veliki horizontalni segment
    DrawSegment(
        pDC, false, 1.1f, 1.0f, true
    );

    // Prvi veliki cvor J1 - (150, 250)
    DrawJoint(pDC);

    XFORM xJ1;
    pDC->GetWorldTransform(&xJ1);

    // Gornja nepokretna grana
    {
        Rotate(
            pDC, -45.0f * DEG, false
        );

        DrawSegment(
            pDC, false, 0.75f, 0.9f, false
        );

        // Povratak u J1
        pDC->SetWorldTransform(&xJ1);
    }

    // Donja desna razgranata struktura
    {
        // LEFT 2 / RIGHT 2 rotiraju ovu
        // granu i sve njene nastavke oko J1.
        Rotate(
            pDC,
            45.0f * DEG + m_angleSegment1,
            false
        );

        // Svetli dijagonalni segment
        DrawSegment(
            pDC, true, 0.5f, 1.0f, true
        );

        // Drugi cvor J2
        DrawJoint(pDC);

        XFORM xJ2;
        pDC->GetWorldTransform(&xJ2);

        // Desna horizontalna grana
        {
            Rotate(
                pDC, -45.0f * DEG, false
            );

            // Veliki horizontalni segment
            DrawSegment(
                pDC, false, 0.8f, 1.0f, true
            );

            // Treci cvor J3
            DrawJoint(pDC);

            XFORM xJ3;
            pDC->GetWorldTransform(&xJ3);

            // Gornji desni svetli krak
            {
                Rotate(
                    pDC, -45.0f * DEG, false
                );

                DrawSegment(
                    pDC, true, 0.4f, 0.9f, false
                );

                pDC->SetWorldTransform(&xJ3);
            }

            // Srednji desni svetli krak
            {
                DrawSegment(
                    pDC, true, 0.4f, 0.85f, false
                );

                pDC->SetWorldTransform(&xJ3);
            }

            // Donji desni svetli krak
            // LEFT / RIGHT rotiraju samo ovaj deo
            {
                Rotate(
                    pDC,
                    45.0f * DEG + m_angleSegment2,
                    false
                );

                DrawSegment(
                    pDC, true, 0.4f, 0.9f, false
                );

                pDC->SetWorldTransform(&xJ3);
            }

            // Povratak u J2
            pDC->SetWorldTransform(&xJ2);
        }

        // Donja vertikalna grana
        {
            Rotate(
                pDC, 45.0f * DEG, false
            );

            // Tanak segment nadole
            DrawSegment(
                pDC, false, 0.55f, 1.0f, true
            );

            // Cetvrti cvor J4
            DrawJoint(pDC);

            XFORM xJ4;
            pDC->GetWorldTransform(&xJ4);

            // Donji levi svetli krak
            {
                Rotate(
                    pDC, 45.0f * DEG, false
                );

                DrawSegment(
                    pDC, true, 0.4f, 0.9f, false
                );

                pDC->SetWorldTransform(&xJ4);
            }

            // Donji srednji svetli krak
            {
                DrawSegment(
                    pDC, true, 0.4f, 0.9f, false
                );

                pDC->SetWorldTransform(&xJ4);
            }

            // Donji desni svetli krak
            {
                Rotate(
                    pDC, -45.0f * DEG, false
                );

                DrawSegment(
                    pDC, true, 0.4f, 0.9f, false
                );

                pDC->SetWorldTransform(&xJ4);
            }

            // Povratak u J2
            pDC->SetWorldTransform(&xJ2);
        }

        // Povratak u nepokretni cvor J1
        pDC->SetWorldTransform(&xJ1);
    }

    // Vracanje pocetne transformacije
    pDC->SetWorldTransform(&xInitial);

    // Saksija sa leve strane
    CPen potPen(
        PS_SOLID, 1, RGB(160, 95, 0));

    CBrush potBrush(RGB(235, 150, 0));
    CBrush lipBrush(RGB(250, 165, 10));

    pDC->SelectObject(&potPen);
    pDC->SelectObject(&potBrush);

    // Telo saksije
    CPoint pot[4] =
    {
        CPoint(0, 210),
        CPoint(50, 195),
        CPoint(50, 305),
        CPoint(0, 290)
    };

    pDC->Polygon(pot, 4);

    // Rub saksije uz kaktus
    pDC->SelectObject(&lipBrush);
    pDC->Rectangle(
        50, 190, 70, 310
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
    const float STEP = 30.0f * PI / 180.0f;

    switch (nChar)
    {
        // Mreza
        case 'g':
        case 'G':
            m_showGrid = !m_showGrid;
            break;

        // LEFT - donji desni svetli krak
        case 'a':
        case 'A':
            m_angleSegment2 -= STEP;
            break;

        // RIGHT - suprotan smer
        case 'd':
        case 'D':
            m_angleSegment2 += STEP;
            break;

        // LEFT 2 - cela desna struktura
        case 'q':
        case 'Q':
            m_angleSegment1 -= STEP;
            break;

        // RIGHT 2 - suprotan smer
        case 'e':
        case 'E':
            m_angleSegment1 += STEP;
            break;

        // Reset rotacija
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

void CIND19728View::Dump(CDumpContext& dc) const
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
