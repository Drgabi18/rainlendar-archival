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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/DialogEvent.cpp,v 1.2 2001/12/23 10:00:18 rainy Exp $

  $Log: DialogEvent.cpp,v $
  Revision 1.2  2001/12/23 10:00:18  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#include "stdafx.h"
#include "rainlendardll.h"
#include "CalendarWindow.h"
#include "DialogEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogEvent property page

IMPLEMENT_DYNCREATE(CDialogEvent, CPropertyPage)

CDialogEvent::CDialogEvent() : CPropertyPage(CDialogEvent::IDD)
{
	//{{AFX_DATA_INIT(CDialogEvent)
	m_Bitmap = _T("");
	m_Enable = FALSE;
	m_Font = _T("");
	m_ShowToolTips = FALSE;
	m_ShowMessageBox = FALSE;
	m_Execute = _T("");
	//}}AFX_DATA_INIT
}

CDialogEvent::~CDialogEvent()
{
}

void CDialogEvent::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogEvent)
	DDX_Control(pDX, IDC_EVENT_RASTERIZER, m_Rasterizer);
	DDX_Control(pDX, IDC_EVENT_ALIGN, m_Align);
	DDX_Text(pDX, IDC_EVENT_BITMAP, m_Bitmap);
	DDX_Check(pDX, IDC_EVENT_ENABLE, m_Enable);
	DDX_Text(pDX, IDC_EVENT_FONTNAME, m_Font);
	DDX_Check(pDX, IDC_EVENT_TOOLTIP, m_ShowToolTips);
	DDX_Check(pDX, IDC_EVENT_MESSAGEBOX, m_ShowMessageBox);
	DDX_Text(pDX, IDC_EVENT_EXECUTE, m_Execute);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogEvent, CPropertyPage)
	//{{AFX_MSG_MAP(CDialogEvent)
	ON_WM_DRAWITEM()
	ON_CBN_SELCHANGE(IDC_EVENT_ALIGN, OnSelchangeEventAlign)
	ON_EN_CHANGE(IDC_EVENT_BITMAP, OnChangeEventBitmap)
	ON_BN_CLICKED(IDC_EVENT_BROWSE, OnEventBrowse)
	ON_BN_CLICKED(IDC_EVENT_COMPONENTS_1, OnEventComponents1)
	ON_BN_CLICKED(IDC_EVENT_COMPONENTS_10, OnEventComponents10)
	ON_BN_CLICKED(IDC_EVENT_COMPONENTS_32, OnEventComponents32)
	ON_BN_CLICKED(IDC_EVENT_ENABLE, OnEventEnable)
	ON_BN_CLICKED(IDC_EVENT_FONTCOLOR, OnEventFontcolor)
	ON_EN_CHANGE(IDC_EVENT_FONTNAME, OnChangeEventFontname)
	ON_CBN_SELCHANGE(IDC_EVENT_RASTERIZER, OnSelchangeEventRasterizer)
	ON_BN_CLICKED(IDC_EVENT_SELECT, OnEventSelect)
	ON_BN_CLICKED(IDC_EVENT_TOOLTIP, OnEventTooltip)
	ON_BN_CLICKED(IDC_EVENT_MESSAGEBOX, OnEventMessagebox)
	ON_EN_CHANGE(IDC_EVENT_EXECUTE, OnChangeEventExecute)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogEvent message handlers

void CDialogEvent::UpdateConfig()
{
	CCalendarWindow::c_Config.SetEventEnable(m_Enable==TRUE);
	CCalendarWindow::c_Config.SetEventBitmapName(m_Bitmap);
	CCalendarWindow::c_Config.SetEventFontColor(m_FontColor);
	CCalendarWindow::c_Config.SetEventFont(m_RealFont);

	switch(m_Rasterizer.GetCurSel()) {
	case 1:
		CCalendarWindow::c_Config.SetEventRasterizer(CRasterizer::TYPE_BITMAP);
		break;
	default:
		CCalendarWindow::c_Config.SetEventRasterizer(CRasterizer::TYPE_FONT);
	}

	switch(m_Align.GetCurSel()) {
	case 1:
		CCalendarWindow::c_Config.SetEventAlign(CRasterizer::ALIGN_CENTER);
		break;
	case 2:
		CCalendarWindow::c_Config.SetEventAlign(CRasterizer::ALIGN_RIGHT);
		break;
	default:
		CCalendarWindow::c_Config.SetEventAlign(CRasterizer::ALIGN_LEFT);
	}

	switch(GetCheckedRadioButton(IDC_EVENT_COMPONENTS_1, IDC_EVENT_COMPONENTS_32)) {
	case IDC_EVENT_COMPONENTS_32:
		CCalendarWindow::c_Config.SetEventNumOfComponents(32);
		break;
	case IDC_EVENT_COMPONENTS_10:
		CCalendarWindow::c_Config.SetEventNumOfComponents(10);
		break;
	default:
		CCalendarWindow::c_Config.SetEventNumOfComponents(1);
	}

	CCalendarWindow::c_Config.SetEventExecute(m_Execute);
	CCalendarWindow::c_Config.SetEventToolTips(m_ShowToolTips==TRUE);
	CCalendarWindow::c_Config.SetEventMessageBox(m_ShowMessageBox==TRUE);
}

