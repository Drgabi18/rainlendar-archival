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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Host/Rainlendar.cpp,v 1.5 2002/05/30 18:23:03 rainy Exp $

  $Log: Rainlendar.cpp,v $
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

static char* WinClass = "DummyRainWClass";
static char* WinName = "Rainlendar control window";

extern "C" {
__declspec( dllimport ) int initModuleEx(HWND ParentWnd, HINSTANCE dllInst, LPCSTR);
__declspec( dllimport ) void quitModule(HINSTANCE dllInst);
__declspec( dllimport ) void Initialize(bool DummyLS, LPCSTR CmdLine);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char Path[256];
	char Filename[256];
	char Message[2048];
	int Length, Pos;
	MSG msg;

	if(lpCmdLine==NULL || lpCmdLine[0]=='\0') 
	{
		GetCurrentDirectory(256, Filename);
		lpCmdLine = Filename;
	}

	if(!hPrevInstance) 
	{
		if (!InitApplication(hInstance)) return FALSE;
	}

	HWND hWnd = InitInstance(hInstance, nCmdShow);

	if (lpCmdLine[0] == '!')
	{
		// It's a !bang
		Bang(hWnd, lpCmdLine);
		return 0;
	}

	Length=strlen(lpCmdLine);
	if(Length>240) 
	{
		MessageBox(NULL, "Unable to initialize Rainlendar!\nError: Path-argument is too long!", "Rainlendar", MB_OK | MB_ICONERROR);
		return 0;
	}

	// Initialize from exe
	Initialize(true, lpCmdLine);

	// Remove quotes from the commandline
	Pos=0;
	for(int i=0; i<=Length; i++) {
		if(lpCmdLine[i]!='\"') Path[Pos++]=lpCmdLine[i];
	}

	// Check that Rainlendar.ini is found
	strcpy(Filename, Path);
	if(Filename[strlen(Filename) - 1] != '\\') strcat(Filename, "\\");
	strcat(Filename, "Rainlendar.ini");
	Filename[255]='\0';
	FILE* File=fopen(Filename, "r");
	if(NULL==File) {
		sprintf(Message, "Unable to initialize Rainlendar!\nError: Rainlendar.ini-file was not found!\n(%s)", Filename);
		MessageBox(NULL, Message, "Rainlendar", MB_OK | MB_ICONERROR);
		return 0;
	}
	fclose(File);

	HMODULE module = GetModuleHandle("Rainlendar.dll");
	if(module == NULL)
	{
		MessageBox(NULL, "Unable to load Rainlendar.dll", "Rainlendar", MB_OK);
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

void Bang(HWND hWnd, const char* command)
{
	// Check if Rainlendar is running
	HWND wnd = FindWindow("Rainlendar", "Rainlendar");
	if (wnd == NULL)
	{
		wnd = FindWindow("Rainlendar", NULL);
	}

	if (wnd != NULL)
	{
		COPYDATASTRUCT copyData;

		copyData.dwData = 1;
		copyData.cbData = strlen(command) + 1;
		copyData.lpData = (void*)command;

		// Send the bang to the Rainlendar window
		SendMessage(wnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&copyData);
	}
	else
	{
		MessageBox(hWnd, "Rainlendar is not running.\nUnable to send the !bang to it.", "Rainlendar", MB_OK);
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


