/*
  Copyright (C) 2003 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Plugin/MessageWindow.cpp,v 1.8 2004/12/05 18:25:05 rainy Exp $

  $Log: MessageWindow.cpp,v $
  Revision 1.8  2004/12/05 18:25:05  rainy
  AddEvent takes pointer.
  Implemented OnMouseMove.

  Revision 1.7  2004/11/06 13:38:59  rainy
  no message

  Revision 1.6  2004/06/10 15:30:25  rainy
  Fixed window size problem with empty events.

  Revision 1.5  2004/04/24 11:22:39  rainy
  Added show and hide.

  Revision 1.4  2004/01/28 18:05:41  rainy
  The text is centered.

  Revision 1.3  2004/01/25 10:01:24  rainy
  Fixed refreshing.

  Revision 1.2  2004/01/10 15:14:07  rainy
  Added ClearEvents()

  Revision 1.1  2003/12/20 21:26:38  Rainy
  Initial version

*/
#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "MessageWindow.h"
#include "CalendarWindow.h"
#include "Litestep.h"

/* 
** CMessageWindow
**
** Constructor
**
*/
CMessageWindow::CMessageWindow() : CRainWindow()
{
	m_WindowName = "MessageWindow";
	m_ClassName = "RainlendarMessageWindow";
	m_WndProc = CMessageWindow::WndProc;

	m_ItemRasterizer = NULL;
	m_StopSnoozing = 0;
	m_TextWidth = 0;
	m_TextHeight = 0;

	m_Visible = false;

	ResetSettings();
}

/* 
** ~CMessageWindow
**
** Destructor
**
*/
CMessageWindow::~CMessageWindow()
{
	if (m_ItemRasterizer) delete m_ItemRasterizer;
}

void CMessageWindow::ResetSettings()
{
	CRainWindow::ResetSettings();

	// Reset the values
	m_MessageBoxBitmapName = "";
	m_MessageBoxBitmapMargins.left = m_MessageBoxBitmapMargins.top = m_MessageBoxBitmapMargins.right = m_MessageBoxBitmapMargins.bottom = 0;
	m_MessageBoxTextMargins.left = m_MessageBoxTextMargins.top = m_MessageBoxTextMargins.right = m_MessageBoxTextMargins.bottom = 8;
	m_MessageBoxFontColor = 0;
	m_MessageBoxFont = "-17/0/0/0/800/0/0/0/0/3/2/1/34/Arial";
	m_MessageBoxSeparation = 5;
}

