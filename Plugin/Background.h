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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Background.h,v 1.5 2002/08/03 16:22:37 rainy Exp $

  $Log: Background.h,v $
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

class CBackground
{
public:
	enum MODE 
	{
		MODE_TILE,
		MODE_COPY,
		MODE_STRETCH
	};

	CBackground();
	~CBackground();

	void Load(std::string Filename);
	void Create(int X, int Y, int Width, int Height);
	void CopyBackground(int X, int Y, int Width, int Height);
	void Paint(HDC dc);

	int GetWidth() { return m_Width; };
	int GetHeight() { return m_Height; };

	void Initialize();

	bool HasAlpha() { return m_Alpha; };

	void UpdateWallpaper(int X, int Y);

	static void FlushWallpaper();

private:
	static HBITMAP GetWallpaper(int X, int Y, int Width, int Height);
	static HBITMAP c_Wallpaper;	// The current wallpaper picture

	std::string m_Filename;
	int m_Width;
	int m_Height;
	bool m_Alpha;
	HBITMAP m_AlphaImage;
	HBITMAP m_Image;
	HBITMAP m_Background;	// The background picture
};

#endif
