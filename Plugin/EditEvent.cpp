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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/EditEvent.cpp,v 1.3 2002/01/10 16:47:15 rainy Exp $

  $Log: EditEvent.cpp,v $
  Revision 1.3  2002/01/10 16:47:15  rainy
  Added support for bitmap/color,

  Revision 1.2  2001/12/23 10:01:46  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#include "stdafx.h"
#include "rainlendardll.h"
#include "CalendarWindow.h"
#include "EditEvent.h"
#include "Resource.h"
#include "Rasterizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditEvent dialog


CEditEvent::CEditEvent(CWnd* pParent /*=NULL*/)
	: CDialog(CEditEvent::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditEvent)
	m_Message = _T("");
	m_Label = _T("");
	m_Bitmap = _T("");
	//}}AFX_DATA_INIT

	m_Day = 0;
	m_FontColor = 0;
}


void CEditEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditEvent)
	DDX_Text(pDX, IDC_EDITEVENT_MESSAGE, m_Message);
	DDX_Text(pDX, IDC_EDITEVENT_LABEL, m_Label);
	DDX_Text(pDX, IDC_EDITEVENT_BITMAP, m_Bitmap);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditEvent, CDialog)
	//{{AFX_MSG_MAP(CEditEvent)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_EDITEVENT_BROWSE, OnEditeventBrowse)
	ON_BN_CLICKED(IDC_EDITEVENT_FONTCOLOR, OnEditeventFontcolor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditEvent message handlers

BOOL CEditEvent::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString Caption;
	Caption.Format("%i-%s-%i", m_Day, CCalendarWindow::c_CurrentDate.Format("%B"), CCalendarWindow::c_CurrentDate.GetYear());
	SetWindowText(Caption);

	ReadEvent();

	GetDlgItem(IDC_EDITEVENT_MESSAGE)->SetFocus();

	// Hide unnecessary widgets
	if(CCalendarWindow::c_Config.GetEventRasterizer() == CRasterizer::TYPE_BITMAP)
	{
		GetDlgItem(IDC_EDITEVENT_FONTCOLOR)->ShowWindow(SW_HIDE);
		m_Label = "Filename:";
	}
	else	// FONT rasterizer
	{
		GetDlgItem(IDC_EDITEVENT_BROWSE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDITEVENT_BITMAP)->ShowWindow(SW_HIDE);
		m_Label = "Font color:";
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/* 
** ReadEvent
**
** Reads the Event for selected day
**
*/
void CEditEvent::ReadEvent()
{
	char tmpSz[MAX_LINE_LENGTH];
	char IniPath[MAX_LINE_LENGTH];
	CTime Current=CCalendarWindow::c_CurrentDate;
	CString Date;
	char* Slash;

	// Get the DLL's directory
	GetModuleFileName(GetModuleHandle("Rainlendar.dll"), IniPath, MAX_LINE_LENGTH);

	Slash=strrchr(IniPath, '\\');
	if(Slash==NULL) {
		strcpy(IniPath, "Events.ini");
	} else {
		strcpy(Slash, "\\Events.ini");
	}

	Date.Format("%i-%i-%i", m_Day, Current.GetMonth(), Current.GetYear());

	if(GetPrivateProfileString( Date, "Message", "", tmpSz, 255, IniPath) > 0) 
	{
		m_Message=tmpSz;
		m_Message.Replace("\\n", "\r\n");
	}

	if(GetPrivateProfileString( Date, "FontColor", "", tmpSz, 255, IniPath) > 0) 
	{
		sscanf(tmpSz, "%X", &m_FontColor);
	}
	else
	{
		m_FontColor = CCalendarWindow::c_Config.GetEventFontColor();
	}

	if(GetPrivateProfileString( Date, "Bitmap", "", tmpSz, 255, IniPath) > 0) 
	{
		m_Bitmap = tmpSz;
	}
	else
	{
		m_Bitmap = CCalendarWindow::c_Config.GetEventBitmapName();
	}

	UpdateData(FALSE);
} 

/* 
** WriteEvent
**
** Writes the Event for selected day
**
*/
void CEditEvent::WriteEvent()
{
	char IniPath[MAX_LINE_LENGTH];
	CTime Current=CCalendarWindow::c_CurrentDate;
	CString Date;
	char* Slash;
	char tmpSz[MAX_LINE_LENGTH];

	// Get the DLL's directory
	GetModuleFileName(GetModuleHandle("Rainlendar.dll"), IniPath, MAX_LINE_LENGTH);

	Slash=strrchr(IniPath, '\\');
	if(Slash==NULL) {
		strcpy(IniPath, "Events.ini");
	} else {
		strcpy(Slash, "\\Events.ini");
	}

	Date.Format("%i-%i-%i", m_Day, Current.GetMonth(), Current.GetYear());

	m_Message.Replace("\n", "\\n");
	m_Message.Replace("\r", "");
	WritePrivateProfileString( Date, "Message", m_Message, IniPath );

	if(CCalendarWindow::c_Config.GetEventFontColor() != m_FontColor)
	{
		sprintf(tmpSz, "%X", m_FontColor);
		WritePrivateProfileString( Date, "FontColor", tmpSz, IniPath );
	}
	else
	{
		WritePrivateProfileString( Date, "FontColor", "", IniPath );
	}

	if(CCalendarWindow::c_Config.GetEventBitmapName() != m_Bitmap)
	{
		WritePrivateProfileString( Date, "Bitmap", m_Bitmap, IniPath );
	}
	else
	{
		WritePrivateProfileString( Date, "Bitmap", "", IniPath );
	}

	// Write timestamp to know when this was last modified
	time_t ltime;
	time(&ltime);
	sprintf(tmpSz, "%ld", ltime);
	WritePrivateProfileString( Date, "TimeStamp", tmpSz, IniPath );
} 

void CEditEvent::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if(nIDCtl == IDC_EDITEVENT_FONTCOLOR)
	{
		HBRUSH Brush=CreateSolidBrush(m_FontColor);
		FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, Brush);
		DeleteObject(Brush);
		return;
	}
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CEditEvent::OnEditeventBrowse() 
{
	CFileDialog FileDialog(TRUE);
		
	if(IDOK==FileDialog.DoModal()) 
	{
		GetDlgItem(IDC_EDITEVENT_BITMAP)->SetWindowText(FileDialog.GetPathName());
	}
}

void CEditEvent::OnEditeventFontcolor() 
{
	CColorDialog ColorDialog(m_FontColor);

	if(IDOK==ColorDialog.DoModal()) 
	{
		m_FontColor=ColorDialog.GetColor();
		GetDlgItem(IDC_EDITEVENT_FONTCOLOR)->Invalidate();
	}
}
