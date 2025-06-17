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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/DialogDays.cpp,v 1.3 2002/02/27 18:55:52 rainy Exp $

  $Log: DialogDays.cpp,v $
  Revision 1.3  2002/02/27 18:55:52  rainy
  Added support for new aligns

  Revision 1.2  2001/12/23 10:00:18  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#include "stdafx.h"
#include "rainlendardll.h"
#include "CalendarWindow.h"
#include "DialogDays.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogDays property page

IMPLEMENT_DYNCREATE(CDialogDays, CPropertyPage)

CDialogDays::CDialogDays() : CPropertyPage(CDialogDays::IDD)
{
	//{{AFX_DATA_INIT(CDialogDays)
	m_Y = 0;
	m_X = 0;
	m_W = 0;
	m_H = 0;
	m_IgnoreToday = FALSE;
	m_Font = _T("");
	m_Enable = FALSE;
	m_Bitmap = _T("");
	m_IgnoreEvent = FALSE;
	//}}AFX_DATA_INIT
}

CDialogDays::~CDialogDays()
{
}

void CDialogDays::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDays)
	DDX_Control(pDX, IDC_DAYS_ALIGN, m_Align);
	DDX_Control(pDX, IDC_DAYS_RASTERIZER, m_Rasterizer);
	DDX_Text(pDX, IDC_DAYS_Y, m_Y);
	DDX_Text(pDX, IDC_DAYS_X, m_X);
	DDX_Text(pDX, IDC_DAYS_W, m_W);
	DDX_Text(pDX, IDC_DAYS_H, m_H);
	DDX_Check(pDX, IDC_DAYS_IGNORE_TODAY, m_IgnoreToday);
	DDX_Text(pDX, IDC_DAYS_FONTNAME, m_Font);
	DDX_Check(pDX, IDC_DAYS_ENABLE, m_Enable);
	DDX_Text(pDX, IDC_DAYS_BITMAP, m_Bitmap);
	DDX_Check(pDX, IDC_DAYS_IGNORE_EVENT, m_IgnoreEvent);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDays, CPropertyPage)
	//{{AFX_MSG_MAP(CDialogDays)
	ON_BN_CLICKED(IDC_DAYS_ENABLE, OnDaysEnable)
	ON_CBN_SELCHANGE(IDC_DAYS_RASTERIZER, OnSelchangeDaysRasterizer)
	ON_BN_CLICKED(IDC_DAYS_SELECT, OnDaysSelect)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_DAYS_FONTCOLOR, OnDaysFontcolor)
	ON_EN_CHANGE(IDC_DAYS_BITMAP, OnChangeDaysBitmap)
	ON_BN_CLICKED(IDC_DAYS_COMPONENTS_10, OnDaysComponents10)
	ON_BN_CLICKED(IDC_DAYS_COMPONENTS_32, OnDaysComponents32)
	ON_EN_CHANGE(IDC_DAYS_H, OnChangeDaysH)
	ON_BN_CLICKED(IDC_DAYS_IGNORE_TODAY, OnDaysIgnoreToday)
	ON_CBN_EDITCHANGE(IDC_DAYS_RASTERIZER, OnEditchangeDaysRasterizer)
	ON_EN_CHANGE(IDC_DAYS_W, OnChangeDaysW)
	ON_EN_CHANGE(IDC_DAYS_X, OnChangeDaysX)
	ON_EN_CHANGE(IDC_DAYS_Y, OnChangeDaysY)
	ON_CBN_SELCHANGE(IDC_DAYS_ALIGN, OnSelchangeDaysAlign)
	ON_BN_CLICKED(IDC_DAYS_BROWSE, OnDaysBrowse)
	ON_BN_CLICKED(IDC_DAYS_IGNORE_EVENT, OnDaysIgnoreEvent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDays message handlers

void CDialogDays::UpdateConfig()
{
	CCalendarWindow::c_Config.SetDaysEnable(m_Enable==TRUE);
	CCalendarWindow::c_Config.SetDaysX(m_X);
	CCalendarWindow::c_Config.SetDaysY(m_Y);
	CCalendarWindow::c_Config.SetDaysW(m_W);
	CCalendarWindow::c_Config.SetDaysH(m_H);
	CCalendarWindow::c_Config.SetDaysBitmapName(m_Bitmap);
	CCalendarWindow::c_Config.SetDaysIgnoreToday(m_IgnoreToday==TRUE);
	CCalendarWindow::c_Config.SetDaysIgnoreEvent(m_IgnoreEvent==TRUE);
	CCalendarWindow::c_Config.SetDaysFontColor(m_FontColor);
	CCalendarWindow::c_Config.SetDaysFont(m_RealFont);

	switch(m_Rasterizer.GetCurSel()) {
	case 1:
		CCalendarWindow::c_Config.SetDaysRasterizer(CRasterizer::TYPE_BITMAP);
		break;
	default:
		CCalendarWindow::c_Config.SetDaysRasterizer(CRasterizer::TYPE_FONT);
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
	CCalendarWindow::c_Config.SetDaysAlign((CRasterizer::ALIGN)align);

	switch(GetCheckedRadioButton(IDC_DAYS_COMPONENTS_10, IDC_DAYS_COMPONENTS_32)) {
	case IDC_DAYS_COMPONENTS_32:
		CCalendarWindow::c_Config.SetDaysNumOfComponents(32);
		break;
	default:
		CCalendarWindow::c_Config.SetDaysNumOfComponents(10);
	}
}

BOOL CDialogDays::OnApply() 
{
	UpdateConfig();

	// Fetch the CCalendarWindow
	CWnd* Parent=GetParent();
	Parent=Parent->GetParent();

	Parent->SendMessage(WM_COMMAND, ID_REFRESH, 0);
	
	return TRUE;
}

void CDialogDays::OnOK() 
{
	UpdateConfig();
	
	CPropertyPage::OnOK();
}

void CDialogDays::OnDaysEnable() 
{
	UpdateData();
	
	if(m_Enable) {
		GetDlgItem(IDC_DAYS_X)->EnableWindow(true);
		GetDlgItem(IDC_DAYS_Y)->EnableWindow(true);
		GetDlgItem(IDC_DAYS_W)->EnableWindow(true);
		GetDlgItem(IDC_DAYS_H)->EnableWindow(true);
		GetDlgItem(IDC_DAYS_RASTERIZER)->EnableWindow(true);
		GetDlgItem(IDC_DAYS_ALIGN)->EnableWindow(true);
		GetDlgItem(IDC_DAYS_IGNORE_TODAY)->EnableWindow(true);
		GetDlgItem(IDC_DAYS_IGNORE_EVENT)->EnableWindow(true);
		OnSelchangeDaysRasterizer();
	} else {
		GetDlgItem(IDC_DAYS_X)->EnableWindow(false);
		GetDlgItem(IDC_DAYS_Y)->EnableWindow(false);
		GetDlgItem(IDC_DAYS_W)->EnableWindow(false);
		GetDlgItem(IDC_DAYS_H)->EnableWindow(false);
		GetDlgItem(IDC_DAYS_RASTERIZER)->EnableWindow(false);
		GetDlgItem(IDC_DAYS_SELECT)->EnableWindow(false);
		GetDlgItem(IDC_DAYS_BROWSE)->EnableWindow(false);
		GetDlgItem(IDC_DAYS_COMPONENTS_10)->EnableWindow(false);
		GetDlgItem(IDC_DAYS_COMPONENTS_32)->EnableWindow(false);
		GetDlgItem(IDC_DAYS_ALIGN)->EnableWindow(false);
		GetDlgItem(IDC_DAYS_IGNORE_TODAY)->EnableWindow(false);
		GetDlgItem(IDC_DAYS_IGNORE_EVENT)->EnableWindow(false);
		GetDlgItem(IDC_DAYS_BITMAP)->EnableWindow(false);
		GetDlgItem(IDC_DAYS_FONTCOLOR)->EnableWindow(false);
	}

	SetModified(TRUE);
}

BOOL CDialogDays::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_Enable=CCalendarWindow::c_Config.GetDaysEnable();
	m_X=CCalendarWindow::c_Config.GetDaysX();
	m_Y=CCalendarWindow::c_Config.GetDaysY();
	m_W=CCalendarWindow::c_Config.GetDaysW();
	m_H=CCalendarWindow::c_Config.GetDaysH();
	m_Bitmap=CCalendarWindow::c_Config.GetDaysBitmapName();
	m_IgnoreToday=CCalendarWindow::c_Config.GetDaysIgnoreToday();
	m_IgnoreEvent=CCalendarWindow::c_Config.GetDaysIgnoreEvent();

	switch(CCalendarWindow::c_Config.GetDaysRasterizer()) {
	case CRasterizer::TYPE_BITMAP:
		m_Rasterizer.SetCurSel(1);
		break;
	case CRasterizer::TYPE_FONT:
	default:
		m_Rasterizer.SetCurSel(0);
	}
	
	switch(CCalendarWindow::c_Config.GetDaysAlign()) 
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

	switch(CCalendarWindow::c_Config.GetDaysNumOfComponents()) {
	case 32:
		CheckRadioButton(IDC_DAYS_COMPONENTS_10, IDC_DAYS_COMPONENTS_32, IDC_DAYS_COMPONENTS_32);
		break;
	default:
		CheckRadioButton(IDC_DAYS_COMPONENTS_10, IDC_DAYS_COMPONENTS_32, IDC_DAYS_COMPONENTS_10);
	}

	m_RealFont=CCalendarWindow::c_Config.GetDaysFont();
	m_Font=m_RealFont.Mid(m_RealFont.ReverseFind('/')+1);

	m_FontColor=CCalendarWindow::c_Config.GetDaysFontColor();

	UpdateData(FALSE);

	OnDaysEnable();
	OnSelchangeDaysRasterizer();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogDays::OnSelchangeDaysRasterizer() 
{
	UpdateData();

	if(m_Enable) {
		switch(m_Rasterizer.GetCurSel()) {
		case 1:
			GetDlgItem(IDC_DAYS_BROWSE)->EnableWindow(true);
			GetDlgItem(IDC_DAYS_COMPONENTS_10)->EnableWindow(true);
			GetDlgItem(IDC_DAYS_COMPONENTS_32)->EnableWindow(true);
			GetDlgItem(IDC_DAYS_BITMAP)->EnableWindow(true);
			GetDlgItem(IDC_DAYS_SELECT)->EnableWindow(false);
			GetDlgItem(IDC_DAYS_FONTCOLOR)->EnableWindow(false);
			break;
		default:
			GetDlgItem(IDC_DAYS_BROWSE)->EnableWindow(false);
			GetDlgItem(IDC_DAYS_COMPONENTS_10)->EnableWindow(false);
			GetDlgItem(IDC_DAYS_COMPONENTS_32)->EnableWindow(false);
			GetDlgItem(IDC_DAYS_BITMAP)->EnableWindow(false);
			GetDlgItem(IDC_DAYS_SELECT)->EnableWindow(true);
			GetDlgItem(IDC_DAYS_FONTCOLOR)->EnableWindow(true);
		}
	}

	SetModified(TRUE);
}

void CDialogDays::OnDaysSelect() 
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

		GetDlgItem(IDC_DAYS_FONTNAME)->SetWindowText(m_Font);
		SetModified(TRUE);
	}

}

