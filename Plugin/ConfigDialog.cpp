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
  $Header: /home/cvsroot/Rainlendar/Plugin/ConfigDialog.cpp,v 1.19 2004/12/05 18:34:12 rainy Exp $

  $Log: ConfigDialog.cpp,v $
  Revision 1.19  2004/12/05 18:34:12  rainy
  Added some more stuff.

  Revision 1.18  2004/11/06 13:38:58  rainy
  no message

  Revision 1.17  2004/01/28 18:05:21  rainy
  Todo is shown/hidden with OK.

  Revision 1.16  2004/01/25 10:00:05  rainy
  OnDesktop / Native transparency are disabled.

  Revision 1.15  2004/01/10 15:20:16  rainy
  Added hotkey and few check boxes.

  Revision 1.14  2003/12/20 22:24:39  rainy
  Added stuff for the message box.

  Revision 1.13  2003/10/27 17:37:12  Rainy
  Config is now singleton.

  Revision 1.12  2003/10/04 14:49:07  Rainy
  StartFromJanuary sets the Previous Months field to zero..

  Revision 1.11  2003/08/23 09:14:36  Rainy
  Added translations to settings group and outlook. hotkey

  Revision 1.10  2003/08/09 16:38:27  Rainy
  Added hotkeys.

  Revision 1.9  2003/06/15 09:44:33  Rainy
  Added Layout tab.

  Revision 1.8  2003/05/25 18:08:50  Rainy
  Added tooltip separator.

  Revision 1.7  2003/05/07 19:15:29  rainy
  Added few new options.

  Revision 1.6  2003/03/22 20:30:54  rainy
  Refresh on resolution change is optional.

  Revision 1.5  2002/11/25 17:11:13  rainy
  Removed a space :-)

  Revision 1.4  2002/11/12 18:03:27  rainy
  Added widgets for the native transparency

  Revision 1.3  2002/08/24 11:12:58  rainy
  Added Copy to clipboard.

  Revision 1.2  2002/08/03 16:18:29  rainy
  Added snap edges.

  Revision 1.1  2002/05/30 18:27:26  rainy
  Initial version

*/

#pragma warning(disable: 4786)
#pragma warning(disable: 4996)

#include "ConfigDialog.h"
#include "RainlendarDLL.h"
#include "PluginManager.h"
#include <Prsht.h>
#include <list>
#include <string.h>
#include <tchar.h>
#include <io.h>
#include <Oleacc.h>
#include <comutil.h>
#include <assert.h>

static HWND g_Dialog = NULL;

BOOL CALLBACK LayoutPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam); 
BOOL CALLBACK GeneralPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam); 
BOOL CALLBACK HotkeysPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PluginsPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

enum CONFIG_MESSAGE
{
	CONFIG_MESSAGE_TRANSPARENCY_ON,
	CONFIG_MESSAGE_TRANSPARENCY_OFF
};

VOID OpenConfigDialog(HWND hwndOwner, HINSTANCE instance)
{
	if (g_Dialog != NULL)
	{
		FLASHWINFO fi;
		fi.cbSize = sizeof(FLASHWINFO);
		fi.hwnd = g_Dialog;
		fi.dwFlags = FLASHW_CAPTION;
		fi.dwTimeout = 0;
		fi.uCount = 5;
		FlashWindowEx(&fi);
		return;
	}
	
	PROPSHEETPAGE psp[4];
    PROPSHEETHEADER psh;

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = PSP_USETITLE | PSP_PREMATURE;
    psp[0].hInstance = instance;
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_GENERAL);
    psp[0].pszIcon = NULL;
    psp[0].pfnDlgProc = GeneralPageProc;
    psp[0].pszTitle = CCalendarWindow::c_Language.GetString("General", 3);
    psp[0].lParam = 0;
    psp[0].pfnCallback = NULL;

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = PSP_USETITLE | PSP_PREMATURE;
    psp[1].hInstance = instance;
    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_LAYOUT);
    psp[1].pszIcon = NULL;
    psp[1].pfnDlgProc = LayoutPageProc;
    psp[1].pszTitle = CCalendarWindow::c_Language.GetString("General", 4);
    psp[1].lParam = 0;
    psp[1].pfnCallback = NULL;

    psp[2].dwSize = sizeof(PROPSHEETPAGE);
    psp[2].dwFlags = PSP_USETITLE | PSP_PREMATURE;
    psp[2].hInstance = instance;
    psp[2].pszTemplate = MAKEINTRESOURCE(IDD_HOTKEYS);
    psp[2].pszIcon = NULL;
    psp[2].pfnDlgProc = HotkeysPageProc;
    psp[2].pszTitle = CCalendarWindow::c_Language.GetString("General", 15);
    psp[2].lParam = 0;
    psp[2].pfnCallback = NULL;

    psp[3].dwSize = sizeof(PROPSHEETPAGE);
    psp[3].dwFlags = PSP_USETITLE | PSP_PREMATURE;
    psp[3].hInstance = instance;
    psp[3].pszTemplate = MAKEINTRESOURCE(IDD_PLUGINS);
    psp[3].pszIcon = NULL;
    psp[3].pfnDlgProc = PluginsPageProc;
    psp[3].pszTitle = CCalendarWindow::c_Language.GetString("General", 19);
    psp[3].lParam = 0;
    psp[3].pfnCallback = NULL;

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP ;
    psh.hwndParent = hwndOwner;
    psh.hInstance = instance;
    psh.pszIcon = NULL;
    psh.pszCaption = (LPSTR)CCalendarWindow::c_Language.GetString("General", 6);
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback = NULL;

    if (PropertySheet(&psh) == IDOK)
	{
		CConfig::Instance().WriteConfig(CConfig::WRITE_FULL);
		GetRainlendar()->GetCalendarWindow().RefreshAll();
	}
	else
	{
		if (CConfig::Instance().GetTransparentOnMouseOver())
		{
			GetRainlendar()->GetCalendarWindow().SetTransparency(255);
		}
		else
		{
			GetRainlendar()->GetCalendarWindow().SetTransparency(CConfig::Instance().GetTransparencyValue());
		}
	}

	g_Dialog = NULL;
}

