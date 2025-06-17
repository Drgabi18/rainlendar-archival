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
  $Header: /home/cvsroot/Rainlendar/Plugin/EntryEvent.cpp,v 1.1 2004/11/06 13:38:58 rainy Exp $

  $Log: EntryEvent.cpp,v $
  Revision 1.1  2004/11/06 13:38:58  rainy
  no message

  Revision 1.11  2004/06/10 15:25:58  rainy
  Added limits to dates

  Revision 1.10  2004/01/10 15:16:05  rainy
  The start time is parsed from the message.

  Revision 1.9  2003/06/15 09:46:19  Rainy
  Strings are read from CLanguage class.

  Revision 1.8  2003/05/26 18:44:40  Rainy
  Added more consts.

  Revision 1.7  2003/05/07 19:13:49  rainy
  Added permanent and timestamp.

  Revision 1.6  2002/08/24 11:12:19  rainy
  Added some trimming support.

  Revision 1.5  2002/08/03 16:17:44  rainy
  Added new variables to support repeating events.

  Revision 1.4  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.3  2002/01/27 16:03:53  rainy
  Changed CEvent to CEntryEvent to avoid name clash

  Revision 1.2  2002/01/10 16:46:56  rainy
  Now holds the bitmap and color too.

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/
#pragma warning(disable: 4996)
#pragma warning(disable: 4786)

#include "RainlendarDLL.h"
#include "EntryEvent.h"
#include <math.h>
#include <time.h>
#include <objbase.h>

CEntryEvent::CEntryEvent() : CEntryItem()
{
	m_Item = new RainlendarEvent;
	memset(m_Item, 0, sizeof(RainlendarEvent));
	m_Item->size = sizeof(RainlendarEvent);
	m_Item->type = RAINLENDAR_TYPE_EVENT; 

	CoCreateGuid(&m_Item->guid);	// Create unique guid for the event
	UpdateTimeStamp();
}

CEntryEvent::CEntryEvent(CEntryItem* item)
{
	m_Item = new RainlendarEvent;
	memset(m_Item, 0, sizeof(RainlendarEvent));
	m_Item->type = RAINLENDAR_TYPE_EVENT;
	Rainlendar_CopyItem(item->GetRainlendarItem(), m_Item);
	m_PluginID = item->GetPluginID();
}

CEntryEvent::CEntryEvent(RainlendarItem* item)
{
	m_Item = new RainlendarEvent;
	memset(m_Item, 0, sizeof(RainlendarEvent));
	m_Item->type = RAINLENDAR_TYPE_EVENT;
	Rainlendar_CopyItem(item, m_Item);
	m_PluginID = 0;
}

CEntryEvent::~CEntryEvent()
{
}

CFileTime CEntryEvent::GetBegin()
{
	if (GetRainlendarEvent()->allDayEvent)
	{
		CFileTime time(GetRainlendarEvent()->startTime);
		time.NormalizeDay();
		return time;
	}

	return CFileTime(GetRainlendarEvent()->startTime);
}

CFileTime CEntryEvent::GetEnd()
{
	CFileTime begin(GetRainlendarEvent()->startTime);
	CFileTime end(GetRainlendarEvent()->endTime);

	if (GetRainlendarEvent()->recurrency && GetRainlendarEvent()->recurrency->type != RECURRENCY_TYPE_SINGLE)
	{
		RainlendarRecurrency* recurrency = GetRainlendarEvent()->recurrency;

		if (recurrency->repeatType == RECURRENCY_REPEAT_FOREVER)
		{
			FILETIME ft;
			ft.dwHighDateTime = FOREVER_VALUE;
			ft.dwLowDateTime = FOREVER_VALUE;
			end = CFileTime(ft);
			return end;
		}
		else if (recurrency->repeatType == RECURRENCY_REPEAT_UNTIL)
		{
			end = CFileTime(recurrency->until);
		}
		else
		{
			switch (recurrency->type)
			{
			case RECURRENCY_TYPE_DAILY:
				end.Add(recurrency->frequency * recurrency->count* 24 * 60 * 60);
				break;

			case RECURRENCY_TYPE_WEEKLY:
				end.Add(recurrency->frequency * recurrency->count * 7 * 24 * 60 * 60);
				break;

			case RECURRENCY_TYPE_MONTHLY_NTH_DAY:
			case RECURRENCY_TYPE_MONTHLY_NTH_WEEKDAY:
			case RECURRENCY_TYPE_MONTHLY_LASTWEEK_WEEKDAY:
			case RECURRENCY_TYPE_MONTHLY_LAST_DAY:
			case RECURRENCY_TYPE_MONTHLY_FIRST_WEEKDAY:
			case RECURRENCY_TYPE_MONTHLY_LAST_WEEKDAY:
				end.Add(recurrency->frequency * recurrency->count * 31 * 24 * 60 * 60);
				break;

			case RECURRENCY_TYPE_YEARLY:
				end.Add(recurrency->frequency * recurrency->count * 366 * 24 * 60 * 60);
				break;
			}
		}
	}

	if (end.Compare(begin) < 0)
	{
		return begin;
	}
	else
	{
		return end;
	}
}

