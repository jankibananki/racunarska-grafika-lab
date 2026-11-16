
#pragma once

#include "DImage.h"

class CIND19728Doc;

class CIND19728View : public CView
{
protected:
    CIND19728View() noexcept;
    DECLARE_DYNCREATE(CIND19728View)

public:
    CIND19728Doc* GetDocument() const;

    virtual void OnDraw(CDC* pDC);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual ~CIND19728View();

protected:
    static const int PIECE_COUNT = 9;
    static const int MOVING_PIECE = 8;

    DImage m_images[PIECE_COUNT];
    CBitmap m_masks[PIECE_COUNT];
    CBitmap m_colors[PIECE_COUNT];

    bool m_loaded[PIECE_COUNT] = {};
    bool m_imagesInitialized = false;

    float m_pieceX = 600.0f;
    float m_pieceY = 400.0f;

    bool m_showGrid = true;

    void Translate(
        CDC* pDC,
        float dX,
        float dY,
        bool rightMultiply);

    void Scale(
        CDC* pDC,
        float sX,
        float sY,
        bool rightMultiply);

    void Rotate(
        CDC* pDC,
        float angle,
        bool rightMultiply);

    void Mirror(
        CDC* pDC,
        bool mx,
        bool my,
        bool rightMultiply);

    void FilterBitmap(DImage& img, bool blue);
    void MakeGray(DImage& img);
    void MakeBlue(DImage& img);

    void LoadImages(CDC* pDC);

    void PreparePiece(
        CDC* pDC,
        int index);

    void DrawPuzzlePiece(
        CDC* pDC,
        DImage& img,
        float x,
        float y,
        float angle,
        float sx,
        float sy,
        bool mx,
        bool my);

    void DrawGrid(CDC* pDC);

    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(
        CDC* pDC,
        CPrintInfo* pInfo);
    virtual void OnEndPrinting(
        CDC* pDC,
        CPrintInfo* pInfo);

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    afx_msg void OnKeyDown(
        UINT nChar,
        UINT nRepCnt,
        UINT nFlags);

    afx_msg void OnLButtonDown(
        UINT nFlags,
        CPoint point);

    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CIND19728Doc* CIND19728View::GetDocument() const
{
    return reinterpret_cast<CIND19728Doc*>(m_pDocument);
}
#endif