void CMessageWindow::ReadSettings(const char* iniFile, const char* section)
{
	CRainWindow::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	// MessageBox stuff
	if(GetPrivateProfileString( section, "MessageBoxBitmapName", m_MessageBoxBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_MessageBoxBitmapName=tmpSz;
	}
	if(GetPrivateProfileString( section, "MessageBoxBitmapMargins", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%i, %i, %i, %i", &m_MessageBoxBitmapMargins.left, &m_MessageBoxBitmapMargins.top, &m_MessageBoxBitmapMargins.right, &m_MessageBoxBitmapMargins.bottom);
	}
	if(GetPrivateProfileString( section, "MessageBoxTextMargins", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%i, %i, %i, %i", &m_MessageBoxTextMargins.left, &m_MessageBoxTextMargins.top, &m_MessageBoxTextMargins.right, &m_MessageBoxTextMargins.bottom);
	}
	if(GetPrivateProfileString( section, "MessageBoxFont", m_MessageBoxFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_MessageBoxFont=tmpSz;
	}
	if(GetPrivateProfileString( section, "MessageBoxFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_MessageBoxFontColor = CConfig::ParseColor(tmpSz);
	}
	m_MessageBoxSeparation=GetPrivateProfileInt( section, "MessageBoxSeparation", m_MessageBoxSeparation, iniFile);
}

void CMessageWindow::WriteSettings()
{
	char tmpSz[256];

	std::string INIPath = m_SettingsFile;

	// MessageBox stuff
	WritePrivateProfileString( m_Section.c_str(), "MessageBoxBitmapName", m_MessageBoxBitmapName.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%i, %i, %i, %i", m_MessageBoxBitmapMargins.left, m_MessageBoxBitmapMargins.top, m_MessageBoxBitmapMargins.right, m_MessageBoxBitmapMargins.bottom);
	WritePrivateProfileString( m_Section.c_str(), "MessageBoxBitmapMargins", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i, %i, %i, %i", m_MessageBoxTextMargins.left, m_MessageBoxTextMargins.top, m_MessageBoxTextMargins.right, m_MessageBoxTextMargins.bottom);
	WritePrivateProfileString( m_Section.c_str(), "MessageBoxTextMargins", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "MessageBoxFont", m_MessageBoxFont.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%X", m_MessageBoxFontColor);
	WritePrivateProfileString( m_Section.c_str(), "MessageBoxFontColor", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_MessageBoxSeparation);
	WritePrivateProfileString( m_Section.c_str(), "MessageBoxSeparation", tmpSz, INIPath.c_str() );
}

bool CMessageWindow::Initialize(HWND parent, HINSTANCE instance)
{
	// Must set the correct position before the window is created so that it can be put in correct place
	m_X = 0;
	m_Y = 0;

	return CRainWindow::Initialize(parent, instance);
}

void CMessageWindow::ShowWindow(bool activate)
{
	m_Visible = true;
	m_StopSnoozing = 0;
	Redraw();
	CRainWindow::ShowWindow(activate);
}

void CMessageWindow::HideWindow()
{
	m_Visible = false;
	CRainWindow::HideWindow();
}

void CMessageWindow::ClearEvents()
{
	// Clear the old events
	m_EventGuidList.clear();
	m_EventTimeList.clear();

	HideWindow();
}

void CMessageWindow::AddEvent(GUID* guid, CFileTime* start)
{
	if (guid)
	{
		// Check if the event already is in the list
		for (int i = 0; i < m_EventGuidList.size(); i++)
		{
			if (IsEqualGUID(m_EventGuidList[i], *guid))
			{
				DebugLog("Event %s is already in the message box.", CEntryItem::GetGUIDAsString(guid));
				return;
			}
		}

		m_EventGuidList.push_back(*guid);
		if (start)
		{
			m_EventTimeList.push_back(*start);
		}
		else
		{
			m_EventTimeList.push_back(CFileTime());
		}
	}
}

void CMessageWindow::GetTextFromEvent(CEntryEvent* event, CFileTime alarmTime, std::string& text)
{
	CFileTime today;
	today.SetToLocalTime();

	event->GetBriefMessage(text, 0, true, true);

	if (alarmTime.IsValid() && !event->IsAllDayEvent())
	{
		int diff = alarmTime.GetAsSeconds() - today.GetAsSeconds();
		diff /= 60;

		char buffer[MAX_LINE_LENGTH];
		if (diff >= 0)
		{
			sprintf(buffer, CCalendarWindow::c_Language.GetString("General", 21), diff);	// Due in ...
		}
		else
		{
			sprintf(buffer, CCalendarWindow::c_Language.GetString("General", 25), -diff);	// ...overdue
		}
		text += buffer;
	}
}


SIZE CMessageWindow::CalcTextSize()
{
	SIZE textSize;
	textSize.cx = 0;
	textSize.cy = 0;
	
	m_Heights.clear();

	RECT rect = {0, 0, 0, 0};
	RECT margins = GetMessageBoxTextMargins();
	int width = CConfig::Instance().GetMessageBoxMaxWidth() - margins.left - margins.right;
	width = max(0, width);

	if (width == 0) 
	{
		GetClientRect(GetDesktopWindow(), &rect);
		width = rect.right;
	}

	// Calculate the size of the texts
	for (UINT i = 0; i < m_EventGuidList.size(); i++)
	{
		CEntryEvent* event = GetRainlendar()->GetCalendarWindow().GetEventManager().GetEvent(&m_EventGuidList[i]);
		if (event)
		{
			rect.right = width;
			
			std::string text;
			GetTextFromEvent(event, m_EventTimeList[i], text);

			m_ItemRasterizer->GetTextSize(text.c_str(), &rect, true);

			int height = rect.bottom;
			m_Heights.push_back(height);

			textSize.cy += height + GetMessageBoxSeparation();
			textSize.cx = max(textSize.cx, rect.right);
		}
	}
	textSize.cy -= GetMessageBoxSeparation();

	return textSize;
}

void CMessageWindow::Refresh()
{
	CRainWindow::Refresh();

	if (m_ItemRasterizer) delete m_ItemRasterizer;

	m_ItemRasterizer=new CRasterizerFont;
	if(m_ItemRasterizer==NULL) THROW(ERR_OUTOFMEM);

	m_ItemRasterizer->SetFont(GetMessageBoxFont());
	m_ItemRasterizer->SetAlign(CRasterizer::ALIGN_LEFT);
	m_ItemRasterizer->SetColor(GetMessageBoxFontColor());

	m_MessageBoxBackground.Initialize(GetMessageBoxBitmapName().c_str());

	Redraw();
}

void CMessageWindow::Redraw()
{
	if (m_ItemRasterizer)
	{
		SIZE textSize = CalcTextSize();

		m_TextWidth = textSize.cx;
		m_TextHeight = textSize.cy;
		
		RECT workArea;
		GetClientRect(GetDesktopWindow(), &workArea);
		
		RECT margins = GetMessageBoxTextMargins();

		m_Width = margins.left + margins.right + textSize.cx;
		m_Height = margins.top + margins.bottom + textSize.cy;
		
		if (m_MessageBoxBackground.IsValid())
		{
			// Check if the background image is bigger than the calculated window size
			m_Width = max(m_Width, m_MessageBoxBackground.GetImageWidth());
			m_Height = max(m_Height, m_MessageBoxBackground.GetImageHeight());
		}

		double x = CConfig::Instance().GetMessageBoxX() / 100.0;
		double y = CConfig::Instance().GetMessageBoxY() / 100.0;

		m_X = (workArea.right - workArea.left) * x - m_Width / 2;
		m_Y = (workArea.bottom - workArea.top) * y - m_Height / 2;

		BackgroundCreateStruct bcs;
		bcs.pos.x = m_X;
		bcs.pos.y = m_Y;
		bcs.size.cx = m_Width;
		bcs.size.cy = m_Height;
		
		if (GetMessageBoxBitmapName().empty())
		{
			bcs.mode = MODE_SOLID;
		}
		else
		{
			bcs.mode = MODE_RECT;
		}
		bcs.solidBevel = TRUE;
		bcs.solidColor = GetSysColor(COLOR_BTNFACE);
		bcs.resizeRect = GetMessageBoxBitmapMargins();
		
		m_MessageBoxBackground.Create(bcs);

		// RedrawBegin is called in the end, since the window position and size need to be calculated before
		CRainWindow::RedrawBegin();
		CRainWindow::RedrawEnd();

		SetWindowZPos(CConfig::WINDOWPOS_ONTOP);		// Always on top
	}
}

SIZE CMessageWindow::CalcWindowSize()
{
	SIZE size;

	// Width and Height are already calculated in the redraw
	size.cx = m_Width;
	size.cy = m_Height;

	return size;
}

void CMessageWindow::DrawWindow()
{
	int X, Y, W, H;

	RECT rc = GetMessageBoxTextMargins();

	// Draw the background
	if (m_MessageBoxBackground.IsValid())
	{
		m_MessageBoxBackground.Paint(m_DoubleBuffer);
	}

	// Center the text
	X = (m_MessageBoxBackground.GetWidth() - rc.left - rc.right - m_TextWidth) / 2;
	X += rc.left;
	Y = (m_MessageBoxBackground.GetHeight() - rc.top - rc.bottom - m_TextHeight) / 2;
	Y += rc.top;
	W = m_TextWidth;

	// Calculate the size of the texts
	for (UINT i = 0; i < m_EventGuidList.size(); i++)
	{
		CEntryEvent* event = GetRainlendar()->GetCalendarWindow().GetEventManager().GetEvent(&m_EventGuidList[i]);
		if (event)
		{
			H = m_Heights[i];

			// Draw the text
			std::string text;
			GetTextFromEvent(event, m_EventTimeList[i], text);

			m_ItemRasterizer->WriteText(m_DoubleBuffer, X, Y, W, H, text.c_str(), true, false);
			Y += m_Heights[i] + GetMessageBoxSeparation();
		}
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

void CMessageWindow::UpdateTransparency(UINT transparency)
{
	CRainWindow::UpdateTransparency(255);	// Always opaque
}

LRESULT CMessageWindow::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	ClearEvents();
	m_StopSnoozing = 0;
	return TRUE;
}

LRESULT CMessageWindow::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	// Hide the window
	HideWindow();

	// Go to snooze mode
	m_StopSnoozing = GetTickCount() + CConfig::Instance().GetSnoozeTime() * 1000;
	
	return TRUE;
}

LRESULT CMessageWindow::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	// Fake the config so that the mouseover/click through are not applied
	bool hide = CConfig::Instance().GetMouseHide();
	bool click = CConfig::Instance().GetClickThrough();

	CConfig::Instance().SetMouseHide(false);
	CConfig::Instance().SetClickThrough(false);

	CRainWindow::OnMouseMove(wParam, lParam);

	CConfig::Instance().SetMouseHide(hide);
	CConfig::Instance().SetClickThrough(click);

	return 0;
}

LRESULT CMessageWindow::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if (m_Visible)
	{
		// Update window on even minutes
		CFileTime now;
		now.SetToLocalTime();

		if (m_LastUpdate.GetAsSeconds() == 0 || now.GetAsSeconds() - m_LastUpdate.GetAsSeconds() >= 60)
		{
			if (m_LastUpdate.GetAsSeconds() != 0)
			{
				Redraw();
			}
			m_LastUpdate = now;
		}
	}

	// Show the window
	if (m_StopSnoozing != 0 && m_StopSnoozing < GetTickCount())
	{
		ShowWindow(false);
	}
	
	return TRUE;
}


/* 
** WndProc
**
** The window procedure
**
*/
LRESULT CALLBACK CMessageWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CMessageWindow* Window = NULL;

	if (Window) Window->m_Message = uMsg;

	if (uMsg == WM_CREATE) 
	{
		// Fetch this window-object from the CreateStruct
		Window=(CMessageWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
	}

	if (Window)
	{
		switch(uMsg) 
		{
			case WM_PAINT: 
				return Window->OnPaint(wParam, lParam);

			case WM_ERASEBKGND: 
				return Window->OnEraseBkgnd(wParam, lParam);

			case WM_SETTINGCHANGE: 
				return Window->OnSettingsChange(wParam, lParam);

			case WM_MOUSEMOVE: 
			case WM_NCMOUSEMOVE: 
				return Window->OnMouseMove(wParam, lParam);

			case WM_LBUTTONDOWN: 
			case WM_NCLBUTTONDOWN: 
				return Window->OnLButtonDown(wParam, lParam);

			case WM_LBUTTONUP: 
			case WM_NCLBUTTONUP: 
				return Window->OnLButtonUp(wParam, lParam);

			case WM_RBUTTONDOWN: 
				return Window->OnRButtonDown(wParam, lParam);

			case WM_TIMER: 
				return Window->OnTimer(wParam, lParam);
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
