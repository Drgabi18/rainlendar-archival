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
  $Header: /home/cvsroot/Rainlendar/Plugin/ItemYear.h,v 1.5 2004/06/10 16:22:25 rainy Exp $

  $Log: ItemYear.h,v $
  Revision 1.5  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.4  2003/06/15 09:49:13  Rainy
  Added support for multiple calendars.

  Revision 1.3  2002/11/12 18:11:33  rainy
  The interface of Paint changed a little.

  Revision 1.2  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#ifndef __ITEMYEAR_H__
#define __ITEMYEAR_H__

#include "Item.h"

class CItemYear : public CItem  
{
public:
	CItemYear();
	virtual ~CItemYear();

	virtual void Initialize();
	virtual void ReadSettings(const char* filename, const char* section);
	virtual void WriteSettings();

	void Paint(CImage& background, POINT offset);

	int GetX();
	int GetY();
	int GetW();
	int GetH();

	// Year
    const std::string& GetYearBitmapName() { return m_YearBitmapName; };
	int GetYearX() { return m_YearX; };
	int GetYearY() { return m_YearY; };
	CRasterizer::ALIGN GetYearAlign() { return m_YearAlign; };
    const std::string& GetYearFont() { return m_YearFont; };
	CRasterizer::TYPE GetYearRasterizer() { return m_YearRasterizer; };
	COLORREF GetYearFontColor() { return m_YearFontColor; };
	int GetYearSeparation() { return m_YearSeparation; };

	void SetYearBitmapName(const std::string& YearBitmapName ) { m_YearBitmapName=YearBitmapName; };
	void SetYearX(int YearX ) { m_YearX=YearX; };
	void SetYearY(int YearY ) { m_YearY=YearY; };
	void SetYearAlign(CRasterizer::ALIGN YearAlign ) { m_YearAlign=YearAlign; };
	void SetYearFont(const std::string& YearFont ) { m_YearFont=YearFont; };
	void SetYearRasterizer(CRasterizer::TYPE YearRasterizer ) { m_YearRasterizer=YearRasterizer; };
	void SetYearFontColor(COLORREF YearFontColor ) { m_YearFontColor=YearFontColor; };
	void SetYearSeparation(int separation) { m_YearSeparation = separation; };

private:
	std::string m_YearBitmapName;	// Name of the year bitmap
	int m_YearX;				// Position of the year
	int m_YearY;
	CRasterizer::ALIGN m_YearAlign;
	CRasterizer::TYPE m_YearRasterizer;
	std::string m_YearFont;
	COLORREF m_YearFontColor;
	int m_YearSeparation;		// Separation between components

};

#endif
