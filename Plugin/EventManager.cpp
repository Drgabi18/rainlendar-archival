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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/EventManager.cpp,v 1.5 2003/05/25 18:10:11 Rainy Exp $

  $Log: EventManager.cpp,v $
  Revision 1.5  2003/05/25 18:10:11  Rainy
  The GetEvent() returns now a sorted vector.

  Revision 1.4  2003/05/07 19:11:44  rainy
  The timestamp is not updated every time.

  Revision 1.3  2002/08/24 11:12:15  rainy
  Added some trimming support.

  Revision 1.2  2002/08/10 08:39:51  rainy
  Loading the sections from the ini does not crash anymore if the file is missing.

  Revision 1.1  2002/08/03 16:37:02  rainy
  Intial version.

*/

#include "EventManager.h"
#include "RainlendarDLL.h"
#include <time.h>
#include <set>
#include <algorithm>

CEventManager::CEventManager()
{

}

CEventManager::~CEventManager()
{

}

void CEventManager::ReadEvents()
{
	char tmpSz[MAX_LINE_LENGTH];
	const char* IniPath = CCalendarWindow::c_Config.GetEventsPath().c_str();
	SYSTEMTIME& Current = CCalendarWindow::c_MonthsFirstDate;
	std::string Date;
	char* events = new char[MAX_LINE_LENGTH];
	int size = MAX_LINE_LENGTH;
	int count = 0;

	// Get all the sections
	while(true)
	{
		int res = GetPrivateProfileString( NULL, NULL, NULL, events, size, IniPath);
		if (res == 0) return;		// File not found
		if (res != size - 2) break;		// Fits in the buffer

		delete [] events;
		size *= 2;
		events = new char[size];
	};

	char* pos = events;
	while(strlen(pos) > 0)
	{
		CEventMessage event;

		if (1 == GetPrivateProfileInt(pos, "Deleted", 0, IniPath)) 
		{
			pos = pos + strlen(pos) + 1;
			continue;	// No need to read deleted events
		}

		// Parse the section
		int day, month, year;
		sscanf(pos, "%i-%i-%i", &day, &month, &year);

		event.SetDate(CEventMessage::DateToValue(day, month, year));

		char* pos2 = strchr(pos, ' ');
		if(pos2)
		{
			event.SetCount(atoi(pos2 + 1));
		}
		else
		{
			event.SetCount(1);
		}

		// Get the data
		if(GetPrivateProfileString(pos, "Message", "", tmpSz, MAX_LINE_LENGTH, IniPath) > 0) 
		{
			// Replace \n's
			std::string tmpString = tmpSz;
			
			int n = tmpString.find("\\n");
			while (n != -1)
			{
				tmpString.replace(tmpString.begin() + n, tmpString.begin() + n + 2, "\r\n");
				n = tmpString.find("\\n");
			}

			// Trim the message
			tmpString.erase(tmpString.find_last_not_of(" ") + 1);
			event.SetMessage(tmpString);
		}

		// Profile
		if(GetPrivateProfileString(pos, "Profile", "", tmpSz, MAX_LINE_LENGTH, IniPath) > 0) 
		{
			// Trim the profile
			std::string tmpString = tmpSz;
			tmpString.erase(tmpString.find_last_not_of(" ") + 1);
			event.SetProfile(tmpString);
		}

		// EveryNth
		event.SetEveryNth(GetPrivateProfileInt(pos, "EveryNth", 1, IniPath));

		// ValidUntil
		event.SetValidUntil(GetPrivateProfileInt(pos, "ValidUntil", 0, IniPath));

		// Type
		event.SetType((EVENT_TYPE)GetPrivateProfileInt(pos, "Type", EVENT_SINGLE, IniPath));

		// TimeStamp (this must be the last)
		event.SetTimeStamp(GetPrivateProfileInt(pos, "TimeStamp", 0, IniPath));

		m_AllEvents.push_back(event);

		count++;

		pos = pos + strlen(pos) + 1;
	}

	delete [] events;

	sprintf(tmpSz, "Read %i events from %s", count, IniPath);
	LSLog(LOG_DEBUG, "Rainlendar", tmpSz);
}

bool CompareEvents(const CEventMessage* arg1, const CEventMessage* arg2)
{
   return arg1->GetCount() < arg2->GetCount(); 
}

