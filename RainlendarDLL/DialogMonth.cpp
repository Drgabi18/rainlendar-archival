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

// DialogMonth.cpp : implementation file
//

#include "stdafx.h"
#include "rainlendardll.h"
#include "CalendarWindow.h"
#include "DialogMonth.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogMonth property page

IMPLEMENT_DYNCREATE(CDialogMonth, CPropertyPage)

CDialogMonth::CDialogMonth() : CPropertyPage(CDialogMonth::IDD)
{
	//{{AFX_DATA_INIT(CDialogMonth)
	m_Bitmap = _T("");
	m_Enable = FALSE;
	m_Font = _T("");
	m_X = 0;
	m_Y = 0;
	//}}AFX_DATA_INIT
}

CDialogMonth::~CDialogMonth()
{
}

void CDialogMonth::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogMonth)
	DDX_Control(pDX, IDC_MONTH_ALIGN, m_Align);
	DDX_Control(pDX, IDC_MONTH_RASTERIZER, m_Rasterizer);
	DDX_Text(pDX, IDC_MONTH_BITMAP, m_Bitmap);
	DDX_Check(pDX, IDC_MONTH_ENABLE, m_Enable);
	DDX_Text(pDX, IDC_MONTH_FONTNAME, m_Font);
	DDX_Text(pDX, IDC_MONTH_X, m_X);
	DDX_Text(pDX, IDC_MONTH_Y, m_Y);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogMonth, CPropertyPage)
	//{{AFX_MSG_MAP(CDialogMonth)
	ON_BN_CLICKED(IDC_MONTH_ENABLE, OnMonthEnable)
	ON_CBN_SELCHANGE(IDC_MONTH_RASTERIZER, OnSelchangeMonthRasterizer)
	ON_BN_CLICKED(IDC_MONTH_BROWSE, OnMonthBrowse)
	ON_BN_CLICKED(IDC_MONTH_SELECT, OnMonthSelect)
	ON_BN_CLICKED(IDC_MONTH_FONTCOLOR, OnMonthFontcolor)
	ON_WM_DRAWITEM()
	ON_CBN_SELCHANGE(IDC_MONTH_ALIGN, OnSelchangeMonthAlign)
	ON_EN_CHANGE(IDC_MONTH_BITMAP, OnChangeMonthBitmap)
	ON_EN_CHANGE(IDC_MONTH_X, OnChangeMonthX)
	ON_EN_CHANGE(IDC_MONTH_Y, OnChangeMonthY)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogMonth message handlers

void CDialogMonth::UpdateConfig()
{
	CCalendarWindow::C_Config.SetMonthEnable(m_Enable==TRUE);
	CCalendarWindow::C_Config.SetMonthBitmapName(m_Bitmap);
	CCalendarWindow::C_Config.SetMonthFontColor(m_FontColor);
	CCalendarWindow::C_Config.SetMonthFont(m_RealFont);
	CCalendarWindow::C_Config.SetMonthX(m_X);
	CCalendarWindow::C_Config.SetMonthY(m_Y);

	switch(m_Rasterizer.GetCurSel()) {
	case 1:
		CCalendarWindow::C_Config.SetMonthRasterizer(CRasterizer::TYPE_BITMAP);
		break;
	default:
		CCalendarWindow::C_Config.SetMonthRasterizer(CRasterizer::TYPE_FONT);
	}

	switch(m_Align.GetCurSel()) {
	case 1:
		CCalendarWindow::C_Config.SetMonthAlign(CRasterizer::ALIGN_CENTER);
		break;
	case 2:
		CCalendarWindow::C_Config.SetMonthAlign(CRasterizer::ALIGN_RIGHT);
		break;
	default:
		CCalendarWindow::C_Config.SetMonthAlign(CRasterizer::ALIGN_LEFT);
	}

}


BOOL CDialogMonth::OnApply() 
{
	UpdateConfig();

	// Fetch the CCalendarWindow
	CWnd* Parent=GetParent();
	Parent=Parent->GetParent();

	Parent->SendMessage(WM_COMMAND, ID_REFRESH, 0);
	
	return TRUE;
}

void CDialogMonth::OnOK() 
{
	UpdateConfig();
	
	CPropertyPage::OnOK();
}

