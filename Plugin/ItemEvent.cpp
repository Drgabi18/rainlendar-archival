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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/ItemEvent.cpp,v 1.7 2002/05/30 18:26:17 rainy Exp $

  $Log: ItemEvent.cpp,v $
  Revision 1.7  2002/05/30 18:26:17  rainy
  Small change

  Revision 1.6  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

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

#include "RainlendarDLL.h"
#include "ItemEvent.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"
#include "CalendarWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemEvent::CItemEvent()
{
	int i;

	for(i = 0; i < 32; i++) 
	{
		m_Events[i]=NULL;
	}
	
	m_EventFont = NULL;
}

CItemEvent::~CItemEvent()
{
	int i;

	for(i = 0; i < 32; i++) 
	{
		if(m_Events[i]) delete m_Events[i];		// Kill the events
	}

	if (m_EventFont) DeleteObject(m_EventFont);
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
			if(BMRast==NULL) throw CError(CError::ERR_OUTOFMEM);

			BMRast->Load(CCalendarWindow::c_Config.GetEventBitmapName());
			BMRast->SetNumOfComponents(CCalendarWindow::c_Config.GetEventNumOfComponents());

			BMRast->SetAlign(CCalendarWindow::c_Config.GetEventAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) throw CError(CError::ERR_OUTOFMEM);

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
	SYSTEMTIME& Current = CCalendarWindow::c_MonthsFirstDate;
	std::string Date;
	int i;

	const char* IniPath = CCalendarWindow::c_Config.GetEventsPath().c_str();

	for(i = 1; i < 32; i++) 
	{
		sprintf(tmpSz, "%i-%i-%i", i, Current.wMonth, Current.wYear);
		Date = tmpSz;

		if(m_Events[i]) delete m_Events[i];		// Kill the old ones
		m_Events[i] = NULL;

		if(GetPrivateProfileString( Date.c_str(), "Message", "", tmpSz, 255, IniPath) > 0) 
		{
			m_Events[i] = new CEventMessage;

			if(m_Events[i]) 
			{
				COLORREF color;
				std::string bitmap;
				CItem::SetDayType(i, EVENT);

				// Replace \n's
				std::string tmpString = tmpSz;
				
				int pos = tmpString.find("\\n");
				while (pos != -1)
				{
					tmpString.replace(tmpString.begin() + pos, tmpString.begin() + pos + 2, "\n");
					pos = tmpString.find("\\n");
				}

				m_Events[i]->SetMessage(tmpString);

				if(GetPrivateProfileString( Date.c_str(), "FontColor", "", tmpSz, 255, IniPath) > 0) 
				{
					sscanf(tmpSz, "%X", &color);
				}
				else
				{
					color = CCalendarWindow::c_Config.GetEventFontColor();
				}

				if(GetPrivateProfileString( Date.c_str(), "Bitmap", "", tmpSz, 255, IniPath) > 0) 
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
		}
	}
} 

/* 
** Paint
**
** Paints the numbers in correct place
**
*/
void CItemEvent::Paint(HDC dc)
{
	int FirstWeekday;
	int X, Y, W, H, Index, DayType, NumOfDays, i;

	// Calculate the number of days in this month
	NumOfDays = GetDaysInMonth(CCalendarWindow::c_MonthsFirstDate.wYear, CCalendarWindow::c_MonthsFirstDate.wMonth);
	FirstWeekday = CCalendarWindow::c_MonthsFirstDate.wDayOfWeek;

	if(CCalendarWindow::c_Config.GetStartFromMonday()) 
	{
		FirstWeekday = (FirstWeekday - 1);
		if(FirstWeekday == -1) FirstWeekday = 6;
	} 

	W = CCalendarWindow::c_Config.GetDaysW() / 7;	// 7 Columns
	H = CCalendarWindow::c_Config.GetDaysH() / 6;	// 6 Rows

	SetBkMode(dc, TRANSPARENT);

	if(m_Rasterizer!=NULL) 
	{
		for(i = 0; i < NumOfDays; i++) 
		{
			Index = i + FirstWeekday;
			DayType = GetDayType(i + 1);

			// Only show event days
			if((DayType & EVENT) && (!CCalendarWindow::c_Config.GetDaysIgnoreToday() ||
				!CCalendarWindow::c_Config.GetDaysIgnoreEvent() || !(DayType & TODAY))) 
			{	
				X = CCalendarWindow::c_Config.GetDaysX() + (Index % 7) * W;
				Y = CCalendarWindow::c_Config.GetDaysY() + (Index / 7) * H;
	
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
					SetTextColor(dc, m_Events[i + 1]->GetColor());
					m_Rasterizer->Paint(dc, X, Y, W, H, i+1);
				}

				// Draw the event texts
				if(CCalendarWindow::c_Config.GetEventInCalendar()) 
				{
					HFONT OldFont;
					RECT rect = {X, Y, X + W, Y + H};

					OldFont = (HFONT)SelectObject(dc, m_EventFont);
					DrawText(dc, m_Events[i + 1]->GetMessage().c_str(), m_Events[i + 1]->GetMessage().size(), &rect, DT_CENTER | DT_NOPREFIX | DT_CALCRECT);

					if(rect.bottom - rect.top >= H)
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
					SetTextColor(dc, CCalendarWindow::c_Config.GetEventFontColor2());
					DrawText(dc, m_Events[i + 1]->GetMessage().c_str(), m_Events[i + 1]->GetMessage().size(), &rect, DT_CENTER | DT_NOPREFIX);
					SelectObject(dc, OldFont);
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
	int X, Y, W, H, i, j, Day;
	RECT Rect;

	W = CCalendarWindow::c_Config.GetDaysW() / 7;	// 7 Columns
	H = CCalendarWindow::c_Config.GetDaysH() / 6;	// 6 Rows
	X = CCalendarWindow::c_Config.GetDaysX();
	Y = CCalendarWindow::c_Config.GetDaysY();

	FirstWeekday = CCalendarWindow::c_MonthsFirstDate.wDayOfWeek;

	if(CCalendarWindow::c_Config.GetStartFromMonday()) 
	{
		FirstWeekday = (FirstWeekday - 1);
		if(FirstWeekday == -1) FirstWeekday = 6;
	} 

	Rect.top = Y;
	Rect.bottom = Y + H;
	for(j = 0; j < 6; j++) 
	{
		Rect.left = X;
		Rect.right = X + W;
		for (i = 0; i < 7; i++) 
		{
			Day = j * 7 + i + 1 - FirstWeekday;
			if(Day > 0 && Day < 32) 
			{
				RemoveToolTip(CalendarWnd, Day);	// Remove old toltips
				if(CCalendarWindow::c_Config.GetEventEnable() && CCalendarWindow::c_Config.GetEventToolTips()) 
				{
					if(m_Events[Day]) AddToolTip(CalendarWnd, &Rect, Day);
				}
			}
			Rect.right += W;
			Rect.left += W;
		}
		Rect.top += H;
		Rect.bottom += H;
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
	if (CalendarWnd->GetToolTip())
	{
		TOOLINFO ti;    // tool information

		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_SUBCLASS | TTF_TRANSPARENT;
		ti.hwnd = CalendarWnd->GetWindow();
		ti.hinst = NULL;
		ti.uId = Day;
		ti.lpszText = LPSTR_TEXTCALLBACK;
		ti.rect = *Rect;

		SendMessage(CalendarWnd->GetToolTip(), TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
	}
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
	ti.hwnd = CalendarWnd->GetWindow();
	ti.uId = Day;

	SendMessage(CalendarWnd->GetToolTip(), TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
}

/* 
** SetFont
**
** Creates the font from given string. The string must have '/'-char as 
** separator for the fields.
**
*/
void CItemEvent::SetFont(const std::string& FontName)
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

	sscanf(FontName.c_str(), "%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i", 
					&nHeight, &nWidth, &nEscapement, &nOrientation, &nWeight,
					&bItalic, &bUnderline, &cStrikeOut, &nCharSet, &nOutPrecision, 
					&nClipPrecision, &nQuality, &nPitchAndFamily);

	int pos = FontName.rfind('/');
	std::string name(FontName.begin() + pos, FontName.end());

	m_EventFont = CreateFont( 
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
				name.c_str());

	if (m_EventFont == NULL) throw CError(CError::ERR_CREATEFONT);
}