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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/DialogYear.cpp,v 1.3 2002/02/27 18:55:52 rainy Exp $

  $Log: DialogYear.cpp,v $
  Revision 1.3  2002/02/27 18:55:52  rainy
  Added support for new aligns

  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

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
	CCalendarWindow::c_Config.SetYearEnable(m_Enable==TRUE);
	CCalendarWindow::c_Config.SetYearBitmapName(m_Bitmap);
	CCalendarWindow::c_Config.SetYearFontColor(m_FontColor);
	CCalendarWindow::c_Config.SetYearFont(m_RealFont);
	CCalendarWindow::c_Config.SetYearX(m_X);
	CCalendarWindow::c_Config.SetYearY(m_Y);

	switch(m_Rasterizer.GetCurSel()) {
	case 1:
		CCalendarWindow::c_Config.SetYearRasterizer(CRasterizer::TYPE_BITMAP);
		break;
	default:
		CCalendarWindow::c_Config.SetYearRasterizer(CRasterizer::TYPE_FONT);
	}

	int align = 0;
	switch(m_Align.GetCurSel()) 
	{
	case 0:
		align = CRasterizer::ALIGN_TOP | CRasterizer::ALIGN_LEFT;
		break;
	case 1:
		align = CRasterizer::ALIGN_TOP | CRasterizer::ALIGN_HCENTER;
		break;
	case 2:
		align = CRasterizer::ALIGN_TOP | CRasterizer::ALIGN_RIGHT;
		break;
	case 3:
		align = CRasterizer::ALIGN_VCENTER | CRasterizer::ALIGN_LEFT;
		break;
	case 4:
		align = CRasterizer::ALIGN_VCENTER | CRasterizer::ALIGN_HCENTER;
		break;
	case 5:
		align = CRasterizer::ALIGN_VCENTER | CRasterizer::ALIGN_RIGHT;
		break;
	case 6:
		align = CRasterizer::ALIGN_BOTTOM | CRasterizer::ALIGN_LEFT;
		break;
	case 7:
		align = CRasterizer::ALIGN_BOTTOM | CRasterizer::ALIGN_HCENTER;
		break;
	case 8:
		align = CRasterizer::ALIGN_BOTTOM | CRasterizer::ALIGN_RIGHT;
		break;
	default:
		align = CRasterizer::ALIGN_VCENTER | CRasterizer::ALIGN_LEFT;
	}
	CCalendarWindow::c_Config.SetYearAlign((CRasterizer::ALIGN)align);
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
	
	m_Enable=CCalendarWindow::c_Config.GetYearEnable();
	m_Bitmap=CCalendarWindow::c_Config.GetYearBitmapName();
	m_X=CCalendarWindow::c_Config.GetYearX();
	m_Y=CCalendarWindow::c_Config.GetYearY();

	switch(CCalendarWindow::c_Config.GetYearRasterizer()) {
	case CRasterizer::TYPE_BITMAP:
		m_Rasterizer.SetCurSel(1);
		break;
	case CRasterizer::TYPE_FONT:
	default:
		m_Rasterizer.SetCurSel(0);
	}
	
	switch(CCalendarWindow::c_Config.GetYearAlign()) 
	{
	case (CRasterizer::ALIGN_TOP | CRasterizer::ALIGN_LEFT):
		m_Align.SetCurSel(0);
		break;

	case (CRasterizer::ALIGN_TOP | CRasterizer::ALIGN_HCENTER):
		m_Align.SetCurSel(1);
		break;

	case (CRasterizer::ALIGN_TOP | CRasterizer::ALIGN_RIGHT):
		m_Align.SetCurSel(2);
		break;

	case (CRasterizer::ALIGN_VCENTER | CRasterizer::ALIGN_LEFT):
		m_Align.SetCurSel(3);
		break;

	case (CRasterizer::ALIGN_VCENTER | CRasterizer::ALIGN_HCENTER):
		m_Align.SetCurSel(4);
		break;

	case (CRasterizer::ALIGN_VCENTER | CRasterizer::ALIGN_RIGHT):
		m_Align.SetCurSel(5);
		break;

	case (CRasterizer::ALIGN_BOTTOM | CRasterizer::ALIGN_LEFT):
		m_Align.SetCurSel(6);
		break;

	case (CRasterizer::ALIGN_BOTTOM | CRasterizer::ALIGN_HCENTER):
		m_Align.SetCurSel(7);
		break;

	case (CRasterizer::ALIGN_BOTTOM | CRasterizer::ALIGN_RIGHT):
		m_Align.SetCurSel(8);
		break;

	default:
		m_Align.SetCurSel(3);
	}

	m_RealFont=CCalendarWindow::c_Config.GetYearFont();
	m_Font=m_RealFont.Mid(m_RealFont.ReverseFind('/')+1);

	m_FontColor=CCalendarWindow::c_Config.GetYearFontColor();

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
