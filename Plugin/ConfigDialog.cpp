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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/ConfigDialog.cpp,v 1.1 2002/05/30 18:27:26 rainy Exp $

  $Log: ConfigDialog.cpp,v $
  Revision 1.1  2002/05/30 18:27:26  rainy
  Initial version

*/

#pragma warning(disable: 4786)

#include "ConfigDialog.h"
#include "RainlendarDLL.h"
#include "NetworkThread.h"
#include <Prsht.h>
#include <list>

// Couple of globals
std::list<std::string> g_StatusStrings;
HANDLE  g_Mutex = NULL;
HWND g_StatusWidget = NULL;


BOOL CALLBACK GeneralPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam); 
BOOL CALLBACK ServerPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

VOID OpenConfigDialog(HWND hwndOwner, HINSTANCE instance)
{
    PROPSHEETPAGE psp[2];
    PROPSHEETHEADER psh;

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = PSP_USETITLE;
    psp[0].hInstance = instance;
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_GENERAL);
    psp[0].pszIcon = NULL;
    psp[0].pfnDlgProc = GeneralPageProc;
    psp[0].pszTitle = "General";
    psp[0].lParam = 0;
    psp[0].pfnCallback = NULL;

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = PSP_USETITLE;
    psp[1].hInstance = instance;
    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_SERVER);
    psp[1].pszIcon = NULL;
    psp[1].pfnDlgProc = ServerPageProc;
    psp[1].pszTitle = "Server";
    psp[1].lParam = 0;
    psp[1].pfnCallback = NULL;

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP ;
    psh.hwndParent = hwndOwner;
    psh.hInstance = instance;
    psh.pszIcon = NULL;
    psh.pszCaption = (LPSTR)"Rainlendar Config";
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback = NULL;

    if (PropertySheet(&psh) == IDOK)
	{
		CCalendarWindow::c_Config.WriteConfig(CConfig::WRITE_FULL);
		// Refresh
		SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);
	}

    return;
}

BOOL OnInitGeneralDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	UINT state, check;

	widget = GetDlgItem(window, IDC_X);
	itoa(CCalendarWindow::c_Config.GetX(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_Y);
	itoa(CCalendarWindow::c_Config.GetY(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	switch (CCalendarWindow::c_Config.GetWindowPos())
	{
	case CConfig::WINDOWPOS_ONBOTTOM:
		check = IDC_WINDOW_ONBOTTOM;
		break;

	case CConfig::WINDOWPOS_ONTOP:
		check = IDC_WINDOW_ONTOP;
		break;

	case CConfig::WINDOWPOS_NORMAL:
	default:
		check = IDC_WINDOW_NORMAL;
		break;
	}
	CheckRadioButton(window, IDC_WINDOW_ONBOTTOM, IDC_WINDOW_ONTOP, check);

	state = CCalendarWindow::c_Config.GetEventToolTips() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENT_TOOLTIP, state);
	state = CCalendarWindow::c_Config.GetEventMessageBox() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENT_MESSAGEBOX, state);
	widget = GetDlgItem(window, IDC_EVENT_EXECUTE);
	SetWindowText(widget, CCalendarWindow::c_Config.GetEventExecute().c_str());

	state = CCalendarWindow::c_Config.GetMovable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_WINDOW_MOVABLE, state);
	state = CCalendarWindow::c_Config.GetMouseHide() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_WINDOW_MOUSEHIDE, state);

	state = CCalendarWindow::c_Config.GetStartHidden() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_START_HIDDEN, state);
	state = CCalendarWindow::c_Config.GetStartFromMonday() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_START_FROM_MONDAY, state);
	state = CCalendarWindow::c_Config.GetDisableHotkeys() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_DISABLE_HOTKEYS, state);
	state = CCalendarWindow::c_Config.GetUseWindowName() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_USE_WINDOW_NAME, state);
	state = CCalendarWindow::c_Config.GetPollWallpaper() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_POLL_WALLPAPER, state);

	return TRUE;
}

