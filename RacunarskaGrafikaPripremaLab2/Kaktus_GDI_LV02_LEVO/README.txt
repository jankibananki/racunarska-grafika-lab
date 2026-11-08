LV02 - GDI KAKTUS (MFC View files + original EMF resources)

1. Kreiraj Windows Visual Studio MFC SDI projekat ili kopiraj svoj projekat LV01.
2. Ako koristis originalni projekat RacunarskaGrafikaLab1-Priprema:
   - napravi backup starih View.h i View.cpp;
   - zameni ta dva fajla istoimenim fajlovima iz ovog paketa;
   - kopiraj ceo RESURSI podfolder u folder gde ti je .vcxproj.
3. Ako se projekat zove drugacije, prilagodi #include imena i View klasu u oba fajla.
4. Sve transformacije u DrawFigure/DrawCactusPart koriste LEFTMULTIPLY (false).
   Radi rotacije oko (250,351) redosled je Translate(+pivot), Rotate, Translate(-pivot).
5. Build x64 Debug, Ctrl+F5. A/D rotacija leve grupe, J/L desne, G grid, R reset.
6. Ako EMF fajlovi nisu pronadjeni, kopiraj RESURSI folder pored .exe datoteke.

NAPOMENA: Ovo je kompletna ilustrativna MFC View implementacija zasnovana na
referentnoj slici. Geometrija je priblizna i moguce su vizuelne korekcije.
Nije kompajlirano na Windowsu u ovom okruzenju.

Napomena: Kao u prethodnoj verziji, rotiraju se leve/desne grupe.
Za rotaciju iskljucivo svetlih segmenata bice potrebna dodatna izmena.
