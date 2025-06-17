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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/ItemEvent.cpp,v 1.16 2003/10/04 14:50:41 Rainy Exp $

  $Log: ItemEvent.cpp,v $
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
#include "Tooltip.h"
#include <set>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemEvent::CItemEvent()
{
	m_EventFont = NULL;
}

CItemEvent::~CItemEvent()
{
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
			if(BMRast==NULL) THROW(ERR_OUTOFMEM);

			BMRast->Load(CCalendarWindow::c_Config.GetEventBitmapName());
			BMRast->SetNumOfComponents(CCalendarWindow::c_Config.GetEventNumOfComponents());
			BMRast->SetSeparation(CCalendarWindow::c_Config.GetEventSeparation());
			BMRast->SetAlign(CCalendarWindow::c_Config.GetEventAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) THROW(ERR_OUTOFMEM);

			FNRast->SetFont(CCalendarWindow::c_Config.GetEventFont());
			FNRast->SetAlign(CCalendarWindow::c_Config.GetEventAlign());
			FNRast->SetColor(CCalendarWindow::c_Config.GetEventFontColor());
			FNRast->UpdateDimensions("XX");
			SetRasterizer(FNRast);
			break;
		}

		if(CCalendarWindow::c_Config.GetEventInCalendar())
		{
			SetFont(CCalendarWindow::c_Config.GetEventFont2());
		}

		// Check for the events
		m_EventManager.ReadEvents();
	}
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

	if(m_Rasterizer!=NULL) 
	{
		for(i = 0; i < NumOfDays; i++) 
		{
			Index = i + FirstWeekday;
			DayType = GetDayType(i + 1, CCalendarWindow::c_MonthsFirstDate.wMonth, CCalendarWindow::c_MonthsFirstDate.wYear);

			X = CCalendarWindow::c_Config.GetDaysX() + (Index % 7) * W;
			Y = CCalendarWindow::c_Config.GetDaysY() + (Index / 7) * H;

			X += offset.x;
			Y += offset.y;

			// Set the correct profile
			const Profile* profile = GetEventProfile(i + 1);

			if (DayType & EVENT)
			{
				if (!((DayType & TODAY) && CCalendarWindow::c_Config.GetDaysIgnoreToday()))
				{
					m_Rasterizer->SetProfile(profile);
					m_Rasterizer->Paint(background, X, Y, W, H, i + 1);
				}
			}

			std::set<const Profile*> drawnProfiles;
			drawnProfiles.insert(profile);

			// Loop the events and check if there is an DrawAlways events and draw them
			std::vector<CEventMessage*>::iterator eventIter;
			std::vector<CEventMessage*> eventList = m_EventManager.GetEvents(i + 1, CCalendarWindow::c_MonthsFirstDate.wMonth, CCalendarWindow::c_MonthsFirstDate.wYear);
			for(eventIter = eventList.begin();  eventIter != eventList.end(); eventIter++)
			{
				if (!((*eventIter)->GetProfile().empty()) && !((*eventIter)->IsDeleted()))
				{
					const Profile* newProfile = CCalendarWindow::c_Config.GetProfile((*eventIter)->GetProfile().c_str());

					// Draw only once per profile
					if (newProfile && newProfile->drawAlways && drawnProfiles.find(newProfile) == drawnProfiles.end())
					{
						m_Rasterizer->SetProfile(newProfile);
						m_Rasterizer->Paint(background, X, Y, W, H, i + 1);
						drawnProfiles.insert(newProfile);
					}
				}
			}

			if (DayType & EVENT)
			{
				// Draw the icon if there is one
				DrawIcon(background, i + 1, X, Y, W, H);

				// Draw the event texts
				if(CCalendarWindow::c_Config.GetEventInCalendar()) 
				{
					HDC dc = CreateCompatibleDC(NULL);
					HFONT OldFont;
					HBITMAP OldBitmap;
					int count = 0, width = 0, height = 0;
					RECT rect;
					std::vector<int> heights;
					std::vector<CEventMessage*>::iterator eventIter;
					std::vector<CEventMessage*> eventList = m_EventManager.GetEvents(i + 1, CCalendarWindow::c_MonthsFirstDate.wMonth, CCalendarWindow::c_MonthsFirstDate.wYear);

					OldFont = (HFONT)SelectObject(dc, m_EventFont);
					OldBitmap = (HBITMAP)SelectObject(dc, background.GetBitmap());

					// First calculate the size of the texts
					for(eventIter = eventList.begin();  eventIter != eventList.end(); eventIter++)
					{
						if (!((*eventIter)->GetMessage().empty()) && !((*eventIter)->IsDeleted()))
						{
							DrawText(dc, (*eventIter)->GetMessage().c_str(), (*eventIter)->GetMessage().size(), &rect, DT_CENTER | DT_NOPREFIX | DT_CALCRECT);
							height += rect.bottom - rect.top;
							width = max (width, rect.right - rect.left);
							heights.push_back(rect.bottom - rect.top);
						}
					}

					SetBkMode(dc, TRANSPARENT);

					rect.top = Y + H / 2 - height / 2;
					rect.bottom = rect.top + height;
					rect.left = X + W / 2 - width / 2;
					rect.right = rect.left + width;

					// Then draw them to the window
					for(eventIter = eventList.begin();  eventIter != eventList.end(); eventIter++)
					{
						if (!((*eventIter)->GetMessage().empty()))
						{
							const Profile* profile = CCalendarWindow::c_Config.GetProfile((*eventIter)->GetProfile().c_str());

							if (profile)
							{
								SetTextColor(dc, profile->fontColor2);
							}
							else
							{
								SetTextColor(dc, CCalendarWindow::c_Config.GetEventFontColor2());
							}

							if (CCalendarWindow::Is2k() && CCalendarWindow::c_Config.GetNativeTransparency())
							{
								// The font rasterizer has a buffer for us, where we can draw the text
								CRasterizerFont::CreateBuffers(background.GetWidth(), background.GetHeight());

								SelectObject(dc, CRasterizerFont::GetColorBuffer().GetBitmap());
								DrawText(dc, (*eventIter)->GetMessage().c_str(), (*eventIter)->GetMessage().size(), &rect, DT_CENTER | DT_NOPREFIX);
								
								SelectObject(dc, CRasterizerFont::GetAlphaBuffer().GetBitmap());
								SetTextColor(dc, RGB(255, 255, 255));
								DrawText(dc, (*eventIter)->GetMessage().c_str(), (*eventIter)->GetMessage().size(), &rect, DT_CENTER | DT_NOPREFIX);
							}
							else
							{
								// Just draw it normally
								DrawText(dc, (*eventIter)->GetMessage().c_str(), (*eventIter)->GetMessage().size(), &rect, DT_CENTER | DT_NOPREFIX);
							}

							rect.top += heights[count];
							count++;
						}
					}

					SelectObject(dc, OldBitmap);
					SelectObject(dc, OldFont);
					DeleteDC(dc);
				}
			}
		}
	}
}

