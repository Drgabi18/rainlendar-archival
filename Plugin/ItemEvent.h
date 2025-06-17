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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/ItemEvent.h,v 1.8 2002/11/25 17:03:24 rainy Exp $

  $Log: ItemEvent.h,v $
  Revision 1.8  2002/11/25 17:03:24  rainy
  Added DrawIcon method.

  Revision 1.7  2002/11/12 18:13:32  rainy
  The interface of Paint changed a little.
  Calendar text can be painted on RasterizerFont's alpha buffer.

  Revision 1.6  2002/08/24 11:11:10  rainy
  Few changes to prevent showing the deleted events.

  Revision 1.5  2002/08/03 16:16:19  rainy
  Changed to use EventManager and customToolTip class.

  Revision 1.4  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.3  2002/01/27 16:03:24  rainy
  Changed CEvent to CEventMessage to avoid name clash

  Revision 1.2  2002/01/10 16:46:35  rainy
  Added possibility to show the events in the calendar window.
  Added support for event specific colors/bitmaps.

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef __ITEMEVENT_H__
#define __ITEMEVENT_H__

#include "Item.h"
#include "Event.h"
#include "EventManager.h"

class CCalendarWindow;

class CItemEvent : public CItem  
{
public:
	CItemEvent();
	virtual ~CItemEvent();

	void Initialize();
	void ResetDayTypes();
	void Paint(CImage& background);

	void AddToolTips(CCalendarWindow* CalendarWnd);
	bool GetEventText(int day, std::string& text);

	CEventManager* GetEventManager() { return &m_EventManager; };

protected:
	void DrawIcon(CImage& background, int day, int X, int Y, int W, int H);
	void SetFont(const std::string& FontName);
	const Profile* GetEventProfile(int day);

	HFONT m_EventFont;

	CEventManager m_EventManager;
};

#endif
