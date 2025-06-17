/*
  Copyright (C) 2004 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Library/FileTime.cpp,v 1.2 2005/09/08 16:09:12 rainy Exp $

  $Log: FileTime.cpp,v $
  Revision 1.2  2005/09/08 16:09:12  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.4  2005/03/25 13:58:43  rainy
  no message

  Revision 1.3  2005/03/01 18:50:45  rainy
  no message

  Revision 1.2  2004/12/05 18:31:28  rainy
  Fixed Compare.

  Revision 1.1  2004/11/06 13:38:59  rainy
  no message

*/
#pragma warning(disable: 4996)
#pragma warning(disable: 4786)

#include "FileTime.h"
#include "Litestep.h"
#include "CalendarWindow.h"
#include "TimeZones.h"
#include <tchar.h>
#include <stdio.h>
#include <math.h>

CFileTime::CFileTime()
{
	m_FileTime.dwLowDateTime = 0;
	m_FileTime.dwHighDateTime = 0;
	m_Valid = false;
}

CFileTime::CFileTime(FILETIME time)
{
	m_FileTime = time;
	m_Valid = true;
}

CFileTime::CFileTime(time_t t)
{
    ULONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
    m_FileTime.dwLowDateTime = (DWORD)ll;
    m_FileTime.dwHighDateTime = (DWORD)(ll >> 32);
	m_Valid = true;
}

CFileTime::CFileTime(int day, int month, int year)
{
	SYSTEMTIME sysTime;
	memset(&sysTime, 0, sizeof(SYSTEMTIME));

	sysTime.wDay = day;
	sysTime.wMonth = month;
	sysTime.wYear = year;

	m_Valid = SystemTimeToFileTime(&sysTime, &m_FileTime) != 0;
}

CFileTime::CFileTime(int day, int month, int year, int hour, int minute, int second)
{
	SYSTEMTIME sysTime;
	memset(&sysTime, 0, sizeof(SYSTEMTIME));

	sysTime.wDay = day;
	sysTime.wMonth = month;
	sysTime.wYear = year;
	sysTime.wHour = hour;
	sysTime.wMinute = minute;
	sysTime.wSecond = second;

	m_Valid = SystemTimeToFileTime(&sysTime, &m_FileTime) != 0;
}

CFileTime::CFileTime(SYSTEMTIME& sysTime)
{
	m_Valid = SystemTimeToFileTime(&sysTime, &m_FileTime) != 0;
}

CFileTime::~CFileTime()
{
}

time_t CFileTime::GetTime_t()
{
	ULARGE_INTEGER value;
	value.LowPart = m_FileTime.dwLowDateTime;
	value.HighPart = m_FileTime.dwHighDateTime;

	value.QuadPart -= 116444736000000000;
	value.QuadPart /= 10000000;

	return (time_t)value.QuadPart;
}

void CFileTime::SetToLocalTime()
{
	FILETIME time;
	GetSystemTimeAsFileTime(&time);
	m_Valid = FileTimeToLocalFileTime(&time, &m_FileTime) != 0;
}

void CFileTime::Add(const CFileTime& time)
{
	ULARGE_INTEGER value1, value2;

	value1.LowPart = m_FileTime.dwLowDateTime;
	value1.HighPart = m_FileTime.dwHighDateTime;
	value2.LowPart = time.m_FileTime.dwLowDateTime;
	value2.HighPart = time.m_FileTime.dwHighDateTime;

	value1.QuadPart += value2.QuadPart;

	m_FileTime.dwLowDateTime = value1.LowPart;
	m_FileTime.dwHighDateTime = value1.HighPart;
}

void CFileTime::Substract(const CFileTime& time)
{
	ULARGE_INTEGER value1, value2;

	value1.LowPart = m_FileTime.dwLowDateTime;
	value1.HighPart = m_FileTime.dwHighDateTime;
	value2.LowPart = time.m_FileTime.dwLowDateTime;
	value2.HighPart = time.m_FileTime.dwHighDateTime;

	value1.QuadPart -= value2.QuadPart;

	m_FileTime.dwLowDateTime = value1.LowPart;
	m_FileTime.dwHighDateTime = value1.HighPart;
}