//////////////////////////////////////////////////////////////////////////
//
// GENERAL
//
//////////////////////////////////////////////////////////////////////////
const std::string& GetLog()
{
	static std::string log;
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	log = buffer;
	log.replace(log.length() - 4, 4, ".log");
	return log;
}

bool CheckLog()
{
	// Check if the log-file exists
	if (_access(GetLog().c_str(), 0) != -1)
	{
		return true;
	}
	return false;
}

void UpdateGeneralWidgets(HWND window)
{
	if (IsDlgButtonChecked(window, IDC_NATIVE_TRANSPARENCY) == BST_CHECKED)
	{
		EnableWindow(GetDlgItem(window, IDC_TRANSPARENCY_SLIDER), TRUE);
		EnableWindow(GetDlgItem(window, IDC_TRANSPARENT_STATIC), TRUE);
		EnableWindow(GetDlgItem(window, IDC_TRANSPARENT_STATIC2), TRUE);
		PropSheet_QuerySiblings(GetParent(window), CONFIG_MESSAGE_TRANSPARENCY_ON, 0);
	}
	else
	{
		EnableWindow(GetDlgItem(window, IDC_TRANSPARENCY_SLIDER), FALSE);
		EnableWindow(GetDlgItem(window, IDC_TRANSPARENT_STATIC), FALSE);
		EnableWindow(GetDlgItem(window, IDC_TRANSPARENT_STATIC2), FALSE);
		PropSheet_QuerySiblings(GetParent(window), CONFIG_MESSAGE_TRANSPARENCY_OFF, 0);
	}

	if (CheckLog())
	{
		EnableWindow(GetDlgItem(window, IDC_SHOW_LOG_BUTTON), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(window, IDC_SHOW_LOG_BUTTON), FALSE);
	}
}

BOOL OnInitGeneralDialog(HWND window) 
{
	UINT state;
	HWND widget;

	state = CConfig::Instance().GetEventToolTips() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENT_TOOLTIP, state);
	state = CConfig::Instance().GetShowSingleEvent() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENT_SHOW_SINGLE, state);
	state = CConfig::Instance().GetShowAllEvents() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENT_SHOW_ALL, state);
	state = CConfig::Instance().GetDisableHotkeys() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_DISABLE_HOTKEYS, state);
	state = CConfig::Instance().GetPollWallpaper() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_POLL_WALLPAPER, state);
	state = CConfig::Instance().GetSnapEdges() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_SNAP_EDGES, state);
	state = CConfig::Instance().GetRefreshOnResolutionChange() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_REFRESH_ON_RESOLUTION_CHANGE, state);
	state = CConfig::Instance().GetTooltipSeparator() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_TOOLTIP_SEPARATOR, state);
	state = CConfig::Instance().GetShowTrayIcon() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_SHOW_TRAYICON, state);

	state = CConfig::Instance().GetNativeTransparency() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_NATIVE_TRANSPARENCY, state);

	widget = GetDlgItem(window, IDC_TRANSPARENCY_SLIDER);
	SendMessage(widget, TBM_SETRANGE, TRUE, MAKELONG(0, 255));
	SendMessage(widget, TBM_SETPOS, TRUE, CConfig::Instance().GetTransparencyValue());

	// Set localized strings
	widget = GetDlgItem(GetParent(window), IDOK);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("General", 0));
	widget = GetDlgItem(GetParent(window), IDCANCEL);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("General", 1));
	widget = GetDlgItem(window, IDC_EVENT_TOOLTIP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 0));
	widget = GetDlgItem(window, IDC_DISABLE_HOTKEYS);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 4));
	widget = GetDlgItem(window, IDC_POLL_WALLPAPER);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 6));
	widget = GetDlgItem(window, IDC_OTHER_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 9));
	widget = GetDlgItem(window, IDC_SNAP_EDGES);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 10));
	widget = GetDlgItem(window, IDC_NATIVE_TRANSPARENCY);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 11));
	widget = GetDlgItem(window, IDC_REFRESH_ON_RESOLUTION_CHANGE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 12));
	widget = GetDlgItem(window, IDC_TOOLTIP_SEPARATOR);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 15));
	widget = GetDlgItem(window, IDC_EVENT_SHOW_SINGLE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 23));
	widget = GetDlgItem(window, IDC_EVENT_SHOW_ALL);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 22));
	widget = GetDlgItem(window, IDC_SHOW_TRAYICON);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 25));

	widget = GetDlgItem(window, IDC_TRANSPARENCY_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 26));
	widget = GetDlgItem(window, IDC_TRANSPARENT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 28));
	widget = GetDlgItem(window, IDC_TRANSPARENT_STATIC2);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 27));

	widget = GetDlgItem(window, IDC_ADVANCED_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 33));
	widget = GetDlgItem(window, IDC_ADVANCED_BUTTON);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 34));

	widget = GetDlgItem(window, IDC_LOG_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 29));
	widget = GetDlgItem(window, IDC_ENABLE_LOG_BUTTON);
	if (CheckLog())
	{
		if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 31));
	}
	else
	{
		if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 30));
	}

	widget = GetDlgItem(window, IDC_SHOW_LOG_BUTTON);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 32));

	if (!CCalendarWindow::Is2k())
	{
		// Disable the button if we're running wintendo
		EnableWindow(GetDlgItem(window, IDC_NATIVE_TRANSPARENCY), FALSE);
		CheckDlgButton(window, IDC_NATIVE_TRANSPARENCY, BST_UNCHECKED);
	}

	UpdateGeneralWidgets(window);

	if (CConfig::Instance().GetRememberDialogPositions())
	{
		POINT pos = CConfig::Instance().GetDialogPosition(CConfig::DIALOG_CONFIG);
		SetWindowPos(GetParent(window), NULL, pos.x, pos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}

	return TRUE;
}

