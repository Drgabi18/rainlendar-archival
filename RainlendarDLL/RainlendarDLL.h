// RainlendarDLL.h : main header file for the RAINLENDARDLL DLL
//

#if !defined(AFX_RAINLENDARDLL_H__B983FF06_2C75_425C_BEEA_7E32FDD0182A__INCLUDED_)
#define AFX_RAINLENDARDLL_H__B983FF06_2C75_425C_BEEA_7E32FDD0182A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "..\..\..\3rdparty\ls-b24\litestep\wharfdata.h"

#define	APPNAME "Rainlendar"
#define	MAX_LINE_LENGTH 4096

/////////////////////////////////////////////////////////////////////////////
// CRainlendarDLLApp
// See RainlendarDLL.cpp for the implementation of this class
//

class CRainlendarDLLApp : public CWinApp
{
public:
	CRainlendarDLLApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRainlendarDLLApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CRainlendarDLLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifdef __cplusplus
extern "C" {
#endif

__declspec( dllexport ) int initModule(HWND ParentWnd, HINSTANCE dllInst, wharfDataType* wd);
__declspec( dllexport ) int initModuleEx(HWND ParentWnd, HINSTANCE dllInst, LPCSTR szPath);
__declspec( dllexport ) int initModuleEx2(HWND ParentWnd, HINSTANCE dllInst, LPCSTR szPath, LPCSTR szIniPath);
__declspec( dllexport ) void quitModule(HINSTANCE dllInst);

void RainlendarToggle(HWND caller, const char* arg);
void RainlendarHide(HWND caller, const char* arg);
void RainlendarShow(HWND caller, const char* arg);
void RainlendarRefresh(HWND caller, const char* arg);
void RainlendarConfig(HWND caller, const char* arg);
void RainlendarQuit(HWND caller, const char* arg);
void RainlendarShowNext(HWND caller, const char* arg);
void RainlendarShowPrev(HWND caller, const char* arg);
void RainlendarShowCurrent(HWND caller, const char* arg);
void RainlendarShowMonth(HWND caller, const char* arg);

#ifdef __cplusplus
}
#endif


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAINLENDARDLL_H__B983FF06_2C75_425C_BEEA_7E32FDD0182A__INCLUDED_)
