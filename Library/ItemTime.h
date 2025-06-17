/*
  Copyright (C) 2004 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Library/ItemTime.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: ItemTime.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.2  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.1  2004/11/06 13:38:59  rainy
  no message

*/

#ifndef __ITEMTIME_H__
#define __ITEMTIME_H__

#include "Item.h"
#include <vector>

enum RAINWINDOW_TYPE;

class CItemTime : public CItem
{
public:
	CItemTime();
	virtual ~CItemTime();

	virtual void Initialize();
	virtual void ReadSettings(const char* filename, const char* section);
	virtual void WriteSettings();

	virtual void Paint(CImage& background, POINT offset);

	virtual bool NeedsUpdating() { return true; };

	RAINWINDOW_TYPE GetWinType() { return m_WinType; }

protected:
	RAINWINDOW_TYPE m_WinType;
	int m_X;
	int m_Y;
	std::string m_Format;
};

#endif
