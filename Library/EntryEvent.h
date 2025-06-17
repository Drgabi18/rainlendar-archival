/*
  Copyright (C) 2000 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Library/EntryEvent.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: EntryEvent.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.1  2004/11/06 13:38:58  rainy
  no message

  Revision 1.12  2004/04/24 11:19:35  rainy
  SetEveryNth doesn't return 0 anymore.

  Revision 1.11  2004/01/10 15:16:05  rainy
  The start time is parsed from the message.

  Revision 1.10  2003/05/26 18:44:40  Rainy
  Added more consts.

  Revision 1.9  2003/05/25 18:09:20  Rainy
  Gets are now consts

  Revision 1.8  2003/05/07 19:13:49  rainy
  Added permanent and timestamp.

  Revision 1.7  2002/11/25 17:10:21  rainy
  Added SetID()

  Revision 1.6  2002/08/24 11:12:19  rainy
  Added some trimming support.

  Revision 1.5  2002/08/03 16:17:44  rainy
  Added new variables to support repeating events.

  Revision 1.4  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.3  2002/01/27 16:03:53  rainy
  Changed CEvent to CEntryEvent to avoid name clash

  Revision 1.2  2002/01/10 16:46:56  rainy
  Now holds the bitmap and color too.

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef _ENTRYEVENT_H_
#define _ENTRYEVENT_H_

#include <string>
#include "EntryItem.h"
#include "FileTime.h"
#include "RainlendarAPI.h"

class CEntryEvent : public CEntryItem
{
public:
	CEntryEvent();
	CEntryEvent(CEntryItem* item);
	CEntryEvent(RainlendarItem* item);
	virtual ~CEntryEvent();

	RainlendarEvent* GetRainlendarEvent() { return (RainlendarEvent*)m_Item; }

	CFileTime GetBegin();
	CFileTime GetEnd();

	void GetBriefMessage(std::string& message, int len, bool addTime, bool longFormat);

	CFileTime GetStartTime() { return GetRainlendarEvent()->startTime; };
	void SetStartTime(CFileTime time) { GetRainlendarEvent()->startTime = time.GetFileTime(); }

	CFileTime GetEndTime() { return GetRainlendarEvent()->endTime; };
	void SetEndTime(CFileTime time) { GetRainlendarEvent()->endTime = time.GetFileTime(); }

	LPCTSTR GetHeader() { return GetRainlendarEvent()->header; };
	void SetHeader(LPCTSTR text);

	LPCTSTR GetMessage() { return GetRainlendarEvent()->message; };
	void SetMessage(LPCTSTR text);

	LPCTSTR GetProfile() { return GetRainlendarEvent()->profile == NULL ? "Default" : GetRainlendarEvent()->profile; };
	void SetProfile(LPCTSTR profile);

	bool IsAllDayEvent() { return GetRainlendarEvent()->allDayEvent != 0; }
	void SetAllDayEvent(bool state) { GetRainlendarEvent()->allDayEvent = state; }

	RainlendarRecurrency* GetRecurrency() { return GetRainlendarEvent()->recurrency; };
	void SetRecurrency(RainlendarRecurrency* recurrency);

	int GetAlarm() { return GetRainlendarEvent()->alarm; };
	void SetAlarm(int alarm) {GetRainlendarEvent()->alarm = alarm; };
};

#endif
