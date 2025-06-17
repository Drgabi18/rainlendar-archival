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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Event.cpp,v 1.6 2002/08/24 11:12:19 rainy Exp $

  $Log: Event.cpp,v $
  Revision 1.6  2002/08/24 11:12:19  rainy
  Added some trimming support.

  Revision 1.5  2002/08/03 16:17:44  rainy
  Added new variables to support repeating events.

  Revision 1.4  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.3  2002/01/27 16:03:53  rainy
  Changed CEvent to CEventMessage to avoid name clash

  Revision 1.2  2002/01/10 16:46:56  rainy
  Now holds the bitmap and color too.

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#include "RainlendarDLL.h"
#include "Event.h"

int CEventMessage::c_CurrentID = 1;

CEventMessage::CEventMessage()
{
	m_ValidUntil = 0;
	m_EveryNth = 1;
	m_Type = EVENT_SINGLE;
	m_Date = 0;
	m_Count = 1;
	m_ID = c_CurrentID++;
	m_Deleted = false;
	m_Profile = "Default";
}

CEventMessage::~CEventMessage()
{

}

CEventMessage::CEventMessage(const CEventMessage& event)
{
	*this = event;
}

CEventMessage& CEventMessage::operator=(const CEventMessage& event)
{
	m_Message = event.m_Message;
	m_Profile = event.m_Profile;
	m_ValidUntil = event.m_ValidUntil;
	m_EveryNth = event.m_EveryNth;
	m_Date = event.m_Date;
	m_Count = event.m_Count;
	m_ID = event.m_ID;
	m_Type = event.m_Type;
	m_Deleted = event.m_Deleted;

	return *this;
}

const char* CEventMessage::GetTypeText()
{
	switch(m_Type)
	{
	case EVENT_SINGLE:
		return "Single";

	case EVENT_DAILY:
		return "Daily";

	case EVENT_WEEKLY:
		return "Weekly";

	case EVENT_MONTHLY:
		return "Monthly";

	case EVENT_ANNUALLY:
		return "Annually";
	}

	return "Unknown";	// What the...
}

void CEventMessage::SetMessage(const std::string& Message )
{ 
	m_Message = Message; 
	// Trim the message
	m_Message.erase(m_Message.find_last_not_of(" ") + 1);
};

int CEventMessage::DateToValue(int day, int month, int year)
{
	return year * 10000 + month * 100 + day;
}

void CEventMessage::ValueToDate(int value, int* day, int* month, int* year)
{
	*day = value % 100;
	*month = (value / 100) % 100;
	*year = value / 10000;
}

int CEventMessage::CalculateNumOfDays(int startValue, int endValue)
{
	int startDay, startMonth, startYear;
	int endDay, endMonth, endYear;
	bool negative = false;
	int numOfDays = 0;

	if (startValue > endValue)
	{
		// Swap, so that we have only positive results
		int tmp = startValue;
		startValue = endValue;
		endValue = tmp;
		negative = true;
	}

	ValueToDate(startValue, &startDay, &startMonth, &startYear);
	ValueToDate(endValue, &endDay, &endMonth, &endYear);

	// Calculate num of months
	int numOfMonths = (endYear - startYear) * 12 + endMonth - startMonth;

	// Calculate the daycount for the months
	for (int i = 0; i < numOfMonths; i++)
	{
		numOfDays += CItem::GetDaysInMonth(startYear, startMonth);

		startMonth++;
		if (startMonth > 12) 
		{
			startMonth = 1;
			startYear++;
		}
	}

	if (numOfMonths == 0)
	{
		numOfDays = endDay - startDay;
	}
	else
	{
		// Add the number of days for the end month
		numOfDays += endDay - 1;

		// And substract the days from the start month
		numOfDays -= startDay - 1;
	}

	return negative ? -numOfDays : numOfDays;
}