void CFileTime::Add(const ULONGLONG secs)
{
	ULARGE_INTEGER value1;

	value1.LowPart = m_FileTime.dwLowDateTime;
	value1.HighPart = m_FileTime.dwHighDateTime;

	value1.QuadPart += secs * 10000000i64;

	m_FileTime.dwLowDateTime = value1.LowPart;
	m_FileTime.dwHighDateTime = value1.HighPart;
}

void CFileTime::Substract(const ULONGLONG secs)
{
	ULARGE_INTEGER value1;

	value1.LowPart = m_FileTime.dwLowDateTime;
	value1.HighPart = m_FileTime.dwHighDateTime;

	value1.QuadPart -= secs * 10000000i64;

	m_FileTime.dwLowDateTime = value1.LowPart;
	m_FileTime.dwHighDateTime = value1.HighPart;
}

void CFileTime::NormalizeDay()
{
	ULARGE_INTEGER value1;

	value1.LowPart = m_FileTime.dwLowDateTime;
	value1.HighPart = m_FileTime.dwHighDateTime;

	value1.QuadPart /= 10000000;
	value1.QuadPart = value1.QuadPart / (60 * 60 * 24);
	value1.QuadPart *= 60 * 60 * 24;
	value1.QuadPart *= 10000000;

	m_FileTime.dwLowDateTime = value1.LowPart;
	m_FileTime.dwHighDateTime = value1.HighPart;
}

int CFileTime::Compare(CFileTime& date)
{
	// -1 First file time is less than second file time. 
	// 0 First file time is equal to second file time. 
	// +1 First file time is greater than second file time. 

	return CompareFileTime(&m_FileTime, &date.m_FileTime);
}

int CFileTime::DifferenceInDays(CFileTime& date)
{
	LARGE_INTEGER value1, value2;

	value1.LowPart = m_FileTime.dwLowDateTime;
	value1.HighPart = m_FileTime.dwHighDateTime;
	value2.LowPart = date.m_FileTime.dwLowDateTime;
	value2.HighPart = date.m_FileTime.dwHighDateTime;

	value2.QuadPart -= value1.QuadPart;
	value1.QuadPart = value2.QuadPart % 864000000000L;
	value2.QuadPart /= 864000000000L;

	if (value1.QuadPart < 0)
	{
		value2.QuadPart--;
	}

	return (int)value2.QuadPart;
}

const SYSTEMTIME& CFileTime::GetSystemTime()
{
	static SYSTEMTIME sysTime;
	if (!FileTimeToSystemTime(&m_FileTime, &sysTime))
	{
		DebugLog("Warning! FileTimeToSystemTime failed (%u:%u).", m_FileTime.dwHighDateTime, m_FileTime.dwLowDateTime);
	}
	return sysTime;
}

int CFileTime::GetWeekday()
{
	const SYSTEMTIME& sysTime = GetSystemTime();
	return sysTime.wDayOfWeek;
}

int CFileTime::GetAsSeconds()
{
	ULARGE_INTEGER value;

	value.LowPart = m_FileTime.dwLowDateTime;
	value.HighPart = m_FileTime.dwHighDateTime;

	return (int)(value.QuadPart / 10000000);
}

int CFileTime::GetOnlyTime()
{
	ULARGE_INTEGER value;

	value.LowPart = m_FileTime.dwLowDateTime;
	value.HighPart = m_FileTime.dwHighDateTime;

	return (int)((value.QuadPart / 10000000) % (24 * 60 * 60));
}

