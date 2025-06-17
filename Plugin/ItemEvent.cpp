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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/ItemEvent.cpp,v 1.5 2002/02/27 18:53:51 rainy Exp $

  $Log: ItemEvent.cpp,v $
  Revision 1.5  2002/02/27 18:53:51  rainy
  Event tooltips are not created inless the window is there.

  Revision 1.4  2002/01/27 16:03:25  rainy
  Changed CEvent to CEventMessage to avoid name clash

  Revision 1.3  2002/01/10 16:46:35  rainy
  Added possibility to show the events in the calendar window.
  Added support for event specific colors/bitmaps.

  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

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

	m_EventFont.DeleteObject();
}

/* 
** Initialize
**
** Initializes the rasterizer
**
*/
void CItemEvent::Initialize()
{
	if( CCalendarWindow::c_Config.GetEventEnable() && 
		CCalendarWindow::c_Config.GetEventRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(CCalendarWindow::c_Config.GetEventRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) throw ERR_OUTOFMEM;

			BMRast->Load(CCalendarWindow::c_Config.GetEventBitmapName());
			BMRast->SetNumOfComponents(CCalendarWindow::c_Config.GetEventNumOfComponents());

			BMRast->SetAlign(CCalendarWindow::c_Config.GetEventAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) throw ERR_OUTOFMEM;

			FNRast->SetFont(CCalendarWindow::c_Config.GetEventFont());
			FNRast->SetAlign(CCalendarWindow::c_Config.GetEventAlign());
			FNRast->UpdateDimensions("XX");
			SetRasterizer(FNRast);
			break;
		}
	}

	if(CCalendarWindow::c_Config.GetEventInCalendar())
	{
		SetFont(CCalendarWindow::c_Config.GetEventFont2());
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
	CTime Current=CCalendarWindow::c_CurrentDate;
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
			m_Events[i]=new CEventMessage;
			if(m_Events[i]) 
			{
				COLORREF color;
				CString bitmap;
				CItem::SetDayType(i, EVENT);
				m_Events[i]->SetMessage(tmpSz);

				// Replace \n's
				CString& tmpString=m_Events[i]->GetMessage();
				tmpString.Replace("\\n", "\n");

				if(GetPrivateProfileString( Date, "FontColor", "", tmpSz, 255, IniPath) > 0) 
				{
					sscanf(tmpSz, "%X", &color);
				}
				else
				{
					color = CCalendarWindow::c_Config.GetEventFontColor();
				}

				if(GetPrivateProfileString( Date, "Bitmap", "", tmpSz, 255, IniPath) > 0) 
				{
					bitmap = tmpSz;
				}
				else
				{
					bitmap = CCalendarWindow::c_Config.GetEventBitmapName();
				}
				m_Events[i]->SetColor(color);
				m_Events[i]->SetBitmap(bitmap);
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
	CTime ThisMonth=CCalendarWindow::c_CurrentDate;
	CTime NextMonth((ThisMonth.GetMonth()==12)?(ThisMonth.GetYear()+1):ThisMonth.GetYear(),
		(ThisMonth.GetMonth()==12)?1:ThisMonth.GetMonth()+1, 1, 0, 0, 0);
	CTimeSpan MonthSpan=NextMonth-ThisMonth;
	NumOfDays=(MonthSpan.GetTotalMinutes()+60)/(24*60);		// Add a hour for possible daylight saving

	FirstWeekday=ThisMonth.GetDayOfWeek();

	if(CCalendarWindow::c_Config.GetStartFromMonday()) {
		FirstWeekday=(FirstWeekday-1);
		if(FirstWeekday==0) FirstWeekday=7;
	} 

	W=CCalendarWindow::c_Config.GetDaysW()/7;	// 7 Columns
	H=CCalendarWindow::c_Config.GetDaysH()/6;	// 6 Rows

	dc.SetBkMode(TRANSPARENT);

	if(m_Rasterizer!=NULL) {
		for(i=0; i<NumOfDays; i++) {
			Index=i+FirstWeekday-1;
			DayType=GetDayType(i+1);

			// Only show event days
			if(DayType&EVENT &&	(!CCalendarWindow::c_Config.GetDaysIgnoreToday() ||
					 !CCalendarWindow::c_Config.GetDaysIgnoreEvent() || !(DayType&TODAY))) {	

				X=CCalendarWindow::c_Config.GetDaysX()+(Index%7)*W;
				Y=CCalendarWindow::c_Config.GetDaysY()+(Index/7)*H;
	
				if(CCalendarWindow::c_Config.GetEventRasterizer() == CRasterizer::TYPE_BITMAP &&
					CCalendarWindow::c_Config.GetEventBitmapName() != m_Events[i + 1]->GetBitmap())
				{
					try
					{
						// Damn this is inefficient way to switch the bitmap
						CRasterizerBitmap* BMRast = new CRasterizerBitmap;
						BMRast->Load(m_Events[i + 1]->GetBitmap());
						BMRast->SetNumOfComponents(CCalendarWindow::c_Config.GetEventNumOfComponents());
						BMRast->SetAlign(CCalendarWindow::c_Config.GetEventAlign());
						BMRast->Paint(dc, X, Y, W, H, i+1);
						delete BMRast;
					}
					catch(...)
					{
						// Do nothing, just fail
					}
				}
				else
				{
					dc.SetTextColor(m_Events[i + 1]->GetColor());
					m_Rasterizer->Paint(dc, X, Y, W, H, i+1);
				}

				// Draw the event texts
				if(CCalendarWindow::c_Config.GetEventInCalendar()) 
				{
					CFont* OldFont;
					OldFont=dc.SelectObject(&m_EventFont);
					CRect rect(X, Y, X + W, Y + H);
					dc.DrawText(m_Events[i + 1]->GetMessage(), rect, DT_CENTER | DT_NOPREFIX | DT_CALCRECT);
					if(rect.Height() >= H)
					{
						rect.top = Y;
						rect.bottom = Y + H;
					}
					else
					{
						rect.top = Y + (H - (rect.bottom - Y)) / 2;
						rect.bottom = rect.top + (rect.top - Y) + H;
					}
					rect.left = X;
					rect.right = X + W;
					dc.SetTextColor(CCalendarWindow::c_Config.GetEventFontColor2());
					dc.DrawText(m_Events[i + 1]->GetMessage(), rect, DT_CENTER | DT_NOPREFIX);
					dc.SelectObject(OldFont);
				}
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
	CTime MonthsFirst=CCalendarWindow::c_CurrentDate;
	int X, Y, W, H, i, j, Day;
	RECT Rect;

	if (!IsWindow(CalendarWnd->GetToolTip().GetSafeHwnd())) return;

	W=CCalendarWindow::c_Config.GetDaysW()/7;	// 7 Columns
	H=CCalendarWindow::c_Config.GetDaysH()/6;	// 6 Rows
	X=CCalendarWindow::c_Config.GetDaysX();
	Y=CCalendarWindow::c_Config.GetDaysY();

	FirstWeekday=MonthsFirst.GetDayOfWeek();

	if(CCalendarWindow::c_Config.GetStartFromMonday()) {
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
				if(CCalendarWindow::c_Config.GetEventEnable() && CCalendarWindow::c_Config.GetEventToolTips()) {
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

/* 
** SetFont
**
** Creates the font from given string. The string must have '/'-char as 
** separator for the fields.
**
*/
void CItemEvent::SetFont(CString& FontName)
{
	int nHeight;
	int nWidth;
	int nEscapement;
	int nOrientation; 
	int nWeight;
	BYTE bItalic; 
	BYTE bUnderline;
	BYTE cStrikeOut; 
	BYTE nCharSet;
	BYTE nOutPrecision;
	BYTE nClipPrecision; 
	BYTE nQuality;
	BYTE nPitchAndFamily;

	sscanf(FontName, "%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i", 
					&nHeight, &nWidth, &nEscapement, &nOrientation, &nWeight,
					&bItalic, &bUnderline, &cStrikeOut, &nCharSet, &nOutPrecision, 
					&nClipPrecision, &nQuality, &nPitchAndFamily);

	if(0==m_EventFont.CreateFont( 
		nHeight, 
		nWidth, 
		nEscapement, 
		nOrientation, 
		nWeight, 
		bItalic, 
		bUnderline, 
		cStrikeOut, 
		nCharSet, 
		nOutPrecision, 
		nClipPrecision, 
		nQuality, 
		nPitchAndFamily, 
		FontName.Mid(FontName.ReverseFind('/')+1)
		)) throw ERR_CREATEFONT;
}
