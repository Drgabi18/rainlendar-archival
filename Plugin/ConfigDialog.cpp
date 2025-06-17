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
  $Header: /home/cvsroot/Rainlendar/Plugin/ConfigDialog.cpp,v 1.17 2004/01/28 18:05:21 rainy Exp $

  $Log: ConfigDialog.cpp,v $
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

#include "ConfigDialog.h"
#include "RainlendarDLL.h"
#include "NetworkThread.h"
#include <Prsht.h>
#include <list>

// Couple of globals
std::list<std::string> g_StatusStrings;
HANDLE  g_Mutex = NULL;
HWND g_StatusWidget = NULL;

BOOL CALLBACK LayoutPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam); 
BOOL CALLBACK GeneralPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam); 
BOOL CALLBACK HotkeysPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ServerPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

enum CONFIG_MESSAGE
{
	CONFIG_MESSAGE_ONDESKTOP_ON = 1,
	CONFIG_MESSAGE_ONDESKTOP_OFF,
	CONFIG_MESSAGE_TRANSPARENCY_ON,
	CONFIG_MESSAGE_TRANSPARENCY_OFF
};

VOID OpenConfigDialog(HWND hwndOwner, HINSTANCE instance)
{
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
    psp[3].pszTemplate = MAKEINTRESOURCE(IDD_SERVER);
    psp[3].pszIcon = NULL;
    psp[3].pfnDlgProc = ServerPageProc;
    psp[3].pszTitle = CCalendarWindow::c_Language.GetString("General", 5);
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
		// Refresh
		SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);

		if (CConfig::Instance().GetTodoEnable()) 
		{
			GetRainlendar()->GetCalendarWindow().GetTodoWindow().ShowWindow(false);
		}
		else
		{
			GetRainlendar()->GetCalendarWindow().GetTodoWindow().HideWindow();
		}
	}

    return;
}

void UpdateGeneralWidgets(HWND window)
{
	if (IsDlgButtonChecked(window, IDC_SHOW_OUTLOOK_APPOINTMENTS) == BST_CHECKED)
	{
		EnableWindow(GetDlgItem(window, IDC_OUTLOOK_UPDATE), TRUE);
		EnableWindow(GetDlgItem(window, IDC_OUTLOOK_UPDATE_STATIC), TRUE);
		EnableWindow(GetDlgItem(window, IDC_OUTLOOK_MINUTES_STATIC), TRUE);
		EnableWindow(GetDlgItem(window, IDC_GET_OUTLOOK_AT_STARTUP), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(window, IDC_OUTLOOK_UPDATE), FALSE);
		EnableWindow(GetDlgItem(window, IDC_OUTLOOK_UPDATE_STATIC), FALSE);
		EnableWindow(GetDlgItem(window, IDC_OUTLOOK_MINUTES_STATIC), FALSE);
		EnableWindow(GetDlgItem(window, IDC_GET_OUTLOOK_AT_STARTUP), FALSE);
	}

	if (IsDlgButtonChecked(window, IDC_EVENT_TOOLTIP) == BST_CHECKED)
	{
		EnableWindow(GetDlgItem(window, IDC_EVENT_TOOLTIP_WIDTH), TRUE);
		EnableWindow(GetDlgItem(window, IDC_TOOLTIP_WIDTH_STATIC), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(window, IDC_EVENT_TOOLTIP_WIDTH), FALSE);
		EnableWindow(GetDlgItem(window, IDC_TOOLTIP_WIDTH_STATIC), FALSE);
	}
}

