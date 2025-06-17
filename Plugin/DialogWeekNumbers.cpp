// DialogWeekNumbers.cpp : implementation file
//

#include "stdafx.h"
#include "rainlendardll.h"
#include "DialogWeekNumbers.h"
#include "CalendarWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogWeekNumbers property page

IMPLEMENT_DYNCREATE(CDialogWeekNumbers, CPropertyPage)

CDialogWeekNumbers::CDialogWeekNumbers() : CPropertyPage(CDialogWeekNumbers::IDD)
{
	//{{AFX_DATA_INIT(CDialogWeekNumbers)
	m_Bitmap = _T("");
	m_Enable = FALSE;
	m_Font = _T("");
	//}}AFX_DATA_INIT
}

CDialogWeekNumbers::~CDialogWeekNumbers()
{
}

void CDialogWeekNumbers::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogWeekNumbers)
	DDX_Control(pDX, IDC_WEEKNUM_RASTERIZER, m_Rasterizer);
	DDX_Control(pDX, IDC_WEEKNUM_ALIGN, m_Align);
	DDX_Text(pDX, IDC_WEEKNUM_BITMAP, m_Bitmap);
	DDX_Check(pDX, IDC_WEEKNUM_ENABLE, m_Enable);
	DDX_Text(pDX, IDC_WEEKNUM_FONTNAME, m_Font);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogWeekNumbers, CPropertyPage)
	//{{AFX_MSG_MAP(CDialogWeekNumbers)
	ON_EN_CHANGE(IDC_WEEKNUM_BITMAP, OnChangeWeeknumBitmap)
	ON_WM_DRAWITEM()
	ON_CBN_SELCHANGE(IDC_WEEKNUM_ALIGN, OnSelchangeWeeknumAlign)
	ON_CBN_SELCHANGE(IDC_WEEKNUM_RASTERIZER, OnSelchangeWeeknumRasterizer)
	ON_BN_CLICKED(IDC_WEEKNUM_BROWSE, OnWeeknumBrowse)
	ON_BN_CLICKED(IDC_WEEKNUM_ENABLE, OnWeeknumEnable)
	ON_BN_CLICKED(IDC_WEEKNUM_FONTCOLOR, OnWeeknumFontcolor)
	ON_BN_CLICKED(IDC_WEEKNUM_SELECT, OnWeeknumSelect)
	ON_BN_CLICKED(IDC_WEEKNUM_COMPONENTS_54, OnWeeknumComponents54)
	ON_BN_CLICKED(IDC_WEEKNUM_COMPONENTS_10, OnWeeknumComponents10)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogWeekNumbers message handlers

void CDialogWeekNumbers::UpdateConfig()
{
	CCalendarWindow::c_Config.SetWeekNumbersEnable(m_Enable==TRUE);
	CCalendarWindow::c_Config.SetWeekNumbersBitmapName(m_Bitmap);
	CCalendarWindow::c_Config.SetWeekNumbersFontColor(m_FontColor);
	CCalendarWindow::c_Config.SetWeekNumbersFont(m_RealFont);

	switch(m_Rasterizer.GetCurSel()) {
	case 1:
		CCalendarWindow::c_Config.SetWeekNumbersRasterizer(CRasterizer::TYPE_BITMAP);
		break;
	default:
		CCalendarWindow::c_Config.SetWeekNumbersRasterizer(CRasterizer::TYPE_FONT);
	}

	switch(m_Align.GetCurSel()) {
	case 1:
		CCalendarWindow::c_Config.SetWeekNumbersAlign(CRasterizer::ALIGN_CENTER);
		break;
	case 2:
		CCalendarWindow::c_Config.SetWeekNumbersAlign(CRasterizer::ALIGN_BOTTOM);
		break;
	default:
		CCalendarWindow::c_Config.SetWeekNumbersAlign(CRasterizer::ALIGN_TOP);
	}

	switch(GetCheckedRadioButton(IDC_WEEKNUM_COMPONENTS_10, IDC_WEEKNUM_COMPONENTS_54)) {
	case IDC_WEEKNUM_COMPONENTS_54:
		CCalendarWindow::c_Config.SetWeekNumbersNumOfComponents(54);
		break;
	default:
		CCalendarWindow::c_Config.SetWeekNumbersNumOfComponents(10);
	}
}

BOOL CDialogWeekNumbers::OnApply() 
{
	UpdateConfig();

	// Fetch the CCalendarWindow
	CWnd* Parent=GetParent();
	Parent=Parent->GetParent();

	Parent->SendMessage(WM_COMMAND, ID_REFRESH, 0);
	
	return TRUE;
}

void CDialogWeekNumbers::OnOK() 
{
	UpdateConfig();
	
	CPropertyPage::OnOK();
}

