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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/Item.h,v 1.6 2003/06/15 09:49:12 Rainy Exp $

  $Log: Item.h,v $
  Revision 1.6  2003/06/15 09:49:12  Rainy
  Added support for multiple calendars.

  Revision 1.5  2002/11/12 18:11:41  rainy
  The interface of Paint changed a little.

  Revision 1.4  2002/08/24 11:11:58  rainy
  Added ResetDayTypes()

  Revision 1.3  2002/08/03 16:16:59  rainy
  GetDaysInMonth is now static.

  Revision 1.2  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef __ITEM_H__
#define __ITEM_H__

#include "Rasterizer.h"

// DayTypes
#define NORMAL 0
#define TODAY 1
#define EVENT 2

class CItem  
{
public:
	CItem();
	virtual ~CItem();

	virtual void Initialize() = 0;
	virtual void Paint(CImage& background, POINT offset) = 0;

	CRasterizer* GetRasterizer() { return m_Rasterizer; };
	void SetRasterizer(CRasterizer* Rasterizer);

	virtual int GetX() { return 0; };
	virtual int GetY() { return 0; };
	virtual int GetW() { return 0; };
	virtual int GetH() { return 0; };

	int GetDayType(int day, int month, int year);

	static UINT GetDaysInMonth(int year, int month);

protected:
	CRasterizer* m_Rasterizer;
};

#endif