BOOL OnOKGeneralDialog(HWND window) 
{
	bool state;
	HWND widget;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_TOOLTIP));
	CConfig::Instance().SetEventToolTips(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_SHOW_SINGLE));
	CConfig::Instance().SetShowSingleEvent(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_SHOW_ALL));
	CConfig::Instance().SetShowAllEvents(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_DISABLE_HOTKEYS));
	CConfig::Instance().SetDisableHotkeys(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_POLL_WALLPAPER));
	CConfig::Instance().SetPollWallpaper(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_SNAP_EDGES));
	CConfig::Instance().SetSnapEdges(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_REFRESH_ON_RESOLUTION_CHANGE));
	CConfig::Instance().SetRefreshOnResolutionChange(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_TOOLTIP_SEPARATOR));
	CConfig::Instance().SetTooltipSeparator(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_SHOW_TRAYICON));
	CConfig::Instance().SetShowTrayIcon(state);

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_NATIVE_TRANSPARENCY));
	CConfig::Instance().SetNativeTransparency(state);

	widget = GetDlgItem(window, IDC_TRANSPARENCY_SLIDER);
	UINT pos = SendMessage(widget, TBM_GETPOS, 0, 0);
	CConfig::Instance().SetTransparencyValue(pos);

	return TRUE;
}

BOOL CALLBACK GeneralPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			g_Dialog = GetParent(hwndDlg);
			return OnInitGeneralDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				{
					return OnOKGeneralDialog(hwndDlg);
				}
			}
			break;

		case WM_DESTROY:
			{
				RECT rc;
				GetWindowRect(GetParent(hwndDlg), &rc);	// Get the sheet position (i.e. parent)
				CConfig::Instance().SetDialogPosition(CConfig::DIALOG_CONFIG, rc.left, rc.top);
				CConfig::Instance().WriteConfig(CConfig::WRITE_DIALOG_POS);
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_NATIVE_TRANSPARENCY:
				UpdateGeneralWidgets(hwndDlg);
				return TRUE;

			case IDC_ENABLE_LOG_BUTTON:
				{
					if (CheckLog())
					{
						// Delete the log-file
						if (DeleteFile(GetLog().c_str()))
						{
							ResetLoggingFlag();	// Re-enable logging

							HWND widget = GetDlgItem(hwndDlg, IDC_ENABLE_LOG_BUTTON);
							if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 30));
						}
					}
					else
					{
						// Create the log file
						HANDLE file = CreateFile(GetLog().c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
						if (file)
						{
							CloseHandle(file);
							ResetLoggingFlag();	// Re-enable logging

							HWND widget = GetDlgItem(hwndDlg, IDC_ENABLE_LOG_BUTTON);
							if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 31));
						}
					}
					UpdateGeneralWidgets(hwndDlg);
				}
				return TRUE;

			case IDC_ADVANCED_BUTTON:
				{
					// Close the dialog and open the skin.ini in notepad
					std::string command = CConfig::Instance().GetConfigEditor();
					command += " \"";
					command += CConfig::Instance().GetPath();
					command += "\\Rainlendar.ini\"";
					LSExecute(hwndDlg, command.c_str(), SW_SHOWNORMAL);

					PostMessage(GetParent(hwndDlg), WM_COMMAND, IDCANCEL, NULL);
					return TRUE;
				}

			case IDC_SHOW_LOG_BUTTON:
				{
					std::string command = CConfig::Instance().GetConfigEditor();
					command += " \"";
					command += GetLog();
					LSExecute(hwndDlg, command.c_str(), SW_SHOWNORMAL);
				}
				return TRUE;
			}

			break;

		case WM_HSCROLL:
			{
				HWND widget = GetDlgItem(hwndDlg, IDC_TRANSPARENCY_SLIDER);
				UINT pos = SendMessage(widget, TBM_GETPOS, 0, 0);
				GetRainlendar()->GetCalendarWindow().SetTransparency(pos);
				return TRUE;
			}
			break;
	}
    return FALSE; 
} 

//////////////////////////////////////////////////////////////////////////
//
// LAYOUT
//
//////////////////////////////////////////////////////////////////////////

void UpdateLayoutWidgets(HWND window)
{
	if (IsDlgButtonChecked(window, IDC_LAYOUT_JANUARY) == BST_CHECKED)
	{
		EnableWindow(GetDlgItem(window, IDC_PREVIOUS_MONTHS), FALSE);
	}
	else
	{
		EnableWindow(GetDlgItem(window, IDC_PREVIOUS_MONTHS), TRUE);
	}

	if (!CCalendarWindow::Is2k())
	{
		// Disable the buttons if we're running wintendo
		EnableWindow(GetDlgItem(window, IDC_WINDOW_MOUSEHIDE), TRUE);
		EnableWindow(GetDlgItem(window, IDC_CLICKTROUGH), FALSE);
		EnableWindow(GetDlgItem(window, IDC_OPAQUE_ON_MOUSE_OVER), FALSE);
		EnableWindow(GetDlgItem(window, IDC_TRANSPARENT_ON_MOUSE_OVER), FALSE);
	}
	else
	{
		if (IsDlgButtonChecked(window, IDC_WINDOW_MOUSEHIDE) == BST_CHECKED)
		{
			EnableWindow(GetDlgItem(window, IDC_WINDOW_MOUSEHIDE), TRUE);
			EnableWindow(GetDlgItem(window, IDC_CLICKTROUGH), FALSE);
			EnableWindow(GetDlgItem(window, IDC_OPAQUE_ON_MOUSE_OVER), FALSE);
			EnableWindow(GetDlgItem(window, IDC_TRANSPARENT_ON_MOUSE_OVER), FALSE);
		}
		else
		{
			EnableWindow(GetDlgItem(window, IDC_CLICKTROUGH), TRUE);

			if (IsDlgButtonChecked(window, IDC_CLICKTROUGH) == BST_CHECKED)
			{
				EnableWindow(GetDlgItem(window, IDC_WINDOW_MOUSEHIDE), FALSE);
			}
			else
			{
				EnableWindow(GetDlgItem(window, IDC_WINDOW_MOUSEHIDE), TRUE);
			}

			if (IsDlgButtonChecked(window, IDC_OPAQUE_ON_MOUSE_OVER) == BST_CHECKED)
			{
				EnableWindow(GetDlgItem(window, IDC_WINDOW_MOUSEHIDE), FALSE);
				EnableWindow(GetDlgItem(window, IDC_OPAQUE_ON_MOUSE_OVER), TRUE);
				EnableWindow(GetDlgItem(window, IDC_TRANSPARENT_ON_MOUSE_OVER), FALSE);
			}
			else if (IsDlgButtonChecked(window, IDC_TRANSPARENT_ON_MOUSE_OVER) == BST_CHECKED)
			{
				EnableWindow(GetDlgItem(window, IDC_WINDOW_MOUSEHIDE), FALSE);
				EnableWindow(GetDlgItem(window, IDC_OPAQUE_ON_MOUSE_OVER), FALSE);
				EnableWindow(GetDlgItem(window, IDC_TRANSPARENT_ON_MOUSE_OVER), TRUE);
			}
			else
			{
				EnableWindow(GetDlgItem(window, IDC_OPAQUE_ON_MOUSE_OVER), TRUE);
				EnableWindow(GetDlgItem(window, IDC_TRANSPARENT_ON_MOUSE_OVER), TRUE);
			}
		}
	}
}

