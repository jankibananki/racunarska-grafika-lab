
#include "pch.h"
#include "framework.h"

#ifndef SHARED_HANDLERS
#include "IND_19728.h"
#endif

#include "IND_19728Doc.h"
#include "IND_19728View.h"

#include <cmath>
#include <cstdlib>

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

// Grayscale filter iz zadatka

void CIND19728View::MakeGray(DImage& img)
{
    unsigned char* pixels = img.GetDIBBits();

    if (!pixels || img.BPP() < 3)
        return;

    int w = img.Width();
    int h = img.Height();
    int bpp = img.BPP();

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int i = (y * w + x) * bpp;

            int b = pixels[i];
            int g = pixels[i + 1];
            int r = pixels[i + 2];

            int gr = 64 + (r + g + b) / 3;

            if (gr > 255)
                gr = 255;

            pixels[i] = (unsigned char)gr;
            pixels[i + 1] = (unsigned char)gr;
            pixels[i + 2] = (unsigned char)gr;
        }
    }

    img.Update();
}

// Plavi filter za pokretni deo

void CIND19728View::MakeBlue(DImage& img)
{
    unsigned char* pixels = img.GetDIBBits();

    if (!pixels || img.BPP() < 3)
        return;

    int w = img.Width();
    int h = img.Height();
    int bpp = img.BPP();

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int i = (y * w + x) * bpp;

            int b = pixels[i];
            int g = pixels[i + 1];
            int r = pixels[i + 2];

            int value = 64 + (r + g + b) / 3;

            if (value > 255)
                value = 255;

            pixels[i] = (unsigned char)value;
            pixels[i + 1] = 0;
            pixels[i + 2] = 0;
        }
    }

    img.Update();
}

// Ucitavanje slika

void CIND19728View::LoadImages(CDC* pDC)
{
    if (m_imagesInitialized)
        return;

    m_imagesInitialized = true;

    // Fajlovi su bez ekstenzije.
    CString filenames[PIECE_COUNT] = {
        _T("Slike\\deo0"),
        _T("Slike\\deo1"),
        _T("Slike\\deo2"),
        _T("Slike\\deo3"),
        _T("Slike\\deo4"),
        _T("Slike\\deo5"),
        _T("Slike\\deo6"),
        _T("Slike\\deo7"),
        _T("Slike\\deo8")
    };

    for (int i = 0; i < PIECE_COUNT; i++)
    {
        m_loaded[i] =
            m_images[i].Load(filenames[i]) &&
            m_images[i].isValid();

        if (!m_loaded[i])
            continue;

        PreparePiece(pDC, i);
    }
}

// Priprema maske, filtera i kolor bitmape

