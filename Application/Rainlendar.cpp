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
  $Header: /home/cvsroot/Rainlendar/Application/Rainlendar.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: Rainlendar.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.13  2005/03/01 18:28:48  rainy
  Added a check to see it we're already running.

  Revision 1.12  2004/11/06 13:30:00  rainy
  no message

  Revision 1.11  2004/06/10 15:17:21  rainy
  no message

  Revision 1.10  2004/01/28 18:06:19  rainy
  Command line is now empty if not set.

  Revision 1.9  2004/01/25 09:50:21  rainy
  Removed check for Rainlendar.ini

  Revision 1.8  2003/08/09 16:39:46  Rainy
  Added icons to messageboxes.

  Revision 1.7  2003/06/15 09:48:13  Rainy
  Comment changed.

  Revision 1.6  2002/08/03 16:11:25  rainy
  Minor modifications.

  Revision 1.5  2002/05/30 18:23:03  rainy
  Removed Litestep specific stuff

  Revision 1.4  2002/02/27 18:14:19  rainy
  Rainlendar !Bangs can be given as arguments.

  Revision 1.3  2002/01/15 17:58:09  rainy
  Now adds \ to the end of the path if necessary.
  Uses Default folder if command line is empty.

  Revision 1.2  2001/12/23 09:59:35  rainy
  Rainlendar.dll uses a bit different interface.

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#include "resource.h"
#include <windows.h>
#include <stdio.h>

BOOL InitApplication(HINSTANCE);
HWND InitInstance(HINSTANCE, INT);
LONG APIENTRY MainWndProc(HWND, UINT, UINT, LONG);
void Bang(HWND hWnd, const char* command);
HWND FindRainlendar();

static char* WinClass = "DummyRainWClass";
static char* WinName = "Rainlendar control window";

extern "C" {
__declspec( dllimport ) int initModuleEx(HWND ParentWnd, HINSTANCE dllInst, LPCSTR);
__declspec( dllimport ) void quitModule(HINSTANCE dllInst);
__declspec( dllimport ) void Initialize(bool DummyLS, LPCSTR CmdLine);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	// TEST
//	SetProcessWorkingSetSize(GetCurrentProcess(), 10 * 1024 * 1024, 10 * 1024 * 1024);
//	VirtualLock(0, -1);
	// ~TEST

	if(!hPrevInstance) 
	{
		if (!InitApplication(hInstance)) return FALSE;
	}

	HWND hWnd = InitInstance(hInstance, nCmdShow);

	if (lpCmdLine != NULL && lpCmdLine[0] == '!')
	{
		// It's a !bang
		Bang(hWnd, lpCmdLine);
		return 0;
	}

	if (lpCmdLine == NULL || strcmp(lpCmdLine, "-m") != 0)
	{
		// Check for multiple instances
		HWND wnd = FindRainlendar();
		if (wnd != NULL)
		{
			BringWindowToTop(wnd);
			::ShowWindow(wnd, SW_SHOWNORMAL);
			SetForegroundWindow(wnd);
			return 0;
		}
	}
	else
	{
		lpCmdLine = NULL;
	}

	if (lpCmdLine != NULL)
	{
		int len = strlen(lpCmdLine);
		if (len > 240) 
		{
			MessageBox(NULL, "Unable to initialize Rainlendar!\nError: Path-argument is too long!", "Rainlendar", MB_OK | MB_ICONERROR);
			return 0;
		}
	}

	// Initialize from exe
	Initialize(true, lpCmdLine);

	HMODULE module = GetModuleHandle("Rainlendar.dll");
	if(module == NULL)
	{
		MessageBox(NULL, "Unable to load Rainlendar.dll", "Rainlendar", MB_OK | MB_ICONERROR);
		return 0;
	}

	initModuleEx(hWnd, module, NULL);

	while(GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg); 
	} 

	return msg.wParam; 
} 

BOOL InitApplication(HINSTANCE hInstance)
{
	WNDCLASS  wc;

	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC) MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_RAINLENDAR));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
	wc.lpszMenuName =  NULL;
	wc.lpszClassName = WinClass;

	return RegisterClass(&wc);
}

HWND InitInstance(HINSTANCE hInstance, INT nCmdShow)
{
	return CreateWindowEx(
		WS_EX_TOOLWINDOW,
		WinClass,
		WinName,
		WS_POPUP,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
}

HWND FindRainlendar()
{
	// Check if Rainlendar is running
	HWND wnd = FindWindow("Rainlendar", "Rainlendar");
	if (wnd == NULL)
	{
		wnd = FindWindow("Rainlendar", NULL);
	}

	if (wnd == NULL)
	{
		// If the window is OnDesktop, FindWindow doesn't find it.
		HWND ProgmanHwnd = FindWindow("Progman", "Program Manager");
		if (ProgmanHwnd)
		{
			wnd = FindWindowEx(ProgmanHwnd, NULL, "Rainlendar", NULL);
		}
	}

	return wnd;
}

void Bang(HWND hWnd, const char* command)
{
	HWND wnd = FindRainlendar();

	if (wnd != NULL)
	{
		// Check for "!RainlendarShow Activate" because it doesn't work
		// if it's run from the same thread as the window itself.
		if (stricmp("!RainlendarShow Activate", command) == 0)
		{
			BringWindowToTop(wnd);
			::ShowWindow(wnd, SW_SHOWNORMAL);
			SetForegroundWindow(wnd);
			return;
		}

		COPYDATASTRUCT copyData;

		copyData.dwData = 1;
		copyData.cbData = strlen(command) + 1;
		copyData.lpData = (void*)command;

		// Send the bang to the Rainlendar window
		SendMessage(wnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&copyData);
	}
	else
	{
		MessageBox(hWnd, "Rainlendar is not running.\nUnable to send the !bang to it.", "Rainlendar", MB_OK | MB_ICONERROR);
	}
}

LONG APIENTRY MainWndProc(HWND hWnd, UINT message, UINT wParam, LONG lParam)
{
	switch(message) {

	case WM_DESTROY:
		quitModule(NULL);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