BOOL OnInitLayoutDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	UINT check, state;

	// Set localized strings
	widget = GetDlgItem(window, IDC_POSITION_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 7));
	widget = GetDlgItem(window, IDC_WINDOW_ONBOTTOM);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 0));
	widget = GetDlgItem(window, IDC_WINDOW_ONTOP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 2));
	widget = GetDlgItem(window, IDC_WINDOW_ONDESKTOP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 8));
	widget = GetDlgItem(window, IDC_WINDOW_NORMAL);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 1));

	widget = GetDlgItem(window, IDC_SETTINGS_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 19));
	widget = GetDlgItem(window, IDC_WINDOW_MOVABLE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 3));
	widget = GetDlgItem(window, IDC_WINDOW_MOUSEHIDE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 4));
	widget = GetDlgItem(window, IDC_REMEMBER_DIALOG_POSITIONS);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 14));
	widget = GetDlgItem(window, IDC_CLICKTROUGH);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 20));
	widget = GetDlgItem(window, IDC_OPAQUE_ON_MOUSE_OVER);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 25));
	widget = GetDlgItem(window, IDC_TRANSPARENT_ON_MOUSE_OVER);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 26));

	widget = GetDlgItem(window, IDC_LAYOUT_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 9));
	widget = GetDlgItem(window, IDC_VERTICAL_COUNT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 10));
	widget = GetDlgItem(window, IDC_HORIZONTAL_COUNT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 11));
	widget = GetDlgItem(window, IDC_PREVIOUS_MONTHS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 12));
	widget = GetDlgItem(window, IDC_LAYOUT_JANUARY);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 13));
	
	widget = GetDlgItem(window, IDC_EVENTLIST_DAYS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 21));
	widget = GetDlgItem(window, IDC_EVENTLIST_DAYS_STATIC2);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 22));
	widget = GetDlgItem(window, IDC_EVENTLIST_SUBSTITUTE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 23));
	widget = GetDlgItem(window, IDC_EVENTLIST_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 24));

	switch (CConfig::Instance().GetWindowPos())
	{
	case CConfig::WINDOWPOS_ONBOTTOM:
		check = IDC_WINDOW_ONBOTTOM;
		break;

	case CConfig::WINDOWPOS_ONTOP:
		check = IDC_WINDOW_ONTOP;
		break;

	case CConfig::WINDOWPOS_ONDESKTOP:
		check = IDC_WINDOW_ONDESKTOP;
		break;

	case CConfig::WINDOWPOS_NORMAL:
	default:
		check = IDC_WINDOW_NORMAL;
		break;
	}
	CheckRadioButton(window, IDC_WINDOW_ONDESKTOP, IDC_WINDOW_ONTOP, check);

	state = CConfig::Instance().GetMovable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_WINDOW_MOVABLE, state);
	state = CConfig::Instance().GetMouseHide() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_WINDOW_MOUSEHIDE, state);
	state = CConfig::Instance().GetRememberDialogPositions() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_REMEMBER_DIALOG_POSITIONS, state);
	state = CConfig::Instance().GetClickThrough() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_CLICKTROUGH, state);
	state = CConfig::Instance().GetOpaqueOnMouseOver() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_OPAQUE_ON_MOUSE_OVER, state);
	state = CConfig::Instance().GetTransparentOnMouseOver() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_TRANSPARENT_ON_MOUSE_OVER, state);

	if (!CConfig::Instance().GetNativeTransparency())
	{
		EnableWindow(GetDlgItem(window, IDC_CLICKTROUGH), FALSE);
		EnableWindow(GetDlgItem(window, IDC_OPAQUE_ON_MOUSE_OVER), FALSE);
		EnableWindow(GetDlgItem(window, IDC_TRANSPARENT_ON_MOUSE_OVER), FALSE);
	}

	widget = GetDlgItem(window, IDC_VERTICAL_COUNT);
	itoa(CConfig::Instance().GetVerticalCount(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_HORIZONTAL_COUNT);
	itoa(CConfig::Instance().GetHorizontalCount(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_PREVIOUS_MONTHS);
	if (CConfig::Instance().GetStartFromJanuary())
	{
		SetWindowText(widget, "0");
	}
	else
	{
		itoa(CConfig::Instance().GetPreviousMonths(), tmpSz, 10);
		SetWindowText(widget, tmpSz);
	}

	state = CConfig::Instance().GetStartFromJanuary() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_LAYOUT_JANUARY, state);

	widget = GetDlgItem(window, IDC_EVENTLIST_DAYS);
	itoa(CConfig::Instance().GetEventListDays(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	state = CConfig::Instance().GetSubstituteDays() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENTLIST_SUBSTITUTE, state);

	UpdateLayoutWidgets(window);

	return TRUE;
}

BOOL OnOKLayoutDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_WINDOW_ONBOTTOM))
	{
		CConfig::Instance().SetWindowPos(CConfig::WINDOWPOS_ONBOTTOM);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_WINDOW_ONTOP))
	{
		CConfig::Instance().SetWindowPos(CConfig::WINDOWPOS_ONTOP);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_WINDOW_ONDESKTOP))
	{
		CConfig::Instance().SetWindowPos(CConfig::WINDOWPOS_ONDESKTOP);
	}
	else
	{
		CConfig::Instance().SetWindowPos(CConfig::WINDOWPOS_NORMAL);
	}

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WINDOW_MOVABLE));
	CConfig::Instance().SetMovable(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WINDOW_MOUSEHIDE));
	CConfig::Instance().SetMouseHide(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_REMEMBER_DIALOG_POSITIONS));
	CConfig::Instance().SetRememberDialogPositions(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_CLICKTROUGH));
	CConfig::Instance().SetClickThrough(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_OPAQUE_ON_MOUSE_OVER));
	CConfig::Instance().SetOpaqueOnMouseOver(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_TRANSPARENT_ON_MOUSE_OVER));
	CConfig::Instance().SetTransparentOnMouseOver(state);

	widget = GetDlgItem(window, IDC_VERTICAL_COUNT);
	GetWindowText(widget, tmpSz, 256);
	int value = atoi(tmpSz);
	if (value < 1)
	{
		//	"You can use only values positive starting from 1 values in the vertical and horizontal count fields.",
		MessageBox(window, CCalendarWindow::c_Language.GetString("Message", 3), "Rainlendar", MB_OK | MB_ICONEXCLAMATION);
		value = 1;
	}
	else if (value > 30)
	{
		char buffer[256];
		//	"%i? Are you mad? Please try to use some more reasonable value in the vertical and horizontal count fields.",
		sprintf(buffer, CCalendarWindow::c_Language.GetString("Message", 4), value);
		MessageBox(window, buffer, "Rainlendar", MB_OK | MB_ICONEXCLAMATION);
		value = 1;
	}
	CConfig::Instance().SetVerticalCount(value);

	widget = GetDlgItem(window, IDC_HORIZONTAL_COUNT);
	GetWindowText(widget, tmpSz, 256);
	value = atoi(tmpSz);
	if (value < 1)
	{
		//	"You can use only values positive starting from 1 values in the vertical and horizontal count fields.",
		MessageBox(window, CCalendarWindow::c_Language.GetString("Message", 3), "Rainlendar", MB_OK | MB_ICONEXCLAMATION);
		value = 1;
	}
	else if (value > 30)
	{
		char buffer[256];
		//	"%i? Are you mad? Please try to use some more reasonable value in the vertical and horizontal count fields.",
		sprintf(buffer, CCalendarWindow::c_Language.GetString("Message", 4), value);
		MessageBox(window, buffer, "Rainlendar", MB_OK | MB_ICONEXCLAMATION);
		value = 1;
	}
	CConfig::Instance().SetHorizontalCount(value);

	widget = GetDlgItem(window, IDC_PREVIOUS_MONTHS);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetPreviousMonths(atoi(tmpSz));

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_LAYOUT_JANUARY));
	CConfig::Instance().SetStartFromJanuary(state);

	widget = GetDlgItem(window, IDC_EVENTLIST_DAYS);
	GetWindowText(widget, tmpSz, 256);
	value = atoi(tmpSz);
	CConfig::Instance().SetEventListDays(max(1, value));

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENTLIST_SUBSTITUTE));
	CConfig::Instance().SetSubstituteDays(state);

	return TRUE;
}

