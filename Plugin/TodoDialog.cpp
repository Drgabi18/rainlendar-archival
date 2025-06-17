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
  $Header: /home/cvsroot/Rainlendar/Plugin/TodoDialog.cpp,v 1.7 2004/12/05 18:21:48 rainy Exp $

  $Log: TodoDialog.cpp,v $
  Revision 1.7  2004/12/05 18:21:48  rainy
  Added profiles to todo items.

  Revision 1.6  2004/11/06 13:38:59  rainy
  no message

  Revision 1.5  2004/06/10 16:21:10  rainy
  Lines are now gray.

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
#pragma warning(disable: 4996)

#include "TodoDialog.h"
#include "RainlendarDLL.h"
#include "EventManager.h"
#include <Prsht.h>
#include <list>
#include <algorithm>

// Globals
std::vector<CEntryTodo*> g_TodoList;
std::vector<CEntryTodo*> g_DoneList;
std::vector<CEntryTodo*> g_NotesList;
std::vector<CEntryTodo*> g_DeletedList;

int g_ActiveTodoIndex = -1;
bool g_Invalid = false;
static HWND g_Dialog = NULL;
HIMAGELIST g_ImageListEnabled = NULL;
HIMAGELIST g_ImageListDisabled = NULL;

#define BUTTON_SIZE 24
#define BUTTON_COUNT 5

BOOL CALLBACK TodoListPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam); 
BOOL CALLBACK DoneListPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam); 
BOOL CALLBACK NotesListPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

bool TodoCompare(CEntryTodo* a, CEntryTodo* b)
{
   return a->GetPosition() < b->GetPosition();
}


void OpenTodoDialog(HWND hwndOwner, HINSTANCE instance, GUID* guid)
{
	CPluginManager& pluginManager = GetRainlendar()->GetCalendarWindow().GetPluginManager();
	CEventManager& eventManager = GetRainlendar()->GetCalendarWindow().GetEventManager();

	CPlugin* plugin = NULL;

	if (guid)
	{
		CEntryTodo* oldTodo = eventManager.GetTodo(guid);

		if (oldTodo)
		{
			// Get the correct plugin from the event
			UINT index = oldTodo->GetPluginID();	// Index starts from 1
			const std::vector<CPlugin*> plugins = pluginManager.GetPlugins();
			if (index > 0 && index <= plugins.size())
			{
				plugin = plugins[index - 1];
			}
			else			
			{
				// Use default plugin
				plugin = pluginManager.GetDefaultPlugin();
			}

			if (plugin && plugin->HasEditItem() && plugin->IsEnabled())
			{
				plugin->EditItem(hwndOwner, oldTodo->GetRainlendarTodo(), RAINLENDAR_TYPE_TODO);
				eventManager.FlushBufferedItems(true);
				GetRainlendar()->GetCalendarWindow().RedrawAll();
			}
			else
			{
				// Use the build-in editor
				ShowTodoDialog(hwndOwner, instance, guid);
			}
		}
		else
		{
			DebugLog("OpenTodoDialog(): Todo not found %s", CEntryItem::GetGUIDAsString(guid));
			return;
		}
	}
	else
	{
		// No guid
		plugin = pluginManager.GetDefaultPlugin();

		if (plugin && plugin->HasEditItem() && plugin->IsEnabled())
		{
			plugin->EditItem(hwndOwner, NULL, RAINLENDAR_TYPE_TODO);
			eventManager.FlushBufferedItems(true);
			GetRainlendar()->GetCalendarWindow().RedrawAll();
		}
		else
		{
			// Use the build-in editor
			ShowTodoDialog(hwndOwner, instance, NULL);
		}
	}
}

