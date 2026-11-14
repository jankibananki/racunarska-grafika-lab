
// IND_19728.h : main header file for the IND_19728 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CIND19728App:
// See IND_19728.cpp for the implementation of this class
//

class CIND19728App : public CWinApp
{
public:
	CIND19728App() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CIND19728App theApp;
