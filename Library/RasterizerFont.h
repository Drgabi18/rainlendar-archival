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
  $Header: /home/cvsroot/Rainlendar/Library/RasterizerFont.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: RasterizerFont.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.9  2005/03/01 18:50:45  rainy
  no message

  Revision 1.8  2004/11/06 13:38:59  rainy
  no message

  Revision 1.7  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.6  2003/10/27 17:39:13  Rainy
  Separated drawing so that it can be used outside this class.

  Revision 1.5  2002/11/25 17:01:27  rainy
  The buffers are now CImages instead of HBITMAPs.

  Revision 1.4  2002/11/12 18:10:18  rainy
  Added support for real alpha.

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

	virtual CRasterizer::TYPE GetType() { return CRasterizer::TYPE_FONT; };

	void SetFont(const std::string& FontName, bool strikeOut = false);
	void SetColor(COLORREF color) { m_Color = color; };
	void CreateStringTable(const std::string& text, int count);

	void UpdateDimensions(const char* defaultString = NULL);
	void GetTextSize(const char* text, RECT* rect, bool multiline);

	void Paint(CImage& background, int X, int Y, int W, int H, int Index);
	void WriteText(CImage& background, int X, int Y, int W, int H, const char* text, bool multiline, bool strikeout);

	static HFONT CreateFont(const std::string& FontName, bool strikeOut = false);
	static void ClearBuffers() { c_AlphaBuffer.Clear(); c_ColorBuffer.Clear(); };
	static CImage& GetAlphaBuffer() { return c_AlphaBuffer; };
	static CImage& GetColorBuffer() { return c_ColorBuffer; };
	static void CreateBuffers(int width, int height);
	static void DrawBuffers(CImage& doubleBuffer, int x = 0, int y = 0);

protected:
	std::vector<std::string> m_StringTable;
	HFONT m_Font;
	HFONT m_StrikeOutFont;
	COLORREF m_Color;

	static CImage c_AlphaBuffer;
	static CImage c_ColorBuffer;
};

#endif
