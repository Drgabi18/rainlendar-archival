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
  $Header: /home/cvsroot/Rainlendar/Plugin/MessageWindow.h,v 1.3 2004/04/24 11:22:39 rainy Exp $

  $Log: MessageWindow.h,v $
  Revision 1.3  2004/04/24 11:22:39  rainy
  Added show and hide.

  Revision 1.2  2004/01/10 15:14:07  rainy
  Added ClearEvents()

  Revision 1.1  2003/12/20 21:26:39  Rainy
  Initial version

*/

#ifndef __MESSAGEWINDOW_H__
#define __MESSAGEWINDOW_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "RainWindow.h"
#include "Background.h"
#include "RasterizerFont.h"

class CMessageWindow: public CRainWindow
{
public:
	CMessageWindow();
	~CMessageWindow();

	virtual bool Initialize(HWND parent, HINSTANCE instance);
	virtual void Refresh(bool lite);

	void ClearEvents();

	void ShowEvents(const std::vector<CEventMessage*>& events);

	virtual void ShowWindow(bool activate);
	virtual void HideWindow();

	virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnRButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnTimer(WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual SIZE CalcWindowSize();
	virtual void DrawWindow();

private:
	SIZE CalcTextSize();
	
	CBackground m_MessageBoxBackground;
	CRasterizerFont* m_ItemRasterizer;
	CImage m_ItemImage;

	std::vector<CEventMessage*> m_EventList;
	std::vector<int> m_Heights;

	int m_TextWidth;
	int m_TextHeight;

	bool m_Visible;

	DWORD m_StopSnoozing;
};

#endif