void CEntryEvent::GetBriefMessage(std::string& message, int len, bool addTime, bool longFormat)
{
	if (GetRainlendarEvent()->header != NULL && strlen(GetRainlendarEvent()->header) > 0)
	{
		message = GetRainlendarEvent()->header;

		if (longFormat && (GetRainlendarEvent()->message != NULL && strlen(GetRainlendarEvent()->message) > 0))
		{
			message += "\n";
			message += GetRainlendarEvent()->message;
		}
	}
	else
	{
		if (GetRainlendarEvent()->message != NULL)
		{
			message = GetRainlendarEvent()->message;
		}
	}

	if (!message.empty() && addTime)
	{
		if (!GetRainlendarEvent()->allDayEvent)
		{
			bool endTime = false;
			bool dateAndTime = false;
			std::string time;

			if (GetStartTime().Compare(GetEndTime()) < 0)
			{
				endTime = true;
				SYSTEMTIME sysStart = GetStartTime().GetSystemTime();
				SYSTEMTIME sysEnd = GetEndTime().GetSystemTime();
				if ((sysStart.wDay != sysEnd.wDay) ||
				    (sysStart.wMonth != sysEnd.wMonth) ||
				    (sysStart.wYear != sysEnd.wYear))
				{
					dateAndTime = true;
				}
			}

			if (dateAndTime)
			{
				time = GetStartTime().ToTimeAndDateString(true);
			}
			else
			{
				time = GetStartTime().ToTime();
			}

			if (endTime)
			{
				time += " - ";
				if (dateAndTime)
				{
					time += GetEndTime().ToTimeAndDateString(true);
				}
				else
				{
					time += GetEndTime().ToTime();
				}
			}
			message = time + " " + message;
		}
	}

	if (len > 0 && (int)message.size() > len)
	{
		message = message.substr(0, len);
		message += "...";
	}

	if (!longFormat)
	{
		// Remove newlines from the string
		int pos = message.find("\r\n");
		while (pos != -1)
		{
			message.replace(message.begin() + pos, message.begin() + pos + 2, " ");
			pos = message.find("\r\n");
		}
	}
}

void CEntryEvent::SetMessage(LPCTSTR text)
{
	if (GetRainlendarEvent()->message)
	{
		free(GetRainlendarEvent()->message);
		GetRainlendarEvent()->message = NULL;
	}
	if (text)
	{
		GetRainlendarEvent()->message = strdup(text);
	}
}

void CEntryEvent::SetHeader(LPCTSTR text)
{
	if (GetRainlendarEvent()->header)
	{
		free(GetRainlendarEvent()->header);
		GetRainlendarEvent()->header = NULL;
	}
	if (text)
	{
		GetRainlendarEvent()->header = strdup(text);
	}
}

void CEntryEvent::SetProfile(LPCTSTR profile)
{
	if (GetRainlendarEvent()->profile)
	{
		free(GetRainlendarEvent()->profile);
		GetRainlendarEvent()->profile = NULL;
	}
	if (profile)
	{
		GetRainlendarEvent()->profile = strdup(profile);
	}
}

void CEntryEvent::SetRecurrency(RainlendarRecurrency* recurrency)
{
	if (GetRainlendarEvent()->recurrency)
	{
		delete GetRainlendarEvent()->recurrency;
		GetRainlendarEvent()->recurrency = NULL;
	}
	
	if (recurrency)
	{
		RainlendarRecurrency* newRecur = new RainlendarRecurrency;
		*newRecur = *recurrency;
		GetRainlendarEvent()->recurrency = newRecur;
	}
}