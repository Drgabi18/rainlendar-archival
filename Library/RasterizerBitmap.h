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
  $Header: /home/cvsroot/Rainlendar/Library/RasterizerBitmap.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: RasterizerBitmap.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.7  2005/03/01 18:50:45  rainy
  no message

  Revision 1.6  2004/11/06 13:38:59  rainy
  no message

  Revision 1.5  2002/11/25 17:02:07  rainy
  Cleaned up the code.

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

	virtual CRasterizer::TYPE GetType() { return CRasterizer::TYPE_BITMAP; };

	void Load(const std::string& filename);
	void SetNumOfComponents(int Number) { m_NumOfComponents = Number; };
	void SetSeparation(int Number) { m_Separation = Number; };
	void SetDigits(int Number) { m_Digits = Number; };
	void Paint(CImage& background, int X, int Y, int W, int H, int Index);

	static bool LoadImage(const std::string& filename, CImage* image);

protected:
	int m_NumOfComponents;
	int m_Separation;
	int m_Digits;

	CImage m_Image;
};

#endif