BOOL OnInitGeneralDialog(HWND window) 
{
	char tmpSz[256];
	UINT state;
	HWND widget;

	state = CConfig::Instance().GetEventToolTips() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENT_TOOLTIP, state);
	widget = GetDlgItem(window, IDC_EVENT_TOOLTIP_WIDTH);
	itoa(CConfig::Instance().GetToolTipMaxWidth(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	state = CConfig::Instance().GetShowAllEvents() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENT_SHOW_ALL, state);
	state = CConfig::Instance().GetShowSingleEvent() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENT_SHOW_SINGLE, state);
	widget = GetDlgItem(window, IDC_EVENT_MESSAGE_WIDTH);
	itoa(CConfig::Instance().GetMessageBoxMaxWidth(), tmpSz, 10);
	SetWindowText(widget, tmpSz);
	widget = GetDlgItem(window, IDC_EVENT_EXECUTE);
	SetWindowText(widget, CConfig::Instance().GetEventExecute().c_str());

	state = CConfig::Instance().GetStartHidden() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_START_HIDDEN, state);
	state = CConfig::Instance().GetStartFromMonday() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_START_FROM_MONDAY, state);
	state = CConfig::Instance().GetDisableHotkeys() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_DISABLE_HOTKEYS, state);
	state = CConfig::Instance().GetUseWindowName() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_USE_WINDOW_NAME, state);
	state = CConfig::Instance().GetPollWallpaper() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_POLL_WALLPAPER, state);
	state = CConfig::Instance().GetSnapEdges() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_SNAP_EDGES, state);
	state = CConfig::Instance().GetNativeTransparency() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_NATIVE_TRANSPARENCY, state);
	state = CConfig::Instance().GetRefreshOnResolutionChange() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_REFRESH_ON_RESOLUTION_CHANGE, state);
	state = CConfig::Instance().GetWeek1HasJanuary1st() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_WEEK_1_JANUARY_1, state);
	state = CConfig::Instance().GetTooltipSeparator() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_TOOLTIP_SEPARATOR, state);
	state = CConfig::Instance().GetShowTrayIcon() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_SHOW_TRAYICON, state);

	state = CConfig::Instance().GetShowOutlookAppointments() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_SHOW_OUTLOOK_APPOINTMENTS, state);
	state = CConfig::Instance().GetOutlookAppointmentsAtStartup() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_GET_OUTLOOK_AT_STARTUP, state);
	widget = GetDlgItem(window, IDC_OUTLOOK_UPDATE);
	itoa(CConfig::Instance().GetOutlookUpdate(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	// Set localized strings
	widget = GetDlgItem(GetParent(window), IDOK);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("General", 0));
	widget = GetDlgItem(GetParent(window), IDCANCEL);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("General", 1));
	widget = GetDlgItem(window, IDC_EVENT_TOOLTIP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 0));
	widget = GetDlgItem(window, IDC_START_HIDDEN);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 2));
	widget = GetDlgItem(window, IDC_START_FROM_MONDAY);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 3));
	widget = GetDlgItem(window, IDC_DISABLE_HOTKEYS);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 4));
	widget = GetDlgItem(window, IDC_USE_WINDOW_NAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 5));
	widget = GetDlgItem(window, IDC_POLL_WALLPAPER);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 6));
	widget = GetDlgItem(window, IDC_EVENT_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 7));
	widget = GetDlgItem(window, IDC_EXECUTE_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 8));
	widget = GetDlgItem(window, IDC_OTHER_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 9));
	widget = GetDlgItem(window, IDC_SNAP_EDGES);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 10));
	widget = GetDlgItem(window, IDC_NATIVE_TRANSPARENCY);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 11));
	widget = GetDlgItem(window, IDC_REFRESH_ON_RESOLUTION_CHANGE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 12));
	widget = GetDlgItem(window, IDC_WEEK_1_JANUARY_1);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 14));
	widget = GetDlgItem(window, IDC_TOOLTIP_SEPARATOR);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 15));
	widget = GetDlgItem(window, IDC_EVENT_SHOW_ALL);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 22));
	widget = GetDlgItem(window, IDC_EVENT_SHOW_SINGLE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 23));
	widget = GetDlgItem(window, IDC_MESSAGE_WIDTH_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 24));

	widget = GetDlgItem(window, IDC_SHOW_OUTLOOK_APPOINTMENTS);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 13));
	widget = GetDlgItem(window, IDC_OUTLOOK_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 16));
	widget = GetDlgItem(window, IDC_OUTLOOK_UPDATE_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 17));
	widget = GetDlgItem(window, IDC_OUTLOOK_MINUTES_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 18));

	widget = GetDlgItem(window, IDC_GET_OUTLOOK_AT_STARTUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 20));
	widget = GetDlgItem(window, IDC_TOOLTIP_WIDTH_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 21));

	widget = GetDlgItem(window, IDC_SHOW_TRAYICON);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 25));

	if (!CCalendarWindow::Is2k() || CConfig::Instance().GetWindowPos() == CConfig::WINDOWPOS_ONDESKTOP)
	{
		// Disable the button if we're running wintendo or the OnDesktop is set
		EnableWindow(GetDlgItem(window, IDC_NATIVE_TRANSPARENCY), FALSE);
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
	char tmpSz[256];

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_TOOLTIP));
	CConfig::Instance().SetEventToolTips(state);
	widget = GetDlgItem(window, IDC_EVENT_TOOLTIP_WIDTH);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetToolTipMaxWidth(atoi(tmpSz));

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_SHOW_ALL));
	CConfig::Instance().SetShowAllEvents(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_SHOW_SINGLE));
	CConfig::Instance().SetShowSingleEvent(state);
	widget = GetDlgItem(window, IDC_EVENT_MESSAGE_WIDTH);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetMessageBoxMaxWidth(atoi(tmpSz));
	widget = GetDlgItem(window, IDC_EVENT_EXECUTE);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetEventExecute(tmpSz);

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_START_HIDDEN));
	CConfig::Instance().SetStartHidden(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_START_FROM_MONDAY));
	CConfig::Instance().SetStartFromMonday(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_DISABLE_HOTKEYS));
	CConfig::Instance().SetDisableHotkeys(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_USE_WINDOW_NAME));
	CConfig::Instance().SetUseWindowName(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_POLL_WALLPAPER));
	CConfig::Instance().SetPollWallpaper(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_SNAP_EDGES));
	CConfig::Instance().SetSnapEdges(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_NATIVE_TRANSPARENCY));
	CConfig::Instance().SetNativeTransparency(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_REFRESH_ON_RESOLUTION_CHANGE));
	CConfig::Instance().SetRefreshOnResolutionChange(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WEEK_1_JANUARY_1));
	CConfig::Instance().SetWeek1HasJanuary1st(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_TOOLTIP_SEPARATOR));
	CConfig::Instance().SetTooltipSeparator(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_SHOW_TRAYICON));
	CConfig::Instance().SetShowTrayIcon(state);

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_SHOW_OUTLOOK_APPOINTMENTS));
	CConfig::Instance().SetShowOutlookAppointments(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_GET_OUTLOOK_AT_STARTUP));
	CConfig::Instance().SetOutlookAppointmentsAtStartup(state);
	widget = GetDlgItem(window, IDC_OUTLOOK_UPDATE);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetOutlookUpdate(atoi(tmpSz));

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
			case IDC_EVENT_TOOLTIP:
			case IDC_SHOW_OUTLOOK_APPOINTMENTS:
				UpdateGeneralWidgets(hwndDlg);
				return TRUE;

			case IDC_NATIVE_TRANSPARENCY:
				// Notify the other sheets that native transparency was selected
				if (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_NATIVE_TRANSPARENCY))
				{
					PropSheet_QuerySiblings(GetParent(hwndDlg), CONFIG_MESSAGE_TRANSPARENCY_ON, 0);
				}
				else
				{
					PropSheet_QuerySiblings(GetParent(hwndDlg), CONFIG_MESSAGE_TRANSPARENCY_OFF, 0);
				}
				return TRUE;
			}
			break;

		case PSM_QUERYSIBLINGS:
			if (wParam == CONFIG_MESSAGE_ONDESKTOP_ON)
			{
				EnableWindow(GetDlgItem(hwndDlg, IDC_NATIVE_TRANSPARENCY), FALSE);
			}
			else if (wParam == CONFIG_MESSAGE_ONDESKTOP_OFF)
			{
				EnableWindow(GetDlgItem(hwndDlg, IDC_NATIVE_TRANSPARENCY), TRUE);
			}
			break;

	}
    return FALSE; 
} 

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

	if (IsDlgButtonChecked(window, IDC_TODO_ENABLE) == BST_CHECKED)
	{
		EnableWindow(GetDlgItem(window, IDC_TODO_X), TRUE);
		EnableWindow(GetDlgItem(window, IDC_TODO_Y), TRUE);
		EnableWindow(GetDlgItem(window, IDC_TODO_X_STATIC), TRUE);
		EnableWindow(GetDlgItem(window, IDC_TODO_Y_STATIC), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(window, IDC_TODO_X), FALSE);
		EnableWindow(GetDlgItem(window, IDC_TODO_Y), FALSE);
		EnableWindow(GetDlgItem(window, IDC_TODO_X_STATIC), FALSE);
		EnableWindow(GetDlgItem(window, IDC_TODO_Y_STATIC), FALSE);
	}
}

