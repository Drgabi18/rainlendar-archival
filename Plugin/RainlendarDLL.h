/*
  Copyright (C) 2000 Kimmo Pekkola

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/RainlendarDLL.h,v 1.2 2001/12/23 10:01:13 rainy Exp $

  $Log: RainlendarDLL.h,v $
  Revision 1.2  2001/12/23 10:01:13  rainy
  Rainlendar.dll uses a bit different interface.

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#if !defined(AFX_RAINLENDARDLL_H__B983FF06_2C75_425C_BEEA_7E32FDD0182A__INCLUDED_)
#define AFX_RAINLENDARDLL_H__B983FF06_2C75_425C_BEEA_7E32FDD0182A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "litestep\wharfdata.h"

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

__declspec( dllexport ) int initWharfModule(HWND ParentWnd, HINSTANCE dllInst, wharfDataType *wd);
__declspec( dllexport ) void quitWharfModule(HINSTANCE dll);
__declspec( dllexport ) int initModuleEx(HWND ParentWnd, HINSTANCE dllInst, LPCSTR szPath);
__declspec( dllexport ) void quitModule(HINSTANCE dllInst);
__declspec( dllexport ) void Initialize(bool DummyLS, LPCSTR CmdLine);

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
