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
  $Header: /home/cvsroot/Rainlendar/Library/Tooltip.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: Tooltip.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.9  2005/03/25 13:58:43  rainy
  no message

  Revision 1.8  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.7  2004/11/06 13:38:59  rainy
  no message

  Revision 1.6  2004/01/10 15:12:39  rainy
  no message

  Revision 1.5  2003/10/04 14:52:36  Rainy
  Added word wrapping.

  Revision 1.4  2003/05/25 18:08:44  Rainy
  Added tooltip separator.

  Revision 1.3  2002/11/25 17:00:29  rainy
  Tooltip timer is only restarted if the mouse position changes.

  Revision 1.2  2002/08/24 11:08:34  rainy
  Removed hooking code.

  Revision 1.1  2002/08/03 16:37:11  rainy
  Intial version.

*/

#ifndef __TOOLTIP_H__
#define __TOOLTIP_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <map>
#include <list>

class CToolTip
{
public:
	struct ToolTipData
	{
		std::string text;
		COLORREF color;
		int height;			// This will contain the height of the text
	};

	CToolTip();
	~CToolTip();
	
	void Initialize(HWND parent, HINSTANCE instance);
	void Finalize();

	void ReadSettings(const char* filename, const char* section);
	void WriteSettings();

	int CreateToolTip(const RECT& rect, HWND hwnd);
	bool DeleteToolTip(int ID);
	void DeleteAllToolTips();
	bool AddData(int ID, const ToolTipData& data);

	void SetDelay(UINT delay) { m_Delay = delay; };
	UINT GetDelay() { return m_Delay; };

	void SetSeparation(UINT seperation) { m_Separation = seperation; };
	UINT GetSeparation() { return m_Separation; };

	void SetSeparator(bool separator) { m_Separator = separator; };
	bool IsSeparator() { return m_Separator; };

	void SetMaxWidth(UINT maxWidth) { m_MaxWidth = maxWidth; };
	UINT GetMaxWidth() { return m_MaxWidth; };

	static CToolTip& Instance() { return c_ToolTip; };

	static LRESULT CALLBACK MouseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Tooltip
	void SetToolTipFontColor(COLORREF ToolTipFontColor ) { m_ToolTipFontColor=ToolTipFontColor; };
	COLORREF GetToolTipFontColor() { return m_ToolTipFontColor; };
	void SetToolTipBGColor(COLORREF ToolTipBGColor ) { m_ToolTipBGColor=ToolTipBGColor; };
	COLORREF GetToolTipBGColor() { return m_ToolTipBGColor; };
	void SetToolTipFont(const std::string& ToolTipFont ) { m_ToolTipFont=ToolTipFont; };
    const std::string& GetToolTipFont() { return m_ToolTipFont; };

	void ResetSettings();

protected:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouse(WPARAM wParam, LPARAM lParam);

private:
	struct ToolTip
	{
		RECT rect;
		HWND hwnd;
		std::list<ToolTipData> datas;
	};

	enum CORNER 
	{
		CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR
	};

	ToolTip* CToolTip::IsOnTip(POINT p, HWND hwnd);
	void ShowTip(const POINT& pos, ToolTip* tip);
	void HideTip();
	bool ResizeTipWindow(const POINT& pos);

	HWND m_Parent;
	UINT m_Message;
	HINSTANCE m_Instance;
	HFONT m_Font;
	std::map<int, ToolTip*> m_ToolTips;
	bool m_Hidden;
	bool m_ClickedAway;
	UINT m_Delay;
	UINT m_Separation;
	bool m_Separator;
	HHOOK m_Hook;
	HWND m_Window;
	ToolTip* m_CurrentTip;
	POINT m_Offset;
	RECT m_InsideRect;
	CORNER m_ArrowCorner;
	UINT m_MaxWidth;

	std::string m_Section;
	std::string m_SettingsFile;

	COLORREF m_ToolTipFontColor;
	COLORREF m_ToolTipBGColor;
	std::string m_ToolTipFont;

	static CToolTip c_ToolTip;
	static int c_CurrentID;
	static POINT c_MousePos;
};

#endif
