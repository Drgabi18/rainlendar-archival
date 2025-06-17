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
  $Header: /home/cvsroot/Rainlendar/Library/ItemEvent.cpp,v 1.2 2005/09/08 16:09:12 rainy Exp $

  $Log: ItemEvent.cpp,v $
  Revision 1.2  2005/09/08 16:09:12  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.21  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.20  2004/12/05 18:29:45  rainy
  Fixed a crash bug when calendar size was 0.

  Revision 1.19  2004/11/06 13:38:59  rainy
  no message

  Revision 1.18  2004/01/25 10:00:37  rainy
  Added separate method for icons.

  Revision 1.17  2003/10/27 17:37:51  Rainy
  Config is now singleton.

  Revision 1.16  2003/10/04 14:50:41  Rainy
  Added always drawn profiles and priority system.

  Revision 1.15  2003/08/09 16:36:05  Rainy
  Removed GetEventText()

  Revision 1.14  2003/06/15 09:49:12  Rainy
  Added support for multiple calendars.

  Revision 1.13  2003/05/25 18:10:11  Rainy
  The GetEvent() returns now a sorted vector.

  Revision 1.12  2002/11/25 17:03:24  rainy
  Added DrawIcon method.

  Revision 1.11  2002/11/12 18:13:32  rainy
  The interface of Paint changed a little.
  Calendar text can be painted on RasterizerFont's alpha buffer.

  Revision 1.10  2002/08/24 11:11:10  rainy
  Few changes to prevent showing the deleted events.

  Revision 1.9  2002/08/10 08:38:59  rainy
  Removed prefixes from the event texts.

  Revision 1.8  2002/08/03 16:16:19  rainy
  Changed to use EventManager and customToolTip class.

  Revision 1.7  2002/05/30 18:26:17  rainy
  Small change

  Revision 1.6  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.5  2002/02/27 18:53:51  rainy
  Event tooltips are not created inless the window is there.

  Revision 1.4  2002/01/27 16:03:25  rainy
  Changed CEvent to CEntryEvent to avoid name clash

  Revision 1.3  2002/01/10 16:46:35  rainy
  Added possibility to show the events in the calendar window.
  Added support for event specific colors/bitmaps.

  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/
#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "ItemEvent.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"
#include "CalendarWindow.h"
#include "Tooltip.h"
#include <set>
#include <list>
#include <algorithm>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemEvent::CItemEvent()
{
	m_InsideEventFont = NULL;

	m_EventBitmapName = "";
	m_EventAlign=CRasterizer::ALIGN_LEFT;
	m_EventNumOfComponents=1;
	m_EventInCalendar=false;
	m_EventRasterizer=CRasterizer::TYPE_NONE;
	m_EventFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_EventFont2="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_EventFontColor=0;
	m_EventFontColor2=0;
	m_EventSeparation=0;
	m_EventIconEnable=true;		// Mitul
}

CItemEvent::~CItemEvent()
{
	if (m_InsideEventFont) DeleteObject(m_InsideEventFont);
}

