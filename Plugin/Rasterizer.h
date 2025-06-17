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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Rasterizer.h,v 1.3 2002/02/27 18:49:52 rainy Exp $

  $Log: Rasterizer.h,v $
  Revision 1.3  2002/02/27 18:49:52  rainy
  Added mode alignments

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
	enum ALIGN
	{
		// The order (and values) must be like this so that it's backwards compatible
		ALIGN_LEFT    = 0x00,
		ALIGN_RIGHT   = 0x01,
		ALIGN_HCENTER = 0x02,
		ALIGN_TOP     = 0x00,
		ALIGN_VCENTER = 0x10,
		ALIGN_BOTTOM  = 0x20
	};

	enum TYPE 
	{
		TYPE_NONE,
		TYPE_BITMAP,
		TYPE_FONT
	};

	CRasterizer();
	virtual ~CRasterizer();

	int GetAlign() { return m_Align; };
	void SetAlign(int Align ) { m_Align=Align; };

	virtual void Paint(CDC& dc, int X, int Y, int W, int H, int Index) = 0;

	int GetHeight() { return m_Height; };
	int GetWidth() { return m_Width; };

protected:
	int m_Align;
	int m_Width;
	int m_Height;
};

#endif // !defined(AFX_RASTERIZER_H__C7D0FDC1_F062_11D3_92A3_0080AD90417B__INCLUDED_)
