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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/ConfigDialog.cpp,v 1.10 2003/08/09 16:38:27 Rainy Exp $

  $Log: ConfigDialog.cpp,v $
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
	CONFIG_MESSAGE_ONDESKTOP = 1,
	CONFIG_MESSAGE_TRANSPARENCY
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
		CCalendarWindow::c_Config.WriteConfig(CConfig::WRITE_FULL);
		// Refresh
		SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);
	}

    return;
}

void UpdateGeneralWidgets(HWND window)
{
	if (IsDlgButtonChecked(window, IDC_SHOW_OUTLOOK_APPOINTMENTS) == BST_CHECKED)
	{
		// Enable all
		EnableWindow(GetDlgItem(window, IDC_OUTLOOK_UPDATE), TRUE);
	}
	else
	{
		// Disable all
		EnableWindow(GetDlgItem(window, IDC_OUTLOOK_UPDATE), FALSE);
	}
}

BOOL OnInitGeneralDialog(HWND window) 
{
	char tmpSz[256];
	UINT state;
	HWND widget;

	state = CCalendarWindow::c_Config.GetEventToolTips() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENT_TOOLTIP, state);
	state = CCalendarWindow::c_Config.GetEventMessageBox() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENT_MESSAGEBOX, state);
	widget = GetDlgItem(window, IDC_EVENT_EXECUTE);
	SetWindowText(widget, CCalendarWindow::c_Config.GetEventExecute().c_str());

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
	state = CCalendarWindow::c_Config.GetSnapEdges() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_SNAP_EDGES, state);
	state = CCalendarWindow::c_Config.GetNativeTransparency() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_NATIVE_TRANSPARENCY, state);
	state = CCalendarWindow::c_Config.GetRefreshOnResolutionChange() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_REFRESH_ON_RESOLUTION_CHANGE, state);
	state = CCalendarWindow::c_Config.GetWeek1HasJanuary1st() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_WEEK_1_JANUARY_1, state);
	state = CCalendarWindow::c_Config.GetTooltipSeparator() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_TOOLTIP_SEPARATOR, state);

	state = CCalendarWindow::c_Config.GetShowOutlookAppointments() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_SHOW_OUTLOOK_APPOINTMENTS, state);
	widget = GetDlgItem(window, IDC_OUTLOOK_UPDATE);
	itoa(CCalendarWindow::c_Config.GetOutlookUpdate(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	// Set localized strings
	widget = GetDlgItem(GetParent(window), IDOK);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("General", 0));
	widget = GetDlgItem(GetParent(window), IDCANCEL);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("General", 1));
	widget = GetDlgItem(window, IDC_EVENT_TOOLTIP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 0));
	widget = GetDlgItem(window, IDC_EVENT_MESSAGEBOX);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 1));
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

	widget = GetDlgItem(window, IDC_SHOW_OUTLOOK_APPOINTMENTS);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 13));
	widget = GetDlgItem(window, IDC_OUTLOOK_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 16));
	widget = GetDlgItem(window, IDC_OUTLOOK_UPDATE_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 17));
	widget = GetDlgItem(window, IDC_OUTLOOK_MINUTES_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 18));

	if (!CCalendarWindow::Is2k())
	{
		// Disable the button if we're running wintendo
		EnableWindow(GetDlgItem(window, IDC_NATIVE_TRANSPARENCY), FALSE);
	}

	UpdateGeneralWidgets(window);

	if (CCalendarWindow::c_Config.GetRememberDialogPositions())
	{
		POINT pos = CCalendarWindow::c_Config.GetDialogPosition(CConfig::DIALOG_CONFIG);
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
	CCalendarWindow::c_Config.SetEventToolTips(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_MESSAGEBOX));
	CCalendarWindow::c_Config.SetEventMessageBox(state);
	widget = GetDlgItem(window, IDC_EVENT_EXECUTE);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetEventExecute(tmpSz);

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
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_SNAP_EDGES));
	CCalendarWindow::c_Config.SetSnapEdges(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_NATIVE_TRANSPARENCY));
	CCalendarWindow::c_Config.SetNativeTransparency(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_REFRESH_ON_RESOLUTION_CHANGE));
	CCalendarWindow::c_Config.SetRefreshOnResolutionChange(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WEEK_1_JANUARY_1));
	CCalendarWindow::c_Config.SetWeek1HasJanuary1st(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_TOOLTIP_SEPARATOR));
	CCalendarWindow::c_Config.SetTooltipSeparator(state);

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_SHOW_OUTLOOK_APPOINTMENTS));
	CCalendarWindow::c_Config.SetShowOutlookAppointments(state);
	widget = GetDlgItem(window, IDC_OUTLOOK_UPDATE);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetOutlookUpdate(atoi(tmpSz));

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
				CCalendarWindow::c_Config.SetDialogPosition(CConfig::DIALOG_CONFIG, rc.left, rc.top);
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_SHOW_OUTLOOK_APPOINTMENTS:
				UpdateGeneralWidgets(hwndDlg);
				return TRUE;

			case IDC_NATIVE_TRANSPARENCY:
				if (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_NATIVE_TRANSPARENCY))
				{
					// Notify the other sheets that native transparency was selected
					PropSheet_QuerySiblings(GetParent(hwndDlg), CONFIG_MESSAGE_TRANSPARENCY, 0);
				}
				return TRUE;
			}
			break;

		case PSM_QUERYSIBLINGS:
			if (wParam == CONFIG_MESSAGE_ONDESKTOP)
			{
				if (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_NATIVE_TRANSPARENCY))
				{
					// Disable native transparency and inform the user about it
					MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("GeneralConfigGUI", 19), "Rainlendar", MB_OK | MB_ICONWARNING);
					CheckDlgButton(hwndDlg, IDC_NATIVE_TRANSPARENCY, BST_UNCHECKED);
					return TRUE;	// No need to send this any further
				}
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

	case CConfig::WINDOWPOS_ONDESKTOP:
		check = IDC_WINDOW_ONDESKTOP;
		break;

	case CConfig::WINDOWPOS_NORMAL:
	default:
		check = IDC_WINDOW_NORMAL;
		break;
	}
	CheckRadioButton(window, IDC_WINDOW_ONDESKTOP, IDC_WINDOW_ONTOP, check);

	state = CCalendarWindow::c_Config.GetMovable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_WINDOW_MOVABLE, state);
	state = CCalendarWindow::c_Config.GetMouseHide() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_WINDOW_MOUSEHIDE, state);
	state = CCalendarWindow::c_Config.GetRememberDialogPositions() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_REMEMBER_DIALOG_POSITIONS, state);
	state = CCalendarWindow::c_Config.GetNegativeCoords() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_NEGATIVE_COORDS, state);

	widget = GetDlgItem(window, IDC_VERTICAL_COUNT);
	itoa(CCalendarWindow::c_Config.GetVerticalCount(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_HORIZONTAL_COUNT);
	itoa(CCalendarWindow::c_Config.GetHorizontalCount(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_PREVIOUS_MONTHS);
	itoa(CCalendarWindow::c_Config.GetPreviousMonths(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	state = CCalendarWindow::c_Config.GetStartFromJanuary() ? BST_CHECKED : BST_UNCHECKED;
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
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_WINDOW_ONDESKTOP))
	{
		CCalendarWindow::c_Config.SetWindowPos(CConfig::WINDOWPOS_ONDESKTOP);
	}
	else
	{
		CCalendarWindow::c_Config.SetWindowPos(CConfig::WINDOWPOS_NORMAL);
	}

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WINDOW_MOVABLE));
	CCalendarWindow::c_Config.SetMovable(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WINDOW_MOUSEHIDE));
	CCalendarWindow::c_Config.SetMouseHide(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_REMEMBER_DIALOG_POSITIONS));
	CCalendarWindow::c_Config.SetRememberDialogPositions(state);
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_NEGATIVE_COORDS));
	CCalendarWindow::c_Config.SetNegativeCoords(state);

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
	CCalendarWindow::c_Config.SetVerticalCount(value);

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
	CCalendarWindow::c_Config.SetHorizontalCount(value);

	widget = GetDlgItem(window, IDC_PREVIOUS_MONTHS);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetPreviousMonths(atoi(tmpSz));

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_LAYOUT_JANUARY));
	CCalendarWindow::c_Config.SetStartFromJanuary(state);

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
				UpdateLayoutWidgets(hwndDlg);
				return TRUE;

			case IDC_WINDOW_ONDESKTOP:
				if (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_WINDOW_ONDESKTOP))
				{
					// Notify the other sheets that native transparency was selected
					PropSheet_QuerySiblings(GetParent(hwndDlg), CONFIG_MESSAGE_ONDESKTOP, 0);
				}
				return TRUE;
			}
			break;

		case PSM_QUERYSIBLINGS:
			if (wParam == CONFIG_MESSAGE_TRANSPARENCY)
			{
				if (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_WINDOW_ONDESKTOP))
				{
					// Disable OnDesktop and inform the user about it
					MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("LayoutConfigGUI", 16), "Rainlendar", MB_OK | MB_ICONWARNING);
					CheckRadioButton(hwndDlg, IDC_WINDOW_ONDESKTOP, IDC_WINDOW_ONTOP, IDC_WINDOW_NORMAL);
					return TRUE;	// No need to send this any further
				}
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


	// Set hotkeys
	for (int i = 0; i < CConfig::HOTKEY_LAST; i++)
	{
		widget = GetDlgItem(window, IDC_HOTKEYS_HIDE + i);

		switch(i)
		{
		case CConfig::HOTKEY_HIDE:
			hotkey = CCalendarWindow::c_Config.GetHideHotkey();
			break;

		case CConfig::HOTKEY_SHOW:
            hotkey = CCalendarWindow::c_Config.GetShowHotkey();
			break;

		case CConfig::HOTKEY_TOGGLE:
            hotkey = CCalendarWindow::c_Config.GetToggleHotkey();
			break;

		case CConfig::HOTKEY_ACTIVATE:
            hotkey =  CCalendarWindow::c_Config.GetActivateHotkey();
			break;

		case CConfig::HOTKEY_REFRESH:
            hotkey = CCalendarWindow::c_Config.GetRefreshHotkey();
			break;

		case CConfig::HOTKEY_CONFIG:
            hotkey = CCalendarWindow::c_Config.GetConfigHotkey();
			break;

		case CConfig::HOTKEY_SKIN:
            hotkey = CCalendarWindow::c_Config.GetSkinHotkey();
			break;

		case CConfig::HOTKEY_NEXT:
            hotkey = CCalendarWindow::c_Config.GetNextHotkey();
			break;

		case CConfig::HOTKEY_PREVIOUS:
            hotkey = CCalendarWindow::c_Config.GetPreviousHotkey();
			break;

		case CConfig::HOTKEY_NEXT_YEAR:
            hotkey = CCalendarWindow::c_Config.GetNextYearHotkey();
			break;

		case CConfig::HOTKEY_PREVIOUS_YEAR:
            hotkey = CCalendarWindow::c_Config.GetPreviousYearHotkey();
			break;

		case CConfig::HOTKEY_CURRENT:
            hotkey = CCalendarWindow::c_Config.GetCurrentHotkey();
			break;

		case CConfig::HOTKEY_ALL:
            hotkey = CCalendarWindow::c_Config.GetAllHotkey();
			break;

		case CConfig::HOTKEY_OUTLOOK:
            hotkey = CCalendarWindow::c_Config.GetOutlookHotkey();
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
			if (widget) CCalendarWindow::c_Config.SetHideHotkey(hotkey);
			break;

		case CConfig::HOTKEY_SHOW:
			if (widget) CCalendarWindow::c_Config.SetShowHotkey(hotkey);
			break;

		case CConfig::HOTKEY_TOGGLE:
			if (widget) CCalendarWindow::c_Config.SetToggleHotkey(hotkey);
			break;

		case CConfig::HOTKEY_ACTIVATE:
			if (widget) CCalendarWindow::c_Config.SetActivateHotkey(hotkey);
			break;

		case CConfig::HOTKEY_REFRESH:
			if (widget) CCalendarWindow::c_Config.SetRefreshHotkey(hotkey);
			break;

		case CConfig::HOTKEY_CONFIG:
			if (widget) CCalendarWindow::c_Config.SetConfigHotkey(hotkey);
			break;

		case CConfig::HOTKEY_SKIN:
			if (widget) CCalendarWindow::c_Config.SetSkinHotkey(hotkey);
			break;

		case CConfig::HOTKEY_NEXT:
			if (widget) CCalendarWindow::c_Config.SetNextHotkey(hotkey);
			break;

		case CConfig::HOTKEY_PREVIOUS:
			if (widget) CCalendarWindow::c_Config.SetPreviousHotkey(hotkey);
			break;

		case CConfig::HOTKEY_NEXT_YEAR:
			if (widget) CCalendarWindow::c_Config.SetNextYearHotkey(hotkey);
			break;

		case CConfig::HOTKEY_PREVIOUS_YEAR:
			if (widget) CCalendarWindow::c_Config.SetPreviousYearHotkey(hotkey);
			break;

		case CConfig::HOTKEY_CURRENT:
			if (widget) CCalendarWindow::c_Config.SetCurrentHotkey(hotkey);
			break;

		case CConfig::HOTKEY_ALL:
			if (widget) CCalendarWindow::c_Config.SetAllHotkey(hotkey);
			break;

		case CConfig::HOTKEY_OUTLOOK:
			if (widget) CCalendarWindow::c_Config.SetOutlookHotkey(hotkey);
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

	state = CCalendarWindow::c_Config.GetServerSyncOnEdit() ? BST_CHECKED : BST_UNCHECKED;
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

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_SERVER_SYNC_ON_EDIT));
	CCalendarWindow::c_Config.SetServerSyncOnEdit(state);

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
