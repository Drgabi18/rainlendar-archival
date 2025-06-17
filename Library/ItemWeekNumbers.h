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
  $Header: /home/cvsroot/Rainlendar/Library/ItemWeekNumbers.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: ItemWeekNumbers.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.6  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.5  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.4  2003/06/15 09:49:13  Rainy
  Added support for multiple calendars.

  Revision 1.3  2002/11/12 18:11:33  rainy
  The interface of Paint changed a little.

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

	virtual void Initialize();
	virtual void ReadSettings(const char* filename, const char* section);
	virtual void WriteSettings();

	void Paint(CImage& background, POINT offset);

	// Week numbers
    const std::string& GetWeekNumbersBitmapName() { return m_WeekNumbersBitmapName; };
	CRasterizer::ALIGN GetWeekNumbersAlign() { return m_WeekNumbersAlign; };
    const std::string& GetWeekNumbersFont() { return m_WeekNumbersFont; };
	CRasterizer::TYPE GetWeekNumbersRasterizer() { return m_WeekNumbersRasterizer; };
	COLORREF GetWeekNumbersFontColor() { return m_WeekNumbersFontColor; };
	int GetWeekNumbersNumOfComponents() { return m_WeekNumbersNumOfComponents; };
	int GetWeekNumbersSeparation() { return m_WeekNumbersSeparation; };

	void SetWeekNumbersBitmapName(const std::string& WeekNumbersBitmapName ) { m_WeekNumbersBitmapName=WeekNumbersBitmapName; };
	void SetWeekNumbersAlign(CRasterizer::ALIGN WeekNumbersAlign ) { m_WeekNumbersAlign=WeekNumbersAlign; };
	void SetWeekNumbersFont(const std::string& WeekNumbersFont ) { m_WeekNumbersFont=WeekNumbersFont; };
	void SetWeekNumbersRasterizer(CRasterizer::TYPE WeekNumbersRasterizer ) { m_WeekNumbersRasterizer=WeekNumbersRasterizer; };
	void SetWeekNumbersFontColor(COLORREF WeekNumbersFontColor ) { m_WeekNumbersFontColor=WeekNumbersFontColor; };
	void SetWeekNumbersNumOfComponents(int WeekNumbersNumOfComponents ) { m_WeekNumbersNumOfComponents=WeekNumbersNumOfComponents; };
	void SetWeekNumbersSeparation(int separation) { m_WeekNumbersSeparation = separation; };

private:
	std::string m_WeekNumbersBitmapName;	// Name of the today bitmap
	CRasterizer::ALIGN m_WeekNumbersAlign;
	CRasterizer::TYPE m_WeekNumbersRasterizer;
	std::string m_WeekNumbersFont;
	COLORREF m_WeekNumbersFontColor;
	int m_WeekNumbersNumOfComponents;	// Components in the bitmap
	int m_WeekNumbersSeparation;		// Separation between components
};

#endif
