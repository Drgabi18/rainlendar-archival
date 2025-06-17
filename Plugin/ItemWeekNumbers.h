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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/ItemWeekNumbers.h,v 1.1 2002/01/10 16:40:37 rainy Exp $

  $Log: ItemWeekNumbers.h,v $
  Revision 1.1  2002/01/10 16:40:37  rainy
  Initial version

*/

#if !defined(AFX_ITEMWEEKNUMBERS_H__CC665490_4370_400D_9903_3471FD1AB032__INCLUDED_)
#define AFX_ITEMWEEKNUMBERS_H__CC665490_4370_400D_9903_3471FD1AB032__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Item.h"

class CItemWeekNumbers : public CItem  
{
public:
	CItemWeekNumbers();
	virtual ~CItemWeekNumbers();

	void Initialize();
	void Paint(CDC& dc);
};

#endif // !defined(AFX_ITEMWEEKNUMBERS_H__CC665490_4370_400D_9903_3471FD1AB032__INCLUDED_)
