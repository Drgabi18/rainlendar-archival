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
  $Header: /home/cvsroot/Rainlendar/Library/ItemMonth.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: ItemMonth.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.6  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.5  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.4  2003/06/15 09:49:12  Rainy
  Added support for multiple calendars.

  Revision 1.3  2002/11/12 18:11:35  rainy
  The interface of Paint changed a little.

  Revision 1.2  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef __ITEMMONTH_H__
#define __ITEMMONTH_H__

#include "Item.h"

class CItemMonth : public CItem  
{
public:
	CItemMonth();
	virtual ~CItemMonth();

	virtual void Initialize();
	virtual void ReadSettings(const char* filename, const char* section);
	virtual void WriteSettings();

	void Paint(CImage& background, POINT offset);

	int GetX();
	int GetY();
	int GetW();
	int GetH();

	// Month
    const std::string& GetMonthBitmapName() { return m_MonthBitmapName; };
	int GetMonthX() { return m_MonthX; };
	int GetMonthY() { return m_MonthY; };
	CRasterizer::ALIGN GetMonthAlign() { return m_MonthAlign; };
    const std::string& GetMonthFont() { return m_MonthFont; };
	CRasterizer::TYPE GetMonthRasterizer() { return m_MonthRasterizer; };
	COLORREF GetMonthFontColor() { return m_MonthFontColor; };
    const std::string& GetMonthNames() { return m_MonthNames; };

	void SetMonthBitmapName(const std::string& MonthBitmapName ) { m_MonthBitmapName=MonthBitmapName; };
	void SetMonthX(int MonthX ) { m_MonthX=MonthX; };
	void SetMonthY(int MonthY ) { m_MonthY=MonthY; };
	void SetMonthAlign(CRasterizer::ALIGN MonthAlign ) { m_MonthAlign=MonthAlign; };
	void SetMonthFont(const std::string& MonthFont ) { m_MonthFont=MonthFont; };
	void SetMonthRasterizer(CRasterizer::TYPE MonthRasterizer ) { m_MonthRasterizer=MonthRasterizer; };
	void SetMonthFontColor(COLORREF MonthFontColor ) { m_MonthFontColor=MonthFontColor; };
	void SetMonthNames(const std::string& MonthNames ) { m_MonthNames=MonthNames; };

private:
	std::string m_MonthBitmapName;	// Name of the month bitmap
	int m_MonthX;				// Position of the month
	int m_MonthY;
	CRasterizer::ALIGN m_MonthAlign;
	CRasterizer::TYPE m_MonthRasterizer;
	std::string m_MonthFont;
	COLORREF m_MonthFontColor;
	std::string m_MonthNames;
};

#endif
