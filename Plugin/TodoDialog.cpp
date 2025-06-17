/*
  Copyright (C) 2003 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Plugin/TodoDialog.cpp,v 1.4 2004/01/28 18:07:24 rainy Exp $

  $Log: TodoDialog.cpp,v $
  Revision 1.4  2004/01/28 18:07:24  rainy
  Localized ok and cancel.

  Revision 1.3  2004/01/25 10:02:19  rainy
  Added dialog position remembering.

  Revision 1.2  2004/01/10 15:18:11  rainy
  Renamed few functions.

  Revision 1.1  2003/10/27 17:34:53  Rainy
  Initial version.

*/

#pragma warning(disable: 4786)

#include "TodoDialog.h"
#include "RainlendarDLL.h"
#include "TodoManager.h"
#include <Prsht.h>
#include <list>

// Globals
std::vector<std::string> g_TodoList;
std::vector<std::string> g_DoneList;
std::vector<std::string> g_NotesList;
int g_ActiveTodoIndex = -1;
bool g_Invalid = false;

BOOL CALLBACK TodoListPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam); 
BOOL CALLBACK DoneListPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam); 
BOOL CALLBACK NotesListPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

VOID OpenTodoDialog(HWND hwndOwner, HINSTANCE instance, int index)
{
    PROPSHEETPAGE psp[3];
    PROPSHEETHEADER psh;

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = PSP_USETITLE | PSP_PREMATURE;
    psp[0].hInstance = instance;
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_TODO);
    psp[0].pszIcon = NULL;
    psp[0].pfnDlgProc = TodoListPageProc;
    psp[0].pszTitle = CCalendarWindow::c_Language.GetString("Todo", 0);
    psp[0].lParam = 0;
    psp[0].pfnCallback = NULL;

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = PSP_USETITLE | PSP_PREMATURE;
    psp[1].hInstance = instance;
    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_DONE);
    psp[1].pszIcon = NULL;
    psp[1].pfnDlgProc = DoneListPageProc;
    psp[1].pszTitle = CCalendarWindow::c_Language.GetString("Todo", 1);
    psp[1].lParam = 0;
    psp[1].pfnCallback = NULL;

    psp[2].dwSize = sizeof(PROPSHEETPAGE);
    psp[2].dwFlags = PSP_USETITLE | PSP_PREMATURE;
    psp[2].hInstance = instance;
    psp[2].pszTemplate = MAKEINTRESOURCE(IDD_NOTES);
    psp[2].pszIcon = NULL;
    psp[2].pfnDlgProc = NotesListPageProc;
    psp[2].pszTitle = CCalendarWindow::c_Language.GetString("Todo", 2);
    psp[2].lParam = 0;
    psp[2].pfnCallback = NULL;

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP ;
    psh.hwndParent = hwndOwner;
    psh.hInstance = instance;
    psh.pszIcon = NULL;
    psh.pszCaption = (LPSTR)CCalendarWindow::c_Language.GetString("Todo", 0);
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback = NULL;

	CTodoManager& manager = GetRainlendar()->GetCalendarWindow().GetTodoManager();

	g_TodoList = manager.GetTodos();
	g_DoneList = manager.GetDones();
	g_NotesList = manager.GetNotes();

	g_ActiveTodoIndex = index;
	g_Invalid = false;

    if (PropertySheet(&psh) == IDOK)
	{
		manager.GetTodos() = g_TodoList;
		manager.GetDones() = g_DoneList;
		manager.GetNotes() = g_NotesList;
		manager.Write();

		// Refresh but only if todo's are displayed in the window
		if (CConfig::Instance().GetTodoEnable())
		{
			GetRainlendar()->GetCalendarWindow().GetTodoWindow().Refresh(false);
		}
	}

    return;
}

