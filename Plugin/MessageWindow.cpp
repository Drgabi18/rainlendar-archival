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
  $Header: /home/cvsroot/Rainlendar/Plugin/MessageWindow.cpp,v 1.5 2004/04/24 11:22:39 rainy Exp $

  $Log: MessageWindow.cpp,v $
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
}

/* 
** ~CMessageWindow
**
** Destructor
**
*/
CMessageWindow::~CMessageWindow()
{
	CRasterizerFont::ClearBuffers();

	for (int i = 0; i < m_EventList.size(); i++)
	{
		delete m_EventList[i];
	}
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
	for (int i = 0; i < m_EventList.size(); i++)
	{
		delete m_EventList[i];
	}
	m_EventList.clear();

	HideWindow();
}

void CMessageWindow::ShowEvents(const std::vector<CEventMessage*>& events)
{
	int i;

	if (events.empty()) 
	{
		return;
	}

	// Copy new events (these are added to the old events if there are some)
	for (i = 0; i < events.size(); i++)
	{
		CEventMessage* event = new CEventMessage(*(events[i]));
		m_EventList.push_back(event);
	}

	m_StopSnoozing = 0;

	m_Visible = true;

	Refresh(false);

	ShowWindow(false);
}

SIZE CMessageWindow::CalcTextSize()
{
	SIZE textSize;
	textSize.cx = 0;
	textSize.cy = 0;
	
	m_Heights.clear();

	RECT rect = {0, 0, 0, 0};
	RECT margins = CConfig::Instance().GetMessageBoxTextMargins();
	int width = CConfig::Instance().GetMessageBoxMaxWidth() - margins.left - margins.right;
	width = max(0, width);

	if (width == 0) 
	{
		GetClientRect(GetDesktopWindow(), &rect);
		width = rect.right;
	}

	// Calculate the size of the texts
	for (int i = 0; i < m_EventList.size(); i++)
	{
		rect.right = width;
		
		const char* text = m_EventList[i]->GetMessage().c_str();
		m_ItemRasterizer->GetTextSize(text, &rect, true);

		int height = rect.bottom;
		m_Heights.push_back(height);

		textSize.cy += height + CConfig::Instance().GetMessageBoxSeparation();
		textSize.cx = max(textSize.cx, rect.right);
	}
	textSize.cy -= CConfig::Instance().GetMessageBoxSeparation();

	return textSize;
}

void CMessageWindow::Refresh(bool lite)
{
	if (m_Visible)
	{
		if (!lite)
		{
			if (m_ItemRasterizer) delete m_ItemRasterizer;

			m_ItemRasterizer=new CRasterizerFont;
			if(m_ItemRasterizer==NULL) THROW(ERR_OUTOFMEM);

			m_ItemRasterizer->SetFont(CConfig::Instance().GetMessageBoxFont());
			m_ItemRasterizer->SetAlign(CRasterizer::ALIGN_LEFT);
			m_ItemRasterizer->SetColor(CConfig::Instance().GetMessageBoxFontColor());

			m_MessageBoxBackground.Initialize();

			SIZE textSize = CalcTextSize();
			
			m_TextWidth = textSize.cx;
			m_TextHeight = textSize.cy;
			
			RECT workArea;
			GetClientRect(GetDesktopWindow(), &workArea);
			
			RECT margins = CConfig::Instance().GetMessageBoxTextMargins();

			m_Width = margins.left + margins.right + textSize.cx;
			m_Height = margins.top + margins.bottom + textSize.cy;
			
			m_X = (workArea.right - workArea.left - m_Width) / 2;
			m_Y = (workArea.bottom - workArea.top - m_Height) / 2;

			BackgroundCreateStruct bcs;
			bcs.pos.x = m_X;
			bcs.pos.y = m_Y;
			bcs.size.cx = m_Width;
			bcs.size.cy = m_Height;
			bcs.filename = CConfig::Instance().GetMessageBoxBitmapName();
			if (CConfig::Instance().GetMessageBoxBitmapName().empty())
			{
				bcs.mode = MODE_SOLID;
			}
			else
			{
				bcs.mode = MODE_RECT;
			}
			bcs.solidBevel = TRUE;
			bcs.solidColor= GetSysColor(COLOR_BTNFACE);
			bcs.resizeRect= CConfig::Instance().GetMessageBoxBitmapMargins();
			
			m_MessageBoxBackground.Create(bcs);

			// The size could be limited by the background, so we need to reset the width and height
			if (m_Width != m_MessageBoxBackground.GetWidth() || m_Height != m_MessageBoxBackground.GetHeight())
			{
				m_Width = m_MessageBoxBackground.GetWidth();
				m_Height = m_MessageBoxBackground.GetHeight();
				MoveWindow(m_X, m_Y);	// This also resizes the window
			}

			SetWindowZPos(CConfig::WINDOWPOS_ONTOP);		// Always on top
		}

		CRainWindow::RefreshBegin(lite);
		CRainWindow::RefreshEnd(lite);
	}
}


SIZE CMessageWindow::CalcWindowSize()
{
	SIZE size;

	// Width and Height are already calculated in the refresh
	size.cx = m_Width;
	size.cy = m_Height;

//	RECT margins = CConfig::Instance().GetMessageBoxTextMargins();
//
//	if (m_MessageBoxBackground.IsValid())
//	{
//		// The proper height has been calculated in the Refresh()
//		size.cx = max(m_TextWidth + margins.left + margins.left, m_MessageBoxBackground.GetWidth());
//		size.cy = max(m_TextHeight + margins.top + margins.bottom, m_MessageBoxBackground.GetHeight());
//	}
//	else
//	{
//		size.cx = m_TextWidth + margins.left + margins.left;
//		size.cy = m_TextHeight + margins.top + margins.bottom;
//	}

	return size;
}

void CMessageWindow::DrawWindow()
{
	int X, Y, W, H;

	CRasterizerFont::ClearBuffers();

	RECT rc = CConfig::Instance().GetMessageBoxTextMargins();

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
	for (int i = 0; i < m_EventList.size(); i++)
	{
		H = m_Heights[i];

		// Draw the text
		const char* text = m_EventList[i]->GetMessage().c_str();

		m_ItemRasterizer->WriteText(m_DoubleBuffer, X, Y, W, H, text, true);
		Y += m_Heights[i] + CConfig::Instance().GetMessageBoxSeparation();
	}

	// The text might be written to the buffers, so draw them
	if (CRasterizerFont::GetColorBuffer().GetBitmap() != NULL  && CRasterizerFont::GetAlphaBuffer().GetBitmap() != NULL)
	{
		CImage textImage;
		textImage.Create(CRasterizerFont::GetColorBuffer().GetBitmap(), CRasterizerFont::GetAlphaBuffer().GetBitmap(), 0);
		m_DoubleBuffer.Blit(textImage, 0, 0, 0, 0, textImage.GetWidth(), textImage.GetHeight());
	}
}

LRESULT CMessageWindow::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	// Just hide the window
	HideWindow();

	// Clear the events
	for (int i = 0; i < m_EventList.size(); i++)
	{
		delete m_EventList[i];
	}
	m_EventList.clear();

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

LRESULT CMessageWindow::OnTimer(WPARAM wParam, LPARAM lParam)
{
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

			case WM_LBUTTONDOWN: 
				return Window->OnLButtonDown(wParam, lParam);

			case WM_RBUTTONDOWN: 
				return Window->OnRButtonDown(wParam, lParam);

			case WM_TIMER: 
				return Window->OnTimer(wParam, lParam);
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
