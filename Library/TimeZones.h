/*
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
  $Header: /home/cvsroot/Rainlendar/Library/TimeZones.h,v 1.1 2005/10/14 17:06:13 rainy Exp $

*/
#ifndef __TIMEZONES_H__
#define __TIMEZONES_H__

#include <time.h>
#include <map>

#define TZREG "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones"
#define TZREG2 "SYSTEM\\CurrentControlSet\\Control\\TimeZoneInformation"

#define MAX_SIZE 512

struct REG_TZI
{
	long Bias;
	long StandardBias;
	long DaylightBias;
	SYSTEMTIME StandardDate; 
	SYSTEMTIME DaylightDate;
};

struct LS_TZREG
{
	TCHAR tcName[MAX_SIZE];
	TCHAR tcDisp[MAX_SIZE];
	TCHAR tcDLT[MAX_SIZE];
	TCHAR tcSTD[MAX_SIZE];
	TCHAR MapID[MAX_SIZE];
	DWORD Index;
	DWORD ActiveTimeBias;
	REG_TZI TZI;
};

class CTimeZones
{
public:
	static CTimeZones& Instance();
	static void DeleteInstance();

	CTimeZones();
	~CTimeZones();

	void GetTimeZoneInformation(const char* nm, TIME_ZONE_INFORMATION &tz);
	std::string GetTimeZoneName(const char* nm);

private:
	static CTimeZones* c_TimeZones;

	void ReadTimeZones();

    std::map<std::string, LS_TZREG> m_TimeZones;
};
#endif