BOOL OnInitLayoutDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	UINT check, state;

	// Set localized strings
	widget = GetDlgItem(window, IDC_X_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 5));
	widget = GetDlgItem(window, IDC_Y_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 6));
	widget = GetDlgItem(window, IDC_WINDOW_ONBOTTOM);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 0));
	widget = GetDlgItem(window, IDC_WINDOW_ONTOP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 2));
	widget = GetDlgItem(window, IDC_WINDOW_ONDESKTOP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 8));
	widget = GetDlgItem(window, IDC_WINDOW_NORMAL);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 1));
	widget = GetDlgItem(window, IDC_WINDOW_MOVABLE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 3));
	widget = GetDlgItem(window, IDC_WINDOW_MOUSEHIDE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 4));
	widget = GetDlgItem(window, IDC_VERTICAL_COUNT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 10));
	widget = GetDlgItem(window, IDC_HORIZONTAL_COUNT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 11));
	widget = GetDlgItem(window, IDC_PREVIOUS_MONTHS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 12));
	widget = GetDlgItem(window, IDC_POSITION_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 7));
	widget = GetDlgItem(window, IDC_LAYOUT_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 9));
	widget = GetDlgItem(window, IDC_LAYOUT_JANUARY);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 13));
	widget = GetDlgItem(window, IDC_REMEMBER_DIALOG_POSITIONS);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 14));
	widget = GetDlgItem(window, IDC_NEGATIVE_COORDS);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 15));
	widget = GetDlgItem(window, IDC_SETTINGS_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 9));
	widget = GetDlgItem(window, IDC_TODO_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 17));
	widget = GetDlgItem(window, IDC_TODO_ENABLE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 18));
	widget = GetDlgItem(window, IDC_TODO_X_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 5));
	widget = GetDlgItem(window, IDC_TODO_Y_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 6));

	widget = GetDlgItem(window, IDC_X);
	itoa(CConfig::Instance().GetX(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_Y);
	itoa(CConfig::Instance().GetY(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

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

	// Disabled if native is on
	EnableWindow(GetDlgItem(window, IDC_WINDOW_ONDESKTOP), !CConfig::Instance().GetNativeTransparency());

	state = CConfig::Instance().GetTodoEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_TODO_ENABLE, state);

	widget = GetDlgItem(window, IDC_TODO_X);
	itoa(CConfig::Instance().GetTodoX(), tmpSz, 10);
	SetWindowText(widget, tmpSz);
	
	widget = GetDlgItem(window, IDC_TODO_Y);
	itoa(CConfig::Instance().GetTodoY(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	state = CConfig::Instance().GetMovable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_WINDOW_MOVABLE, state);
	state = CConfig::Instance().GetMouseHide() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_WINDOW_MOUSEHIDE, state);
	state = CConfig::Instance().GetRememberDialogPositions() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_REMEMBER_DIALOG_POSITIONS, state);
	state = CConfig::Instance().GetNegativeCoords() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_NEGATIVE_COORDS, state);

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

	UpdateLayoutWidgets(window);

	return TRUE;
}

