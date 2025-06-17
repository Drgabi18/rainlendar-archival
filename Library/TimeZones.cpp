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
  $Header: /home/cvsroot/Rainlendar/Library/TimeZones.cpp,v 1.1 2005/10/14 17:06:13 rainy Exp $
*/

#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "TimeZones.h"

CTimeZones* CTimeZones::c_TimeZones = NULL;

CTimeZones& CTimeZones::Instance()
{
	if (c_TimeZones == NULL)
	{
		c_TimeZones = new CTimeZones;
	}

	return *c_TimeZones;
}

void CTimeZones::DeleteInstance()
{
	delete c_TimeZones;
	c_TimeZones = NULL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimeZones::CTimeZones()
{
	ReadTimeZones();
}

CTimeZones::~CTimeZones()
{
	m_TimeZones.clear();
}

void CTimeZones::GetTimeZoneInformation(const char* nm, TIME_ZONE_INFORMATION &tz)
{
    std::map<std::string, LS_TZREG>::iterator iter = m_TimeZones.find(nm);
    
    if (iter != m_TimeZones.end())
    {
		LS_TZREG Temp;

		Temp=(*iter).second;
		tz.Bias = Temp.TZI.Bias;
		tz.DaylightBias = Temp.TZI.DaylightBias;
		tz.DaylightDate = Temp.TZI.DaylightDate;
		tz.StandardDate = Temp.TZI.StandardDate;
		tz.StandardBias = Temp.TZI.StandardBias;
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, 
			Temp.tcSTD, MAX_SIZE, tz.StandardName, 32);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, 
			Temp.tcDLT, MAX_SIZE, tz.DaylightName, 32);
    }
}

std::string CTimeZones::GetTimeZoneName(const char* nm)
{
    std::map<std::string, LS_TZREG>::iterator iter = m_TimeZones.find(nm);
    
    if (iter != m_TimeZones.end())
    {
		static LS_TZREG Temp;
		Temp=(*iter).second;
		return Temp.tcName;
	}
	return "";
}


void CTimeZones::ReadTimeZones()
{
	HKEY HKlmtz;
	HKEY KKtz;
	DWORD dwIndex = 0;
	CHAR tcName[MAX_SIZE];
	DWORD dwcbName = MAX_SIZE;
	DWORD dwcbValue;
	DWORD dwcbSTD;
	DWORD dwcbDLT;
	FILETIME ftLastWrite;

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,TZREG,0,
		KEY_ALL_ACCESS,&HKlmtz) != ERROR_SUCCESS)
	return;

	while(RegEnumKeyEx(HKlmtz,dwIndex++,tcName,&dwcbName,NULL,NULL,NULL,&ftLastWrite) != ERROR_NO_MORE_ITEMS)
	{
		dwcbName = MAX_SIZE;

		if(RegOpenKeyEx(HKlmtz,tcName,0,KEY_ALL_ACCESS,&KKtz) != ERROR_SUCCESS)
		{
			RegCloseKey(HKlmtz);
			return;
		}
		else
		{
			LS_TZREG Temp;
			std::string nm;

			nm = tcName;
			std::string::size_type start = nm.find(" Standard Time");
			if (start != -1)
				nm.replace (start,14,"");

			strcpy(Temp.tcName, nm.c_str());

			dwcbValue = MAX_SIZE;
			RegQueryValueEx(KKtz,"Display",NULL,NULL,(BYTE*)Temp.tcDisp,&dwcbValue);
			nm = Temp.tcDisp;

			dwcbDLT = MAX_SIZE;
			RegQueryValueEx(KKtz,"Dlt",NULL,NULL,(BYTE*)Temp.tcDLT,&dwcbDLT);

			dwcbSTD = MAX_SIZE;
			RegQueryValueEx(KKtz,"Std",NULL,NULL,(BYTE*)Temp.tcSTD,&dwcbSTD);

			dwcbValue = MAX_SIZE;
			RegQueryValueEx(KKtz,"MapID",NULL,NULL,(BYTE*)Temp.MapID,&dwcbValue);

			dwcbValue = sizeof(DWORD);
			RegQueryValueEx(KKtz,"Index",NULL,NULL,(BYTE*)&Temp.Index,&dwcbValue);

			dwcbValue = sizeof(REG_TZI);
			RegQueryValueEx(KKtz,"TZI",NULL,NULL,(BYTE*)&Temp.TZI,&dwcbValue);

			m_TimeZones[nm] = Temp;
		}
		RegCloseKey(KKtz);
	}
	RegCloseKey(HKlmtz);
}