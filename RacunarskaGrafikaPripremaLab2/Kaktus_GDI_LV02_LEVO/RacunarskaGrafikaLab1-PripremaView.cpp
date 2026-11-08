#include "pch.h"
#include "framework.h"

#ifndef SHARED_HANDLERS
#include "RacunarskaGrafikaLab1-Priprema.h"
#endif

#include "RacunarskaGrafikaLab1-PripremaDoc.h"
#include "RacunarskaGrafikaLab1-PripremaView.h"

#include <cmath>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CRacunarskaGrafikaLab1PripremaView, CView)

BEGIN_MESSAGE_MAP(CRacunarskaGrafikaLab1PripremaView, CView)
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CRacunarskaGrafikaLab1PripremaView::CRacunarskaGrafikaLab1PripremaView() noexcept
{
    m_cactusDark = LoadCactusFile(L"cactus_part.emf");
    m_cactusLight = LoadCactusFile(L"cactus_part_light.emf");
}

CRacunarskaGrafikaLab1PripremaView::~CRacunarskaGrafikaLab1PripremaView()
{
    if (m_cactusDark)
        ::DeleteEnhMetaFile(m_cactusDark);
    if (m_cactusLight)
        ::DeleteEnhMetaFile(m_cactusLight);
}

BOOL CRacunarskaGrafikaLab1PripremaView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

// Najpre pokusava iz direktorijuma projekta (radni direktorijum),
// zatim pored .exe datoteke. Oba puta trazi potfolder RESURSI.
HENHMETAFILE CRacunarskaGrafikaLab1PripremaView::LoadCactusFile(
    const wchar_t* filename)
{
    std::wstring relative = std::wstring(L"RESURSI\\") + filename;
    HENHMETAFILE result = ::GetEnhMetaFileW(relative.c_str());
    if (result)
        return result;

    result = ::GetEnhMetaFileW(filename);
    if (result)
        return result;

    wchar_t module[MAX_PATH] = {};
    DWORD length = ::GetModuleFileNameW(nullptr, module, MAX_PATH);
    if (length == 0 || length >= MAX_PATH)
        return nullptr;

    std::wstring exePath(module);
    size_t slash = exePath.find_last_of(L"\\/");
    if (slash == std::wstring::npos)
        return nullptr;

    std::wstring folder = exePath.substr(0, slash + 1);
    return ::GetEnhMetaFileW((folder + relative).c_str());
}

