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
  $Header: /home/cvsroot/Rainlendar/Plugin/AllEventsDialog.cpp,v 1.7 2004/04/24 11:16:25 rainy Exp $

  $Log: AllEventsDialog.cpp,v $
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

#include "AllEventsDialog.h"
#include "CalendarWindow.h"
#include "RainlendarDLL.h"
#include <Commdlg.h>
#include <time.h>
#include "EditEvent.h"

const CEventMessage* GetEvent(int index)
{
	const std::list<CEventMessage>& allEvents = GetRainlendar()->GetCalendarWindow().GetEventManager()->GetAllEvents();

	if (index >= 0 && index < allEvents.size())
	{
		std::list<CEventMessage>::const_iterator iter = allEvents.begin();
		for (int i = 0; i < index; i++)
		{
			iter++;
		}
		return &(*iter);
	}
	return NULL;
}

int CALLBACK CompareCallback(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	const CEventMessage* src = GetEvent(lParam1);
	const CEventMessage* dest = GetEvent(lParam2);

	if (src && dest)
	{
		if (src->GetDate() < dest->GetDate()) return -1;
		if (src->GetDate() > dest->GetDate()) return 1;
	}
	return 0;
}

void UpdateWidgets(HWND window)
{
	HWND widget = GetDlgItem(window, IDC_ALLEVENTS_LIST);

	ListView_DeleteAllItems(widget);

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
		lvI.lParam = (LPARAM)i;
		ListView_InsertItem(widget, &lvI);
		
		ListView_SetItemText(widget, i, 1, (char*)(*iter).GetTypeText());

		const char* profile = (*iter).GetProfile().c_str();
		ListView_SetItemText(widget, i, 2, (char*)CCalendarWindow::c_Language.GetTranslatedProfile(profile));

		// Remove newlines from the string
		std::string message = (*iter).GetMessage();
		int pos = message.find("\r\n");
		while (pos != -1)
		{
			message.replace(message.begin() + pos, message.begin() + pos + 2, " ");
			pos = message.find("\r\n");
		}
		ListView_SetItemText(widget, i, 3, (char*)message.c_str());

		i++;
	}
	ListView_SetExtendedListViewStyle(widget, LVS_EX_FULLROWSELECT);
	ListView_SortItems(widget, CompareCallback, (LPARAM)0);
}

BOOL OnInitAllEventsDialog(HWND window) 
{
	SetWindowText(window, CCalendarWindow::c_Language.GetString("AllEventsGUI", 4));
	
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
		
		UpdateWidgets(window);

		// Activate today (or next event from today)
		int today = CEventMessage::DateToValue(CCalendarWindow::c_TodaysDate.wDay, CCalendarWindow::c_TodaysDate.wMonth, CCalendarWindow::c_TodaysDate.wYear);
		int current = 0;
		int count = ListView_GetItemCount(widget);
		for (int i = 0; i < count; i++)
		{
			LVITEM item;
			item.iItem = i;
			item.iSubItem = 0;
			item.mask = LVIF_PARAM;
			ListView_GetItem(widget, &item);

			const CEventMessage* event = GetEvent(item.lParam);
			if (event && event->GetDate() >= today && current == 0)
			{
				current = i;
			}
		}
		ListView_SetItemState(widget, current, LVIS_SELECTED, LVIS_SELECTED);
		ListView_EnsureVisible(widget, current, FALSE);
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
							// "Are you sure you want to delete the event(s)?",
							if (IDYES == MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("Message", 0), "Rainlendar", MB_YESNO | MB_ICONQUESTION))
							{
								std::vector<int> indices;
								bool permanent = false;

								while (index != -1)
								{
									LVITEM item;
									item.iItem = index;
									item.iSubItem = 0;
									item.mask = LVIF_PARAM;
									ListView_GetItem(widget, &item);

									const CEventMessage* event = GetEvent(item.lParam);

									if (event && event->IsPermanent())
									{
										indices.push_back(index);
										GetRainlendar()->GetCalendarWindow().GetEventManager()->RemoveEvent(*event);
										GetRainlendar()->GetCalendarWindow().GetEventManager()->WriteEvent(*event);
									}
									else
									{
										permanent = true;
									}
									index = ListView_GetNextItem(widget, index, LVNI_SELECTED);
								}

								for (int i = indices.size() - 1; i >= 0; i--)
								{
									ListView_DeleteItem(widget, indices[i]);
									changed = true;
								}

								if (permanent) 
								{
									//	"The event is read-only and cannot be deleted!",
									MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("Message", 1), "Rainlendar", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
					}
				}
				else if (pNMHDR->code == NM_DBLCLK)
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

						const CEventMessage* event = GetEvent(item.lParam);

						if (event)
						{
							LSLog(LOG_DEBUG, "Rainlendar", "Opening Edit Event dialog.");
							OpenEditEventDialog(hwndDlg, CRainlendar::GetInstance(), event->GetDate(), event->GetID());
							changed = true;
							LSLog(LOG_DEBUG, "Rainlendar", "Edit Event dialog closed.");
						}

						UpdateWidgets(hwndDlg);
					}
    			}
				else if (pNMHDR->code == NM_CUSTOMDRAW) 
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
							const CEventMessage* event = GetEvent(lpNMCustomDraw->nmcd.lItemlParam);
							if (event && !event->IsPermanent()) 
							{
								lpNMCustomDraw->clrText = RGB(128, 128, 128);	// Change non permanet to gray
							}
						}
						SetWindowLong(hwndDlg, DWL_MSGRESULT, CDRF_DODEFAULT);
						return TRUE;
					}
					SetWindowLong(hwndDlg, DWL_MSGRESULT, CDRF_DODEFAULT);
					return TRUE;
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
                case IDOK:
					EndDialog(hwndDlg, changed ? IDOK : IDCANCEL); 
                    return TRUE; 
            }
            break;
    } 
    return FALSE; 
} 

