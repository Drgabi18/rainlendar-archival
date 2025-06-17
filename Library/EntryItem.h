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
  $Header: /home/cvsroot/Rainlendar/Library/EntryItem.h,v 1.2 2005/10/14 17:05:41 rainy Exp $

  $Log: EntryItem.h,v $
  Revision 1.2  2005/10/14 17:05:41  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.2  2005/03/01 18:50:45  rainy
  no message

  Revision 1.1  2004/11/06 13:38:58  rainy
  no message

*/

#ifndef _ENTRYITEM_H_
#define _ENTRYITEM_H_

#include <string>
#include "RainlendarAPI.h"

class CEntryItem
{
public:
	CEntryItem();
	virtual ~CEntryItem();

	RainlendarItem* GetRainlendarItem() { return m_Item; }

	RAINLENDAR_TYPE GetType() { return m_Item->type; }

	GUID* GetGUID() { return &m_Item->guid; }
	FILETIME GetTimeStamp() { return m_Item->timeStamp; }


	void SetPluginID(UINT ID) { m_PluginID = ID; }
	UINT GetPluginID() { return m_PluginID; }

	bool IsReadOnly() { return m_Item->readOnly != 0; }

	bool IsDeleted() { return m_Item->deleted != 0; }
	void MarkAsDeleted() { m_Item->deleted = true; }

	void UpdateTimeStamp();

	static LPCTSTR GetGUIDAsString(GUID* guid);

protected:
	RainlendarItem* m_Item;
	UINT m_PluginID;		// This is the plugin that created the event
};

#endif
