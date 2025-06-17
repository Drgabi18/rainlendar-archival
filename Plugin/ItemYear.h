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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/ItemYear.h,v 1.1.1.1 2001/10/29 18:56:24 rainy Exp $

  $Log: ItemYear.h,v $
  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#if !defined(AFX_ITEMYEAR_H__371A4547_F1CB_11D3_92A3_0080AD90417B__INCLUDED_)
#define AFX_ITEMYEAR_H__371A4547_F1CB_11D3_92A3_0080AD90417B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Item.h"

class CItemYear : public CItem  
{
public:
	CItemYear();
	virtual ~CItemYear();

	void Initialize();
	void Paint(CDC& dc);

	int GetX();
	int GetY();
	int GetW();
	int GetH();
};

#endif // !defined(AFX_ITEMYEAR_H__371A4547_F1CB_11D3_92A3_0080AD90417B__INCLUDED_)
