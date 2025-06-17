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
  $Header: /home/cvsroot/Rainlendar/Plugins/PluginICalendar/SetupDialog.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: SetupDialog.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.1  2005/03/01 18:26:06  rainy
  Initial version.


*/

#include <windows.h>
#include "iCalPlugin.h"
#include "resource.h"
#include <commctrl.h>

extern std::vector<ICALFILENAME> g_EventFiles;
extern bool g_MonitorChanges;
extern bool g_IgnoreCompletedTasks;
extern HINSTANCE g_Instance;

std::vector<ICALFILENAME> g_TmpEventFiles;

HIMAGELIST g_ImageListNormal = NULL;
HIMAGELIST g_ImageListPressed = NULL;
HIMAGELIST g_ImageListDisabled = NULL;

#define BUTTON_SIZE 24
#define BUTTON_COUNT 3

void AddTip(HWND hwndParent, HWND hwndTip, HWND button, LPCSTR lpszText)
{
	TOOLINFO ti;
	memset(&ti, 0, sizeof(TOOLINFO));
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
	ti.hwnd = hwndParent;
	ti.uId = (UINT)button;
	ti.lpszText = (LPSTR)lpszText;
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
}

void CreateToolBars(HWND hwndParent)
{ 
	HWND button;

	// Add tooltips
	HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
                            WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            hwndParent, NULL, g_Instance,
                            NULL);

	SetWindowPos(hwndTip, HWND_TOPMOST,0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	RECT rc;
	HWND fileList = GetDlgItem(hwndParent, IDC_FILELIST);
	GetClientRect(fileList, &rc);
	MapWindowPoints(fileList, hwndParent, (LPPOINT)&rc, 2);
	
	int x = rc.right;
	int y = rc.top - BUTTON_SIZE - 5;

	button = CreateWindowEx(0, "BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, x - BUTTON_SIZE * 3, y, BUTTON_SIZE, BUTTON_SIZE, hwndParent, (HMENU)(IDC_BUTTON_ADD), g_Instance, NULL);
	AddTip(hwndParent, hwndTip, button, Rainlendar_GetString(PLUGIN_NAME, 7, _T("Add")));
	button = CreateWindowEx(0, "BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, x - BUTTON_SIZE * 2, y, BUTTON_SIZE, BUTTON_SIZE, hwndParent, (HMENU)(IDC_BUTTON_REMOVE), g_Instance, NULL);
	AddTip(hwndParent, hwndTip, button, Rainlendar_GetString(PLUGIN_NAME, 8, _T("Remove")));
	button = CreateWindowEx(0, "BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, x - BUTTON_SIZE * 1, y, BUTTON_SIZE, BUTTON_SIZE, hwndParent, (HMENU)(IDC_BUTTON_READONLY), g_Instance, NULL);
	AddTip(hwndParent, hwndTip, button, Rainlendar_GetString(PLUGIN_NAME, 9, _T("Toggle type")));
} 

void SetToolBarState(HWND hwndParent)
{
	HWND widget = GetDlgItem(hwndParent, IDC_FILELIST);
	if (widget) 
	{
		HWND btn;
		int index = ListView_GetSelectedCount(widget);
		
		BOOL enabled = TRUE;
		if (index == 0)
		{
			enabled = FALSE;
		}

		// Add is always enabled

		btn = GetDlgItem(hwndParent, IDC_BUTTON_REMOVE);
		if (btn) EnableWindow(btn, enabled);

		btn = GetDlgItem(hwndParent, IDC_BUTTON_READONLY);
		if (btn) EnableWindow(btn, enabled);
	}
}

bool DrawToolBar(LPDRAWITEMSTRUCT dis, UINT id)
{
	int x = 0, y = 0;

	HIMAGELIST iList = g_ImageListNormal;
	if (dis->itemState & ODS_DISABLED)
	{
		iList = g_ImageListDisabled;
	}
	else if (dis->itemState & ODS_SELECTED)
	{
		iList = g_ImageListPressed;
	}

	switch(id)
	{
	case IDC_BUTTON_REMOVE:
		ImageList_Draw(iList, 0, dis->hDC, x, y, ILD_NORMAL);
		break;

	case IDC_BUTTON_ADD:
		ImageList_Draw(iList, 1, dis->hDC, x, y, ILD_NORMAL);
		break;

	case IDC_BUTTON_READONLY:
		ImageList_Draw(iList, 2, dis->hDC, x, y, ILD_NORMAL);
		break;
	}
	
	return TRUE;
}

void UpdateWidgets(HWND window)
{
	HWND widget = GetDlgItem(window, IDC_FILELIST);
	std::vector<ICALFILENAME>* list = &g_TmpEventFiles;

	if (widget)
	{
		ListView_DeleteAllItems(widget);

		// Add Items
		LVITEM lvI; 
		
		lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
		lvI.state = 0; 
		lvI.stateMask = 0; 
		
		for (int i = 0; i < list->size(); i++)
		{
			lvI.iItem = i;
			lvI.iSubItem = 0;
			lvI.pszText = (char*)(*list)[i].filename.c_str();
			lvI.lParam = (LPARAM)i;
			ListView_InsertItem(widget, &lvI);

			if ((*list)[i].readOnly)
			{
				ListView_SetItemText(widget, i, 1, (TCHAR*)Rainlendar_GetString(PLUGIN_NAME, 0, _T("Read Only")));
			}
			else
			{
				ListView_SetItemText(widget, i, 1, (TCHAR*)Rainlendar_GetString(PLUGIN_NAME, 1, _T("Read/Write")));
			}
		}
	}

	SetToolBarState(window);
}

BOOL OnInitSetupDialog(HWND hwndDlg)
{
	SetWindowText(hwndDlg, Plugin_GetName());
	
	HWND widget = GetDlgItem(hwndDlg, IDC_FILELIST);
	if(widget ) 
	{
		// Add Columns
		LVCOLUMN lvc; 
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
		lvc.iSubItem = 0;
		lvc.pszText = (TCHAR*)Rainlendar_GetString(PLUGIN_NAME, 2, _T("Filename"));
		lvc.cx = 250;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 0, &lvc);
		
		lvc.iSubItem = 1;
		lvc.pszText = (TCHAR*)Rainlendar_GetString(PLUGIN_NAME, 3, _T("Type"));
		lvc.cx = 100;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 1, &lvc);
	
		ListView_SetExtendedListViewStyle(widget, LVS_EX_FULLROWSELECT);
	}
	g_TmpEventFiles = g_EventFiles;

	CheckDlgButton(hwndDlg, IDC_MONITOR_CHANGES, g_MonitorChanges ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hwndDlg, IDC_COMPLETED_TASKS, g_IgnoreCompletedTasks ? BST_CHECKED : BST_UNCHECKED);

	UpdateWidgets(hwndDlg);

	widget = GetDlgItem(hwndDlg, IDOK);
	if (widget) SetWindowText(widget, Rainlendar_GetString(_T("General"), 0, _T("OK")));

	widget = GetDlgItem(hwndDlg, IDCANCEL);
	if (widget) SetWindowText(widget, Rainlendar_GetString(_T("General"), 1, _T("Cancel")));

	widget = GetDlgItem(hwndDlg, IDC_EVENT_FILES);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 4, _T("iCal Files")));

	widget = GetDlgItem(hwndDlg, IDC_MONITOR_CHANGES);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 5, _T("Refresh when file changes")));

	widget = GetDlgItem(hwndDlg, IDC_COMPLETED_TASKS);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 6, _T("Ignore completed tasks")));

	HBITMAP bitmap;
	g_ImageListNormal = ImageList_Create(BUTTON_SIZE, BUTTON_SIZE, ILC_COLOR32, BUTTON_COUNT, BUTTON_COUNT);
	bitmap = LoadBitmap(g_Instance, MAKEINTRESOURCE(IDB_BUTTONS_NORMAL));
	ImageList_Add(g_ImageListNormal, bitmap, NULL);
	DeleteObject(bitmap);

	g_ImageListPressed = ImageList_Create(BUTTON_SIZE, BUTTON_SIZE, ILC_COLOR32, BUTTON_COUNT, BUTTON_COUNT);
	bitmap = LoadBitmap(g_Instance, MAKEINTRESOURCE(IDB_BUTTONS_PRESSED));
	ImageList_Add(g_ImageListPressed, bitmap, NULL);
	DeleteObject(bitmap);

	g_ImageListDisabled = ImageList_Create(BUTTON_SIZE, BUTTON_SIZE, ILC_COLOR32, BUTTON_COUNT, BUTTON_COUNT);
	bitmap = LoadBitmap(g_Instance, MAKEINTRESOURCE(IDB_BUTTONS_DISABLED));
	ImageList_Add(g_ImageListDisabled, bitmap, NULL);
	DeleteObject(bitmap);

	CreateToolBars(hwndDlg);
	SetToolBarState(hwndDlg);

	return TRUE;
}

