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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/AllEventsDialog.cpp,v 1.2 2003/06/15 09:39:35 Rainy Exp $

  $Log: AllEventsDialog.cpp,v $
  Revision 1.2  2003/06/15 09:39:35  Rainy
  Strings are read from CLanguage class.
  The today's event is selected automatically.

  Revision 1.1  2003/05/26 18:44:26  Rainy
  Initial version.

*/

#pragma warning(disable: 4786)

#include "AllEventsDialog.h"
#include "CalendarWindow.h"
#include "RainlendarDLL.h"
#include <Commdlg.h>
#include <time.h>

int CALLBACK CompareCallback(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	if(((CEventMessage*)lParam1)->GetDate() < ((CEventMessage*)lParam2)->GetDate()) return -1;
	if(((CEventMessage*)lParam1)->GetDate() > ((CEventMessage*)lParam2)->GetDate()) return 1;
	return 0;
}

BOOL OnInitAllEventsDialog(HWND window) 
{
	HWND widget = GetDlgItem(window, IDC_ALLEVENTS_LIST);
	
	if(widget ) 
	{
		// Add Columns
		LVCOLUMN lvc; 
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
		lvc.iSubItem = 0;
		lvc.pszText = (char*)CCalendarWindow::c_Language.GetString("AllEventsGUI", 0);
		lvc.cx = 80;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 0, &lvc);

		lvc.iSubItem = 1;
		lvc.pszText = (char*)CCalendarWindow::c_Language.GetString("AllEventsGUI", 1);
		lvc.cx = 50;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 1, &lvc);
		
		lvc.iSubItem = 2;
		lvc.pszText = (char*)CCalendarWindow::c_Language.GetString("AllEventsGUI", 2);
		lvc.cx = 60;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 2, &lvc);
		
		lvc.iSubItem = 3;
		lvc.pszText = (char*)CCalendarWindow::c_Language.GetString("AllEventsGUI", 3);
		lvc.cx = 285;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 3, &lvc);
		
		// Add Items
		LVITEM lvI; 
		
		lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
		lvI.state = 0; 
		lvI.stateMask = 0; 
		
		const std::list<CEventMessage>& allEvents = GetRainlendar()->GetCalendarWindow().GetEventManager()->GetAllEvents();

		std::list<CEventMessage>::const_iterator iter = allEvents.begin();
		int i = 0;
		for (; iter != allEvents.end(); iter++)
		{
			if ((*iter).IsDeleted()) continue;

			lvI.iItem = i;
			lvI.iSubItem = 0;
			lvI.pszText = (char*)(*iter).GetDateText();
			lvI.lParam = (LPARAM)&(*iter);
			ListView_InsertItem(widget, &lvI);
			
			ListView_SetItemText(widget, i, 1, (char*)(*iter).GetTypeText());
			ListView_SetItemText(widget, i, 2, (char*)(*iter).GetProfile().c_str());
			ListView_SetItemText(widget, i, 3, (char*)(*iter).GetMessage().c_str());
			i++;
		}
		ListView_SetExtendedListViewStyle(widget, LVS_EX_FULLROWSELECT);
		ListView_SortItems(widget, CompareCallback, (LPARAM)0);

		// Activate today (or next event from today)
		int today = CEventMessage::DateToValue(CCalendarWindow::c_TodaysDate.wDay, CCalendarWindow::c_TodaysDate.wMonth, CCalendarWindow::c_TodaysDate.wYear);
		int current = 0;
		int count = ListView_GetItemCount(widget);
		for (i = 0; i < count; i++)
		{
			LVITEM item;
			item.iItem = i;
			item.iSubItem = 0;
			item.mask = LVIF_PARAM;
			ListView_GetItem(widget, &item);
			if (current == 0 && ((CEventMessage*)item.lParam)->GetDate() >= today)
			{
				current = i;
			}
		}
		ListView_SetItemState(widget, current, LVIS_SELECTED, LVIS_SELECTED);
		ListView_EnsureVisible(widget, current, FALSE);
	}

	widget = GetDlgItem(window, IDOK);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("General", 0));

	SetWindowPos(window, NULL, 0, 0, 500, 400, SWP_NOMOVE | SWP_NOZORDER);
    return TRUE;
}

BOOL CALLBACK AllEventsProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
	static bool changed = false;

    switch (message) 
    { 
        case WM_WINDOWPOSCHANGING:
			{
				LPWINDOWPOS winPos = (LPWINDOWPOS)lParam;
				winPos->cx = max(winPos->cx, 200);
				winPos->cy = max(winPos->cy, 200);
			}
			break;

        case WM_SIZE:
			{
				int w = LOWORD(lParam);
				int h = HIWORD(lParam);
				HWND widget; 

				widget = GetDlgItem(hwndDlg, IDC_ALLEVENTS_LIST);
				SetWindowPos(widget, NULL, 5, 5, w - 10, h - 45, SWP_NOZORDER);
				widget = GetDlgItem(hwndDlg, IDOK);
				SetWindowPos(widget, NULL, w / 2 - 36, h - 30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}
			break;

        case WM_INITDIALOG:
			changed = false;
			return OnInitAllEventsDialog(hwndDlg);
	
        case WM_NOTIFY:
			{
				LPNMHDR pNMHDR = (LPNMHDR)lParam;

				if(pNMHDR->code == LVN_KEYDOWN)
				{
					LPNMLVKEYDOWN pnkd = (LPNMLVKEYDOWN)lParam; 

					if (pnkd->wVKey == VK_DELETE)
					{
						HWND widget; 
						widget = GetDlgItem(hwndDlg, IDC_ALLEVENTS_LIST);
						int index = ListView_GetNextItem(widget, -1, LVNI_SELECTED);
						if (index != -1)
						{
							LVITEM item;
							item.iItem = index;
							item.iSubItem = 0;
							item.mask = LVIF_PARAM;
							ListView_GetItem(widget, &item);

							if (((CEventMessage*)item.lParam)->IsPermanent())
							{
								if (IDYES == MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("Message", 0), "Rainlendar", MB_YESNO))
								{
									GetRainlendar()->GetCalendarWindow().GetEventManager()->RemoveEvent(*((CEventMessage*)item.lParam));
									ListView_DeleteItem(widget, index);
									changed = true;
								}
							}
							else
							{
								MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("Message", 1), "Rainlendar", MB_OK);
							}
						}
					}
				}
			}
			return FALSE;

        case WM_CLOSE: 
            EndDialog(hwndDlg, changed ? IDOK : IDCANCEL); 
            return TRUE; 
			
        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            {
                case IDOK:
		            EndDialog(hwndDlg, changed ? IDOK : IDCANCEL); 
                    return TRUE; 
            }
            break;
    } 
    return FALSE; 
} 