BOOL CALLBACK LayoutPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitLayoutDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				{
					return OnOKLayoutDialog(hwndDlg);
				}
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_LAYOUT_JANUARY:
			case IDC_TODO_ENABLE:
			case IDC_CLICKTROUGH:
			case IDC_WINDOW_MOUSEHIDE:
			case IDC_OPAQUE_ON_MOUSE_OVER:
			case IDC_TRANSPARENT_ON_MOUSE_OVER:
				UpdateLayoutWidgets(hwndDlg);
				return TRUE;
			}
			break;

		case PSM_QUERYSIBLINGS:
			if (wParam == CONFIG_MESSAGE_TRANSPARENCY_ON)
			{
				UpdateLayoutWidgets(hwndDlg);
			}
			else if (wParam == CONFIG_MESSAGE_TRANSPARENCY_OFF)
			{
				EnableWindow(GetDlgItem(hwndDlg, IDC_WINDOW_MOUSEHIDE), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_CLICKTROUGH), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_OPAQUE_ON_MOUSE_OVER), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_TRANSPARENT_ON_MOUSE_OVER), FALSE);

				CheckDlgButton(hwndDlg, IDC_CLICKTROUGH, BST_UNCHECKED);
				CheckDlgButton(hwndDlg, IDC_OPAQUE_ON_MOUSE_OVER, BST_UNCHECKED);
				CheckDlgButton(hwndDlg, IDC_TRANSPARENT_ON_MOUSE_OVER, BST_UNCHECKED);
			}
			break;
	}
    return FALSE; 
} 

//////////////////////////////////////////////////////////////////////////
//
// HOTKEYS
//
//////////////////////////////////////////////////////////////////////////
DWORD GetHotkey(int i)
{
	DWORD hotkey = 0;

	switch(i)
	{
	case CConfig::HOTKEY_HIDE:
		hotkey = CConfig::Instance().GetHideHotkey();
		break;

	case CConfig::HOTKEY_SHOW:
		hotkey = CConfig::Instance().GetShowHotkey();
		break;

	case CConfig::HOTKEY_TOGGLE:
		hotkey = CConfig::Instance().GetToggleHotkey();
		break;

	case CConfig::HOTKEY_ACTIVATE:
		hotkey =  CConfig::Instance().GetActivateHotkey();
		break;

	case CConfig::HOTKEY_REFRESH:
		hotkey = CConfig::Instance().GetRefreshHotkey();
		break;

	case CConfig::HOTKEY_CONFIG:
		hotkey = CConfig::Instance().GetConfigHotkey();
		break;

	case CConfig::HOTKEY_SKIN:
		hotkey = CConfig::Instance().GetSkinHotkey();
		break;

	case CConfig::HOTKEY_NEXT:
		hotkey = CConfig::Instance().GetNextHotkey();
		break;

	case CConfig::HOTKEY_PREVIOUS:
		hotkey = CConfig::Instance().GetPreviousHotkey();
		break;

	case CConfig::HOTKEY_NEXT_YEAR:
		hotkey = CConfig::Instance().GetNextYearHotkey();
		break;

	case CConfig::HOTKEY_PREVIOUS_YEAR:
		hotkey = CConfig::Instance().GetPreviousYearHotkey();
		break;

	case CConfig::HOTKEY_CURRENT:
		hotkey = CConfig::Instance().GetCurrentHotkey();
		break;

	case CConfig::HOTKEY_ALL:
		hotkey = CConfig::Instance().GetAllHotkey();
		break;

	case CConfig::HOTKEY_TODO:
		hotkey = CConfig::Instance().GetTodoHotkey();
		break;

	case CConfig::HOTKEY_HIDE_CALENDAR:
		hotkey = CConfig::Instance().GetHideCalendarHotkey();
		break;

	case CConfig::HOTKEY_SHOW_CALENDAR:
		hotkey = CConfig::Instance().GetShowCalendarHotkey();
		break;

	case CConfig::HOTKEY_TOGGLE_CALENDAR:
		hotkey = CConfig::Instance().GetToggleCalendarHotkey();
		break;

	case CConfig::HOTKEY_HIDE_TODO:
		hotkey = CConfig::Instance().GetHideTodoHotkey();
		break;

	case CConfig::HOTKEY_SHOW_TODO:
		hotkey = CConfig::Instance().GetShowTodoHotkey();
		break;

	case CConfig::HOTKEY_TOGGLE_TODO:
		hotkey = CConfig::Instance().GetToggleTodoHotkey();
		break;

	case CConfig::HOTKEY_HIDE_EVENTS:
		hotkey = CConfig::Instance().GetHideEventsHotkey();
		break;

	case CConfig::HOTKEY_SHOW_EVENTS:
		hotkey = CConfig::Instance().GetShowEventsHotkey();
		break;

	case CConfig::HOTKEY_TOGGLE_EVENTS:
		hotkey = CConfig::Instance().GetToggleEventsHotkey();
		break;

	case CConfig::HOTKEY_NEW_EVENT:
		hotkey = CConfig::Instance().GetAddEventHotkey();
		break;

	}

	return hotkey;
}