bool ShowTodoDialog(HWND hwndOwner, HINSTANCE instance, GUID* guid)
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
		return false;
	}
    
	PROPSHEETPAGE psp[3];
    PROPSHEETHEADER psh;

	int num = 0;

    psp[num].dwSize = sizeof(PROPSHEETPAGE);
    psp[num].dwFlags = PSP_USETITLE | PSP_PREMATURE;
    psp[num].hInstance = instance;
    psp[num].pszTemplate = MAKEINTRESOURCE(IDD_TODO);
    psp[num].pszIcon = NULL;
    psp[num].pfnDlgProc = TodoListPageProc;
    psp[num].pszTitle = CCalendarWindow::c_Language.GetString("Todo", 0);
    psp[num].lParam = 0;
    psp[num].pfnCallback = NULL;

	if (!CConfig::Instance().GetDisableDoneList())
	{
		num++;

		psp[num].dwSize = sizeof(PROPSHEETPAGE);
		psp[num].dwFlags = PSP_USETITLE | PSP_PREMATURE;
		psp[num].hInstance = instance;
		psp[num].pszTemplate = MAKEINTRESOURCE(IDD_DONE);
		psp[num].pszIcon = NULL;
		psp[num].pfnDlgProc = DoneListPageProc;
		psp[num].pszTitle = CCalendarWindow::c_Language.GetString("Todo", 1);
		psp[num].lParam = 0;
		psp[num].pfnCallback = NULL;
	}

	num++;

    psp[num].dwSize = sizeof(PROPSHEETPAGE);
    psp[num].dwFlags = PSP_USETITLE | PSP_PREMATURE;
    psp[num].hInstance = instance;
    psp[num].pszTemplate = MAKEINTRESOURCE(IDD_NOTES);
    psp[num].pszIcon = NULL;
    psp[num].pfnDlgProc = NotesListPageProc;
    psp[num].pszTitle = CCalendarWindow::c_Language.GetString("Todo", 2);
    psp[num].lParam = 0;
    psp[num].pfnCallback = NULL;

	num++;

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP ;
    psh.hwndParent = hwndOwner;
    psh.hInstance = instance;
    psh.pszIcon = NULL;
    psh.pszCaption = (LPSTR)CCalendarWindow::c_Language.GetString("Todo", 0);
    psh.nPages = num;
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback = NULL;

	CEventManager& manager = GetRainlendar()->GetCalendarWindow().GetEventManager();

	int index = 0;

	// Copy the lists
	int i;
	std::vector<CEntryTodo*>* vector = &(manager.GetTodos());
	for (i = 0; i < (int)vector->size(); i++)
	{
		if (!(*vector)[i]->IsReadOnly())
		{
			CEntryTodo* newTodo = new CEntryTodo((*vector)[i]);
			g_TodoList.push_back(newTodo);
		}
	}
	vector = &(manager.GetDones());
	for (i = 0; i < (int)vector->size(); i++)
	{
		CEntryTodo* newTodo = new CEntryTodo((*vector)[i]);
		g_DoneList.push_back(newTodo);
	}
	vector = &(manager.GetNotes());
	for (i = 0; i < (int)vector->size(); i++)
	{
		CEntryTodo* newTodo = new CEntryTodo((*vector)[i]);
		g_NotesList.push_back(newTodo);
	}

	// Sort by position
	std::sort(g_TodoList.begin(), g_TodoList.end(), TodoCompare);
	std::sort(g_DoneList.begin(), g_DoneList.end(), TodoCompare);
	std::sort(g_NotesList.begin(), g_NotesList.end(), TodoCompare);

	// Check active index
	g_ActiveTodoIndex = -1;
	if (guid)
	{
		for (i = 0; i < (int)g_TodoList.size(); i++)
		{
			if (memcmp(g_TodoList[i]->GetGUID(), guid, sizeof(GUID)) == 0)
			{
				g_ActiveTodoIndex = i;
				break;
			}
		}
	}

	g_Invalid = false;

	HBITMAP bitmap;
	g_ImageListEnabled = ImageList_Create(BUTTON_SIZE, BUTTON_SIZE, ILC_COLOR32, BUTTON_COUNT, BUTTON_COUNT);
	bitmap = LoadBitmap(instance, MAKEINTRESOURCE(IDB_BUTTONS));
	ImageList_Add(g_ImageListEnabled, bitmap, NULL);
	DeleteObject(bitmap);

	g_ImageListDisabled = ImageList_Create(BUTTON_SIZE, BUTTON_SIZE, ILC_COLOR32, BUTTON_COUNT, BUTTON_COUNT);
	bitmap = LoadBitmap(instance, MAKEINTRESOURCE(IDB_BUTTONS_DISABLED));
	ImageList_Add(g_ImageListDisabled, bitmap, NULL);
	DeleteObject(bitmap);

    UINT res = PropertySheet(&psh);

	// Delete the temp objects from the vectors
	for (i = 0; i < (int)g_TodoList.size(); i++)
	{
		if (res == IDOK)
		{
			g_TodoList[i]->SetTodoType(TODO_TYPE_TODO);		// Update the type since it could be wrong
			g_TodoList[i]->SetProfile("Todo");
			manager.AddItem(g_TodoList[i], true);
		}
		else
		{
			delete g_TodoList[i];
		}
	}
	g_TodoList.clear();
	for (i = 0; i < (int)g_DoneList.size(); i++)
	{
		if (res == IDOK)
		{
			g_DoneList[i]->SetTodoType(TODO_TYPE_DONE);		// Update the type since it could be wrong
			g_DoneList[i]->SetProfile("Done");
			manager.AddItem(g_DoneList[i], true);
		}
		else
		{
			delete g_DoneList[i];
		}
	}
	g_DoneList.clear();
	for (i = 0; i < (int)g_NotesList.size(); i++)
	{
		if (res == IDOK)
		{
			g_NotesList[i]->SetTodoType(TODO_TYPE_NOTE);		// Update the type since it could be wrong
			g_NotesList[i]->SetProfile("Note");
			manager.AddItem(g_NotesList[i], true);
		}
		else
		{
			delete g_NotesList[i];
		}
	}
	g_NotesList.clear();
	for (i = 0; i < (int)g_DeletedList.size(); i++)
	{
		if (res == IDOK)
		{
			manager.AddItem(g_DeletedList[i], true);
		}
		else
		{
			delete g_DeletedList[i];
		}
	}
	g_DeletedList.clear();

	manager.FlushBufferedItems(true);

	ImageList_Destroy(g_ImageListEnabled);
	ImageList_Destroy(g_ImageListDisabled);

	if (res == IDOK)
	{
		GetRainlendar()->GetCalendarWindow().GetTodoWindow().Redraw();
	}

	g_Dialog = NULL;

	return res == IDOK ? true : false;
}