void CItemEvent::DrawIcon(CImage& background, int day, int X, int Y, int W, int H)
{
	// Draw all icons 
	std::vector<CEventMessage*> eventList = m_EventManager.GetEvents(day, CCalendarWindow::c_MonthsFirstDate.wMonth, CCalendarWindow::c_MonthsFirstDate.wYear);
	
	if (!eventList.empty())
	{
		std::vector<CEventMessage*>::iterator i = eventList.begin();
		for( ;  i != eventList.end(); i++)
		{
			int x = X;
			int y = Y;

			if (!((*i)->GetProfile().empty()) && !((*i)->IsDeleted()))
			{
				const Profile* profile = CCalendarWindow::c_Config.GetProfile((*i)->GetProfile().c_str());

				if (profile  && profile->icon.GetBitmap() != NULL)
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
}

/* 
** AddToolTips
**
** Adds tooltips for the current month
**
*/
void CItemEvent::AddToolTips(CCalendarWindow* CalendarWnd, POINT offset)
{
	int FirstWeekday;
	int X, Y, W, H, i, j, Day;
	RECT Rect;

	W = CCalendarWindow::c_Config.GetDaysW() / 7;	// 7 Columns
	H = CCalendarWindow::c_Config.GetDaysH() / 6;	// 6 Rows
	X = CCalendarWindow::c_Config.GetDaysX();
	Y = CCalendarWindow::c_Config.GetDaysY();

	X += offset.x;
	Y += offset.y;

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
				if(CCalendarWindow::c_Config.GetEventEnable() && CCalendarWindow::c_Config.GetEventToolTips()) 
				{
					std::vector<CEventMessage*> eventList = m_EventManager.GetEvents(Day, CCalendarWindow::c_MonthsFirstDate.wMonth, CCalendarWindow::c_MonthsFirstDate.wYear);

					if (!eventList.empty())
					{
						int ID = CToolTip::Instance().CreateToolTip(Rect);

						std::vector<CEventMessage*>::iterator eventIter = eventList.begin();
						for( ;  eventIter != eventList.end(); eventIter++)
						{
							if (!((*eventIter)->GetMessage().empty()) && !((*eventIter)->IsDeleted()))
							{
								const Profile* profile = CCalendarWindow::c_Config.GetProfile((*eventIter)->GetProfile().c_str());

								CToolTip::ToolTipData ttd;

								if (profile)
								{
									ttd.color = profile->toolTipColor;
								}
								else
								{
									ttd.color = CCalendarWindow::c_Config.GetToolTipFontColor();
								}

								ttd.text = (*eventIter)->GetMessage();
								CToolTip::Instance().AddData(ID, ttd);
							}
						}
					}
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
** GetEventProfile
**
** Returns the event profile for the given day in current month.
** If there are several events on the same day the priority is used
** to determine which one is shown.
**
*/
const Profile* CItemEvent::GetEventProfile(int day)
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

	std::vector<CEventMessage*> eventList = m_EventManager.GetEvents(day, CCalendarWindow::c_MonthsFirstDate.wMonth, CCalendarWindow::c_MonthsFirstDate.wYear);
	
	if (!eventList.empty())
	{
		std::vector<CEventMessage*>::iterator i = eventList.begin();
		for( ;  i != eventList.end(); i++)
		{
			if (!((*i)->GetProfile().empty()) && !((*i)->IsDeleted()))
			{
				const Profile* profile = CCalendarWindow::c_Config.GetProfile((*i)->GetProfile().c_str());

				if (profile)
				{
					if (profile->drawAlways)	// Ignore always drawn profiles
					{
						continue;
					}
					profilePriority = profile->priority;
				}
				else
				{
					profilePriority = 0;
				}

				switch((*i)->GetType())
				{
				case EVENT_SINGLE:
					newPriority = singlePriority + profilePriority;
					break;

				case EVENT_DAILY:
					newPriority = dailyPriority + profilePriority;
					break;

				case EVENT_WEEKLY:
					newPriority = weeklyPriority + profilePriority;
					break;

				case EVENT_MONTHLY:
					newPriority = monthlyPriority + profilePriority;
					break;

				case EVENT_ANNUALLY:
					newPriority = annuallyPriority + profilePriority;
					break;
				}

				if (newPriority >= currentPriority)
				{
					currentProfile = (*i)->GetProfile().c_str();
					currentPriority = newPriority;
				}
			}
		}

		return CCalendarWindow::c_Config.GetProfile(currentProfile);
	}

	return NULL;
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
	if (m_EventFont) DeleteObject(m_EventFont);
	m_EventFont = CRasterizerFont::CreateFont(FontName);
}