// TRANSLACIJA
void CRacunarskaGrafikaLab1PripremaView::Translate(
    CDC* pDC, float dX, float dY, bool rightMultiply)
{
    XFORM xf = {1.0f, 0.0f, 0.0f, 1.0f, dX, dY};
    pDC->ModifyWorldTransform(&xf,
        rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

// SKALIRANJE
void CRacunarskaGrafikaLab1PripremaView::Scale(
    CDC* pDC, float sX, float sY, bool rightMultiply)
{
    XFORM xf = {sX, 0.0f, 0.0f, sY, 0.0f, 0.0f};
    pDC->ModifyWorldTransform(&xf,
        rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

// ROTACIJA - angle je u stepenima
void CRacunarskaGrafikaLab1PripremaView::Rotate(
    CDC* pDC, float angle, bool rightMultiply)
{
    const double PI = 3.14159265358979323846;
    double a = angle * PI / 180.0;
    float c = static_cast<float>(std::cos(a));
    float s = static_cast<float>(std::sin(a));

    XFORM xf = {c, s, -s, c, 0.0f, 0.0f};
    pDC->ModifyWorldTransform(&xf,
        rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

// Celo polje je 500x500. Mreza ide preko pozadine i kaktusa.
void CRacunarskaGrafikaLab1PripremaView::DrawGrid(CDC* pDC)
{
    CPen pen(PS_SOLID, 1, RGB(226, 244, 251));
    CPen* old = pDC->SelectObject(&pen);

    for (int i = 0; i <= 500; i += 25)
    {
        pDC->MoveTo(i, 0);
        pDC->LineTo(i, 500);
        pDC->MoveTo(0, i);
        pDC->LineTo(500, i);
    }

    pDC->SelectObject(old);
}

// Saksija: pravougaoni obod i trapezasto telo.
void CRacunarskaGrafikaLab1PripremaView::DrawPot(CDC* pDC)
{
    CPen border(PS_SOLID, 1, RGB(156, 89, 0));
    CBrush body(RGB(229, 145, 0));
    CBrush lip(RGB(245, 166, 14));
    CPen* oldPen = pDC->SelectObject(&border);
    CBrush* oldBrush = pDC->SelectObject(&body);

    CPoint pot[] = {
        CPoint(199, 445), CPoint(301, 445),
        CPoint(287, 500), CPoint(213, 500)
    };
    pDC->Polygon(pot, 4);
    pDC->SelectObject(&lip);
    pDC->Rectangle(194, 430, 306, 449);

    pDC->SelectObject(oldBrush);
    pDC->SelectObject(oldPen);
}

// Jedan univerzalni segment: ucitava originalni EMF, menja mu
// velicinu, rotira ga i postavlja na zadatu poziciju.
// Ako je aktivna rotacija grupe, LEFTMULTIPLY dodaje lokalne
// transformacije PRE grupne i zadrzava korektnu rotaciju oko cvora.
void CRacunarskaGrafikaLab1PripremaView::DrawCactusPart(
    CDC* pDC, float x, float y,
    float width, float height, float angle, bool light)
{
    HENHMETAFILE emf = light ? m_cactusLight : m_cactusDark;
    if (!emf)
        return;

    int state = pDC->SaveDC();

    // LEFTMULTIPLY: konacni redosled za tacku je
    // Scale -> Rotate -> Translate -> (eventualna rotacija cele grupe)
    Translate(pDC, x, y, false);
    Rotate(pDC, angle, false);
    Scale(pDC, width / 200.0f, height / 200.0f, false);

    RECT dst = {-100, -100, 100, 100};
    ::PlayEnhMetaFile(pDC->GetSafeHdc(), emf, &dst);

    pDC->RestoreDC(state);
}

void CRacunarskaGrafikaLab1PripremaView::DrawFigure(CDC* pDC)
{
    // Saksija miruje.
    DrawPot(pDC);

    // Sredisnji deo i stablo miruju.
    DrawCactusPart(pDC, 250, 427, 39, 28, 0, false);
    DrawCactusPart(pDC, 250, 391, 106, 76, 0, false);
    DrawCactusPart(pDC, 250, 351, 53, 27, 0, false);
    DrawCactusPart(pDC, 250, 316, 38, 76, 0, false);

    // LEVA GRUPA: okretanje oko sredisnjeg cvora (250,351).
    {
        int state = pDC->SaveDC();
        // LEFTMULTIPLY: za rotaciju oko tacke (250, 351),
        // pozivi su obrnutim redosledom od RIGHTMULTIPLY varijante.
        // Konacna matrica: T(-pivot) * R * T(+pivot).
        Translate(pDC, 250, 351, false);
        Rotate(pDC, m_leftAngle, false);
        Translate(pDC, -250, -351, false);

        DrawCactusPart(pDC, 225, 327, 53, 90, -45, true);
        DrawCactusPart(pDC, 200, 300, 57, 28, 0, true);
        DrawCactusPart(pDC, 162, 300, 60, 79, 90, false);
        DrawCactusPart(pDC, 200, 260, 73, 70, 0, true);
        DrawCactusPart(pDC, 200, 222, 44, 30, 0, true);
        DrawCactusPart(pDC, 200, 184, 108, 79, 0, false);

        pDC->RestoreDC(state);
    }

    // DESNA GRUPA: zasebna rotacija oko istog centralnog cvora.
    {
        int state = pDC->SaveDC();
        // LEFTMULTIPLY: ista sema kao za levu granu.
        Translate(pDC, 250, 351, false);
        Rotate(pDC, m_rightAngle, false);
        Translate(pDC, -250, -351, false);

        DrawCactusPart(pDC, 279, 327, 53, 90, 45, true);
        DrawCactusPart(pDC, 307, 300, 57, 28, 0, true);
        DrawCactusPart(pDC, 307, 260, 73, 70, 0, false);
        DrawCactusPart(pDC, 346, 300, 60, 80, 90, false);
        DrawCactusPart(pDC, 382, 300, 57, 28, 0, true);
        DrawCactusPart(pDC, 409, 277, 64, 78, 45, false);
        DrawCactusPart(pDC, 409, 325, 64, 78, -45, false);

        pDC->RestoreDC(state);
    }
}

void CRacunarskaGrafikaLab1PripremaView::OnDraw(CDC* pDC)
{
    CRacunarskaGrafikaLab1PripremaDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    int state = pDC->SaveDC();
    pDC->SetMapMode(MM_TEXT);
    pDC->SetGraphicsMode(GM_ADVANCED);

    // Reset transformacije na identicnu.
    XFORM identity = {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    pDC->SetWorldTransform(&identity);

    // Pozadina i svi delovi figure.
    pDC->FillSolidRect(0, 0, 500, 500, RGB(129, 199, 230));
    DrawFigure(pDC);

    if (m_showGrid)
        DrawGrid(pDC);

    // Ako fajlovi nisu pronadjeni, ispisace se korisno upozorenje.
    if (!m_cactusDark || !m_cactusLight)
    {
        pDC->SetBkMode(TRANSPARENT);
        pDC->SetTextColor(RGB(150, 0, 0));
        pDC->TextOut(8, 8, _T("Nedostaju EMF fajlovi u folderu RESURSI!"));
    }

    pDC->RestoreDC(state);
}

void CRacunarskaGrafikaLab1PripremaView::OnKeyDown(
    UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar)
    {
    case 'G':
        m_showGrid = !m_showGrid;
        break;
    case 'A':
        m_leftAngle -= 10.0f;
        break;
    case 'D':
        m_leftAngle += 10.0f;
        break;
    case 'J':
        m_rightAngle -= 10.0f;
        break;
    case 'L':
        m_rightAngle += 10.0f;
        break;
    case 'R':
        m_leftAngle = 0.0f;
        m_rightAngle = 0.0f;
        break;
    default:
        CView::OnKeyDown(nChar, nRepCnt, nFlags);
        return;
    }

    Invalidate(FALSE);
    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CRacunarskaGrafikaLab1PripremaView::OnPreparePrinting(CPrintInfo* pInfo)
{
    return DoPreparePrinting(pInfo);
}

void CRacunarskaGrafikaLab1PripremaView::OnBeginPrinting(
    CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CRacunarskaGrafikaLab1PripremaView::OnEndPrinting(
    CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

#ifdef _DEBUG
void CRacunarskaGrafikaLab1PripremaView::AssertValid() const
{
    CView::AssertValid();
}

void CRacunarskaGrafikaLab1PripremaView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CRacunarskaGrafikaLab1PripremaDoc*
CRacunarskaGrafikaLab1PripremaView::GetDocument() const
{
    ASSERT(m_pDocument->IsKindOf(
        RUNTIME_CLASS(CRacunarskaGrafikaLab1PripremaDoc)));
    return static_cast<CRacunarskaGrafikaLab1PripremaDoc*>(m_pDocument);
}
#endif
