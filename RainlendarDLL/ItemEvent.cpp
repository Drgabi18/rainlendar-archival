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

// ItemEvent.cpp: implementation of the CItemEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RainlendarDLL.h"
#include "ItemEvent.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"
#include "CalendarWindow.h"
#include <direct.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemEvent::CItemEvent()
{
	int i;

	for(i=0; i<32; i++) {
		m_Events[i]=NULL;
	}
}

CItemEvent::~CItemEvent()
{
	int i;

	for(i=0; i<32; i++) {
		if(m_Events[i]) delete m_Events[i];		// Kill the events
	}
}

/* 
** Initialize
**
** Initializes the rasterizer
**
*/
void CItemEvent::Initialize()
{
	if( CCalendarWindow::C_Config.GetEventEnable() && 
		CCalendarWindow::C_Config.GetEventRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(CCalendarWindow::C_Config.GetEventRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) throw ERR_OUTOFMEM;

			BMRast->Load(CCalendarWindow::C_Config.GetEventBitmapName());
			BMRast->SetNumOfComponents(CCalendarWindow::C_Config.GetEventNumOfComponents());

			BMRast->SetAlign(CCalendarWindow::C_Config.GetEventAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) throw ERR_OUTOFMEM;

			FNRast->SetFont(CCalendarWindow::C_Config.GetEventFont());
			FNRast->SetAlign(CCalendarWindow::C_Config.GetEventAlign());
			FNRast->UpdateDimensions("XX");
			SetRasterizer(FNRast);
			break;
		}
	}

	// Check for the events
	ReadEvents();
}

/* 
** ReadEvents
**
** Reads the Events for current month
**
*/
void CItemEvent::ReadEvents()
{
	char tmpSz[MAX_LINE_LENGTH];
	char IniPath[MAX_LINE_LENGTH];
	CTime Current=CCalendarWindow::C_CurrentDate;
	CString Date;
	int i;
	char* Slash;

	// Get the DLL's directory
	GetModuleFileName(GetModuleHandle("Rainlendar.dll"), IniPath, MAX_LINE_LENGTH);

	Slash=strrchr(IniPath, '\\');
	if(Slash==NULL) {
		strcpy(IniPath, "Events.ini");
	} else {
		strcpy(Slash, "\\Events.ini");
	}

	for(i=1; i<32; i++) {
		Date.Format("%i-%i-%i", i, Current.GetMonth(), Current.GetYear());

		if(m_Events[i]) delete m_Events[i];		// Kill the old ones

		if(GetPrivateProfileString( Date, "Message", "", tmpSz, 255, IniPath) > 0) {
			m_Events[i]=new CEvent;
			if(m_Events[i]) {
				CItem::SetDayType(i, EVENT);
				m_Events[i]->SetMessage(tmpSz);

				// Replace \n's
				CString& tmpString=m_Events[i]->GetMessage();
				tmpString.Replace("\\n", "\n");
			}
		} else {
			m_Events[i]=NULL;
		}
	}
} 

/* 
** Paint
**
** Paints the numbers in correct place
**
*/
void CItemEvent::Paint(CDC& dc)
{
	int FirstWeekday;
	int X, Y, W, H, Index, DayType, NumOfDays, i;

	// Calculate the number of days in this month
	CTime ThisMonth=CCalendarWindow::C_CurrentDate;
	CTime NextMonth((ThisMonth.GetMonth()==12)?(ThisMonth.GetYear()+1):ThisMonth.GetYear(),
		(ThisMonth.GetMonth()==12)?1:ThisMonth.GetMonth()+1, 1, 0, 0, 0);
	CTimeSpan MonthSpan=NextMonth-ThisMonth;
	NumOfDays=(MonthSpan.GetTotalMinutes()+60)/(24*60);		// Add a hour for possible daylight saving

	FirstWeekday=ThisMonth.GetDayOfWeek();

	if(CCalendarWindow::C_Config.GetStartFromMonday()) {
		FirstWeekday=(FirstWeekday-1);
		if(FirstWeekday==0) FirstWeekday=7;
	} 

	W=CCalendarWindow::C_Config.GetDaysW()/7;	// 7 Columns
	H=CCalendarWindow::C_Config.GetDaysH()/6;	// 6 Rows

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(CCalendarWindow::C_Config.GetEventFontColor());

	if(m_Rasterizer!=NULL) {
		for(i=0; i<NumOfDays; i++) {
			Index=i+FirstWeekday-1;
			DayType=GetDayType(i+1);

			// Only show event days
			if(DayType&EVENT &&	(!CCalendarWindow::C_Config.GetDaysIgnoreToday() ||
					 !CCalendarWindow::C_Config.GetDaysIgnoreEvent() || !(DayType&TODAY))) {	

				X=CCalendarWindow::C_Config.GetDaysX()+(Index%7)*W;
				Y=CCalendarWindow::C_Config.GetDaysY()+(Index/7)*H;
	
				m_Rasterizer->Paint(dc, X, Y, W, H, i+1);
			}
		}
	}
}


/* 
** AddToolTips
**
** Adds tooltips for the current month
**
*/
void CItemEvent::AddToolTips(CCalendarWindow* CalendarWnd)
{
	int FirstWeekday;
	CTime MonthsFirst=CCalendarWindow::C_CurrentDate;
	int X, Y, W, H, i, j, Day;
	RECT Rect;

	W=CCalendarWindow::C_Config.GetDaysW()/7;	// 7 Columns
	H=CCalendarWindow::C_Config.GetDaysH()/6;	// 6 Rows
	X=CCalendarWindow::C_Config.GetDaysX();
	Y=CCalendarWindow::C_Config.GetDaysY();

	FirstWeekday=MonthsFirst.GetDayOfWeek();

	if(CCalendarWindow::C_Config.GetStartFromMonday()) {
		FirstWeekday=(FirstWeekday-1);
		if(FirstWeekday==0) FirstWeekday=7;
	} 

	Rect.top=Y;
	Rect.bottom=Y+H;
	for(j=0; j<6; j++) {
		Rect.left=X;
		Rect.right=X+W;
		for(i=0; i<7; i++) {
			Day=j*7+i+2-FirstWeekday;
			if(Day>0 && Day<32) {
				RemoveToolTip(CalendarWnd, Day);	// Remove old toltips
				if(CCalendarWindow::C_Config.GetEventEnable() && CCalendarWindow::C_Config.GetEventToolTips()) {
					if(m_Events[Day]) AddToolTip(CalendarWnd, &Rect, Day);
				}
			}
			Rect.right+=W;
			Rect.left+=W;
		}
		Rect.top+=H;
		Rect.bottom+=H;
	}
}

/* 
** AddToolTip
**
** Adds the Tooltip-message for  a window
**
*/
void CItemEvent::AddToolTip(CCalendarWindow* CalendarWnd, RECT* Rect, int Day) 
{
	TOOLINFO ti;    // tool information

	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = CalendarWnd->m_hWnd;
	ti.hinst = NULL;
	ti.uId = Day;
	ti.lpszText = LPSTR_TEXTCALLBACK;
	ti.rect = *Rect;

	CalendarWnd->GetToolTip().SendMessage(TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
}

/* 
** RemoveToolTip
**
** Removes the Tooltip-message for  a window
**
*/
void CItemEvent::RemoveToolTip(CCalendarWindow* CalendarWnd, int Day) 
{
	TOOLINFO ti;    // tool information

	ti.cbSize = sizeof(TOOLINFO);
	ti.hwnd = CalendarWnd->m_hWnd;
	ti.uId = Day;

	CalendarWnd->GetToolTip().SendMessage(TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
}
