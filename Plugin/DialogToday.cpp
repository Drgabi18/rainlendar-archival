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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/DialogToday.cpp,v 1.2 2001/12/23 10:00:18 rainy Exp $

  $Log: DialogToday.cpp,v $
  Revision 1.2  2001/12/23 10:00:18  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#include "stdafx.h"
#include "rainlendardll.h"
#include "CalendarWindow.h"
#include "DialogToday.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogToday property page

IMPLEMENT_DYNCREATE(CDialogToday, CPropertyPage)

CDialogToday::CDialogToday() : CPropertyPage(CDialogToday::IDD)
{
	//{{AFX_DATA_INIT(CDialogToday)
	m_Bitmap = _T("");
	m_Enable = FALSE;
	m_Font = _T("");
	//}}AFX_DATA_INIT
}

CDialogToday::~CDialogToday()
{
}

void CDialogToday::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogToday)
	DDX_Control(pDX, IDC_TODAY_RASTERIZER, m_Rasterizer);
	DDX_Control(pDX, IDC_TODAY_ALIGN, m_Align);
	DDX_Text(pDX, IDC_TODAY_BITMAP, m_Bitmap);
	DDX_Check(pDX, IDC_TODAY_ENABLE, m_Enable);
	DDX_Text(pDX, IDC_TODAY_FONTNAME, m_Font);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogToday, CPropertyPage)
	//{{AFX_MSG_MAP(CDialogToday)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_TODAY_ENABLE, OnTodayEnable)
	ON_CBN_SELCHANGE(IDC_TODAY_RASTERIZER, OnSelchangeTodayRasterizer)
	ON_CBN_SELCHANGE(IDC_TODAY_ALIGN, OnSelchangeTodayAlign)
	ON_EN_CHANGE(IDC_TODAY_BITMAP, OnChangeTodayBitmap)
	ON_BN_CLICKED(IDC_TODAY_COMPONENTS_1, OnTodayComponents1)
	ON_BN_CLICKED(IDC_TODAY_COMPONENTS_10, OnTodayComponents10)
	ON_BN_CLICKED(IDC_TODAY_COMPONENTS_32, OnTodayComponents32)
	ON_BN_CLICKED(IDC_TODAY_FONTCOLOR, OnTodayFontcolor)
	ON_EN_CHANGE(IDC_TODAY_FONTNAME, OnChangeTodayFontname)
	ON_BN_CLICKED(IDC_TODAY_SELECT, OnTodaySelect)
	ON_BN_CLICKED(IDC_TODAY_BROWSE, OnTodayBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogToday message handlers

void CDialogToday::UpdateConfig()
{
	CCalendarWindow::c_Config.SetTodayEnable(m_Enable==TRUE);
	CCalendarWindow::c_Config.SetTodayBitmapName(m_Bitmap);
	CCalendarWindow::c_Config.SetTodayFontColor(m_FontColor);
	CCalendarWindow::c_Config.SetTodayFont(m_RealFont);

	switch(m_Rasterizer.GetCurSel()) {
	case 1:
		CCalendarWindow::c_Config.SetTodayRasterizer(CRasterizer::TYPE_BITMAP);
		break;
	default:
		CCalendarWindow::c_Config.SetTodayRasterizer(CRasterizer::TYPE_FONT);
	}

	switch(m_Align.GetCurSel()) {
	case 1:
		CCalendarWindow::c_Config.SetTodayAlign(CRasterizer::ALIGN_CENTER);
		break;
	case 2:
		CCalendarWindow::c_Config.SetTodayAlign(CRasterizer::ALIGN_RIGHT);
		break;
	default:
		CCalendarWindow::c_Config.SetTodayAlign(CRasterizer::ALIGN_LEFT);
	}

	switch(GetCheckedRadioButton(IDC_TODAY_COMPONENTS_1, IDC_TODAY_COMPONENTS_32)) {
	case IDC_TODAY_COMPONENTS_32:
		CCalendarWindow::c_Config.SetTodayNumOfComponents(32);
		break;
	case IDC_TODAY_COMPONENTS_10:
		CCalendarWindow::c_Config.SetTodayNumOfComponents(10);
		break;
	default:
		CCalendarWindow::c_Config.SetTodayNumOfComponents(1);
	}
}

BOOL CDialogToday::OnApply() 
{
	UpdateConfig();

	// Fetch the CCalendarWindow
	CWnd* Parent=GetParent();
	Parent=Parent->GetParent();

	Parent->SendMessage(WM_COMMAND, ID_REFRESH, 0);
	
	return TRUE;
}

void CDialogToday::OnOK() 
{
	UpdateConfig();
	
	CPropertyPage::OnOK();
}

void CDialogToday::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	HBRUSH Brush=CreateSolidBrush(m_FontColor);
	FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, Brush);
	DeleteObject(Brush);
	
	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

