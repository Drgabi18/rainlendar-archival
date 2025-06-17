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
  $Header: /home/cvsroot/Rainlendar/Library/Item.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: Item.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.9  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.8  2004/11/06 13:38:59  rainy
  no message

  Revision 1.7  2004/06/10 16:22:25  rainy
  New address to repository.

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
#define DAYTYPE_NORMAL 0
#define DAYTYPE_TODAY 1
#define DAYTYPE_EVENT 2

class CRainWindow;

class CItem  
{
public:
	CItem();
	virtual ~CItem();

	virtual void Initialize() = 0;
	virtual void ReadSettings(const char* filename, const char* section);
	virtual void WriteSettings() = 0;

	virtual void Paint(CImage& background, POINT offset) = 0;

	CRasterizer* GetRasterizer() { return m_Rasterizer; };
	void SetRasterizer(CRasterizer* Rasterizer);

	virtual bool MouseUp(POINT pos, CRainWindow* window) { return false; };
	virtual bool MouseDown(POINT pos, CRainWindow* window) { return false; };
	virtual bool MouseMove(POINT pos, CRainWindow* window) { return false; };

	virtual int GetX() { return 0; };
	virtual int GetY() { return 0; };
	virtual int GetW() { return 0; };
	virtual int GetH() { return 0; };

	bool IsEnabled() { return m_Enabled; };
	void Enable() { m_Enabled = true; };
	void Disable() { m_Enabled = false; };
	void SetEnabled(bool state) { m_Enabled = state; };

	virtual bool NeedsUpdating() { return false; };

	std::string& GetSection() { return m_Section; };

protected:
	CRasterizer* m_Rasterizer;
	std::string m_Section;
	std::string m_SettingsFile;
	bool m_Enabled;
};

#endif
