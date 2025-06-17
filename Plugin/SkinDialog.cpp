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
  $Header: /home/cvsroot/Rainlendar/Plugin/SkinDialog.cpp,v 1.7 2004/01/10 15:18:43 rainy Exp $

  $Log: SkinDialog.cpp,v $
  Revision 1.7  2004/01/10 15:18:43  rainy
  Added pages for todo and message box.

  Revision 1.6  2003/10/27 17:40:01  Rainy
  Config is now singleton.

  Revision 1.5  2003/08/09 16:31:03  Rainy
  Added support for "Remember dialog position"-feature.

  Revision 1.4  2003/08/09 15:25:04  Rainy
  Added support for "Remember dialog position"-feature.

  Revision 1.3  2003/06/15 09:50:21  Rainy
  Strings are read from CLanguage class.

  Revision 1.2  2002/11/12 18:09:53  rainy
  Added solid background widgets.

  Revision 1.1  2002/08/03 16:37:08  rainy
  Intial version.

*/

#pragma warning(disable: 4786)

#include "SkinDialog.h"
#include "RainlendarDLL.h"
#include <Prsht.h>
#include <Commdlg.h>
#include <list>

#define NUM_OF_TABS 10

// Few globals
COLORREF g_SolidColor = 0;
COLORREF g_DaysFontColor = 0;
LOGFONT g_DaysFont;
COLORREF g_TodayFontColor = 0;
LOGFONT g_TodayFont;
COLORREF g_EventFontColor = 0;
LOGFONT g_EventFont;
COLORREF g_EventFontColor2 = 0;
LOGFONT g_EventFont2;
COLORREF g_WeekdaysFontColor = 0;
LOGFONT g_WeekdaysFont;
COLORREF g_WeekNumbersFontColor = 0;
LOGFONT g_WeekNumbersFont;
COLORREF g_MonthFontColor = 0;
LOGFONT g_MonthFont;
COLORREF g_YearFontColor = 0;
LOGFONT g_YearFont;
COLORREF g_ToolTipFontColor = 0;
COLORREF g_ToolTipBGColor = 0;
LOGFONT g_ToolTipFont;
COLORREF g_TodoFontColor = 0;
LOGFONT g_TodoFont;
COLORREF g_MessageBoxFontColor = 0;
LOGFONT g_MessageBoxFont;

BOOL CALLBACK BackgroundPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DaysPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TodayPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK EventPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK WeekdaysPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK WeekNumbersPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MonthPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK YearPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TodoPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MessageBoxPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

VOID OpenSkinDialog(HWND hwndOwner, HINSTANCE instance)
{
    PROPSHEETPAGE psp[NUM_OF_TABS];
    PROPSHEETHEADER psh;

	for (int i = 0; i < NUM_OF_TABS; i++)
	{
		psp[i].dwSize = sizeof(PROPSHEETPAGE);
		psp[i].dwFlags = PSP_USETITLE;
		psp[i].hInstance = instance;
		psp[i].pszIcon = NULL;
		psp[i].lParam = 0;
		psp[i].pfnCallback = NULL;

		switch(i)
		{
		case 0:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_BACKGROUND);
			psp[i].pfnDlgProc = BackgroundPageProc;
			psp[i].pszTitle = CCalendarWindow::c_Language.GetString("General", 7);
			break;

		case 1:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_DAYS);
			psp[i].pfnDlgProc = DaysPageProc;
			psp[i].pszTitle = CCalendarWindow::c_Language.GetString("General", 8);
			break;

		case 2:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_TODAY);
			psp[i].pfnDlgProc = TodayPageProc;
			psp[i].pszTitle = CCalendarWindow::c_Language.GetString("General", 9);
			break;

		case 3:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_EVENT);
			psp[i].pfnDlgProc = EventPageProc;
			psp[i].pszTitle = CCalendarWindow::c_Language.GetString("General", 10);
			break;

		case 4:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_WEEKDAYS);
			psp[i].pfnDlgProc = WeekdaysPageProc;
			psp[i].pszTitle = CCalendarWindow::c_Language.GetString("General", 11);
			break;

		case 5:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_WEEKNUMBERS);
			psp[i].pfnDlgProc = WeekNumbersPageProc;
			psp[i].pszTitle = CCalendarWindow::c_Language.GetString("General", 12);
			break;

		case 6:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_MONTH);
			psp[i].pfnDlgProc = MonthPageProc;
			psp[i].pszTitle = CCalendarWindow::c_Language.GetString("General", 13);
			break;

		case 7:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_YEAR);
			psp[i].pfnDlgProc = YearPageProc;
			psp[i].pszTitle = CCalendarWindow::c_Language.GetString("General", 14);
			break;

		case 8:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_TODO_CONFIG);
			psp[i].pfnDlgProc = TodoPageProc;
			psp[i].pszTitle = CCalendarWindow::c_Language.GetString("General", 16);
			break;

		case 9:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_MESSAGEBOX);
			psp[i].pfnDlgProc = MessageBoxPageProc;
			psp[i].pszTitle = CCalendarWindow::c_Language.GetString("General", 17);
			break;
		}
	}

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOCONTEXTHELP ;
    psh.hwndParent = hwndOwner;
    psh.hInstance = instance;
    psh.pszIcon = NULL;
    psh.pszCaption = (LPSTR)CCalendarWindow::c_Language.GetString("General", 18);
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback = NULL;

	if (CConfig::Instance().GetCurrentSkin().empty() || CConfig::Instance().GetCurrentSkinIni().empty())
	{
		//	"Sorry!\n\nSkin editing is only possible with separate skin configurations.\n\nWhat you need to do is to remove all the skinning stuff from Rainlendar.ini\nand put them in a separate ini-file inside a subfolder under Rainlendar's folder.\nConsult the documentation for more info.",
		MessageBox(hwndOwner, CCalendarWindow::c_Language.GetString("Message", 6), "Rainlendar", MB_OK | MB_ICONEXCLAMATION);
	}
	else
	{
		if (PropertySheet(&psh) == IDOK)
		{
			CConfig::Instance().WriteConfig(CConfig::WRITE_SKIN);
			SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);		// Refresh
		}
	}
}

/*
** Some convenience functions below
*/

int InitRasterizer(int ras)
{
	if (ras == CRasterizer::TYPE_BITMAP)
	{
		return 1;
	}
	return 0;
}

int InitAlign(int align)
{
	int state = 0;

	switch (align & 0x0F)
	{
	case CRasterizer::ALIGN_LEFT:
		state += 0;
		break;

	case CRasterizer::ALIGN_HCENTER:
		state += 1;
		break;

	case CRasterizer::ALIGN_RIGHT:
		state += 2;
		break;
	};

	switch (align & 0x0F0)
	{
	case CRasterizer::ALIGN_TOP:
		state += 0;
		break;

	case CRasterizer::ALIGN_VCENTER:
		state += 3;
		break;

	case CRasterizer::ALIGN_BOTTOM:
		state += 6;
		break;
	};

	return state;
}

void InitFont(const std::string& fontStr, LOGFONT* logFont)
{
	int nHeight;
	int nWidth;
	int nEscapement;
	int nOrientation; 
	int nWeight;
	DWORD bItalic; 
	DWORD bUnderline;
	DWORD cStrikeOut; 
	DWORD nCharSet;
	DWORD nOutPrecision;
	DWORD nClipPrecision; 
	DWORD nQuality;
	DWORD nPitchAndFamily;
	
	sscanf(fontStr.c_str(), "%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i", 
		&nHeight, &nWidth, &nEscapement, &nOrientation, &nWeight,
		&bItalic, &bUnderline, &cStrikeOut, &nCharSet, &nOutPrecision, 
		&nClipPrecision, &nQuality, &nPitchAndFamily);

	logFont->lfHeight = nHeight;
	logFont->lfWidth = nWidth;
	logFont->lfEscapement = nEscapement;
	logFont->lfOrientation = nOrientation;
	logFont->lfWeight = nWeight;
	logFont->lfItalic = bItalic;
	logFont->lfUnderline = bUnderline;
	logFont->lfStrikeOut = cStrikeOut;
	logFont->lfCharSet = nCharSet;
	logFont->lfOutPrecision = nOutPrecision;
	logFont->lfClipPrecision =  nClipPrecision;
	logFont->lfQuality = nQuality;
	logFont->lfPitchAndFamily = nPitchAndFamily;

	int pos = fontStr.rfind('/');
	std::string typeface(fontStr.begin() + pos + 1, fontStr.end());	
	strcpy(logFont->lfFaceName, typeface.c_str());
}

CRasterizer::ALIGN GetAlign(HWND window, int item)
{
	int align;
	int index = SendDlgItemMessage(window, item, CB_GETCURSEL, NULL, NULL);
	switch(index) 
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

	return (CRasterizer::ALIGN)align;
}

CRasterizer::TYPE GetRasterizer(HWND window, int item)
{
	int index = SendDlgItemMessage(window, item, CB_GETCURSEL, NULL, NULL);
	if (index == 1)
	{
		return CRasterizer::TYPE_BITMAP;
	}
	return CRasterizer::TYPE_FONT;
}

std::string GetFontString(LOGFONT* logFont)
{
	char tmpSz[1024];

	sprintf(tmpSz, "%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/", 
		   logFont->lfHeight,
		   logFont->lfWidth,
		   logFont->lfEscapement,
		   logFont->lfOrientation,
		   logFont->lfWeight,
		   logFont->lfItalic, 
		   logFont->lfUnderline,
		   logFont->lfStrikeOut, 
		   logFont->lfCharSet,
		   logFont->lfOutPrecision,
		   logFont->lfClipPrecision, 
		   logFont->lfQuality,
		   logFont->lfPitchAndFamily);

	std::string fontStr(tmpSz);
	fontStr += logFont->lfFaceName;
	return fontStr;
}

BOOL DrawItemFontColor(COLORREF color, WPARAM wParam, LPARAM lParam)
{
	DRAWITEMSTRUCT* drawItem = (DRAWITEMSTRUCT*)lParam;
	HBRUSH Brush = CreateSolidBrush(color);
	FillRect(drawItem->hDC, &drawItem->rcItem, Brush);
	DrawEdge(drawItem->hDC, &drawItem->rcItem, EDGE_RAISED, BF_RECT);
	DeleteObject(Brush);

	return TRUE;
}

BOOL BrowseBitmap(HWND hwndDlg, int item)
{
	char file[MAX_LINE_LENGTH];
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	file[0] = 0;

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwndDlg;
	ofn.lpstrFile = file;
	ofn.nMaxFile = MAX_LINE_LENGTH;
	ofn.Flags = OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		HWND widget = NULL;
		widget = GetDlgItem(hwndDlg, item);
		SetWindowText(widget, file);
	}

	return TRUE;
}

void SelectFont(HWND hwndDlg, LOGFONT* font)
{
	CHOOSEFONT cf;
	memset(&cf, 0, sizeof(CHOOSEFONT));

	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = hwndDlg;
	cf.lpLogFont = font;
	cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;

	ChooseFont(&cf);
}

BOOL SelectFontColor(HWND hwndDlg, COLORREF* color)
{
	CHOOSECOLOR cc;
	COLORREF custom[16];
	memset(custom, 0, sizeof(COLORREF) * 16);
	memset(&cc, 0, sizeof(CHOOSECOLOR));
	
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = hwndDlg;
	cc.rgbResult = *color;
	cc.lpCustColors = custom;
	cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	
	if (ChooseColor(&cc))
	{
		*color = cc.rgbResult;
		InvalidateRect(hwndDlg, NULL, false);
	}
	return TRUE;
}

/****************************************************************************\ 
**
**                       B A C K G R O U N D
**
\****************************************************************************/

