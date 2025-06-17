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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Background.h,v 1.1.1.1 2001/10/29 18:56:24 rainy Exp $

  $Log: Background.h,v $
  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#if !defined(AFX_BACKGROUND_H__C7D0FDBE_F062_11D3_92A3_0080AD90417B__INCLUDED_)
#define AFX_BACKGROUND_H__C7D0FDBE_F062_11D3_92A3_0080AD90417B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBackground
{
public:
	CBackground();
	~CBackground();

	void Load(CString& Filename);
	void Create(int Width, int Height);
	void CopyBackground(int Width, int Height);
	void Paint(CDC& dc);

	void SetWindow(CWnd* CalendarWindow) { m_CalendarWindow=CalendarWindow; };

	int GetWidth() { return m_Width; };
	int GetHeight() { return m_Height; };

	void Initialize();

private:
	CString m_Filename;
	
	int m_Width;
	int m_Height;

	CWnd* m_CalendarWindow;

	bool m_Alpha;

	CBitmap m_AlphaImage;
	CBitmap m_Image;

	CBitmap m_Background;	// The background picture
};

#endif // !defined(AFX_BACKGROUND_H__C7D0FDBE_F062_11D3_92A3_0080AD90417B__INCLUDED_)
