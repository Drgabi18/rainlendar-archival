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
  $Header: /home/cvsroot/Rainlendar/Library/EntryItem.cpp,v 1.2 2005/10/14 17:05:41 rainy Exp $

  $Log: EntryItem.cpp,v $
  Revision 1.2  2005/10/14 17:05:41  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.2  2005/03/01 18:50:45  rainy
  no message

  Revision 1.1  2004/11/06 13:38:58  rainy
  no message

*/
#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "EntryItem.h"
#include <math.h>
#include <time.h>
#include <objbase.h>

CEntryItem::CEntryItem()
{
	m_Item = NULL;
	m_PluginID = 0;
}

CEntryItem::~CEntryItem()
{
	Rainlendar_DeleteItem(m_Item);
}

void CEntryItem::UpdateTimeStamp()
{
	FILETIME time;
	GetSystemTimeAsFileTime(&time);
	FileTimeToLocalFileTime(&time, &m_Item->timeStamp);	
}

LPCTSTR CEntryItem::GetGUIDAsString(GUID* guid)
{
	static TCHAR buffer[256];	// 256 should be enough...
	WCHAR* wStr = NULL;
	HRESULT hr = StringFromCLSID(*guid, &wStr);

	if (SUCCEEDED(hr))
	{
		if (WideCharToMultiByte(CP_ACP, 0, wStr, -1, buffer, 256, NULL, NULL) == 0)
		{
			CoTaskMemFree(wStr);
			return NULL;
		}
		CoTaskMemFree(wStr);
	}
	return buffer;
}

