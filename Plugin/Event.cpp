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
  $Header: /home/cvsroot/Rainlendar/Plugin/Event.cpp,v 1.10 2004/01/10 15:16:05 rainy Exp $

  $Log: Event.cpp,v $
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
  Changed CEvent to CEventMessage to avoid name clash

  Revision 1.2  2002/01/10 16:46:56  rainy
  Now holds the bitmap and color too.

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#include "RainlendarDLL.h"
#include "Event.h"
#include <math.h>
#include <time.h>

int CEventMessage::c_CurrentID = 1;

CEventMessage::CEventMessage(bool Permanent)
{
	m_ValidUntil = 0;
	m_EveryNth = 1;
	m_Type = EVENT_SINGLE;
	m_Date = 0;
	m_Count = 1;
	m_ID = c_CurrentID++;
	m_Deleted = false;
	m_Permanent = Permanent;
	m_Profile = "Default";
	m_StartTime = -1;		// This means that there is no start time
	m_Shown = false;
    UpdateTimeStamp();
}

void CEventMessage::UpdateTimeStamp()
{
	time(&m_TimeStamp);
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
	m_Permanent = event.m_Permanent;
	m_TimeStamp = event.m_TimeStamp;
	m_StartTime = event.m_StartTime;
	m_Shown = event.m_Shown;

	return *this;
}

const char* CEventMessage::GetTypeText() const
{
	switch(m_Type)
	{
	case EVENT_SINGLE:
		return CCalendarWindow::c_Language.GetString("EventGUI", 2);

	case EVENT_DAILY:
		return CCalendarWindow::c_Language.GetString("EventGUI", 3);

	case EVENT_WEEKLY:
		return CCalendarWindow::c_Language.GetString("EventGUI", 4);

	case EVENT_MONTHLY:
		return CCalendarWindow::c_Language.GetString("EventGUI", 5);

	case EVENT_ANNUALLY:
		return CCalendarWindow::c_Language.GetString("EventGUI", 6);
	}

	return "Unknown";	// What the...
}

const char* CEventMessage::GetDateText() const
{
	static char buffer[256];
	int day, month, year;

	ValueToDate(m_Date, &day, &month, &year);
	sprintf(buffer, "%02i-%02i-%04i", day, month, year);

	return buffer;
}

void CEventMessage::SetMessage(const std::string& Message )
{ 
	m_Message = Message; 
	// Trim the message
	m_Message.erase(m_Message.find_last_not_of(" ") + 1);

	// Parse the start time
	int hour = -1, min = -1;
	sscanf(m_Message.c_str(), "%i:%i", &hour, &min);
	
	if (hour != -1 && min != -1)
	{
		int pos = m_Message.find_first_not_of(" 0123456789:");
		if (pos != -1) 
		{
			if (strnicmp(m_Message.c_str() + pos, "AM", 2) == 0) 
			{
				if (hour == 12) hour = 0;
			}
			else if (strnicmp(m_Message.c_str() + pos, "PM", 2) == 0) 
			{
				if (hour == 12) hour = 0;
				hour += 12;
			}
		}

		m_StartTime = hour * 60 + min;
	}

    UpdateTimeStamp();
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

	*day = max(1, *day);
	*day = min(31, *day);

	*month = max(1, *month);
	*month = min(12, *month);
}

void CEventMessage::GetDate(double dtSrc, int *year, int *month, int *day, int *hour, int *minute, int *second)
{
    const double HALF_SECOND = (1.0/172800.0); // Half a second, expressed in days
    const int rgMonthDays[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365}; // Days per month

    long nDays;         // Number of days since Dec. 30, 1899
    long nDaysAbsolute; // Number of days since 1/1/0
    long nSecsInDay;    // Time in seconds since midnight
    long nMinutesInDay; // Minutes in day
    
    long n400Years;     // Number of 400 year increments since 1/1/0
    long n400Century;   // Century within 400 year block (0,1,2 or 3)
    long n4Years;       // Number of 4 year increments since 1/1/0
    long n4Day;         // Day within 4 year block
    //  (0 is 1/1/yr1, 1460 is 12/31/yr4)
    long n4Yr;          // Year within 4 year block (0,1,2 or 3)
    bool bLeap4 = true; // TRUE if 4 year block includes leap year
    
    // values in terms of year month date.
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    
    double dblDate = dtSrc; // temporary serial date
    
    // If a valid date, then this conversion should not overflow
    nDays = (long)dblDate;
    
    // Round to the second
    dblDate += ((dtSrc > 0.0) ? HALF_SECOND : -HALF_SECOND);
    
    // Add days from 1/1/0 to 12/30/1899
    nDaysAbsolute = (long)dblDate + 693959;
    
    dblDate = fabs(dblDate);
    nSecsInDay = (long)((dblDate - floor(dblDate)) * 86400.);
    
    // Leap years every 4 yrs except centuries not multiples of 400.
    n400Years = nDaysAbsolute / 146097;
    
    // Set nDaysAbsolute to day within 400-year block
    nDaysAbsolute %= 146097;
    
    // -1 because first century has extra day
    n400Century = (nDaysAbsolute - 1) / 36524;
    
    // Non-leap century
    if (n400Century != 0)
    {
        // Set nDaysAbsolute to day within century
        nDaysAbsolute = (nDaysAbsolute - 1) % 36524;
        
        // +1 because 1st 4 year increment has 1460 days
        n4Years = (nDaysAbsolute + 1) / 1461;
        
        if (n4Years != 0)
            n4Day = (nDaysAbsolute + 1) % 1461;
        else
        {
            bLeap4 = false;
            n4Day = nDaysAbsolute;
        }
    }
    else
    {
        // Leap century - not special case!
        n4Years = nDaysAbsolute / 1461;
        n4Day = nDaysAbsolute % 1461;
    }
    
    if (bLeap4)
    {
        // -1 because first year has 366 days
        n4Yr = (n4Day - 1) / 365;
        
        if (n4Yr != 0)
            n4Day = (n4Day - 1) % 365;
    }
    else
    {
        n4Yr = n4Day / 365;
        n4Day %= 365;
    }
    
    tm_year = (int)(n400Years * 400 + n400Century * 100 + n4Years * 4 +
        n4Yr);
    
    // Handle leap year: before, on, and after Feb. 29.
    if (n4Yr == 0 && bLeap4 && n4Day == 59)
    {
        /* Feb. 29 */
        tm_mon = 2;
        tm_mday = 29;
    }
    else
    {
        if (n4Yr == 0 && bLeap4 && n4Day >= 59)
            --n4Day;
        
        // Make n4DaY a 1-based day of non-leap year and compute
        //  month/day for everything but Feb. 29.
        ++n4Day;
        
        // Month number always >= n/32, so save some loop time */
        for (tm_mon = (int)((n4Day >> 5) + 1);
        n4Day > rgMonthDays[tm_mon]; tm_mon++);
        
        tm_mday = (int)(n4Day - rgMonthDays[tm_mon-1]);
    }
    
    if (nSecsInDay == 0)
        tm_hour = tm_min = tm_sec = 0;
    else
    {
        tm_sec = (int)(nSecsInDay % 60L);
        nMinutesInDay = nSecsInDay / 60L;
        tm_min = (int)(nMinutesInDay % 60);
        tm_hour = (int)(nMinutesInDay / 60);
    }
    
    *year = tm_year;
    *month = tm_mon;
    *day = tm_mday;
    *hour = tm_hour;
    *minute = tm_min;
    *second = tm_sec;
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
