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
  $Header: /home/cvsroot/Rainlendar/Plugin/AllEventsDialog.cpp,v 1.9 2004/12/05 18:35:21 rainy Exp $

  $Log: AllEventsDialog.cpp,v $
  Revision 1.9  2004/12/05 18:35:21  rainy
  All plugins can import stuff

  Revision 1.8  2004/11/06 13:38:58  rainy
  no message

  Revision 1.7  2004/04/24 11:16:25  rainy
  Fixed a crash bug.

  Revision 1.6  2004/01/25 09:58:04  rainy
  Fixed dialog position remembering.

  Revision 1.5  2003/12/05 15:45:28  Rainy
  Added multiselect.

  Revision 1.4  2003/10/27 17:35:31  Rainy
  Config is now singleton.

  Revision 1.3  2003/08/09 16:39:27  Rainy
  Added support for "Remember dialog position"-feature.
  Event can be edited by doubclicking it.

  Revision 1.2  2003/06/15 09:39:35  Rainy
  Strings are read from CLanguage class.
  The today's event is selected automatically.

  Revision 1.1  2003/05/26 18:44:26  Rainy
  Initial version.

*/

#pragma warning(disable: 4786)
#pragma warning(disable: 4996)

#include "AllEventsDialog.h"
#include "CalendarWindow.h"
#include "RainlendarDLL.h"
#include <Commdlg.h>
#include <time.h>
#include "EditEvent.h"

static std::vector<GUID> g_AllGuids;
static bool g_SortOrder = true;
static HWND g_Dialog = NULL;

BOOL CALLBACK AllEventsProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

bool OpenAllEventsDialog(HWND parent)
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
	
	DWORD ret = DialogBox(CRainlendar::GetInstance(), MAKEINTRESOURCE(IDD_ALLEVENTS), parent, (DLGPROC)AllEventsProc);
	g_Dialog = NULL;

	return ret == IDOK;
}

static CEntryEvent* GetEvent(int index)
{
	if (index >= 0 && index < (int)g_AllGuids.size())
	{
		return GetRainlendar()->GetCalendarWindow().GetEventManager().GetEvent(&g_AllGuids[index]);
	}
	return NULL;
}

static int CALLBACK CompareCallback(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CEntryEvent* src = GetEvent(lParam1);
	CEntryEvent* dest = GetEvent(lParam2);

	if (src && dest)
	{
		if (!g_SortOrder)
		{
			std::swap<CEntryEvent*>(src, dest);
		}

		switch (lParamSort)
		{
			case 0:	// start date
				return (src->GetBegin().Compare(dest->GetBegin()));

			case 1:	// end date
				return (src->GetEnd().Compare(dest->GetEnd()));

			case 2:	// profile
				{
					const char* profile1 = src->GetProfile();
					const char* profile2 = dest->GetProfile();
					return strcmp(CCalendarWindow::c_Language.GetTranslatedProfile(profile1), CCalendarWindow::c_Language.GetTranslatedProfile(profile2));
				}

			case 3:	// text
				{
					std::string brief1, brief2;
					src->GetBriefMessage(brief1, 256, false, false);
					dest->GetBriefMessage(brief2, 256, false, false);
					return strcmp(brief1.c_str(), brief2.c_str());
				}
		}
	}

	return 0;
}