BOOL CDialogMonth::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_Enable=CCalendarWindow::C_Config.GetMonthEnable();
	m_Bitmap=CCalendarWindow::C_Config.GetMonthBitmapName();
	m_X=CCalendarWindow::C_Config.GetMonthX();
	m_Y=CCalendarWindow::C_Config.GetMonthY();

	switch(CCalendarWindow::C_Config.GetMonthRasterizer()) {
	case CRasterizer::TYPE_BITMAP:
		m_Rasterizer.SetCurSel(1);
		break;
	case CRasterizer::TYPE_FONT:
	default:
		m_Rasterizer.SetCurSel(0);
	}
	
	switch(CCalendarWindow::C_Config.GetMonthAlign()) {
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

	m_RealFont=CCalendarWindow::C_Config.GetMonthFont();
	m_Font=m_RealFont.Mid(m_RealFont.ReverseFind('/')+1);

	m_FontColor=CCalendarWindow::C_Config.GetMonthFontColor();

	UpdateData(FALSE);

	OnMonthEnable();
	OnSelchangeMonthRasterizer();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogMonth::OnMonthEnable() 
{
	UpdateData();
	
	if(m_Enable) {
		GetDlgItem(IDC_MONTH_RASTERIZER)->EnableWindow(true);
		GetDlgItem(IDC_MONTH_ALIGN)->EnableWindow(true);
		GetDlgItem(IDC_MONTH_X)->EnableWindow(true);
		GetDlgItem(IDC_MONTH_Y)->EnableWindow(true);
		OnSelchangeMonthRasterizer();
	} else {
		GetDlgItem(IDC_MONTH_RASTERIZER)->EnableWindow(false);
		GetDlgItem(IDC_MONTH_SELECT)->EnableWindow(false);
		GetDlgItem(IDC_MONTH_BROWSE)->EnableWindow(false);
		GetDlgItem(IDC_MONTH_ALIGN)->EnableWindow(false);
		GetDlgItem(IDC_MONTH_BITMAP)->EnableWindow(false);
		GetDlgItem(IDC_MONTH_FONTCOLOR)->EnableWindow(false);
		GetDlgItem(IDC_MONTH_X)->EnableWindow(false);
		GetDlgItem(IDC_MONTH_Y)->EnableWindow(false);
	}

	SetModified(TRUE);
}

void CDialogMonth::OnSelchangeMonthRasterizer() 
{
	UpdateData();

	if(m_Enable) {
		switch(m_Rasterizer.GetCurSel()) {
		case 1:
			GetDlgItem(IDC_MONTH_BROWSE)->EnableWindow(true);
			GetDlgItem(IDC_MONTH_BITMAP)->EnableWindow(true);
			GetDlgItem(IDC_MONTH_SELECT)->EnableWindow(false);
			GetDlgItem(IDC_MONTH_FONTCOLOR)->EnableWindow(false);
			break;
		default:
			GetDlgItem(IDC_MONTH_BROWSE)->EnableWindow(false);
			GetDlgItem(IDC_MONTH_BITMAP)->EnableWindow(false);
			GetDlgItem(IDC_MONTH_SELECT)->EnableWindow(true);
			GetDlgItem(IDC_MONTH_FONTCOLOR)->EnableWindow(true);
		}
	}

	SetModified(TRUE);
}

void CDialogMonth::OnMonthBrowse() 
{
	CFileDialog FileDialog(TRUE);
		
	if(IDOK==FileDialog.DoModal()) {
		m_Bitmap=FileDialog.GetPathName();
		GetDlgItem(IDC_MONTH_BITMAP)->SetWindowText(m_Bitmap);
		SetModified(TRUE);
	}
}

void CDialogMonth::OnMonthSelect() 
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

		GetDlgItem(IDC_MONTH_FONTNAME)->SetWindowText(m_Font);
		SetModified(TRUE);
	}
}

void CDialogMonth::OnMonthFontcolor() 
{
	CColorDialog ColorDialog(m_FontColor);

	if(IDOK==ColorDialog.DoModal()) {
		m_FontColor=ColorDialog.GetColor();
		GetDlgItem(IDC_MONTH_FONTCOLOR)->Invalidate();
		SetModified(TRUE);
	}
}

void CDialogMonth::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	HBRUSH Brush=CreateSolidBrush(m_FontColor);
	FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, Brush);
	DeleteObject(Brush);
	
	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CDialogMonth::OnSelchangeMonthAlign() 
{
	SetModified(TRUE);
}

void CDialogMonth::OnChangeMonthBitmap() 
{
	SetModified(TRUE);
}

void CDialogMonth::OnChangeMonthX() 
{
	SetModified(TRUE);
}

void CDialogMonth::OnChangeMonthY() 
{
	SetModified(TRUE);
}
