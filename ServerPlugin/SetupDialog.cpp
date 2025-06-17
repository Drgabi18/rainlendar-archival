/*
  Copyright (C) 2004 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/ServerPlugin/SetupDialog.cpp,v 1.1 2004/12/05 18:17:47 rainy Exp $

  $Log: SetupDialog.cpp,v $
  Revision 1.1  2004/12/05 18:17:47  rainy
  Initial version.


*/

#include <windows.h>
#include <tchar.h>
#include "../plugin/RainlendarAPI.h"
#include "resource.h"
#include "ServerPlugin.h"
#include <string>

#ifdef UNICODE
#define tstring wstring
#else
#define tstring string 
#endif

extern HINSTANCE g_Instance;
extern std::tstring g_UserID;
extern std::tstring g_ServerAddress;
extern UINT g_ServerPort;
extern bool g_ServerPoll;
extern UINT g_ServerPollFrequency;

BOOL OnInitSetupDialog(HWND hwndDlg)
{
	SetWindowText(hwndDlg, Plugin_GetName());
	
	HWND widget = GetDlgItem(hwndDlg, IDOK);
	if (widget) SetWindowText(widget, Rainlendar_GetString(_T("General"), 0, _T("OK")));

	widget = GetDlgItem(hwndDlg, IDCANCEL);
	if (widget) SetWindowText(widget, Rainlendar_GetString(_T("General"), 1, _T("Cancel")));

	widget = GetDlgItem(hwndDlg, IDC_SERVER_ID_STATIC);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 0, _T("User ID:")));

	widget = GetDlgItem(hwndDlg, IDC_SERVER_ADDRESS_STATIC);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 1, _T("Server Address:")));

	widget = GetDlgItem(hwndDlg, IDC_SERVER_PORT_STATIC);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 2, _T("Server Port:")));

	widget = GetDlgItem(hwndDlg, IDC_SERVER_POLL);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 3, _T("Poll events from the server")));

	widget = GetDlgItem(hwndDlg, IDC_SERVER_POLL_STATIC);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 4, _T("Polling frequency:")));

	widget = GetDlgItem(hwndDlg, IDC_SERVER_POLL2_STATIC);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 5, _T("minutes")));

	widget = GetDlgItem(hwndDlg, IDC_SERVER_ID);
	if (widget) SetWindowText(widget, g_UserID.c_str());

	widget = GetDlgItem(hwndDlg, IDC_SERVER_ADDRESS);
	if (widget) SetWindowText(widget, g_ServerAddress.c_str());

	widget = GetDlgItem(hwndDlg, IDC_SERVER_PORT);
	if (widget) 
	{
		TCHAR buffer[256];
		_stprintf(buffer, "%i", g_ServerPort);
		SetWindowText(widget, buffer);
	}

	widget = GetDlgItem(hwndDlg, IDC_SERVER_POLL_EDIT);
	if (widget) 
	{
		TCHAR buffer[256];
		_stprintf(buffer, "%i", g_ServerPollFrequency);
		SetWindowText(widget, buffer);

		if (!g_ServerPoll)
		{
			EnableWindow(widget, FALSE);
		}
	}

	CheckDlgButton(hwndDlg, IDC_SERVER_POLL, g_ServerPoll ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;
}

BOOL CALLBACK SetupProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitSetupDialog(hwndDlg);
	
        case WM_CLOSE:
			EndDialog(hwndDlg, IDCANCEL);
            return TRUE; 
			
        case WM_COMMAND:
            switch (LOWORD(wParam)) 
            {
                case IDC_SERVER_POLL:
					if (IsDlgButtonChecked(hwndDlg, IDC_SERVER_POLL))
					{
						EnableWindow(GetDlgItem(hwndDlg, IDC_SERVER_POLL_EDIT), TRUE);
					}
					else
					{
						EnableWindow(GetDlgItem(hwndDlg, IDC_SERVER_POLL_EDIT), FALSE);
					}
					break;

                case IDOK:
					{
						TCHAR buffer[1024];
						HWND widget = GetDlgItem(hwndDlg, IDC_SERVER_ID);
						GetWindowText(widget, buffer, 1024);
						g_UserID = buffer;

						widget = GetDlgItem(hwndDlg, IDC_SERVER_ADDRESS);
						GetWindowText(widget, buffer, 1024);
						g_ServerAddress = buffer;

						widget = GetDlgItem(hwndDlg, IDC_SERVER_PORT);
						GetWindowText(widget, buffer, 1024);
						g_ServerPort = atoi(buffer);
						g_ServerPort = max(1, g_ServerPort);
						g_ServerPort = min(0xFFFF, g_ServerPort);

						if (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_SERVER_POLL))
						{
							g_ServerPoll = true;
						}
						else
						{
							g_ServerPoll = false;
						}

						widget = GetDlgItem(hwndDlg, IDC_SERVER_POLL_EDIT);
						GetWindowText(widget, buffer, 1024);
						g_ServerPollFrequency = atoi(buffer);
						g_ServerPollFrequency = max(1, g_ServerPollFrequency);

						EndDialog(hwndDlg, IDOK);
					}
                    return TRUE; 

                case IDCANCEL:
					EndDialog(hwndDlg, IDCANCEL);
                    return TRUE; 
            }
            break;
    } 
    return FALSE; 
} 