BOOL MeasureItem(LPMEASUREITEMSTRUCT lpmis, std::vector<CEntryTodo*>& list, HWND widget)
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
		DrawText(hDC, list[lpmis->itemID]->GetMessage(), -1, &rc, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);
		SelectObject(hDC, oldFont);
		DeleteDC(hDC);

		lpmis->itemHeight = rc.bottom - rc.top + 3;
	}
    return TRUE;
}

BOOL DrawItem(LPDRAWITEMSTRUCT lpdis, std::vector<CEntryTodo*>& list)
{
	// If there are no list box items, skip this message. 
	if (lpdis->itemID != -1) 
	{ 
		SetBkMode(lpdis->hDC, TRANSPARENT);
		// Is the item selected? 
		if (lpdis->itemState & ODS_SELECTED) 
		{ 
			FillRect(lpdis->hDC, &lpdis->rcItem, GetSysColorBrush(COLOR_HIGHLIGHT));
			DrawText(lpdis->hDC, list[lpdis->itemID]->GetMessage(), -1, &lpdis->rcItem, DT_WORDBREAK | DT_NOPREFIX);
		} 
		else
		{
			FillRect(lpdis->hDC, &lpdis->rcItem, GetSysColorBrush(COLOR_WINDOW));
			DrawText(lpdis->hDC, list[lpdis->itemID]->GetMessage(), -1, &lpdis->rcItem, DT_WORDBREAK | DT_NOPREFIX);
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


void CreateToolBar(HWND hwndParent, bool removeButton, bool undoButton) 
{ 
	RECT rc;
	GetClientRect(hwndParent, &rc);
	int x = rc.right - 5;
	int y = 5;
    HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwndParent, GWL_HINSTANCE);
	HWND button;

	// Add tooltips
	HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
                            WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            hwndParent, NULL, hInstance,
                            NULL);

	SetWindowPos(hwndTip, HWND_TOPMOST,0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	if (undoButton)
	{
		button = CreateWindowEx(0, "BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, x - BUTTON_SIZE * 4, y, BUTTON_SIZE, BUTTON_SIZE, hwndParent, (HMENU)(IDC_TODO_UNDO), hInstance, NULL);
		AddTip(hwndParent, hwndTip, button, CCalendarWindow::c_Language.GetString("Todo", 6));
	}
	
	if (removeButton)
	{
		button = CreateWindowEx(0, "BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, x - BUTTON_SIZE * 3, y, BUTTON_SIZE, BUTTON_SIZE, hwndParent, (HMENU)(IDC_TODO_REMOVE), hInstance, NULL);
		AddTip(hwndParent, hwndTip, button, CCalendarWindow::c_Language.GetString("Todo", 5));
	}
	else
	{
		button = CreateWindowEx(0, "BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, x - BUTTON_SIZE * 3, y, BUTTON_SIZE, BUTTON_SIZE, hwndParent, (HMENU)(IDC_TODO_DONE), hInstance, NULL);
		AddTip(hwndParent, hwndTip, button, CCalendarWindow::c_Language.GetString("Todo", 1));
	}
	button = CreateWindowEx(0, "BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, x - BUTTON_SIZE * 2, y, BUTTON_SIZE, BUTTON_SIZE, hwndParent, (HMENU)(IDC_TODO_UP), hInstance, NULL);
	AddTip(hwndParent, hwndTip, button, CCalendarWindow::c_Language.GetString("Todo", 9));
	button = CreateWindowEx(0, "BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, x - BUTTON_SIZE * 1, y, BUTTON_SIZE, BUTTON_SIZE, hwndParent, (HMENU)(IDC_TODO_DOWN), hInstance, NULL);
	AddTip(hwndParent, hwndTip, button, CCalendarWindow::c_Language.GetString("Todo", 10));
} 

void SetToolBarState(HWND hwndParent, HWND list)
{
	BOOL down, up, ok, remove, undo;

	int count = SendMessage(list, LB_GETSELCOUNT, NULL, NULL);
	int index = SendMessage(list, LB_GETCURSEL, NULL, NULL);
	if (count > 0)
	{
		if (index > 0 && count == 1)
		{
			up = TRUE;
		}
		else
		{
			up = FALSE;
		}

	    if ((index + 1) == SendMessage(list, LB_GETCOUNT, NULL, NULL) || count != 1)
		{
			down = FALSE;
		}
		else
		{
			down = TRUE;
		}

		ok = remove = undo = TRUE;
	}
	else
	{
		// Disable all
		down = up = ok = remove = undo = FALSE;
	}

	HWND btn;
	btn = GetDlgItem(hwndParent, IDC_TODO_UNDO);
	if (btn) EnableWindow(btn, undo);

	btn = GetDlgItem(hwndParent, IDC_TODO_DOWN);
	if (btn) EnableWindow(btn, down);

	btn = GetDlgItem(hwndParent, IDC_TODO_UP);
	if (btn) EnableWindow(btn, up);
	
	btn = GetDlgItem(hwndParent, IDC_TODO_DONE);
	if (btn) EnableWindow(btn, ok);
	
	btn = GetDlgItem(hwndParent, IDC_TODO_REMOVE);
	if (btn) EnableWindow(btn, remove);
}

bool DrawToolBar(LPDRAWITEMSTRUCT dis, UINT id)
{
	int x = 0, y = 0;

	HIMAGELIST iList = g_ImageListEnabled;
	if (dis->itemState & ODS_DISABLED)
	{
		iList = g_ImageListDisabled;
	}

	if (dis->itemState & ODS_SELECTED)
	{
		x = y = 1;
	}

	switch(id)
	{
	case IDC_TODO_DONE:
		ImageList_Draw(iList, 0, dis->hDC, x, y, ILD_NORMAL);
		break;

	case IDC_TODO_REMOVE:
		ImageList_Draw(iList, 1, dis->hDC, x, y, ILD_NORMAL);
		break;

	case IDC_TODO_UP:
		ImageList_Draw(iList, 2, dis->hDC, x, y, ILD_NORMAL);
		break;

	case IDC_TODO_DOWN:
		ImageList_Draw(iList, 3, dis->hDC, x, y, ILD_NORMAL);
		break;

	case IDC_TODO_UNDO:
		ImageList_Draw(iList, 4, dis->hDC, x, y, ILD_NORMAL);
		break;
	}
	
	return TRUE;
}

BOOL UpdateTodoListWidgets(HWND hwndDlg, bool refreshList, std::vector<CEntryTodo*>& list)
{
	HWND widget = GetDlgItem(hwndDlg, IDC_TODO_LIST);

	if (refreshList)
	{
		SendMessage(widget, LB_RESETCONTENT, NULL, NULL);

		for (UINT i = 0; i < list.size(); i++)
		{
			if (list[i]->GetPosition() != i)
			{
				// The position has been changed -> Mark the todo item changed.
				list[i]->SetPosition(i);
				list[i]->UpdateTimeStamp();
			}
			SendMessage(widget, LB_ADDSTRING, NULL, (LPARAM)list[i]->GetMessage());
		}
	}

	HWND button = GetDlgItem(hwndDlg, IDC_TODO_NEW);
	HWND edit = GetDlgItem(hwndDlg, IDC_TODO_EDIT);

	if (button && edit)
	{
		int count = SendMessage(widget, LB_GETSELCOUNT, NULL, NULL);
		if (count > 1)
		{
			EnableWindow(button, FALSE);
			EnableWindow(edit, FALSE);
			SetWindowText(edit, "");
		}
		else
		{
			EnableWindow(button, TRUE);
			EnableWindow(edit, TRUE);

			int index = SendMessage(widget, LB_GETCURSEL, NULL, NULL);
			if (index != LB_ERR && index < (int)list.size() && count == 1)
			{
				SetWindowText(button, CCalendarWindow::c_Language.GetString("Todo", 4));	// Modify
				SetWindowText(edit, list[index]->GetMessage());
			}
			else
			{
				SetWindowText(button, CCalendarWindow::c_Language.GetString("Todo", 3));	// New
				SetWindowText(edit, "");
			}

			SetFocus(edit);
			SendMessage(edit, EM_SETSEL, 0, -1);
		}
	}

	SetToolBarState(hwndDlg, widget);

	return TRUE;
}

BOOL OnCommand(HWND hwndDlg, WPARAM wParam, LPARAM lParam, std::vector<CEntryTodo*>& list)
{
	switch (LOWORD(wParam))
	{
	case IDC_TODO_LIST:
		if (HIWORD(wParam) == LBN_SELCHANGE)
		{
			UpdateTodoListWidgets(hwndDlg, false, list);
		}
		break;

	case IDC_TODO_NEW:
		{
			HWND widget = GetDlgItem(hwndDlg, IDC_TODO_LIST);
			HWND edit = GetDlgItem(hwndDlg, IDC_TODO_EDIT);
			int len = GetWindowTextLength(edit);
			char* buffer = new char[len + 1];
			GetWindowText(edit, buffer, len + 1);
			int count = SendMessage(widget, LB_GETSELCOUNT, NULL, NULL);
			int index = SendMessage(widget, LB_GETCURSEL, NULL, NULL);
			if (count == 1 && index != LB_ERR)
			{
				if (strlen(buffer) > 0)
				{
					// Modify old item
					list[index]->SetMessage(buffer);
					list[index]->UpdateTimeStamp();
				}
				else
				{
					if (MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("Todo", 7), "Rainlendar", MB_YESNO | MB_ICONQUESTION) == IDYES)
					{
						CEntryTodo* item = list[index];
						item->MarkAsDeleted();
						item->UpdateTimeStamp();
						g_DeletedList.push_back(item);

						list.erase(list.begin() + index);
					}
				}
			}
			else
			{
				// Add new item
				if (strlen(buffer) > 0)
				{
					CEntryTodo* newTodo = new CEntryTodo;
					newTodo->SetMessage(buffer);
					list.push_back(newTodo);
				}
			}

			UpdateTodoListWidgets(hwndDlg, true, list);
		}
		return TRUE;

	case IDC_TODO_DOWN:
	case IDC_TODO_UP:
		{
			HWND widget = GetDlgItem(hwndDlg, IDC_TODO_LIST);
			int index = SendMessage(widget, LB_GETCURSEL, NULL, NULL);
			if (index != LB_ERR)
			{
				CEntryTodo* tmpTodo = list[index];
				list.erase(list.begin() + index);

				SendMessage(widget, LB_SETSEL, FALSE, index);
				if (LOWORD(wParam) == IDC_TODO_UP)
				{
					list.insert(list.begin() + index - 1, tmpTodo);
					SendMessage(widget, LB_SETSEL, TRUE, index - 1);
				}
				else
				{
					list.insert(list.begin() + index + 1, tmpTodo);
					SendMessage(widget, LB_SETSEL, TRUE, index + 1);
				}
				g_Invalid = true;
			}
			UpdateTodoListWidgets(hwndDlg, false, list);
		}
		return TRUE;

	case IDC_TODO_REMOVE:		// NOTES and DONE
		{
			HWND widget = GetDlgItem(hwndDlg, IDC_TODO_LIST);
			int count = SendMessage(widget, LB_GETSELCOUNT, NULL, NULL);

			if (count > 0)
			{
				int* indices = new int[list.size()];
				int count = SendMessage(widget, LB_GETSELITEMS, list.size(), (LPARAM)indices);

				if (count == 1)
				{
					if (MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("Todo", 7), "Rainlendar", MB_YESNO | MB_ICONQUESTION) == IDYES)
					{
						CEntryTodo* item = list[indices[0]];
						item->MarkAsDeleted();
						item->UpdateTimeStamp();
						g_DeletedList.push_back(item);

						list.erase(list.begin() + indices[0]);

						UpdateTodoListWidgets(hwndDlg, true, list);
					}
				}
				else
				{
					if (MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("Todo", 8), "Rainlendar", MB_YESNO | MB_ICONQUESTION) == IDYES)
					{
						// Erase selected from the list
						for (int j = list.size() - 1; j >= 0; j--)
						{
							for (int i = 0; i < count; i++)
							{
								if (j == indices[i])
								{
									CEntryTodo* item = list[j];
									item->MarkAsDeleted();
									item->UpdateTimeStamp();
									g_DeletedList.push_back(item);

									list.erase(list.begin() + j);
									break;
								}
							}
						}
						UpdateTodoListWidgets(hwndDlg, true, list);
					}
				}
				delete [] indices;
			}
		}
		return TRUE;

	case IDC_TODO_UNDO:		// Only happens with DONE
	case IDC_TODO_DONE:		// Only happens with TODO
		{
			HWND widget = GetDlgItem(hwndDlg, IDC_TODO_LIST);
			int count = SendMessage(widget, LB_GETSELCOUNT, NULL, NULL);
			if (count > 0)
			{
				int* indices = new int[list.size()];
				count = SendMessage(widget, LB_GETSELITEMS, list.size(), (LPARAM)indices);

				for (int i = 0; i < count; i++)
				{
					int index = indices[i];
					if (LOWORD(wParam) == IDC_TODO_UNDO)
					{
						g_TodoList.push_back(list[index]);	// <-- Note the use of g_TodoList
					}
					else
					{
						g_DoneList.push_back(list[index]);	// <-- Note the use of g_DoneList
					}
					list[index]->UpdateTimeStamp();
					g_Invalid = true;
				}

				// Erase selected from the other list
				for (int j = list.size() - 1; j >= 0; j--)
				{
					for (int i = 0; i < count; i++)
					{
						if (j == indices[i])
						{
							list.erase(list.begin() + j);
							break;
						}
					}
				}
				
				delete [] indices;
				UpdateTodoListWidgets(hwndDlg, true, list);
			}
		}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
// TODO
//
//////////////////////////////////////////////////////////////////////////

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
	
	UpdateTodoListWidgets(hwndDlg, true, g_TodoList);

	if (g_ActiveTodoIndex != -1)
	{
		HWND widget = GetDlgItem(hwndDlg, IDC_TODO_LIST);
		SendMessage(widget, LB_SETSEL, TRUE, g_ActiveTodoIndex);
	}

	// Create toolbar
	if (CConfig::Instance().GetDisableDoneList())
	{
		CreateToolBar(hwndDlg, true, false);
	}
	else
	{
		CreateToolBar(hwndDlg, false, false);
	}

	UpdateTodoListWidgets(hwndDlg, false, g_TodoList);

	PostMessage(hwndDlg, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(hwndDlg, IDC_TODO_EDIT), TRUE);

	return FALSE;
}

BOOL CALLBACK TodoListPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
	RECT rc;
	HWND widget;

    switch (message) 
    { 
        case WM_INITDIALOG:
			g_Dialog = GetParent(hwndDlg);
			return OnInitTodoListDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_SETACTIVE:
				UpdateTodoListWidgets(hwndDlg, g_Invalid, g_TodoList);
				g_Invalid = false;
				return FALSE;

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
			if (wParam == IDC_TODO_LIST)
			{
				widget = GetDlgItem(hwndDlg, IDC_TODO_LIST);
				return MeasureItem((LPMEASUREITEMSTRUCT)lParam, g_TodoList, widget);
			}
			return true;

        case WM_DRAWITEM:
			if (wParam == IDC_TODO_LIST)
			{
				return DrawItem((LPDRAWITEMSTRUCT)lParam, g_TodoList);
			}
			else
			{
				return DrawToolBar((LPDRAWITEMSTRUCT)lParam, wParam);
			}

		case WM_COMMAND:
			return OnCommand(hwndDlg, wParam, lParam, g_TodoList);
	}
    return FALSE; 
} 

//////////////////////////////////////////////////////////////////////////
//
// DONE
//
//////////////////////////////////////////////////////////////////////////

BOOL OnInitDoneListDialog(HWND hwndDlg)
{
	// Create toolbar
	CreateToolBar(hwndDlg, true, true);

	UpdateTodoListWidgets(hwndDlg, true, g_DoneList);
	return TRUE;
}

BOOL CALLBACK DoneListPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
	HWND widget;

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
					UpdateTodoListWidgets(hwndDlg, true, g_DoneList);
					g_Invalid = false;
				}
				return TRUE;
			}
			break;

		case WM_MEASUREITEM:
			if (wParam == IDC_TODO_LIST)
			{
				widget = GetDlgItem(hwndDlg, IDC_TODO_LIST);
				return MeasureItem((LPMEASUREITEMSTRUCT)lParam, g_DoneList, widget);
			}
			return true;

        case WM_DRAWITEM:
			if (wParam == IDC_TODO_LIST)
			{
				return DrawItem((LPDRAWITEMSTRUCT)lParam, g_DoneList);
			}
			else
			{
				return DrawToolBar((LPDRAWITEMSTRUCT)lParam, wParam);
			}

		case WM_COMMAND:
			return OnCommand(hwndDlg, wParam, lParam, g_DoneList);
	}
    return FALSE; 
}

