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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Rasterizer.h,v 1.2 2002/01/10 16:41:17 rainy Exp $

  $Log: Rasterizer.h,v $
  Revision 1.2  2002/01/10 16:41:17  rainy
  Added vertical positioning.

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#if !defined(AFX_RASTERIZER_H__C7D0FDC1_F062_11D3_92A3_0080AD90417B__INCLUDED_)
#define AFX_RASTERIZER_H__C7D0FDC1_F062_11D3_92A3_0080AD90417B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRasterizer  
{
public:
	enum ALIGN {
		ALIGN_LEFT = 0,
		ALIGN_RIGHT = 1,
		ALIGN_CENTER = 2,
		ALIGN_TOP = 4,
		ALIGN_BOTTOM = 8
	};

	enum TYPE {
		TYPE_NONE,
		TYPE_BITMAP,
		TYPE_FONT
	};

	CRasterizer();
	virtual ~CRasterizer();

	ALIGN GetAlign() { return m_Align; };
	void SetAlign(ALIGN Align ) { m_Align=Align; };

	virtual void Paint(CDC& dc, int X, int Y, int W, int H, int Index) = 0;

	int GetHeight() { return m_Height; };
	int GetWidth() { return m_Width; };

protected:
	ALIGN m_Align;

	int m_Width;
	int m_Height;
};

#endif // !defined(AFX_RASTERIZER_H__C7D0FDC1_F062_11D3_92A3_0080AD90417B__INCLUDED_)
