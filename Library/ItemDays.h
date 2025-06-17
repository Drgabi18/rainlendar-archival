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
  $Header: /home/cvsroot/Rainlendar/Library/ItemDays.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: ItemDays.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.8  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.7  2004/11/06 13:38:59  rainy
  no message

  Revision 1.6  2004/06/10 16:22:25  rainy
  New address to repository.

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

	virtual void Initialize();
	virtual void ReadSettings(const char* filename, const char* section);
	virtual void WriteSettings();

	void Paint(CImage& background, POINT offset);

	int GetX();
	int GetY();
	int GetW();
	int GetH();

	// Days
	int GetDaysH() { return m_DaysH; };
	int GetDaysW() { return m_DaysW; };
	int GetDaysY() { return m_DaysY; };
	int GetDaysX() { return m_DaysX; };
    const std::string& GetDaysBitmapName() { return m_DaysBitmapName; };
    const std::string& GetDaysWeekendBitmapName() { return m_DaysWeekendBitmapName; };
	int GetDaysNumOfComponents() { return m_DaysNumOfComponents; };
	CRasterizer::ALIGN GetDaysAlign() { return m_DaysAlign; };
    const std::string& GetDaysFont() { return m_DaysFont; };
	CRasterizer::TYPE GetDaysRasterizer() { return m_DaysRasterizer; };
	COLORREF GetDaysFontColor() { return m_DaysFontColor; };
	COLORREF GetDaysWeekendFontColor() { return m_DaysWeekendFontColor; };
	bool GetDaysIgnoreToday() { return m_DaysIgnoreToday; };
	bool GetDaysIgnoreEvent() { return m_DaysIgnoreEvent; };
	int GetDaysSeparation() { return m_DaysSeparation; };
	CConfig::DAY_LAYOUT GetDaysLayout() { return m_DaysLayout; };

	void SetDaysH(int DaysH ) { m_DaysH=DaysH; };
	void SetDaysW(int DaysW ) { m_DaysW=DaysW; };
	void SetDaysY(int DaysY ) { m_DaysY=DaysY; };
	void SetDaysX(int DaysX ) { m_DaysX=DaysX; };
	void SetDaysBitmapName(const std::string& DaysBitmapName ) { m_DaysBitmapName=DaysBitmapName; };
	void SetDaysWeekendBitmapName(const std::string& DaysBitmapName ) { m_DaysWeekendBitmapName=DaysBitmapName; };
	void SetDaysNumOfComponents(int DaysNumOfComponents ) { m_DaysNumOfComponents=DaysNumOfComponents; };
	void SetDaysAlign(CRasterizer::ALIGN DaysAlign ) { m_DaysAlign=DaysAlign; };
	void SetDaysFont(const std::string& DaysFont ) { m_DaysFont=DaysFont; };
	void SetDaysRasterizer(CRasterizer::TYPE DaysRasterizer ) { m_DaysRasterizer=DaysRasterizer; };
	void SetDaysFontColor(COLORREF DaysFontColor ) { m_DaysFontColor=DaysFontColor; };
	void SetDaysWeekendFontColor(COLORREF DaysFontColor ) { m_DaysWeekendFontColor=DaysFontColor; };
	void SetDaysIgnoreToday(bool DaysIgnoreToday ) { m_DaysIgnoreToday=DaysIgnoreToday; };
	void SetDaysIgnoreEvent(bool DaysIgnoreEvent ) { m_DaysIgnoreEvent=DaysIgnoreEvent; };
	void SetDaysSeparation(int separation) { m_DaysSeparation = separation; };
	void SetDaysLayout(CConfig::DAY_LAYOUT layout) { m_DaysLayout = layout; };

private:
	CRasterizer* m_WeekendRasterizer;

	std::string m_DaysBitmapName;	// Name of the day number bitmap
	std::string m_DaysWeekendBitmapName;	// Name of the day number bitmap
	int m_DaysX;				// Size & position of the days
	int m_DaysY;
	int m_DaysW;
	int m_DaysH;
	int m_DaysNumOfComponents;	// Components in the bitmap
	CRasterizer::ALIGN m_DaysAlign;
	CRasterizer::TYPE m_DaysRasterizer;
	std::string m_DaysFont;
	COLORREF m_DaysFontColor;
	COLORREF m_DaysWeekendFontColor;
	bool m_DaysIgnoreToday;
	bool m_DaysIgnoreEvent;
	int m_DaysSeparation;		// Separation between components
	CConfig::DAY_LAYOUT m_DaysLayout;
};

#endif
