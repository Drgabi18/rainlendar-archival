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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/ItemDays.h,v 1.5 2003/06/15 09:49:12 Rainy Exp $

  $Log: ItemDays.h,v $
  Revision 1.5  2003/06/15 09:49:12  Rainy
  Added support for multiple calendars.

  Revision 1.4  2002/11/12 18:11:41  rainy
  The interface of Paint changed a little.

  Revision 1.3  2002/08/24 11:11:54  rainy
  Added ResetDayTypes()

  Revision 1.2  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef __ITEMDAYS_H__
#define __ITEMDAYS_H__

#include "Item.h"

class CItemDays : public CItem  
{
public:
	CItemDays();
	virtual ~CItemDays();

	void Initialize();
	void Paint(CImage& background, POINT offset);
	int HitTest(int x, int y);

	int GetX();
	int GetY();
	int GetW();
	int GetH();
};

#endif
