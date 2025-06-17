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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/ItemToday.h,v 1.5 2003/06/15 09:49:12 Rainy Exp $

  $Log: ItemToday.h,v $
  Revision 1.5  2003/06/15 09:49:12  Rainy
  Added support for multiple calendars.

  Revision 1.4  2003/03/22 20:30:25  rainy
  Overwrote dimension getters.

  Revision 1.3  2002/11/12 18:11:34  rainy
  The interface of Paint changed a little.

  Revision 1.2  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef __ITEMTODAY_H__
#define __ITEMTODAY_H__

#include "Item.h"

class CItemToday : public CItem  
{
public:
	CItemToday();
	virtual ~CItemToday();

	void Initialize();
	void Paint(CImage& background, POINT offset);

	virtual int GetX();
	virtual int GetY();
	virtual int GetW();
	virtual int GetH();
};

#endif
