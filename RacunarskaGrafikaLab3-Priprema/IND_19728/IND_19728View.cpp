
#include "pch.h"
#include "framework.h"

#ifndef SHARED_HANDLERS
#include "IND_19728.h"
#endif

#include "IND_19728Doc.h"
#include "IND_19728View.h"

#include <cmath>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const float PI = 3.14159265358979323846f;

IMPLEMENT_DYNCREATE(CIND19728View, CView)

BEGIN_MESSAGE_MAP(CIND19728View, CView)
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    ON_WM_KEYDOWN()
    ON_WM_LBUTTONDOWN()
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

// Translacija

void CIND19728View::Translate(
    CDC* pDC,
    float dX,
    float dY,
    bool rightMultiply)
{
    XFORM xf = {
        1.0f, 0.0f,
        0.0f, 1.0f,
        dX, dY
    };

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
    XFORM xf = {
        sX, 0.0f,
        0.0f, sY,
        0.0f, 0.0f
    };

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
    float c = static_cast<float>(cos(angle));
    float s = static_cast<float>(sin(angle));

    XFORM xf = {
         c, s,
        -s, c,
        0.0f, 0.0f
    };

    pDC->ModifyWorldTransform(
        &xf,
        rightMultiply
            ? MWT_RIGHTMULTIPLY
            : MWT_LEFTMULTIPLY
    );
}

// Preslikavanje

void CIND19728View::Mirror(
    CDC* pDC,
    bool mx,
    bool my,
    bool rightMultiply)
{
    XFORM xf = {
        mx ? -1.0f : 1.0f, 0.0f,
        0.0f, my ? -1.0f : 1.0f,
        0.0f, 0.0f
    };

    pDC->ModifyWorldTransform(
        &xf,
        rightMultiply
            ? MWT_RIGHTMULTIPLY
            : MWT_LEFTMULTIPLY
    );
}

// Bitmapa koju DImage ucita ima 32 bita po pikselu (B, G, R, rezervisano).
// Ne menjamo DImage klasu niti njen interni bafer.
void CIND19728View::FilterBitmap(DImage& img, bool blue)
{
    CBitmap* bitmap = img.GetBitmap();
    BITMAP info = {};
    bitmap->GetBitmap(&info);
    const LONG size = info.bmWidthBytes * info.bmHeight;
    std::vector<BYTE> pixels(size);
    if (info.bmBitsPixel != 32 || bitmap->GetBitmapBits(size, pixels.data()) != size)
        return;

    for (int y = 0; y < info.bmHeight; ++y)
        for (int x = 0; x < info.bmWidth; ++x)
        {
            BYTE* pixel = pixels.data() + y * info.bmWidthBytes + x * 4;
            int gray = 64 + (pixel[0] + pixel[1] + pixel[2]) / 3;
            if (gray > 255) gray = 255;
            // Plavi deo zadrzava detalje, sa potisnutim R i G kanalima.
            pixel[0] = static_cast<BYTE>(blue ? gray - 64 : gray);
            pixel[1] = pixel[2] = blue ? 0 : static_cast<BYTE>(gray);
        }
    bitmap->SetBitmapBits(size, pixels.data());
}

void CIND19728View::MakeGray(DImage& img) { FilterBitmap(img, false); }
void CIND19728View::MakeBlue(DImage& img) { FilterBitmap(img, true); }

void CIND19728View::LoadImages(CDC* pDC)
{
    if (m_imagesInitialized) return;
    m_imagesInitialized = true;
    TCHAR executable[MAX_PATH] = {};
    GetModuleFileName(nullptr, executable, MAX_PATH);
    CString folder(executable);
    folder = folder.Left(folder.ReverseFind(_T('\\')) + 1);

    for (int i = 0; i < PIECE_COUNT; ++i)
    {
        CString relative;
        relative.Format(_T("Slike\\deo%d.bmp"), i);
        // Visual Studio i pokretanje iz izlaznog direktorijuma.
        CString path = relative;
        if (GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES)
            path = folder + relative;
        if (GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES)
            continue;
        m_loaded[i] = m_images[i].Load(path) && m_images[i].isValid();
        if (m_loaded[i]) PreparePiece(pDC, i);
    }
}

