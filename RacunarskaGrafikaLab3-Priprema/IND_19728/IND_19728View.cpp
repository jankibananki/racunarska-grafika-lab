
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

// Rotacija

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

// Filtriranje preko GetBitmapBits i SetBitmapBits

void CIND19728View::ApplyFilter(
    CBitmap& bmp,
    bool blue)
{
    BITMAP bm = {};
    bmp.GetBitmap(&bm);

    if (bm.bmPlanes != 1 || bm.bmBitsPixel != 32)
        return;

    LONG size = bm.bmWidthBytes * bm.bmHeight;

    std::vector<BYTE> pixels(size);

    if (bmp.GetBitmapBits(size, pixels.data()) != size)
        return;

    for (int y = 0; y < bm.bmHeight; y++)
    {
        for (int x = 0; x < bm.bmWidth; x++)
        {
            int i = y * bm.bmWidthBytes + x * 4;

            int b = pixels[i];
            int g = pixels[i + 1];
            int r = pixels[i + 2];

            int gr = 64 + (r + g + b) / 3;

            if (gr > 255)
                gr = 255;

            pixels[i] = static_cast<BYTE>(gr);

            if (blue)
            {
                pixels[i + 1] = 0;
                pixels[i + 2] = 0;
            }
            else
            {
                pixels[i + 1] = static_cast<BYTE>(gr);
                pixels[i + 2] = static_cast<BYTE>(gr);
            }
        }
    }

    bmp.SetBitmapBits(size, pixels.data());
}

// Ucitavanje slika

void CIND19728View::LoadImages(CDC* pDC)
{
    if (m_imagesInitialized)
        return;

    m_imagesInitialized = true;

    for (int i = 0; i < PIECE_COUNT; i++)
    {
        CString filename;
        filename.Format(_T("Slike\\deo%d.bmp"), i);

        m_loaded[i] = m_images[i].Load(filename);

        // Podrska i za fajlove bez ekstenzije
        if (!m_loaded[i])
        {
            filename.Format(_T("Slike\\deo%d"), i);
            m_loaded[i] = m_images[i].Load(filename);
        }

        if (m_loaded[i])
        {
            m_loaded[i] = m_images[i].isValid();

            if (m_loaded[i])
                m_loaded[i] = PreparePiece(pDC, i);
        }
    }
}

// Priprema transparentnosti i filtrirane slike

bool CIND19728View::PreparePiece(
    CDC* pDC,
    int index)
{
    DImage& img = m_images[index];

    int w = img.Width();
    int h = img.Height();

    if (w <= 0 || h <= 0 || !img.GetBitmap())
        return false;

    CDC sourceDC;
    CDC maskDC;
    CDC colorDC;

    if (!sourceDC.CreateCompatibleDC(pDC))
        return false;

    if (!maskDC.CreateCompatibleDC(pDC))
        return false;

    if (!colorDC.CreateCompatibleDC(pDC))
        return false;

    if (!m_masks[index].CreateBitmap(
        w, h, 1, 1, nullptr))
        return false;

    if (!m_colors[index].CreateBitmap(
        w, h, 1, 32, nullptr))
        return false;

    CBitmap* oldSource =
        sourceDC.SelectObject(img.GetBitmap());

    CBitmap* oldMask =
        maskDC.SelectObject(&m_masks[index]);

    CBitmap* oldColor =
        colorDC.SelectObject(&m_colors[index]);

    // Uzimamo boju pozadine iz originalne slike
    COLORREF background = sourceDC.GetPixel(0, 0);

    // Prvo kopiramo originalnu sliku
    colorDC.BitBlt(
        0, 0, w, h,
        &sourceDC,
        0, 0,
        SRCCOPY
    );

    // Kreiranje monohromatske maske
    COLORREF oldSourceBk =
        sourceDC.SetBkColor(background);

    maskDC.BitBlt(
        0, 0, w, h,
        &sourceDC,
        0, 0,
        SRCCOPY
    );

    sourceDC.SetBkColor(oldSourceBk);

    // Vracamo GDI objekte pre direktne izmene bitmape
    colorDC.SelectObject(oldColor);

    // Filter se primenjuje samo na kolor bitmapu
    ApplyFilter(
        m_colors[index],
        index == MOVING_PIECE
    );

    // Ponovo biramo filtriranu bitmapu
    oldColor =
        colorDC.SelectObject(&m_colors[index]);

    // Crna pozadina za SRCPAINT
    COLORREF oldText =
        colorDC.SetTextColor(RGB(255, 255, 255));

    COLORREF oldBk =
        colorDC.SetBkColor(RGB(0, 0, 0));

    colorDC.BitBlt(
        0, 0, w, h,
        &maskDC,
        0, 0,
        SRCAND
    );

    colorDC.SetTextColor(oldText);
    colorDC.SetBkColor(oldBk);

    sourceDC.SelectObject(oldSource);
    maskDC.SelectObject(oldMask);
    colorDC.SelectObject(oldColor);

    return true;
}