void UpdateHotkeyWidgets(HWND window)
{
	char buffer[MAX_LINE_LENGTH];
	HWND widget = GetDlgItem(window, IDC_HOTKEY_LIST);

	for (int i = 0; i < CConfig::HOTKEY_LAST; i++)
	{
		buffer[0] = 0;

		LVITEM item;
		item.iItem = i;
		item.iSubItem = 0;
		item.mask = LVIF_PARAM;
		ListView_GetItem(widget, &item);

		HWND hkWidget = GetDlgItem(window, IDC_HOTKEY);
		if (hkWidget) 
		{
			SendMessage(hkWidget, HKM_SETHOTKEY, item.lParam, 0);

			// Get the text from the hotkey control
			IAccessible* acc = NULL;
			HRESULT hr = AccessibleObjectFromWindow(hkWidget, OBJID_CLIENT, IID_IAccessible, (void**)&acc);
			if (SUCCEEDED(hr))
			{
				VARIANT var;
				VariantInit(&var);
				var.lVal = CHILDID_SELF;
				var.vt = VT_I4;
				BSTR value;
				hr = acc->get_accValue(var, &value);
				VariantClear(&var);
				acc->Release();

				_bstr_t name(value, false);
				const char* szName = name;
				ListView_SetItemText(widget, i, 1, (char*)szName);
			}
		}
	}

	int index = ListView_GetNextItem(widget, -1, LVNI_SELECTED | LVNI_FOCUSED);
	if (index != -1)
	{
		EnableWindow(GetDlgItem(window, IDC_SET_HOTKEY_BUTTON), TRUE);

		HWND hkWidget = GetDlgItem(window, IDC_HOTKEY);
		EnableWindow(hkWidget, TRUE);

		LVITEM item;
		item.iItem = index;
		item.iSubItem = 0;
		item.mask = LVIF_PARAM;
		ListView_GetItem(widget, &item);

		SendMessage(hkWidget, HKM_SETHOTKEY, item.lParam, 0);
	}
	else
	{
		EnableWindow(GetDlgItem(window, IDC_SET_HOTKEY_BUTTON), FALSE);
		EnableWindow(GetDlgItem(window, IDC_HOTKEY), FALSE);
	}
}

BOOL OnInitHotkeysDialog(HWND window) 
{
	HWND widget;
	DWORD hotkey;
	DWORD modifiers;

	widget = GetDlgItem(window, IDC_SET_HOTKEY_BUTTON);
	SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 17));

	widget = GetDlgItem(window, IDC_HOTKEY_FRAME);
	SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 18));

	widget = GetDlgItem(window, IDC_HOTKEY_LIST);
	if(widget) 
	{
		// Add Columns
		LVCOLUMN lvc; 
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
		lvc.iSubItem = 0;
		lvc.pszText = (char*)CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 15);
		lvc.cx = 200;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 0, &lvc);

		lvc.iSubItem = 1;
		lvc.pszText = (char*)CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 16);
		lvc.cx = 100;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 1, &lvc);

		ListView_SetExtendedListViewStyle(widget, LVS_EX_FULLROWSELECT);

		// This must match with CConfig::HOTKEY
		int langIDs[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28};

		assert(sizeof(langIDs) / sizeof(int) == CConfig::HOTKEY_LAST);

		// Add Items
		LVITEM lvI; 
		
		lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
		lvI.state = 0; 
		lvI.stateMask = 0; 
		lvI.iSubItem = 0;

		for (int i = 0; i < CConfig::HOTKEY_LAST; i++)
		{
			lvI.iItem = i;

			hotkey = GetHotkey(i);

			modifiers = hotkey >> 8;
			modifiers = ((modifiers & MOD_ALT) ? HOTKEYF_ALT : 0) |
						((modifiers & MOD_CONTROL) ? HOTKEYF_CONTROL : 0) |
						((modifiers & MOD_SHIFT) ? HOTKEYF_SHIFT : 0);
			hotkey &= 0xFF;
			hotkey |= (modifiers << 8);

			lvI.lParam = hotkey;
			lvI.pszText = (char*)CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", langIDs[i]);
			ListView_InsertItem(widget, &lvI);
		}
	}

	UpdateHotkeyWidgets(window);

	return TRUE;
}