BOOL CDialogToday::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_Enable=CCalendarWindow::c_Config.GetTodayEnable();
	m_Bitmap=CCalendarWindow::c_Config.GetTodayBitmapName();

	switch(CCalendarWindow::c_Config.GetTodayRasterizer()) {
	case CRasterizer::TYPE_BITMAP:
		m_Rasterizer.SetCurSel(1);
		break;
	case CRasterizer::TYPE_FONT:
	default:
		m_Rasterizer.SetCurSel(0);
	}
	
	switch(CCalendarWindow::c_Config.GetTodayAlign()) {
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

	switch(CCalendarWindow::c_Config.GetTodayNumOfComponents()) {
	case 32:
		CheckRadioButton(IDC_TODAY_COMPONENTS_1, IDC_TODAY_COMPONENTS_32, IDC_TODAY_COMPONENTS_32);
		break;
	case 10:
		CheckRadioButton(IDC_TODAY_COMPONENTS_1, IDC_TODAY_COMPONENTS_32, IDC_TODAY_COMPONENTS_10);
		break;
	default:
		CheckRadioButton(IDC_TODAY_COMPONENTS_1, IDC_TODAY_COMPONENTS_32, IDC_TODAY_COMPONENTS_1);
	}

	m_RealFont=CCalendarWindow::c_Config.GetTodayFont();
	m_Font=m_RealFont.Mid(m_RealFont.ReverseFind('/')+1);

	m_FontColor=CCalendarWindow::c_Config.GetTodayFontColor();

	UpdateData(FALSE);

	OnTodayEnable();
	OnSelchangeTodayRasterizer();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDialogToday::OnTodayEnable() 
{
	UpdateData();
	
	if(m_Enable) {
		GetDlgItem(IDC_TODAY_RASTERIZER)->EnableWindow(true);
		GetDlgItem(IDC_TODAY_ALIGN)->EnableWindow(true);
		OnSelchangeTodayRasterizer();
	} else {
		GetDlgItem(IDC_TODAY_RASTERIZER)->EnableWindow(false);
		GetDlgItem(IDC_TODAY_SELECT)->EnableWindow(false);
		GetDlgItem(IDC_TODAY_BROWSE)->EnableWindow(false);
		GetDlgItem(IDC_TODAY_ALIGN)->EnableWindow(false);
		GetDlgItem(IDC_TODAY_BITMAP)->EnableWindow(false);
		GetDlgItem(IDC_TODAY_FONTCOLOR)->EnableWindow(false);
		GetDlgItem(IDC_TODAY_COMPONENTS_1)->EnableWindow(false);
		GetDlgItem(IDC_TODAY_COMPONENTS_10)->EnableWindow(false);
		GetDlgItem(IDC_TODAY_COMPONENTS_32)->EnableWindow(false);
	}

	SetModified(TRUE);
}

void CDialogToday::OnSelchangeTodayRasterizer() 
{
	UpdateData();

	if(m_Enable) {
		switch(m_Rasterizer.GetCurSel()) {
		case 1:
			GetDlgItem(IDC_TODAY_BROWSE)->EnableWindow(true);
			GetDlgItem(IDC_TODAY_BITMAP)->EnableWindow(true);
			GetDlgItem(IDC_TODAY_COMPONENTS_1)->EnableWindow(true);
			GetDlgItem(IDC_TODAY_COMPONENTS_10)->EnableWindow(true);
			GetDlgItem(IDC_TODAY_COMPONENTS_32)->EnableWindow(true);
			GetDlgItem(IDC_TODAY_SELECT)->EnableWindow(false);
			GetDlgItem(IDC_TODAY_FONTCOLOR)->EnableWindow(false);
			break;
		default:
			GetDlgItem(IDC_TODAY_BROWSE)->EnableWindow(false);
			GetDlgItem(IDC_TODAY_BITMAP)->EnableWindow(false);
			GetDlgItem(IDC_TODAY_COMPONENTS_1)->EnableWindow(false);
			GetDlgItem(IDC_TODAY_COMPONENTS_10)->EnableWindow(false);
			GetDlgItem(IDC_TODAY_COMPONENTS_32)->EnableWindow(false);
			GetDlgItem(IDC_TODAY_SELECT)->EnableWindow(true);
			GetDlgItem(IDC_TODAY_FONTCOLOR)->EnableWindow(true);
		}
	}

	SetModified(TRUE);

}

void CDialogToday::OnTodayFontcolor() 
{
	CColorDialog ColorDialog(m_FontColor);

	if(IDOK==ColorDialog.DoModal()) {
		m_FontColor=ColorDialog.GetColor();
		GetDlgItem(IDC_TODAY_FONTCOLOR)->Invalidate();
		SetModified(TRUE);
	}
}


void CDialogToday::OnTodaySelect() 
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

		GetDlgItem(IDC_TODAY_FONTNAME)->SetWindowText(m_Font);
		SetModified(TRUE);
	}

}

void CDialogToday::OnTodayBrowse() 
{
	CFileDialog FileDialog(TRUE);
		
	if(IDOK==FileDialog.DoModal()) {
		m_Bitmap=FileDialog.GetPathName();
		GetDlgItem(IDC_TODAY_BITMAP)->SetWindowText(m_Bitmap);
		SetModified(TRUE);
	}
}

void CDialogToday::OnSelchangeTodayAlign() 
{
	SetModified(TRUE);
}

void CDialogToday::OnChangeTodayBitmap() 
{
	SetModified(TRUE);
}

void CDialogToday::OnTodayComponents1() 
{
	SetModified(TRUE);
}

void CDialogToday::OnTodayComponents10() 
{
	SetModified(TRUE);
}

void CDialogToday::OnTodayComponents32() 
{
	SetModified(TRUE);
}

void CDialogToday::OnChangeTodayFontname() 
{
	SetModified(TRUE);
}
