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
  $Header: /home/cvsroot/Rainlendar/Plugin/MessageWindow.h,v 1.5 2004/12/05 18:25:05 rainy Exp $

  $Log: MessageWindow.h,v $
  Revision 1.5  2004/12/05 18:25:05  rainy
  AddEvent takes pointer.
  Implemented OnMouseMove.

  Revision 1.4  2004/11/06 13:38:59  rainy
  no message

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
	virtual void ReadSettings(const char* filename, const char* section);
	virtual void WriteSettings();

	virtual void Refresh();
	virtual void Redraw();
	virtual RAINWINDOW_TYPE GetType() { return RAINWINDOW_TYPE_MESSAGEBOX; };

	void ClearEvents();
	void AddEvent(GUID* guid, CFileTime* start);

	virtual void ShowWindow(bool activate);
	virtual void HideWindow();

	virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnRButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnTimer(WPARAM wParam, LPARAM lParam);

	virtual void UpdateTransparency(UINT transparency);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// MessageBox
    const std::string& GetMessageBoxBitmapName() { return m_MessageBoxBitmapName; };
	RECT GetMessageBoxBitmapMargins() { return m_MessageBoxBitmapMargins; };
	RECT GetMessageBoxTextMargins() { return m_MessageBoxTextMargins; };
    const std::string& GetMessageBoxFont() { return m_MessageBoxFont; };
	COLORREF GetMessageBoxFontColor() { return m_MessageBoxFontColor; };
	int GetMessageBoxSeparation() { return m_MessageBoxSeparation; };
	
	void SetMessageBoxBitmapName(const std::string& MessageBoxBitmapName ) { m_MessageBoxBitmapName=MessageBoxBitmapName; };
	void SetMessageBoxBitmapMargins(RECT MessageBoxBitmapMargins) { m_MessageBoxBitmapMargins=MessageBoxBitmapMargins; };
	void SetMessageBoxTextMargins(RECT MessageBoxTextMargins) { m_MessageBoxTextMargins=MessageBoxTextMargins; };
	void SetMessageBoxFont(const std::string& MessageBoxFont ) { m_MessageBoxFont=MessageBoxFont; };
	void SetMessageBoxFontColor(COLORREF MessageBoxFontColor ) { m_MessageBoxFontColor=MessageBoxFontColor; };
	void SetMessageBoxSeparation(int MessageBoxSeparation) { m_MessageBoxSeparation=MessageBoxSeparation; };

	void ResetSettings();

protected:
	virtual SIZE CalcWindowSize();
	virtual void DrawWindow();

private:
	SIZE CalcTextSize();
	void GetTextFromEvent(CEntryEvent* event, CFileTime alarmTime, std::string& text);
	
	CBackground m_MessageBoxBackground;
	CRasterizerFont* m_ItemRasterizer;
	CImage m_ItemImage;

	CFileTime m_LastUpdate;

	std::vector<GUID> m_EventGuidList;
	std::vector<CFileTime> m_EventTimeList;
	std::vector<int> m_Heights;

	int m_TextWidth;
	int m_TextHeight;

	bool m_Visible;

	DWORD m_StopSnoozing;

	std::string m_MessageBoxBitmapName;
	RECT m_MessageBoxBitmapMargins;
	RECT m_MessageBoxTextMargins;
	COLORREF m_MessageBoxFontColor;
	std::string m_MessageBoxFont;
	int m_MessageBoxSeparation;

};

#endif

