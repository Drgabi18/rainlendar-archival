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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/SkinDialog.cpp,v 1.1 2002/08/03 16:37:08 rainy Exp $

  $Log: SkinDialog.cpp,v $
  Revision 1.1  2002/08/03 16:37:08  rainy
  Intial version.

*/

#pragma warning(disable: 4786)

#include "SkinDialog.h"
#include "RainlendarDLL.h"
#include <Prsht.h>
#include <Commdlg.h>
#include <list>

#define NUM_OF_TABS 8

// Few globals
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

BOOL CALLBACK BackgroundPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DaysPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TodayPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK EventPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK WeekdaysPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK WeekNumbersPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MonthPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK YearPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

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
			psp[i].pszTitle = "Background";
			break;

		case 1:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_DAYS);
			psp[i].pfnDlgProc = DaysPageProc;
			psp[i].pszTitle = "Days";
			break;

		case 2:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_TODAY);
			psp[i].pfnDlgProc = TodayPageProc;
			psp[i].pszTitle = "Today";
			break;

		case 3:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_EVENT);
			psp[i].pfnDlgProc = EventPageProc;
			psp[i].pszTitle = "Event";
			break;

		case 4:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_WEEKDAYS);
			psp[i].pfnDlgProc = WeekdaysPageProc;
			psp[i].pszTitle = "Weekdays";
			break;

		case 5:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_WEEKNUMBERS);
			psp[i].pfnDlgProc = WeekNumbersPageProc;
			psp[i].pszTitle = "Week Numbers";
			break;

		case 6:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_MONTH);
			psp[i].pfnDlgProc = MonthPageProc;
			psp[i].pszTitle = "Month";
			break;

		case 7:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_YEAR);
			psp[i].pfnDlgProc = YearPageProc;
			psp[i].pszTitle = "Year";
			break;
		}
	}

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOCONTEXTHELP ;
    psh.hwndParent = hwndOwner;
    psh.hInstance = instance;
    psh.pszIcon = NULL;
    psh.pszCaption = (LPSTR)"Skin Config";
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback = NULL;

	if (CCalendarWindow::c_Config.GetCurrentSkin().empty() || CCalendarWindow::c_Config.GetCurrentSkinIni().empty())
	{
		MessageBox(hwndOwner, "Sorry!\n\nSkin editing is only possible with separate skin configurations.\n\n" \
			                  "What you need to do is to remove all the skinning stuff from Rainlendar.ini\n" \
							  "and put them in a separate ini-file inside a subfolder under Rainlendar's folder.\n" \
							  "Consult the documentation for more info.", "Rainlendar", MB_OK);
	}
	else
	{
		if (PropertySheet(&psh) == IDOK)
		{
			CCalendarWindow::c_Config.WriteConfig(CConfig::WRITE_SKIN);
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
	sscanf(fontStr.c_str(), "%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i", 
		   &(logFont->lfHeight),
		   &(logFont->lfWidth),
		   &(logFont->lfEscapement),
		   &(logFont->lfOrientation),
		   &(logFont->lfWeight),
		   &(logFont->lfItalic), 
		   &(logFont->lfUnderline),
		   &(logFont->lfStrikeOut), 
		   &(logFont->lfCharSet),
		   &(logFont->lfOutPrecision),
		   &(logFont->lfClipPrecision), 
		   &(logFont->lfQuality),
		   &(logFont->lfPitchAndFamily));

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

	widget = GetDlgItem(window, IDC_BACKGROUND_FILENAME);
	SetWindowText(widget, CCalendarWindow::c_Config.GetBackgroundBitmapName().c_str());

	switch (CCalendarWindow::c_Config.GetBackgroundMode())
	{
	case CBackground::MODE_TILE:
		state = IDC_BACKGROUND_TILE;
		break;

	case CBackground::MODE_STRETCH:
		state = IDC_BACKGROUND_STRETCH;
		break;

	case CBackground::MODE_COPY:
	default:
		state = IDC_BACKGROUND_COPYWALLPAPER;
		break;
	}
	CheckRadioButton(window, IDC_BACKGROUND_TILE, IDC_BACKGROUND_COPYWALLPAPER, state);

	g_ToolTipFontColor = CCalendarWindow::c_Config.GetToolTipFontColor();
	g_ToolTipBGColor = CCalendarWindow::c_Config.GetToolTipBGColor();
	InitFont(CCalendarWindow::c_Config.GetToolTipFont(), &g_ToolTipFont);

	UpdateBackgroundWidgets(window);

	return TRUE;
}

void OnOKBackgroundDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;

	widget = GetDlgItem(window, IDC_BACKGROUND_FILENAME);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetBackgroundBitmapName(tmpSz);

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_BACKGROUND_STRETCH))
	{
		CCalendarWindow::c_Config.SetBackgroundMode(CBackground::MODE_STRETCH);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_BACKGROUND_TILE))
	{
		CCalendarWindow::c_Config.SetBackgroundMode(CBackground::MODE_TILE);
	}
	else
	{
		CCalendarWindow::c_Config.SetBackgroundMode(CBackground::MODE_COPY);
	}

	CCalendarWindow::c_Config.SetToolTipFontColor(g_ToolTipFontColor);
	CCalendarWindow::c_Config.SetToolTipBGColor(g_ToolTipBGColor);
	std::string fontStr = GetFontString(&g_ToolTipFont);
	CCalendarWindow::c_Config.SetToolTipFont(fontStr);
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
			return FALSE;

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				OnOKBackgroundDialog(hwndDlg);
				CCalendarWindow::c_Config.WriteConfig(CConfig::WRITE_SKIN);
				SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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
				UpdateBackgroundWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_BACKGROUND_FILENAME:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_TOOLTIP_TEXTFONTCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_ToolTipFontColor);

			case IDC_TOOLTIP_BGCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_ToolTipBGColor);

			case IDC_TOOLTIP_SELECT:
				SelectFont(hwndDlg, &g_ToolTipFont);
				UpdateBackgroundWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

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

	state = CCalendarWindow::c_Config.GetDaysEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_DAYS_ENABLE, state);

	widget = GetDlgItem(window, IDC_DAYS_X);
	itoa(CCalendarWindow::c_Config.GetDaysX(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_DAYS_Y);
	itoa(CCalendarWindow::c_Config.GetDaysY(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_DAYS_W);
	itoa(CCalendarWindow::c_Config.GetDaysW(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_DAYS_H);
	itoa(CCalendarWindow::c_Config.GetDaysH(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	state = CCalendarWindow::c_Config.GetDaysIgnoreToday() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_DAYS_IGNORE_TODAY, state);

	state = CCalendarWindow::c_Config.GetDaysIgnoreEvent() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_DAYS_IGNORE_EVENT, state);

	// Fill the combos
	SendDlgItemMessage(window, IDC_DAYS_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)"Font");
	SendDlgItemMessage(window, IDC_DAYS_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)"Bitmap");

	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Left");
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Center");
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Right");
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Left");
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Center");
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Right");
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Left");
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Center");
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Right");

	state = InitRasterizer(CCalendarWindow::c_Config.GetDaysRasterizer());
	SendDlgItemMessage(window, IDC_DAYS_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(CCalendarWindow::c_Config.GetDaysAlign());
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_DAYS_BITMAP);
	SetWindowText(widget, CCalendarWindow::c_Config.GetDaysBitmapName().c_str());

	switch (CCalendarWindow::c_Config.GetDaysNumOfComponents())
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
	itoa(CCalendarWindow::c_Config.GetDaysSeparation(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_DaysFontColor = CCalendarWindow::c_Config.GetDaysFontColor();
	InitFont(CCalendarWindow::c_Config.GetDaysFont(), &g_DaysFont);

	UpdateDaysWidgets(window);

	return TRUE;
}

void OnOKDaysDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_DAYS_ENABLE));
	CCalendarWindow::c_Config.SetDaysEnable(state);

	widget = GetDlgItem(window, IDC_DAYS_X);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetDaysX(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_DAYS_Y);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetDaysY(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_DAYS_W);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetDaysW(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_DAYS_H);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetDaysH(atoi(tmpSz));

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_DAYS_IGNORE_TODAY));
	CCalendarWindow::c_Config.SetDaysIgnoreToday(state);

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_DAYS_IGNORE_EVENT));
	CCalendarWindow::c_Config.SetDaysIgnoreEvent(state);

	widget = GetDlgItem(window, IDC_DAYS_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetDaysBitmapName(tmpSz);

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_DAYS_COMPONENTS_10))
	{
		CCalendarWindow::c_Config.SetDaysNumOfComponents(10);
	}
	else
	{
		CCalendarWindow::c_Config.SetDaysNumOfComponents(32);
	}

	widget = GetDlgItem(window, IDC_DAYS_SEPARATION);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetDaysSeparation(atoi(tmpSz));

	CRasterizer::ALIGN align = GetAlign(window, IDC_DAYS_ALIGN);
	CCalendarWindow::c_Config.SetDaysAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_DAYS_RASTERIZER);
	CCalendarWindow::c_Config.SetDaysRasterizer(ras);

	CCalendarWindow::c_Config.SetDaysFontColor(g_DaysFontColor);
	
	std::string fontStr = GetFontString(&g_DaysFont);
	CCalendarWindow::c_Config.SetDaysFont(fontStr);
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
				OnOKDaysDialog(hwndDlg);
				CCalendarWindow::c_Config.WriteConfig(CConfig::WRITE_SKIN);
				SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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

	state = CCalendarWindow::c_Config.GetTodayEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_TODAY_ENABLE, state);

	// Fill the combos
	SendDlgItemMessage(window, IDC_TODAY_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)"Font");
	SendDlgItemMessage(window, IDC_TODAY_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)"Bitmap");

	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Left");
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Center");
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Right");
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Left");
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Center");
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Right");
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Left");
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Center");
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Right");

	state = InitRasterizer(CCalendarWindow::c_Config.GetTodayRasterizer());
	SendDlgItemMessage(window, IDC_TODAY_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(CCalendarWindow::c_Config.GetTodayAlign());
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_TODAY_BITMAP);
	SetWindowText(widget, CCalendarWindow::c_Config.GetTodayBitmapName().c_str());

	switch (CCalendarWindow::c_Config.GetTodayNumOfComponents())
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
	itoa(CCalendarWindow::c_Config.GetTodaySeparation(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_TodayFontColor = CCalendarWindow::c_Config.GetTodayFontColor();
	InitFont(CCalendarWindow::c_Config.GetTodayFont(), &g_TodayFont);

	UpdateTodayWidgets(window);

	return TRUE;
}

void OnOKTodayDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_TODAY_ENABLE));
	CCalendarWindow::c_Config.SetTodayEnable(state);

	widget = GetDlgItem(window, IDC_TODAY_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetTodayBitmapName(tmpSz);

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_TODAY_COMPONENTS_1))
	{
		CCalendarWindow::c_Config.SetTodayNumOfComponents(1);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_TODAY_COMPONENTS_10))
	{
		CCalendarWindow::c_Config.SetTodayNumOfComponents(10);
	}
	else
	{
		CCalendarWindow::c_Config.SetTodayNumOfComponents(32);
	}

	widget = GetDlgItem(window, IDC_TODAY_SEPARATION);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetTodaySeparation(atoi(tmpSz));

	CRasterizer::ALIGN align = GetAlign(window, IDC_TODAY_ALIGN);
	CCalendarWindow::c_Config.SetTodayAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_TODAY_RASTERIZER);
	CCalendarWindow::c_Config.SetTodayRasterizer(ras);

	CCalendarWindow::c_Config.SetTodayFontColor(g_TodayFontColor);
	
	std::string fontStr = GetFontString(&g_TodayFont);
	CCalendarWindow::c_Config.SetTodayFont(fontStr);
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
				OnOKTodayDialog(hwndDlg);
				CCalendarWindow::c_Config.WriteConfig(CConfig::WRITE_SKIN);
				SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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

	state = CCalendarWindow::c_Config.GetEventEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENT_ENABLE, state);

	// Fill the combos
	SendDlgItemMessage(window, IDC_EVENT_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)"Font");
	SendDlgItemMessage(window, IDC_EVENT_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)"Bitmap");

	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Left");
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Center");
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Right");
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Left");
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Center");
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Right");
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Left");
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Center");
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Right");

	state = InitRasterizer(CCalendarWindow::c_Config.GetEventRasterizer());
	SendDlgItemMessage(window, IDC_EVENT_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(CCalendarWindow::c_Config.GetEventAlign());
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_EVENT_BITMAP);
	SetWindowText(widget, CCalendarWindow::c_Config.GetEventBitmapName().c_str());

	switch (CCalendarWindow::c_Config.GetEventNumOfComponents())
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
	itoa(CCalendarWindow::c_Config.GetEventSeparation(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_EventFontColor = CCalendarWindow::c_Config.GetEventFontColor();
	InitFont(CCalendarWindow::c_Config.GetEventFont(), &g_EventFont);

	state = CCalendarWindow::c_Config.GetEventInCalendar() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENT_CALENDAR, state);

	g_EventFontColor2 = CCalendarWindow::c_Config.GetEventFontColor2();
	InitFont(CCalendarWindow::c_Config.GetEventFont2(), &g_EventFont2);

	UpdateEventWidgets(window);

	return TRUE;
}

void OnOKEventDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_ENABLE));
	CCalendarWindow::c_Config.SetEventEnable(state);

	widget = GetDlgItem(window, IDC_EVENT_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetEventBitmapName(tmpSz);

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_COMPONENTS_1))
	{
		CCalendarWindow::c_Config.SetEventNumOfComponents(1);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_COMPONENTS_10))
	{
		CCalendarWindow::c_Config.SetEventNumOfComponents(10);
	}
	else
	{
		CCalendarWindow::c_Config.SetEventNumOfComponents(32);
	}

	widget = GetDlgItem(window, IDC_EVENT_SEPARATION);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetEventSeparation(atoi(tmpSz));

	CRasterizer::ALIGN align = GetAlign(window, IDC_EVENT_ALIGN);
	CCalendarWindow::c_Config.SetEventAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_EVENT_RASTERIZER);
	CCalendarWindow::c_Config.SetEventRasterizer(ras);

	CCalendarWindow::c_Config.SetEventFontColor(g_EventFontColor);
	
	std::string fontStr = GetFontString(&g_EventFont);
	CCalendarWindow::c_Config.SetEventFont(fontStr);

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_CALENDAR));
	CCalendarWindow::c_Config.SetEventInCalendar(state);

	CCalendarWindow::c_Config.SetEventFontColor2(g_EventFontColor2);
	
	fontStr = GetFontString(&g_EventFont2);
	CCalendarWindow::c_Config.SetEventFont2(fontStr);
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
				OnOKEventDialog(hwndDlg);
				CCalendarWindow::c_Config.WriteConfig(CConfig::WRITE_SKIN);
				SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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

	state = CCalendarWindow::c_Config.GetWeekdaysEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_WEEKDAYS_ENABLE, state);

	// Fill the combos
	SendDlgItemMessage(window, IDC_WEEKDAYS_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)"Font");
	SendDlgItemMessage(window, IDC_WEEKDAYS_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)"Bitmap");

	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Left");
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Center");
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Right");
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Left");
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Center");
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Right");
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Left");
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Center");
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Right");

	state = InitRasterizer(CCalendarWindow::c_Config.GetWeekdaysRasterizer());
	SendDlgItemMessage(window, IDC_WEEKDAYS_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(CCalendarWindow::c_Config.GetWeekdaysAlign());
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_WEEKDAYS_BITMAP);
	SetWindowText(widget, CCalendarWindow::c_Config.GetWeekdaysBitmapName().c_str());

	g_WeekdaysFontColor = CCalendarWindow::c_Config.GetWeekdaysFontColor();
	InitFont(CCalendarWindow::c_Config.GetWeekdaysFont(), &g_WeekdaysFont);

	UpdateWeekdaysWidgets(window);

	return TRUE;
}

void OnOKWeekdaysDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WEEKDAYS_ENABLE));
	CCalendarWindow::c_Config.SetWeekdaysEnable(state);

	widget = GetDlgItem(window, IDC_WEEKDAYS_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetWeekdaysBitmapName(tmpSz);

	CRasterizer::ALIGN align = GetAlign(window, IDC_WEEKDAYS_ALIGN);
	CCalendarWindow::c_Config.SetWeekdaysAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_WEEKDAYS_RASTERIZER);
	CCalendarWindow::c_Config.SetWeekdaysRasterizer(ras);

	CCalendarWindow::c_Config.SetWeekdaysFontColor(g_WeekdaysFontColor);
	
	std::string fontStr = GetFontString(&g_WeekdaysFont);
	CCalendarWindow::c_Config.SetWeekdaysFont(fontStr);
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
				OnOKWeekdaysDialog(hwndDlg);
				CCalendarWindow::c_Config.WriteConfig(CConfig::WRITE_SKIN);
				SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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

	state = CCalendarWindow::c_Config.GetWeekNumbersEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_WEEKNUM_ENABLE, state);

	// Fill the combos
	SendDlgItemMessage(window, IDC_WEEKNUM_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)"Font");
	SendDlgItemMessage(window, IDC_WEEKNUM_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)"Bitmap");

	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Left");
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Center");
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Right");
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Left");
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Center");
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Right");
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Left");
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Center");
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Right");

	state = InitRasterizer(CCalendarWindow::c_Config.GetWeekNumbersRasterizer());
	SendDlgItemMessage(window, IDC_WEEKNUM_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(CCalendarWindow::c_Config.GetWeekNumbersAlign());
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_WEEKNUM_BITMAP);
	SetWindowText(widget, CCalendarWindow::c_Config.GetWeekNumbersBitmapName().c_str());

	switch (CCalendarWindow::c_Config.GetWeekNumbersNumOfComponents())
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
	itoa(CCalendarWindow::c_Config.GetWeekNumbersSeparation(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_WeekNumbersFontColor = CCalendarWindow::c_Config.GetWeekNumbersFontColor();
	InitFont(CCalendarWindow::c_Config.GetWeekNumbersFont(), &g_WeekNumbersFont);

	UpdateWeekNumbersWidgets(window);

	return TRUE;
}

void OnOKWeekNumbersDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WEEKNUM_ENABLE));
	CCalendarWindow::c_Config.SetWeekNumbersEnable(state);

	widget = GetDlgItem(window, IDC_WEEKNUM_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetWeekNumbersBitmapName(tmpSz);

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_WEEKNUM_COMPONENTS_10))
	{
		CCalendarWindow::c_Config.SetWeekNumbersNumOfComponents(10);
	}
	else
	{
		CCalendarWindow::c_Config.SetWeekNumbersNumOfComponents(54);
	}

	widget = GetDlgItem(window, IDC_WEEKNUM_SEPARATION);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetWeekNumbersSeparation(atoi(tmpSz));

	CRasterizer::ALIGN align = GetAlign(window, IDC_WEEKNUM_ALIGN);
	CCalendarWindow::c_Config.SetWeekNumbersAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_WEEKNUM_RASTERIZER);
	CCalendarWindow::c_Config.SetWeekNumbersRasterizer(ras);

	CCalendarWindow::c_Config.SetWeekNumbersFontColor(g_WeekNumbersFontColor);
	
	std::string fontStr = GetFontString(&g_WeekNumbersFont);
	CCalendarWindow::c_Config.SetWeekNumbersFont(fontStr);
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
				OnOKWeekNumbersDialog(hwndDlg);
				CCalendarWindow::c_Config.WriteConfig(CConfig::WRITE_SKIN);
				SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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

	state = CCalendarWindow::c_Config.GetMonthEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_MONTH_ENABLE, state);

	widget = GetDlgItem(window, IDC_MONTH_X);
	itoa(CCalendarWindow::c_Config.GetMonthX(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_MONTH_Y);
	itoa(CCalendarWindow::c_Config.GetMonthY(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	// Fill the combos
	SendDlgItemMessage(window, IDC_MONTH_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)"Font");
	SendDlgItemMessage(window, IDC_MONTH_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)"Bitmap");

	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Left");
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Center");
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Right");
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Left");
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Center");
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Right");
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Left");
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Center");
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Right");

	state = InitRasterizer(CCalendarWindow::c_Config.GetMonthRasterizer());
	SendDlgItemMessage(window, IDC_MONTH_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(CCalendarWindow::c_Config.GetMonthAlign());
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_MONTH_BITMAP);
	SetWindowText(widget, CCalendarWindow::c_Config.GetMonthBitmapName().c_str());

	g_MonthFontColor = CCalendarWindow::c_Config.GetMonthFontColor();
	InitFont(CCalendarWindow::c_Config.GetMonthFont(), &g_MonthFont);

	UpdateMonthWidgets(window);

	return TRUE;
}

void OnOKMonthDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_MONTH_ENABLE));
	CCalendarWindow::c_Config.SetMonthEnable(state);

	widget = GetDlgItem(window, IDC_MONTH_X);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetMonthX(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_MONTH_Y);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetMonthY(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_MONTH_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetMonthBitmapName(tmpSz);

	CRasterizer::ALIGN align = GetAlign(window, IDC_MONTH_ALIGN);
	CCalendarWindow::c_Config.SetMonthAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_MONTH_RASTERIZER);
	CCalendarWindow::c_Config.SetMonthRasterizer(ras);

	CCalendarWindow::c_Config.SetMonthFontColor(g_MonthFontColor);
	
	std::string fontStr = GetFontString(&g_MonthFont);
	CCalendarWindow::c_Config.SetMonthFont(fontStr);
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
				OnOKMonthDialog(hwndDlg);
				CCalendarWindow::c_Config.WriteConfig(CConfig::WRITE_SKIN);
				SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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

	state = CCalendarWindow::c_Config.GetYearEnable() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_YEAR_ENABLE, state);

	widget = GetDlgItem(window, IDC_YEAR_X);
	itoa(CCalendarWindow::c_Config.GetYearX(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_YEAR_Y);
	itoa(CCalendarWindow::c_Config.GetYearY(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	// Fill the combos
	SendDlgItemMessage(window, IDC_YEAR_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)"Font");
	SendDlgItemMessage(window, IDC_YEAR_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)"Bitmap");

	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Left");
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Center");
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Top Right");
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Left");
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Center");
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Right");
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Left");
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Center");
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)"Bottom Right");

	state = InitRasterizer(CCalendarWindow::c_Config.GetYearRasterizer());
	SendDlgItemMessage(window, IDC_YEAR_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(CCalendarWindow::c_Config.GetYearAlign());
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_YEAR_BITMAP);
	SetWindowText(widget, CCalendarWindow::c_Config.GetYearBitmapName().c_str());

	widget = GetDlgItem(window, IDC_YEAR_SEPARATION);
	itoa(CCalendarWindow::c_Config.GetYearSeparation(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_YearFontColor = CCalendarWindow::c_Config.GetYearFontColor();
	InitFont(CCalendarWindow::c_Config.GetYearFont(), &g_YearFont);

	UpdateYearWidgets(window);

	return TRUE;
}

void OnOKYearDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_YEAR_ENABLE));
	CCalendarWindow::c_Config.SetYearEnable(state);

	widget = GetDlgItem(window, IDC_YEAR_X);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetYearX(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_YEAR_Y);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetYearY(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_YEAR_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetYearBitmapName(tmpSz);

	widget = GetDlgItem(window, IDC_YEAR_SEPARATION);
	GetWindowText(widget, tmpSz, 256);
	CCalendarWindow::c_Config.SetYearSeparation(atoi(tmpSz));

	CRasterizer::ALIGN align = GetAlign(window, IDC_YEAR_ALIGN);
	CCalendarWindow::c_Config.SetYearAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_YEAR_RASTERIZER);
	CCalendarWindow::c_Config.SetYearRasterizer(ras);

	CCalendarWindow::c_Config.SetYearFontColor(g_YearFontColor);
	
	std::string fontStr = GetFontString(&g_YearFont);
	CCalendarWindow::c_Config.SetYearFont(fontStr);
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
				OnOKYearDialog(hwndDlg);
				CCalendarWindow::c_Config.WriteConfig(CConfig::WRITE_SKIN);
				SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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