void UpdateBackgroundWidgets(HWND window)
{
	if (IsDlgButtonChecked(window, IDC_BACKGROUND_COPYWALLPAPER) == BST_CHECKED)
	{
		EnableWindow(GetDlgItem(window, IDC_BACKGROUND_FILENAME), FALSE);
	}
	else
	{
		EnableWindow(GetDlgItem(window, IDC_BACKGROUND_FILENAME), TRUE);
	}

	if (IsDlgButtonChecked(window, IDC_BACKGROUND_SOLID) == BST_CHECKED)
	{
		EnableWindow(GetDlgItem(window, IDC_BACKGROUND_FILENAME), FALSE);
		EnableWindow(GetDlgItem(window, IDC_BACKGROUND_SOLIDCOLOR), TRUE);
		EnableWindow(GetDlgItem(window, IDC_BACKGROUND_BEVEL), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(window, IDC_BACKGROUND_SOLIDCOLOR), FALSE);
		EnableWindow(GetDlgItem(window, IDC_BACKGROUND_BEVEL), FALSE);
	}

	// This is always disabled
	EnableWindow(GetDlgItem(window, IDC_TOOLTIP_FONTNAME), FALSE);

	// Update font name
	char tmpSz[256];
	HWND widget = GetDlgItem(window, IDC_TOOLTIP_FONTNAME);
	std::string name = g_ToolTipFont.lfFaceName;
	name += " / ";
	HDC dc = GetWindowDC(window);
	int size = -MulDiv(g_ToolTipFont.lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	ReleaseDC(window, dc);
	itoa(size, tmpSz, 10);
	name += tmpSz;
	SetWindowText(widget, name.c_str());
}

BOOL OnInitBackgroundDialog(HWND window) 
{
	HWND widget;
	UINT state;

	// Set localized strings
	widget = GetDlgItem(GetParent(window), IDOK);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("General", 0));
	widget = GetDlgItem(GetParent(window), IDCANCEL);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("General", 1));
	widget = GetDlgItem(GetParent(window), 0x3021);		// APPLY
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("General", 2));

	widget = GetDlgItem(window, IDC_BACKGROUND_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 21));
	widget = GetDlgItem(window, IDC_BACKGROUND_FILENAME_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 22));
	widget = GetDlgItem(window, IDC_BACKGROUND_BROWSE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 3));
	widget = GetDlgItem(window, IDC_BACKGROUND_STRETCH);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 18));
	widget = GetDlgItem(window, IDC_BACKGROUND_TILE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 19));
	widget = GetDlgItem(window, IDC_BACKGROUND_COPYWALLPAPER);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 20));
	widget = GetDlgItem(window, IDC_BACKGROUND_SOLID);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 26));
	widget = GetDlgItem(window, IDC_BACKGROUND_BEVEL);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 27));
	widget = GetDlgItem(window, IDC_BACKGROUND_TOOLTIP_GROUP);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 24));
	widget = GetDlgItem(window, IDC_BACKGROUND_FONT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 15));
	widget = GetDlgItem(window, IDC_BACKGROUND_TEXT_COLOR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 23));
	widget = GetDlgItem(window, IDC_BACKGROUND_BACKGROUND_COLOR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 25));
	widget = GetDlgItem(window, IDC_TOOLTIP_SELECT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 16));

	widget = GetDlgItem(window, IDC_BACKGROUND_FILENAME);
	SetWindowText(widget, CConfig::Instance().GetBackgroundBitmapName().c_str());

	switch (CConfig::Instance().GetBackgroundMode())
	{
	case MODE_TILE:
		state = IDC_BACKGROUND_TILE;
		break;

	case MODE_STRETCH:
		state = IDC_BACKGROUND_STRETCH;
		break;

	case MODE_SOLID:
		state = IDC_BACKGROUND_SOLID;
		break;

	case MODE_COPY:
	default:
		state = IDC_BACKGROUND_COPYWALLPAPER;
		break;
	}
	CheckRadioButton(window, IDC_BACKGROUND_TILE, IDC_BACKGROUND_SOLID, state);

	g_SolidColor = CConfig::Instance().GetBackgroundSolidColor();
	g_ToolTipFontColor = CConfig::Instance().GetToolTipFontColor();
	g_ToolTipBGColor = CConfig::Instance().GetToolTipBGColor();
	InitFont(CConfig::Instance().GetToolTipFont(), &g_ToolTipFont);

	state = CConfig::Instance().GetBackgroundBevel() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_BACKGROUND_BEVEL, state);

	UpdateBackgroundWidgets(window);

	if (CConfig::Instance().GetRememberDialogPositions())
	{
		POINT pos = CConfig::Instance().GetDialogPosition(CConfig::DIALOG_EDITSKIN);
		SetWindowPos(GetParent(window), NULL, pos.x, pos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}

	return TRUE;
}

void OnOKBackgroundDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;

	widget = GetDlgItem(window, IDC_BACKGROUND_FILENAME);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetBackgroundBitmapName(tmpSz);

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_BACKGROUND_STRETCH))
	{
		CConfig::Instance().SetBackgroundMode(MODE_STRETCH);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_BACKGROUND_TILE))
	{
		CConfig::Instance().SetBackgroundMode(MODE_TILE);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_BACKGROUND_SOLID))
	{
		CConfig::Instance().SetBackgroundMode(MODE_SOLID);
	}
	else
	{
		CConfig::Instance().SetBackgroundMode(MODE_COPY);
	}

	CConfig::Instance().SetBackgroundSolidColor(g_SolidColor);
	CConfig::Instance().SetToolTipFontColor(g_ToolTipFontColor);
	CConfig::Instance().SetToolTipBGColor(g_ToolTipBGColor);
	std::string fontStr = GetFontString(&g_ToolTipFont);
	CConfig::Instance().SetToolTipFont(fontStr);

	bool state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_BACKGROUND_BEVEL));
	CConfig::Instance().SetBackgroundBevel(state);
}

BOOL CALLBACK BackgroundPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitBackgroundDialog(hwndDlg);

        case WM_DRAWITEM:
			if (wParam == IDC_TOOLTIP_TEXTFONTCOLOR)
			{
				return DrawItemFontColor(g_ToolTipFontColor, wParam, lParam);
			}
			if (wParam == IDC_TOOLTIP_BGCOLOR)
			{
				return DrawItemFontColor(g_ToolTipBGColor, wParam, lParam);
			}
			if (wParam == IDC_BACKGROUND_SOLIDCOLOR)
			{
				return DrawItemFontColor(g_SolidColor, wParam, lParam);
			}
			return FALSE;

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				LPPSHNOTIFY pshNotify = (LPPSHNOTIFY)lParam;
				OnOKBackgroundDialog(hwndDlg);
				if (pshNotify->lParam != TRUE)
				{
					CConfig::Instance().WriteConfig(CConfig::WRITE_SKIN);
					SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
					PropSheet_CancelToClose(GetParent(hwndDlg));
				}
				SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
				return TRUE;
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_BACKGROUND_BROWSE:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return BrowseBitmap(hwndDlg, IDC_BACKGROUND_FILENAME);

			case IDC_BACKGROUND_TILE:
			case IDC_BACKGROUND_STRETCH:
			case IDC_BACKGROUND_COPYWALLPAPER:
			case IDC_BACKGROUND_SOLID:
				UpdateBackgroundWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_BACKGROUND_FILENAME:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_BACKGROUND_BEVEL:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_TOOLTIP_TEXTFONTCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_ToolTipFontColor);

			case IDC_TOOLTIP_BGCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_ToolTipBGColor);

			case IDC_BACKGROUND_SOLIDCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_SolidColor);

			case IDC_TOOLTIP_SELECT:
				SelectFont(hwndDlg, &g_ToolTipFont);
				UpdateBackgroundWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			}
			break;

		case WM_DESTROY:
			{
				RECT rc;
				GetWindowRect(GetParent(hwndDlg), &rc);	// Get the sheet position (i.e. parent)
				CConfig::Instance().SetDialogPosition(CConfig::DIALOG_EDITSKIN, rc.left, rc.top);
				CConfig::Instance().WriteConfig(CConfig::WRITE_FULL);
			}
			break;
	}
    return FALSE; 
}

/****************************************************************************\ 
**
**                           D A Y S
**
\****************************************************************************/

void UpdateDaysWidgets(HWND window)
{
	if (IsDlgButtonChecked(window, IDC_DAYS_ENABLE) == BST_CHECKED)
	{
		EnableWindow(GetDlgItem(window, IDC_DAYS_X), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_Y), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_W), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_H), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_IGNORE_TODAY), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_IGNORE_EVENT), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_ALIGN), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_RASTERIZER), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_BITMAP), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_BROWSE), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_10), TRUE); 
		EnableWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_32), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_SELECT), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR), TRUE);

		if (IsDlgButtonChecked(window, IDC_DAYS_COMPONENTS_10) == BST_CHECKED)
		{
			EnableWindow(GetDlgItem(window, IDC_DAYS_SEPARATION), TRUE);
		}
		else
		{
			EnableWindow(GetDlgItem(window, IDC_DAYS_SEPARATION), FALSE);
		}
	}
	else
	{
		// Disable all
		EnableWindow(GetDlgItem(window, IDC_DAYS_X), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_Y), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_W), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_H), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_IGNORE_TODAY), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_IGNORE_EVENT), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_ALIGN), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_RASTERIZER), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_BITMAP), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_BROWSE), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_10), FALSE); 
		EnableWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_32), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_SEPARATION), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_SELECT), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR), FALSE);
	}

	// This is always disabled
	EnableWindow(GetDlgItem(window, IDC_DAYS_FONTNAME), FALSE);

	// Hide the Bitmap or Font stuff depending which rasterizer is in use
	int index = SendDlgItemMessage(window, IDC_DAYS_RASTERIZER, CB_GETCURSEL, NULL, NULL);
	if (index == 0)
	{
		// Hide Font show Bitmap
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_SEPARATION), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_10), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_32), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BROWSE), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_SEPARATION_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP_STATIC), SW_HIDE);
		
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTNAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONT_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_SELECT), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONT_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR_STATIC), SW_SHOW);
	}
	else
	{
		// Hide Bitmap show Font
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_SEPARATION), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_10), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_32), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BROWSE), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_SEPARATION_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP_STATIC), SW_SHOW);
		
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTNAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONT_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_SELECT), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONT_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR_STATIC), SW_HIDE);
	}

	// Update font name
	char tmpSz[256];
	HWND widget = GetDlgItem(window, IDC_DAYS_FONTNAME);
	std::string name = g_DaysFont.lfFaceName;
	name += " / ";
	HDC dc = GetWindowDC(window);
	int size = -MulDiv(g_DaysFont.lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	ReleaseDC(window, dc);
	itoa(size, tmpSz, 10);
	name += tmpSz;
	SetWindowText(widget, name.c_str());

}

