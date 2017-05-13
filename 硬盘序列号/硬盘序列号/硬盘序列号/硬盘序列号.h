
// ”≤≈Ã–Ú¡–∫≈.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// C”≤≈Ã–Ú¡–∫≈App:
// See ”≤≈Ã–Ú¡–∫≈.cpp for the implementation of this class
//

class C”≤≈Ã–Ú¡–∫≈App : public CWinApp
{
public:
	C”≤≈Ã–Ú¡–∫≈App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern C”≤≈Ã–Ú¡–∫≈App theApp;