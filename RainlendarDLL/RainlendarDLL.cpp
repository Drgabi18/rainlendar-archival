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

// RainlendarDLL.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "RainlendarDLL.h"
#include "CalendarWindow.h"
#include "Error.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CRainlendarDLLApp

BEGIN_MESSAGE_MAP(CRainlendarDLLApp, CWinApp)
	//{{AFX_MSG_MAP(CRainlendarDLLApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRainlendarDLLApp construction

CRainlendarDLLApp::CRainlendarDLLApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRainlendarDLLApp object

CRainlendarDLLApp theApp;

LPCSTR IniPath=NULL;

int initModule(HWND ParentWnd, HINSTANCE dllInst, wharfDataType* wd)
{
	return initModuleEx(ParentWnd, dllInst, wd->lsPath);
}

// This is a custom init-function so that we can pass commandline arguments if ran from exe
int initModuleEx2(HWND ParentWnd, HINSTANCE dllInst, LPCSTR, LPCSTR szIniPath)
{
	IniPath=szIniPath;

	return initModuleEx(ParentWnd, dllInst, NULL);
}

int initModuleEx(HWND ParentWnd, HINSTANCE dllInst, LPCSTR)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	try {
		theApp.m_pMainWnd=new CCalendarWindow;
		if(theApp.m_pMainWnd==NULL) throw ERR_OUTOFMEM;

		((CCalendarWindow*)theApp.m_pMainWnd)->Initialize(ParentWnd, dllInst, IniPath);
	
	} catch(RAIN_ERROR Err) {

		if(Err!=ERR_NONE) {
			CString ErrorStr;
			ErrorStr.Format("Unable to initialize Rainlendar!\nError: %s", GetErrorMessage(Err));

			MessageBox(NULL, ErrorStr, APPNAME, MB_OK | MB_ICONERROR);
		}

		return 1;
	}

	return 0;
}


void quitModule(HINSTANCE)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int Msgs[]={LM_GETREVID, 0};

	if(theApp.m_pMainWnd) {
		CCalendarWindow* CW=(CCalendarWindow*)theApp.m_pMainWnd;
		CW->DestroyWindow();
		delete CW;
		theApp.m_pMainWnd=NULL;
	}
}


/*
** Bang callbacks
*/

extern "C" void RainlendarToggle(HWND, const char* arg)
{
	if(theApp.m_pMainWnd) {
		CCalendarWindow* CW=(CCalendarWindow*)theApp.m_pMainWnd;
		CW->ToggleWindow();
	}
}

extern "C" void RainlendarShow(HWND, const char* arg)
{
	if(theApp.m_pMainWnd) {
		CCalendarWindow* CW=(CCalendarWindow*)theApp.m_pMainWnd;
		CW->ShowWindow();
	}
}

extern "C" void RainlendarHide(HWND, const char* arg)
{
	if(theApp.m_pMainWnd) {
		CCalendarWindow* CW=(CCalendarWindow*)theApp.m_pMainWnd;
		CW->HideWindow();
	}
}

extern "C" void RainlendarRefresh(HWND, const char* arg)
{
	if(theApp.m_pMainWnd) {
		CCalendarWindow* CW=(CCalendarWindow*)theApp.m_pMainWnd;
		CW->RefreshWindow();
	}
}

extern "C" void RainlendarConfig(HWND, const char* arg)
{
	if(theApp.m_pMainWnd) {
		CCalendarWindow* CW=(CCalendarWindow*)theApp.m_pMainWnd;
		CW->ShowConfig();
	}
}

extern "C" void RainlendarQuit(HWND, const char* arg)
{
	if(theApp.m_pMainWnd) {
		CCalendarWindow* CW=(CCalendarWindow*)theApp.m_pMainWnd;
		CW->QuitRainlendar();
	}
}

extern "C" void RainlendarShowNext(HWND, const char* arg)
{
	if(theApp.m_pMainWnd) {
		CCalendarWindow* CW=(CCalendarWindow*)theApp.m_pMainWnd;
		CW->ShowNextMonth();
	}
}

extern "C" void RainlendarShowPrev(HWND, const char* arg)
{
	if(theApp.m_pMainWnd) {
		CCalendarWindow* CW=(CCalendarWindow*)theApp.m_pMainWnd;
		CW->ShowPrevMonth();
	}
}

extern "C" void RainlendarShowCurrent(HWND, const char* arg)
{
	if(theApp.m_pMainWnd) {
		CCalendarWindow* CW=(CCalendarWindow*)theApp.m_pMainWnd;
		CW->ShowCurrentMonth();
	}
}

extern "C" void RainlendarShowMonth(HWND, const char* arg)
{
	char Buffer[256];
	char* Token;
	int Month=0, Year=0;
	
	if(theApp.m_pMainWnd) {
		CCalendarWindow* CW=(CCalendarWindow*)theApp.m_pMainWnd;
		if(arg!=NULL && arg[0]!='\0') {
			strncpy(Buffer, arg, 255);
			Buffer[255]='\0';
			Token=strtok(Buffer, " ");
			if(Token!=NULL) {
				Month=atoi(Token);
				Token=strtok(NULL, " ");
				if(Token!=NULL) {
					Year=atoi(Token);
					CW->ShowMonth(Month, Year);
				} else {
					CW->ShowMonth(Month, -1);
				}
			}
		} else {
			CW->ShowMonth(-1, -1);
		}
	}
}
