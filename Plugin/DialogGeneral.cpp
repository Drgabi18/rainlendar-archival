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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/DialogGeneral.cpp,v 1.2 2001/12/23 10:00:18 rainy Exp $

  $Log: DialogGeneral.cpp,v $
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
	m_StartDelay = 0;
	m_StartHidden = FALSE;
	m_DisableHotkeys = FALSE;
	m_UseWindowName = FALSE;
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
	DDX_Text(pDX, IDC_START_DELAY, m_StartDelay);
	DDX_Check(pDX, IDC_START_HIDDEN, m_StartHidden);
	DDX_Check(pDX, IDC_DISABLE_HOTKEYS, m_DisableHotkeys);
	DDX_Check(pDX, IDC_USE_WINDOW_NAME, m_UseWindowName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CDialogGeneral)
	ON_BN_CLICKED(IDC_BACKGROUND_BROWSE, OnBackgroundBrowse)
	ON_EN_CHANGE(IDC_X, OnChangeX)
	ON_EN_CHANGE(IDC_Y, OnChangeY)
	ON_EN_CHANGE(IDC_BACKGROUND, OnChangeBackground)
	ON_EN_CHANGE(IDC_START_DELAY, OnChangeStartDelay)
	ON_BN_CLICKED(IDC_START_FROM_MONDAY, OnStartFromMonday)
	ON_BN_CLICKED(IDC_START_HIDDEN, OnStartHidden)
	ON_BN_CLICKED(IDC_DISABLE_HOTKEYS, OnDisableHotkeys)
	ON_BN_CLICKED(IDC_USE_WINDOW_NAME, OnUseWindowName)
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
	m_StartDelay=CCalendarWindow::c_Config.GetStartDelay();
	m_DisableHotkeys=CCalendarWindow::c_Config.GetDisableHotkeys();
	m_UseWindowName=CCalendarWindow::c_Config.GetUseWindowName();

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
	CCalendarWindow::c_Config.SetStartDelay(m_StartDelay);
	CCalendarWindow::c_Config.SetDisableHotkeys(m_DisableHotkeys==TRUE);
	CCalendarWindow::c_Config.SetUseWindowName(m_UseWindowName==TRUE);
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

void CDialogGeneral::OnChangeStartDelay() 
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
