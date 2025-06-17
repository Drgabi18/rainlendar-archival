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
  $Header: /home/cvsroot/Rainlendar/Library/EventListWindow.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: EventListWindow.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.4  2005/03/25 13:58:43  rainy
  no message

  Revision 1.3  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.2  2004/12/05 18:32:14  rainy
  Small fixes.

  Revision 1.1  2004/11/06 13:38:58  rainy
  no message

*/
#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "EventListWindow.h"
#include "CalendarWindow.h"
#include "Litestep.h"
#include "Tooltip.h"
#include "EditEvent.h"
#include "RasterizerBitmap.h"
#include <time.h>
#include <algorithm>

/* 
** CEventListWindow
**
** Constructor
**
*/
CEventListWindow::CEventListWindow() : CRainWindow()
{
	m_WindowName = "EventListWindow";
	m_ClassName = "RainlendarEventListWindow";
	m_WndProc = CEventListWindow::WndProc;

	m_ItemRasterizer = NULL;
	m_HeaderItemRasterizer = NULL;
	m_TotalHeight = 0;
	m_SelectedEventList = -1;

	m_ItemHeight = 0;
	m_HeaderHeight = 0;

	ResetSettings();
}

/* 
** ~CEventListWindow
**
** Destructor
**
*/
CEventListWindow::~CEventListWindow()
{
	if (m_ItemRasterizer) delete m_ItemRasterizer;
	if (m_HeaderItemRasterizer) delete m_HeaderItemRasterizer;
}

void CEventListWindow::ResetSettings()
{
	CRainWindow::ResetSettings();

	// Reset the values
	m_EventListBitmapName = "";
	m_EventListBitmapMargins.left = m_EventListBitmapMargins.top = m_EventListBitmapMargins.right = m_EventListBitmapMargins.bottom = 0;
	m_EventListTextMargins.left = m_EventListTextMargins.top = m_EventListTextMargins.right = m_EventListTextMargins.bottom = 8;
	m_EventListW = 200;
	m_EventListFontColor = 0;
	m_EventListFont = "-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_EventListHeaderFontColor = 0;
	m_EventListHeaderFont = "-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";;
	m_EventListSeparation = 0;
	m_EventListHeaderSeparation = 5;
	m_EventListHeaderFormat = "%A";
	m_EventListHeaderItemBitmapName = "";
	m_EventListHeaderItemOffset.x = m_EventListHeaderItemOffset.y = 0;
	m_EventListHeaderItemAlign = CRasterizer::ALIGN_LEFT;
}