int CFileTime::GetDaysInMonth()
{
	const SYSTEMTIME& sysTime = GetSystemTime();

	UINT dates[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if (sysTime.wMonth == 2 && IsLeapYear(sysTime.wYear))
	{
		return 29;
	}

	return dates[sysTime.wMonth - 1];
}

bool CFileTime::IsLeapYear(int y)
{
	if ( ( ((y % 4) == 0) && ((y % 100) != 0) ) || ((y % 400) == 0) )
		return true;
	return false;
}

bool CFileTime::IsLeapYear()
{
	const SYSTEMTIME& sysTime = GetSystemTime();

	if (sysTime.wMonth == 2 && IsLeapYear(sysTime.wYear))
	{
		return true;
	}
	return false;
}

int CFileTime::GetMonthsFirstWeekday()
{
	const SYSTEMTIME& sysTime = GetSystemTime();

	LONGLONG deltaDays = sysTime.wDay - 1;
	deltaDays *= 24 * 60 * 60;

	CFileTime monthsFirst(m_FileTime);
	monthsFirst.Substract(deltaDays);

	return monthsFirst.GetSystemTime().wDayOfWeek;
}

LPCTSTR CFileTime::ToString()
{
	static TCHAR buffer[256];
	_stprintf(buffer, _T("%u:%u"), m_FileTime.dwHighDateTime, m_FileTime.dwLowDateTime);
	return buffer;
}

bool CFileTime::FromString(LPCTSTR str)
{
	return 2 == _stscanf(str, _T("%u:%u"), &m_FileTime.dwHighDateTime, &m_FileTime.dwLowDateTime);
}

LPCTSTR CFileTime::ToTime()
{
	static TCHAR buffer[256];
	const SYSTEMTIME& sysTime = GetSystemTime();
	GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &sysTime, NULL, buffer, 256);
	return buffer;
}

LPCTSTR CFileTime::ToDate()
{
	if (m_FileTime.dwHighDateTime == FOREVER_VALUE && m_FileTime.dwLowDateTime == FOREVER_VALUE)
	{
		return CCalendarWindow::c_Language.GetString("EventGUI", 53);	// "Forever"
	}

	static TCHAR buffer[256];
	const SYSTEMTIME& sysTime = GetSystemTime();
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, NULL, buffer, 256);
	return buffer;
}

bool CFileTime::FromTime(LPCTSTR str)
{
	SYSTEMTIME sysTime = GetSystemTime();

	bool ok = false;
	int hour = 0, min = 0;
	std::string tmpString = str;

	// Extract the time
	int pos = tmpString.find_first_not_of(" 0123456789");
	if (pos != -1)
	{
		hour = atoi(tmpString.c_str());
		min = atoi(tmpString.substr(pos + 1).c_str());
	}

	pos = tmpString.find_first_not_of(" 0123456789:.");
	if (pos != -1) 
	{
		if (strnicmp(tmpString.c_str() + pos, "AM", 2) == 0) 
		{
			if (hour == 12) hour = 0;
			pos += 2;
		}
		else if (strnicmp(tmpString.c_str() + pos, "PM", 2) == 0) 
		{
			if (hour == 12) hour = 0;
			hour += 12;
			pos += 2;
		}
	}
	sysTime.wHour = hour;
	sysTime.wMinute = min;
	
	SystemTimeToFileTime(&sysTime, &m_FileTime);
	return false;
}

LPCTSTR CFileTime::ToTimeAndDateString(bool noYear)
{
	if (m_FileTime.dwHighDateTime == FOREVER_VALUE && m_FileTime.dwLowDateTime == FOREVER_VALUE)
	{
		return CCalendarWindow::c_Language.GetString("EventGUI", 53);	// "Forever"
	}

	static TCHAR buffer[256];
	const SYSTEMTIME& sysTime = GetSystemTime();

	if (noYear)
	{
		std::string format;
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SSHORTDATE, buffer, 256);
		format = buffer;

		int start = format.find_first_not_of("y");
		int end = format.find_last_not_of("y");
		if (end < format.size() - 1) format.erase(end);
		if (start > 0) format.erase(0, start + 1);

		GetDateFormat(LOCALE_USER_DEFAULT, NULL, &sysTime, format.c_str(), buffer, 256);
	}
	else
	{
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, NULL, buffer, 256);
	}

	strcat(buffer, " ");
	int len = strlen(buffer);
	GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &sysTime, NULL, buffer + len, 256 - len);
	return buffer;
}

// Mitul{

int CFileTime::GetYear()
{
	const SYSTEMTIME& sysTime = GetSystemTime();
	return sysTime.wYear;
}

int CFileTime::DifferenceInYears(CFileTime& dt)
{
	return abs(GetYear() - dt.GetYear());
}

bool CFileTime::IsSameDate(CFileTime& dt)
{
	const SYSTEMTIME& sysTime1 = GetSystemTime();
	const SYSTEMTIME& sysTime2 = dt.GetSystemTime();

	return ((sysTime1.wYear == sysTime2.wYear) && (sysTime1.wMonth == sysTime2.wMonth) && (sysTime1.wDay == sysTime2.wDay));
}

