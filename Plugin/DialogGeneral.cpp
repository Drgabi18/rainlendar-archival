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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/DialogGeneral.cpp,v 1.4 2002/02/27 18:56:13 rainy Exp $

  $Log: DialogGeneral.cpp,v $
  Revision 1.4  2002/02/27 18:56:13  rainy
  Added a lot of new options in the dialog.

  Revision 1.3  2002/01/15 17:58:42  rainy
  Removed the StartDelay

  Revision 1.2  2001/12/23 10:00:18  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#include "stdafx.h"
#include "rainlendardll.h"
#include "CalendarWindow.h"
#include "DialogGeneral.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogGeneral property page

IMPLEMENT_DYNCREATE(CDialogGeneral, CPropertyPage)

CDialogGeneral::CDialogGeneral() : CPropertyPage(CDialogGeneral::IDD)
{
	//{{AFX_DATA_INIT(CDialogGeneral)
	m_Background = _T("");
	m_X = 0;
	m_Y = 0;
	m_StartFromMonday = FALSE;
	m_StartHidden = FALSE;
	m_DisableHotkeys = FALSE;
	m_UseWindowName = FALSE;
	m_PollWallpaper = FALSE;
	m_Movable = FALSE;
	m_MouseHide = FALSE;
	//}}AFX_DATA_INIT
}

CDialogGeneral::~CDialogGeneral()
{
}

void CDialogGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogGeneral)
	DDX_Text(pDX, IDC_BACKGROUND, m_Background);
	DDX_Text(pDX, IDC_X, m_X);
	DDX_Text(pDX, IDC_Y, m_Y);
	DDX_Check(pDX, IDC_START_FROM_MONDAY, m_StartFromMonday);
	DDX_Check(pDX, IDC_START_HIDDEN, m_StartHidden);
	DDX_Check(pDX, IDC_DISABLE_HOTKEYS, m_DisableHotkeys);
	DDX_Check(pDX, IDC_USE_WINDOW_NAME, m_UseWindowName);
	DDX_Check(pDX, IDC_POLL_WALLPAPER, m_PollWallpaper);
	DDX_Check(pDX, IDC_WINDOW_MOVABLE, m_Movable);
	DDX_Check(pDX, IDC_WINDOW_MOUSEHIDE, m_MouseHide);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CDialogGeneral)
	ON_BN_CLICKED(IDC_BACKGROUND_BROWSE, OnBackgroundBrowse)
	ON_EN_CHANGE(IDC_X, OnChangeX)
	ON_EN_CHANGE(IDC_Y, OnChangeY)
	ON_EN_CHANGE(IDC_BACKGROUND, OnChangeBackground)
	ON_BN_CLICKED(IDC_START_FROM_MONDAY, OnStartFromMonday)
	ON_BN_CLICKED(IDC_START_HIDDEN, OnStartHidden)
	ON_BN_CLICKED(IDC_DISABLE_HOTKEYS, OnDisableHotkeys)
	ON_BN_CLICKED(IDC_USE_WINDOW_NAME, OnUseWindowName)
	ON_BN_CLICKED(IDC_POLL_WALLPAPER, OnPollWallpaper)
	ON_BN_CLICKED(IDC_BACKGROUND_TILE, OnBackgroundTile)
	ON_BN_CLICKED(IDC_BACKGROUND_STRETCH, OnBackgroundStretch)
	ON_BN_CLICKED(IDC_BACKGROUND_COPY, OnBackgroundCopy)
	ON_BN_CLICKED(IDC_WINDOW_MOVABLE, OnWindowMovable)
	ON_BN_CLICKED(IDC_WINDOW_NORMAL, OnWindowNormal)
	ON_BN_CLICKED(IDC_WINDOW_ONBOTTOM, OnWindowOnbottom)
	ON_BN_CLICKED(IDC_WINDOW_ONTOP, OnWindowOntop)
	ON_BN_CLICKED(IDC_WINDOW_MOUSEHIDE, OnWindowMousehide)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogGeneral message handlers

void CDialogGeneral::OnBackgroundBrowse() 
{
	CFileDialog FileDialog(TRUE);
	
	if(FileDialog.DoModal()) {
		m_Background=FileDialog.GetPathName();
		UpdateData(FALSE);
		SetModified(TRUE);
	}
}

