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
  $Header: /home/cvsroot/Rainlendar/Plugin/FileTime.cpp,v 1.2 2004/12/05 18:31:28 rainy Exp $

  $Log: FileTime.cpp,v $
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

	m_Valid = (bool)SystemTimeToFileTime(&sysTime, &m_FileTime);
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

	m_Valid = (bool)SystemTimeToFileTime(&sysTime, &m_FileTime);
}

CFileTime::CFileTime(SYSTEMTIME& sysTime)
{
	m_Valid = (bool)SystemTimeToFileTime(&sysTime, &m_FileTime);
}

CFileTime::~CFileTime()
{
}

void CFileTime::SetToLocalTime()
{
	FILETIME time;
	GetSystemTimeAsFileTime(&time);
	m_Valid = (bool)FileTimeToLocalFileTime(&time, &m_FileTime);
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

	value1.QuadPart += secs * 10000000;

	m_FileTime.dwLowDateTime = value1.LowPart;
	m_FileTime.dwHighDateTime = value1.HighPart;
}

void CFileTime::Substract(const ULONGLONG secs)
{
	ULARGE_INTEGER value1;

	value1.LowPart = m_FileTime.dwLowDateTime;
	value1.HighPart = m_FileTime.dwHighDateTime;

	value1.QuadPart -= secs * 10000000;

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

int CFileTime::GetDaysInMonth()
{
	const SYSTEMTIME& sysTime = GetSystemTime();

	UINT dates[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if (sysTime.wMonth == 2 && ((((sysTime.wYear % 4) == 0) && ((sysTime.wYear % 100) != 0)) || (sysTime.wYear % 400) == 0))
	{
		return 29;
	}

	return dates[sysTime.wMonth - 1];
}

bool CFileTime::IsLeapYear()
{
	const SYSTEMTIME& sysTime = GetSystemTime();

	if (sysTime.wMonth == 2 && ((((sysTime.wYear % 4) == 0) && ((sysTime.wYear % 100) != 0)) || (sysTime.wYear % 400) == 0))
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
	static TCHAR buffer[256];
	const SYSTEMTIME& sysTime = GetSystemTime();
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, NULL, buffer, 256);
	return buffer;
}

bool CFileTime::FromTime(LPCTSTR str)
{
	SYSTEMTIME sysTime = GetSystemTime();

	bool ok = false;
	int hour, min;
	std::string tmpString = str;
	if (2 == _stscanf(tmpString.c_str(), "%i:%i", &hour, &min))
	{
		ok = true;
	}
	else if (2 == _stscanf(tmpString.c_str(), "0%i:%i", &hour, &min))		// Parsing fails for times between 08:00-09:59 if there is a 0-prefix. Wierd.
	{
		ok = true;
	}
	else if (2 == _stscanf(tmpString.c_str(), "%i.%i", &hour, &min))
	{
		ok = true;
	}
	else if (2 == _stscanf(tmpString.c_str(), "0%i.%i", &hour, &min))		// Parsing fails for times between 08:00-09:59 if there is a 0-prefix. Wierd.
	{
		ok = true;
	}

	if (ok)
	{
		int pos = tmpString.find_first_not_of(" 0123456789:");
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
	}
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