// Crtanje pojedinacnog dela slagalice

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

    Translate(pDC, x, y, false);
    Rotate(pDC, angle, false);
    Mirror(pDC, mx, my, false);
    Scale(pDC, sx, sy, false);

    Translate(
        pDC,
        -w / 2.0f,
        -h / 2.0f,
        false
    );

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

    // Prvi prolaz - maska
    pDC->StretchBlt(
        0, 0, w, h,
        &maskDC,
        0, 0, w, h,
        SRCAND
    );

    // Drugi prolaz - filtrirana slika
    pDC->StretchBlt(
        0, 0, w, h,
        &colorDC,
        0, 0, w, h,
        SRCPAINT
    );

    pDC->SetTextColor(oldText);
    pDC->SetBkColor(oldBk);

    maskDC.SelectObject(oldMask);
    colorDC.SelectObject(oldColor);

    pDC->RestoreDC(saved);
}

// Crtanje mreze

void CIND19728View::DrawGrid(CDC* pDC)
{
    CPen pen(PS_SOLID, 1, RGB(230, 230, 230));

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

    // Mreza se crta iza slagalice
    if (m_showGrid)
        DrawGrid(&memDC);

    const float DEG = PI / 180.0f;

    // Redosled fajlova po pozicijama
    const int order[PIECE_COUNT] = {
        2, 1, 0,
        3, 8, 4,
        5, 7, 6
    };

    // Rotacije po indeksima fajlova
    const float angles[PIECE_COUNT] = {
        -20.0f,
        100.0f,
         62.0f,
        161.0f,
         51.0f,
       -160.0f,
         -5.0f,
        127.0f,
        -47.0f
    };

    const bool mirrorX[PIECE_COUNT] = {
        true, true, true,
        true, true, true,
        true, true, true
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

    // Korekcije po pozicijama u slagalici
    const float offsetX[PIECE_COUNT] = {
         6.0f,  8.0f, -6.0f,
         5.0f, -8.0f,  4.0f,
         0.0f,  8.0f, -4.0f
    };

    const float offsetY[PIECE_COUNT] = {
        -6.0f, -2.0f, -6.0f,
         7.0f, -3.0f, -7.0f,
         9.0f,  2.0f, -8.0f
    };

    // Osam nepokretnih sivih delova
    for (int slot = 0; slot < PIECE_COUNT; slot++)
    {
        int i = order[slot];

        if (i == MOVING_PIECE || !m_loaded[i])
            continue;

        int col = slot % 3;
        int row = slot / 3;

        float x =
            startX + (col + 0.5f) * tile
            + offsetX[slot];

        float y =
            startY + (row + 0.5f) * tile
            + offsetY[slot];

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

    // Pokretni plavi deo
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

    // Poruka za nedostajuce slike
    for (int i = 0; i < PIECE_COUNT; i++)
    {
        if (!m_loaded[i])
        {
            memDC.SetBkMode(TRANSPARENT);
            memDC.SetTextColor(RGB(180, 0, 0));

            memDC.TextOut(
                10, 480,
                _T("Nedostaju slike u folderu Slike!")
            );

            break;
        }
    }

    // Prenos gotovog prikaza na ekran
    pDC->BitBlt(
        0, 0, width, height,
        &memDC, 0, 0,
        SRCCOPY
    );

    memDC.SelectObject(oldBuffer);
}

// Kontrole tastature

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

    case VK_RETURN:
        m_pieceX = 242.0f;
        m_pieceY = 247.0f;
        break;

    default:
        changed = false;
        break;
    }

    if (changed)
        Invalidate(FALSE);

    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

// Klik za postavljanje plavog dela

void CIND19728View::OnLButtonDown(
    UINT nFlags,
    CPoint point)
{
    SetFocus();

    const float targetX = 242.0f;
    const float targetY = 247.0f;
    const float halfSize = 90.0f;

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

    return static_cast<CIND19728Doc*>(m_pDocument);
}

#endif