/*
**
** GetEvents
**
** Gets all events for the given day. This returns even deleted events.
**
*/
std::vector<CEventMessage*> CEventManager::GetEvents(int day, int month, int year)
{
	std::vector<CEventMessage*> eventVector;
	int value = CEventMessage::DateToValue(day, month, year);
	int eventDay, eventMonth, eventYear;
	int days;

	// Check events
	std::list<CEventMessage>::iterator j = m_AllEvents.begin();
	for( ; j != m_AllEvents.end(); j++)
	{
		if ((*j).GetDate() <= value && ((*j).GetValidUntil() >= value || (*j).GetValidUntil() == 0))
		{
			CEventMessage::ValueToDate((*j).GetDate(), &eventDay, &eventMonth, &eventYear);

			switch((*j).GetType())
			{
			case EVENT_SINGLE:
				if (day == eventDay && month == eventMonth && year == eventYear)
				{
					eventVector.push_back(&(*j));
				}
				break;

			case EVENT_DAILY:
				days = CEventMessage::CalculateNumOfDays((*j).GetDate(), value);
				if (days % (*j).GetEveryNth() == 0)
				{
					eventVector.push_back(&(*j));
				}
				break;

			case EVENT_WEEKLY:
				days = CEventMessage::CalculateNumOfDays((*j).GetDate(), value);
				if (days == 0 || (days % 7 == 0 && ((days / 7) % (*j).GetEveryNth() == 0)))
				{
					eventVector.push_back(&(*j));
				}
				break;

			case EVENT_MONTHLY:
				if (eventDay == day)	// If day matches...
				{
					days = (eventYear * 12 + eventMonth) - (year * 12 + month);
					if (days == 0 || days % (*j).GetEveryNth() == 0)
					{
						eventVector.push_back(&(*j));
					}
				}
				break;

			case EVENT_ANNUALLY:
				if (eventDay == day && eventMonth == month)	// If day & month matches...
				{
					if ((eventYear - year) % (*j).GetEveryNth() == 0)
					{
						eventVector.push_back(&(*j));
					}
				}
				break;
			}
		}
	}

	// Sort the vector so that the count increases
	std::sort(eventVector.begin(), eventVector.end(), CompareEvents);

	return eventVector;
}

CEventMessage* CEventManager::GetEvent(int ID)
{
	// Check the events
	std::list<CEventMessage>::iterator j;
	for(j = m_AllEvents.begin() ; j != m_AllEvents.end(); j++)
	{
		if(ID == (*j).GetID())
		{
			return &(*j);
		}
	}

	return NULL;
}

void CEventManager::RemoveEvent(CEventMessage& event)
{
	CEventMessage* oldEvent = GetEvent(event.GetID());

	if (oldEvent)
	{
		oldEvent->SetDeleted();
	}
}

void CEventManager::AddEvent(CEventMessage& event)
{
	// The date and type must be set before calling this!

	// First check if the event is already in the manager
	CEventMessage* oldEvent = GetEvent(event.GetID());

	if (oldEvent && !oldEvent->IsDeleted())
	{
		// Update the old event
		*oldEvent = event;
		return;
	}

	// If the event is a new one add it to the proper place
	std::set<int> counts;
	
	std::list<CEventMessage>::iterator j;
	for(j = m_AllEvents.begin() ; j != m_AllEvents.end(); j++)
	{
		if ((*j).GetDate() == event.GetDate())
		{
			if (!((*j).IsDeleted()))
			{
				counts.insert((*j).GetCount());
			}
		}
	}

	// Scan the found counts and use the first available
	int newCount = 1;
	while (counts.find(newCount) != counts.end())
	{
		newCount++;
	}

	event.SetCount(newCount);

	m_AllEvents.push_back(event);
}

void CEventManager::WriteEvents(int day, int month, int year)
{
	std::vector<CEventMessage*> eventList = GetEvents(day, month, year);

	std::vector<CEventMessage*>::iterator i = eventList.begin();
	for( ; i != eventList.end(); i++)
	{
		WriteEvent(*(*i));
	}
}

void CEventManager::WriteEvent(CEventMessage& event)
{
	char tmpSz[MAX_LINE_LENGTH];
	char Date[MAX_LINE_LENGTH];
	std::string message = event.GetMessage();
	const char* IniPath = CCalendarWindow::c_Config.GetEventsPath().c_str();
	int day, month, year;

   	if (!event.IsPermanent()) return; // No need to write read only events

	CEventMessage::ValueToDate(event.GetDate(), &day, &month, &year);
	sprintf(Date, "%i-%i-%i", day, month, year);

	int len = strlen(Date);

	if(event.GetCount() != 1) 
	{
		strcat(Date, " ");
		itoa(event.GetCount(), Date + len + 1, 10);
	}

	if (event.IsDeleted())
	{
		// Remove all entries	
		WritePrivateProfileString( Date, "Type", NULL, IniPath );
		WritePrivateProfileString( Date, "Message", NULL, IniPath );
		WritePrivateProfileString( Date, "Profile", NULL, IniPath );
		WritePrivateProfileString( Date, "ValidUntil", NULL, IniPath );
		WritePrivateProfileString( Date, "EveryNth", NULL, IniPath );
	}
	else
	{
		int pos = message.find("\r\n");
		while (pos != -1)
		{
			message.replace(message.begin() + pos, message.begin() + pos + 2, "\\n");
			pos = message.find("\r\n");
		}

		sprintf(tmpSz, "%i", event.GetType());
		WritePrivateProfileString( Date, "Type", tmpSz, IniPath );
		WritePrivateProfileString( Date, "Message", message.c_str(), IniPath );
		WritePrivateProfileString( Date, "Profile", event.GetProfile().c_str(), IniPath );
		sprintf(tmpSz, "%i", event.GetValidUntil());
		WritePrivateProfileString( Date, "ValidUntil", tmpSz, IniPath );
		sprintf(tmpSz, "%i", event.GetEveryNth());
		WritePrivateProfileString( Date, "EveryNth", tmpSz, IniPath );
	}
	WritePrivateProfileString( Date, "Deleted", event.IsDeleted()?"1":"0", IniPath );

	// Write timestamp to know when this was last modified
	sprintf(tmpSz, "%ld", event.GetTimeStamp());
	WritePrivateProfileString( Date, "TimeStamp", tmpSz, IniPath );
}