void CItemEvent::ReadSettings(const char* iniFile, const char* section)
{
	CItem::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	// Event stuff
	m_Enabled=(1==GetPrivateProfileInt( section, "EventEnable", m_Enabled?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( section, "EventBitmapName", m_EventBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventBitmapName=tmpSz;
	}
	m_EventAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( section, "EventAlign", m_EventAlign, iniFile);
	m_EventNumOfComponents=GetPrivateProfileInt( section, "EventNumOfComponents", m_EventNumOfComponents, iniFile);
	if(GetPrivateProfileString( section, "EventRasterizer", CConfig::ConvertRasterizer(m_EventRasterizer), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventRasterizer=CConfig::ConvertRasterizer(tmpSz);
	}
	else 
	{
		m_EventRasterizer=CRasterizer::TYPE_NONE;
	}
	if(GetPrivateProfileString( section, "EventFont", m_EventFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventFont=tmpSz;
	}
	if(GetPrivateProfileString( section, "EventFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventFontColor = CConfig::ParseColor(tmpSz);
	}
	m_EventInCalendar=(1==GetPrivateProfileInt( section, "EventInCalendar", m_EventInCalendar?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( section, "EventFont2", m_EventFont2.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventFont2=tmpSz;
	}
	if(GetPrivateProfileString( section, "EventFontColor2", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventFontColor2 = CConfig::ParseColor(tmpSz);
	}
	m_EventSeparation=GetPrivateProfileInt( section, "EventSeparation", m_EventSeparation, iniFile);
	m_EventIconEnable=(1==GetPrivateProfileInt( section, "EventIconEnable", m_EventIconEnable?1:0, iniFile))?true:false; // Mitul
}

void CItemEvent::WriteSettings()
{
	char tmpSz[256];

	std::string INIPath = m_SettingsFile;

	// Event stuff
	sprintf(tmpSz, "%i", m_Enabled);
	WritePrivateProfileString( m_Section.c_str(), "EventEnable", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "EventBitmapName", m_EventBitmapName.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%i", m_EventAlign);
	WritePrivateProfileString( m_Section.c_str(), "EventAlign", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_EventNumOfComponents);
	WritePrivateProfileString( m_Section.c_str(), "EventNumOfComponents", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "EventRasterizer", CConfig::ConvertRasterizer(m_EventRasterizer), INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "EventFont", m_EventFont.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%X", m_EventFontColor);
	WritePrivateProfileString( m_Section.c_str(), "EventFontColor", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_EventInCalendar);
	WritePrivateProfileString( m_Section.c_str(), "EventInCalendar", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "EventFont2", m_EventFont2.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%X", m_EventFontColor2);
	WritePrivateProfileString( m_Section.c_str(), "EventFontColor2", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_EventSeparation);
	WritePrivateProfileString( m_Section.c_str(), "EventSeparation", tmpSz, INIPath.c_str() );
	// Mitul{
	sprintf(tmpSz, "%i", m_EventIconEnable);
	WritePrivateProfileString( m_Section.c_str(), "EventIconEnable", tmpSz, INIPath.c_str() );
	// Mitul}
}

/* 
** Initialize
**
** Initializes the rasterizer
**
*/
void CItemEvent::Initialize()
{
	if (GetEventRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(GetEventRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) THROW(ERR_OUTOFMEM);

			BMRast->Load(GetEventBitmapName());
			BMRast->SetNumOfComponents(GetEventNumOfComponents());
			BMRast->SetSeparation(GetEventSeparation());
			BMRast->SetAlign(GetEventAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) THROW(ERR_OUTOFMEM);

			FNRast->SetFont(GetEventFont());
			FNRast->SetAlign(GetEventAlign());
			FNRast->SetColor(GetEventFontColor());
			FNRast->UpdateDimensions("XX");
			SetRasterizer(FNRast);
			break;
		}

		if(GetEventInCalendar())
		{
			SetFont(GetEventFont2());
		}
	}

	m_Enabled = m_Enabled && GetRainlendar()->GetCalendarWindow().GetItemDays()->IsEnabled();
}

/* 
** Paint
**
** Paints the numbers in correct place
**
*/
void CItemEvent::Paint(CImage& background, POINT offset)
{
	int FirstWeekday;
	int X, Y, W, H, Index, DayType, NumOfDays, i;

	CItemDays* days = GetRainlendar()->GetCalendarWindow().GetItemDays();

	// Calculate the number of days in this month
	NumOfDays = CCalendarWindow::c_MonthsFirstDate.GetDaysInMonth();
	FirstWeekday = CCalendarWindow::c_MonthsFirstDate.GetWeekday();

	if(CConfig::Instance().GetStartFromMonday()) 
	{
		FirstWeekday = (FirstWeekday - 1);
		if(FirstWeekday == -1) FirstWeekday = 6;
	} 

	switch (days->GetDaysLayout())
	{
	case CConfig::DAY_LAYOUT_HORIZONTAL:
		W = days->GetDaysW() / 31;
		H = days->GetDaysH();
		break;

	case CConfig::DAY_LAYOUT_VERTICAL:
		W = days->GetDaysW();
		H = days->GetDaysH() / 31;	// 6 Rows
		break;

	default:
		W = days->GetDaysW() / 7;	// 7 Columns
		H = days->GetDaysH() / 6;	// 6 Rows
	}

	SYSTEMTIME sysTime = CCalendarWindow::c_MonthsFirstDate.GetSystemTime();

	if(m_Rasterizer!=NULL) 
	{
		for(i = 0; i < NumOfDays; i++) 
		{
			Index = i + FirstWeekday;
			DayType = GetDayType(i + 1, sysTime.wMonth, sysTime.wYear, false);

			switch (days->GetDaysLayout())
			{
			case CConfig::DAY_LAYOUT_HORIZONTAL:
				X = days->GetDaysX() + i * W;
				Y = days->GetDaysY();
				break;

			case CConfig::DAY_LAYOUT_VERTICAL:
				X = days->GetDaysX();
				Y = days->GetDaysY() + i * H;
				break;

			default:
				X = days->GetDaysX() + (Index % 7) * W;
				Y = days->GetDaysY() + (Index / 7) * H;
			}

			X += offset.x;
			Y += offset.y;


			if (DayType & DAYTYPE_EVENT)
			{
				if (!((DayType & DAYTYPE_TODAY) && days->GetDaysIgnoreToday()))
				{
					std::set<const Profile*> drawnProfiles;
					std::multimap<int, CEntryEvent*> eventMap;
					std::list<const Profile*> drawThese;
					bool normalFound = false;

					CalculatePriorities(i + 1, sysTime.wMonth, sysTime.wYear, eventMap);

					std::multimap<int, CEntryEvent*>::reverse_iterator riter = eventMap.rbegin();
					for ( ; riter != eventMap.rend(); riter++)
					{
						LPCTSTR profileName = ((*riter).second)->GetProfile();
						const Profile* profile = CConfig::Instance().GetProfile(profileName);

						if (profile && profile->drawAlways && drawnProfiles.find(profile) == drawnProfiles.end())
						{
							drawnProfiles.insert(profile);
							drawThese.push_front(profile);
						}
						else
						{
							if (!normalFound)
							{
								drawThese.push_front(profile);
								normalFound = true;
							}
						}
					}

					std::list<const Profile*>::iterator iter = drawThese.begin();
					for ( ; iter != drawThese.end(); iter++)
					{
						m_Rasterizer->SetProfile(*iter);
						m_Rasterizer->Paint(background, X, Y, W, H, i + 1);
					}
				}
			}

			if (DayType & DAYTYPE_EVENT)
			{
				// Draw the event texts
				if(GetEventInCalendar()) 
				{
					HDC dc = CreateCompatibleDC(NULL);
					HFONT OldFont;
					HBITMAP OldBitmap;
					int count = 0, width = 0, height = 0;
					RECT rect;
					std::vector<int> heights;
					std::vector<CEntryEvent*>::iterator eventIter;
					std::vector<CEntryEvent*> eventList = GetRainlendar()->GetCalendarWindow().GetEventManager().GetEvents(i + 1, sysTime.wMonth, sysTime.wYear, EVENT_FLAG_NO_DELETED | EVENT_FLAG_NO_CALENDAR_HIDDEN);

					OldFont = (HFONT)SelectObject(dc, m_InsideEventFont);
					OldBitmap = (HBITMAP)SelectObject(dc, background.GetBitmap());

					std::string text;

					// First calculate the size of the texts
					for(eventIter = eventList.begin();  eventIter != eventList.end(); eventIter++)
					{
						std::string message;
						(*eventIter)->GetBriefMessage(message, 100, true, false);
						if (!message.empty())
						{
							rect.left = 0;
							rect.right = W;
							rect.top = 0;
							rect.bottom = 0;
							DrawText(dc, message.c_str(), -1, &rect, DT_NOPREFIX | DT_CALCRECT | DT_WORDBREAK);
							height += rect.bottom - rect.top;
							width = max (width, rect.right - rect.left);
							heights.push_back(rect.bottom - rect.top);
						}
					}

					SetBkMode(dc, TRANSPARENT);

					rect.top = Y + H / 2 - height / 2;
					rect.bottom = rect.top + height;
					rect.left = X;
					rect.right = rect.left + W;

					rect.top = max(rect.top, Y);
					rect.bottom = min(rect.bottom, rect.top + H);

					if (CCalendarWindow::Is2k() && CConfig::Instance().GetNativeTransparency())
					{
						// The font rasterizer has a buffer for us, where we can draw the text
						CRasterizerFont::CreateBuffers(background.GetWidth(), background.GetHeight());
					}

					// Then draw them to the window
					for(eventIter = eventList.begin();  eventIter != eventList.end(); eventIter++)
					{
						std::string message;
						(*eventIter)->GetBriefMessage(message, 100, true, false);
						if (!message.empty())
						{
							LPCTSTR profileName = (*eventIter)->GetProfile();
							const Profile* profile = CConfig::Instance().GetProfile(profileName);

							if (profile)
							{
								SetTextColor(dc, profile->fontColor2);
							}
							else
							{
								SetTextColor(dc, GetEventFontColor2());
							}

							if (CCalendarWindow::Is2k() && CConfig::Instance().GetNativeTransparency())
							{
								SelectObject(dc, CRasterizerFont::GetColorBuffer().GetBitmap());
								DrawText(dc, message.c_str(), -1, &rect, DT_NOPREFIX | DT_WORDBREAK);
								
								SelectObject(dc, CRasterizerFont::GetAlphaBuffer().GetBitmap());
								SetTextColor(dc, RGB(255, 255, 255));
								DrawText(dc, message.c_str(), -1, &rect, DT_NOPREFIX | DT_WORDBREAK);
							}
							else
							{
								// Just draw it normally
								DrawText(dc, message.c_str(), -1, &rect, DT_NOPREFIX | DT_WORDBREAK);
							}

							rect.top += heights[count];
							count++;
						}
					}

					CRasterizerFont::DrawBuffers(background);

					SelectObject(dc, OldBitmap);
					SelectObject(dc, OldFont);
					DeleteDC(dc);
				}
			}
		}
	}
}

void CItemEvent::PaintIcons(CImage& background, POINT offset)
{
	int FirstWeekday;
	int X, Y, W, H, Index, DayType, NumOfDays, i;

	CItemDays* days = GetRainlendar()->GetCalendarWindow().GetItemDays();

	// Calculate the number of days in this month
	NumOfDays = CCalendarWindow::c_MonthsFirstDate.GetDaysInMonth();
	FirstWeekday = CCalendarWindow::c_MonthsFirstDate.GetWeekday();

	if(CConfig::Instance().GetStartFromMonday()) 
	{
		FirstWeekday = (FirstWeekday - 1);
		if(FirstWeekday == -1) FirstWeekday = 6;
	} 

	switch (days->GetDaysLayout())
	{
	case CConfig::DAY_LAYOUT_HORIZONTAL:
		W = days->GetDaysW() / 31;
		H = days->GetDaysH();
		break;

	case CConfig::DAY_LAYOUT_VERTICAL:
		W = days->GetDaysW();
		H = days->GetDaysH() / 31;	// 6 Rows
		break;

	default:
		W = days->GetDaysW() / 7;	// 7 Columns
		H = days->GetDaysH() / 6;	// 6 Rows
	}

	SYSTEMTIME sysTime = CCalendarWindow::c_MonthsFirstDate.GetSystemTime();

	for(i = 0; i < NumOfDays; i++) 
	{
		Index = i + FirstWeekday;
		DayType = GetDayType(i + 1, sysTime.wMonth, sysTime.wYear, false);

		switch (days->GetDaysLayout())
		{
		case CConfig::DAY_LAYOUT_HORIZONTAL:
			X = days->GetDaysX() + i * W;
			Y = days->GetDaysY();
			break;

		case CConfig::DAY_LAYOUT_VERTICAL:
			X = days->GetDaysX();
			Y = days->GetDaysY() + i * H;
			break;

		default:
			X = days->GetDaysX() + (Index % 7) * W;
			Y = days->GetDaysY() + (Index / 7) * H;
		}

		X += offset.x;
		Y += offset.y;

		if (DayType & DAYTYPE_EVENT)
		{
			// Draw the icon if there is one
			DrawIcon(background, i + 1, sysTime.wMonth, sysTime.wYear, X, Y, W, H);
		}
	}
}

void CItemEvent::DrawIcon(CImage& background, int day, int month, int year, int X, int Y, int W, int H)
{
	// Draw all icons 
	std::vector<CEntryEvent*> eventList = GetRainlendar()->GetCalendarWindow().GetEventManager().GetEvents(day, month, year, EVENT_FLAG_NO_DELETED | EVENT_FLAG_NO_CALENDAR_HIDDEN);
	
	if (!eventList.empty())
	{
		std::vector<CEntryEvent*>::iterator i = eventList.begin();
		for( ;  i != eventList.end(); i++)
		{
			int x = X;
			int y = Y;

			const Profile* profile = CConfig::Instance().GetProfile((*i)->GetProfile());

			if (profile && profile->icon.GetBitmap() != NULL)
			{
				// Align the bitmap correctly
				switch (profile->iconAlign & 0x0F)
				{
				case CRasterizer::ALIGN_LEFT:
					x = X;
					break;

				case CRasterizer::ALIGN_HCENTER:
					x = X + (W - profile->icon.GetWidth()) / 2;
					break;

				case CRasterizer::ALIGN_RIGHT:
					x = X + W - profile->icon.GetWidth();
					break;
				};

				switch (profile->iconAlign & 0x0F0)
				{
				case CRasterizer::ALIGN_TOP:
					y = Y;
					break;

				case CRasterizer::ALIGN_VCENTER:
					y = Y + (H - profile->icon.GetHeight()) / 2;
					break;

				case CRasterizer::ALIGN_BOTTOM:
					y = Y + (H - profile->icon.GetHeight());
					break;
				};

				background.Blit(profile->icon, x, y, 0, 0, profile->icon.GetWidth(), profile->icon.GetHeight());
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
void CItemEvent::AddToolTips(CCalendarWindow* CalendarWnd, POINT offset)
{
	if (IsEnabled() && CConfig::Instance().GetEventToolTips()) 
	{
		int FirstWeekday;
		int X, Y, W, H, i, j, Day;
		RECT Rect;
		SYSTEMTIME sysTime = CCalendarWindow::c_MonthsFirstDate.GetSystemTime();

		CItemDays* days = GetRainlendar()->GetCalendarWindow().GetItemDays();

		X = days->GetDaysX();
		Y = days->GetDaysY();

		X += offset.x;
		Y += offset.y;

		if (days->GetDaysLayout() == CConfig::DAY_LAYOUT_HORIZONTAL)
		{
			W = days->GetDaysW() / 31;
			H = days->GetDaysH();

			for (i = 0; i < 31; i++) 
			{
				Rect.top = Y;
				Rect.bottom = Y + H;
				Rect.left = X + i * W;
				Rect.right = Rect.left + W;

				AddTip(i + 1, sysTime.wMonth, sysTime.wYear, CalendarWnd->GetWindow(), Rect);
			}
		}
		else if (days->GetDaysLayout() == CConfig::DAY_LAYOUT_VERTICAL)
		{
			W = days->GetDaysW();
			H = days->GetDaysH() / 31;

			for (i = 0; i < 31; i++) 
			{
				Rect.top = Y + i * H;
				Rect.bottom = Rect.top + H;
				Rect.left = X;
				Rect.right = X + W;

				AddTip(i + 1, sysTime.wMonth, sysTime.wYear, CalendarWnd->GetWindow(), Rect);
			}
		}
		else
		{
			W = days->GetDaysW() / 7;	// 7 Columns
			H = days->GetDaysH() / 6;	// 6 Rows

			FirstWeekday = sysTime.wDayOfWeek;

			if(CConfig::Instance().GetStartFromMonday()) 
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
						AddTip(Day, sysTime.wMonth, sysTime.wYear, CalendarWnd->GetWindow(), Rect);
					}
					Rect.right += W;
					Rect.left += W;
				}
				Rect.top += H;
				Rect.bottom += H;
			}
		}
	}
}

extern bool CompareEvents(CEntryEvent* arg1, CEntryEvent* arg2);		// This is in EventListWindow

void CItemEvent::AddTip(int day, int month, int year, HWND hWnd, const RECT& rect)
{
	std::vector<CEntryEvent*> eventList = GetRainlendar()->GetCalendarWindow().GetEventManager().GetEvents(day, month, year, EVENT_FLAG_NO_DELETED);
	std::sort(eventList.begin(), eventList.end(), CompareEvents);

	if (!eventList.empty())
	{
		int ID = CToolTip::Instance().CreateToolTip(rect, hWnd);

		std::vector<CEntryEvent*>::iterator eventIter = eventList.begin();
		for( ;  eventIter != eventList.end(); eventIter++)
		{
			CToolTip::ToolTipData ttd;
			
			(*eventIter)->GetBriefMessage(ttd.text, 0, true, true);

			if (!ttd.text.empty())
			{
				const Profile* profile = CConfig::Instance().GetProfile((*eventIter)->GetProfile());

				if (profile)
				{
					ttd.color = profile->toolTipColor;
				}
				else
				{
					ttd.color = CToolTip::Instance().GetToolTipFontColor();
				}

				CToolTip::Instance().AddData(ID, ttd);
			}
		}
	}
}


/* 
** SortEventsByProfile
**
** Calculates priorities for the profiles for given day.
**
*/
void CItemEvent::CalculatePriorities(int day, int month, int year, std::multimap<int, CEntryEvent*>& eventMap)
{
	const char* currentProfile = NULL;
	int newPriority = 0;
	int profilePriority = 0;
	int currentPriority = 0;

	// Events that occur more rarely override frequent events
	const int dailyPriority    = 10;
	const int weeklyPriority   = 20;
	const int monthlyPriority  = 30;
	const int annuallyPriority = 40;
	const int singlePriority   = 50;

	std::vector<CEntryEvent*>& eventList = GetRainlendar()->GetCalendarWindow().GetEventManager().GetEvents(day, month, year, EVENT_FLAG_NO_DELETED | EVENT_FLAG_NO_CALENDAR_HIDDEN);
	
	if (!eventList.empty())
	{
		std::vector<CEntryEvent*>::iterator i = eventList.begin();
		for( ;  i != eventList.end(); i++)
		{
			const Profile* profile = CConfig::Instance().GetProfile((*i)->GetProfile());

			if (profile)
			{
				profilePriority = profile->priority;
			}
			else
			{
				profilePriority = 0;
			}

			RainlendarRecurrency* recurrency = (*i)->GetRecurrency();

			if (recurrency)
			{
				switch(recurrency->type)
				{
				case RECURRENCY_TYPE_DAILY:
					newPriority = dailyPriority + profilePriority;
					break;

				case RECURRENCY_TYPE_WEEKLY:
					newPriority = weeklyPriority + profilePriority;
					break;

				case RECURRENCY_TYPE_MONTHLY_NTH_DAY:
				case RECURRENCY_TYPE_MONTHLY_NTH_WEEKDAY:
				case RECURRENCY_TYPE_MONTHLY_LASTWEEK_WEEKDAY:
				case RECURRENCY_TYPE_MONTHLY_LAST_DAY:
				case RECURRENCY_TYPE_MONTHLY_FIRST_WEEKDAY:
				case RECURRENCY_TYPE_MONTHLY_LAST_WEEKDAY:
					newPriority = monthlyPriority + profilePriority;
					break;

				case RECURRENCY_TYPE_YEARLY:
					newPriority = annuallyPriority + profilePriority;
					break;

				default:
					newPriority = singlePriority + profilePriority;
					break;
				}
			}
			else
			{
				newPriority = singlePriority + profilePriority;
			}

			eventMap.insert(std::pair<int, CEntryEvent*>(newPriority, (*i)));
		}
	}
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
	if (m_InsideEventFont) DeleteObject(m_InsideEventFont);
	m_InsideEventFont = CRasterizerFont::CreateFont(FontName);
}

/* 
** HitTest
**
** Returns the day under the given coordinates
**
*/
std::vector<CEntryEvent*>* CItemEvent::HitTest(int x, int y, CFileTime* startTime)
{
	const SYSTEMTIME& sysTime = CCalendarWindow::c_MonthsFirstDate.GetSystemTime();
	CItemDays* days = GetRainlendar()->GetCalendarWindow().GetItemDays();

	int thisMonth = sysTime.wMonth;
	int thisYear = sysTime.wYear;

	int startMonth = thisMonth - CConfig::Instance().GetPreviousMonths();
	int startYear = thisYear;

	if (CConfig::Instance().GetStartFromJanuary())
	{
		startMonth = 1;
		startYear = thisYear;
	}

	while (startMonth <= 0)
	{
		startYear--;
		startMonth += 12;
	}

	POINT offset;

	int w = GetRainlendar()->GetCalendarWindow().GetWidth() / CConfig::Instance().GetHorizontalCount();
	int h = GetRainlendar()->GetCalendarWindow().GetHeight() / CConfig::Instance().GetVerticalCount();

	if (w == 0 || h == 0)
	{
		return NULL;
	}

	offset.x = (x / w) * w;
	offset.y = (y / h) * h;

	// Mitul{
	if (CConfig::Instance().GetGridMonth((y / h), (x / w), startMonth, startYear) < 0) return NULL;

	/*
	startMonth += (y / h) * CConfig::Instance().GetHorizontalCount() + (x / w);
	while (startMonth > 12)
	{
		startYear++;
		startMonth -= 12;
	}
	*/
	// Mitul}

	CFileTime monthDate(1, startMonth, startYear);
	int NumOfDays = monthDate.GetDaysInMonth();

	x -= offset.x;
	y -= offset.y;

	int X, Y, W, H, Day;

	X = days->GetDaysX();
	Y = days->GetDaysY();

	x -= X;
	y -= Y;

	if (days->GetDaysLayout() == CConfig::DAY_LAYOUT_HORIZONTAL)
	{
		W = days->GetDaysW() / 31;
		H = days->GetDaysH();

		if(x < 0 || x > (W * 31) || y < 0 || y > H) return NULL;	// No Hit

		Day = x / W + 1;
	}
	else if (days->GetDaysLayout() == CConfig::DAY_LAYOUT_VERTICAL)
	{
		W = days->GetDaysW();
		H = days->GetDaysH() / 31;

		if(x < 0 || x > W || y < 0 || y > (H * 31)) return NULL;	// No Hit

		Day = y / H + 1;
	}
	else
	{
		int FirstWeekday;

		W = days->GetDaysW() / 7;	// 7 Columns
		H = days->GetDaysH() / 6;	// 6 Rows

		FirstWeekday = monthDate.GetWeekday();

		if(CConfig::Instance().GetStartFromMonday()) 
		{
			FirstWeekday = (FirstWeekday - 1);
			if(FirstWeekday == -1) FirstWeekday = 6;
		} 

		if(x < 0 || x > (W * 7) || y < 0 || y > (H * 6)) return NULL;	// No Hit

		Day = (y / H) * 7 + (x / W);
		Day = (Day + 1) - FirstWeekday;
	}

	if(NumOfDays == 0) 
	{
		if(Day < 1 || Day > 31) return NULL;	// No hit
	}
	else 
	{
		if(Day < 1 || Day > NumOfDays) return NULL;	// No hit
	}

	if (startTime)
	{
		*startTime = CFileTime(Day, startMonth, startYear);
	}

	CEventManager& manager = GetRainlendar()->GetCalendarWindow().GetEventManager();
	return &manager.GetEvents(Day, startMonth, startYear, EVENT_FLAG_NO_DELETED);
}

int CItemEvent::GetDayType(int day, int month, int year, bool ignoreAllwaysDrawn)
{
	int type = DAYTYPE_NORMAL;

	std::vector<CEntryEvent*> eventList = GetRainlendar()->GetCalendarWindow().GetEventManager().GetEvents(day, month, year, EVENT_FLAG_NO_DELETED | EVENT_FLAG_NO_CALENDAR_HIDDEN);
	if (!eventList.empty())
	{
		if (ignoreAllwaysDrawn)
		{
			std::vector<CEntryEvent*>::iterator iter = eventList.begin();
			for( ; iter != eventList.end(); iter++)
			{
				// Ignore always drawn events
				const Profile* profile = CConfig::Instance().GetProfile((*iter)->GetProfile());
				
				if (profile == NULL || !profile->drawAlways)
				{
					type = DAYTYPE_EVENT;
					break;
				}
			}
		}
		else
		{
			type = DAYTYPE_EVENT;
		}
	}

	// If this month is shown, mark today
	const SYSTEMTIME& sysTime = CCalendarWindow::c_TodaysDate.GetSystemTime();
	if (sysTime.wMonth == month && sysTime.wDay == day && sysTime.wYear == year) 
	{
		type |= DAYTYPE_TODAY;
	}

	return type;
}