BOOL OnInitDaysDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	UINT state;

	// Set localized strings
	widget = GetDlgItem(window, IDC_DAYS_ENABLE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 0));
	widget = GetDlgItem(window, IDC_DAYS_X_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 4));
	widget = GetDlgItem(window, IDC_DAYS_Y_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 5));
	widget = GetDlgItem(window, IDC_DAYS_W_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 6));
	widget = GetDlgItem(window, IDC_DAYS_H_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 7));
	widget = GetDlgItem(window, IDC_DAYS_ALIGN_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 9));
	widget = GetDlgItem(window, IDC_DAYS_IGNORE_TODAY);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 1));
	widget = GetDlgItem(window, IDC_DAYS_IGNORE_EVENT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 2));
	widget = GetDlgItem(window, IDC_DAYS_RASTERIZER_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 8));
	widget = GetDlgItem(window, IDC_DAYS_BITMAP_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 10));
	widget = GetDlgItem(window, IDC_DAYS_BITMAP_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 11));
	widget = GetDlgItem(window, IDC_DAYS_COMPONENTS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 12));
	widget = GetDlgItem(window, IDC_DAYS_SEPARATION_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 13));
	widget = GetDlgItem(window, IDC_DAYS_BROWSE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 3));
	widget = GetDlgItem(window, IDC_DAYS_FONT_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	widget = GetDlgItem(window, IDC_DAYS_FONT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 15));
	widget = GetDlgItem(window, IDC_DAYS_FONTCOLOR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 17));
	widget = GetDlgItem(window, IDC_DAYS_SELECT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 16));

	state = CConfig::Instance().GetDaysEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_DAYS_ENABLE, state);

	widget = GetDlgItem(window, IDC_DAYS_X);
	itoa(CConfig::Instance().GetDaysX(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_DAYS_Y);
	itoa(CConfig::Instance().GetDaysY(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_DAYS_W);
	itoa(CConfig::Instance().GetDaysW(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_DAYS_H);
	itoa(CConfig::Instance().GetDaysH(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	state = CConfig::Instance().GetDaysIgnoreToday() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_DAYS_IGNORE_TODAY, state);

	state = CConfig::Instance().GetDaysIgnoreEvent() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_DAYS_IGNORE_EVENT, state);

	// Fill the combos
	SendDlgItemMessage(window, IDC_DAYS_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	SendDlgItemMessage(window, IDC_DAYS_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 10));

	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 35));
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 36));
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 37));
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 38));
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 39));
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 40));
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 41));
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 42));
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 43));

	state = InitRasterizer(CConfig::Instance().GetDaysRasterizer());
	SendDlgItemMessage(window, IDC_DAYS_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(CConfig::Instance().GetDaysAlign());
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_DAYS_BITMAP);
	SetWindowText(widget, CConfig::Instance().GetDaysBitmapName().c_str());

	switch (CConfig::Instance().GetDaysNumOfComponents())
	{
	case 10:
		state = IDC_DAYS_COMPONENTS_10;
		break;

	case 32:
	default:
		state = IDC_DAYS_COMPONENTS_32;
		break;
	}
	CheckRadioButton(window, IDC_DAYS_COMPONENTS_10, IDC_DAYS_COMPONENTS_32, state);

	widget = GetDlgItem(window, IDC_DAYS_SEPARATION);
	itoa(CConfig::Instance().GetDaysSeparation(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_DaysFontColor = CConfig::Instance().GetDaysFontColor();
	InitFont(CConfig::Instance().GetDaysFont(), &g_DaysFont);

	UpdateDaysWidgets(window);

	return TRUE;
}

void OnOKDaysDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_DAYS_ENABLE));
	CConfig::Instance().SetDaysEnable(state);

	widget = GetDlgItem(window, IDC_DAYS_X);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetDaysX(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_DAYS_Y);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetDaysY(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_DAYS_W);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetDaysW(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_DAYS_H);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetDaysH(atoi(tmpSz));

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_DAYS_IGNORE_TODAY));
	CConfig::Instance().SetDaysIgnoreToday(state);

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_DAYS_IGNORE_EVENT));
	CConfig::Instance().SetDaysIgnoreEvent(state);

	widget = GetDlgItem(window, IDC_DAYS_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetDaysBitmapName(tmpSz);

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_DAYS_COMPONENTS_10))
	{
		CConfig::Instance().SetDaysNumOfComponents(10);
	}
	else
	{
		CConfig::Instance().SetDaysNumOfComponents(32);
	}

	widget = GetDlgItem(window, IDC_DAYS_SEPARATION);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetDaysSeparation(atoi(tmpSz));

	CRasterizer::ALIGN align = GetAlign(window, IDC_DAYS_ALIGN);
	CConfig::Instance().SetDaysAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_DAYS_RASTERIZER);
	CConfig::Instance().SetDaysRasterizer(ras);

	CConfig::Instance().SetDaysFontColor(g_DaysFontColor);
	
	std::string fontStr = GetFontString(&g_DaysFont);
	CConfig::Instance().SetDaysFont(fontStr);
}

BOOL CALLBACK DaysPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitDaysDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				LPPSHNOTIFY pshNotify = (LPPSHNOTIFY)lParam;
				OnOKDaysDialog(hwndDlg);
				if (pshNotify->lParam != TRUE)
				{
					CConfig::Instance().WriteConfig(CConfig::WRITE_SKIN);
					SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
					PropSheet_CancelToClose(GetParent(hwndDlg));
				}
				SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
				return TRUE;
			}
			break;

        case WM_DRAWITEM:
			if (wParam == IDC_DAYS_FONTCOLOR)
			{
				return DrawItemFontColor(g_DaysFontColor, wParam, lParam);
			}
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_DAYS_BROWSE:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return BrowseBitmap(hwndDlg, IDC_DAYS_BITMAP);

			case IDC_DAYS_SELECT:
				SelectFont(hwndDlg, &g_DaysFont);
				UpdateDaysWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_DAYS_FONTCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_DaysFontColor);

			case IDC_DAYS_ENABLE:
			case IDC_DAYS_COMPONENTS_10:
			case IDC_DAYS_COMPONENTS_32:
				UpdateDaysWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_DAYS_RASTERIZER:
				if (HIWORD(wParam) == CBN_SELCHANGE)
				{
					UpdateDaysWidgets(hwndDlg);
					PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				}
				return TRUE;

			case IDC_DAYS_X:
			case IDC_DAYS_Y:
			case IDC_DAYS_W:
			case IDC_DAYS_H:
			case IDC_DAYS_ALIGN:
			case IDC_DAYS_SEPARATION:
			case IDC_DAYS_BITMAP:
			case IDC_DAYS_IGNORE_EVENT:
			case IDC_DAYS_IGNORE_TODAY:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
			}
			break;

	}
    return FALSE; 
}

/****************************************************************************\ 
**
**                           T O D A Y
**
\****************************************************************************/

void UpdateTodayWidgets(HWND window)
{
	if (IsDlgButtonChecked(window, IDC_TODAY_ENABLE) == BST_CHECKED)
	{
		EnableWindow(GetDlgItem(window, IDC_TODAY_ALIGN), TRUE);
		EnableWindow(GetDlgItem(window, IDC_TODAY_RASTERIZER), TRUE);
		EnableWindow(GetDlgItem(window, IDC_TODAY_BITMAP), TRUE);
		EnableWindow(GetDlgItem(window, IDC_TODAY_BROWSE), TRUE);
		EnableWindow(GetDlgItem(window, IDC_TODAY_COMPONENTS_1), TRUE); 
		EnableWindow(GetDlgItem(window, IDC_TODAY_COMPONENTS_10), TRUE); 
		EnableWindow(GetDlgItem(window, IDC_TODAY_COMPONENTS_32), TRUE);
		EnableWindow(GetDlgItem(window, IDC_TODAY_SELECT), TRUE);
		EnableWindow(GetDlgItem(window, IDC_TODAY_FONTCOLOR), TRUE);

		if (IsDlgButtonChecked(window, IDC_TODAY_COMPONENTS_10) == BST_CHECKED)
		{
			EnableWindow(GetDlgItem(window, IDC_TODAY_SEPARATION), TRUE);
		}
		else
		{
			EnableWindow(GetDlgItem(window, IDC_TODAY_SEPARATION), FALSE);
		}
	}
	else
	{
		// Disable all
		EnableWindow(GetDlgItem(window, IDC_TODAY_ALIGN), FALSE);
		EnableWindow(GetDlgItem(window, IDC_TODAY_RASTERIZER), FALSE);
		EnableWindow(GetDlgItem(window, IDC_TODAY_BITMAP), FALSE);
		EnableWindow(GetDlgItem(window, IDC_TODAY_BROWSE), FALSE);
		EnableWindow(GetDlgItem(window, IDC_TODAY_COMPONENTS_1), FALSE); 
		EnableWindow(GetDlgItem(window, IDC_TODAY_COMPONENTS_10), FALSE); 
		EnableWindow(GetDlgItem(window, IDC_TODAY_COMPONENTS_32), FALSE);
		EnableWindow(GetDlgItem(window, IDC_TODAY_SEPARATION), FALSE);
		EnableWindow(GetDlgItem(window, IDC_TODAY_SELECT), FALSE);
		EnableWindow(GetDlgItem(window, IDC_TODAY_FONTCOLOR), FALSE);
	}

	// This is always disabled
	EnableWindow(GetDlgItem(window, IDC_TODAY_FONTNAME), FALSE);

	// Hide the Bitmap or Font stuff depending which rasterizer is in use
	int index = SendDlgItemMessage(window, IDC_TODAY_RASTERIZER, CB_GETCURSEL, NULL, NULL);
	if (index == 0)
	{
		// Hide Font show Bitmap
		ShowWindow(GetDlgItem(window, IDC_TODAY_BITMAP_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_TODAY_SEPARATION), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_TODAY_BITMAP), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_TODAY_COMPONENTS_1), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_TODAY_COMPONENTS_10), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_TODAY_COMPONENTS_32), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_TODAY_BROWSE), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_TODAY_COMPONENTS_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_TODAY_SEPARATION_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_TODAY_BITMAP_STATIC), SW_HIDE);
		
		ShowWindow(GetDlgItem(window, IDC_TODAY_FONTNAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_TODAY_FONT_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_TODAY_SELECT), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_TODAY_FONTCOLOR), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_TODAY_FONT_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_TODAY_FONTCOLOR_STATIC), SW_SHOW);
	}
	else
	{
		// Hide Bitmap show Font
		ShowWindow(GetDlgItem(window, IDC_TODAY_BITMAP_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_TODAY_SEPARATION), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_TODAY_BITMAP), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_TODAY_COMPONENTS_1), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_TODAY_COMPONENTS_10), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_TODAY_COMPONENTS_32), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_TODAY_BROWSE), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_TODAY_COMPONENTS_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_TODAY_SEPARATION_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_TODAY_BITMAP_STATIC), SW_SHOW);
		
		ShowWindow(GetDlgItem(window, IDC_TODAY_FONTNAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_TODAY_FONT_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_TODAY_SELECT), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_TODAY_FONTCOLOR), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_TODAY_FONT_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_TODAY_FONTCOLOR_STATIC), SW_HIDE);
	}

	// Update font name
	char tmpSz[256];
	HWND widget = GetDlgItem(window, IDC_TODAY_FONTNAME);
	std::string name = g_TodayFont.lfFaceName;
	name += " / ";
	HDC dc = GetWindowDC(window);
	int size = -MulDiv(g_TodayFont.lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	ReleaseDC(window, dc);
	itoa(size, tmpSz, 10);
	name += tmpSz;
	SetWindowText(widget, name.c_str());

}