void CIND19728View::PreparePiece(CDC* pDC, int index)
{
    DImage& img = m_images[index];
    const int w = img.Width(), h = img.Height();
    CDC sourceDC, maskDC, colorDC;
    sourceDC.CreateCompatibleDC(pDC);
    maskDC.CreateCompatibleDC(pDC);
    colorDC.CreateCompatibleDC(pDC);
    m_masks[index].CreateBitmap(w, h, 1, 1, nullptr);
    m_colors[index].CreateCompatibleBitmap(pDC, w, h);
    CBitmap* oldSource = sourceDC.SelectObject(img.GetBitmap());
    CBitmap* oldMask = maskDC.SelectObject(&m_masks[index]);
    CBitmap* oldColor = colorDC.SelectObject(&m_colors[index]);

    // Pre filtriranja: boja gornjeg levog piksela postaje bela u maski.
    sourceDC.SetBkColor(sourceDC.GetPixel(0, 0));
    maskDC.BitBlt(0, 0, w, h, &sourceDC, 0, 0, SRCCOPY);
    sourceDC.SelectObject(oldSource);
    if (index == MOVING_PIECE) MakeBlue(img); else MakeGray(img);
    oldSource = sourceDC.SelectObject(img.GetBitmap());
    colorDC.BitBlt(0, 0, w, h, &sourceDC, 0, 0, SRCCOPY);
    // Bela oblast maske mora postati crna na kolor bitmap istoj poziciji.
    colorDC.SetTextColor(RGB(255, 255, 255));
    colorDC.SetBkColor(RGB(0, 0, 0));
    colorDC.BitBlt(0, 0, w, h, &maskDC, 0, 0, SRCAND);
    sourceDC.SelectObject(oldSource);
    maskDC.SelectObject(oldMask);
    colorDC.SelectObject(oldColor);
}

// Crtanje jednog dela slagalice

void CIND19728View::DrawPuzzlePiece(
    CDC* pDC,
    DImage& img,
    float x,
    float y,
    float angle,
    float sx,
    float sy,
    bool mx,
    bool my)
{
    int index = -1;

    for (int i = 0; i < PIECE_COUNT; i++)
    {
        if (&m_images[i] == &img)
        {
            index = i;
            break;
        }
    }

    if (index < 0 || !m_loaded[index])
        return;

    int w = img.Width();
    int h = img.Height();

    int saved = pDC->SaveDC();

    pDC->SetGraphicsMode(GM_ADVANCED);

    // Centriranje, skaliranje, preslikavanje,
    // rotacija i postavljanje na zadatu poziciju.

    Translate(pDC, x, y, false);
    Rotate(pDC, angle, false);
    Mirror(pDC, mx, my, false);
    Scale(pDC, sx, sy, false);
    // Centar kvadratnog jezgra, bez ispupcenja (nije centar cele bitmape).
    const float pivotX[PIECE_COUNT] = { 123.914f, 125.126f, 125.161f, 123.957f, 124.577f, 124.764f, 124.212f, 124.283f, 124.238f };
    const float pivotY[PIECE_COUNT] = { 135.190f, 135.205f, 135.657f, 136.011f, 135.014f, 135.018f, 135.818f, 135.093f, 135.397f };
    Translate(pDC, -pivotX[index], -pivotY[index], false);

    CDC maskDC;
    CDC colorDC;

    maskDC.CreateCompatibleDC(pDC);
    colorDC.CreateCompatibleDC(pDC);

    CBitmap* oldMask =
        maskDC.SelectObject(&m_masks[index]);

    CBitmap* oldColor =
        colorDC.SelectObject(&m_colors[index]);

    COLORREF oldText =
        pDC->SetTextColor(RGB(0, 0, 0));

    COLORREF oldBk =
        pDC->SetBkColor(RGB(255, 255, 255));

    pDC->SetStretchBltMode(COLORONCOLOR);

    // Transparentnost - maska
    pDC->StretchBlt(
        0, 0, w, h,
        &maskDC,
        0, 0, w, h,
        SRCAND
    );

    // Transparentnost - slika
    pDC->StretchBlt(
        0, 0, w, h,
        &colorDC,
        0, 0, w, h,
        SRCPAINT
    );

    pDC->SetTextColor(oldText);
    pDC->SetBkColor(oldBk);

    colorDC.SelectObject(oldColor);
    maskDC.SelectObject(oldMask);

    pDC->RestoreDC(saved);
}

// Crtanje mreze

