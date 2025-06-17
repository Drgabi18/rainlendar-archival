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

// DialogYear.cpp : implementation file
//

#include "stdafx.h"
#include "DialogYear.h"
#include "CalendarWindow.h"
#include "rainlendardll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogYear property page

IMPLEMENT_DYNCREATE(CDialogYear, CPropertyPage)

CDialogYear::CDialogYear() : CPropertyPage(CDialogYear::IDD)
{
	//{{AFX_DATA_INIT(CDialogYear)
	m_Bitmap = _T("");
	m_Enable = FALSE;
	m_Font = _T("");
	m_X = 0;
	m_Y = 0;
	//}}AFX_DATA_INIT
}

CDialogYear::~CDialogYear()
{
}

void CDialogYear::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogYear)
	DDX_Control(pDX, IDC_YEAR_RASTERIZER, m_Rasterizer);
	DDX_Control(pDX, IDC_YEAR_ALIGN, m_Align);
	DDX_Text(pDX, IDC_YEAR_BITMAP, m_Bitmap);
	DDX_Check(pDX, IDC_YEAR_ENABLE, m_Enable);
	DDX_Text(pDX, IDC_YEAR_FONTNAME, m_Font);
	DDX_Text(pDX, IDC_YEAR_X, m_X);
	DDX_Text(pDX, IDC_YEAR_Y, m_Y);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogYear, CPropertyPage)
	//{{AFX_MSG_MAP(CDialogYear)
	ON_BN_CLICKED(IDC_YEAR_ENABLE, OnYearEnable)
	ON_CBN_SELCHANGE(IDC_YEAR_RASTERIZER, OnSelchangeYearRasterizer)
	ON_BN_CLICKED(IDC_YEAR_BROWSE, OnYearBrowse)
	ON_BN_CLICKED(IDC_YEAR_SELECT, OnYearSelect)
	ON_BN_CLICKED(IDC_YEAR_FONTCOLOR, OnYearFontcolor)
	ON_WM_DRAWITEM()
	ON_CBN_SELCHANGE(IDC_YEAR_ALIGN, OnSelchangeYearAlign)
	ON_EN_CHANGE(IDC_YEAR_BITMAP, OnChangeYearBitmap)
	ON_EN_CHANGE(IDC_YEAR_X, OnChangeYearX)
	ON_EN_CHANGE(IDC_YEAR_Y, OnChangeYearY)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogYear message handlers

void CDialogYear::UpdateConfig()
{
	CCalendarWindow::C_Config.SetYearEnable(m_Enable==TRUE);
	CCalendarWindow::C_Config.SetYearBitmapName(m_Bitmap);
	CCalendarWindow::C_Config.SetYearFontColor(m_FontColor);
	CCalendarWindow::C_Config.SetYearFont(m_RealFont);
	CCalendarWindow::C_Config.SetYearX(m_X);
	CCalendarWindow::C_Config.SetYearY(m_Y);

	switch(m_Rasterizer.GetCurSel()) {
	case 1:
		CCalendarWindow::C_Config.SetYearRasterizer(CRasterizer::TYPE_BITMAP);
		break;
	default:
		CCalendarWindow::C_Config.SetYearRasterizer(CRasterizer::TYPE_FONT);
	}

	switch(m_Align.GetCurSel()) {
	case 1:
		CCalendarWindow::C_Config.SetYearAlign(CRasterizer::ALIGN_CENTER);
		break;
	case 2:
		CCalendarWindow::C_Config.SetYearAlign(CRasterizer::ALIGN_RIGHT);
		break;
	default:
		CCalendarWindow::C_Config.SetYearAlign(CRasterizer::ALIGN_LEFT);
	}

}

BOOL CDialogYear::OnApply() 
{
	UpdateConfig();

	// Fetch the CCalendarWindow
	CWnd* Parent=GetParent();
	Parent=Parent->GetParent();

	Parent->SendMessage(WM_COMMAND, ID_REFRESH, 0);
	
	return TRUE;
}

void CDialogYear::OnOK() 
{
	UpdateConfig();
	
	CPropertyPage::OnOK();
}