BOOL OnInitTodayDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	UINT state;

	widget = GetDlgItem(window, IDC_TODAY_ENABLE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 30));
	widget = GetDlgItem(window, IDC_TODAY_ALIGN_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 9));
	widget = GetDlgItem(window, IDC_TODAY_RASTERIZER_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 8));
	widget = GetDlgItem(window, IDC_TODAY_BITMAP_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 10));
	widget = GetDlgItem(window, IDC_TODAY_BITMAP_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 11));
	widget = GetDlgItem(window, IDC_TODAY_COMPONENTS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 12));
	widget = GetDlgItem(window, IDC_TODAY_SEPARATION_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 13));
	widget = GetDlgItem(window, IDC_TODAY_BROWSE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 3));
	widget = GetDlgItem(window, IDC_TODAY_FONT_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	widget = GetDlgItem(window, IDC_TODAY_FONT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 15));
	widget = GetDlgItem(window, IDC_TODAY_FONTCOLOR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 17));
	widget = GetDlgItem(window, IDC_TODAY_SELECT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 16));

	state = CConfig::Instance().GetTodayEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_TODAY_ENABLE, state);

	// Fill the combos
	SendDlgItemMessage(window, IDC_TODAY_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	SendDlgItemMessage(window, IDC_TODAY_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 10));

	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 35));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 36));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 37));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 38));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 39));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 40));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 41));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 42));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 43));

	state = InitRasterizer(CConfig::Instance().GetTodayRasterizer());
	SendDlgItemMessage(window, IDC_TODAY_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(CConfig::Instance().GetTodayAlign());
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_TODAY_BITMAP);
	SetWindowText(widget, CConfig::Instance().GetTodayBitmapName().c_str());

	switch (CConfig::Instance().GetTodayNumOfComponents())
	{
	case 1:
		state = IDC_TODAY_COMPONENTS_1;
		break;

	case 10:
		state = IDC_TODAY_COMPONENTS_10;
		break;

	case 32:
	default:
		state = IDC_TODAY_COMPONENTS_32;
		break;
	}
	CheckRadioButton(window, IDC_TODAY_COMPONENTS_1, IDC_TODAY_COMPONENTS_32, state);

	widget = GetDlgItem(window, IDC_TODAY_SEPARATION);
	itoa(CConfig::Instance().GetTodaySeparation(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_TodayFontColor = CConfig::Instance().GetTodayFontColor();
	InitFont(CConfig::Instance().GetTodayFont(), &g_TodayFont);

	UpdateTodayWidgets(window);

	return TRUE;
}

void OnOKTodayDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_TODAY_ENABLE));
	CConfig::Instance().SetTodayEnable(state);

	widget = GetDlgItem(window, IDC_TODAY_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetTodayBitmapName(tmpSz);

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_TODAY_COMPONENTS_1))
	{
		CConfig::Instance().SetTodayNumOfComponents(1);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_TODAY_COMPONENTS_10))
	{
		CConfig::Instance().SetTodayNumOfComponents(10);
	}
	else
	{
		CConfig::Instance().SetTodayNumOfComponents(32);
	}

	widget = GetDlgItem(window, IDC_TODAY_SEPARATION);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetTodaySeparation(atoi(tmpSz));

	CRasterizer::ALIGN align = GetAlign(window, IDC_TODAY_ALIGN);
	CConfig::Instance().SetTodayAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_TODAY_RASTERIZER);
	CConfig::Instance().SetTodayRasterizer(ras);

	CConfig::Instance().SetTodayFontColor(g_TodayFontColor);
	
	std::string fontStr = GetFontString(&g_TodayFont);
	CConfig::Instance().SetTodayFont(fontStr);
}

BOOL CALLBACK TodayPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitTodayDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				LPPSHNOTIFY pshNotify = (LPPSHNOTIFY)lParam;
				OnOKTodayDialog(hwndDlg);
				if (pshNotify->lParam != TRUE)
				{
					CConfig::Instance().WriteConfig(CConfig::WRITE_SKIN);
					SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
					PropSheet_CancelToClose(GetParent(hwndDlg));
				}
				SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
				return TRUE;
			}
			break;

        case WM_DRAWITEM:
			if (wParam == IDC_TODAY_FONTCOLOR)
			{
				return DrawItemFontColor(g_TodayFontColor, wParam, lParam);
			}
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_TODAY_BROWSE:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return BrowseBitmap(hwndDlg, IDC_TODAY_BITMAP);

			case IDC_TODAY_SELECT:
				SelectFont(hwndDlg, &g_TodayFont);
				UpdateTodayWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_TODAY_FONTCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_TodayFontColor);

			case IDC_TODAY_ENABLE:
			case IDC_TODAY_COMPONENTS_1:
			case IDC_TODAY_COMPONENTS_10:
			case IDC_TODAY_COMPONENTS_32:
				UpdateTodayWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_TODAY_RASTERIZER:
				if (HIWORD(wParam) == CBN_SELCHANGE)
				{
					UpdateTodayWidgets(hwndDlg);
					PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				}
				return TRUE;

			case IDC_TODAY_ALIGN:
			case IDC_TODAY_SEPARATION:
			case IDC_TODAY_BITMAP:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
			}
			break;

	}
    return FALSE; 
}

/****************************************************************************\ 
**
**                           E V E N T
**
\****************************************************************************/

void UpdateEventWidgets(HWND window)
{
	if (IsDlgButtonChecked(window, IDC_EVENT_ENABLE) == BST_CHECKED)
	{
		EnableWindow(GetDlgItem(window, IDC_EVENT_ALIGN), TRUE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_RASTERIZER), TRUE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_BITMAP), TRUE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_BROWSE), TRUE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_COMPONENTS_1), TRUE); 
		EnableWindow(GetDlgItem(window, IDC_EVENT_COMPONENTS_10), TRUE); 
		EnableWindow(GetDlgItem(window, IDC_EVENT_COMPONENTS_32), TRUE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_SELECT), TRUE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_FONTCOLOR), TRUE);

		if (IsDlgButtonChecked(window, IDC_EVENT_COMPONENTS_10) == BST_CHECKED)
		{
			EnableWindow(GetDlgItem(window, IDC_EVENT_SEPARATION), TRUE);
		}
		else
		{
			EnableWindow(GetDlgItem(window, IDC_EVENT_SEPARATION), FALSE);
		}

		if (IsDlgButtonChecked(window, IDC_EVENT_CALENDAR) == BST_CHECKED)
		{
			EnableWindow(GetDlgItem(window, IDC_EVENT_FONTCOLOR2), TRUE);
			EnableWindow(GetDlgItem(window, IDC_EVENT_SELECT2), TRUE);
		}
		else
		{
			EnableWindow(GetDlgItem(window, IDC_EVENT_FONTCOLOR2), FALSE);
			EnableWindow(GetDlgItem(window, IDC_EVENT_SELECT2), FALSE);
		}
	}
	else
	{
		// Disable all
		EnableWindow(GetDlgItem(window, IDC_EVENT_ALIGN), FALSE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_RASTERIZER), FALSE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_BITMAP), FALSE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_BROWSE), FALSE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_COMPONENTS_1), FALSE); 
		EnableWindow(GetDlgItem(window, IDC_EVENT_COMPONENTS_10), FALSE); 
		EnableWindow(GetDlgItem(window, IDC_EVENT_COMPONENTS_32), FALSE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_SEPARATION), FALSE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_SELECT), FALSE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_FONTCOLOR), FALSE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_FONTCOLOR2), FALSE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_SELECT2), FALSE);
		EnableWindow(GetDlgItem(window, IDC_EVENT_CALENDAR), FALSE);
	}

	// This is always disabled
	EnableWindow(GetDlgItem(window, IDC_EVENT_FONTNAME), FALSE);
	EnableWindow(GetDlgItem(window, IDC_EVENT_FONTNAME2), FALSE);

	// Hide the Bitmap or Font stuff depending which rasterizer is in use
	int index = SendDlgItemMessage(window, IDC_EVENT_RASTERIZER, CB_GETCURSEL, NULL, NULL);
	if (index == 0)
	{
		// Hide Font show Bitmap
		ShowWindow(GetDlgItem(window, IDC_EVENT_BITMAP_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EVENT_SEPARATION), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EVENT_BITMAP), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EVENT_COMPONENTS_1), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EVENT_COMPONENTS_10), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EVENT_COMPONENTS_32), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EVENT_BROWSE), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EVENT_COMPONENTS_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EVENT_SEPARATION_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EVENT_BITMAP_STATIC), SW_HIDE);
		
		ShowWindow(GetDlgItem(window, IDC_EVENT_FONTNAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EVENT_FONT_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EVENT_SELECT), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EVENT_FONTCOLOR), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EVENT_FONT_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EVENT_FONTCOLOR_STATIC), SW_SHOW);
	}
	else
	{
		// Hide Bitmap show Font
		ShowWindow(GetDlgItem(window, IDC_EVENT_BITMAP_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EVENT_SEPARATION), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EVENT_BITMAP), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EVENT_COMPONENTS_1), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EVENT_COMPONENTS_10), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EVENT_COMPONENTS_32), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EVENT_BROWSE), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EVENT_COMPONENTS_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EVENT_SEPARATION_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EVENT_BITMAP_STATIC), SW_SHOW);
		
		ShowWindow(GetDlgItem(window, IDC_EVENT_FONTNAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EVENT_FONT_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EVENT_SELECT), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EVENT_FONTCOLOR), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EVENT_FONT_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EVENT_FONTCOLOR_STATIC), SW_HIDE);
	}

	// Update font name
	char tmpSz[256];
	HWND widget = GetDlgItem(window, IDC_EVENT_FONTNAME);
	std::string name = g_EventFont.lfFaceName;
	name += " / ";
	HDC dc = GetWindowDC(window);
	int size = -MulDiv(g_EventFont.lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	ReleaseDC(window, dc);
	itoa(size, tmpSz, 10);
	name += tmpSz;
	SetWindowText(widget, name.c_str());

	// Update font name 2
	widget = GetDlgItem(window, IDC_EVENT_FONTNAME2);
	name = g_EventFont2.lfFaceName;
	name += " / ";
	dc = GetWindowDC(window);
	size = -MulDiv(g_EventFont2.lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	ReleaseDC(window, dc);
	itoa(size, tmpSz, 10);
	name += tmpSz;
	SetWindowText(widget, name.c_str());
}

BOOL OnInitEventDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	UINT state;
	
	widget = GetDlgItem(window, IDC_EVENT_ENABLE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 28));
	widget = GetDlgItem(window, IDC_EVENT_ALIGN_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 9));
	widget = GetDlgItem(window, IDC_EVENT_RASTERIZER_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 8));
	widget = GetDlgItem(window, IDC_EVENT_BITMAP_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 10));
	widget = GetDlgItem(window, IDC_EVENT_BITMAP_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 11));
	widget = GetDlgItem(window, IDC_EVENT_COMPONENTS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 12));
	widget = GetDlgItem(window, IDC_EVENT_SEPARATION_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 13));
	widget = GetDlgItem(window, IDC_EVENT_BROWSE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 3));
	widget = GetDlgItem(window, IDC_EVENT_FONT_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	widget = GetDlgItem(window, IDC_EVENT_FONT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 15));
	widget = GetDlgItem(window, IDC_EVENT_FONTCOLOR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 17));
	widget = GetDlgItem(window, IDC_EVENT_SELECT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 16));
	widget = GetDlgItem(window, IDC_EVENT_CALENDAR);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 29));
	widget = GetDlgItem(window, IDC_EVENT_COLOR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 17));
	widget = GetDlgItem(window, IDC_EVENT_FONT2_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 15));
	widget = GetDlgItem(window, IDC_EVENT_SELECT2);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 16));

	state = CConfig::Instance().GetEventEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENT_ENABLE, state);

	// Fill the combos
	SendDlgItemMessage(window, IDC_EVENT_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	SendDlgItemMessage(window, IDC_EVENT_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 10));

	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 35));
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 36));
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 37));
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 38));
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 39));
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 40));
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 41));
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 42));
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 43));

	state = InitRasterizer(CConfig::Instance().GetEventRasterizer());
	SendDlgItemMessage(window, IDC_EVENT_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(CConfig::Instance().GetEventAlign());
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_EVENT_BITMAP);
	SetWindowText(widget, CConfig::Instance().GetEventBitmapName().c_str());

	switch (CConfig::Instance().GetEventNumOfComponents())
	{
	case 1:
		state = IDC_EVENT_COMPONENTS_1;
		break;

	case 10:
		state = IDC_EVENT_COMPONENTS_10;
		break;

	case 32:
	default:
		state = IDC_EVENT_COMPONENTS_32;
		break;
	}
	CheckRadioButton(window, IDC_EVENT_COMPONENTS_1, IDC_EVENT_COMPONENTS_32, state);

	widget = GetDlgItem(window, IDC_EVENT_SEPARATION);
	itoa(CConfig::Instance().GetEventSeparation(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_EventFontColor = CConfig::Instance().GetEventFontColor();
	InitFont(CConfig::Instance().GetEventFont(), &g_EventFont);

	state = CConfig::Instance().GetEventInCalendar() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENT_CALENDAR, state);

	g_EventFontColor2 = CConfig::Instance().GetEventFontColor2();
	InitFont(CConfig::Instance().GetEventFont2(), &g_EventFont2);

	UpdateEventWidgets(window);

	return TRUE;
}

void OnOKEventDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_ENABLE));
	CConfig::Instance().SetEventEnable(state);

	widget = GetDlgItem(window, IDC_EVENT_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetEventBitmapName(tmpSz);

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_COMPONENTS_1))
	{
		CConfig::Instance().SetEventNumOfComponents(1);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_COMPONENTS_10))
	{
		CConfig::Instance().SetEventNumOfComponents(10);
	}
	else
	{
		CConfig::Instance().SetEventNumOfComponents(32);
	}

	widget = GetDlgItem(window, IDC_EVENT_SEPARATION);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetEventSeparation(atoi(tmpSz));

	CRasterizer::ALIGN align = GetAlign(window, IDC_EVENT_ALIGN);
	CConfig::Instance().SetEventAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_EVENT_RASTERIZER);
	CConfig::Instance().SetEventRasterizer(ras);

	CConfig::Instance().SetEventFontColor(g_EventFontColor);
	
	std::string fontStr = GetFontString(&g_EventFont);
	CConfig::Instance().SetEventFont(fontStr);

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_CALENDAR));
	CConfig::Instance().SetEventInCalendar(state);

	CConfig::Instance().SetEventFontColor2(g_EventFontColor2);
	
	fontStr = GetFontString(&g_EventFont2);
	CConfig::Instance().SetEventFont2(fontStr);
}

BOOL CALLBACK EventPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitEventDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				LPPSHNOTIFY pshNotify = (LPPSHNOTIFY)lParam;
				OnOKEventDialog(hwndDlg);
				if (pshNotify->lParam != TRUE)
				{
					CConfig::Instance().WriteConfig(CConfig::WRITE_SKIN);
					SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
					PropSheet_CancelToClose(GetParent(hwndDlg));
				}
				SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
				return TRUE;
			}
			break;

        case WM_DRAWITEM:
			if (wParam == IDC_EVENT_FONTCOLOR)
			{
				return DrawItemFontColor(g_EventFontColor, wParam, lParam);
			}
			if (wParam == IDC_EVENT_FONTCOLOR2)
			{
				return DrawItemFontColor(g_EventFontColor2, wParam, lParam);
			}
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_EVENT_BROWSE:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return BrowseBitmap(hwndDlg, IDC_EVENT_BITMAP);

			case IDC_EVENT_SELECT:
				SelectFont(hwndDlg, &g_EventFont);
				UpdateEventWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_EVENT_SELECT2:
				SelectFont(hwndDlg, &g_EventFont2);
				UpdateEventWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_EVENT_FONTCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_EventFontColor);

			case IDC_EVENT_FONTCOLOR2:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_EventFontColor2);

			case IDC_EVENT_CALENDAR:
			case IDC_EVENT_ENABLE:
			case IDC_EVENT_COMPONENTS_1:
			case IDC_EVENT_COMPONENTS_10:
			case IDC_EVENT_COMPONENTS_32:
				UpdateEventWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_EVENT_RASTERIZER:
				if (HIWORD(wParam) == CBN_SELCHANGE)
				{
					UpdateEventWidgets(hwndDlg);
					PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				}
				return TRUE;

			case IDC_EVENT_ALIGN:
			case IDC_EVENT_SEPARATION:
			case IDC_EVENT_BITMAP:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
			}
			break;

	}
    return FALSE; 
}

/****************************************************************************\ 
**
**                        W E E K D A Y S
**
\****************************************************************************/

void UpdateWeekdaysWidgets(HWND window)
{
	if (IsDlgButtonChecked(window, IDC_WEEKDAYS_ENABLE) == BST_CHECKED)
	{
		EnableWindow(GetDlgItem(window, IDC_WEEKDAYS_ALIGN), TRUE);
		EnableWindow(GetDlgItem(window, IDC_WEEKDAYS_RASTERIZER), TRUE);
		EnableWindow(GetDlgItem(window, IDC_WEEKDAYS_BITMAP), TRUE);
		EnableWindow(GetDlgItem(window, IDC_WEEKDAYS_BROWSE), TRUE);
		EnableWindow(GetDlgItem(window, IDC_WEEKDAYS_SELECT), TRUE);
		EnableWindow(GetDlgItem(window, IDC_WEEKDAYS_FONTCOLOR), TRUE);
	}
	else
	{
		// Disable all
		EnableWindow(GetDlgItem(window, IDC_WEEKDAYS_ALIGN), FALSE);
		EnableWindow(GetDlgItem(window, IDC_WEEKDAYS_RASTERIZER), FALSE);
		EnableWindow(GetDlgItem(window, IDC_WEEKDAYS_BITMAP), FALSE);
		EnableWindow(GetDlgItem(window, IDC_WEEKDAYS_BROWSE), FALSE);
		EnableWindow(GetDlgItem(window, IDC_WEEKDAYS_SELECT), FALSE);
		EnableWindow(GetDlgItem(window, IDC_WEEKDAYS_FONTCOLOR), FALSE);
	}

	// This is always disabled
	EnableWindow(GetDlgItem(window, IDC_WEEKDAYS_FONTNAME), FALSE);

	// Hide the Bitmap or Font stuff depending which rasterizer is in use
	int index = SendDlgItemMessage(window, IDC_WEEKDAYS_RASTERIZER, CB_GETCURSEL, NULL, NULL);
	if (index == 0)
	{
		// Hide Font show Bitmap
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_BITMAP_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_BITMAP), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_BROWSE), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_BITMAP_STATIC), SW_HIDE);
		
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_FONTNAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_FONT_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_SELECT), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_FONTCOLOR), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_FONT_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_FONTCOLOR_STATIC), SW_SHOW);
	}
	else
	{
		// Hide Bitmap show Font
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_BITMAP_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_BITMAP), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_BROWSE), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_BITMAP_STATIC), SW_SHOW);
		
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_FONTNAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_FONT_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_SELECT), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_FONTCOLOR), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_FONT_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKDAYS_FONTCOLOR_STATIC), SW_HIDE);
	}

	// Update font name
	char tmpSz[256];
	HWND widget = GetDlgItem(window, IDC_WEEKDAYS_FONTNAME);
	std::string name = g_WeekdaysFont.lfFaceName;
	name += " / ";
	HDC dc = GetWindowDC(window);
	int size = -MulDiv(g_WeekdaysFont.lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	ReleaseDC(window, dc);
	itoa(size, tmpSz, 10);
	name += tmpSz;
	SetWindowText(widget, name.c_str());

}

BOOL OnInitWeekdaysDialog(HWND window) 
{
	HWND widget;
	UINT state;

	widget = GetDlgItem(window, IDC_WEEKDAYS_ENABLE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 31));
	widget = GetDlgItem(window, IDC_WEEKDAYS_ALIGN_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 9));
	widget = GetDlgItem(window, IDC_WEEKDAYS_RASTERIZER_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 8));
	widget = GetDlgItem(window, IDC_WEEKDAYS_BITMAP_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 10));
	widget = GetDlgItem(window, IDC_WEEKDAYS_BITMAP_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 11));
	widget = GetDlgItem(window, IDC_WEEKDAYS_BROWSE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 3));
	widget = GetDlgItem(window, IDC_WEEKDAYS_FONT_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	widget = GetDlgItem(window, IDC_WEEKDAYS_FONT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 15));
	widget = GetDlgItem(window, IDC_WEEKDAYS_FONTCOLOR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 17));
	widget = GetDlgItem(window, IDC_WEEKDAYS_SELECT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 16));

	state = CConfig::Instance().GetWeekdaysEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_WEEKDAYS_ENABLE, state);

	// Fill the combos
	SendDlgItemMessage(window, IDC_WEEKDAYS_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	SendDlgItemMessage(window, IDC_WEEKDAYS_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 10));

	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 35));
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 36));
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 37));
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 38));
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 39));
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 40));
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 41));
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 42));
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 43));

	state = InitRasterizer(CConfig::Instance().GetWeekdaysRasterizer());
	SendDlgItemMessage(window, IDC_WEEKDAYS_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(CConfig::Instance().GetWeekdaysAlign());
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_WEEKDAYS_BITMAP);
	SetWindowText(widget, CConfig::Instance().GetWeekdaysBitmapName().c_str());

	g_WeekdaysFontColor = CConfig::Instance().GetWeekdaysFontColor();
	InitFont(CConfig::Instance().GetWeekdaysFont(), &g_WeekdaysFont);

	UpdateWeekdaysWidgets(window);

	return TRUE;
}

void OnOKWeekdaysDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WEEKDAYS_ENABLE));
	CConfig::Instance().SetWeekdaysEnable(state);

	widget = GetDlgItem(window, IDC_WEEKDAYS_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetWeekdaysBitmapName(tmpSz);

	CRasterizer::ALIGN align = GetAlign(window, IDC_WEEKDAYS_ALIGN);
	CConfig::Instance().SetWeekdaysAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_WEEKDAYS_RASTERIZER);
	CConfig::Instance().SetWeekdaysRasterizer(ras);

	CConfig::Instance().SetWeekdaysFontColor(g_WeekdaysFontColor);
	
	std::string fontStr = GetFontString(&g_WeekdaysFont);
	CConfig::Instance().SetWeekdaysFont(fontStr);
}

BOOL CALLBACK WeekdaysPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitWeekdaysDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				LPPSHNOTIFY pshNotify = (LPPSHNOTIFY)lParam;
				OnOKWeekdaysDialog(hwndDlg);
				if (pshNotify->lParam != TRUE)
				{
					CConfig::Instance().WriteConfig(CConfig::WRITE_SKIN);
					SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
					PropSheet_CancelToClose(GetParent(hwndDlg));
				}
				SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
				return TRUE;
			}
			break;

        case WM_DRAWITEM:
			if (wParam == IDC_WEEKDAYS_FONTCOLOR)
			{
				return DrawItemFontColor(g_WeekdaysFontColor, wParam, lParam);
			}
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_WEEKDAYS_BROWSE:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return BrowseBitmap(hwndDlg, IDC_WEEKDAYS_BITMAP);

			case IDC_WEEKDAYS_SELECT:
				SelectFont(hwndDlg, &g_WeekdaysFont);
				UpdateWeekdaysWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_WEEKDAYS_FONTCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_WeekdaysFontColor);

			case IDC_WEEKDAYS_ENABLE:
				UpdateWeekdaysWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_WEEKDAYS_RASTERIZER:
				if (HIWORD(wParam) == CBN_SELCHANGE)
				{
					UpdateWeekdaysWidgets(hwndDlg);
					PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				}
				return TRUE;

			case IDC_WEEKDAYS_ALIGN:
			case IDC_WEEKDAYS_BITMAP:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
			}
			break;

	}
    return FALSE; 
}

/****************************************************************************\ 
**
**                     W E E K N U M B E R S
**
\****************************************************************************/