BOOL OnOKHotkeysDialog(HWND window) 
{
	HWND widget;
	DWORD hotkey;
	DWORD modifiers;

	widget = GetDlgItem(window, IDC_HOTKEY_LIST);

	// Get hotkeys
	for (int i = 0; i < CConfig::HOTKEY_LAST; i++)
	{
		LVITEM item;
		item.iItem = i;
		item.iSubItem = 0;
		item.mask = LVIF_PARAM;
		ListView_GetItem(widget, &item);
		
		hotkey = item.lParam;
		modifiers = hotkey >> 8;
		modifiers = ((modifiers & HOTKEYF_ALT) ? MOD_ALT : 0) |
					((modifiers & HOTKEYF_CONTROL) ? MOD_CONTROL : 0) |
					((modifiers & HOTKEYF_SHIFT) ? MOD_SHIFT : 0);
		hotkey &= 0xFF;
		hotkey |= (modifiers << 8);

		switch(i)
		{
		case CConfig::HOTKEY_HIDE:
			if (widget) CConfig::Instance().SetHideHotkey(hotkey);
			break;

		case CConfig::HOTKEY_SHOW:
			if (widget) CConfig::Instance().SetShowHotkey(hotkey);
			break;

		case CConfig::HOTKEY_TOGGLE:
			if (widget) CConfig::Instance().SetToggleHotkey(hotkey);
			break;

		case CConfig::HOTKEY_ACTIVATE:
			if (widget) CConfig::Instance().SetActivateHotkey(hotkey);
			break;

		case CConfig::HOTKEY_REFRESH:
			if (widget) CConfig::Instance().SetRefreshHotkey(hotkey);
			break;

		case CConfig::HOTKEY_CONFIG:
			if (widget) CConfig::Instance().SetConfigHotkey(hotkey);
			break;

		case CConfig::HOTKEY_SKIN:
			if (widget) CConfig::Instance().SetSkinHotkey(hotkey);
			break;

		case CConfig::HOTKEY_NEXT:
			if (widget) CConfig::Instance().SetNextHotkey(hotkey);
			break;

		case CConfig::HOTKEY_PREVIOUS:
			if (widget) CConfig::Instance().SetPreviousHotkey(hotkey);
			break;

		case CConfig::HOTKEY_NEXT_YEAR:
			if (widget) CConfig::Instance().SetNextYearHotkey(hotkey);
			break;

		case CConfig::HOTKEY_PREVIOUS_YEAR:
			if (widget) CConfig::Instance().SetPreviousYearHotkey(hotkey);
			break;

		case CConfig::HOTKEY_CURRENT:
			if (widget) CConfig::Instance().SetCurrentHotkey(hotkey);
			break;

		case CConfig::HOTKEY_ALL:
			if (widget) CConfig::Instance().SetAllHotkey(hotkey);
			break;

		case CConfig::HOTKEY_TODO:
			if (widget) CConfig::Instance().SetTodoHotkey(hotkey);
			break;

		case CConfig::HOTKEY_HIDE_CALENDAR:
			if (widget) CConfig::Instance().SetHideCalendarHotkey(hotkey);
			break;

		case CConfig::HOTKEY_SHOW_CALENDAR:
			if (widget) CConfig::Instance().SetShowCalendarHotkey(hotkey);
			break;

		case CConfig::HOTKEY_TOGGLE_CALENDAR:
			if (widget) CConfig::Instance().SetToggleCalendarHotkey(hotkey);
			break;

		case CConfig::HOTKEY_HIDE_TODO:
			if (widget) CConfig::Instance().SetHideTodoHotkey(hotkey);
			break;

		case CConfig::HOTKEY_SHOW_TODO:
			if (widget) CConfig::Instance().SetShowTodoHotkey(hotkey);
			break;

		case CConfig::HOTKEY_TOGGLE_TODO:
			if (widget) CConfig::Instance().SetToggleTodoHotkey(hotkey);
			break;

		case CConfig::HOTKEY_HIDE_EVENTS:
			if (widget) CConfig::Instance().SetHideEventsHotkey(hotkey);
			break;

		case CConfig::HOTKEY_SHOW_EVENTS:
			if (widget) CConfig::Instance().SetShowEventsHotkey(hotkey);
			break;

		case CConfig::HOTKEY_TOGGLE_EVENTS:
			if (widget) CConfig::Instance().SetToggleEventsHotkey(hotkey);
			break;

		case CConfig::HOTKEY_NEW_EVENT:
			if (widget) CConfig::Instance().SetAddEventHotkey(hotkey);
			break;
		}
	}

	return TRUE;
}

BOOL CALLBACK HotkeysPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitHotkeysDialog(hwndDlg);

		case WM_COMMAND:
			if (LOWORD(wParam) == IDC_SET_HOTKEY_BUTTON)
			{
				HWND widget = GetDlgItem(hwndDlg, IDC_HOTKEY_LIST);

				int index = ListView_GetNextItem(widget, -1, LVNI_SELECTED | LVNI_FOCUSED);
				if (index != -1)
				{
					HWND hkWidget = GetDlgItem(hwndDlg, IDC_HOTKEY);
					DWORD hotkey = SendMessage(hkWidget, HKM_GETHOTKEY, 0, 0);

					LVITEM item;
					item.iItem = index;
					item.iSubItem = 0;
					item.mask = LVIF_PARAM;
					item.lParam = hotkey;
					ListView_SetItem(widget, &item);
				}
				else
				{
					DebugLog("Nothing selected. Unable to set the hotkey.");
				}
				return TRUE;
			}
			break;

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case LVN_ITEMCHANGED:
				UpdateHotkeyWidgets(hwndDlg);
				break;

			case PSN_APPLY:
				return OnOKHotkeysDialog(hwndDlg);
			}
			break;
	}
    return FALSE; 
} 

//////////////////////////////////////////////////////////////////////////
//
// PLUGINS
//
//////////////////////////////////////////////////////////////////////////
void UpdatePluginsWidgets(HWND window)
{
	HWND widget = GetDlgItem(window, IDC_PLUGIN_LIST);

	const std::vector<CPlugin*>& allPlugins = GetRainlendar()->GetCalendarWindow().GetPluginManager().GetAllPlugins();

	for (UINT i = 0; i < allPlugins.size(); i++)
	{
		if (_tcsicmp(allPlugins[i]->GetName(), GetRainlendar()->GetCalendarWindow().GetPluginManager().GetDefaultPluginName().c_str()) == 0)
		{
			ListView_SetItemText(widget, i, 2, (char*)CCalendarWindow::c_Language.GetString("Plugins", 3));
		}
		else
		{
			ListView_SetItemText(widget, i, 2, "");
		}
	}

	int index = ListView_GetNextItem(widget, -1, LVNI_SELECTED | LVNI_FOCUSED);
	if (index != -1)
	{
		widget = GetDlgItem(window, IDC_PLUGIN_MAKE_DEFAULT);
		EnableWindow(widget, TRUE);

		widget = GetDlgItem(window, IDC_PLUGIN_SETTINGS);
		
		if (index < (int)allPlugins.size())
		{
			if (allPlugins[index]->HasConfig())
			{
				EnableWindow(widget, TRUE);
			}
			else
			{
				EnableWindow(widget, FALSE);
			}
		}
		else
		{
			EnableWindow(widget, FALSE);
		}
	}
	else
	{
		widget = GetDlgItem(window, IDC_PLUGIN_MAKE_DEFAULT);
		EnableWindow(widget, FALSE);

		widget = GetDlgItem(window, IDC_PLUGIN_SETTINGS);
		EnableWindow(widget, FALSE);
	}
}


