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
  $Header: /home/cvsroot/Rainlendar/Library/ItemWeekdays.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: ItemWeekdays.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.6  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.5  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.4  2003/06/15 09:49:13  Rainy
  Added support for multiple calendars.

  Revision 1.3  2002/11/12 18:11:34  rainy
  The interface of Paint changed a little.

  Revision 1.2  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef __ITEMWEEKDAYS_H__
#define __ITEMWEEKDAYS_H__

#include "Item.h"

class CItemWeekdays : public CItem  
{
public:
	CItemWeekdays();
	virtual ~CItemWeekdays();

	virtual void Initialize();
	virtual void ReadSettings(const char* filename, const char* section);
	virtual void WriteSettings();

	void Paint(CImage& background, POINT offset);

	// Weekdays
    const std::string& GetWeekdaysBitmapName() { return m_WeekdaysBitmapName; };
	CRasterizer::ALIGN GetWeekdaysAlign() { return m_WeekdaysAlign; };
    const std::string& GetWeekdaysFont() { return m_WeekdaysFont; };
	CRasterizer::TYPE GetWeekdaysRasterizer() { return m_WeekdaysRasterizer; };
	COLORREF GetWeekdaysFontColor() { return m_WeekdaysFontColor; };
    const std::string& GetWeekdayNames() { return m_WeekdayNames; };

	void SetWeekdaysBitmapName(const std::string& WeekdaysBitmapName ) { m_WeekdaysBitmapName=WeekdaysBitmapName; };
	void SetWeekdaysAlign(CRasterizer::ALIGN WeekdaysAlign ) { m_WeekdaysAlign=WeekdaysAlign; };
	void SetWeekdaysFont(const std::string& WeekdaysFont ) { m_WeekdaysFont=WeekdaysFont; };
	void SetWeekdaysRasterizer(CRasterizer::TYPE WeekdaysRasterizer ) { m_WeekdaysRasterizer=WeekdaysRasterizer; };
	void SetWeekdaysFontColor(COLORREF WeekdaysFontColor ) { m_WeekdaysFontColor=WeekdaysFontColor; };
	void SetWeekdayNames(const std::string& WeekdayNames ) { m_WeekdayNames=WeekdayNames; };

private:
	std::string m_WeekdaysBitmapName;	// Name of the today bitmap
	CRasterizer::ALIGN m_WeekdaysAlign;
	CRasterizer::TYPE m_WeekdaysRasterizer;
	std::string m_WeekdaysFont;
	COLORREF m_WeekdaysFontColor;
	std::string m_WeekdayNames;
};

#endif
