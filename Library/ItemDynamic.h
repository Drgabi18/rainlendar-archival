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
  $Header: /home/cvsroot/Rainlendar/Library/ItemDynamic.h,v 1.2 2005/10/14 17:05:41 rainy Exp $

  $Log: ItemDynamic.h,v $
  Revision 1.2  2005/10/14 17:05:41  rainy
  no message

  Revision 1.1  2005/09/08 16:09:54  rainy
  no message

  Revision 1.0  2005/08/22 11:00:00  Mitul
  no message

*/

#ifndef __ITEMDYNAMIC_H__
#define __ITEMDYNAMIC_H__

#include "Item.h"
#include <vector>

enum RAINWINDOW_TYPE;
enum REPEAT_TYPE
{
	REPEAT_TYPE_NO,
	REPEAT_TYPE_ALL,
	REPEAT_TYPE_CURRENT_MONTH,
	REPEAT_TYPE_VIEW_MONTH
};

class CItemDynamic : public CItem
{
public:
	CItemDynamic();
	virtual ~CItemDynamic();

	virtual void Initialize();
	virtual void ReadSettings(const char* filename, const char* section);
	virtual void WriteSettings();

	virtual void Paint(CImage& background, POINT offset) = 0;
	virtual int GetX() { return m_X; };
	virtual int GetY() { return m_Y; };
	virtual int GetW() { return GetRasterizer() ? GetRasterizer()->GetWidth() : 0; };
	virtual int GetH() { return GetRasterizer() ? GetRasterizer()->GetHeight() : 0; };

	bool IsIncludeSize() { return m_IncludeSize; };
	REPEAT_TYPE GetRepeatType() { return m_RepeatType; };

	RAINWINDOW_TYPE GetWinType() { return m_WinType; }
	
	virtual LPCTSTR ToString() { return NULL; };

protected:
	RAINWINDOW_TYPE m_WinType;
	int m_X;
	int m_Y;
	REPEAT_TYPE m_RepeatType;         // 0 - No repeat, 1 - Repeat All, 2 - Repeat Once
	bool m_IncludeSize;
};

#endif