//////////////////////////////////////////////////////////////////////////
//
// NOTES
//
//////////////////////////////////////////////////////////////////////////

BOOL OnInitNotesListDialog(HWND hwndDlg)
{
	// Create toolbar
	CreateToolBar(hwndDlg, true, false);

	UpdateTodoListWidgets(hwndDlg, true, g_NotesList);
	
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
				SendMessage(GetDlgItem(hwndDlg, IDC_TODO_LIST), LB_SETCURSEL, -1, NULL);
				return TRUE;

			case PSN_SETACTIVE:
				UpdateTodoListWidgets(hwndDlg, g_Invalid, g_NotesList);
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

		case WM_MEASUREITEM:
			if (wParam == IDC_TODO_LIST)
			{
				widget = GetDlgItem(hwndDlg, IDC_TODO_LIST);
				return MeasureItem((LPMEASUREITEMSTRUCT)lParam, g_NotesList, widget);
			}
			return true;

        case WM_DRAWITEM:
			if (wParam == IDC_TODO_LIST)
			{
				return DrawItem((LPDRAWITEMSTRUCT)lParam, g_NotesList);
			}
			else
			{
				return DrawToolBar((LPDRAWITEMSTRUCT)lParam, wParam);
			}

		case WM_COMMAND:
			return OnCommand(hwndDlg, wParam, lParam, g_NotesList);
	}
    return FALSE; 
} 
