
// RacunarskaGrafikaLab1-Priprema.h : main header file for the RacunarskaGrafikaLab1-Priprema application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CRacunarskaGrafikaLab1PripremaApp:
// See RacunarskaGrafikaLab1-Priprema.cpp for the implementation of this class
//

class CRacunarskaGrafikaLab1PripremaApp : public CWinApp
{
public:
	CRacunarskaGrafikaLab1PripremaApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CRacunarskaGrafikaLab1PripremaApp theApp;
