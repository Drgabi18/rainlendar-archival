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
  $Header: /home/cvsroot/Rainlendar/Library/ItemButton.h,v 1.3 2005/10/14 17:05:41 rainy Exp $

  $Log: ItemButton.h,v $
  Revision 1.3  2005/10/14 17:05:41  rainy
  no message

  Revision 1.2  2005/09/08 16:09:12  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.2  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.1  2004/11/06 13:38:59  rainy
  no message

*/

#ifndef __ITEMBUTTON_H__
#define __ITEMBUTTON_H__

#include "ItemDynamic.h"

class CRasterizerBitmap;
class CRainWindow;

enum BUTTON_MOUSE_EVENT
{
	BUTTON_MOUSE_EVENT_MOVE,
	BUTTON_MOUSE_EVENT_DOWN,
	BUTTON_MOUSE_EVENT_UP
};

class CItemButton : public CItemDynamic
{
public:
	CItemButton();
	virtual ~CItemButton();

	virtual void ReadSettings(const char* filename, const char* section);

	virtual void Paint(CImage& background, POINT offset);

	bool MouseUp(POINT pos, CRainWindow* window);
	bool MouseDown(POINT pos, CRainWindow* window);
	bool MouseMove(POINT pos, CRainWindow* window);

protected:
	bool HandleMouseEvent(POINT pos, CRainWindow* window, BUTTON_MOUSE_EVENT event);

	std::string m_Command;
	int m_State;
	bool m_Clicked;
};

#endif