void UpdateWeekNumbersWidgets(HWND window)
{
	if (IsDlgButtonChecked(window, IDC_WEEKNUM_ENABLE) == BST_CHECKED)
	{
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_ALIGN), TRUE);
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_RASTERIZER), TRUE);
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_BITMAP), TRUE);
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_BROWSE), TRUE);
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_COMPONENTS_10), TRUE); 
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_COMPONENTS_54), TRUE);
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_SELECT), TRUE);
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_FONTCOLOR), TRUE);

		if (IsDlgButtonChecked(window, IDC_WEEKNUM_COMPONENTS_10) == BST_CHECKED)
		{
			EnableWindow(GetDlgItem(window, IDC_WEEKNUM_SEPARATION), TRUE);
		}
		else
		{
			EnableWindow(GetDlgItem(window, IDC_WEEKNUM_SEPARATION), FALSE);
		}
	}
	else
	{
		// Disable all
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_ALIGN), FALSE);
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_RASTERIZER), FALSE);
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_BITMAP), FALSE);
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_BROWSE), FALSE);
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_COMPONENTS_10), FALSE); 
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_COMPONENTS_54), FALSE);
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_SEPARATION), FALSE);
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_SELECT), FALSE);
		EnableWindow(GetDlgItem(window, IDC_WEEKNUM_FONTCOLOR), FALSE);
	}

	// This is always disabled
	EnableWindow(GetDlgItem(window, IDC_WEEKNUM_FONTNAME), FALSE);

	// Hide the Bitmap or Font stuff depending which rasterizer is in use
	int index = SendDlgItemMessage(window, IDC_WEEKNUM_RASTERIZER, CB_GETCURSEL, NULL, NULL);
	if (index == 0)
	{
		// Hide Font show Bitmap
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_BITMAP_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_SEPARATION), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_BITMAP), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_COMPONENTS_10), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_COMPONENTS_54), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_BROWSE), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_COMPONENTS_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_SEPARATION_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_BITMAP_STATIC), SW_HIDE);
		
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_FONTNAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_FONT_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_SELECT), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_FONTCOLOR), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_FONT_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_FONTCOLOR_STATIC), SW_SHOW);
	}
	else
	{
		// Hide Bitmap show Font
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_BITMAP_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_SEPARATION), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_BITMAP), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_COMPONENTS_10), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_COMPONENTS_54), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_BROWSE), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_COMPONENTS_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_SEPARATION_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_BITMAP_STATIC), SW_SHOW);
		
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_FONTNAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_FONT_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_SELECT), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_FONTCOLOR), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_FONT_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_WEEKNUM_FONTCOLOR_STATIC), SW_HIDE);
	}

	// Update font name
	char tmpSz[256];
	HWND widget = GetDlgItem(window, IDC_WEEKNUM_FONTNAME);
	std::string name = g_WeekNumbersFont.lfFaceName;
	name += " / ";
	HDC dc = GetWindowDC(window);
	int size = -MulDiv(g_WeekNumbersFont.lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	ReleaseDC(window, dc);
	itoa(size, tmpSz, 10);
	name += tmpSz;
	SetWindowText(widget, name.c_str());

}

BOOL OnInitWeekNumbersDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	UINT state;

	widget = GetDlgItem(window, IDC_WEEKNUM_ENABLE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 32));
	widget = GetDlgItem(window, IDC_WEEKNUM_ALIGN_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 9));
	widget = GetDlgItem(window, IDC_WEEKNUM_RASTERIZER_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 8));
	widget = GetDlgItem(window, IDC_WEEKNUM_BITMAP_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 10));
	widget = GetDlgItem(window, IDC_WEEKNUM_BITMAP_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 11));
	widget = GetDlgItem(window, IDC_WEEKNUM_COMPONENTS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 12));
	widget = GetDlgItem(window, IDC_WEEKNUM_SEPARATION_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 13));
	widget = GetDlgItem(window, IDC_WEEKNUM_BROWSE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 3));
	widget = GetDlgItem(window, IDC_WEEKNUM_FONT_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	widget = GetDlgItem(window, IDC_WEEKNUM_FONT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 15));
	widget = GetDlgItem(window, IDC_WEEKNUM_FONTCOLOR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 17));
	widget = GetDlgItem(window, IDC_WEEKNUM_SELECT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 16));

	state = CConfig::Instance().GetWeekNumbersEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_WEEKNUM_ENABLE, state);

	// Fill the combos
	SendDlgItemMessage(window, IDC_WEEKNUM_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	SendDlgItemMessage(window, IDC_WEEKNUM_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 10));

	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 35));
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 36));
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 37));
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 38));
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 39));
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 40));
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 41));
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 42));
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 43));

	state = InitRasterizer(CConfig::Instance().GetWeekNumbersRasterizer());
	SendDlgItemMessage(window, IDC_WEEKNUM_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(CConfig::Instance().GetWeekNumbersAlign());
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_WEEKNUM_BITMAP);
	SetWindowText(widget, CConfig::Instance().GetWeekNumbersBitmapName().c_str());

	switch (CConfig::Instance().GetWeekNumbersNumOfComponents())
	{
	case 10:
		state = IDC_WEEKNUM_COMPONENTS_10;
		break;

	case 54:
	default:
		state = IDC_WEEKNUM_COMPONENTS_54;
		break;
	}
	CheckRadioButton(window, IDC_WEEKNUM_COMPONENTS_10, IDC_WEEKNUM_COMPONENTS_54, state);

	widget = GetDlgItem(window, IDC_WEEKNUM_SEPARATION);
	itoa(CConfig::Instance().GetWeekNumbersSeparation(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_WeekNumbersFontColor = CConfig::Instance().GetWeekNumbersFontColor();
	InitFont(CConfig::Instance().GetWeekNumbersFont(), &g_WeekNumbersFont);

	UpdateWeekNumbersWidgets(window);

	return TRUE;
}

void OnOKWeekNumbersDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WEEKNUM_ENABLE));
	CConfig::Instance().SetWeekNumbersEnable(state);

	widget = GetDlgItem(window, IDC_WEEKNUM_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetWeekNumbersBitmapName(tmpSz);

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_WEEKNUM_COMPONENTS_10))
	{
		CConfig::Instance().SetWeekNumbersNumOfComponents(10);
	}
	else
	{
		CConfig::Instance().SetWeekNumbersNumOfComponents(54);
	}

	widget = GetDlgItem(window, IDC_WEEKNUM_SEPARATION);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetWeekNumbersSeparation(atoi(tmpSz));

	CRasterizer::ALIGN align = GetAlign(window, IDC_WEEKNUM_ALIGN);
	CConfig::Instance().SetWeekNumbersAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_WEEKNUM_RASTERIZER);
	CConfig::Instance().SetWeekNumbersRasterizer(ras);

	CConfig::Instance().SetWeekNumbersFontColor(g_WeekNumbersFontColor);
	
	std::string fontStr = GetFontString(&g_WeekNumbersFont);
	CConfig::Instance().SetWeekNumbersFont(fontStr);
}

BOOL CALLBACK WeekNumbersPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitWeekNumbersDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				LPPSHNOTIFY pshNotify = (LPPSHNOTIFY)lParam;
				OnOKWeekNumbersDialog(hwndDlg);
				if (pshNotify->lParam != TRUE)
				{
					CConfig::Instance().WriteConfig(CConfig::WRITE_SKIN);
					SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
					PropSheet_CancelToClose(GetParent(hwndDlg));
				}
				SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
				return TRUE;
			}
			break;

        case WM_DRAWITEM:
			if (wParam == IDC_WEEKNUM_FONTCOLOR)
			{
				return DrawItemFontColor(g_WeekNumbersFontColor, wParam, lParam);
			}
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_WEEKNUM_BROWSE:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return BrowseBitmap(hwndDlg, IDC_WEEKNUM_BITMAP);

			case IDC_WEEKNUM_SELECT:
				SelectFont(hwndDlg, &g_WeekNumbersFont);
				UpdateWeekNumbersWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_WEEKNUM_FONTCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_WeekNumbersFontColor);

			case IDC_WEEKNUM_ENABLE:
			case IDC_WEEKNUM_COMPONENTS_10:
			case IDC_WEEKNUM_COMPONENTS_54:
				UpdateWeekNumbersWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_WEEKNUM_RASTERIZER:
				if (HIWORD(wParam) == CBN_SELCHANGE)
				{
					UpdateWeekNumbersWidgets(hwndDlg);
					PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				}
				return TRUE;

			case IDC_WEEKNUM_ALIGN:
			case IDC_WEEKNUM_SEPARATION:
			case IDC_WEEKNUM_BITMAP:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
			}
			break;

	}
    return FALSE; 
}

/****************************************************************************\ 
**
**                           M O N T H
**
\****************************************************************************/

void UpdateMonthWidgets(HWND window)
{
	if (IsDlgButtonChecked(window, IDC_MONTH_ENABLE) == BST_CHECKED)
	{
		EnableWindow(GetDlgItem(window, IDC_MONTH_X), TRUE);
		EnableWindow(GetDlgItem(window, IDC_MONTH_Y), TRUE);
		EnableWindow(GetDlgItem(window, IDC_MONTH_ALIGN), TRUE);
		EnableWindow(GetDlgItem(window, IDC_MONTH_RASTERIZER), TRUE);
		EnableWindow(GetDlgItem(window, IDC_MONTH_BITMAP), TRUE);
		EnableWindow(GetDlgItem(window, IDC_MONTH_BROWSE), TRUE);
		EnableWindow(GetDlgItem(window, IDC_MONTH_SELECT), TRUE);
		EnableWindow(GetDlgItem(window, IDC_MONTH_FONTCOLOR), TRUE);
	}
	else
	{
		// Disable all
		EnableWindow(GetDlgItem(window, IDC_MONTH_X), FALSE);
		EnableWindow(GetDlgItem(window, IDC_MONTH_Y), FALSE);
		EnableWindow(GetDlgItem(window, IDC_MONTH_ALIGN), FALSE);
		EnableWindow(GetDlgItem(window, IDC_MONTH_RASTERIZER), FALSE);
		EnableWindow(GetDlgItem(window, IDC_MONTH_BITMAP), FALSE);
		EnableWindow(GetDlgItem(window, IDC_MONTH_BROWSE), FALSE);
		EnableWindow(GetDlgItem(window, IDC_MONTH_SELECT), FALSE);
		EnableWindow(GetDlgItem(window, IDC_MONTH_FONTCOLOR), FALSE);
	}

	// This is always disabled
	EnableWindow(GetDlgItem(window, IDC_MONTH_FONTNAME), FALSE);

	// Hide the Bitmap or Font stuff depending which rasterizer is in use
	int index = SendDlgItemMessage(window, IDC_MONTH_RASTERIZER, CB_GETCURSEL, NULL, NULL);
	if (index == 0)
	{
		// Hide Font show Bitmap
		ShowWindow(GetDlgItem(window, IDC_MONTH_BITMAP_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_MONTH_BITMAP), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_MONTH_BROWSE), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_MONTH_BITMAP_STATIC), SW_HIDE);
		
		ShowWindow(GetDlgItem(window, IDC_MONTH_FONTNAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_MONTH_FONT_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_MONTH_SELECT), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_MONTH_FONTCOLOR), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_MONTH_FONT_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_MONTH_FONTCOLOR_STATIC), SW_SHOW);
	}
	else
	{
		// Hide Bitmap show Font
		ShowWindow(GetDlgItem(window, IDC_MONTH_BITMAP_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_MONTH_BITMAP), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_MONTH_BROWSE), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_MONTH_BITMAP_STATIC), SW_SHOW);
		
		ShowWindow(GetDlgItem(window, IDC_MONTH_FONTNAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_MONTH_FONT_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_MONTH_SELECT), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_MONTH_FONTCOLOR), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_MONTH_FONT_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_MONTH_FONTCOLOR_STATIC), SW_HIDE);
	}

	// Update font name
	char tmpSz[256];
	HWND widget = GetDlgItem(window, IDC_MONTH_FONTNAME);
	std::string name = g_MonthFont.lfFaceName;
	name += " / ";
	HDC dc = GetWindowDC(window);
	int size = -MulDiv(g_MonthFont.lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	ReleaseDC(window, dc);
	itoa(size, tmpSz, 10);
	name += tmpSz;
	SetWindowText(widget, name.c_str());

}

BOOL OnInitMonthDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	UINT state;

	widget = GetDlgItem(window, IDC_MONTH_ENABLE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 33));
	widget = GetDlgItem(window, IDC_MONTH_X_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 4));
	widget = GetDlgItem(window, IDC_MONTH_Y_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 5));
	widget = GetDlgItem(window, IDC_MONTH_ALIGN_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 9));
	widget = GetDlgItem(window, IDC_MONTH_RASTERIZER_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 8));
	widget = GetDlgItem(window, IDC_MONTH_BITMAP_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 10));
	widget = GetDlgItem(window, IDC_MONTH_BITMAP_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 11));
	widget = GetDlgItem(window, IDC_MONTH_BROWSE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 3));
	widget = GetDlgItem(window, IDC_MONTH_FONT_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	widget = GetDlgItem(window, IDC_MONTH_FONT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 15));
	widget = GetDlgItem(window, IDC_MONTH_FONTCOLOR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 17));
	widget = GetDlgItem(window, IDC_MONTH_SELECT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 16));

	state = CConfig::Instance().GetMonthEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_MONTH_ENABLE, state);

	widget = GetDlgItem(window, IDC_MONTH_X);
	itoa(CConfig::Instance().GetMonthX(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_MONTH_Y);
	itoa(CConfig::Instance().GetMonthY(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	// Fill the combos
	SendDlgItemMessage(window, IDC_MONTH_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	SendDlgItemMessage(window, IDC_MONTH_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 10));

	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 35));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 36));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 37));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 38));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 39));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 40));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 41));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 42));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 43));
	
	state = InitRasterizer(CConfig::Instance().GetMonthRasterizer());
	SendDlgItemMessage(window, IDC_MONTH_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(CConfig::Instance().GetMonthAlign());
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_MONTH_BITMAP);
	SetWindowText(widget, CConfig::Instance().GetMonthBitmapName().c_str());

	g_MonthFontColor = CConfig::Instance().GetMonthFontColor();
	InitFont(CConfig::Instance().GetMonthFont(), &g_MonthFont);

	UpdateMonthWidgets(window);

	return TRUE;
}

void OnOKMonthDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_MONTH_ENABLE));
	CConfig::Instance().SetMonthEnable(state);

	widget = GetDlgItem(window, IDC_MONTH_X);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetMonthX(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_MONTH_Y);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetMonthY(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_MONTH_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetMonthBitmapName(tmpSz);

	CRasterizer::ALIGN align = GetAlign(window, IDC_MONTH_ALIGN);
	CConfig::Instance().SetMonthAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_MONTH_RASTERIZER);
	CConfig::Instance().SetMonthRasterizer(ras);

	CConfig::Instance().SetMonthFontColor(g_MonthFontColor);
	
	std::string fontStr = GetFontString(&g_MonthFont);
	CConfig::Instance().SetMonthFont(fontStr);
}

BOOL CALLBACK MonthPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitMonthDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				LPPSHNOTIFY pshNotify = (LPPSHNOTIFY)lParam;
				OnOKMonthDialog(hwndDlg);
				if (pshNotify->lParam != TRUE)
				{
					CConfig::Instance().WriteConfig(CConfig::WRITE_SKIN);
					SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
					PropSheet_CancelToClose(GetParent(hwndDlg));
				}
				SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
				return TRUE;
			}
			break;

        case WM_DRAWITEM:
			if (wParam == IDC_MONTH_FONTCOLOR)
			{
				return DrawItemFontColor(g_MonthFontColor, wParam, lParam);
			}
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_MONTH_BROWSE:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return BrowseBitmap(hwndDlg, IDC_MONTH_BITMAP);

			case IDC_MONTH_SELECT:
				SelectFont(hwndDlg, &g_MonthFont);
				UpdateMonthWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_MONTH_FONTCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_MonthFontColor);

			case IDC_MONTH_ENABLE:
				UpdateMonthWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_MONTH_RASTERIZER:
				if (HIWORD(wParam) == CBN_SELCHANGE)
				{
					UpdateMonthWidgets(hwndDlg);
					PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				}
				return TRUE;

			case IDC_MONTH_X:
			case IDC_MONTH_Y:
			case IDC_MONTH_ALIGN:
			case IDC_MONTH_BITMAP:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
			}
			break;

	}
    return FALSE; 
}

/****************************************************************************\ 
**
**                           Y E A R
**
\****************************************************************************/

void UpdateYearWidgets(HWND window)
{
	if (IsDlgButtonChecked(window, IDC_YEAR_ENABLE) == BST_CHECKED)
	{
		EnableWindow(GetDlgItem(window, IDC_YEAR_X), TRUE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_Y), TRUE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_ALIGN), TRUE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_RASTERIZER), TRUE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_BITMAP), TRUE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_BROWSE), TRUE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_SELECT), TRUE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_FONTCOLOR), TRUE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_SEPARATION), TRUE);
	}
	else
	{
		// Disable all
		EnableWindow(GetDlgItem(window, IDC_YEAR_X), FALSE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_Y), FALSE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_ALIGN), FALSE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_RASTERIZER), FALSE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_BITMAP), FALSE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_BROWSE), FALSE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_SEPARATION), FALSE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_SELECT), FALSE);
		EnableWindow(GetDlgItem(window, IDC_YEAR_FONTCOLOR), FALSE);
	}

	// This is always disabled
	EnableWindow(GetDlgItem(window, IDC_YEAR_FONTNAME), FALSE);

	// Hide the Bitmap or Font stuff depending which rasterizer is in use
	int index = SendDlgItemMessage(window, IDC_YEAR_RASTERIZER, CB_GETCURSEL, NULL, NULL);
	if (index == 0)
	{
		// Hide Font show Bitmap
		ShowWindow(GetDlgItem(window, IDC_YEAR_BITMAP_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_YEAR_SEPARATION), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_YEAR_BITMAP), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_YEAR_BROWSE), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_YEAR_SEPARATION_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_YEAR_BITMAP_STATIC), SW_HIDE);
		
		ShowWindow(GetDlgItem(window, IDC_YEAR_FONTNAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_YEAR_FONT_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_YEAR_SELECT), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_YEAR_FONTCOLOR), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_YEAR_FONT_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_YEAR_FONTCOLOR_STATIC), SW_SHOW);
	}
	else
	{
		// Hide Bitmap show Font
		ShowWindow(GetDlgItem(window, IDC_YEAR_BITMAP_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_YEAR_SEPARATION), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_YEAR_BITMAP), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_YEAR_BROWSE), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_YEAR_SEPARATION_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_YEAR_BITMAP_STATIC), SW_SHOW);
		
		ShowWindow(GetDlgItem(window, IDC_YEAR_FONTNAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_YEAR_FONT_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_YEAR_SELECT), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_YEAR_FONTCOLOR), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_YEAR_FONT_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_YEAR_FONTCOLOR_STATIC), SW_HIDE);
	}

	// Update font name
	char tmpSz[256];
	HWND widget = GetDlgItem(window, IDC_YEAR_FONTNAME);
	std::string name = g_YearFont.lfFaceName;
	name += " / ";
	HDC dc = GetWindowDC(window);
	int size = -MulDiv(g_YearFont.lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	ReleaseDC(window, dc);
	itoa(size, tmpSz, 10);
	name += tmpSz;
	SetWindowText(widget, name.c_str());

}

BOOL OnInitYearDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	UINT state;

	widget = GetDlgItem(window, IDC_YEAR_ENABLE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 34));
	widget = GetDlgItem(window, IDC_YEAR_X_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 4));
	widget = GetDlgItem(window, IDC_YEAR_Y_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 5));
	widget = GetDlgItem(window, IDC_YEAR_ALIGN_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 9));
	widget = GetDlgItem(window, IDC_YEAR_RASTERIZER_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 8));
	widget = GetDlgItem(window, IDC_YEAR_BITMAP_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 10));
	widget = GetDlgItem(window, IDC_YEAR_BITMAP_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 11));
	widget = GetDlgItem(window, IDC_YEAR_BROWSE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 3));
	widget = GetDlgItem(window, IDC_YEAR_SEPARATION_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 13));
	widget = GetDlgItem(window, IDC_YEAR_FONT_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	widget = GetDlgItem(window, IDC_YEAR_FONT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 15));
	widget = GetDlgItem(window, IDC_YEAR_FONTCOLOR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 17));
	widget = GetDlgItem(window, IDC_YEAR_SELECT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 16));

	state = CConfig::Instance().GetYearEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_YEAR_ENABLE, state);

	widget = GetDlgItem(window, IDC_YEAR_X);
	itoa(CConfig::Instance().GetYearX(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_YEAR_Y);
	itoa(CConfig::Instance().GetYearY(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	// Fill the combos
	SendDlgItemMessage(window, IDC_YEAR_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	SendDlgItemMessage(window, IDC_YEAR_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 10));

	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 35));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 36));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 37));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 38));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 39));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 40));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 41));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 42));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 43));

	state = InitRasterizer(CConfig::Instance().GetYearRasterizer());
	SendDlgItemMessage(window, IDC_YEAR_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(CConfig::Instance().GetYearAlign());
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_YEAR_BITMAP);
	SetWindowText(widget, CConfig::Instance().GetYearBitmapName().c_str());

	widget = GetDlgItem(window, IDC_YEAR_SEPARATION);
	itoa(CConfig::Instance().GetYearSeparation(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_YearFontColor = CConfig::Instance().GetYearFontColor();
	InitFont(CConfig::Instance().GetYearFont(), &g_YearFont);

	UpdateYearWidgets(window);

	return TRUE;
}

void OnOKYearDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_YEAR_ENABLE));
	CConfig::Instance().SetYearEnable(state);

	widget = GetDlgItem(window, IDC_YEAR_X);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetYearX(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_YEAR_Y);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetYearY(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_YEAR_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetYearBitmapName(tmpSz);

	widget = GetDlgItem(window, IDC_YEAR_SEPARATION);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetYearSeparation(atoi(tmpSz));

	CRasterizer::ALIGN align = GetAlign(window, IDC_YEAR_ALIGN);
	CConfig::Instance().SetYearAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_YEAR_RASTERIZER);
	CConfig::Instance().SetYearRasterizer(ras);

	CConfig::Instance().SetYearFontColor(g_YearFontColor);
	
	std::string fontStr = GetFontString(&g_YearFont);
	CConfig::Instance().SetYearFont(fontStr);
}

BOOL CALLBACK YearPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitYearDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				LPPSHNOTIFY pshNotify = (LPPSHNOTIFY)lParam;
				OnOKYearDialog(hwndDlg);
				if (pshNotify->lParam != TRUE)
				{
					CConfig::Instance().WriteConfig(CConfig::WRITE_SKIN);
					SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
					PropSheet_CancelToClose(GetParent(hwndDlg));
				}
				SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
				return TRUE;
			}
			break;

        case WM_DRAWITEM:
			if (wParam == IDC_YEAR_FONTCOLOR)
			{
				return DrawItemFontColor(g_YearFontColor, wParam, lParam);
			}
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_YEAR_BROWSE:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return BrowseBitmap(hwndDlg, IDC_YEAR_BITMAP);

			case IDC_YEAR_SELECT:
				SelectFont(hwndDlg, &g_YearFont);
				UpdateYearWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_YEAR_FONTCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_YearFontColor);

			case IDC_YEAR_ENABLE:
				UpdateYearWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_YEAR_RASTERIZER:
				if (HIWORD(wParam) == CBN_SELCHANGE)
				{
					UpdateYearWidgets(hwndDlg);
					PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				}
				return TRUE;

			case IDC_YEAR_X:
			case IDC_YEAR_Y:
			case IDC_YEAR_ALIGN:
			case IDC_YEAR_SEPARATION:
			case IDC_YEAR_BITMAP:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
			}
			break;

	}
    return FALSE; 
}

/****************************************************************************\ 
**
**                           T O D O
**
\****************************************************************************/