static void UpdateWidgets(HWND window, GUID* activeGuid)
{
	int active = -1;

	HWND widget = GetDlgItem(window, IDC_ALLEVENTS_LIST);

	ListView_DeleteAllItems(widget);

	// Add Items
	LVITEM lvI; 
	
	lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
	lvI.state = 0; 
	lvI.stateMask = 0; 

	const std::map<GUID, CEntryEvent*, GUIDComparator>& allEvents = GetRainlendar()->GetCalendarWindow().GetEventManager().GetAllEvents();

	g_AllGuids.clear();

	std::map<GUID, CEntryEvent*, GUIDComparator>::const_iterator iter = allEvents.begin();

	int i = 0, index = 0;
	for (; iter != allEvents.end(); iter++)
	{
		CEntryEvent* event = (*iter).second;

		if (event->IsDeleted()) continue;		// Deleted messages are skipped

		if (activeGuid && IsEqualGUID(*activeGuid, *event->GetGUID()))
		{
			active = index;
		}

		CFileTime begin = event->GetBegin();
		CFileTime end = event->GetEnd();
		LPCTSTR endString;

		if (event->IsAllDayEvent())
		{
			lvI.pszText = (char*)begin.ToDate();
		}
		else
		{
			lvI.pszText = (char*)begin.ToTimeAndDateString();
		}

		lvI.iItem = i;
		lvI.iSubItem = 0;
		lvI.lParam = (LPARAM)index++;
		ListView_InsertItem(widget, &lvI);

		g_AllGuids.push_back(*event->GetGUID());

		if (event->IsAllDayEvent())
		{
			endString = (char*)end.ToDate();
		}
		else
		{
			endString = (char*)end.ToTimeAndDateString();
		}

		ListView_SetItemText(widget, i, 1, (char*)endString);

		const char* profile = event->GetProfile();
		ListView_SetItemText(widget, i, 2, (char*)CCalendarWindow::c_Language.GetTranslatedProfile(profile));

		std::string message;
		event->GetBriefMessage(message, 256, false, false);
		ListView_SetItemText(widget, i, 3, (char*)message.c_str());

		i++;
	}
	ListView_SetExtendedListViewStyle(widget, LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	ListView_SortItems(widget, CompareCallback, (LPARAM)0);

	// Activate the 'active'
	index = 0;
	while (index != -1)
	{
		LVITEM item;
		item.iItem = index;
		item.iSubItem = 0;
		item.mask = LVIF_PARAM;
		ListView_GetItem(widget, &item);

		if (active == -1)
		{
			CEntryEvent* event = GetEvent(item.lParam);
			
			if (event && GetRainlendar()->GetCalendarWindow().c_TodaysDate.Compare(event->GetBegin()) < 0)
			{
				ListView_SetItemState(widget, index, LVIS_SELECTED, LVIS_SELECTED);
				ListView_EnsureVisible(widget, index, FALSE);
				break;
			}
		}
		else if (active == item.lParam)
		{
			ListView_SetItemState(widget, index, LVIS_SELECTED, LVIS_SELECTED);
			ListView_EnsureVisible(widget, index, FALSE);
			break;
		}
		index = ListView_GetNextItem(widget, index, LVNI_ALL);
	}
}

static BOOL OnInitAllEventsDialog(HWND window) 
{
	SetWindowText(window, CCalendarWindow::c_Language.GetString("AllEventsGUI", 4));

	SetDlgItemText(window, IDC_ALLEVENTS_DELETE, CCalendarWindow::c_Language.GetString("EventGUI", 1));		// Delete
	SetDlgItemText(window, IDC_ALLEVENTS_IMPORT, CCalendarWindow::c_Language.GetString("AllEventsGUI", 7));
	SetDlgItemText(window, IDC_ALLEVENTS_EXPORT, CCalendarWindow::c_Language.GetString("AllEventsGUI", 8));

	HWND widget = GetDlgItem(window, IDC_ALLEVENTS_LIST);
	if(widget) 
	{
		// Add Columns
		LVCOLUMN lvc; 
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
		lvc.iSubItem = 0;
		lvc.pszText = (char*)CCalendarWindow::c_Language.GetString("AllEventsGUI", 5);	// Start date
		lvc.cx = 100;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 0, &lvc);

		lvc.iSubItem = 1;
		lvc.pszText = (char*)CCalendarWindow::c_Language.GetString("AllEventsGUI", 6);	// End date
		lvc.cx = 100;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 1, &lvc);
		
		lvc.iSubItem = 2;
		lvc.pszText = (char*)CCalendarWindow::c_Language.GetString("AllEventsGUI", 2);
		lvc.cx = 60;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 2, &lvc);
		
		lvc.iSubItem = 3;
		lvc.pszText = (char*)CCalendarWindow::c_Language.GetString("AllEventsGUI", 3);
		lvc.cx = 255;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 3, &lvc);
		
		UpdateWidgets(window, NULL);
	}

	widget = GetDlgItem(window, IDOK);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("General", 0));

	if (CConfig::Instance().GetRememberDialogPositions())
	{
		POINT pos = CConfig::Instance().GetDialogPosition(CConfig::DIALOG_ALLEVENTS);
		POINT size = CConfig::Instance().GetDialogPosition(CConfig::DIALOG_ALLEVENTS_SIZE);
		SetWindowPos(window, NULL, pos.x, pos.y, size.x, size.y, SWP_NOZORDER);
	}
	else
	{
		SetWindowPos(window, NULL, 0, 0, 500, 400, SWP_NOMOVE | SWP_NOZORDER);
	}

	// Disable Import and Export if there is no plugins that support it
	CPluginManager& manager = GetRainlendar()->GetCalendarWindow().GetPluginManager();
	const std::vector<CPlugin*>& plugins = manager.GetAllPlugins();
	bool importButton = false;
	bool exportButton = false;

	for (int i = 0; i < plugins.size(); i++)
	{
		if (plugins[i]->HasImport())
		{
			importButton = true;
		}
		if (plugins[i]->HasExport())
		{
			exportButton = true;
		}
	}

	if (!importButton)
	{
		EnableWindow(GetDlgItem(window, IDC_ALLEVENTS_IMPORT), FALSE);
	}
	if (!exportButton)
	{
		EnableWindow(GetDlgItem(window, IDC_ALLEVENTS_EXPORT), FALSE);
	}

	BringWindowToTop(g_Dialog);
	SetForegroundWindow(g_Dialog);

    return TRUE;
}