BOOL MeasureItem(LPMEASUREITEMSTRUCT lpmis, std::vector<std::string>& list, HWND widget)
{
	if (lpmis->itemID >= 0 && lpmis->itemID < list.size())
	{
		HDC winDC = GetDC(widget);
		HDC hDC = CreateCompatibleDC(winDC);
		ReleaseDC(widget, winDC);

		HFONT font = (HFONT)SendMessage(widget, WM_GETFONT, NULL, NULL);
		HFONT oldFont = (HFONT)SelectObject(hDC, font);

		RECT rc;
		GetClientRect(widget, &rc);
		DrawText(hDC, list[lpmis->itemID].c_str(), list[lpmis->itemID].size(), &rc, DT_CALCRECT | DT_WORDBREAK);
		SelectObject(hDC, oldFont);
		DeleteDC(hDC);

		lpmis->itemHeight = rc.bottom - rc.top + 3;
	}
    return TRUE;
}

BOOL DrawItem(LPDRAWITEMSTRUCT lpdis, std::vector<std::string>& list)
{
	// If there are no list box items, skip this message. 
	if (lpdis->itemID != -1) 
	{ 
		SetBkMode(lpdis->hDC, TRANSPARENT);
		// Is the item selected? 
		if (lpdis->itemState & ODS_SELECTED) 
		{ 
			FillRect(lpdis->hDC, &lpdis->rcItem, GetSysColorBrush(COLOR_HIGHLIGHT));
			DrawText(lpdis->hDC, list[lpdis->itemID].c_str(), list[lpdis->itemID].size(), &lpdis->rcItem, DT_WORDBREAK);
		} 
		else
		{
			FillRect(lpdis->hDC, &lpdis->rcItem, GetSysColorBrush(COLOR_WINDOW));
			DrawText(lpdis->hDC, list[lpdis->itemID].c_str(), list[lpdis->itemID].size(), &lpdis->rcItem, DT_WORDBREAK);
		}
		
		// Draw separator line
		HPEN pen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
		HPEN oldPen = (HPEN)SelectObject(lpdis->hDC, pen);
		MoveToEx(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.bottom - 1, NULL);
		LineTo(lpdis->hDC, lpdis->rcItem.right, lpdis->rcItem.bottom - 1);
		SelectObject(lpdis->hDC, oldPen);
	}
	return TRUE;
}

BOOL UpdateTodoListWidgets(HWND hwndDlg, bool refreshList)
{
	HWND widget = GetDlgItem(hwndDlg, IDC_TODO_LIST);

	if (refreshList)
	{
		SendMessage(widget, LB_RESETCONTENT, NULL, NULL);

		for (int i = 0; i < g_TodoList.size(); i++)
		{
			SendMessage(widget, LB_ADDSTRING, NULL, (LPARAM)g_TodoList[i].c_str());
		}
	}

	HWND button = GetDlgItem(hwndDlg, IDC_TODO_NEW);
	HWND edit = GetDlgItem(hwndDlg, IDC_TODO_EDIT);
	int index = SendMessage(widget, LB_GETCURSEL, NULL, NULL);
	if (index != LB_ERR && index < g_TodoList.size())
	{
		SetWindowText(button, CCalendarWindow::c_Language.GetString("Todo", 4));	// Modify
		SetWindowText(edit, g_TodoList[index].c_str());
		EnableWindow(GetDlgItem(hwndDlg, IDC_TODO_DONE), TRUE);
	}
	else
	{
		SetWindowText(button, CCalendarWindow::c_Language.GetString("Todo", 3));	// New
		SetWindowText(edit, "");
		EnableWindow(GetDlgItem(hwndDlg, IDC_TODO_DONE), FALSE);
	}

	SetFocus(edit);
	SendMessage(edit, EM_SETSEL, 0, -1);

	return TRUE;
}