void CEventListWindow::ReadSettings(const char* iniFile, const char* section)
{
	CRainWindow::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	// EventList stuff
	if(GetPrivateProfileString( section, "EventListBitmapName", m_EventListBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventListBitmapName=tmpSz;
	}
	if(GetPrivateProfileString( section, "EventListBitmapMargins", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%i, %i, %i, %i", &m_EventListBitmapMargins.left, &m_EventListBitmapMargins.top, &m_EventListBitmapMargins.right, &m_EventListBitmapMargins.bottom);
	}
	if(GetPrivateProfileString( section, "EventListTextMargins", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%i, %i, %i, %i", &m_EventListTextMargins.left, &m_EventListTextMargins.top, &m_EventListTextMargins.right, &m_EventListTextMargins.bottom);
	}
	m_EventListW=GetPrivateProfileInt( section, "EventListW", m_EventListW, iniFile);
	if(GetPrivateProfileString( section, "EventListFont", m_EventListFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventListFont=tmpSz;
	}
	if(GetPrivateProfileString( section, "EventListFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventListFontColor = CConfig::ParseColor(tmpSz);
	}
	if(GetPrivateProfileString( section, "EventListHeaderFont", m_EventListHeaderFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventListHeaderFont=tmpSz;
	}
	if(GetPrivateProfileString( section, "EventListHeaderFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventListHeaderFontColor = CConfig::ParseColor(tmpSz);
	}
	m_EventListHeaderItemAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( section, "EventListHeaderItemAlign", m_EventListHeaderItemAlign, iniFile);
	if(GetPrivateProfileString( section, "EventListHeaderItemOffset", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%i, %i", &m_EventListHeaderItemOffset.x, &m_EventListHeaderItemOffset.y);
	}
	m_EventListSeparation=GetPrivateProfileInt( section, "EventListSeparation", m_EventListSeparation, iniFile);
	m_EventListHeaderSeparation=GetPrivateProfileInt( section, "EventListHeaderSeparation", m_EventListHeaderSeparation, iniFile);
	if(GetPrivateProfileString( section, "EventListHeaderItemBitmapName", m_EventListHeaderItemBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventListHeaderItemBitmapName=tmpSz;
	}
	if(GetPrivateProfileString( section, "EventListHeaderFormat", m_EventListHeaderFormat.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventListHeaderFormat=tmpSz;
	}
}

void CEventListWindow::WriteSettings()
{
	char tmpSz[256];

	std::string INIPath = m_SettingsFile;

	// EventList stuff
	WritePrivateProfileString( m_Section.c_str(), "EventListBitmapName", m_EventListBitmapName.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%i, %i, %i, %i", m_EventListBitmapMargins.left, m_EventListBitmapMargins.top, m_EventListBitmapMargins.right, m_EventListBitmapMargins.bottom);
	WritePrivateProfileString( m_Section.c_str(), "EventListBitmapMargins", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i, %i, %i, %i", m_EventListTextMargins.left, m_EventListTextMargins.top, m_EventListTextMargins.right, m_EventListTextMargins.bottom);
	WritePrivateProfileString( m_Section.c_str(), "EventListTextMargins", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_EventListW);
	WritePrivateProfileString( m_Section.c_str(), "EventListW", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "EventListFont", m_EventListFont.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%X", m_EventListFontColor);
	WritePrivateProfileString( m_Section.c_str(), "EventListFontColor", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_EventListSeparation);
	WritePrivateProfileString( m_Section.c_str(), "EventListSeparation", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_EventListHeaderSeparation);
	WritePrivateProfileString( m_Section.c_str(), "EventListHeaderSeparation", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "EventListHeaderFont", m_EventListHeaderFont.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%X", m_EventListHeaderFontColor);
	WritePrivateProfileString( m_Section.c_str(), "EventListHeaderFontColor", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "EventListHeaderItemBitmapName", m_EventListHeaderItemBitmapName.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%i, %i", m_EventListHeaderItemOffset.x, m_EventListHeaderItemOffset.y);
	WritePrivateProfileString( m_Section.c_str(), "EventListHeaderItemOffset", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_EventListHeaderItemAlign);
	WritePrivateProfileString( m_Section.c_str(), "EventListHeaderItemAlign", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "EventListHeaderFormat", m_EventListHeaderFormat.c_str(), INIPath.c_str() );
}

bool CEventListWindow::Initialize(HWND parent, HINSTANCE instance)
{
	// Must set the correct position before the window is created so that it can be put in correct place
	m_X = CConfig::Instance().GetEventListX();
	m_Y = CConfig::Instance().GetEventListY();

	return CRainWindow::Initialize(parent, instance);
}

int CEventListWindow::GetY()
{
	if (CConfig::Instance().GetGrowUpwards() & (1 << (int)RAINWINDOW_TYPE_EVENTLIST))
	{
		return m_Y - m_Height;
	}
	return m_Y;
}

void CEventListWindow::Refresh()
{
	CRainWindow::Refresh();

	if (m_ItemRasterizer) delete m_ItemRasterizer;

	m_ItemRasterizer=new CRasterizerFont;
	if(m_ItemRasterizer==NULL) THROW(ERR_OUTOFMEM);

	m_ItemRasterizer->SetFont(GetEventListFont());
	m_ItemRasterizer->SetAlign(CRasterizer::ALIGN_LEFT);
	m_ItemRasterizer->SetColor(GetEventListFontColor());

	if (m_HeaderItemRasterizer) delete m_HeaderItemRasterizer;

	m_HeaderItemRasterizer=new CRasterizerFont;
	if(m_HeaderItemRasterizer==NULL) THROW(ERR_OUTOFMEM);

	m_HeaderItemRasterizer->SetFont(GetEventListHeaderFont());
	m_HeaderItemRasterizer->SetAlign(CRasterizer::ALIGN_LEFT);
	m_HeaderItemRasterizer->SetColor(GetEventListHeaderFontColor());

	m_X = CConfig::Instance().GetEventListX();
	m_Y = CConfig::Instance().GetEventListY();
		
	m_HeaderItemImage.Clear();
	if (!GetEventListHeaderItemBitmapName().empty())
	{
		CRasterizerBitmap::LoadImage(GetEventListHeaderItemBitmapName(), &m_HeaderItemImage);
	}

	m_EventListBackground.Initialize(GetEventListBitmapName().c_str());

	Redraw();
}

void CEventListWindow::Redraw()
{
	if (m_ItemRasterizer)
	{
		m_TotalHeight = CalcTextHeight();
		SIZE size = CalcWindowSize();

		CRainWindow::RedrawBegin();
		
		BackgroundCreateStruct bcs;
		bcs.pos.x = m_X;
		bcs.pos.y = m_Y;
		bcs.size.cx = size.cx;
		bcs.size.cy = size.cy;
		
		if (GetEventListBitmapName().empty())
		{
			bcs.mode = MODE_SOLID;
		}
		else
		{
			bcs.mode = MODE_RECT;
		}
		bcs.solidBevel = TRUE;
		bcs.solidColor= GetSysColor(COLOR_BTNFACE);
		bcs.resizeRect= GetEventListBitmapMargins();
		
		m_EventListBackground.Create(bcs);

	}

	CRainWindow::RedrawEnd();

	SetWindowZPos(CConfig::Instance().GetWindowPos());		// Uses the global z-pos
}

bool CompareEvents(CEntryEvent* arg1, CEntryEvent* arg2)
{
	int a = arg1->GetStartTime().GetOnlyTime();
	int b = arg2->GetStartTime().GetOnlyTime();

	// Check only the time, not date
	return a < b;
}

int CEventListWindow::CalcTextHeight()
{
	int headerHeight = 0, itemHeight = 0;
	int totalHeight = 0;
	m_Heights.clear();

	// Calc text heights
	RECT rect = {0, 0, 0, 0};
	RECT margins = GetEventListTextMargins();
	int width = GetEventListW() - margins.left - margins.right;
	width = max(0, width);

	CEventManager& manager = GetRainlendar()->GetCalendarWindow().GetEventManager();

	CFileTime now = CCalendarWindow::c_TodaysDate;

	m_EventListItems.clear();

	rect.right = 0;
	m_ItemRasterizer->GetTextSize("X", &rect, false);
	m_ItemHeight = rect.bottom;
	
	rect.right = 0;
	m_HeaderItemRasterizer->GetTextSize("X", &rect, false);
	m_HeaderHeight = rect.bottom;

	for (UINT i = 0; i < CConfig::Instance().GetEventListDays(); i++)
	{
		SYSTEMTIME sysTime = now.GetSystemTime();

		std::vector<CEntryEvent*> events = manager.GetEvents(sysTime.wDay, sysTime.wMonth, sysTime.wYear, EVENT_FLAG_NO_DELETED | EVENT_FLAG_NO_EVENTLIST_HIDDEN);
		std::sort(events.begin(), events.end(), CompareEvents);

		if (!events.empty())
		{
			char buffer[MAX_LINE_LENGTH];
			EVENTLISTITEM item;

			int oldTotalHeight = totalHeight;

			time_t t = now.GetTime_t();
			struct tm* time = gmtime(&t);

			if (CConfig::Instance().GetSubstituteDays() && i <= 1)
			{
				item.header = CCalendarWindow::c_Language.GetString("General", 22 + i);
			}
			else
			{
				strftime(buffer, MAX_LINE_LENGTH, GetEventListHeaderFormat().c_str(), time);
				item.header = buffer;
			}

			totalHeight += m_HeaderHeight + GetEventListHeaderSeparation() * 2;

			for (int i = 0; i < (int)events.size(); i++)
			{
				std::string message;
				events[i]->GetBriefMessage(message, 0, true, false);

				if (!message.empty())
				{
					std::string text = message;
					m_ItemRasterizer->GetTextSize(text.c_str(), &rect, false);

					RECT rc = GetEventListTextMargins();
					int W = GetEventListW() - rc.left - rc.right;

					if (rect.right > W)
					{
						int len = 1;
						do 
						{
							text = message.substr(0, len);
							text += "...";

							m_ItemRasterizer->GetTextSize(text.c_str(), &rect, false);
							
							len++;

						} while(rect.right < W);

						text = message.substr(0, len - 1);
						text += "...";
					}					

					item.items.push_back(text);
					
					const Profile* profile = CConfig::Instance().GetProfile(events[i]->GetProfile());
					if (profile)
					{
						item.colors.push_back(profile->eventColor);
					}
					else
					{
						item.colors.push_back(GetEventListFontColor());
					}

					item.guids.push_back(*(events[i]->GetGUID()));

					// Tooltip
					if (CConfig::Instance().GetEventToolTips())
					{
						RECT rect;
						rect.left = rc.left;
						rect.right = W - rc.right;
						rect.top = totalHeight + rc.top;
						rect.bottom = rect.top + m_ItemHeight;
						AddToolTip(rect, events[i]);
					}
					
					totalHeight += m_ItemHeight + GetEventListSeparation();
				}
			}
			totalHeight -= GetEventListSeparation();

			if (!item.items.empty())
			{
				m_EventListItems.push_back(item);
			}
			else
			{
				// No events -> don't increase the height
				totalHeight = oldTotalHeight;
			}
		}

		now.Add(24 * 60 * 60);	// Next day
	}

	RECT rc = GetEventListTextMargins();

	return totalHeight + rc.top + rc.bottom;
}

SIZE CEventListWindow::CalcWindowSize()
{
	SIZE size;

	size.cx = GetEventListW();
	size.cy = m_TotalHeight;

	if (m_EventListBackground.IsValid())
	{
		// Check if the background image is bigger than the calculated window size
		size.cx = max(size.cx, m_EventListBackground.GetImageWidth());
		size.cy = max(size.cy, m_EventListBackground.GetImageHeight());
	}

	return size;
}

void CEventListWindow::AddToolTip(RECT rect, CEntryEvent* event)
{
	int ID = CToolTip::Instance().CreateToolTip(rect, m_Window);

	const Profile* profile = CConfig::Instance().GetProfile(event->GetProfile());

	CToolTip::ToolTipData ttd;

	if (profile)
	{
		ttd.color = profile->toolTipColor;
	}
	else
	{
		ttd.color = CToolTip::Instance().GetToolTipFontColor();
	}

	 event->GetBriefMessage(ttd.text, 0, true, true);

	CToolTip::Instance().AddData(ID, ttd);
}

void CEventListWindow::DrawWindow()
{
	int X, Y, W, H;

	RECT rc = GetEventListTextMargins();

	SIZE size = CalcWindowSize();

	X = rc.left;
	Y = rc.top;
	W = size.cx - rc.left - rc.right;

	// Draw the background
	if (m_EventListBackground.IsValid())
	{
		m_EventListBackground.Paint(m_DoubleBuffer);
	}

	std::vector<EVENTLISTITEM>::iterator iter = m_EventListItems.begin();

	for ( ; iter != m_EventListItems.end(); iter++)
	{
		// Draw the item image
		if (m_HeaderItemImage.GetBitmap())
		{
			int x = X;
			int y = Y;

			// Align the bitmap correctly
			switch (GetEventListHeaderItemAlign() & 0x0F)
			{
			case CRasterizer::ALIGN_LEFT:
				x = X;
				break;

			case CRasterizer::ALIGN_HCENTER:
				x = X + (W - m_HeaderItemImage.GetWidth()) / 2;
				break;

			case CRasterizer::ALIGN_RIGHT:
				x = X + W - m_HeaderItemImage.GetWidth();
				break;
			};

			switch (GetEventListHeaderItemAlign() & 0x0F0)
			{
			case CRasterizer::ALIGN_TOP:
				y = Y;
				break;

			case CRasterizer::ALIGN_VCENTER:
				y = Y + (m_HeaderHeight - m_HeaderItemImage.GetHeight()) / 2;
				break;

			case CRasterizer::ALIGN_BOTTOM:
				y = Y + (m_HeaderHeight - m_HeaderItemImage.GetHeight());
				break;
			};

			x += GetEventListHeaderItemOffset().x;
			y += GetEventListHeaderItemOffset().y;

			m_DoubleBuffer.Blit(m_HeaderItemImage, x, y, 0, 0, m_HeaderItemImage.GetWidth(), m_HeaderItemImage.GetHeight());
		}

		// Draw the header
		m_HeaderItemRasterizer->SetAlign(GetEventListHeaderItemAlign());
		H = m_HeaderHeight + GetEventListHeaderSeparation() * 2;
		m_HeaderItemRasterizer->WriteText(m_DoubleBuffer, X, Y, W, H, (*iter).header.c_str(), false, false);
		Y += m_HeaderHeight + GetEventListHeaderSeparation() * 2;

		// Draw the items
		
		std::vector<std::string>::iterator itemIter = (*iter).items.begin();

		for (int i = 0 ; i < (*iter).items.size(); i++)
		{
			m_ItemRasterizer->SetColor((*iter).colors[i]);
			m_ItemRasterizer->WriteText(m_DoubleBuffer, X, Y, 0, 0, (*iter).items[i].c_str(), false, false);
			Y += m_ItemHeight + GetEventListSeparation();
		}
		Y -= GetEventListSeparation();
	}

	POINT offset = {0, 0};
	for (int k = 0; k < m_StaticSkinItems.size(); k++)
	{
		if (m_StaticSkinItems[k]->IsEnabled())
		{
			m_StaticSkinItems[k]->Paint(m_DoubleBuffer, offset);
		}
	}
}

/* 
** HitTest
**
** Returns the item's id under the given coordinates
**
*/
GUID* CEventListWindow::HitTest(int x, int y, CFileTime* day)
{
	if (day) 
	{
		*day = CCalendarWindow::c_TodaysDate;
	}

	RECT rc = GetEventListTextMargins();

	if (x < rc.left || x > GetEventListW() - rc.right) return NULL;
	if (y < rc.top) return NULL;

	int curH = rc.top;
	for (int i = 0; i < (int)m_EventListItems.size(); i++)
	{
		// Check header
		if (day && curH < y && curH + m_HeaderHeight + GetEventListHeaderSeparation() * 2 > y)
		{
			// Get the time from the first item
			CEventManager& manager = GetRainlendar()->GetCalendarWindow().GetEventManager();
			CEntryEvent* event = manager.GetEvent(&m_EventListItems[i].guids[0]);
			if (event) 
			{
				*day = event->GetStartTime();
				day->NormalizeDay();
			}
			return NULL;		// Clicked on header
		}

		// Check items
		curH += m_HeaderHeight + GetEventListHeaderSeparation() * 2;
		for (int j = 0; j < (int)m_EventListItems[i].guids.size(); j++)
		{
			if (curH < y && curH + m_ItemHeight > y)
			{
				return &(m_EventListItems[i].guids[j]);		// Clicked on item
			}

			curH += m_ItemHeight + GetEventListSeparation();
		}
		curH -= GetEventListSeparation();
	}

	return NULL; // Clicked somewhere else
}

LRESULT CEventListWindow::OnLButtonDblClk(WPARAM wParam, LPARAM lParam) 
{
	int x = (SHORT)LOWORD(lParam);
	int y = (SHORT)HIWORD(lParam);

	if(m_Message == WM_NCLBUTTONDBLCLK)
	{
		// Transform the point to client rect
		RECT rect;
		GetWindowRect(m_Window, &rect);
		x = x - rect.left;
		y = y - rect.top;
	}

	CFileTime day;
	GUID* guid = HitTest(x, y, &day);

	switch(CConfig::Instance().GetEventListAction()) 
	{
	case 1:		// Create
		OpenEditEventDialog(m_Window, CRainlendar::GetInstance(), NULL, &CCalendarWindow::c_TodaysDate);
		break;

	case 2:		// Delete
		if (guid != NULL)
		{
			CEventManager& manager = GetRainlendar()->GetCalendarWindow().GetEventManager();
			CEntryEvent* event = manager.GetEvent(guid);

			if (event && !event->IsReadOnly())
			{
				// "Are you sure you want to delete the event(s)?"
				if (IDYES == MessageBox(m_Window, CCalendarWindow::c_Language.GetString("Message", 0), "Rainlendar", MB_YESNO | MB_ICONQUESTION))
				{
					manager.DeleteItem(guid);
					GetRainlendar()->GetCalendarWindow().RedrawAll();
				}
			}
		}
		break;

	default:
		if (guid != NULL)
		{
			CEventManager& manager = GetRainlendar()->GetCalendarWindow().GetEventManager();
			CEntryEvent* event = manager.GetEvent(guid);

			if (event == NULL || event->IsReadOnly())
			{
				guid = NULL;
			}
		}
		OpenEditEventDialog(m_Window, CRainlendar::GetInstance(), guid, &day);
	}

	return 0;
}

LRESULT CEventListWindow::OnMove(WPARAM wParam, LPARAM lParam) 
{
	short int x = LOWORD(lParam);
	short int y = HIWORD(lParam);

	m_X = x;
	m_Y = y;

	if (CConfig::Instance().GetGrowUpwards() & (1 << (int)GetType()))
	{
		m_Y += m_Height;
	}

	if(CConfig::Instance().GetMovable())
	{
		if (CConfig::Instance().GetNegativeCoords()) 
		{
			if (m_X < 0) m_X = 0;
			if (m_Y < 0) m_Y = 0;
		}

		CConfig::Instance().SetEventListX(m_X);
		CConfig::Instance().SetEventListY(m_Y);
		CConfig::Instance().WriteConfig(CConfig::WRITE_EVENTLISTPOS);	// Store the new position to the ini file
	}

	if (GetRainlendar()->GetCalendarWindow().GetBackgroundMode() == MODE_COPY || m_EventListBackground.HasAlpha())
	{
		if (!CConfig::Instance().GetNativeTransparency())
		{
			m_EventListBackground.UpdateWallpaper(x, y);
			DrawWindow();
			InvalidateRect(m_Window, NULL, false);
		}
	}

	CRainWindow::OnMove(wParam, lParam);

	return 0;
}

LRESULT CEventListWindow::OnTimer(WPARAM wParam, LPARAM lParam) 
{
	if (CConfig::Instance().GetEventListEnable())
	{
		return CRainWindow::OnTimer(wParam, lParam);
	}
	return 0;
}

/* 
** WndProc
**
** The window procedure
**
*/
LRESULT CALLBACK CEventListWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CEventListWindow* Window = NULL;

	if (Window) Window->m_Message = uMsg;

	if (uMsg == WM_CREATE) 
	{
		// Fetch this window-object from the CreateStruct
		Window=(CEventListWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
	}

	// Run the tooltip's mouse proc
	CToolTip::MouseProc(hWnd, uMsg, wParam, lParam);

	if (Window)
	{
		switch(uMsg) 
		{
			case WM_CONTEXTMENU:
			case WM_NCRBUTTONUP:
				return GetRainlendar()->GetCalendarWindow().OnContextMenu((WPARAM)hWnd, lParam);

			case WM_LBUTTONDBLCLK:
			case WM_NCLBUTTONDBLCLK:
				return Window->OnLButtonDblClk(wParam, lParam);

			case WM_MOVE: 
				return Window->OnMove(wParam, lParam);

			case WM_PAINT: 
				return Window->OnPaint(wParam, lParam);

			case WM_TIMER: 
				return Window->OnTimer(wParam, lParam);

			case WM_WINDOWPOSCHANGING: 
				return Window->OnWindowPosChanging(wParam, lParam);

			case WM_SETTINGCHANGE: 
				return Window->OnSettingsChange(wParam, lParam);

			case WM_ERASEBKGND: 
				return Window->OnEraseBkgnd(wParam, lParam);

			case WM_MOUSEMOVE: 
			case WM_NCMOUSEMOVE: 
				return Window->OnMouseMove(wParam, lParam);

			case WM_LBUTTONDOWN: 
			case WM_NCLBUTTONDOWN: 
				return Window->OnLButtonDown(wParam, lParam);

			case WM_LBUTTONUP: 
			case WM_NCLBUTTONUP: 
				return Window->OnLButtonUp(wParam, lParam);

			case WM_DISPLAYCHANGE: 
				return Window->OnDisplayChange(wParam, lParam);

			case WM_NCHITTEST: 
				return Window->OnNcHitTest(wParam, lParam);

			case WM_DROPFILES: 
				return Window->OnDropFiles(wParam, lParam);
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


