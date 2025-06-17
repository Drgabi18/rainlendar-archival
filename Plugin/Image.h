/*
  Copyright (C) 2002 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Plugin/Image.h,v 1.6 2004/04/24 11:18:51 rainy Exp $

  $Log: Image.h,v $
  Revision 1.6  2004/04/24 11:18:51  rainy
  Fixed alpha blend bug in WIn9x.

  Revision 1.5  2004/01/10 15:15:26  rainy
  Images can be loaded from resources too.

  Revision 1.4  2003/10/27 17:37:31  Rainy
  Added new resize mode.

  Revision 1.3  2003/08/09 16:36:25  Rainy
  Added a check if the file exists.

  Revision 1.2  2002/11/25 17:03:55  rainy
  Small fixes.

  Revision 1.1  2002/11/12 18:00:38  rainy
  Initial version

*/

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <windows.h>
#include <string>

enum IMAGE_RESIZE_TYPE
{
	IMAGE_RESIZE_TYPE_TILE,
	IMAGE_RESIZE_TYPE_STRETCH
};

class CImage
{
public:
	CImage();
	~CImage();

	bool Load(const std::string& filename);
	bool LoadResource(HINSTANCE instance, int resourceID);
	void Create(HBITMAP bitmap, HBITMAP alpha, BYTE alphaValue);
	void Create(int width, int height, BYTE alphaValue);
	void Blit(const CImage& source, int dx, int dy, int sx, int sy, int w, int h);
	void Resize(int w, int h, IMAGE_RESIZE_TYPE type, RECT* margins);
	void Clear();
	void MultiplyAlpha();

	bool HasAlpha() const { return m_Alpha; };
	int GetWidth() const { return m_Width; };
	int GetHeight() const { return m_Height; };
	HBITMAP GetBitmap() const { return m_Image; };

private:
	bool LoadPNG(const std::string& filename);
	void AlphaBlit(const CImage& source, int dx, int dy, int sx, int sy, int w, int h);
	void AlphaBlit(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
		           HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc);

	int m_Width;
	int m_Height;
	bool m_Alpha;
	HBITMAP m_Image;
	BYTE* m_Bits;
};

#endif