BOOL OnInitTodoListDialog(HWND hwndDlg)
{
	if (CConfig::Instance().GetRememberDialogPositions())
	{
		POINT pos = CConfig::Instance().GetDialogPosition(CConfig::DIALOG_TODO);
		SetWindowPos(GetParent(hwndDlg), NULL, pos.x, pos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}

	// Localize OK and Cancel buttons in parent window
	HWND parent = GetParent(hwndDlg);
	HWND button = GetDlgItem(parent, IDOK);
	SetWindowText(button, CCalendarWindow::c_Language.GetString("General", 0));	// OK
	
	button = GetDlgItem(parent, IDCANCEL);
	SetWindowText(button, CCalendarWindow::c_Language.GetString("General", 1));	// Cancel
	

	button = GetDlgItem(hwndDlg, IDC_TODO_DONE);
	SetWindowText(button, CCalendarWindow::c_Language.GetString("Todo", 1));	// Done
	EnableWindow(button, FALSE);

	UpdateTodoListWidgets(hwndDlg, true);

	if (g_ActiveTodoIndex != -1)
	{
		HWND widget = GetDlgItem(hwndDlg, IDC_TODO_LIST);
		SendMessage(widget, LB_SETCURSEL, g_ActiveTodoIndex, NULL);
	}

	UpdateTodoListWidgets(hwndDlg, false);
	return FALSE;
}

BOOL CALLBACK TodoListPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
	RECT rc;
	HWND widget;

    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitTodoListDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_SETACTIVE:
				UpdateTodoListWidgets(hwndDlg, g_Invalid);
				g_Invalid = false;
				return TRUE;

			case PSN_APPLY:
				widget = GetDlgItem(hwndDlg, IDC_TODO_EDIT);
				if (GetWindowTextLength(widget) > 0)
				{
					// New/Modify if there is text written
					SendMessage(hwndDlg, WM_COMMAND, IDC_TODO_NEW, NULL);
				}
				return TRUE;
			}
			break;

		case WM_DESTROY:
			GetWindowRect(GetParent(hwndDlg), &rc);	// Get the sheet position (i.e. parent)
			CConfig::Instance().SetDialogPosition(CConfig::DIALOG_TODO, rc.left, rc.top);
			CConfig::Instance().WriteConfig(CConfig::WRITE_DIALOG_POS);
			break;

		case WM_MEASUREITEM:
			widget = GetDlgItem(hwndDlg, IDC_TODO_LIST);
			return MeasureItem((LPMEASUREITEMSTRUCT)lParam, g_TodoList, widget);

        case WM_DRAWITEM:
			return DrawItem((LPDRAWITEMSTRUCT)lParam, g_TodoList);
			
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_TODO_LIST:
				if (HIWORD(wParam) == LBN_SELCHANGE)
				{
					UpdateTodoListWidgets(hwndDlg, false);
				}
				break;

			case IDC_TODO_NEW:
				{
					HWND widget = GetDlgItem(hwndDlg, IDC_TODO_LIST);
					HWND edit = GetDlgItem(hwndDlg, IDC_TODO_EDIT);
					int len = GetWindowTextLength(edit);
					char* buffer = new char[len + 1];
					GetWindowText(edit, buffer, len + 1);
					int index = SendMessage(widget, LB_GETCURSEL, NULL, NULL);
					if (index != LB_ERR)
					{
						if (strlen(buffer) > 0)
						{
							// Modify old item
							g_TodoList[index] = buffer;
						}
						else
						{
							if (MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("Todo", 7), "Rainlendar", MB_YESNO) == IDYES)
							{
								g_TodoList.erase(g_TodoList.begin() + index);
							}
						}
					}
					else
					{
						// Add new item
						if (strlen(buffer) > 0)
						{
							g_TodoList.push_back(buffer);
						}
					}

					UpdateTodoListWidgets(hwndDlg, true);
				}
				return TRUE;

			case IDC_TODO_DONE:
				{
					HWND widget = GetDlgItem(hwndDlg, IDC_TODO_LIST);
					int index = SendMessage(widget, LB_GETCURSEL, NULL, NULL);
					if (index != LB_ERR)
					{
						g_DoneList.push_back(g_TodoList[index]);
						g_TodoList.erase(g_TodoList.begin() + index);
						g_Invalid = true;
					}
					UpdateTodoListWidgets(hwndDlg, true);
				}
				return TRUE;
			}
			break;
	}
    return FALSE; 
} 