void OnDestroySetupDialog()
{
	ImageList_Destroy(g_ImageListNormal);
	ImageList_Destroy(g_ImageListPressed);
	ImageList_Destroy(g_ImageListDisabled);
}

BOOL BrowseFile(HWND hwndDlg, std::vector<ICALFILENAME>& list)
{
	char file[_MAX_PATH];
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	file[0] = 0;

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwndDlg;
	ofn.lpstrFile = file;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrFilter = _T("iCal Calendar File (*.ics)\0*.ics\0");
//	ofn.Flags = OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		ICALFILENAME iniFile;
		iniFile.filename = file;
		if (ofn.Flags | OFN_NOREADONLYRETURN)
		{
			iniFile.readOnly = FALSE;
		}
		else
		{
			iniFile.readOnly = TRUE;
		}
		list.push_back(iniFile);

		UpdateWidgets(hwndDlg);
	}

	return TRUE;
}

BOOL CALLBACK SetupProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitSetupDialog(hwndDlg);
	
        case WM_CLOSE:
			OnDestroySetupDialog();
			EndDialog(hwndDlg, IDCANCEL);
            return TRUE; 
			
        case WM_COMMAND:
            switch (LOWORD(wParam)) 
            {
                case IDC_BUTTON_ADD:
					BrowseFile(hwndDlg, g_TmpEventFiles);
					break;

                case IDC_BUTTON_REMOVE:
					{
						HWND widget = GetDlgItem(hwndDlg, IDC_FILELIST);
						int index = ListView_GetNextItem(widget, -1, LVNI_SELECTED);
						if (index != -1 && index < g_TmpEventFiles.size())
						{
							g_TmpEventFiles.erase(g_TmpEventFiles.begin() + index);
						}
						UpdateWidgets(hwndDlg);
					}
					break;

                case IDC_BUTTON_READONLY:
					{
						HWND widget = GetDlgItem(hwndDlg, IDC_FILELIST);
						int index = ListView_GetNextItem(widget, -1, LVNI_SELECTED);
						if (index != -1 && index < g_TmpEventFiles.size())
						{
							g_TmpEventFiles[index].readOnly = !g_TmpEventFiles[index].readOnly;
						}
						UpdateWidgets(hwndDlg);
					}
					break;

                case IDOK:
					g_EventFiles = g_TmpEventFiles;

					if (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_MONITOR_CHANGES))
					{
						g_MonitorChanges = true;
					}
					else
					{
						g_MonitorChanges = false;
					}

					if (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_COMPLETED_TASKS))
					{
						g_IgnoreCompletedTasks = true;
					}
					else
					{
						g_IgnoreCompletedTasks = false;
					}

					OnDestroySetupDialog();
					EndDialog(hwndDlg, IDOK);
                    return TRUE; 

                case IDCANCEL:
					OnDestroySetupDialog();
					EndDialog(hwndDlg, IDCANCEL);
                    return TRUE; 
            }
            break;

        case WM_DRAWITEM:
			return DrawToolBar((LPDRAWITEMSTRUCT)lParam, wParam);

        case WM_NOTIFY:
			{
				LPNMHDR pNMHDR = (LPNMHDR)lParam;

				if(pNMHDR->code == LVN_ITEMCHANGED)
				{
					SetToolBarState(hwndDlg);
				}
			}
    } 
    return FALSE; 
} 

