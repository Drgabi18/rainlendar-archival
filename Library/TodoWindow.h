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
  $Header: /home/cvsroot/Rainlendar/Library/TodoWindow.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: TodoWindow.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.5  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.4  2004/11/06 13:38:59  rainy
  no message

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
#include "EventManager.h"
#include "Background.h"
#include "RasterizerFont.h"

class CTodoWindow: public CRainWindow
{
public:
	CTodoWindow();
	~CTodoWindow();

	virtual bool Initialize(HWND parent, HINSTANCE instance);
	virtual void ReadSettings(const char* filename, const char* section);
	virtual void WriteSettings();

	virtual void Refresh();
	virtual void Redraw();
	virtual RAINWINDOW_TYPE GetType() { return RAINWINDOW_TYPE_TODO; };

	virtual int GetY();

	LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	LRESULT OnMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Todo
    const std::string& GetTodoBitmapName() { return m_TodoBitmapName; };
	RECT GetTodoBitmapMargins() { return m_TodoBitmapMargins; };
	RECT GetTodoTextMargins() { return m_TodoTextMargins; };
	int GetTodoW() { return m_TodoW; };
    const std::string& GetTodoFont() { return m_TodoFont; };
	COLORREF GetTodoFontColor() { return m_TodoFontColor; };
	int GetTodoSeparation() { return m_TodoSeparation; };
    const std::string& GetTodoItemBitmapName() { return m_TodoItemBitmapName; };
	POINT GetTodoItemOffset() { return m_TodoItemOffset; };
	CRasterizer::ALIGN GetTodoItemAlign() { return m_TodoItemAlign; };

	void SetTodoBitmapName(const std::string& TodoBitmapName ) { m_TodoBitmapName=TodoBitmapName; };
	void SetTodoBitmapMargins(RECT TodoBitmapMargins) { m_TodoBitmapMargins=TodoBitmapMargins; };
	void SetTodoTextMargins(RECT TodoTextMargins) { m_TodoTextMargins=TodoTextMargins; };
	void SetTodoW(int TodoW ) { m_TodoW=TodoW; };
	void SetTodoFont(const std::string& TodoFont ) { m_TodoFont=TodoFont; };
	void SetTodoFontColor(COLORREF TodoFontColor ) { m_TodoFontColor=TodoFontColor; };
	void SetTodoSeparation(int TodoSeparation) { m_TodoSeparation=TodoSeparation; };
	void SetTodoItemBitmapName(const std::string& TodoItemBitmapName) { m_TodoItemBitmapName=TodoItemBitmapName; };
	void SetTodoItemOffset(POINT TodoItemOffset) { m_TodoItemOffset=TodoItemOffset; };
	void SetTodoItemAlign(CRasterizer::ALIGN TodoItemAlign) { m_TodoItemAlign=TodoItemAlign; };

	void ResetSettings();
	
protected:
	virtual SIZE CalcWindowSize();
	virtual void DrawWindow();

private:

	GUID* HitTest(int x, int y);
	int CalcTextHeight();

	GUID* m_SelectedTodo;
	int m_TotalHeight;
	std::vector<int> m_Heights;

	CBackground m_TodoBackground;
	CRasterizerFont* m_ItemRasterizer;
	CImage m_ItemImage;

	std::string m_TodoBitmapName;
	RECT m_TodoBitmapMargins;
	RECT m_TodoTextMargins;
	int m_TodoW;
	COLORREF m_TodoFontColor;
	std::string m_TodoFont;
	int m_TodoSeparation;
	std::string m_TodoItemBitmapName;
	POINT m_TodoItemOffset;
	CRasterizer::ALIGN m_TodoItemAlign;
};

#endif

