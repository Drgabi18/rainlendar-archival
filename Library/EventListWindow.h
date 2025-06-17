/*
  Copyright (C) 2004 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Library/EventListWindow.h,v 1.2 2005/09/08 16:09:12 rainy Exp $

  $Log: EventListWindow.h,v $
  Revision 1.2  2005/09/08 16:09:12  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.2  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.1  2004/11/06 13:38:58  rainy
  no message

*/

#ifndef __EVENTLISTWINDOW_H__
#define __EVENTLISTWINDOW_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "RainWindow.h"
#include "Background.h"
#include "RasterizerFont.h"

class CEventListWindow: public CRainWindow
{
public:
	CEventListWindow();
	~CEventListWindow();

	virtual bool Initialize(HWND parent, HINSTANCE instance);
	virtual void ReadSettings(const char* filename, const char* section);
	virtual void WriteSettings();

	virtual void Refresh();
	virtual void Redraw();
	virtual RAINWINDOW_TYPE GetType() { return RAINWINDOW_TYPE_EVENTLIST; };

	virtual int GetY();

	LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	LRESULT OnMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// EventList
    const std::string& GetEventListBitmapName() { return m_EventListBitmapName; };
	RECT GetEventListBitmapMargins() { return m_EventListBitmapMargins; };
	RECT GetEventListTextMargins() { return m_EventListTextMargins; };
	int GetEventListW() { return m_EventListW; };
    const std::string& GetEventListFont() { return m_EventListFont; };
	COLORREF GetEventListFontColor() { return m_EventListFontColor; };
	int GetEventListSeparation() { return m_EventListSeparation; };
	int GetEventListDaySeparation() { return m_EventListDaySeparation; };		// Mitul
	int GetEventListHeaderSeparation() { return m_EventListHeaderSeparation; };
	int GetEventListHeaderSeparation2() { return m_EventListHeaderSeparation2; };
    const std::string& GetEventListHeaderFont() { return m_EventListHeaderFont; };
	COLORREF GetEventListHeaderFontColor() { return m_EventListHeaderFontColor; };
    const std::string& GetEventListHeaderItemBitmapName() { return m_EventListHeaderItemBitmapName; };
	POINT GetEventListHeaderItemOffset() { return m_EventListHeaderItemOffset; };
	CRasterizer::ALIGN GetEventListHeaderItemAlign() { return m_EventListHeaderItemAlign; };
    const std::string& GetEventListHeaderFormat() { return m_EventListHeaderFormat; };
	// Mitul{
	int GetEventListItemMargin() { return m_EventListItemMargin; };
	bool GetEventListItemIconEnable() { return m_EventListItemIconEnable; };
	// Mitul}

	void SetEventListBitmapName(const std::string& EventListBitmapName ) { m_EventListBitmapName=EventListBitmapName; };
	void SetEventListBitmapMargins(RECT EventListBitmapMargins) { m_EventListBitmapMargins=EventListBitmapMargins; };
	void SetEventListTextMargins(RECT EventListTextMargins) { m_EventListTextMargins=EventListTextMargins; };
	void SetEventListW(int EventListW ) { m_EventListW=EventListW; };
	void SetEventListFont(const std::string& EventListFont ) { m_EventListFont=EventListFont; };
	void SetEventListFontColor(COLORREF EventListFontColor ) { m_EventListFontColor=EventListFontColor; };
	void SetEventListHeaderFont(const std::string& EventListFont ) { m_EventListHeaderFont=EventListFont; };
	void SetEventListHeaderFontColor(COLORREF EventListFontColor ) { m_EventListHeaderFontColor=EventListFontColor; };
	void SetEventListSeparation(int EventListSeparation) { m_EventListSeparation=EventListSeparation; };
	void SetEventListDaySeparation(int EventListSeparation) { m_EventListDaySeparation=EventListSeparation; };	// Mitul
	void SetEventListHeaderSeparation(int EventListSeparation) { m_EventListHeaderSeparation=EventListSeparation; };
	void SetEventListHeaderSeparation2(int EventListSeparation) { m_EventListHeaderSeparation2=EventListSeparation; };
	void SetEventListHeaderItemBitmapName(const std::string& EventListItemBitmapName) { m_EventListHeaderItemBitmapName=EventListItemBitmapName; };
	void SetEventListHeaderItemOffset(POINT EventListItemOffset) { m_EventListHeaderItemOffset=EventListItemOffset; };
	void SetEventListHeaderItemAlign(CRasterizer::ALIGN EventListItemAlign) { m_EventListHeaderItemAlign=EventListItemAlign; };
	void SetEventListHeaderFormat(const std::string& EventListHeaderFormat ) { m_EventListHeaderFormat=EventListHeaderFormat; };
	// Mitul{
	void SetEventListItemMargin(int EventListItemMargin) { m_EventListItemMargin=EventListItemMargin; };
	void SetEventListItemIconEnable(bool EventListItemIconEnable) { m_EventListItemIconEnable=EventListItemIconEnable; };
	// Mitul}

	void ResetSettings();

	GUID* HitTest(int x, int y, CFileTime* day);

protected:
	virtual SIZE CalcWindowSize();
	virtual void DrawWindow();
	
private:
	int CalcTextHeight();
	void AddToolTip(RECT rect, CEntryEvent* event);

	int m_SelectedEventList;
	int m_TotalHeight;
	std::vector<int> m_Heights;

	int m_HeaderHeight;
	int m_ItemHeight;

	struct EVENTLISTITEM 
	{
		std::string header;
		std::vector<std::string> items;
		std::vector<LPCTSTR> profiles;		// Mitul
		std::vector<GUID> guids;
	};
	std::vector<EVENTLISTITEM> m_EventListItems;

	CBackground m_EventListBackground;
	CRasterizerFont* m_ItemRasterizer;
	CRasterizerFont* m_HeaderItemRasterizer;
	CImage m_HeaderItemImage;

	std::string m_EventListBitmapName;
	RECT m_EventListBitmapMargins;
	RECT m_EventListTextMargins;
	int m_EventListW;
	COLORREF m_EventListFontColor;
	std::string m_EventListFont;
	COLORREF m_EventListHeaderFontColor;
	std::string m_EventListHeaderFont;
	int m_EventListSeparation;
	int m_EventListDaySeparation;		// Mitul
	int m_EventListHeaderSeparation;
	int m_EventListHeaderSeparation2;
	std::string m_EventListHeaderFormat;
	std::string m_EventListHeaderItemBitmapName;
	POINT m_EventListHeaderItemOffset;
	CRasterizer::ALIGN m_EventListHeaderItemAlign;
	// Mitul{
	int m_EventListItemMargin;
	bool m_EventListItemIconEnable;
	// Mitul}
};

#endif