BOOL CDialogYear::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_Enable=CCalendarWindow::C_Config.GetYearEnable();
	m_Bitmap=CCalendarWindow::C_Config.GetYearBitmapName();
	m_X=CCalendarWindow::C_Config.GetYearX();
	m_Y=CCalendarWindow::C_Config.GetYearY();

	switch(CCalendarWindow::C_Config.GetYearRasterizer()) {
	case CRasterizer::TYPE_BITMAP:
		m_Rasterizer.SetCurSel(1);
		break;
	case CRasterizer::TYPE_FONT:
	default:
		m_Rasterizer.SetCurSel(0);
	}
	
	switch(CCalendarWindow::C_Config.GetYearAlign()) {
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

	m_RealFont=CCalendarWindow::C_Config.GetYearFont();
	m_Font=m_RealFont.Mid(m_RealFont.ReverseFind('/')+1);

	m_FontColor=CCalendarWindow::C_Config.GetYearFontColor();

	UpdateData(FALSE);

	OnYearEnable();
	OnSelchangeYearRasterizer();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogYear::OnYearEnable() 
{
	UpdateData();
	
	if(m_Enable) {
		GetDlgItem(IDC_YEAR_RASTERIZER)->EnableWindow(true);
		GetDlgItem(IDC_YEAR_ALIGN)->EnableWindow(true);
		GetDlgItem(IDC_YEAR_X)->EnableWindow(true);
		GetDlgItem(IDC_YEAR_Y)->EnableWindow(true);
		OnSelchangeYearRasterizer();
	} else {
		GetDlgItem(IDC_YEAR_RASTERIZER)->EnableWindow(false);
		GetDlgItem(IDC_YEAR_SELECT)->EnableWindow(false);
		GetDlgItem(IDC_YEAR_BROWSE)->EnableWindow(false);
		GetDlgItem(IDC_YEAR_ALIGN)->EnableWindow(false);
		GetDlgItem(IDC_YEAR_BITMAP)->EnableWindow(false);
		GetDlgItem(IDC_YEAR_FONTCOLOR)->EnableWindow(false);
		GetDlgItem(IDC_YEAR_X)->EnableWindow(false);
		GetDlgItem(IDC_YEAR_Y)->EnableWindow(false);
	}

	SetModified(TRUE);
}

void CDialogYear::OnSelchangeYearRasterizer() 
{
	UpdateData();

	if(m_Enable) {
		switch(m_Rasterizer.GetCurSel()) {
		case 1:
			GetDlgItem(IDC_YEAR_BROWSE)->EnableWindow(true);
			GetDlgItem(IDC_YEAR_BITMAP)->EnableWindow(true);
			GetDlgItem(IDC_YEAR_SELECT)->EnableWindow(false);
			GetDlgItem(IDC_YEAR_FONTCOLOR)->EnableWindow(false);
			break;
		default:
			GetDlgItem(IDC_YEAR_BROWSE)->EnableWindow(false);
			GetDlgItem(IDC_YEAR_BITMAP)->EnableWindow(false);
			GetDlgItem(IDC_YEAR_SELECT)->EnableWindow(true);
			GetDlgItem(IDC_YEAR_FONTCOLOR)->EnableWindow(true);
		}
	}

	SetModified(TRUE);
}

void CDialogYear::OnYearBrowse() 
{
	CFileDialog FileDialog(TRUE);
		
	if(IDOK==FileDialog.DoModal()) {
		m_Bitmap=FileDialog.GetPathName();
		GetDlgItem(IDC_YEAR_BITMAP)->SetWindowText(m_Bitmap);
		SetModified(TRUE);
	}
}

void CDialogYear::OnYearSelect() 
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

		GetDlgItem(IDC_YEAR_FONTNAME)->SetWindowText(m_Font);
		SetModified(TRUE);
	}
}

void CDialogYear::OnYearFontcolor() 
{
	CColorDialog ColorDialog(m_FontColor);

	if(IDOK==ColorDialog.DoModal()) {
		m_FontColor=ColorDialog.GetColor();
		GetDlgItem(IDC_YEAR_FONTCOLOR)->Invalidate();
		SetModified(TRUE);
	}
}

void CDialogYear::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	HBRUSH Brush=CreateSolidBrush(m_FontColor);
	FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, Brush);
	DeleteObject(Brush);
	
	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CDialogYear::OnSelchangeYearAlign() 
{
	SetModified(TRUE);
}

void CDialogYear::OnChangeYearBitmap() 
{
	SetModified(TRUE);
}

void CDialogYear::OnChangeYearX() 
{
	SetModified(TRUE);
}

void CDialogYear::OnChangeYearY() 
{
	SetModified(TRUE);
}
