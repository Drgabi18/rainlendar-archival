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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/EditEvent.cpp,v 1.6 2002/05/30 18:26:40 rainy Exp $

  $Log: EditEvent.cpp,v $
  Revision 1.6  2002/05/30 18:26:40  rainy
  Includes commdlg.h

  Revision 1.5  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.4  2002/01/27 16:11:14  rainy
  Changed CEvent to CEventMessage to avoid name clash

  Revision 1.3  2002/01/10 16:47:15  rainy
  Added support for bitmap/color,

  Revision 1.2  2001/12/23 10:01:46  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#pragma warning(disable: 4786)

#include "EditEvent.h"
#include "CalendarWindow.h"
#include "RainlendarDLL.h"
#include <Commdlg.h>
#include <time.h>

// This needs to be a global (even though globals are evil)
static COLORREF g_FontColor = 0;

/* 
** ReadEvent
**
** Reads the Event for selected day
**
*/
void ReadEvent(HWND window)
{
	char tmpSz[MAX_LINE_LENGTH];
	char Date[MAX_LINE_LENGTH];
	std::string bitmap;
	std::string message;

	const char* IniPath = CCalendarWindow::c_Config.GetEventsPath().c_str();

	int day = GetRainlendar()->GetCalendarWindow().GetSelectedDay();
	int month = CCalendarWindow::c_MonthsFirstDate.wMonth;
	int year = CCalendarWindow::c_MonthsFirstDate.wYear;
	sprintf(Date, "%i-%i-%i", day, month, year);

	if(GetPrivateProfileString( Date, "Message", "", tmpSz, 255, IniPath) > 0) 
	{
		message = tmpSz;
		int pos = message.find("\\n");
		while (pos != -1)
		{
			message.replace(message.begin() + pos, message.begin() + pos + 2, "\r\n");
			pos = message.find("\\n");
		}
	}

	if(GetPrivateProfileString( Date, "FontColor", "", tmpSz, 255, IniPath) > 0) 
	{
		sscanf(tmpSz, "%X", &g_FontColor);
	}
	else
	{
		g_FontColor = CCalendarWindow::c_Config.GetEventFontColor();
	}

	if(GetPrivateProfileString( Date, "Bitmap", "", tmpSz, 255, IniPath) > 0) 
	{
		bitmap = tmpSz;
	}
	else
	{
		bitmap = CCalendarWindow::c_Config.GetEventBitmapName();
	}

	// Update the widgets
	HWND widget = NULL;
	widget = GetDlgItem(window, IDC_EDITEVENT_BITMAP);
	SetWindowText(widget, bitmap.c_str());
	widget = GetDlgItem(window, IDC_EDITEVENT_MESSAGE);
	SetWindowText(widget, message.c_str());
} 

/* 
** WriteEvent
**
** Writes the Event for selected day
**
*/
void WriteEvent(HWND window)
{
	char tmpSz[MAX_LINE_LENGTH];
	char Date[MAX_LINE_LENGTH];
	std::string bitmap;
	std::string message;

	// Get the strings from the widgets
	HWND widget = NULL;
	widget = GetDlgItem(window, IDC_EDITEVENT_BITMAP);
	GetWindowText(widget, tmpSz, MAX_LINE_LENGTH);
	bitmap = tmpSz;
	widget = GetDlgItem(window, IDC_EDITEVENT_MESSAGE);
	GetWindowText(widget, tmpSz, MAX_LINE_LENGTH);
	message = tmpSz;

	const char* IniPath = CCalendarWindow::c_Config.GetEventsPath().c_str();

	int day = GetRainlendar()->GetCalendarWindow().GetSelectedDay();
	int month = CCalendarWindow::c_MonthsFirstDate.wMonth;
	int year = CCalendarWindow::c_MonthsFirstDate.wYear;
	sprintf(Date, "%i-%i-%i", day, month, year);

	int pos = message.find("\r\n");
	while (pos != -1)
	{
		message.replace(message.begin() + pos, message.begin() + pos + 2, "\\n");
		pos = message.find("\r\n");
	}

	WritePrivateProfileString( Date, "Message", message.c_str(), IniPath );

	if(CCalendarWindow::c_Config.GetEventFontColor() != g_FontColor)
	{
		sprintf(tmpSz, "%X", g_FontColor);
		WritePrivateProfileString( Date, "FontColor", tmpSz, IniPath );
	}
	else
	{
		WritePrivateProfileString( Date, "FontColor", "", IniPath );
	}

	if(CCalendarWindow::c_Config.GetEventBitmapName() != bitmap)
	{
		WritePrivateProfileString( Date, "Bitmap", bitmap.c_str(), IniPath );
	}
	else
	{
		WritePrivateProfileString( Date, "Bitmap", "", IniPath );
	}

	// Write timestamp to know when this was last modified
	time_t ltime;
	time(&ltime);
	sprintf(tmpSz, "%ld", ltime);
	WritePrivateProfileString( Date, "TimeStamp", tmpSz, IniPath );
} 