BOOL CDialogEvent::OnApply() 
{
	UpdateConfig();

	// Fetch the CCalendarWindow
	CWnd* Parent=GetParent();
	Parent=Parent->GetParent();

	Parent->SendMessage(WM_COMMAND, ID_REFRESH, 0);
	
	return TRUE;
}

void CDialogEvent::OnOK() 
{
	UpdateConfig();
	
	CPropertyPage::OnOK();
}

void CDialogEvent::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	HBRUSH Brush=CreateSolidBrush(m_FontColor);
	FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, Brush);
	DeleteObject(Brush);
	
	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

BOOL CDialogEvent::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_Enable=CCalendarWindow::c_Config.GetEventEnable();
	m_Bitmap=CCalendarWindow::c_Config.GetEventBitmapName();

	switch(CCalendarWindow::c_Config.GetEventRasterizer()) {
	case CRasterizer::TYPE_BITMAP:
		m_Rasterizer.SetCurSel(1);
		break;
	case CRasterizer::TYPE_FONT:
	default:
		m_Rasterizer.SetCurSel(0);
	}
	
	switch(CCalendarWindow::c_Config.GetEventAlign()) {
	case CRasterizer::ALIGN_RIGHT:
		m_Align.SetCurSel(2);
		break;
	case CRasterizer::ALIGN_CENTER:
		m_Align.SetCurSel(1);
		break;
	case CRasterizer::ALIGN_LEFT:
	default:
		m_Align.SetCurSel(0);
	}

	switch(CCalendarWindow::c_Config.GetEventNumOfComponents()) {
	case 32:
		CheckRadioButton(IDC_EVENT_COMPONENTS_1, IDC_EVENT_COMPONENTS_32, IDC_EVENT_COMPONENTS_32);
		break;
	case 10:
		CheckRadioButton(IDC_EVENT_COMPONENTS_1, IDC_EVENT_COMPONENTS_32, IDC_EVENT_COMPONENTS_10);
		break;
	default:
		CheckRadioButton(IDC_EVENT_COMPONENTS_1, IDC_EVENT_COMPONENTS_32, IDC_EVENT_COMPONENTS_1);
	}

	m_RealFont=CCalendarWindow::c_Config.GetEventFont();
	m_Font=m_RealFont.Mid(m_RealFont.ReverseFind('/')+1);

	m_FontColor=CCalendarWindow::c_Config.GetEventFontColor();

	m_ShowMessageBox=CCalendarWindow::c_Config.GetEventMessageBox();
	m_ShowToolTips=CCalendarWindow::c_Config.GetEventToolTips();
	m_Execute=CCalendarWindow::c_Config.GetEventExecute();

	UpdateData(FALSE);

	OnEventEnable();
	OnSelchangeEventRasterizer();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogEvent::OnSelchangeEventAlign() 
{
	SetModified(TRUE);
}

void CDialogEvent::OnChangeEventBitmap() 
{
	SetModified(TRUE);
}

void CDialogEvent::OnEventBrowse() 
{
	CFileDialog FileDialog(TRUE);
		
	if(IDOK==FileDialog.DoModal()) {
		m_Bitmap=FileDialog.GetPathName();
		GetDlgItem(IDC_EVENT_BITMAP)->SetWindowText(m_Bitmap);
		SetModified(TRUE);
	}
}

void CDialogEvent::OnEventComponents1() 
{
	SetModified(TRUE);
}

void CDialogEvent::OnEventComponents10() 
{
	SetModified(TRUE);
}

void CDialogEvent::OnEventComponents32() 
{
	SetModified(TRUE);
}

void CDialogEvent::OnEventEnable() 
{
	UpdateData();
	
	if(m_Enable) {
		GetDlgItem(IDC_EVENT_RASTERIZER)->EnableWindow(true);
		GetDlgItem(IDC_EVENT_ALIGN)->EnableWindow(true);
		GetDlgItem(IDC_EVENT_TOOLTIP)->EnableWindow(true);
		GetDlgItem(IDC_EVENT_MESSAGEBOX)->EnableWindow(true);
		GetDlgItem(IDC_EVENT_EXECUTE)->EnableWindow(true);
		OnSelchangeEventRasterizer();
	} else {
		GetDlgItem(IDC_EVENT_RASTERIZER)->EnableWindow(false);
		GetDlgItem(IDC_EVENT_SELECT)->EnableWindow(false);
		GetDlgItem(IDC_EVENT_BROWSE)->EnableWindow(false);
		GetDlgItem(IDC_EVENT_ALIGN)->EnableWindow(false);
		GetDlgItem(IDC_EVENT_BITMAP)->EnableWindow(false);
		GetDlgItem(IDC_EVENT_FONTCOLOR)->EnableWindow(false);
		GetDlgItem(IDC_EVENT_COMPONENTS_1)->EnableWindow(false);
		GetDlgItem(IDC_EVENT_COMPONENTS_10)->EnableWindow(false);
		GetDlgItem(IDC_EVENT_COMPONENTS_32)->EnableWindow(false);
		GetDlgItem(IDC_EVENT_TOOLTIP)->EnableWindow(false);
		GetDlgItem(IDC_EVENT_MESSAGEBOX)->EnableWindow(false);
		GetDlgItem(IDC_EVENT_EXECUTE)->EnableWindow(false);
	}

	SetModified(TRUE);
}