void CDialogDays::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	HBRUSH Brush=CreateSolidBrush(m_FontColor);
	FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, Brush);
	DeleteObject(Brush);

	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct); 
}

void CDialogDays::OnDaysFontcolor() 
{
	CColorDialog ColorDialog(m_FontColor);

	if(IDOK==ColorDialog.DoModal()) {
		m_FontColor=ColorDialog.GetColor();
		GetDlgItem(IDC_DAYS_FONTCOLOR)->Invalidate();
		SetModified(TRUE);
	}
}

void CDialogDays::OnDaysBrowse() 
{
	CFileDialog FileDialog(TRUE);
		
	if(IDOK==FileDialog.DoModal()) {
		m_Bitmap=FileDialog.GetPathName();
		GetDlgItem(IDC_DAYS_BITMAP)->SetWindowText(m_Bitmap);
		SetModified(TRUE);
	}
}

void CDialogDays::OnChangeDaysBitmap() 
{
	SetModified(TRUE);
}

void CDialogDays::OnDaysComponents10() 
{
	SetModified(TRUE);
}

void CDialogDays::OnDaysComponents32() 
{
	SetModified(TRUE);
}

void CDialogDays::OnChangeDaysH() 
{
	SetModified(TRUE);
}

void CDialogDays::OnDaysIgnoreToday() 
{
	SetModified(TRUE);
}

void CDialogDays::OnEditchangeDaysRasterizer() 
{
	SetModified(TRUE);
}

void CDialogDays::OnChangeDaysW() 
{
	SetModified(TRUE);
}

void CDialogDays::OnChangeDaysX() 
{
	SetModified(TRUE);
}

void CDialogDays::OnChangeDaysY() 
{
	SetModified(TRUE);
}

void CDialogDays::OnSelchangeDaysAlign() 
{
	SetModified(TRUE);
}

void CDialogDays::OnDaysIgnoreEvent() 
{
	SetModified(TRUE);
}