BOOL OnOKLayoutDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	widget = GetDlgItem(window, IDC_X);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetX(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_Y);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetY(atoi(tmpSz));

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

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_TODO_ENABLE));
	CConfig::Instance().SetTodoEnable(state);
	widget = GetDlgItem(window, IDC_TODO_X);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetTodoX(atoi(tmpSz));
	
	widget = GetDlgItem(window, IDC_TODO_Y);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetTodoY(atoi(tmpSz));

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WINDOW_MOVABLE));
	CConfig::Instance().SetMovable(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WINDOW_MOUSEHIDE));
	CConfig::Instance().SetMouseHide(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_REMEMBER_DIALOG_POSITIONS));
	CConfig::Instance().SetRememberDialogPositions(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_NEGATIVE_COORDS));
	CConfig::Instance().SetNegativeCoords(state);

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
				UpdateLayoutWidgets(hwndDlg);
				return TRUE;

			case IDC_WINDOW_ONDESKTOP:
				PropSheet_QuerySiblings(GetParent(hwndDlg), CONFIG_MESSAGE_ONDESKTOP_ON, 0);
				break;

			case IDC_WINDOW_ONBOTTOM:
			case IDC_WINDOW_ONTOP:
			case IDC_WINDOW_NORMAL:
				PropSheet_QuerySiblings(GetParent(hwndDlg), CONFIG_MESSAGE_ONDESKTOP_OFF, 0);
				return TRUE;
			}
			break;

		case PSM_QUERYSIBLINGS:
			if (wParam == CONFIG_MESSAGE_TRANSPARENCY_ON)
			{
				EnableWindow(GetDlgItem(hwndDlg, IDC_WINDOW_ONDESKTOP), FALSE);
			}
			else if (wParam == CONFIG_MESSAGE_TRANSPARENCY_OFF)
			{
				EnableWindow(GetDlgItem(hwndDlg, IDC_WINDOW_ONDESKTOP), TRUE);
			}
			break;
	}
    return FALSE; 
} 