BOOL UpdateDoneListWidgets(HWND hwndDlg, bool refreshList)
{
	HWND widget = GetDlgItem(hwndDlg, IDC_DONE_LIST);

	if (refreshList)
	{
		SendMessage(widget, LB_RESETCONTENT, NULL, NULL);

		for (int i = 0; i < g_DoneList.size(); i++)
		{
			SendMessage(widget, LB_ADDSTRING, NULL, (LPARAM)g_DoneList[i].c_str());
		}

	}

	int index = SendMessage(widget, LB_GETCURSEL, NULL, NULL);
	if (index != LB_ERR)
	{
		EnableWindow(GetDlgItem(hwndDlg, IDC_DONE_DELETE), TRUE);
		EnableWindow(GetDlgItem(hwndDlg, IDC_DONE_UNDO), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hwndDlg, IDC_DONE_DELETE), FALSE);
		EnableWindow(GetDlgItem(hwndDlg, IDC_DONE_UNDO), FALSE);
	}

	return TRUE;
}

BOOL OnInitDoneListDialog(HWND hwndDlg)
{
	HWND button = GetDlgItem(hwndDlg, IDC_DONE_DELETE);
	SetWindowText(button, CCalendarWindow::c_Language.GetString("Todo", 5));	// Delete
	EnableWindow(button, FALSE);

	button = GetDlgItem(hwndDlg, IDC_DONE_UNDO);
	SetWindowText(button, CCalendarWindow::c_Language.GetString("Todo", 6));	// Undo
	EnableWindow(button, FALSE);

	UpdateDoneListWidgets(hwndDlg, true);
	return TRUE;
}


BOOL CALLBACK DoneListPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
	HWND widget;
	int index;

    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitDoneListDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_SETACTIVE:
				if (g_Invalid)
				{
					UpdateDoneListWidgets(hwndDlg, true);
					g_Invalid = false;
				}
				return TRUE;
			}
			break;

		case WM_MEASUREITEM:
			widget = GetDlgItem(hwndDlg, IDC_DONE_LIST);
			return MeasureItem((LPMEASUREITEMSTRUCT)lParam, g_DoneList, widget);

        case WM_DRAWITEM:
			return DrawItem((LPDRAWITEMSTRUCT)lParam, g_DoneList);
			
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_DONE_LIST:
				if (HIWORD(wParam) == LBN_SELCHANGE)
				{
					UpdateDoneListWidgets(hwndDlg, false);
				}
				break;

			case IDC_DONE_DELETE:
				widget = GetDlgItem(hwndDlg, IDC_DONE_LIST);
				index = SendMessage(widget, LB_GETCURSEL, NULL, NULL);
				if (index != LB_ERR)
				{
					if (MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("Todo", 7), "Rainlendar", MB_YESNO) == IDYES)
					{
						g_DoneList.erase(g_DoneList.begin() + index);
					}
				}
				UpdateDoneListWidgets(hwndDlg, true);
				return TRUE;

			case IDC_DONE_UNDO:
				widget = GetDlgItem(hwndDlg, IDC_DONE_LIST);
				index = SendMessage(widget, LB_GETCURSEL, NULL, NULL);
				if (index != LB_ERR)
				{
					g_TodoList.push_back(g_DoneList[index]);
					g_DoneList.erase(g_DoneList.begin() + index);
					g_Invalid = true;
				}
				UpdateDoneListWidgets(hwndDlg, true);
				return TRUE;
			}
			break;
	}
    return FALSE; 
}

BOOL UpdateNotesListWidgets(HWND hwndDlg, bool refreshList)
{
	HWND widget = GetDlgItem(hwndDlg, IDC_NOTES_LIST);

	if (refreshList)
	{
		SendMessage(widget, LB_RESETCONTENT, NULL, NULL);

		for (int i = 0; i < g_NotesList.size(); i++)
		{
			SendMessage(widget, LB_ADDSTRING, NULL, (LPARAM)g_NotesList[i].c_str());
		}
	}

	HWND button = GetDlgItem(hwndDlg, IDC_NOTES_NEW);
	HWND edit = GetDlgItem(hwndDlg, IDC_NOTES_EDIT);
	int index = SendMessage(widget, LB_GETCURSEL, NULL, NULL);
	if (index != LB_ERR && index < g_NotesList.size())
	{
		SetWindowText(button, CCalendarWindow::c_Language.GetString("Todo", 4));	// Modify
		SetWindowText(edit, g_NotesList[index].c_str());
		EnableWindow(GetDlgItem(hwndDlg, IDC_NOTES_DELETE), TRUE);
	}
	else
	{
		SetWindowText(button, CCalendarWindow::c_Language.GetString("Todo", 3));	// New
		SetWindowText(edit, "");
		EnableWindow(GetDlgItem(hwndDlg, IDC_NOTES_DELETE), FALSE);
	}

	SetFocus(edit);
	SendMessage(edit, EM_SETSEL, 0, -1);

	return TRUE;
}