void UpdateTodoWidgets(HWND window)
{
	// Update font name
	char tmpSz[256];
	HWND widget = GetDlgItem(window, IDC_TODO_FONTNAME);
	std::string name = g_TodoFont.lfFaceName;
	name += " / ";
	HDC dc = GetWindowDC(window);
	int size = -MulDiv(g_TodoFont.lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	ReleaseDC(window, dc);
	itoa(size, tmpSz, 10);
	name += tmpSz;
	SetWindowText(widget, name.c_str());
}

BOOL OnInitTodoDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	UINT state;
	
	widget = GetDlgItem(window, IDC_TODO_BITMAP_BACKGROUND_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 21));
	widget = GetDlgItem(window, IDC_TODO_WIDTH_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 44));
	widget = GetDlgItem(window, IDC_TODO_BITMAP_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 11));
	widget = GetDlgItem(window, IDC_TODO_BITMAP_MARGINS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 45));
	widget = GetDlgItem(window, IDC_TODO_TEXT_MARGINS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 46));
	widget = GetDlgItem(window, IDC_TODO_BROWSE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 3));

	widget = GetDlgItem(window, IDC_TODO_ITEM_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 47));
	widget = GetDlgItem(window, IDC_TODO_ITEM_ALIGN_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 9));
	widget = GetDlgItem(window, IDC_TODO_ITEM_BITMAP_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 11));
	widget = GetDlgItem(window, IDC_TODO_ITEM_OFFSET_X_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 48));
	widget = GetDlgItem(window, IDC_TODO_ITEM_OFFSET_Y_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 5));
	widget = GetDlgItem(window, IDC_TODO_ITEM_BITMAP_BROWSE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 3));

	widget = GetDlgItem(window, IDC_TODO_TEXT_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 49));
	widget = GetDlgItem(window, IDC_TODO_SELECT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 16));
	widget = GetDlgItem(window, IDC_TODO_FONT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 15));
	widget = GetDlgItem(window, IDC_TODO_FONTCOLOR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 17));

	widget = GetDlgItem(window, IDC_TODO_WIDTH);
	itoa(CConfig::Instance().GetTodoW(), tmpSz, 10);
	SetWindowText(widget, tmpSz);
	
	widget = GetDlgItem(window, IDC_TODO_BITMAP);
	SetWindowText(widget, CConfig::Instance().GetTodoBitmapName().c_str());

	RECT margins = CConfig::Instance().GetTodoBitmapMargins();
	sprintf(tmpSz, "%i, %i, %i, %i", margins.left, margins.top, margins.right, margins.bottom);
	widget = GetDlgItem(window, IDC_TODO_BITMAP_MARGINS);
	SetWindowText(widget, tmpSz);

	margins = CConfig::Instance().GetTodoTextMargins();
	sprintf(tmpSz, "%i, %i, %i, %i", margins.left, margins.top, margins.right, margins.bottom);
	widget = GetDlgItem(window, IDC_TODO_TEXT_MARGINS);
	SetWindowText(widget, tmpSz);

	// Fill the combos
	SendDlgItemMessage(window, IDC_TODO_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 35));
	SendDlgItemMessage(window, IDC_TODO_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 36));
	SendDlgItemMessage(window, IDC_TODO_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 37));
	SendDlgItemMessage(window, IDC_TODO_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 38));
	SendDlgItemMessage(window, IDC_TODO_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 39));
	SendDlgItemMessage(window, IDC_TODO_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 40));
	SendDlgItemMessage(window, IDC_TODO_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 41));
	SendDlgItemMessage(window, IDC_TODO_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 42));
	SendDlgItemMessage(window, IDC_TODO_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 43));

	state = InitAlign(CConfig::Instance().GetTodoItemAlign());
	SendDlgItemMessage(window, IDC_TODO_ITEM_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_TODO_ITEM_BITMAP);
	SetWindowText(widget, CConfig::Instance().GetTodoItemBitmapName().c_str());

	POINT offset = CConfig::Instance().GetTodoItemOffset();
	widget = GetDlgItem(window, IDC_TODO_ITEM_OFFSET_X);
	itoa(offset.x, tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_TODO_ITEM_OFFSET_Y);
	itoa(offset.y, tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_TodoFontColor = CConfig::Instance().GetTodoFontColor();
	InitFont(CConfig::Instance().GetTodoFont(), &g_TodoFont);

	UpdateTodoWidgets(window);

	return TRUE;
}

void OnOKTodoDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	RECT margins;
	POINT offset;

	widget = GetDlgItem(window, IDC_TODO_WIDTH);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetTodoW(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_TODO_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetTodoBitmapName(tmpSz);

	margins.left = margins.top = margins.right = margins.bottom = 0;
	widget = GetDlgItem(window, IDC_TODO_BITMAP_MARGINS);
	GetWindowText(widget, tmpSz, 256);
	sscanf(tmpSz, "%i,%i,%i,%i", &margins.left, &margins.top, &margins.right, &margins.bottom);
	CConfig::Instance().SetTodoBitmapMargins(margins);

	margins.left = margins.top = margins.right = margins.bottom = 0;
	widget = GetDlgItem(window, IDC_TODO_TEXT_MARGINS);
	GetWindowText(widget, tmpSz, 256);
	sscanf(tmpSz, "%i,%i,%i,%i", &margins.left, &margins.top, &margins.right, &margins.bottom);
	CConfig::Instance().SetTodoTextMargins(margins);

	CRasterizer::ALIGN align = GetAlign(window, IDC_TODO_ITEM_ALIGN);
	CConfig::Instance().SetTodoItemAlign(align);

	widget = GetDlgItem(window, IDC_TODO_ITEM_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetTodoItemBitmapName(tmpSz);
	
	widget = GetDlgItem(window, IDC_TODO_ITEM_OFFSET_X);
	GetWindowText(widget, tmpSz, 256);
	offset.x = atoi(tmpSz);

	widget = GetDlgItem(window, IDC_TODO_ITEM_OFFSET_Y);
	GetWindowText(widget, tmpSz, 256);
	offset.y = atoi(tmpSz);
	CConfig::Instance().SetTodoItemOffset(offset);

	std::string fontStr = GetFontString(&g_TodoFont);
	CConfig::Instance().SetTodoFont(fontStr);

	CConfig::Instance().SetTodoFontColor(g_TodoFontColor);
}

BOOL CALLBACK TodoPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitTodoDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				LPPSHNOTIFY pshNotify = (LPPSHNOTIFY)lParam;
				OnOKTodoDialog(hwndDlg);
				if (pshNotify->lParam != TRUE)
				{
					CConfig::Instance().WriteConfig(CConfig::WRITE_SKIN);
					SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
					PropSheet_CancelToClose(GetParent(hwndDlg));
				}
				SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
				return TRUE;
			}
			break;

        case WM_DRAWITEM:
			if (wParam == IDC_TODO_FONTCOLOR)
			{
				return DrawItemFontColor(g_TodoFontColor, wParam, lParam);
			}
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_TODO_BROWSE:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return BrowseBitmap(hwndDlg, IDC_TODO_BITMAP);

			case IDC_TODO_ITEM_BITMAP_BROWSE:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return BrowseBitmap(hwndDlg, IDC_TODO_ITEM_BITMAP);
				
			case IDC_TODO_SELECT:
				SelectFont(hwndDlg, &g_TodoFont);
				UpdateTodoWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_TODO_FONTCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_TodoFontColor);

			case IDC_TODO_BITMAP:
			case IDC_TODO_ITEM_BITMAP:
			case IDC_TODO_BITMAP_MARGINS:
			case IDC_TODO_TEXT_MARGINS:
			case IDC_TODO_ITEM_ALIGN:
			case IDC_TODO_ITEM_OFFSET_X:
			case IDC_TODO_ITEM_OFFSET_Y:
			case IDC_TODO_WIDTH:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
			}
			break;

	}
    return FALSE; 
}

/****************************************************************************\ 
**
**                       M E S S A G E B O X
**
\****************************************************************************/

void UpdateMessageBoxWidgets(HWND window)
{
	// Update font name
	char tmpSz[256];
	HWND widget = GetDlgItem(window, IDC_MESSAGEBOX_FONTNAME);
	std::string name = g_MessageBoxFont.lfFaceName;
	name += " / ";
	HDC dc = GetWindowDC(window);
	int size = -MulDiv(g_MessageBoxFont.lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	ReleaseDC(window, dc);
	itoa(size, tmpSz, 10);
	name += tmpSz;
	SetWindowText(widget, name.c_str());
}

BOOL OnInitMessageBoxDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	
	widget = GetDlgItem(window, IDC_MESSAGEBOX_BITMAP_BACKGROUND_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 21));
	widget = GetDlgItem(window, IDC_MESSAGEBOX_BITMAP_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 11));
	widget = GetDlgItem(window, IDC_MESSAGEBOX_BITMAP_MARGINS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 45));
	widget = GetDlgItem(window, IDC_MESSAGEBOX_TEXT_MARGINS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 46));
	widget = GetDlgItem(window, IDC_MESSAGEBOX_BROWSE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 3));

	widget = GetDlgItem(window, IDC_MESSAGEBOX_TEXT_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 50));
	widget = GetDlgItem(window, IDC_MESSAGEBOX_SELECT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 16));
	widget = GetDlgItem(window, IDC_MESSAGEBOX_FONT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 15));
	widget = GetDlgItem(window, IDC_MESSAGEBOX_FONTCOLOR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 17));

	widget = GetDlgItem(window, IDC_MESSAGEBOX_BITMAP);
	SetWindowText(widget, CConfig::Instance().GetMessageBoxBitmapName().c_str());

	RECT margins = CConfig::Instance().GetMessageBoxBitmapMargins();
	sprintf(tmpSz, "%i, %i, %i, %i", margins.left, margins.top, margins.right, margins.bottom);
	widget = GetDlgItem(window, IDC_MESSAGEBOX_BITMAP_MARGINS);
	SetWindowText(widget, tmpSz);

	margins = CConfig::Instance().GetMessageBoxTextMargins();
	sprintf(tmpSz, "%i, %i, %i, %i", margins.left, margins.top, margins.right, margins.bottom);
	widget = GetDlgItem(window, IDC_MESSAGEBOX_TEXT_MARGINS);
	SetWindowText(widget, tmpSz);

	g_MessageBoxFontColor = CConfig::Instance().GetMessageBoxFontColor();
	InitFont(CConfig::Instance().GetMessageBoxFont(), &g_MessageBoxFont);

	UpdateMessageBoxWidgets(window);

	return TRUE;
}

void OnOKMessageBoxDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	RECT margins;

	widget = GetDlgItem(window, IDC_MESSAGEBOX_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CConfig::Instance().SetMessageBoxBitmapName(tmpSz);

	margins.left = margins.top = margins.right = margins.bottom = 0;
	widget = GetDlgItem(window, IDC_MESSAGEBOX_BITMAP_MARGINS);
	GetWindowText(widget, tmpSz, 256);
	sscanf(tmpSz, "%i,%i,%i,%i", &margins.left, &margins.top, &margins.right, &margins.bottom);
	CConfig::Instance().SetMessageBoxBitmapMargins(margins);

	margins.left = margins.top = margins.right = margins.bottom = 0;
	widget = GetDlgItem(window, IDC_MESSAGEBOX_TEXT_MARGINS);
	GetWindowText(widget, tmpSz, 256);
	sscanf(tmpSz, "%i,%i,%i,%i", &margins.left, &margins.top, &margins.right, &margins.bottom);
	CConfig::Instance().SetMessageBoxTextMargins(margins);

	std::string fontStr = GetFontString(&g_MessageBoxFont);
	CConfig::Instance().SetMessageBoxFont(fontStr);

	CConfig::Instance().SetMessageBoxFontColor(g_MessageBoxFontColor);
}

BOOL CALLBACK MessageBoxPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitMessageBoxDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				LPPSHNOTIFY pshNotify = (LPPSHNOTIFY)lParam;
				OnOKMessageBoxDialog(hwndDlg);
				if (pshNotify->lParam != TRUE)
				{
					CConfig::Instance().WriteConfig(CConfig::WRITE_SKIN);
					SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
					PropSheet_CancelToClose(GetParent(hwndDlg));
				}
				SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
				return TRUE;
			}
			break;

        case WM_DRAWITEM:
			if (wParam == IDC_MESSAGEBOX_FONTCOLOR)
			{
				return DrawItemFontColor(g_MessageBoxFontColor, wParam, lParam);
			}
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_MESSAGEBOX_BROWSE:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return BrowseBitmap(hwndDlg, IDC_MESSAGEBOX_BITMAP);
				
			case IDC_MESSAGEBOX_SELECT:
				SelectFont(hwndDlg, &g_MessageBoxFont);
				UpdateMessageBoxWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_MESSAGEBOX_FONTCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_MessageBoxFontColor);

			case IDC_MESSAGEBOX_BITMAP:
			case IDC_MESSAGEBOX_BITMAP_MARGINS:
			case IDC_MESSAGEBOX_TEXT_MARGINS:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
			}
			break;

	}
    return FALSE; 
}