void CIND19728View::DrawGrid(CDC* pDC)
{
    CPen pen(
        PS_SOLID, 1,
        RGB(230, 230, 230)
    );

    CPen* oldPen = pDC->SelectObject(&pen);

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

// Glavno crtanje sa anti-flickerom

void CIND19728View::OnDraw(CDC* pDC)
{
    CIND19728Doc* pDoc = GetDocument();

    ASSERT_VALID(pDoc);

    if (!pDoc)
        return;

    CRect client;
    GetClientRect(&client);

    int width = client.Width();
    int height = client.Height();

    if (width <= 0 || height <= 0)
        return;

    LoadImages(pDC);

    CDC memDC;
    memDC.CreateCompatibleDC(pDC);

    CBitmap backBuffer;
    backBuffer.CreateCompatibleBitmap(
        pDC, width, height
    );

    CBitmap* oldBuffer =
        memDC.SelectObject(&backBuffer);

    memDC.FillSolidRect(
        0, 0, width, height,
        RGB(255, 255, 255)
    );

    if (m_showGrid) DrawGrid(&memDC);

    // Slika je okrenuta kao na zadatom primeru: glava ulevo.
    const int order[PIECE_COUNT] = { 2, 1, 0, 3, 8, 4, 5, 7, 6 };
    // Uglovi su indeksirani brojem fajla, u stepenima.
    const float angles[PIECE_COUNT] = { -20, 100, 61, 162, 51, 200, -5, 127, -47 };
    // Pokretni deo crtamo poslednji da ostane vidljiv pri pomeranju.
    for (int pass = 0; pass < 2; ++pass)
    for (int slot = 0; slot < PIECE_COUNT; ++slot)
    {
        const int i = order[slot];
        if ((i == MOVING_PIECE) != (pass == 1)) continue;
        const float x = i == MOVING_PIECE ? m_pieceX : 100.0f + (slot % 3) * 150.0f;
        const float y = i == MOVING_PIECE ? m_pieceY : 100.0f + (slot / 3) * 150.0f;
        DrawPuzzlePiece(&memDC, m_images[i], x, y,
            angles[i] * PI / 180.0f, 1.0f, 1.0f, true, false);
    }

    // Prikaz upozorenja ako nedostaje slika

    bool missing = false;

    for (int i = 0; i < PIECE_COUNT; i++)
    {
        if (!m_loaded[i])
        {
            missing = true;
            break;
        }
    }

    if (missing)
    {
        memDC.SetBkMode(TRANSPARENT);
        memDC.SetTextColor(RGB(180, 0, 0));

        memDC.TextOut(
            10, 470,
            _T("Nedostaju slike u folderu Slike!")
        );
    }

    // Prenos gotovog kadra na prozor

    pDC->BitBlt(
        0, 0, width, height,
        &memDC, 0, 0,
        SRCCOPY
    );

    memDC.SelectObject(oldBuffer);
}

// Tastaturaa

void CIND19728View::OnKeyDown(
    UINT nChar,
    UINT nRepCnt,
    UINT nFlags)
{
    const float STEP = 10.0f;

    bool changed = true;

    switch (nChar)
    {
    case VK_LEFT:
        m_pieceX -= STEP;
        break;

    case VK_RIGHT:
        m_pieceX += STEP;
        break;

    case VK_UP:
        m_pieceY -= STEP;
        break;

    case VK_DOWN:
        m_pieceY += STEP;
        break;

    case 'G':
        m_showGrid = !m_showGrid;
        break;

    case 'R':
        m_pieceX = 600.0f;
        m_pieceY = 400.0f;
        break;

    default:
        changed = false;
        break;
    }

    if (changed)
        Invalidate(FALSE);

    CView::OnKeyDown(
        nChar, nRepCnt, nFlags
    );
}

// Klik za postavljanje plavog dela

void CIND19728View::OnLButtonDown(
    UINT nFlags,
    CPoint point)
{
    SetFocus();

    const float targetX = 250.0f;
    const float targetY = 250.0f;
    const float halfSize = 75.0f;

    bool clickedBlue =
        point.x >= m_pieceX - halfSize &&
        point.x <= m_pieceX + halfSize &&
        point.y >= m_pieceY - halfSize &&
        point.y <= m_pieceY + halfSize;

    bool clickedTarget =
        point.x >= targetX - halfSize &&
        point.x <= targetX + halfSize &&
        point.y >= targetY - halfSize &&
        point.y <= targetY + halfSize;

    if (clickedBlue || clickedTarget)
    {
        m_pieceX = targetX;
        m_pieceY = targetY;

        Invalidate(FALSE);
    }

    CView::OnLButtonDown(nFlags, point);
}

// Pozadinu crtamo u memorijskom DC-ju

BOOL CIND19728View::OnEraseBkgnd(CDC* pDC)
{
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

    return static_cast<CIND19728Doc*>(
        m_pDocument
    );
}

#endif
