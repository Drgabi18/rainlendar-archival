/*
  Copyright (C) 2005 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Plugins/PluginOutlook/AdviseSink.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: AdviseSink.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

*/

#ifndef _ADVISESINK_H_
#define _ADVISESINK_H_

#include <mapidefs.h>
#include <vector>

class CAdviseSink : public IMAPIAdviseSink
{
public:
	CAdviseSink();
	~CAdviseSink();

	STDMETHODIMP			QueryInterface(REFIID riid, LPVOID * ppvObj);
	STDMETHODIMP_(ULONG)	AddRef();
	STDMETHODIMP_(ULONG)	Release();

	STDMETHODIMP_(ULONG)	OnNotify (ULONG cNotify, LPNOTIFICATION lpNotifications);

	void AddParentID(LPENTRYID id, ULONG size);

private:
	void DebugLogID(LPENTRYID id, ULONG size);

	int m_RefCount;

	struct AS_EVENTID
	{
		int size;
		BYTE* data;
	};

	std::vector<AS_EVENTID> m_EventIDs;
};

#endif