BOOL CDialogWeekNumbers::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_Enable=CCalendarWindow::c_Config.GetWeekNumbersEnable();
	m_Bitmap=CCalendarWindow::c_Config.GetWeekNumbersBitmapName();

	switch(CCalendarWindow::c_Config.GetWeekNumbersRasterizer()) {
	case CRasterizer::TYPE_BITMAP:
		m_Rasterizer.SetCurSel(1);
		break;
	case CRasterizer::TYPE_FONT:
	default:
		m_Rasterizer.SetCurSel(0);
	}
	
	switch(CCalendarWindow::c_Config.GetWeekNumbersAlign()) {
	case CRasterizer::ALIGN_TOP:
		m_Align.SetCurSel(0);
		break;
	case CRasterizer::ALIGN_CENTER:
		m_Align.SetCurSel(1);
		break;
	case CRasterizer::ALIGN_BOTTOM:
	default:
		m_Align.SetCurSel(2);
	}

	switch(CCalendarWindow::c_Config.GetWeekNumbersNumOfComponents()) {
	case 54:
		CheckRadioButton(IDC_WEEKNUM_COMPONENTS_10, IDC_WEEKNUM_COMPONENTS_54, IDC_WEEKNUM_COMPONENTS_54);
		break;
	default:
		CheckRadioButton(IDC_WEEKNUM_COMPONENTS_10, IDC_WEEKNUM_COMPONENTS_54, IDC_WEEKNUM_COMPONENTS_10);
	}

	m_RealFont=CCalendarWindow::c_Config.GetWeekNumbersFont();
	m_Font=m_RealFont.Mid(m_RealFont.ReverseFind('/')+1);

	m_FontColor=CCalendarWindow::c_Config.GetWeekNumbersFontColor();

	UpdateData(FALSE);

	OnWeeknumEnable();
	OnSelchangeWeeknumRasterizer();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogWeekNumbers::OnWeeknumEnable() 
{
	UpdateData();
	
	if(m_Enable) {
		GetDlgItem(IDC_WEEKNUM_RASTERIZER)->EnableWindow(true);
		GetDlgItem(IDC_WEEKNUM_ALIGN)->EnableWindow(true);
		OnSelchangeWeeknumRasterizer();
	} else {
		GetDlgItem(IDC_WEEKNUM_RASTERIZER)->EnableWindow(false);
		GetDlgItem(IDC_WEEKNUM_SELECT)->EnableWindow(false);
		GetDlgItem(IDC_WEEKNUM_BROWSE)->EnableWindow(false);
		GetDlgItem(IDC_WEEKNUM_ALIGN)->EnableWindow(false);
		GetDlgItem(IDC_WEEKNUM_BITMAP)->EnableWindow(false);
		GetDlgItem(IDC_WEEKNUM_FONTCOLOR)->EnableWindow(false);
		GetDlgItem(IDC_WEEKNUM_COMPONENTS_10)->EnableWindow(false);
		GetDlgItem(IDC_WEEKNUM_COMPONENTS_54)->EnableWindow(false);
	}

	SetModified(TRUE);
}

void CDialogWeekNumbers::OnSelchangeWeeknumRasterizer() 
{
	UpdateData();

	if(m_Enable) {
		switch(m_Rasterizer.GetCurSel()) {
		case 1:
			GetDlgItem(IDC_WEEKNUM_BROWSE)->EnableWindow(true);
			GetDlgItem(IDC_WEEKNUM_BITMAP)->EnableWindow(true);
			GetDlgItem(IDC_WEEKNUM_SELECT)->EnableWindow(false);
			GetDlgItem(IDC_WEEKNUM_FONTCOLOR)->EnableWindow(false);
			GetDlgItem(IDC_WEEKNUM_COMPONENTS_10)->EnableWindow(true);
			GetDlgItem(IDC_WEEKNUM_COMPONENTS_54)->EnableWindow(true);
			break;
		default:
			GetDlgItem(IDC_WEEKNUM_BROWSE)->EnableWindow(false);
			GetDlgItem(IDC_WEEKNUM_BITMAP)->EnableWindow(false);
			GetDlgItem(IDC_WEEKNUM_SELECT)->EnableWindow(true);
			GetDlgItem(IDC_WEEKNUM_FONTCOLOR)->EnableWindow(true);
			GetDlgItem(IDC_WEEKNUM_COMPONENTS_10)->EnableWindow(false);
			GetDlgItem(IDC_WEEKNUM_COMPONENTS_54)->EnableWindow(false);
		}
	}

	SetModified(TRUE);
}

void CDialogWeekNumbers::OnWeeknumBrowse() 
{
	CFileDialog FileDialog(TRUE);
		
	if(IDOK==FileDialog.DoModal()) {
		m_Bitmap=FileDialog.GetPathName();
		UpdateData(FALSE);
		GetDlgItem(IDC_WEEKNUM_BITMAP)->SetWindowText(m_Bitmap);
	}
}

void CDialogWeekNumbers::OnWeeknumSelect() 
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

		GetDlgItem(IDC_WEEKNUM_FONTNAME)->SetWindowText(m_Font);
		SetModified(TRUE);
	}
}

void CDialogWeekNumbers::OnWeeknumFontcolor() 
{
	CColorDialog ColorDialog(m_FontColor);

	if(IDOK==ColorDialog.DoModal()) {
		m_FontColor=ColorDialog.GetColor();
		GetDlgItem(IDC_WEEKNUM_FONTCOLOR)->Invalidate();
		SetModified(TRUE);
	}
}

void CDialogWeekNumbers::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	HBRUSH Brush=CreateSolidBrush(m_FontColor);
	FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, Brush);
	DeleteObject(Brush);
	
	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


void CDialogWeekNumbers::OnSelchangeWeeknumAlign() 
{
	SetModified(TRUE);
}

void CDialogWeekNumbers::OnChangeWeeknumBitmap() 
{
	SetModified(TRUE);
}

void CDialogWeekNumbers::OnWeeknumComponents54() 
{
	SetModified(TRUE);
}

void CDialogWeekNumbers::OnWeeknumComponents10() 
{
	SetModified(TRUE);
}