BOOL OnOKGeneralDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	widget = GetDlgItem(window, IDC_X);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetX(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_Y);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetY(atoi(tmpSz));

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_WINDOW_ONBOTTOM))
	{
		CCalendarWindow::c_Config.SetWindowPos(CConfig::WINDOWPOS_ONBOTTOM);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_WINDOW_ONTOP))
	{
		CCalendarWindow::c_Config.SetWindowPos(CConfig::WINDOWPOS_ONTOP);
	}
	else
	{
		CCalendarWindow::c_Config.SetWindowPos(CConfig::WINDOWPOS_NORMAL);
	}

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_TOOLTIP));
	CCalendarWindow::c_Config.SetEventToolTips(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_MESSAGEBOX));
	CCalendarWindow::c_Config.SetEventMessageBox(state);
	widget = GetDlgItem(window, IDC_EVENT_EXECUTE);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetEventExecute(tmpSz);

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WINDOW_MOVABLE));
	CCalendarWindow::c_Config.SetMovable(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WINDOW_MOUSEHIDE));
	CCalendarWindow::c_Config.SetMouseHide(state);

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_START_HIDDEN));
	CCalendarWindow::c_Config.SetStartHidden(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_START_FROM_MONDAY));
	CCalendarWindow::c_Config.SetStartFromMonday(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_DISABLE_HOTKEYS));
	CCalendarWindow::c_Config.SetDisableHotkeys(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_USE_WINDOW_NAME));
	CCalendarWindow::c_Config.SetUseWindowName(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_POLL_WALLPAPER));
	CCalendarWindow::c_Config.SetPollWallpaper(state);

	return TRUE;
}

BOOL CALLBACK GeneralPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitGeneralDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				{
					return OnOKGeneralDialog(hwndDlg);
				}
			}
	}
    return FALSE; 
} 

void UpdateServerWidgets(HWND window)
{
	if (IsDlgButtonChecked(window, IDC_SERVER_ENABLE) == BST_CHECKED)
	{
		// Enable all
		EnableWindow(GetDlgItem(window, IDC_SERVER_ADDRESS), TRUE);
		EnableWindow(GetDlgItem(window, IDC_SERVER_PORT), TRUE);
		EnableWindow(GetDlgItem(window, IDC_SERVER_ID), TRUE);
		EnableWindow(GetDlgItem(window, IDC_SERVER_STARTUP), TRUE);
		EnableWindow(GetDlgItem(window, IDC_SERVER_FREQUENCY), TRUE);
		EnableWindow(GetDlgItem(window, IDC_SERVER_SYNC), TRUE);
		EnableWindow(GetDlgItem(window, IDC_SERVER_STATUS), TRUE);
	}
	else
	{
		// Disable all
		EnableWindow(GetDlgItem(window, IDC_SERVER_ADDRESS), FALSE);
		EnableWindow(GetDlgItem(window, IDC_SERVER_PORT), FALSE);
		EnableWindow(GetDlgItem(window, IDC_SERVER_ID), FALSE);
		EnableWindow(GetDlgItem(window, IDC_SERVER_STARTUP), FALSE);
		EnableWindow(GetDlgItem(window, IDC_SERVER_FREQUENCY), FALSE);
		EnableWindow(GetDlgItem(window, IDC_SERVER_SYNC), FALSE);
		EnableWindow(GetDlgItem(window, IDC_SERVER_STATUS), FALSE);
	}
}

