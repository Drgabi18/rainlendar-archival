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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/RasterizerFont.h,v 1.3 2002/08/03 16:07:54 rainy Exp $

  $Log: RasterizerFont.h,v $
  Revision 1.3  2002/08/03 16:07:54  rainy
  Added CreateFont and modified SetFont to use that.

  Revision 1.2  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef __RASTERIZERFONT_H__
#define __RASTERIZERFONT_H__

#include "Rasterizer.h"
#include <string>
#include <vector>

class CRasterizerFont : public CRasterizer  
{
public:
	CRasterizerFont();
	virtual ~CRasterizerFont();

	void SetFont(const std::string& FontName);
	void SetColor(COLORREF color) { m_Color = color; };
	void CreateStringTable(const std::string& text, int count);

	void UpdateDimensions(const char* defaultString = NULL);

	void Paint(HDC dc, int X, int Y, int W, int H, int Index);

	static HFONT CreateFont(const std::string& FontName);

protected:
	std::vector<std::string> m_StringTable;
	HFONT m_Font;
	COLORREF m_Color;
};

#endif
