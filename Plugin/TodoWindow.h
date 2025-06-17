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
  $Header: /home/cvsroot/Rainlendar/Plugin/TodoWindow.h,v 1.3 2003/12/20 22:15:18 rainy Exp $

  $Log: TodoWindow.h,v $
  Revision 1.3  2003/12/20 22:15:18  rainy
  Added OnPowerBroadcast.
  Refreshing is not done if the window is not enabled.

  Revision 1.2  2003/10/27 19:50:43  Rainy
  Fixed the WndProc (it cannot be a member).

  Revision 1.1  2003/10/27 17:34:54  Rainy
  Initial version.

*/

#ifndef __TODOWINDOW_H__
#define __TODOWINDOW_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "RainWindow.h"
#include "TodoManager.h"
#include "Background.h"
#include "RasterizerFont.h"

class CTodoWindow: public CRainWindow
{
public:
	CTodoWindow();
	~CTodoWindow();

	CTodoManager& GetTodoManager() { return m_TodoManager; }

	virtual bool Initialize(HWND parent, HINSTANCE instance);
	virtual void Refresh(bool lite);

	LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	LRESULT OnMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnPowerBroadcast(WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual SIZE CalcWindowSize();
	virtual void DrawWindow();

private:
	int HitTest(int x, int y);
	int CalcTextHeight();

	int m_SelectedTodo;
	int m_TotalHeight;
	std::vector<int> m_Heights;

	CBackground m_TodoBackground;
	CRasterizerFont* m_ItemRasterizer;
	CImage m_ItemImage;

	CTodoManager m_TodoManager;
};

#endif

