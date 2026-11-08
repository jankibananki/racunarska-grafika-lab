#pragma once

class CRacunarskaGrafikaLab1PripremaDoc;

class CRacunarskaGrafikaLab1PripremaView : public CView
{
protected:
    CRacunarskaGrafikaLab1PripremaView() noexcept;
    DECLARE_DYNCREATE(CRacunarskaGrafikaLab1PripremaView)

public:
    virtual ~CRacunarskaGrafikaLab1PripremaView();
    virtual void OnDraw(CDC* pDC);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
    // Stanja animacije i mreze
    bool m_showGrid = true;
    float m_leftAngle = 0.0f;
    float m_rightAngle = 0.0f;

    // Ucitani originalni EMF resursi
    HENHMETAFILE m_cactusDark = nullptr;
    HENHMETAFILE m_cactusLight = nullptr;

    // Trazeni metodi za transformacije
    void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
    void Scale(CDC* pDC, float sX, float sY, bool rightMultiply);
    void Rotate(CDC* pDC, float angle, bool rightMultiply);
    void DrawFigure(CDC* pDC);

    // Pomocni metodi View klase
    void DrawGrid(CDC* pDC);
    void DrawPot(CDC* pDC);
    void DrawCactusPart(CDC* pDC, float x, float y,
                        float width, float height,
                        float angle, bool light);
    HENHMETAFILE LoadCactusFile(const wchar_t* filename);

    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

public:
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    CRacunarskaGrafikaLab1PripremaDoc* GetDocument() const;

protected:
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CRacunarskaGrafikaLab1PripremaDoc* CRacunarskaGrafikaLab1PripremaView::GetDocument() const
{
    return reinterpret_cast<CRacunarskaGrafikaLab1PripremaDoc*>(m_pDocument);
}
#endif