BOOL OnInitHotkeysDialog(HWND window) 
{
	HWND widget;
	DWORD hotkey;
	DWORD modifiers;

	// Set localized strings
	widget = GetDlgItem(window, IDC_HOTKEYS_HIDE_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 0));
	widget = GetDlgItem(window, IDC_HOTKEYS_SHOW_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 1));
	widget = GetDlgItem(window, IDC_HOTKEYS_TOGGLE_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 2));
	widget = GetDlgItem(window, IDC_HOTKEYS_ACTIVATE_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 3));
	widget = GetDlgItem(window, IDC_HOTKEYS_REFRESH_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 4));
	widget = GetDlgItem(window, IDC_HOTKEYS_CONFIG_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 5));
	widget = GetDlgItem(window, IDC_HOTKEYS_SKIN_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 6));
	widget = GetDlgItem(window, IDC_HOTKEYS_NEXT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 7));
	widget = GetDlgItem(window, IDC_HOTKEYS_PREVIOUS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 8));
	widget = GetDlgItem(window, IDC_HOTKEYS_NEXT_YEAR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 11));
	widget = GetDlgItem(window, IDC_HOTKEYS_PREVIOUS_YEAR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 12));
	widget = GetDlgItem(window, IDC_HOTKEYS_CURRENT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 9));
	widget = GetDlgItem(window, IDC_HOTKEYS_ALL_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 10));
	widget = GetDlgItem(window, IDC_HOTKEYS_OUTLOOK_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 13));
	widget = GetDlgItem(window, IDC_HOTKEYS_TODO_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("HotkeysConfigGUI", 14));


	// Set hotkeys
	for (int i = 0; i < CConfig::HOTKEY_LAST; i++)
	{
		widget = GetDlgItem(window, IDC_HOTKEYS_HIDE + i);

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

		case CConfig::HOTKEY_OUTLOOK:
            hotkey = CConfig::Instance().GetOutlookHotkey();
			break;

		case CConfig::HOTKEY_TODO:
            hotkey = CConfig::Instance().GetTodoHotkey();
			break;
		}

		modifiers = hotkey >> 8;
		modifiers = ((modifiers & MOD_ALT) ? HOTKEYF_ALT : 0) |
					((modifiers & MOD_CONTROL) ? HOTKEYF_CONTROL : 0) |
					((modifiers & MOD_SHIFT) ? HOTKEYF_SHIFT : 0);
		hotkey &= 0xFF;
		hotkey |= (modifiers << 8);

		if (widget) SendMessage(widget, HKM_SETHOTKEY, hotkey, 0);
	}


	return TRUE;
}

BOOL OnOKHotkeysDialog(HWND window) 
{
	HWND widget;
	DWORD hotkey;
	DWORD modifiers;

	// Get hotkeys
	for (int i = 0; i < CConfig::HOTKEY_LAST; i++)
	{
		widget = GetDlgItem(window, IDC_HOTKEYS_HIDE + i);

		hotkey = SendMessage(widget, HKM_GETHOTKEY, 0, 0);
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

		case CConfig::HOTKEY_OUTLOOK:
			if (widget) CConfig::Instance().SetOutlookHotkey(hotkey);
			break;

		case CConfig::HOTKEY_TODO:
			if (widget) CConfig::Instance().SetTodoHotkey(hotkey);
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

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				{
					return OnOKHotkeysDialog(hwndDlg);
				}
			}
			break;
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
		EnableWindow(GetDlgItem(window, IDC_SERVER_COPY), TRUE);
		EnableWindow(GetDlgItem(window, IDC_SERVER_SYNC_ON_EDIT), TRUE);
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
		EnableWindow(GetDlgItem(window, IDC_SERVER_COPY), FALSE);
		EnableWindow(GetDlgItem(window, IDC_SERVER_SYNC_ON_EDIT), FALSE);
	}
}