static void ExportEvents(HWND window)
{
	int i;

	CPluginManager& manager = GetRainlendar()->GetCalendarWindow().GetPluginManager();
	const std::vector<CPlugin*>& plugins = manager.GetAllPlugins();

	TCHAR file[MAX_PATH];
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	file[0] = 0;

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = window;
	ofn.lpstrFile = file;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	// Create filter string
	std::string filter;
	std::vector<CPlugin*> exportPlugins;
	for (i = 0; i < (int)plugins.size(); i++)
	{
		if (plugins[i]->HasExport())
		{
			exportPlugins.push_back(plugins[i]);
			filter += plugins[i]->GetName();
			filter += "\n";
			filter += "*.*\n";
		}
	}
	filter += "\n";

	// Replace \n with \0
	for (i = 0; i < (int)filter.size(); i++)
	{
		if (filter[i] == '\n')
		{
			filter[i] = '\0';
		}
	}

	ofn.lpstrFilter = filter.c_str();

	HWND widget; 
	widget = GetDlgItem(window, IDC_ALLEVENTS_LIST);
	int index = ListView_GetNextItem(widget, -1, LVNI_SELECTED);

	GUID** guids = NULL;
	std::vector<GUID> exportedGuids;
	if (index != -1) 
	{
		// "Do you want to export only the selected events?",
		if (IDYES == MessageBox(window, CCalendarWindow::c_Language.GetString("AllEventsGUI", 11), "Rainlendar", MB_YESNO | MB_ICONQUESTION))
		{
			while (index != -1)
			{
				LVITEM item;
				item.iItem = index;
				item.iSubItem = 0;
				item.mask = LVIF_PARAM;
				ListView_GetItem(widget, &item);

				CEntryEvent* event = GetEvent(item.lParam);
				if (event)
				{
					exportedGuids.push_back(*(event->GetGUID()));
				}
				index = ListView_GetNextItem(widget, index, LVNI_SELECTED);
			}
		}
		else
		{
			// Export everything
			exportedGuids = g_AllGuids;
		}
	}
	else
	{
		// Export everything
		exportedGuids = g_AllGuids;
	}

	// Create GUID** table from the selected
	guids = new GUID*[exportedGuids.size() + 1];
	for (i = 0; i < (int)exportedGuids.size(); i++)
	{
		guids[i] = &exportedGuids[i];
	}
	guids[i] = NULL;

	if (exportPlugins.size() > 0)
	{
		if (GetSaveFileName(&ofn))
		{
			exportPlugins[ofn.nFilterIndex - 1]->ExportItems(file, guids, RAINLENDAR_TYPE_EVENT);
		}
	}
	else
	{
		// "None of the plugins is able to export events.",
		MessageBox(window, CCalendarWindow::c_Language.GetString("AllEventsGUI", 9), "Rainlendar", MB_OK | MB_ICONEXCLAMATION);
	}

	delete [] guids;
}