BOOL OnInitServerDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	UINT state;

	g_StatusWidget = GetDlgItem(window, IDC_SERVER_STATUS);
	// Fill the widget
	if (g_Mutex)
	{
		DWORD result = WaitForSingleObject(g_Mutex, 1000L);
		if (result == WAIT_OBJECT_0)
		{
			// Add the string to the widget
			std::list<std::string>::iterator i = g_StatusStrings.begin();
			for( ; i != g_StatusStrings.end(); i++)
			{
				SendMessage(g_StatusWidget, LB_ADDSTRING, 0, (LPARAM)((*i).c_str())); 
			}

			ReleaseMutex(g_Mutex);
		}
	}

	state = CCalendarWindow::c_Config.GetServerEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_SERVER_ENABLE, state);

	widget = GetDlgItem(window, IDC_SERVER_ADDRESS);
	SetWindowText(widget, CCalendarWindow::c_Config.GetServerAddress().c_str());

	widget = GetDlgItem(window, IDC_SERVER_PORT);
	itoa(CCalendarWindow::c_Config.GetServerPort(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_SERVER_ID);
	SetWindowText(widget, CCalendarWindow::c_Config.GetServerID().c_str());

	widget = GetDlgItem(window, IDC_SERVER_FREQUENCY);
	itoa(CCalendarWindow::c_Config.GetServerFrequency(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	state = CCalendarWindow::c_Config.GetServerStartup() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_SERVER_STARTUP, state);

	UpdateServerWidgets(window);

	return TRUE;
}

BOOL OnOKServerDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_SERVER_ENABLE));
	CCalendarWindow::c_Config.SetServerEnable(state);

	widget = GetDlgItem(window, IDC_SERVER_ADDRESS);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetServerAddress(tmpSz);

	widget = GetDlgItem(window, IDC_SERVER_PORT);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetServerPort(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_SERVER_FREQUENCY);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetServerFrequency(atoi(tmpSz));

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_SERVER_STARTUP));
	CCalendarWindow::c_Config.SetServerStartup(state);

	widget = GetDlgItem(window, IDC_SERVER_ID);
	GetWindowText(widget, tmpSz, 256);

	// Check the ID so that it doesn't contain illegal chars
	bool changed = false;
	for (int i = 0; i < strlen(tmpSz); i++)
	{
		if (((tmpSz[i] < '0') || 
		    (tmpSz[i] > '9' && tmpSz[i] < 'A') ||
		    (tmpSz[i] > 'Z' && tmpSz[i] < 'a') ||
		    (tmpSz[i] > 'z')) && tmpSz[i] != '_')
		{
			tmpSz[i] = '_';
			changed = true;
		}
	}

	if (changed)
	{
		MessageBox(NULL, "Please use only numbers (0-9)\nand letters (A-Z, a-z) in the ID.\nThe illegal chars are automatically changed to '_'", "Rainlendar", MB_OK);
	}

	CCalendarWindow::c_Config.SetServerID(tmpSz);

	return TRUE;
}

BOOL CALLBACK ServerPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitServerDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				{
					return OnOKServerDialog(hwndDlg);
				}
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_SERVER_SYNC:
				// Resync all events
				GetRainlendar()->GetCalendarWindow().ConnectServer(REQUEST_SYNCRONIZATION);
				return TRUE;

			case IDC_SERVER_ENABLE:
				UpdateServerWidgets(hwndDlg);
				return TRUE;
			}
			break;
	}
    return FALSE; 
}

void AddStatusString(const char* string)
{
	DWORD result;
	char tmpSz[256];
	SYSTEMTIME time;

	// Create a mutex if it's not yet created
	if (g_Mutex == NULL)
	{
		g_Mutex = CreateMutex (NULL, FALSE, "AddStatusStringMutex");
	}

	GetLocalTime(&time);
	sprintf(tmpSz, "[%02i:%02i:%02i] ", time.wHour, time.wMinute, time.wSecond); 
	
	std::string str = tmpSz;
	str += string;

	result = WaitForSingleObject(g_Mutex, 1000L);
	if (result == WAIT_OBJECT_0)
	{
		if (IsWindow(g_StatusWidget))
		{
			// Add the string to the widget
			SendMessage(g_StatusWidget, LB_ADDSTRING, 0, (LPARAM)str.c_str()); 
		}

		g_StatusStrings.push_back(str);
		ReleaseMutex(g_Mutex);
	}
}