BOOL OnInitPluginsDialog(HWND window) 
{
	HWND widget;

	// Set localized strings
	widget = GetDlgItem(window, IDC_PLUGIN_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("Plugins", 0));

	widget = GetDlgItem(window, IDC_PLUGIN_MAKE_DEFAULT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("Plugins", 4));

	widget = GetDlgItem(window, IDC_PLUGIN_SETTINGS);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("Plugins", 5));

	widget = GetDlgItem(window, IDC_PLUGINS_INFO);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("Plugins", 6));

	widget = GetDlgItem(window, IDC_PLUGIN_LIST);
	if(widget) 
	{
		// Add Columns
		LVCOLUMN lvc; 
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
		lvc.iSubItem = 0;
		lvc.pszText = (char*)CCalendarWindow::c_Language.GetString("Plugins", 1);
		lvc.cx = 240;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 0, &lvc);

		lvc.iSubItem = 1;
		lvc.pszText = (char*)CCalendarWindow::c_Language.GetString("Plugins", 2);
		lvc.cx = 60;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 1, &lvc);
		
		lvc.iSubItem = 2;
		lvc.pszText = (char*)CCalendarWindow::c_Language.GetString("Plugins", 3);
		lvc.cx = 60;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 2, &lvc);

		ListView_SetExtendedListViewStyle(widget, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);


		// Add Items
		LVITEM lvI; 
		
		lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
		lvI.state = 0; 
		lvI.stateMask = 0; 
		
		const std::vector<CPlugin*>& allPlugins = GetRainlendar()->GetCalendarWindow().GetPluginManager().GetAllPlugins();

		std::vector<CPlugin*>::const_iterator iter = allPlugins.begin();
		int i = 0;
		for (; iter != allPlugins.end(); iter++)
		{
			lvI.iItem = i;
			lvI.iSubItem = 0;
			lvI.pszText = (char*)(*iter)->GetName();
			lvI.lParam = (LPARAM)i;
			ListView_InsertItem(widget, &lvI);
			
			TCHAR buffer[256];
			int ver = (*iter)->GetVersion();
			_stprintf(buffer, "%i.%i", ver / 1000, ver % 1000);
			ListView_SetItemText(widget, i, 1, buffer);

			if (_tcsicmp((*iter)->GetName(), GetRainlendar()->GetCalendarWindow().GetPluginManager().GetDefaultPluginName().c_str()) == 0)
			{
				ListView_SetItemText(widget, i, 2, (char*)CCalendarWindow::c_Language.GetString("Plugins", 3));
			}

			if ((*iter)->IsEnabled())
			{
				ListView_SetCheckState(widget, i, TRUE);
			}

			i++;
		}
	}

	return TRUE;
}

BOOL OnOKPluginsDialog(HWND window) 
{
	HWND widget = GetDlgItem(window, IDC_PLUGIN_LIST);
	if(widget) 
	{
		const std::vector<CPlugin*>& allPlugins = GetRainlendar()->GetCalendarWindow().GetPluginManager().GetAllPlugins();
		for (UINT i = 0; i < allPlugins.size(); i++)
		{
			if (ListView_GetCheckState(widget, i))
			{
				allPlugins[i]->Enable();
			}
			else
			{
				allPlugins[i]->Disable();
			}
		}
	}

	GetRainlendar()->GetCalendarWindow().GetPluginManager().WriteConfigs();

	return TRUE;
}

BOOL CALLBACK PluginsPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitPluginsDialog(hwndDlg);

        case WM_COMMAND:
            switch (LOWORD(wParam)) 
            {
                case IDC_PLUGIN_MAKE_DEFAULT:
					{
						HWND widget; 
						widget = GetDlgItem(hwndDlg, IDC_PLUGIN_LIST);
						int index = ListView_GetNextItem(widget, -1, LVNI_SELECTED | LVNI_FOCUSED);
						if (index != -1)
						{
							CPluginManager& manager = GetRainlendar()->GetCalendarWindow().GetPluginManager();
							const std::vector<CPlugin*>& allPlugins = manager.GetAllPlugins();
							if ((int)allPlugins.size() > index)
							{
								manager.SetDefaultPluginName(allPlugins[index]->GetName());
								UpdatePluginsWidgets(hwndDlg);
							}
						}
					}
                    return FALSE; 

                case IDC_PLUGIN_SETTINGS:
					{
						HWND widget; 
						widget = GetDlgItem(hwndDlg, IDC_PLUGIN_LIST);
						int index = ListView_GetNextItem(widget, -1, LVNI_SELECTED | LVNI_FOCUSED);
						if (index != -1)
						{
							CPluginManager& manager = GetRainlendar()->GetCalendarWindow().GetPluginManager();
							const std::vector<CPlugin*>& allPlugins = manager.GetAllPlugins();
							if ((int)allPlugins.size() > index)
							{
								std::string path = CConfig::Instance().GetPath();
								path += "Rainlendar.ini";
								allPlugins[index]->ShowConfig(hwndDlg, path.c_str());
							}
						}
					}
                    return FALSE; 
            }
            break;

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case NM_DBLCLK:
				PostMessage(hwndDlg, WM_COMMAND, IDC_PLUGIN_SETTINGS, NULL);
				break;

			case LVN_ITEMCHANGED:
				UpdatePluginsWidgets(hwndDlg);
				break;

			case PSN_APPLY:
				return OnOKPluginsDialog(hwndDlg);
			}
			break;
	}
    return FALSE; 
} 