CFileTime* GetDSTDate(SYSTEMTIME& sysTime, int yr)
{
	CFileTime* ft;
	SYSTEMTIME st;
	if (sysTime.wYear > 0)
	{
		ft = new CFileTime(sysTime.wDay, sysTime.wMonth, yr, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	}
	else
	{
		ft = new CFileTime(1, sysTime.wMonth, yr, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		
		st =  ft->GetSystemTime();
		if (sysTime.wDayOfWeek >= st.wDayOfWeek)
			ft->Add ( (sysTime.wDayOfWeek - st.wDayOfWeek) * 24 * 3600 );
		else
			ft->Add ( (7 - st.wDayOfWeek) * 24 * 3600 );
		
		for (int i=1; i < sysTime.wDay; i++)
			ft->Add(7 * 24 * 3600 );
		
		st =  ft->GetSystemTime();
		while (st.wMonth != sysTime.wMonth)
		{
			ft->Substract(7 * 24 * 3600 );
			st =  ft->GetSystemTime();
		}
	}
	
	return ft;
}

int isDst(SYSTEMTIME& sysTime, TIME_ZONE_INFORMATION& tzInfo)
{
	if ( (tzInfo.DaylightDate.wMonth == 0) || (tzInfo.StandardDate.wMonth == 0)
		|| (tzInfo.DaylightDate.wDay == 0) || (tzInfo.StandardDate.wDay == 0) )
		return 0;

	CFileTime* ftS = GetDSTDate(tzInfo.StandardDate, sysTime.wYear);
	CFileTime* ftD = GetDSTDate(tzInfo.DaylightDate, sysTime.wYear);

	/*
	SYSTEMTIME st = ftS->GetSystemTime();
	DebugLog("Stnadard Time: %i/%i/%i %i:%i:%i %i", st.wMonth,st.wDay,st.wYear,st.wHour,st.wMinute,st.wSecond,st.wDayOfWeek);
	
	st = ftD->GetSystemTime();
	DebugLog("Stnadard Time: %i/%i/%i %i:%i:%i %i", st.wMonth,st.wDay,st.wYear,st.wHour,st.wMinute,st.wSecond,st.wDayOfWeek);
	*/
	
	CFileTime* ft = new CFileTime(sysTime);
	if (ftS->Compare(*ftD) == -1)
	{
		if (!((ft->Compare(*ftS) >= 0) && (ft->Compare(*ftD) == -1)))
			return 2;
		else
			return 1;
	}
	else if (ftS->Compare(*ftD) == 1)
	{
		if ((ft->Compare(*ftD) >= 0) && (ft->Compare(*ftS) == -1))
			return 2;
		else
			return 1;
	}
	else
		return 0;
}

void CFileTime::SetWorldTimeAt(const char* place)
{
	TIME_ZONE_INFORMATION tzInfo;
	TIME_ZONE_INFORMATION tzLocal;
	
	/*
	TzSpecificLocalTimeToSystemTime
	SystemTimeToTzSpecificLocalTime
	GetTimeZoneInformation
	http://support.microsoft.com/kb/221542
	*/

	CTimeZones::Instance().GetTimeZoneInformation(place, tzInfo);
	int dst = GetTimeZoneInformation(&tzLocal);
	if ((&tzInfo) && (dst != TIME_ZONE_ID_INVALID) )
	{
		if (CCalendarWindow::Is2k())
		{
			SYSTEMTIME timeGMT = GetSystemTime();
			SYSTEMTIME timeOther = GetSystemTime();
			//TzSpecificLocalTimeToSystemTime(NULL, &timeOther, &timeGMT);
			tzLocal.Bias *= -1;
			tzLocal.DaylightBias *= -1;
			tzLocal.StandardBias *= -1;
			SystemTimeToTzSpecificLocalTime(&tzLocal, &timeOther, &timeGMT);
			SystemTimeToTzSpecificLocalTime(&tzInfo, &timeGMT, &timeOther);
			m_Valid = SystemTimeToFileTime(&timeOther, &m_FileTime) != 0;
		}
		else
		{
			ULONGLONG secs;
			if (dst == 2) 
				secs = abs(tzLocal.Bias + tzLocal.DaylightBias) * 60;
			else if (dst == 1)
				secs = abs(tzLocal.Bias + tzLocal.StandardBias) * 60;
			else
				secs = abs(tzLocal.Bias) * 60;

			if (tzLocal.Bias<0)
				Substract(secs);
			else
				Add(secs);
			
			secs = abs(tzInfo.Bias) * 60;
			if (tzInfo.Bias < 0)
				Add(secs);
			else
				Substract(secs);

			SYSTEMTIME time = GetSystemTime();
			dst = isDst(time, tzInfo);
			if (dst == 2)
			{
				secs = abs(tzInfo.DaylightBias) * 60;
				if (tzInfo.DaylightBias < 0)
					Add(secs);
				else
					Substract(secs);
			}
			else if (dst == 1)
			{
				secs = abs(tzInfo.StandardBias) * 60;
				if (tzInfo.StandardBias < 0)
					Add(secs);
				else
					Substract(secs);
			}
		}
	}
}

void CFileTime::Get_tm(struct tm &ret)
{
	SYSTEMTIME timeGMT = GetSystemTime();

	int x;
	int monthday[12] = {0,31,59,90,120,151,181,212,243,273,304,334};

	x = monthday[(int)timeGMT.wMonth-1] + (int)timeGMT.wDay;
	if ( (timeGMT.wMonth>2) && (IsLeapYear(timeGMT.wYear)) )
		x++;
	
	//static struct tm* ret = {(int)timeGMT.wSecond, (int)timeGMT.wMinute, (int)timeGMT.wHour, (int)timeGMT.wDay, (int)timeGMT.wMonth - 1, (int)timeGMT.wYear - 1900, (int)timeGMT.wDayOfWeek, x};
	ret.tm_hour = (int)timeGMT.wHour;
	ret.tm_isdst = 0;
	ret.tm_mday = (int)timeGMT.wDay;
	ret.tm_min = (int)timeGMT.wMinute;
	ret.tm_mon = (int)timeGMT.wMonth - 1;
	ret.tm_sec = (int)timeGMT.wSecond;
	ret.tm_wday = (int)timeGMT.wDayOfWeek;
	ret.tm_yday = x;
	ret.tm_year = (int)timeGMT.wYear - 1900;

	//return &ret;
}

void CFileTime::ConvertToGMT()
{
	TIME_ZONE_INFORMATION tzLocal;
	//SYSTEMTIME timeGMT = GetSystemTime();
	//SYSTEMTIME timeOther = GetSystemTime();

	int dst = GetTimeZoneInformation(&tzLocal);
	ULONGLONG secs;
	if (dst == 2) 
		secs = abs(tzLocal.Bias + tzLocal.DaylightBias) * 60;
	else if (dst == 1)
		secs = abs(tzLocal.Bias + tzLocal.StandardBias) * 60;		
	else
		secs = abs(tzLocal.Bias) * 60;

	if (tzLocal.Bias<0)
		Substract(secs);
	else
		Add(secs);
	//TzSpecificLocalTimeToSystemTime(NULL, &timeOther, &timeGMT);
	//timeGMT = GetSystemTime();
	//m_Valid = SystemTimeToFileTime(&timeGMT, &m_FileTime) != 0;
}

void CFileTime::ConvertToLocal()
{
	TIME_ZONE_INFORMATION tzLocal;
	//SYSTEMTIME timeGMT = GetSystemTime();
	//SYSTEMTIME timeOther = GetSystemTime();

	int dst = GetTimeZoneInformation(&tzLocal);
	ULONGLONG secs;
	if (dst == 2) 
		secs = abs(tzLocal.Bias + tzLocal.DaylightBias) * 60;
	else if (dst == 1)
		secs = abs(tzLocal.Bias + tzLocal.StandardBias) * 60;		
	else
		secs = abs(tzLocal.Bias) * 60;

	if (tzLocal.Bias<0)
		Add(secs);
	else
		Substract(secs);
	//SystemTimeToTzSpecificLocalTime(NULL, &timeGMT, &timeOther);
	//m_Valid = SystemTimeToFileTime(&timeOther, &m_FileTime) != 0;
}
// Mitul}
