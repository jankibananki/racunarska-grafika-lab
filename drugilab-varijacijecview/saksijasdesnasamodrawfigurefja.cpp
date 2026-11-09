void CIND19728View::DrawFigure(CDC* pDC)
{
    const float DEG = PI / 180.0f;

    XFORM xInitial;
    pDC->GetWorldTransform(&xInitial);

    CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));
    CBrush cactusBrush(RGB(0, 204, 0));

    CPen* oldPen = pDC->SelectObject(&blackPen);
    CBrush* oldBrush = pDC->SelectObject(&cactusBrush);

    // Pocetak kaktusa - spoj sa saksijom sa desne strane
    Translate(pDC, 390, 250, false);

    // Segment sada ide ulevo.
    // m_angleSegment1 rotira ceo kaktus.
    Rotate(pDC, PI / 2 + m_angleSegment1, false);

    DrawJoint(pDC);

    // Glavni segment od saksije ka unutra
    DrawSegment(pDC, false, 1.15f, 1.0f, true);

    // Prvi veliki cvor
    DrawJoint(pDC);

    XFORM xJ1;
    pDC->GetWorldTransform(&xJ1);

    // Gornja grana
    {
        Rotate(pDC, -45.0f * DEG, false);

        // Tanak svetli spoj
        DrawSegment(pDC, true, 0.45f, 1.0f, true);
        DrawJoint(pDC);

        XFORM xUpper;
        pDC->GetWorldTransform(&xUpper);

        // Veliki segment koji se rotira na LEFT / RIGHT
        Rotate(pDC, m_angleSegment2, false);
        DrawSegment(pDC, true, 1.1f, 0.9f, false);

        pDC->SetWorldTransform(&xUpper);

        // Nastavak nagore
        Rotate(pDC, -45.0f * DEG, false);
        DrawSegment(pDC, false, 0.7f, 1.0f, true);
        DrawJoint(pDC);

        XFORM xTop;
        pDC->GetWorldTransform(&xTop);

        // Gore-levo
        Rotate(pDC, 45.0f * DEG, false);
        DrawSegment(pDC, true, 0.55f, 0.8f, false);

        pDC->SetWorldTransform(&xTop);

        // Gore
        DrawSegment(pDC, true, 0.32f, 1.0f, false);

        pDC->SetWorldTransform(&xTop);

        // Gore-desno
        Rotate(pDC, -45.0f * DEG, false);
        DrawSegment(pDC, true, 0.55f, 0.8f, false);

        pDC->SetWorldTransform(&xJ1);
    }

    // Srednja leva horizontalna grana
    {
        DrawSegment(pDC, true, 0.75f, 0.85f, true);
        DrawJoint(pDC);

        // Veliki levi horizontalni segment
        DrawSegment(pDC, false, 1.1f, 0.9f, false);

        pDC->SetWorldTransform(&xJ1);
    }

    // Donja grana
    {
        Rotate(pDC, 45.0f * DEG, false);

        // Tanak svetli spoj
        DrawSegment(pDC, true, 0.45f, 1.0f, true);
        DrawJoint(pDC);

        XFORM xLower;
        pDC->GetWorldTransform(&xLower);

        // Veliki donji levi segment
        DrawSegment(pDC, false, 1.1f, 0.9f, false);

        pDC->SetWorldTransform(&xLower);

        // Vertikalni donji segment
        Rotate(pDC, -45.0f * DEG, false);
        DrawSegment(pDC, false, 0.65f, 0.85f, false);

        pDC->SetWorldTransform(&xJ1);
    }

    // Vrati transformaciju pre crtanja saksije
    pDC->SetWorldTransform(&xInitial);

    // Saksija sa desne strane
    CPen potPen(PS_SOLID, 1, RGB(160, 95, 0));
    CBrush potBrush(RGB(235, 150, 0));
    CBrush lipBrush(RGB(250, 165, 10));

    pDC->SelectObject(&potPen);
    pDC->SelectObject(&potBrush);

    // Telo saksije
    CPoint pot[4] =
    {
        CPoint(410, 205),
        CPoint(500, 220),
        CPoint(500, 280),
        CPoint(410, 295)
    };

    pDC->Polygon(pot, 4);

    // Rub saksije uz spoj sa kaktusom
    pDC->SelectObject(&lipBrush);
    pDC->Rectangle(390, 190, 410, 310);

    pDC->SelectObject(oldBrush);
    pDC->SelectObject(oldPen);
}