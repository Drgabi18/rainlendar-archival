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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/Item.cpp,v 1.3 2003/06/15 09:49:11 Rainy Exp $

  $Log: Item.cpp,v $
  Revision 1.3  2003/06/15 09:49:11  Rainy
  Added support for multiple calendars.

  Revision 1.2  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#include "RainlendarDLL.h"
#include "Item.h"

CItem::CItem()
{
	m_Rasterizer=NULL;
}

CItem::~CItem()
{
	if(m_Rasterizer) delete m_Rasterizer;
}

void CItem::SetRasterizer(CRasterizer* Rasterizer)
{
	if(m_Rasterizer) delete m_Rasterizer;	// Kill the old
	m_Rasterizer=Rasterizer;
}

int CItem::GetDayType(int day, int month, int year)
{
	int type = NORMAL;

	std::vector<CEventMessage*> eventList = GetRainlendar()->GetCalendarWindow().GetEventManager()->GetEvents(day, month, year);

	// Make sure that the events are not deleted
	std::vector<CEventMessage*>::iterator iter = eventList.begin();
	for( ;  iter != eventList.end(); iter++)
	{
		if (!((*iter)->IsDeleted()))
		{
			type = EVENT;
			break;
		}
	}

	// If this month is shown, mark today
	if (CCalendarWindow::c_TodaysDate.wMonth == month &&
	    CCalendarWindow::c_TodaysDate.wDay == day && 
	    CCalendarWindow::c_TodaysDate.wYear == year) 
	{
		type |= TODAY;
	}

	return type;
}

UINT CItem::GetDaysInMonth(int year, int month)
{
	UINT dates[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if (month == 2 && ((((year % 4) == 0) && ((year % 100) != 0)) || (year % 400) == 0))
	{
		return 29;
	}

	return dates[month - 1];
}