BOOL CDialogGeneral::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_Background=CCalendarWindow::c_Config.GetBackgroundBitmapName();
	m_X=CCalendarWindow::c_Config.GetX();
	m_Y=CCalendarWindow::c_Config.GetY();
	m_StartFromMonday=CCalendarWindow::c_Config.GetStartFromMonday();
	m_StartHidden=CCalendarWindow::c_Config.GetStartHidden();
	m_DisableHotkeys=CCalendarWindow::c_Config.GetDisableHotkeys();
	m_UseWindowName=CCalendarWindow::c_Config.GetUseWindowName();
	m_PollWallpaper=CCalendarWindow::c_Config.GetPollWallpaper();
	m_Movable=CCalendarWindow::c_Config.GetMovable();
	m_MouseHide=CCalendarWindow::c_Config.GetMouseHide();

	switch(CCalendarWindow::c_Config.GetBackgroundMode()) {
	case CBackground::MODE_STRETCH:
		CheckRadioButton(IDC_BACKGROUND_TILE, IDC_BACKGROUND_COPY, IDC_BACKGROUND_STRETCH);
		break;

	case CBackground::MODE_COPY:
		CheckRadioButton(IDC_BACKGROUND_TILE, IDC_BACKGROUND_COPY, IDC_BACKGROUND_COPY);
		break;

	default:
		CheckRadioButton(IDC_BACKGROUND_TILE, IDC_BACKGROUND_COPY, IDC_BACKGROUND_TILE);
	}

	switch(CCalendarWindow::c_Config.GetWindowPos()) {
	case CConfig::WINDOWPOS_ONBOTTOM:
		CheckRadioButton(IDC_WINDOW_ONBOTTOM, IDC_WINDOW_ONTOP, IDC_WINDOW_ONBOTTOM);
		break;

	case CConfig::WINDOWPOS_NORMAL:
		CheckRadioButton(IDC_WINDOW_ONBOTTOM, IDC_WINDOW_ONTOP, IDC_WINDOW_NORMAL);
		break;

	default:
		CheckRadioButton(IDC_WINDOW_ONBOTTOM, IDC_WINDOW_ONTOP, IDC_WINDOW_ONTOP);
	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogGeneral::UpdateConfig()
{
	CCalendarWindow::c_Config.SetBackgroundBitmapName(m_Background);
	CCalendarWindow::c_Config.SetX(m_X);
	CCalendarWindow::c_Config.SetY(m_Y);
	CCalendarWindow::c_Config.SetStartFromMonday(m_StartFromMonday==TRUE);
	CCalendarWindow::c_Config.SetStartHidden(m_StartHidden==TRUE);
	CCalendarWindow::c_Config.SetDisableHotkeys(m_DisableHotkeys==TRUE);
	CCalendarWindow::c_Config.SetUseWindowName(m_UseWindowName==TRUE);
	CCalendarWindow::c_Config.SetPollWallpaper(m_PollWallpaper==TRUE);
	CCalendarWindow::c_Config.SetMovable(m_Movable==TRUE);
	CCalendarWindow::c_Config.SetMouseHide(m_MouseHide==TRUE);

	switch(GetCheckedRadioButton(IDC_BACKGROUND_TILE, IDC_BACKGROUND_COPY)) {
	case IDC_BACKGROUND_STRETCH:
		CCalendarWindow::c_Config.SetBackgroundMode(CBackground::MODE_STRETCH);
		break;

	case IDC_BACKGROUND_COPY:
		CCalendarWindow::c_Config.SetBackgroundMode(CBackground::MODE_COPY);
		break;

	default:
		CCalendarWindow::c_Config.SetBackgroundMode(CBackground::MODE_TILE);
	}

	switch(GetCheckedRadioButton(IDC_WINDOW_ONBOTTOM, IDC_WINDOW_ONTOP)) {
	case IDC_WINDOW_ONBOTTOM:
		CCalendarWindow::c_Config.SetWindowPos(CConfig::WINDOWPOS_ONBOTTOM);
		break;

	case IDC_WINDOW_NORMAL:
		CCalendarWindow::c_Config.SetWindowPos(CConfig::WINDOWPOS_NORMAL);
		break;

	default:
		CCalendarWindow::c_Config.SetWindowPos(CConfig::WINDOWPOS_ONTOP);
	}
}

void CDialogGeneral::OnOK() 
{
	UpdateConfig();

	CPropertyPage::OnOK();
}

BOOL CDialogGeneral::OnApply() 
{
	UpdateConfig();

	// Fetch the CCalendarWindow
	CWnd* Parent=GetParent();
	Parent=Parent->GetParent();

	Parent->SendMessage(WM_COMMAND, ID_REFRESH, 0);
	
	return TRUE;
}

void CDialogGeneral::OnChangeBackground() 
{
	SetModified(TRUE);
}

void CDialogGeneral::OnStartFromMonday() 
{
	SetModified(TRUE);
}

void CDialogGeneral::OnChangeX() 
{
	SetModified(TRUE);
}

void CDialogGeneral::OnChangeY() 
{
	SetModified(TRUE);
}

void CDialogGeneral::OnStartHidden() 
{
	SetModified(TRUE);
}

void CDialogGeneral::OnDisableHotkeys() 
{
	SetModified(TRUE);
}

void CDialogGeneral::OnUseWindowName() 
{
	SetModified(TRUE);
}

void CDialogGeneral::OnPollWallpaper() 
{
	SetModified(TRUE);
}

void CDialogGeneral::OnBackgroundTile() 
{
	SetModified(TRUE);
	GetDlgItem(IDC_BACKGROUND)->EnableWindow(true);
}

void CDialogGeneral::OnBackgroundStretch() 
{
	SetModified(TRUE);
	GetDlgItem(IDC_BACKGROUND)->EnableWindow(true);
}

void CDialogGeneral::OnBackgroundCopy() 
{
	SetModified(TRUE);
	GetDlgItem(IDC_BACKGROUND)->EnableWindow(false);
}

void CDialogGeneral::OnWindowMovable() 
{
	SetModified(TRUE);
}

void CDialogGeneral::OnWindowNormal() 
{
	SetModified(TRUE);
}

void CDialogGeneral::OnWindowOnbottom() 
{
	SetModified(TRUE);
}

void CDialogGeneral::OnWindowOntop() 
{
	SetModified(TRUE);
}

void CDialogGeneral::OnWindowMousehide() 
{
	SetModified(TRUE);
}
