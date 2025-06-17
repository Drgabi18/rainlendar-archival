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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Item.h,v 1.1.1.1 2001/10/29 18:56:23 rainy Exp $

  $Log: Item.h,v $
  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#if !defined(AFX_ITEM_H__7F453361_F122_11D3_92A3_0080AD90417B__INCLUDED_)
#define AFX_ITEM_H__7F453361_F122_11D3_92A3_0080AD90417B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
	virtual void Paint(CDC& dc) = 0;

	CRasterizer* GetRasterizer() { return m_Rasterizer; };
	void SetRasterizer(CRasterizer* Rasterizer);

	virtual int GetX() { return 0; };
	virtual int GetY() { return 0; };
	virtual int GetW() { return 0; };
	virtual int GetH() { return 0; };

	static int GetDayType(int Index) { return c_DayTypes[Index]; };
	static void SetDayType(int Index, int DayType) { c_DayTypes[Index]|=DayType; };		// Note the OR !!

protected:
	CRasterizer* m_Rasterizer;

	static int c_DayTypes[32];
};

#endif // !defined(AFX_ITEM_H__7F453361_F122_11D3_92A3_0080AD90417B__INCLUDED_)
