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
  $Header: /home/cvsroot/Rainlendar/Plugin/UpdateCheck.cpp,v 1.2 2004/12/05 18:20:06 rainy Exp $

  $Log: UpdateCheck.cpp,v $
  Revision 1.2  2004/12/05 18:20:06  rainy
  Changed the url.

  Revision 1.1  2004/11/06 13:38:59  rainy
  no message

*/

#pragma warning(disable: 4786)
#pragma warning(disable: 4996)

#include <windows.h>
#include "RainlendarDll.h"
#include "Litestep.h"
#include <string>
#include <Wininet.h>
#include <process.h>

void CheckVersion(void* dummy)
{
	int version = 0;

	HINTERNET hRootHandle = InternetOpen("Rainlendar",
									INTERNET_OPEN_TYPE_PRECONFIG,
									NULL,
									NULL,
									0);
	if (hRootHandle == NULL)
	{
		DebugLog("CheckUpdate: InternetOpen failed.");
		return;
	}

	HINTERNET hUrlDump = InternetOpenUrl(hRootHandle, "http://www.rainlendar.net/Rainlendar/version", NULL, NULL, INTERNET_FLAG_RESYNCHRONIZE, 0);
	if (hUrlDump)
	{
		DWORD dwSize;
		char buffer[16];	// 16 should be enough for the version number
		buffer[0] = 0;
		if (InternetReadFile(hUrlDump, (LPVOID)buffer, 16, &dwSize))
		{
			// Add a null terminator to the end of the buffer (just in case...). 
			std::string verStr = buffer;
			int pos = verStr.find('.');
			if (pos != -1)
			{
				std::string verStr1 = verStr.substr(pos + 1);
				version = atoi(verStr1.c_str()) * 1000;
				pos = verStr.find('.', pos + 1);
				if (pos != -1)
				{
					std::string verStr1 = verStr.substr(pos + 1);
					version += atoi(verStr1.c_str());
				}
			}

			if (version > RAINLENDAR_VERSION)
			{
				if (IDYES == MessageBox(NULL, CCalendarWindow::c_Language.GetString("Message", 9), APPNAME, MB_YESNO | MB_ICONQUESTION))
				{
					LSExecute(NULL, "http://www.iki.fi/rainy/Rainlendar.html", SW_SHOWNORMAL);
				}
			}
			else
			{
				DebugLog("CheckUpdate: No new version available.");
			}
		}
		else
		{
			DebugLog("CheckUpdate: InternetReadFile failed.");
		}
		InternetCloseHandle(hUrlDump);
	}
	else
	{
		DebugLog("CheckUpdate: InternetOpenUrl failed.");
	}

	InternetCloseHandle(hRootHandle);
}

void CheckUpdate()
{
	_beginthread(CheckVersion, 0, NULL );
}
