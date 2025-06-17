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
  $Header: /home/cvsroot/Rainlendar/Library/FileTime.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: FileTime.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.3  2005/03/25 13:58:43  rainy
  no message

  Revision 1.2  2005/03/01 18:50:45  rainy
  no message

  Revision 1.1  2004/11/06 13:38:59  rainy
  no message

*/

#ifndef __FILETIME_H__
#define __FILETIME_H__

#include <windows.h>
#include <time.h>

#define FOREVER_VALUE 0x7FFFFFFF

class CFileTime
{
public:
	CFileTime();
	CFileTime(FILETIME time);
	CFileTime(time_t t);
	CFileTime(int day, int month, int year);
	CFileTime(int day, int month, int year, int hour, int minute, int second);
	CFileTime(SYSTEMTIME& sysTime);
	~CFileTime();

	CFileTime& operator=(const CFileTime& fileTime) { m_FileTime = fileTime.m_FileTime; return *this; };

	void SetToLocalTime();

	void SetFileTime(FILETIME time) { m_FileTime = time; };
	FILETIME GetFileTime() { return m_FileTime; };

	void Add(const CFileTime& time);
	void Substract(const CFileTime& time);

	void Add(const ULONGLONG secs);
	void Substract(const ULONGLONG secs);

	int Compare(CFileTime& date);
	int DifferenceInDays(CFileTime& date);
	void NormalizeDay();

	time_t GetTime_t();

	const SYSTEMTIME& GetSystemTime();
	int GetAsSeconds();
	int GetOnlyTime();
	int GetWeekday();	// 0-6, Sunday=0
	int GetDaysInMonth();
	int GetMonthsFirstWeekday();
	bool IsLeapYear();

	LPCTSTR ToString();
	bool FromString(LPCTSTR str);

	bool CFileTime::FromTime(LPCTSTR str);

	LPCTSTR ToTime();
	LPCTSTR ToDate();
	LPCTSTR ToTimeAndDateString(bool noYear = false);

	bool IsValid() { return m_Valid; }

private:
	FILETIME m_FileTime;
	bool m_Valid;
};

#endif