void CIND19728View::PreparePiece(
    CDC* pDC,
    int index)
{
    DImage& img = m_images[index];

    int w = img.Width();
    int h = img.Height();
    int bpp = img.BPP();

    if (w <= 0 || h <= 0 || bpp < 3)
    {
        m_loaded[index] = false;
        return;
    }

    unsigned char* pixels = img.GetDIBBits();

    if (!pixels)
    {
        m_loaded[index] = false;
        return;
    }

    int keyB = pixels[0];
    int keyG = pixels[1];
    int keyR = pixels[2];

    bool hasTransparent = false;
    bool hasOpaque = false;

    if (bpp == 4)
    {
        for (int i = 0; i < w * h; i++)
        {
            int alpha = pixels[i * bpp + 3];

            if (alpha < 128)
                hasTransparent = true;
            else
                hasOpaque = true;

            if (hasTransparent && hasOpaque)
                break;
        }
    }

    bool useAlpha = hasTransparent && hasOpaque;

    m_masks[index].CreateBitmap(
        w, h, 1, 1, nullptr
    );

    CDC maskDC;
    maskDC.CreateCompatibleDC(pDC);

    CBitmap* oldMask =
        maskDC.SelectObject(&m_masks[index]);

    // Bela maska je providna oblast.
    // Crna maska je vidljivi deo slagalice.

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int i = (y * w + x) * bpp;

            bool transparent = false;

            if (useAlpha)
            {
                transparent = pixels[i + 3] < 128;
            }
            else
            {
                int db = abs((int)pixels[i] - keyB);
                int dg = abs((int)pixels[i + 1] - keyG);
                int dr = abs((int)pixels[i + 2] - keyR);

                transparent =
                    db < 25 &&
                    dg < 25 &&
                    dr < 25;
            }

            maskDC.SetPixel(
                x, y,
                transparent
                    ? RGB(255, 255, 255)
                    : RGB(0, 0, 0)
            );
        }
    }

    // Filtriranje nakon kreiranja maske

    if (index == MOVING_PIECE)
        MakeBlue(img);
    else
        MakeGray(img);

    m_colors[index].CreateCompatibleBitmap(
        pDC, w, h
    );

    CDC colorDC;
    colorDC.CreateCompatibleDC(pDC);

    CBitmap* oldColor =
        colorDC.SelectObject(&m_colors[index]);

    img.Draw(
        &colorDC,
        CRect(0, 0, w, h),
        CRect(0, 0, w, h)
    );

    COLORREF oldText =
        colorDC.SetTextColor(RGB(255, 255, 255));

    COLORREF oldBk =
        colorDC.SetBkColor(RGB(0, 0, 0));

    colorDC.BitBlt(
        0, 0, w, h,
        &maskDC, 0, 0,
        SRCAND
    );

    colorDC.SetTextColor(oldText);
    colorDC.SetBkColor(oldBk);

    colorDC.SelectObject(oldColor);
    maskDC.SelectObject(oldMask);
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
    Translate(pDC, -w / 2.0f, -h / 2.0f, false);

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

    if (m_showGrid)
        DrawGrid(&memDC);

    const float DEG = PI / 180.0f;

    // Redosled delova po mestima u slagalici
    const int order[PIECE_COUNT] = {
        0, 6, 4,
        8, 1, 2,
        3, 5, 7
    };

    // Uglovi po broju fajla
    const float angles[PIECE_COUNT] = {
         155.0f,
         -60.0f,
         -70.0f,
         -20.0f,
          75.0f,
        -115.0f,
         125.0f,
          75.0f,
        -115.0f
    };

    const bool mirrorX[PIECE_COUNT] = {
        true, true, true,
        true, true, true,
        true, false, true
    };

    const bool mirrorY[PIECE_COUNT] = {
        false, false, false,
        false, false, false,
        false, false, false
    };

    const float startX = 25.0f;
    const float startY = 25.0f;
    const float tile = 150.0f;
    const float pieceScale = 1.0f;

    // Osam sivih delova

    for (int slot = 0; slot < PIECE_COUNT; slot++)
    {
        int i = order[slot];

        if (i == MOVING_PIECE || !m_loaded[i])
            continue;

        int col = slot % 3;
        int row = slot / 3;

        float x =
            startX + (col + 0.5f) * tile;

        float y =
            startY + (row + 0.5f) * tile;

        float sx =
            pieceScale * 256.0f / m_images[i].Width();

        float sy =
            pieceScale * 256.0f / m_images[i].Height();

        DrawPuzzlePiece(
            &memDC,
            m_images[i],
            x,
            y,
            angles[i] * DEG,
            sx,
            sy,
            mirrorX[i],
            mirrorY[i]
        );
    }

    // Plavi deo, inicijalno van slagalice

    if (m_loaded[MOVING_PIECE])
    {
        int i = MOVING_PIECE;

        float sx =
            pieceScale * 256.0f / m_images[i].Width();

        float sy =
            pieceScale * 256.0f / m_images[i].Height();

        DrawPuzzlePiece(
            &memDC,
            m_images[i],
            m_pieceX,
            m_pieceY,
            angles[i] * DEG,
            sx,
            sy,
            mirrorX[i],
            mirrorY[i]
        );
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

// Tastatura

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
    const float targetY = 400.0f;
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