BOOL OnInitServerDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	UINT state;

	widget = GetDlgItem(window, IDC_SERVER_ENABLE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("ServerConfigGUI", 0));
	widget = GetDlgItem(window, IDC_SERVER_ADDRESS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("ServerConfigGUI", 3));
	widget = GetDlgItem(window, IDC_SERVER_PORT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("ServerConfigGUI", 4));
	widget = GetDlgItem(window, IDC_SERVER_ID_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("ServerConfigGUI", 5));
	widget = GetDlgItem(window, IDC_SERVER_MINUTES_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("ServerConfigGUI", 7));
	widget = GetDlgItem(window, IDC_SERVER_UPDATE_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("ServerConfigGUI", 6));
	widget = GetDlgItem(window, IDC_SERVER_STARTUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("ServerConfigGUI", 1));
	widget = GetDlgItem(window, IDC_SERVER_SYNC_ON_EDIT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("ServerConfigGUI", 9));
	widget = GetDlgItem(window, IDC_SERVER_SYNC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("ServerConfigGUI", 2));
	widget = GetDlgItem(window, IDC_SERVER_COPY);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("ServerConfigGUI", 8));

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

	state = CConfig::Instance().GetServerEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_SERVER_ENABLE, state);

	widget = GetDlgItem(window, IDC_SERVER_ADDRESS);
	SetWindowText(widget, CConfig::Instance().GetServerAddress().c_str());

	widget = GetDlgItem(window, IDC_SERVER_PORT);
	itoa(CConfig::Instance().GetServerPort(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_SERVER_ID);
	SetWindowText(widget, CConfig::Instance().GetServerID().c_str());

	widget = GetDlgItem(window, IDC_SERVER_FREQUENCY);
	itoa(CConfig::Instance().GetServerFrequency(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	state = CConfig::Instance().GetServerStartup() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_SERVER_STARTUP, state);

	state = CConfig::Instance().GetServerSyncOnEdit() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_SERVER_SYNC_ON_EDIT, state);

	UpdateServerWidgets(window);

	return TRUE;
}

BOOL OnOKServerDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_SERVER_ENABLE));
	CConfig::Instance().SetServerEnable(state);

	widget = GetDlgItem(window, IDC_SERVER_ADDRESS);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetServerAddress(tmpSz);

	widget = GetDlgItem(window, IDC_SERVER_PORT);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetServerPort(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_SERVER_FREQUENCY);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetServerFrequency(atoi(tmpSz));

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_SERVER_STARTUP));
	CConfig::Instance().SetServerStartup(state);

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_SERVER_SYNC_ON_EDIT));
	CConfig::Instance().SetServerSyncOnEdit(state);

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
		//	"Please use only numbers (0-9)\nand letters (A-Z, a-z) in the ID.\nThe illegal chars are automatically changed to '_'",
		MessageBox(NULL, CCalendarWindow::c_Language.GetString("Message", 5), "Rainlendar", MB_OK | MB_ICONEXCLAMATION);
	}

	CConfig::Instance().SetServerID(tmpSz);

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
				OnOKServerDialog(hwndDlg);
				GetRainlendar()->GetCalendarWindow().ConnectServer(REQUEST_SYNCRONIZATION);
				return TRUE;

			case IDC_SERVER_ENABLE:
				UpdateServerWidgets(hwndDlg);
				return TRUE;

			case IDC_SERVER_COPY:
				if(OpenClipboard(hwndDlg))
				{
					HGLOBAL clipbuffer;
					char * buffer;
					EmptyClipboard();
					std::string text;
					if (g_Mutex)
					{
						DWORD result = WaitForSingleObject(g_Mutex, 1000L);
						if (result == WAIT_OBJECT_0)
						{
							// Add the string to the widget
							std::list<std::string>::iterator i = g_StatusStrings.begin();
							for( ; i != g_StatusStrings.end(); i++)
							{
								text += (*i);
								text += "\n";
							}

							ReleaseMutex(g_Mutex);
						}
					}

					clipbuffer = GlobalAlloc(GMEM_DDESHARE, text.size() + 1);
					buffer = (char*)GlobalLock(clipbuffer);
					strcpy(buffer, text.c_str());
					GlobalUnlock(clipbuffer);
					SetClipboardData(CF_TEXT, clipbuffer);
					CloseClipboard();
				}
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
		g_Mutex = CreateMutex(NULL, FALSE, "AddStatusStringMutex");
	}

	GetLocalTime(&time);
	sprintf(tmpSz, "[%02i:%02i:%02i] ", time.wHour, time.wMinute, time.wSecond); 
	
	std::string str = tmpSz;
	str += string;

	LSLog(LOG_DEBUG, "Rainlendar", str.c_str());

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
