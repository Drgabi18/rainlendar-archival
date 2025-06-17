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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/RasterizerBitmap.h,v 1.4 2002/11/12 18:10:23 rainy Exp $

  $Log: RasterizerBitmap.h,v $
  Revision 1.4  2002/11/12 18:10:23  rainy
  Added support for real alpha.

  Revision 1.3  2002/08/03 16:08:59  rainy
  Added support for profiles.

  Revision 1.2  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef __RASTERIZERBITMAP_H__
#define __RASTERIZERBITMAP_H__

#include "Rasterizer.h"
#include "Image.h"

class CRasterizerBitmap : public CRasterizer  
{
public:
	CRasterizerBitmap();
	virtual ~CRasterizerBitmap();

	void Load(const std::string& filename);
	void SetNumOfComponents(int Number) { m_NumOfComponents = Number; };
	void SetSeparation(int Number) { m_Separation = Number; };
	void Paint(CImage& background, int X, int Y, int W, int H, int Index);

protected:
	int m_NumOfComponents;
	int m_Separation;
	CImage m_Image;
};

#endif
