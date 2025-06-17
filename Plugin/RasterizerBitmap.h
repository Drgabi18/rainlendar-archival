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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/RasterizerBitmap.h,v 1.2 2002/05/23 17:33:40 rainy Exp $

  $Log: RasterizerBitmap.h,v $
  Revision 1.2  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef __RASTERIZERBITMAP_H__
#define __RASTERIZERBITMAP_H__

#include "Rasterizer.h"

class CRasterizerBitmap : public CRasterizer  
{
public:
	CRasterizerBitmap();
	virtual ~CRasterizerBitmap();

	void Load(std::string filename);
	void SetNumOfComponents(int Number) { m_NumOfComponents = Number; };
	void Paint(HDC dc, int X, int Y, int W, int H, int Index);

	static bool CreateAlpha(HBITMAP Source, HBITMAP Alpha, HBITMAP Background);

protected:
	HBITMAP GetBackground(HDC dc, int X, int Y, int Width, int Height);
	void PaintAlpha(HDC dc, int X, int Y, int NumOfNums, int Index);

	int m_NumOfComponents;
	HBITMAP m_Bitmap;
	HBITMAP m_AlphaBitmap;
	bool m_Alpha;
};

#endif