BOOL OnInitDialog(HWND window) 
{
	char caption[256];
	HWND widget = NULL;

	int day = GetRainlendar()->GetCalendarWindow().GetSelectedDay();
	const std::string month = CCalendarWindow::c_Config.GetMonthName(CCalendarWindow::c_MonthsFirstDate.wMonth - 1);
	int year = CCalendarWindow::c_MonthsFirstDate.wYear;

	sprintf(caption, "%i-%s-%i", day, month.c_str(), year);
	SetWindowText(window, caption);

	ReadEvent(window);

	// Hide unnecessary widgets
	if(CCalendarWindow::c_Config.GetEventRasterizer() == CRasterizer::TYPE_BITMAP)
	{
		widget = GetDlgItem(window, IDC_EDITEVENT_FONTCOLOR);
		ShowWindow(widget, SW_HIDE);
		widget = GetDlgItem(window, IDC_EDITEVENT_LABEL);
		SetWindowText(widget, "Filename:");
	}
	else	// FONT rasterizer
	{
		widget = GetDlgItem(window, IDC_EDITEVENT_BROWSE);
		ShowWindow(widget, SW_HIDE);
		widget = GetDlgItem(window, IDC_EDITEVENT_BITMAP);
		ShowWindow(widget, SW_HIDE);
		widget = GetDlgItem(window, IDC_EDITEVENT_LABEL);
		SetWindowText(widget, "Font color:");
	}

	return TRUE;
}

BOOL CALLBACK EditEventProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitDialog(hwndDlg);

        case WM_DRAWITEM:
			if (wParam == IDC_EDITEVENT_FONTCOLOR)
			{
				DRAWITEMSTRUCT* drawItem = (DRAWITEMSTRUCT*)lParam;

				HBRUSH Brush = CreateSolidBrush(g_FontColor);
				FillRect(drawItem->hDC, &drawItem->rcItem, Brush);
				DeleteObject(Brush);

				return TRUE;
			}
			return FALSE;

        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            {
				case IDC_EDITEVENT_BROWSE:
					char file[MAX_LINE_LENGTH];
					OPENFILENAME ofn;
					memset(&ofn, 0, sizeof(OPENFILENAME));
					file[0] = 0;

					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = hwndDlg;
					ofn.lpstrFile = file;
					ofn.nMaxFile = MAX_LINE_LENGTH;
					ofn.Flags = OFN_FILEMUSTEXIST;

					if (GetOpenFileName(&ofn))
					{
						HWND widget = NULL;
						widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_BITMAP);
						SetWindowText(widget, file);
					}
					return TRUE;

				case IDC_EDITEVENT_FONTCOLOR:
					CHOOSECOLOR cc;
					COLORREF custom[16];
					memset(custom, 0, sizeof(COLORREF) * 16);
					memset(&cc, 0, sizeof(CHOOSECOLOR));

					cc.lStructSize = sizeof(CHOOSECOLOR);
					cc.hwndOwner = hwndDlg;
					cc.rgbResult = g_FontColor;
					cc.lpCustColors = custom;
					cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;

					if (ChooseColor(&cc))
					{
						g_FontColor = cc.rgbResult;
						InvalidateRect(hwndDlg, NULL, false);
					}
					return TRUE;

                case IDOK:
					WriteEvent(hwndDlg);
                    // Fall through. 
 
                case IDCANCEL: 
                    EndDialog(hwndDlg, wParam); 
                    return TRUE; 
            } 
    } 
    return FALSE; 
} 

