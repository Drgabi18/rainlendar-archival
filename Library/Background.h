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
  $Header: /home/cvsroot/Rainlendar/Library/Background.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: Background.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.12  2005/03/01 18:44:33  rainy
  Added loadimage to Initialize

  Revision 1.11  2004/12/05 18:34:54  rainy
  Changed wallpaper polling.

  Revision 1.10  2004/11/06 13:38:58  rainy
  no message

  Revision 1.9  2004/01/28 18:07:53  rainy
  no message

  Revision 1.8  2003/10/27 17:36:12  Rainy
  Settings are not read directly from the config anymore.

  Revision 1.7  2003/06/15 09:42:42  Rainy
  Added support for multiple calendars.

  Revision 1.6  2002/11/12 18:01:41  rainy
  Modified to use the CImage class.

  Revision 1.5  2002/08/03 16:22:37  rainy
  Changed the background handling to cache the wallpaper when
  the window is moved.

  Revision 1.4  2002/05/23 17:33:42  rainy
  Removed all MFC stuff

  Revision 1.3  2002/02/27 18:59:00  rainy
  Fixed multimonitor stuff.
  Added support for background stretching.

  Revision 1.2  2002/01/15 17:59:44  rainy
  Now uses different way to get the desktop image.

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

#include "Image.h"

enum BACKGROUND_MODE  
{
	MODE_TILE,
	MODE_COPY,
	MODE_STRETCH,
	MODE_SOLID,
	MODE_RECT
};

struct BackgroundCreateStruct
{
	POINT pos;
	SIZE size;
	BACKGROUND_MODE mode;
	COLORREF solidColor;
	BOOL solidBevel;
	RECT resizeRect;
};

class CBackground
{
public:
	CBackground();
	~CBackground();

	bool Create(BackgroundCreateStruct& bcs);
	void Paint(CImage& background);

	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }

	int GetImageWidth() { return m_BGImage.GetBitmap() != NULL ? m_BGImage.GetWidth() : 0; }
	int GetImageHeight() { return m_BGImage.GetBitmap() != NULL ? m_BGImage.GetHeight() : 0; }

	void Initialize(LPCTSTR filename, bool loadImage = true);

	bool IsValid() { return (m_Image.GetBitmap() != NULL) || (m_BGImage.GetBitmap() != NULL); }
	bool HasAlpha() { return m_Alpha; }

	void UpdateWallpaper(int X, int Y);

	static bool PollWallpaper(bool set);
	static void FlushWallpaper();
	static const char* CBackground::GetWallpaperFile(int* tile, int* style);

private:
	void Load(const std::string& Filename);
	HBITMAP CopyBackground(int X, int Y, int Width, int Height);

	static HBITMAP GetWallpaper(int X, int Y, int Width, int Height);
	static CImage c_Wallpaper;	// The current wallpaper picture
	static std::string c_WallpaperName;
	static FILETIME c_WallpaperTime;

	std::string m_Filename;
	int m_Width;
	int m_Height;
	bool m_Alpha;

	CImage m_BGImage;
	CImage m_Wallpaper;
	CImage m_Image;
};

#endif