void CDialogEvent::OnEventFontcolor() 
{
	CColorDialog ColorDialog(m_FontColor);

	if(IDOK==ColorDialog.DoModal()) {
		m_FontColor=ColorDialog.GetColor();
		GetDlgItem(IDC_EVENT_FONTCOLOR)->Invalidate();
		SetModified(TRUE);
	}
}

void CDialogEvent::OnChangeEventFontname() 
{
	SetModified(TRUE);
}

void CDialogEvent::OnSelchangeEventRasterizer() 
{
	UpdateData();

	if(m_Enable) {
		switch(m_Rasterizer.GetCurSel()) {
		case 1:
			GetDlgItem(IDC_EVENT_BROWSE)->EnableWindow(true);
			GetDlgItem(IDC_EVENT_BITMAP)->EnableWindow(true);
			GetDlgItem(IDC_EVENT_COMPONENTS_1)->EnableWindow(true);
			GetDlgItem(IDC_EVENT_COMPONENTS_10)->EnableWindow(true);
			GetDlgItem(IDC_EVENT_COMPONENTS_32)->EnableWindow(true);
			GetDlgItem(IDC_EVENT_SELECT)->EnableWindow(false);
			GetDlgItem(IDC_EVENT_FONTCOLOR)->EnableWindow(false);
			break;
		default:
			GetDlgItem(IDC_EVENT_BROWSE)->EnableWindow(false);
			GetDlgItem(IDC_EVENT_BITMAP)->EnableWindow(false);
			GetDlgItem(IDC_EVENT_COMPONENTS_1)->EnableWindow(false);
			GetDlgItem(IDC_EVENT_COMPONENTS_10)->EnableWindow(false);
			GetDlgItem(IDC_EVENT_COMPONENTS_32)->EnableWindow(false);
			GetDlgItem(IDC_EVENT_SELECT)->EnableWindow(true);
			GetDlgItem(IDC_EVENT_FONTCOLOR)->EnableWindow(true);
		}
	}

	SetModified(TRUE);

}

void CDialogEvent::OnEventSelect() 
{
	LOGFONT lf;
	sscanf(m_RealFont, "%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i", 
					&lf.lfHeight, &lf.lfWidth, &lf.lfEscapement, &lf.lfOrientation, &lf.lfWeight,
					&lf.lfItalic, &lf.lfUnderline, &lf.lfStrikeOut, &lf.lfCharSet, &lf.lfOutPrecision, 
					&lf.lfClipPrecision, &lf.lfQuality, &lf.lfPitchAndFamily);

	strncpy(lf.lfFaceName, m_Font, 32);

	CFontDialog	FontDialog(&lf, CF_SCREENFONTS);

	if(IDOK==FontDialog.DoModal()) {
		m_RealFont.Format("%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%s",
			FontDialog.m_lf.lfHeight,
			FontDialog.m_lf.lfWidth,
			FontDialog.m_lf.lfEscapement,
			FontDialog.m_lf.lfOrientation,
			FontDialog.m_lf.lfWeight,
			FontDialog.m_lf.lfItalic,
			FontDialog.m_lf.lfUnderline,
			FontDialog.m_lf.lfStrikeOut,
			FontDialog.m_lf.lfCharSet,
			FontDialog.m_lf.lfOutPrecision,
			FontDialog.m_lf.lfClipPrecision,
			FontDialog.m_lf.lfQuality,
			FontDialog.m_lf.lfPitchAndFamily,
			FontDialog.m_lf.lfFaceName);

		m_Font=m_RealFont.Mid(m_RealFont.ReverseFind('/')+1);

		GetDlgItem(IDC_EVENT_FONTNAME)->SetWindowText(m_Font);
		SetModified(TRUE);
	}
}


void CDialogEvent::OnEventTooltip() 
{
	SetModified(TRUE);
}

void CDialogEvent::OnEventMessagebox() 
{
	SetModified(TRUE);
}

void CDialogEvent::OnChangeEventExecute() 
{
	SetModified(TRUE);
}
