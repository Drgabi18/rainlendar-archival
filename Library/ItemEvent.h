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
  $Header: /home/cvsroot/Rainlendar/Library/ItemEvent.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: ItemEvent.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.13  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.12  2004/11/06 13:38:59  rainy
  no message

  Revision 1.11  2004/01/25 10:00:37  rainy
  Added separate method for icons.

  Revision 1.10  2003/08/09 16:36:05  Rainy
  Removed GetEventText()

  Revision 1.9  2003/06/15 09:49:12  Rainy
  Added support for multiple calendars.

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
  Changed CEvent to CEntryEvent to avoid name clash

  Revision 1.2  2002/01/10 16:46:35  rainy
  Added possibility to show the events in the calendar window.
  Added support for event specific colors/bitmaps.

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef __ITEMEVENT_H__
#define __ITEMEVENT_H__

#include "Item.h"
#include "EntryEvent.h"
#include "EventManager.h"

class CCalendarWindow;

class CItemEvent : public CItem  
{
public:
	CItemEvent();
	virtual ~CItemEvent();

	virtual void Initialize();
	virtual void ReadSettings(const char* filename, const char* section);
	virtual void WriteSettings();

	void Paint(CImage& background, POINT offset);
	void PaintIcons(CImage& background, POINT offset);
	std::vector<CEntryEvent*>* HitTest(int x, int y, CFileTime* startTime);

	void AddToolTips(CCalendarWindow* CalendarWnd, POINT offset);
	static int GetDayType(int day, int month, int year, bool ignoreAllwaysDrawn);

	// Event
	CRasterizer::ALIGN GetEventAlign() { return m_EventAlign; };
    const std::string& GetEventBitmapName() { return m_EventBitmapName; };
	int GetEventNumOfComponents() { return m_EventNumOfComponents; };
	CRasterizer::TYPE GetEventRasterizer() { return m_EventRasterizer; };
	COLORREF GetEventFontColor() { return m_EventFontColor; };
    const std::string& GetEventFont() { return m_EventFont; };
	bool GetEventInCalendar() { return m_EventInCalendar; };
	COLORREF GetEventFontColor2() { return m_EventFontColor2; };
    const std::string& GetEventFont2() { return m_EventFont2; };
	int GetEventSeparation() { return m_EventSeparation; };

	void SetEventFont(const std::string& EventFont ) { m_EventFont=EventFont; };
	void SetEventAlign(CRasterizer::ALIGN EventAlign ) { m_EventAlign=EventAlign; };
	void SetEventBitmapName(const std::string& EventBitmapName ) { m_EventBitmapName=EventBitmapName; };
	void SetEventNumOfComponents(int EventNumOfComponents ) { m_EventNumOfComponents=EventNumOfComponents; };
	void SetEventRasterizer(CRasterizer::TYPE EventRasterizer ) { m_EventRasterizer=EventRasterizer; };
	void SetEventFontColor(COLORREF EventFontColor ) { m_EventFontColor=EventFontColor; };
	void SetEventInCalendar(bool EventInCalendar) { m_EventInCalendar=EventInCalendar; };
	void SetEventFont2(const std::string& EventFont2 ) { m_EventFont2=EventFont2; };
	void SetEventFontColor2(COLORREF EventFontColor2 ) { m_EventFontColor2=EventFontColor2; };
	void SetEventSeparation(int separation) { m_EventSeparation = separation; };

protected:
	void DrawIcon(CImage& background, int day, int month, int year, int X, int Y, int W, int H);
	void SetFont(const std::string& FontName);
	void CalculatePriorities(int day, int month, int year, std::multimap<int, CEntryEvent*>& eventMap);
	void AddTip(int day, int month, int year, HWND hWnd, const RECT& rect);

	HFONT m_InsideEventFont;

	std::string m_EventBitmapName;	// Name of the Event bitmap
	int m_EventNumOfComponents;	// Components in the bitmap
	CRasterizer::ALIGN m_EventAlign;
	CRasterizer::TYPE m_EventRasterizer;
	std::string m_EventFont;
	COLORREF m_EventFontColor;
	bool m_EventInCalendar;
	std::string m_EventFont2;
	COLORREF m_EventFontColor2;
	int m_EventSeparation;		// Separation between components
};

#endif