static void ImportEvents(HWND window)
{
	CPluginManager& manager = GetRainlendar()->GetCalendarWindow().GetPluginManager();
	const std::vector<CPlugin*>& plugins = manager.GetAllPlugins();

	TCHAR file[MAX_PATH];
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	file[0] = 0;

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = window;
	ofn.lpstrFile = file;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if (GetOpenFileName(&ofn))
	{
		// Try to load the file with all plugins until one can import it
		int i;
		for (i = 0; i < (int)plugins.size(); i++)
		{
			if (plugins[i]->ImportItems(file, RAINLENDAR_TYPE_EVENT))
			{
				// Found a plugin that could import the file
				GetRainlendar()->GetCalendarWindow().GetEventManager().FlushBufferedItems(true);
				GetRainlendar()->GetCalendarWindow().RedrawAll();
				UpdateWidgets(window, NULL);
				break;
			}
		}
		if (i == plugins.size())
		{
			// "Unable to import the file."
			MessageBox(window, CCalendarWindow::c_Language.GetString("AllEventsGUI", 9), "Rainlendar", MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

void DeleteSelectedEvents(HWND hwndDlg)
{
	HWND widget; 
	widget = GetDlgItem(hwndDlg, IDC_ALLEVENTS_LIST);
	int index = ListView_GetNextItem(widget, -1, LVNI_SELECTED);

	if (index != -1) 
	{
		// "Are you sure you want to delete the event(s)?",
		if (IDYES == MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("Message", 0), "Rainlendar", MB_YESNO | MB_ICONQUESTION))
		{
			std::vector<GUID> guids;
			bool permanent = false;

			while (index != -1 && !permanent) 
			{
				LVITEM item;
				item.iItem = index;
				item.iSubItem = 0;
				item.mask = LVIF_PARAM;
				ListView_GetItem(widget, &item);

				CEntryEvent* event = GetEvent(item.lParam);

				// Check for read-only events
				if (event && !event->IsReadOnly())
				{
					guids.push_back(*(event->GetGUID()));
				}
				else
				{
					permanent = true;
				}
				index = ListView_GetNextItem(widget, index, LVNI_SELECTED);
			}

			if (permanent) 
			{
				//	"The event is read-only and cannot be deleted!",
				MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("Message", 1), "Rainlendar", MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				// Delete the events
				for (int i = 0; i < (int)guids.size(); i++)
				{
					GetRainlendar()->GetCalendarWindow().GetEventManager().DeleteItem(&guids[i]);
				}

				UpdateWidgets(hwndDlg, NULL);
				GetRainlendar()->GetCalendarWindow().RedrawAll();
			}
		}
	}
}


BOOL CALLBACK AllEventsProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
	static bool changed = false;

    switch (message) 
    { 
        case WM_WINDOWPOSCHANGING:
			{
				LPWINDOWPOS winPos = (LPWINDOWPOS)lParam;
				winPos->cx = max(winPos->cx, 300);
				winPos->cy = max(winPos->cy, 200);
			}
			break;

        case WM_SIZE:
			{
				RECT r;
				int w = LOWORD(lParam);
				int h = HIWORD(lParam);
				HWND widget; 

				widget = GetDlgItem(hwndDlg, IDC_ALLEVENTS_LIST);
				SetWindowPos(widget, NULL, 5, 45, w - 10, h - 85, SWP_NOZORDER);
				widget = GetDlgItem(hwndDlg, IDOK);
				SetWindowPos(widget, NULL, w / 2 - 36, h - 30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				GetClientRect(GetDlgItem(hwndDlg, IDC_ALLEVENTS_DELETE), &r);
				widget = GetDlgItem(hwndDlg, IDC_ALLEVENTS_DELETE);
				SetWindowPos(widget, NULL, w - r.right - 11, 11, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}
			break;

        case WM_INITDIALOG:
			g_Dialog = hwndDlg;
			changed = false;
			return OnInitAllEventsDialog(hwndDlg);
	
        case WM_NOTIFY:
			{
				LPNMHDR pNMHDR = (LPNMHDR)lParam;

				switch (pNMHDR->code)
				{
					case LVN_COLUMNCLICK:
						{
							LPNMLISTVIEW lv = (LPNMLISTVIEW)lParam;
							g_SortOrder = !g_SortOrder;
							HWND widget = GetDlgItem(hwndDlg, IDC_ALLEVENTS_LIST);
							ListView_SortItems(widget, CompareCallback, (LPARAM)lv->iSubItem);
						}
						break;

					case LVN_ITEMCHANGED:
						{
							// If something is selected, enable delete
							HWND widget = GetDlgItem(hwndDlg, IDC_ALLEVENTS_LIST);
							int index = ListView_GetNextItem(widget, -1, LVNI_SELECTED);
							if (index != -1)
							{
								EnableWindow(GetDlgItem(hwndDlg, IDC_ALLEVENTS_DELETE), TRUE);
							}
							else
							{
								EnableWindow(GetDlgItem(hwndDlg, IDC_ALLEVENTS_DELETE), FALSE);
							}
						}
						break;

					case LVN_KEYDOWN:
						{
							LPNMLVKEYDOWN pnkd = (LPNMLVKEYDOWN)lParam; 

							if (pnkd->wVKey == VK_DELETE)
							{
								DeleteSelectedEvents(hwndDlg);
							}
						}
						break;

					case NM_DBLCLK:
						{
							HWND widget; 
							widget = GetDlgItem(hwndDlg, IDC_ALLEVENTS_LIST);
							int index = ListView_GetNextItem(widget, -1, LVNI_SELECTED | LVNI_FOCUSED);
							if (index != -1)
							{
								LVITEM item;
								item.iItem = index;
								item.iSubItem = 0;
								item.mask = LVIF_PARAM;
								ListView_GetItem(widget, &item);

								CEntryEvent* event = GetEvent(item.lParam);
								GUID guid = *event->GetGUID();

								if (event && !event->IsReadOnly())
								{
									OpenEditEventDialog(hwndDlg, CRainlendar::GetInstance(), event->GetGUID(), NULL);
								}
								else
								{
									DebugLog("The event is a read-only. It's not possible to edit it.");
								}

								UpdateWidgets(hwndDlg, &guid);
								GetRainlendar()->GetCalendarWindow().RedrawAll();
							}
    					}
						break;

					case NM_CUSTOMDRAW:
						{
							LPNMLVCUSTOMDRAW lpNMCustomDraw = (LPNMLVCUSTOMDRAW)lParam;

							if (lpNMCustomDraw->nmcd.dwDrawStage == CDDS_PREPAINT) 
							{
								SetWindowLong(hwndDlg, DWL_MSGRESULT, CDRF_NOTIFYITEMDRAW);
								return TRUE;
							}
							else if (lpNMCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) 
							{
								if (lpNMCustomDraw->iSubItem == 0) 
								{
									CEntryEvent* event = GetEvent(lpNMCustomDraw->nmcd.lItemlParam);
									if (event && event->IsReadOnly()) 
									{
										lpNMCustomDraw->clrText = RGB(128, 128, 128);	// Change read-only events to gray
									}
								}
								SetWindowLong(hwndDlg, DWL_MSGRESULT, CDRF_DODEFAULT);
								return TRUE;
							}
							SetWindowLong(hwndDlg, DWL_MSGRESULT, CDRF_DODEFAULT);
							return TRUE;
						}
				}
			}
			return FALSE;

		case WM_DESTROY:
			{
				RECT rc;
				GetWindowRect(hwndDlg, &rc);
				CConfig::Instance().SetDialogPosition(CConfig::DIALOG_ALLEVENTS, rc.left, rc.top);
				CConfig::Instance().SetDialogPosition(CConfig::DIALOG_ALLEVENTS_SIZE, rc.right - rc.left, rc.bottom - rc.top);
				CConfig::Instance().WriteConfig(CConfig::WRITE_DIALOG_POS);
			}
			break;

        case WM_CLOSE:
			EndDialog(hwndDlg, changed ? IDOK : IDCANCEL); 
            return TRUE; 
			
        case WM_COMMAND:
		switch (LOWORD(wParam)) 
            {
                case IDC_ALLEVENTS_DELETE:
					DeleteSelectedEvents(hwndDlg);
					break;

                case IDC_ALLEVENTS_IMPORT:
					ImportEvents(hwndDlg);
					break;

                case IDC_ALLEVENTS_EXPORT:
					ExportEvents(hwndDlg);
					break;

                case IDOK:
					EndDialog(hwndDlg, changed ? IDOK : IDCANCEL); 
                    return TRUE; 
            }
            break;
    } 
    return FALSE; 
} 