BOOL OnInitNotesListDialog(HWND hwndDlg)
{
	HWND button = GetDlgItem(hwndDlg, IDC_NOTES_DELETE);
	SetWindowText(button, CCalendarWindow::c_Language.GetString("Todo", 5));	// Delete
	EnableWindow(button, FALSE);

	UpdateNotesListWidgets(hwndDlg, true);
	return TRUE;
}

BOOL CALLBACK NotesListPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
	HWND widget;

    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitNotesListDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_KILLACTIVE:
				SendMessage(GetDlgItem(hwndDlg, IDC_NOTES_LIST), LB_SETCURSEL, -1, NULL);
				return TRUE;

			case PSN_SETACTIVE:
				UpdateNotesListWidgets(hwndDlg, g_Invalid);
				g_Invalid = false;
				return TRUE;

			case PSN_APPLY:
				widget = GetDlgItem(hwndDlg, IDC_NOTES_EDIT);
				if (GetWindowTextLength(widget) > 0)
				{
					// New/Modify if there is text written
					SendMessage(hwndDlg, WM_COMMAND, IDC_NOTES_NEW, NULL);
				}
				return TRUE;
			}
			break;

		case WM_MEASUREITEM:
			widget = GetDlgItem(hwndDlg, IDC_NOTES_LIST);
			return MeasureItem((LPMEASUREITEMSTRUCT)lParam, g_NotesList, widget);

        case WM_DRAWITEM:
			return DrawItem((LPDRAWITEMSTRUCT)lParam, g_NotesList);
			
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_NOTES_LIST:
				if (HIWORD(wParam) == LBN_SELCHANGE)
				{
					UpdateNotesListWidgets(hwndDlg, false);
				}
				break;

			case IDC_NOTES_NEW:
				{
					HWND widget = GetDlgItem(hwndDlg, IDC_NOTES_LIST);
					HWND edit = GetDlgItem(hwndDlg, IDC_NOTES_EDIT);
					int len = GetWindowTextLength(edit);
					char* buffer = new char[len + 1];
					GetWindowText(edit, buffer, len + 1);
					int index = SendMessage(widget, LB_GETCURSEL, NULL, NULL);
					if (index != LB_ERR)
					{
						if (strlen(buffer) > 0)
						{
							// Modify old item
							g_NotesList[index] = buffer;
						}
						else
						{
							if (MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("Todo", 7), "Rainlendar", MB_YESNO) == IDYES)
							{
								g_NotesList.erase(g_NotesList.begin() + index);
							}
						}
					}
					else
					{
						// Add new item
						if (strlen(buffer) > 0)
						{
							g_NotesList.push_back(buffer);
						}
					}

					UpdateNotesListWidgets(hwndDlg, true);
				}
				return TRUE;

			case IDC_NOTES_DELETE:
				{
					HWND widget = GetDlgItem(hwndDlg, IDC_NOTES_LIST);
					int index = SendMessage(widget, LB_GETCURSEL, NULL, NULL);
					if (index != LB_ERR)
					{
						if (MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("Todo", 7), "Rainlendar", MB_YESNO) == IDYES)
						{
							g_NotesList.erase(g_NotesList.begin() + index);
						}
					}
					UpdateNotesListWidgets(hwndDlg, true);
				}
				return TRUE;
			}
			break;
	}
    return FALSE; 
} 
