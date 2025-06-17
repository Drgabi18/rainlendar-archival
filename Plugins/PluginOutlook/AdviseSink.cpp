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
  $Header: /home/cvsroot/Rainlendar/Plugins/PluginOutlook/AdviseSink.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: AdviseSink.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.3  2005/03/01 18:33:11  rainy
  Added possibility to select which message stores are imported.
  No direct linking to mapi32.dll anymore.

  Revision 1.2  2004/12/05 18:14:35  rainy
  Made the code more robust

  Revision 1.1  2004/11/06 13:32:07  rainy
  Initial version

*/

#include "AdviseSink.h"
#include "OutlookPlugin.h"
#include "../../Library/RainlendarAPI.h"
#define USES_IID_IMAPIAdviseSink
#include <initguid.h>
#include <MAPIGuid.h>

CAdviseSink::CAdviseSink()
{
	m_RefCount = 1;

	DebugLog("OUTLOOK: Created the advise sink");
}

CAdviseSink::~CAdviseSink()
{
	for (int i = 0; i < m_EventIDs.size(); i++)
	{
		free(m_EventIDs[i].data);
	}

	DebugLog("OUTLOOK: Deleted the advise sink");
}

STDMETHODIMP_(ULONG) CAdviseSink::AddRef()
{
	return ++m_RefCount;
}

STDMETHODIMP_(ULONG) CAdviseSink::Release() 
{
	m_RefCount--;
	if (m_RefCount == 0)
	{
		delete this;
		return 0;
	}
	return m_RefCount;
}	

STDMETHODIMP CAdviseSink::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	*ppvObj = 0;

	if (riid == IID_IMAPIAdviseSink || riid == IID_IUnknown)
	{
		*ppvObj = (LPVOID)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CAdviseSink::OnNotify (ULONG cNotify, LPNOTIFICATION lpNotifications)
{
	for (int i = 0; i < cNotify; i++)
	{
		int type = lpNotifications[i].info.obj.ulObjType;

		if (type == MAPI_MESSAGE)
		{
			DebugLog("OUTLOOK: Got notification about message change with ParentID:");

			if (lpNotifications[i].info.obj.lpParentID)
			{
				DebugLogID(lpNotifications[i].info.obj.lpParentID, lpNotifications[i].info.obj.cbParentID);

				// Check the parent
				for (int i = 0; i < m_EventIDs.size(); i++)
				{
					int size = lpNotifications[i].info.obj.cbParentID;
					if (size > 4 && size == m_EventIDs[i].size) 
					{
						BYTE* a = (BYTE*)lpNotifications[i].info.obj.lpParentID;
						BYTE* b = m_EventIDs[i].data;

						// Skip the flags
						size -= 4;
						a += 4;
						b += 4;
						if (memcmp(a, b, size) == 0)
						{
							// Refresh
							DebugLog("OUTLOOK: New Refresh %i - %i", lpNotifications[i].ulEventType, type);
							Rainlendar_Refresh(1000);
							return S_OK;
						}
					}
				}
			}
			else
			{
				DebugLog("No ParentID");
			}

			DebugLog(".. and OldParentID:");

			if (lpNotifications[i].info.obj.lpOldParentID)
			{
				DebugLogID(lpNotifications[i].info.obj.lpOldParentID, lpNotifications[i].info.obj.cbOldParentID);

				// Check the parent
				for (int i = 0; i < m_EventIDs.size(); i++)
				{
					int size = lpNotifications[i].info.obj.cbOldParentID;
					if (size > 4 && size == m_EventIDs[i].size) 
					{
						BYTE* a = (BYTE*)lpNotifications[i].info.obj.lpOldParentID;
						BYTE* b = m_EventIDs[i].data;

						// Skip the flags
						size -= 4;
						a += 4;
						b += 4;
						if (memcmp(a, b, size) == 0)
						{
							// Refresh
							DebugLog("OUTLOOK: Old Refresh %i - %i", lpNotifications[i].ulEventType, type);
							Rainlendar_Refresh(1000);
							return S_OK;
						}
					}
				}
			}
			else
			{
				DebugLog("No OldParentID");
			}
		}
	}

	return S_OK;
}

void CAdviseSink::AddParentID(LPENTRYID id, ULONG size)
{
	DebugLog("Adding parent ID:");
	DebugLogID(id, size);

	// Check if the ID is already in the vector
	for (int i = 0; i < m_EventIDs.size(); i++)
	{
		if (memcmp(id, m_EventIDs[i].data, size) == 0)
		{
			DebugLog("The ID already exists");
			return;
		}
	}

	AS_EVENTID pEid;
	pEid.size = size;
	pEid.data = (BYTE*)malloc(size);
	memcpy(pEid.data, id, size);
	m_EventIDs.push_back(pEid);
}

void CAdviseSink::DebugLogID(LPENTRYID id, ULONG size)
{
	char buffer[4096];
	char buffer2[256];

	buffer[0] = 0;

	for (int i = 0; i < size; i++)
	{
		sprintf(buffer2, "%02X ", ((BYTE*)id)[i]);
		strcat(buffer, buffer2);
	}
	Rainlendar_Log(buffer);
}
