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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/ItemWeekNumbers.h,v 1.2 2002/05/23 17:33:40 rainy Exp $

  $Log: ItemWeekNumbers.h,v $
  Revision 1.2  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.1  2002/01/10 16:40:37  rainy
  Initial version

*/

#ifndef __ITEMWEEKNUMBERS_H__
#define __ITEMWEEKNUMBERS_H__

#include "Item.h"

class CItemWeekNumbers : public CItem  
{
public:
	CItemWeekNumbers();
	virtual ~CItemWeekNumbers();

	void Initialize();
	void Paint(HDC dc);
};

#endif
