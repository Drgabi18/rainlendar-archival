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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/DialogWeekdays.cpp,v 1.3 2002/02/27 18:55:52 rainy Exp $

  $Log: DialogWeekdays.cpp,v $
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
#include "DialogWeekdays.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogWeekdays property page

IMPLEMENT_DYNCREATE(CDialogWeekdays, CPropertyPage)

CDialogWeekdays::CDialogWeekdays() : CPropertyPage(CDialogWeekdays::IDD)
{
	//{{AFX_DATA_INIT(CDialogWeekdays)
	m_Enable = FALSE;
	m_Bitmap = _T("");
	m_Font = _T("");
	//}}AFX_DATA_INIT
}

CDialogWeekdays::~CDialogWeekdays()
{
}

void CDialogWeekdays::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogWeekdays)
	DDX_Control(pDX, IDC_WEEKDAYS_RASTERIZER, m_Rasterizer);
	DDX_Control(pDX, IDC_WEEKDAYS_ALIGN, m_Align);
	DDX_Check(pDX, IDC_WEEKDAYS_ENABLE, m_Enable);
	DDX_Text(pDX, IDC_WEEKDAYS_BITMAP, m_Bitmap);
	DDX_Text(pDX, IDC_WEEKDAYS_FONTNAME, m_Font);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogWeekdays, CPropertyPage)
	//{{AFX_MSG_MAP(CDialogWeekdays)
	ON_BN_CLICKED(IDC_WEEKDAYS_ENABLE, OnWeekdaysEnable)
	ON_CBN_SELCHANGE(IDC_WEEKDAYS_RASTERIZER, OnSelchangeWeekdaysRasterizer)
	ON_BN_CLICKED(IDC_WEEKDAYS_BROWSE, OnWeekdaysBrowse)
	ON_BN_CLICKED(IDC_WEEKDAYS_SELECT, OnWeekdaysSelect)
	ON_BN_CLICKED(IDC_WEEKDAYS_FONTCOLOR, OnWeekdaysFontcolor)
	ON_WM_DRAWITEM()
	ON_CBN_SELCHANGE(IDC_WEEKDAYS_ALIGN, OnSelchangeWeekdaysAlign)
	ON_EN_CHANGE(IDC_WEEKDAYS_BITMAP, OnChangeWeekdaysBitmap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogWeekdays message handlers

void CDialogWeekdays::UpdateConfig()
{
	CCalendarWindow::c_Config.SetWeekdaysEnable(m_Enable==TRUE);
	CCalendarWindow::c_Config.SetWeekdaysBitmapName(m_Bitmap);
	CCalendarWindow::c_Config.SetWeekdaysFontColor(m_FontColor);
	CCalendarWindow::c_Config.SetWeekdaysFont(m_RealFont);

	switch(m_Rasterizer.GetCurSel()) {
	case 1:
		CCalendarWindow::c_Config.SetWeekdaysRasterizer(CRasterizer::TYPE_BITMAP);
		break;
	default:
		CCalendarWindow::c_Config.SetWeekdaysRasterizer(CRasterizer::TYPE_FONT);
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
	CCalendarWindow::c_Config.SetWeekdaysAlign((CRasterizer::ALIGN)align);


}

BOOL CDialogWeekdays::OnApply() 
{
	UpdateConfig();

	// Fetch the CCalendarWindow
	CWnd* Parent=GetParent();
	Parent=Parent->GetParent();

	Parent->SendMessage(WM_COMMAND, ID_REFRESH, 0);
	
	return TRUE;
}

void CDialogWeekdays::OnOK() 
{
	UpdateConfig();
	
	CPropertyPage::OnOK();
}

BOOL CDialogWeekdays::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_Enable=CCalendarWindow::c_Config.GetWeekdaysEnable();
	m_Bitmap=CCalendarWindow::c_Config.GetWeekdaysBitmapName();

	switch(CCalendarWindow::c_Config.GetWeekdaysRasterizer()) {
	case CRasterizer::TYPE_BITMAP:
		m_Rasterizer.SetCurSel(1);
		break;
	case CRasterizer::TYPE_FONT:
	default:
		m_Rasterizer.SetCurSel(0);
	}
	
	switch(CCalendarWindow::c_Config.GetWeekdaysAlign()) 
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


	m_RealFont=CCalendarWindow::c_Config.GetWeekdaysFont();
	m_Font=m_RealFont.Mid(m_RealFont.ReverseFind('/')+1);

	m_FontColor=CCalendarWindow::c_Config.GetWeekdaysFontColor();

	UpdateData(FALSE);

	OnWeekdaysEnable();
	OnSelchangeWeekdaysRasterizer();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogWeekdays::OnWeekdaysEnable() 
{
	UpdateData();
	
	if(m_Enable) {
		GetDlgItem(IDC_WEEKDAYS_RASTERIZER)->EnableWindow(true);
		GetDlgItem(IDC_WEEKDAYS_ALIGN)->EnableWindow(true);
		OnSelchangeWeekdaysRasterizer();
	} else {
		GetDlgItem(IDC_WEEKDAYS_RASTERIZER)->EnableWindow(false);
		GetDlgItem(IDC_WEEKDAYS_SELECT)->EnableWindow(false);
		GetDlgItem(IDC_WEEKDAYS_BROWSE)->EnableWindow(false);
		GetDlgItem(IDC_WEEKDAYS_ALIGN)->EnableWindow(false);
		GetDlgItem(IDC_WEEKDAYS_BITMAP)->EnableWindow(false);
		GetDlgItem(IDC_WEEKDAYS_FONTCOLOR)->EnableWindow(false);
	}

	SetModified(TRUE);
}

void CDialogWeekdays::OnSelchangeWeekdaysRasterizer() 
{
	UpdateData();

	if(m_Enable) {
		switch(m_Rasterizer.GetCurSel()) {
		case 1:
			GetDlgItem(IDC_WEEKDAYS_BROWSE)->EnableWindow(true);
			GetDlgItem(IDC_WEEKDAYS_BITMAP)->EnableWindow(true);
			GetDlgItem(IDC_WEEKDAYS_SELECT)->EnableWindow(false);
			GetDlgItem(IDC_WEEKDAYS_FONTCOLOR)->EnableWindow(false);
			break;
		default:
			GetDlgItem(IDC_WEEKDAYS_BROWSE)->EnableWindow(false);
			GetDlgItem(IDC_WEEKDAYS_BITMAP)->EnableWindow(false);
			GetDlgItem(IDC_WEEKDAYS_SELECT)->EnableWindow(true);
			GetDlgItem(IDC_WEEKDAYS_FONTCOLOR)->EnableWindow(true);
		}
	}

	SetModified(TRUE);
}

void CDialogWeekdays::OnWeekdaysBrowse() 
{
	CFileDialog FileDialog(TRUE);
		
	if(IDOK==FileDialog.DoModal()) {
		m_Bitmap=FileDialog.GetPathName();
		UpdateData(FALSE);
		GetDlgItem(IDC_WEEKDAYS_BITMAP)->SetWindowText(m_Bitmap);
	}
}

void CDialogWeekdays::OnWeekdaysSelect() 
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

		GetDlgItem(IDC_WEEKDAYS_FONTNAME)->SetWindowText(m_Font);
		SetModified(TRUE);
	}
}

void CDialogWeekdays::OnWeekdaysFontcolor() 
{
	CColorDialog ColorDialog(m_FontColor);

	if(IDOK==ColorDialog.DoModal()) {
		m_FontColor=ColorDialog.GetColor();
		GetDlgItem(IDC_WEEKDAYS_FONTCOLOR)->Invalidate();
		SetModified(TRUE);
	}
}

void CDialogWeekdays::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	HBRUSH Brush=CreateSolidBrush(m_FontColor);
	FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, Brush);
	DeleteObject(Brush);
	
	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CDialogWeekdays::OnSelchangeWeekdaysAlign() 
{
	SetModified(TRUE);
}

void CDialogWeekdays::OnChangeWeekdaysBitmap() 
{
	SetModified(TRUE);
}

