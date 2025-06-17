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
  $Header: /home/cvsroot/Rainlendar/Library/SkinDialog.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: SkinDialog.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.11  2005/03/25 13:58:43  rainy
  no message

  Revision 1.10  2005/03/01 18:42:44  rainy
  Opening second instance of the dialog is disabled.

  Revision 1.9  2004/12/05 18:22:13  rainy
  The fields are set to read-only instead of disabled.

  Revision 1.8  2004/11/06 13:38:59  rainy
  no message

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
#pragma warning(disable: 4996)

#include "SkinDialog.h"
#include "RainlendarDLL.h"
#include "ToolTip.h"
#include <Prsht.h>
#include <Commdlg.h>
#include <list>

#define NUM_OF_TABS 11

static HWND g_Dialog = NULL;

// Few globals
COLORREF g_SolidColor = 0;
COLORREF g_DaysFontColor = 0;
COLORREF g_DaysWeekendFontColor = 0;
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
COLORREF g_EventListFontColor = 0;
LOGFONT g_EventListFont;
COLORREF g_EventListHeaderFontColor = 0;
LOGFONT g_EventListHeaderFont;

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
BOOL CALLBACK EventListPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

VOID OpenSkinDialog(HWND hwndOwner, HINSTANCE instance)
{
	if (g_Dialog != NULL)
	{
		FLASHWINFO fi;
		fi.cbSize = sizeof(FLASHWINFO);
		fi.hwnd = g_Dialog;
		fi.dwFlags = FLASHW_CAPTION;
		fi.dwTimeout = 0;
		fi.uCount = 5;
		FlashWindowEx(&fi);
		return;
	}
    
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

		case 10:
			psp[i].pszTemplate = MAKEINTRESOURCE(IDD_EVENTLIST_CONFIG);
			psp[i].pfnDlgProc = EventListPageProc;
			psp[i].pszTitle = CCalendarWindow::c_Language.GetString("General", 24);
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
			SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);		// Refresh
		}
	}

	g_Dialog = NULL;
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
	logFont->lfItalic = (BYTE)bItalic;
	logFont->lfUnderline = (BYTE)bUnderline;
	logFont->lfStrikeOut = (BYTE)cStrikeOut;
	logFont->lfCharSet = (BYTE)nCharSet;
	logFont->lfOutPrecision = (BYTE)nOutPrecision;
	logFont->lfClipPrecision =  (BYTE)nClipPrecision;
	logFont->lfQuality = (BYTE)nQuality;
	logFont->lfPitchAndFamily = (BYTE)nPitchAndFamily;

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

	std::string path;
	CConfig::Instance().AddPath(path);

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwndDlg;
	ofn.lpstrFile = file;
	ofn.lpstrInitialDir = path.c_str();
	ofn.nMaxFile = MAX_LINE_LENGTH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrFilter = "Image Files (*.png, *.bmp, *.jpg)\0*.png;*.bmp;*.jpg;*.jpeg)\0All Files (*.*)\0*.*\0\0";

	if (GetOpenFileName(&ofn))
	{
		// If file has the same path as the skin, remove it
		int len = path.length();
		if (strnicmp(path.c_str(), file, len) != 0)
		{
			len = 0;
		}

		HWND widget = NULL;
		widget = GetDlgItem(hwndDlg, item);
		SetWindowText(widget, file + len);
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
//	EnableWindow(GetDlgItem(window, IDC_TOOLTIP_FONTNAME), FALSE);

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

	CCalendarWindow& calendar = GetRainlendar()->GetCalendarWindow();

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
	widget = GetDlgItem(window, IDC_ADVANCED_EDIT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 58));

	widget = GetDlgItem(window, IDC_BACKGROUND_FILENAME);
	SetWindowText(widget, calendar.GetBackgroundBitmapName().c_str());

	switch (calendar.GetBackgroundMode())
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

	g_SolidColor = calendar.GetBackgroundSolidColor();
	g_ToolTipFontColor = CToolTip::Instance().GetToolTipFontColor();
	g_ToolTipBGColor = CToolTip::Instance().GetToolTipBGColor();
	InitFont(CToolTip::Instance().GetToolTipFont(), &g_ToolTipFont);

	state = calendar.GetBackgroundBevel() ? BST_CHECKED : BST_UNCHECKED;
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

	CCalendarWindow& calendar = GetRainlendar()->GetCalendarWindow();

	widget = GetDlgItem(window, IDC_BACKGROUND_FILENAME);
	GetWindowText(widget, tmpSz, 256);
	calendar.SetBackgroundBitmapName(tmpSz);

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_BACKGROUND_STRETCH))
	{
		calendar.SetBackgroundMode(MODE_STRETCH);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_BACKGROUND_TILE))
	{
		calendar.SetBackgroundMode(MODE_TILE);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_BACKGROUND_SOLID))
	{
		calendar.SetBackgroundMode(MODE_SOLID);
	}
	else
	{
		calendar.SetBackgroundMode(MODE_COPY);
	}

	calendar.SetBackgroundSolidColor(g_SolidColor);

	CToolTip::Instance().SetToolTipFontColor(g_ToolTipFontColor);
	CToolTip::Instance().SetToolTipBGColor(g_ToolTipBGColor);
	std::string fontStr = GetFontString(&g_ToolTipFont);
	CToolTip::Instance().SetToolTipFont(fontStr);

	bool state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_BACKGROUND_BEVEL));
	calendar.SetBackgroundBevel(state);
}

BOOL CALLBACK BackgroundPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			g_Dialog = hwndDlg;
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
				GetRainlendar()->GetCalendarWindow().WriteSettings();
				CToolTip::Instance().WriteSettings();
				PropSheet_QuerySiblings(GetParent(hwndDlg), 0, 0);		// This makes refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
				SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
				return TRUE;
			}
			break;

		case PSM_QUERYSIBLINGS:
			{
				// Only the first tab sends the refresh
				SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);				// Refresh
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

			case IDC_ADVANCED_EDIT:
				// Close the dialog and open the skin.ini in notepad
				std::string command = CConfig::Instance().GetConfigEditor();
				command += " \"";
				std::string path = CConfig::Instance().GetCurrentSkinIni();
				CConfig::Instance().AddPath(path);
				command += path + "\"";
				LSExecute(hwndDlg, command.c_str(), SW_SHOWNORMAL);

				PostMessage(GetParent(hwndDlg), WM_COMMAND, IDCANCEL, NULL);
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
		EnableWindow(GetDlgItem(window, IDC_DAYS_BITMAP2), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_BROWSE2), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_10), TRUE); 
		EnableWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_32), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_SELECT), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR2), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_LAYOUT_NORMAL), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_LAYOUT_HORIZONTAL), TRUE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_LAYOUT_VERTICAL), TRUE);

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
		EnableWindow(GetDlgItem(window, IDC_DAYS_BITMAP2), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_BROWSE2), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_10), FALSE); 
		EnableWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_32), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_SEPARATION), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_SELECT), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR2), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_LAYOUT_NORMAL), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_LAYOUT_HORIZONTAL), FALSE);
		EnableWindow(GetDlgItem(window, IDC_DAYS_LAYOUT_VERTICAL), FALSE);
	}

	// This is always disabled
//	EnableWindow(GetDlgItem(window, IDC_DAYS_FONTNAME), FALSE);

	// Hide the Bitmap or Font stuff depending which rasterizer is in use
	int index = SendDlgItemMessage(window, IDC_DAYS_RASTERIZER, CB_GETCURSEL, NULL, NULL);
	if (index == 0)
	{
		// Hide Font show Bitmap
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_SEPARATION), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP2), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_10), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_32), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BROWSE), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BROWSE2), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_SEPARATION_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP_STATIC2), SW_HIDE);
		
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTNAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONT_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_SELECT), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR2), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONT_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR_STATIC2), SW_SHOW);
	}
	else
	{
		// Hide Bitmap show Font
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_SEPARATION), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP2), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_10), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_32), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BROWSE), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BROWSE2), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_COMPONENTS_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_SEPARATION_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_DAYS_BITMAP_STATIC2), SW_SHOW);
		
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTNAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONT_FRAME), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_SELECT), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR2), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONT_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_DAYS_FONTCOLOR_STATIC2), SW_HIDE);
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

	CItemDays& days = *GetRainlendar()->GetCalendarWindow().GetItemDays();

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
	widget = GetDlgItem(window, IDC_DAYS_BITMAP_STATIC2);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 52));
	widget = GetDlgItem(window, IDC_DAYS_COMPONENTS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 12));
	widget = GetDlgItem(window, IDC_DAYS_SEPARATION_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 13));
	widget = GetDlgItem(window, IDC_DAYS_BROWSE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 3));
	widget = GetDlgItem(window, IDC_DAYS_BROWSE2);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 3));
	widget = GetDlgItem(window, IDC_DAYS_FONT_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	widget = GetDlgItem(window, IDC_DAYS_FONT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 15));
	widget = GetDlgItem(window, IDC_DAYS_FONTCOLOR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 17));
	widget = GetDlgItem(window, IDC_DAYS_FONTCOLOR_STATIC2);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 51));
	widget = GetDlgItem(window, IDC_DAYS_SELECT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 16));

	widget = GetDlgItem(window, IDC_DAYS_LAYOUT_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 53));
	widget = GetDlgItem(window, IDC_DAYS_LAYOUT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 54));
	widget = GetDlgItem(window, IDC_DAYS_LAYOUT_NORMAL);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 55));
	widget = GetDlgItem(window, IDC_DAYS_LAYOUT_HORIZONTAL);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 56));
	widget = GetDlgItem(window, IDC_DAYS_LAYOUT_VERTICAL);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 57));

	state = days.IsEnabled() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_DAYS_ENABLE, state);

	widget = GetDlgItem(window, IDC_DAYS_X);
	itoa(days.GetDaysX(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_DAYS_Y);
	itoa(days.GetDaysY(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_DAYS_W);
	itoa(days.GetDaysW(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_DAYS_H);
	itoa(days.GetDaysH(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	state = days.GetDaysIgnoreToday() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_DAYS_IGNORE_TODAY, state);

	state = days.GetDaysIgnoreEvent() ? BST_CHECKED : BST_UNCHECKED;
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

	state = InitRasterizer(days.GetDaysRasterizer());
	SendDlgItemMessage(window, IDC_DAYS_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(days.GetDaysAlign());
	SendDlgItemMessage(window, IDC_DAYS_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_DAYS_BITMAP);
	SetWindowText(widget, days.GetDaysBitmapName().c_str());

	widget = GetDlgItem(window, IDC_DAYS_BITMAP2);
	SetWindowText(widget, days.GetDaysWeekendBitmapName().c_str());

	switch (days.GetDaysNumOfComponents())
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

	switch (days.GetDaysLayout())
	{
	case CConfig::DAY_LAYOUT_HORIZONTAL:
		state = IDC_DAYS_LAYOUT_HORIZONTAL;
		break;

	case CConfig::DAY_LAYOUT_VERTICAL:
		state = IDC_DAYS_LAYOUT_VERTICAL;
		break;

	default:
		state = IDC_DAYS_LAYOUT_NORMAL;
		break;
	}
	CheckRadioButton(window, IDC_DAYS_LAYOUT_NORMAL, IDC_DAYS_LAYOUT_VERTICAL, state);

	widget = GetDlgItem(window, IDC_DAYS_SEPARATION);
	itoa(days.GetDaysSeparation(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_DaysFontColor = days.GetDaysFontColor();
	g_DaysWeekendFontColor = days.GetDaysWeekendFontColor();
	InitFont(days.GetDaysFont(), &g_DaysFont);

	UpdateDaysWidgets(window);

	return TRUE;
}

void OnOKDaysDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	CItemDays& days = *GetRainlendar()->GetCalendarWindow().GetItemDays();

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_DAYS_ENABLE));
	days.SetEnabled(state);

	widget = GetDlgItem(window, IDC_DAYS_X);
	GetWindowText(widget, tmpSz, 256);
	days.SetDaysX(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_DAYS_Y);
	GetWindowText(widget, tmpSz, 256);
	days.SetDaysY(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_DAYS_W);
	GetWindowText(widget, tmpSz, 256);
	days.SetDaysW(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_DAYS_H);
	GetWindowText(widget, tmpSz, 256);
	days.SetDaysH(atoi(tmpSz));

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_DAYS_IGNORE_TODAY));
	days.SetDaysIgnoreToday(state);

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_DAYS_IGNORE_EVENT));
	days.SetDaysIgnoreEvent(state);

	widget = GetDlgItem(window, IDC_DAYS_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	days.SetDaysBitmapName(tmpSz);

	tmpSz[0] = 0;
	widget = GetDlgItem(window, IDC_DAYS_BITMAP2);
	GetWindowText(widget, tmpSz, 256);
	days.SetDaysWeekendBitmapName(tmpSz);

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_DAYS_COMPONENTS_10))
	{
		days.SetDaysNumOfComponents(10);
	}
	else
	{
		days.SetDaysNumOfComponents(32);
	}

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_DAYS_LAYOUT_NORMAL))
	{
		days.SetDaysLayout(CConfig::DAY_LAYOUT_NORMAL);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_DAYS_LAYOUT_HORIZONTAL))
	{
		days.SetDaysLayout(CConfig::DAY_LAYOUT_HORIZONTAL);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_DAYS_LAYOUT_VERTICAL))
	{
		days.SetDaysLayout(CConfig::DAY_LAYOUT_VERTICAL);
	}

	widget = GetDlgItem(window, IDC_DAYS_SEPARATION);
	GetWindowText(widget, tmpSz, 256);
	days.SetDaysSeparation(atoi(tmpSz));

	CRasterizer::ALIGN align = GetAlign(window, IDC_DAYS_ALIGN);
	days.SetDaysAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_DAYS_RASTERIZER);
	days.SetDaysRasterizer(ras);

	days.SetDaysFontColor(g_DaysFontColor);
	days.SetDaysWeekendFontColor(g_DaysWeekendFontColor);
	
	std::string fontStr = GetFontString(&g_DaysFont);
	days.SetDaysFont(fontStr);
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
				GetRainlendar()->GetCalendarWindow().GetItemDays()->WriteSettings();
				PropSheet_QuerySiblings(GetParent(hwndDlg), 0, 0);		// This makes refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
				SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
				return TRUE;
			}
			break;

        case WM_DRAWITEM:
			if (wParam == IDC_DAYS_FONTCOLOR)
			{
				return DrawItemFontColor(g_DaysFontColor, wParam, lParam);
			}
			else if (wParam == IDC_DAYS_FONTCOLOR2)
			{
				return DrawItemFontColor(g_DaysWeekendFontColor, wParam, lParam);
			}
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_DAYS_BROWSE:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return BrowseBitmap(hwndDlg, IDC_DAYS_BITMAP);

			case IDC_DAYS_BROWSE2:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return BrowseBitmap(hwndDlg, IDC_DAYS_BITMAP2);

			case IDC_DAYS_SELECT:
				SelectFont(hwndDlg, &g_DaysFont);
				UpdateDaysWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_DAYS_FONTCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_DaysFontColor);

			case IDC_DAYS_FONTCOLOR2:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_DaysWeekendFontColor);

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
			case IDC_DAYS_BITMAP2:
			case IDC_DAYS_IGNORE_EVENT:
			case IDC_DAYS_IGNORE_TODAY:
			case IDC_DAYS_LAYOUT_HORIZONTAL:
			case IDC_DAYS_LAYOUT_VERTICAL:
			case IDC_DAYS_LAYOUT_NORMAL:
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
//	EnableWindow(GetDlgItem(window, IDC_TODAY_FONTNAME), FALSE);

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

	CItemToday& today = *GetRainlendar()->GetCalendarWindow().GetItemToday();

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

	state = today.IsEnabled() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_TODAY_ENABLE, state);

	// Fill the combos
	SendDlgItemMessage(window, IDC_TODAY_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	SendDlgItemMessage(window, IDC_TODAY_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 10));

	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 43));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 42));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 41));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 40));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 39));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 38));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 37));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 36));
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 35));

	state = InitRasterizer(today.GetTodayRasterizer());
	SendDlgItemMessage(window, IDC_TODAY_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(today.GetTodayAlign());
	SendDlgItemMessage(window, IDC_TODAY_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_TODAY_BITMAP);
	SetWindowText(widget, today.GetTodayBitmapName().c_str());

	switch (today.GetTodayNumOfComponents())
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
	itoa(today.GetTodaySeparation(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_TodayFontColor = today.GetTodayFontColor();
	InitFont(today.GetTodayFont(), &g_TodayFont);

	UpdateTodayWidgets(window);

	return TRUE;
}

void OnOKTodayDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	CItemToday& today = *GetRainlendar()->GetCalendarWindow().GetItemToday();

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_TODAY_ENABLE));
	today.SetEnabled(state);

	widget = GetDlgItem(window, IDC_TODAY_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	today.SetTodayBitmapName(tmpSz);

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_TODAY_COMPONENTS_1))
	{
		today.SetTodayNumOfComponents(1);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_TODAY_COMPONENTS_10))
	{
		today.SetTodayNumOfComponents(10);
	}
	else
	{
		today.SetTodayNumOfComponents(32);
	}

	widget = GetDlgItem(window, IDC_TODAY_SEPARATION);
	GetWindowText(widget, tmpSz, 256);
	today.SetTodaySeparation(atoi(tmpSz));

	CRasterizer::ALIGN align = GetAlign(window, IDC_TODAY_ALIGN);
	today.SetTodayAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_TODAY_RASTERIZER);
	today.SetTodayRasterizer(ras);

	today.SetTodayFontColor(g_TodayFontColor);
	
	std::string fontStr = GetFontString(&g_TodayFont);
	today.SetTodayFont(fontStr);
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
				GetRainlendar()->GetCalendarWindow().GetItemToday()->WriteSettings();
				PropSheet_QuerySiblings(GetParent(hwndDlg), 0, 0);		// This makes refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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
		EnableWindow(GetDlgItem(window, IDC_EVENT_CALENDAR), TRUE);

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
//	EnableWindow(GetDlgItem(window, IDC_EVENT_FONTNAME), FALSE);
//	EnableWindow(GetDlgItem(window, IDC_EVENT_FONTNAME2), FALSE);

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
	
	CItemEvent& event = *GetRainlendar()->GetCalendarWindow().GetItemEvent();

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

	state = event.IsEnabled() ? BST_CHECKED : BST_UNCHECKED;
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

	state = InitRasterizer(event.GetEventRasterizer());
	SendDlgItemMessage(window, IDC_EVENT_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(event.GetEventAlign());
	SendDlgItemMessage(window, IDC_EVENT_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_EVENT_BITMAP);
	SetWindowText(widget, event.GetEventBitmapName().c_str());

	switch (event.GetEventNumOfComponents())
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
	itoa(event.GetEventSeparation(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_EventFontColor = event.GetEventFontColor();
	InitFont(event.GetEventFont(), &g_EventFont);

	state = event.GetEventInCalendar() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EVENT_CALENDAR, state);

	g_EventFontColor2 = event.GetEventFontColor2();
	InitFont(event.GetEventFont2(), &g_EventFont2);

	UpdateEventWidgets(window);

	return TRUE;
}

void OnOKEventDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	CItemEvent& event = *GetRainlendar()->GetCalendarWindow().GetItemEvent();

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_ENABLE));
	event.SetEnabled(state);

	widget = GetDlgItem(window, IDC_EVENT_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	event.SetEventBitmapName(tmpSz);

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_COMPONENTS_1))
	{
		event.SetEventNumOfComponents(1);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_COMPONENTS_10))
	{
		event.SetEventNumOfComponents(10);
	}
	else
	{
		event.SetEventNumOfComponents(32);
	}

	widget = GetDlgItem(window, IDC_EVENT_SEPARATION);
	GetWindowText(widget, tmpSz, 256);
	event.SetEventSeparation(atoi(tmpSz));

	CRasterizer::ALIGN align = GetAlign(window, IDC_EVENT_ALIGN);
	event.SetEventAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_EVENT_RASTERIZER);
	event.SetEventRasterizer(ras);

	event.SetEventFontColor(g_EventFontColor);
	
	std::string fontStr = GetFontString(&g_EventFont);
	event.SetEventFont(fontStr);

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EVENT_CALENDAR));
	event.SetEventInCalendar(state);

	event.SetEventFontColor2(g_EventFontColor2);
	
	fontStr = GetFontString(&g_EventFont2);
	event.SetEventFont2(fontStr);
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
				GetRainlendar()->GetCalendarWindow().GetItemEvent()->WriteSettings();
				PropSheet_QuerySiblings(GetParent(hwndDlg), 0, 0);		// This makes refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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
//	EnableWindow(GetDlgItem(window, IDC_WEEKDAYS_FONTNAME), FALSE);

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

	CItemWeekdays& weekdays = *GetRainlendar()->GetCalendarWindow().GetItemWeekdays();

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

	state = weekdays.IsEnabled() ? BST_CHECKED : BST_UNCHECKED;
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

	state = InitRasterizer(weekdays.GetWeekdaysRasterizer());
	SendDlgItemMessage(window, IDC_WEEKDAYS_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(weekdays.GetWeekdaysAlign());
	SendDlgItemMessage(window, IDC_WEEKDAYS_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_WEEKDAYS_BITMAP);
	SetWindowText(widget, weekdays.GetWeekdaysBitmapName().c_str());

	g_WeekdaysFontColor = weekdays.GetWeekdaysFontColor();
	InitFont(weekdays.GetWeekdaysFont(), &g_WeekdaysFont);

	UpdateWeekdaysWidgets(window);

	return TRUE;
}

void OnOKWeekdaysDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	CItemWeekdays& weekdays = *GetRainlendar()->GetCalendarWindow().GetItemWeekdays();

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WEEKDAYS_ENABLE));
	weekdays.SetEnabled(state);

	widget = GetDlgItem(window, IDC_WEEKDAYS_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	weekdays.SetWeekdaysBitmapName(tmpSz);

	CRasterizer::ALIGN align = GetAlign(window, IDC_WEEKDAYS_ALIGN);
	weekdays.SetWeekdaysAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_WEEKDAYS_RASTERIZER);
	weekdays.SetWeekdaysRasterizer(ras);

	weekdays.SetWeekdaysFontColor(g_WeekdaysFontColor);
	
	std::string fontStr = GetFontString(&g_WeekdaysFont);
	weekdays.SetWeekdaysFont(fontStr);
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
				GetRainlendar()->GetCalendarWindow().GetItemWeekdays()->WriteSettings();
				PropSheet_QuerySiblings(GetParent(hwndDlg), 0, 0);		// This makes refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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
//	EnableWindow(GetDlgItem(window, IDC_WEEKNUM_FONTNAME), FALSE);

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

	CItemWeekNumbers& weeknums = *GetRainlendar()->GetCalendarWindow().GetItemWeekNumbers();

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

	state = weeknums.IsEnabled() ? BST_CHECKED : BST_UNCHECKED;
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

	state = InitRasterizer(weeknums.GetWeekNumbersRasterizer());
	SendDlgItemMessage(window, IDC_WEEKNUM_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(weeknums.GetWeekNumbersAlign());
	SendDlgItemMessage(window, IDC_WEEKNUM_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_WEEKNUM_BITMAP);
	SetWindowText(widget, weeknums.GetWeekNumbersBitmapName().c_str());

	switch (weeknums.GetWeekNumbersNumOfComponents())
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
	itoa(weeknums.GetWeekNumbersSeparation(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_WeekNumbersFontColor = weeknums.GetWeekNumbersFontColor();
	InitFont(weeknums.GetWeekNumbersFont(), &g_WeekNumbersFont);

	UpdateWeekNumbersWidgets(window);

	return TRUE;
}

void OnOKWeekNumbersDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	CItemWeekNumbers& weeknums = *GetRainlendar()->GetCalendarWindow().GetItemWeekNumbers();

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_WEEKNUM_ENABLE));
	weeknums.SetEnabled(state);

	widget = GetDlgItem(window, IDC_WEEKNUM_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	weeknums.SetWeekNumbersBitmapName(tmpSz);

	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_WEEKNUM_COMPONENTS_10))
	{
		weeknums.SetWeekNumbersNumOfComponents(10);
	}
	else
	{
		weeknums.SetWeekNumbersNumOfComponents(54);
	}

	widget = GetDlgItem(window, IDC_WEEKNUM_SEPARATION);
	GetWindowText(widget, tmpSz, 256);
	weeknums.SetWeekNumbersSeparation(atoi(tmpSz));

	CRasterizer::ALIGN align = GetAlign(window, IDC_WEEKNUM_ALIGN);
	weeknums.SetWeekNumbersAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_WEEKNUM_RASTERIZER);
	weeknums.SetWeekNumbersRasterizer(ras);

	weeknums.SetWeekNumbersFontColor(g_WeekNumbersFontColor);
	
	std::string fontStr = GetFontString(&g_WeekNumbersFont);
	weeknums.SetWeekNumbersFont(fontStr);
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
				GetRainlendar()->GetCalendarWindow().GetItemWeekNumbers()->WriteSettings();
				PropSheet_QuerySiblings(GetParent(hwndDlg), 0, 0);		// This makes refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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
//	EnableWindow(GetDlgItem(window, IDC_MONTH_FONTNAME), FALSE);

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

	CItemMonth& month = *GetRainlendar()->GetCalendarWindow().GetItemMonth();

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

	state = month.IsEnabled() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_MONTH_ENABLE, state);

	widget = GetDlgItem(window, IDC_MONTH_X);
	itoa(month.GetMonthX(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_MONTH_Y);
	itoa(month.GetMonthY(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	// Fill the combos
	SendDlgItemMessage(window, IDC_MONTH_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	SendDlgItemMessage(window, IDC_MONTH_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 10));

	// NOTE: Flipped values
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 43));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 42));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 41));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 40));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 39));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 38));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 37));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 36));
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 35));
	
	state = InitRasterizer(month.GetMonthRasterizer());
	SendDlgItemMessage(window, IDC_MONTH_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(month.GetMonthAlign());
	SendDlgItemMessage(window, IDC_MONTH_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_MONTH_BITMAP);
	SetWindowText(widget, month.GetMonthBitmapName().c_str());

	g_MonthFontColor = month.GetMonthFontColor();
	InitFont(month.GetMonthFont(), &g_MonthFont);

	UpdateMonthWidgets(window);

	return TRUE;
}

void OnOKMonthDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	CItemMonth& month = *GetRainlendar()->GetCalendarWindow().GetItemMonth();

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_MONTH_ENABLE));
	month.SetEnabled(state);

	widget = GetDlgItem(window, IDC_MONTH_X);
	GetWindowText(widget, tmpSz, 256);
	month.SetMonthX(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_MONTH_Y);
	GetWindowText(widget, tmpSz, 256);
	month.SetMonthY(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_MONTH_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	month.SetMonthBitmapName(tmpSz);

	CRasterizer::ALIGN align = GetAlign(window, IDC_MONTH_ALIGN);
	month.SetMonthAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_MONTH_RASTERIZER);
	month.SetMonthRasterizer(ras);

	month.SetMonthFontColor(g_MonthFontColor);
	
	std::string fontStr = GetFontString(&g_MonthFont);
	month.SetMonthFont(fontStr);
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
				GetRainlendar()->GetCalendarWindow().GetItemMonth()->WriteSettings();
				PropSheet_QuerySiblings(GetParent(hwndDlg), 0, 0);		// This makes refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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
//	EnableWindow(GetDlgItem(window, IDC_YEAR_FONTNAME), FALSE);

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

	CItemYear& year = *GetRainlendar()->GetCalendarWindow().GetItemYear();

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

	state = year.IsEnabled() ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_YEAR_ENABLE, state);

	widget = GetDlgItem(window, IDC_YEAR_X);
	itoa(year.GetYearX(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_YEAR_Y);
	itoa(year.GetYearY(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	// Fill the combos
	SendDlgItemMessage(window, IDC_YEAR_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 14));
	SendDlgItemMessage(window, IDC_YEAR_RASTERIZER, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 10));

	// NOTE: Flipped values
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 43));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 42));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 41));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 40));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 39));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 38));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 37));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 36));
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 35));

	state = InitRasterizer(year.GetYearRasterizer());
	SendDlgItemMessage(window, IDC_YEAR_RASTERIZER, CB_SETCURSEL, (WPARAM)state, NULL);

	state = InitAlign(year.GetYearAlign());
	SendDlgItemMessage(window, IDC_YEAR_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_YEAR_BITMAP);
	SetWindowText(widget, year.GetYearBitmapName().c_str());

	widget = GetDlgItem(window, IDC_YEAR_SEPARATION);
	itoa(year.GetYearSeparation(), tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_YearFontColor = year.GetYearFontColor();
	InitFont(year.GetYearFont(), &g_YearFont);

	UpdateYearWidgets(window);

	return TRUE;
}

void OnOKYearDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	bool state;

	CItemYear& year = *GetRainlendar()->GetCalendarWindow().GetItemYear();

	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_YEAR_ENABLE));
	year.SetEnabled(state);

	widget = GetDlgItem(window, IDC_YEAR_X);
	GetWindowText(widget, tmpSz, 256);
	year.SetYearX(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_YEAR_Y);
	GetWindowText(widget, tmpSz, 256);
	year.SetYearY(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_YEAR_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	year.SetYearBitmapName(tmpSz);

	widget = GetDlgItem(window, IDC_YEAR_SEPARATION);
	GetWindowText(widget, tmpSz, 256);
	year.SetYearSeparation(atoi(tmpSz));

	CRasterizer::ALIGN align = GetAlign(window, IDC_YEAR_ALIGN);
	year.SetYearAlign(align);
	
	CRasterizer::TYPE ras = GetRasterizer(window, IDC_YEAR_RASTERIZER);
	year.SetYearRasterizer(ras);

	year.SetYearFontColor(g_YearFontColor);
	
	std::string fontStr = GetFontString(&g_YearFont);
	year.SetYearFont(fontStr);
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
				GetRainlendar()->GetCalendarWindow().GetItemYear()->WriteSettings();
				PropSheet_QuerySiblings(GetParent(hwndDlg), 0, 0);		// This makes refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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

	CTodoWindow& todo = GetRainlendar()->GetCalendarWindow().GetTodoWindow();
	
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
	itoa(todo.GetTodoW(), tmpSz, 10);
	SetWindowText(widget, tmpSz);
	
	widget = GetDlgItem(window, IDC_TODO_BITMAP);
	SetWindowText(widget, todo.GetTodoBitmapName().c_str());

	RECT margins = todo.GetTodoBitmapMargins();
	sprintf(tmpSz, "%i, %i, %i, %i", margins.left, margins.top, margins.right, margins.bottom);
	widget = GetDlgItem(window, IDC_TODO_BITMAP_MARGINS);
	SetWindowText(widget, tmpSz);

	margins = todo.GetTodoTextMargins();
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

	state = InitAlign(todo.GetTodoItemAlign());
	SendDlgItemMessage(window, IDC_TODO_ITEM_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_TODO_ITEM_BITMAP);
	SetWindowText(widget, todo.GetTodoItemBitmapName().c_str());

	POINT offset = todo.GetTodoItemOffset();
	widget = GetDlgItem(window, IDC_TODO_ITEM_OFFSET_X);
	itoa(offset.x, tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_TODO_ITEM_OFFSET_Y);
	itoa(offset.y, tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_TodoFontColor = todo.GetTodoFontColor();
	InitFont(todo.GetTodoFont(), &g_TodoFont);

	UpdateTodoWidgets(window);

	return TRUE;
}

void OnOKTodoDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	RECT margins;
	POINT offset;

	CTodoWindow& todo = GetRainlendar()->GetCalendarWindow().GetTodoWindow();

	widget = GetDlgItem(window, IDC_TODO_WIDTH);
	GetWindowText(widget, tmpSz, 256);
	todo.SetTodoW(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_TODO_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	todo.SetTodoBitmapName(tmpSz);

	margins.left = margins.top = margins.right = margins.bottom = 0;
	widget = GetDlgItem(window, IDC_TODO_BITMAP_MARGINS);
	GetWindowText(widget, tmpSz, 256);
	sscanf(tmpSz, "%i,%i,%i,%i", &margins.left, &margins.top, &margins.right, &margins.bottom);
	todo.SetTodoBitmapMargins(margins);

	margins.left = margins.top = margins.right = margins.bottom = 0;
	widget = GetDlgItem(window, IDC_TODO_TEXT_MARGINS);
	GetWindowText(widget, tmpSz, 256);
	sscanf(tmpSz, "%i,%i,%i,%i", &margins.left, &margins.top, &margins.right, &margins.bottom);
	todo.SetTodoTextMargins(margins);

	CRasterizer::ALIGN align = GetAlign(window, IDC_TODO_ITEM_ALIGN);
	todo.SetTodoItemAlign(align);

	widget = GetDlgItem(window, IDC_TODO_ITEM_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	todo.SetTodoItemBitmapName(tmpSz);
	
	widget = GetDlgItem(window, IDC_TODO_ITEM_OFFSET_X);
	GetWindowText(widget, tmpSz, 256);
	offset.x = atoi(tmpSz);

	widget = GetDlgItem(window, IDC_TODO_ITEM_OFFSET_Y);
	GetWindowText(widget, tmpSz, 256);
	offset.y = atoi(tmpSz);
	todo.SetTodoItemOffset(offset);

	std::string fontStr = GetFontString(&g_TodoFont);
	todo.SetTodoFont(fontStr);

	todo.SetTodoFontColor(g_TodoFontColor);
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
				GetRainlendar()->GetCalendarWindow().GetTodoWindow().WriteSettings();
				PropSheet_QuerySiblings(GetParent(hwndDlg), 0, 0);		// This makes refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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

	CMessageWindow& messagebox = GetRainlendar()->GetCalendarWindow().GetMessageWindow();
	
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
	SetWindowText(widget, messagebox.GetMessageBoxBitmapName().c_str());

	RECT margins = messagebox.GetMessageBoxBitmapMargins();
	sprintf(tmpSz, "%i, %i, %i, %i", margins.left, margins.top, margins.right, margins.bottom);
	widget = GetDlgItem(window, IDC_MESSAGEBOX_BITMAP_MARGINS);
	SetWindowText(widget, tmpSz);

	margins = messagebox.GetMessageBoxTextMargins();
	sprintf(tmpSz, "%i, %i, %i, %i", margins.left, margins.top, margins.right, margins.bottom);
	widget = GetDlgItem(window, IDC_MESSAGEBOX_TEXT_MARGINS);
	SetWindowText(widget, tmpSz);

	g_MessageBoxFontColor = messagebox.GetMessageBoxFontColor();
	InitFont(messagebox.GetMessageBoxFont(), &g_MessageBoxFont);

	UpdateMessageBoxWidgets(window);

	return TRUE;
}

void OnOKMessageBoxDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	RECT margins;

	CMessageWindow& messagebox = GetRainlendar()->GetCalendarWindow().GetMessageWindow();

	widget = GetDlgItem(window, IDC_MESSAGEBOX_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	messagebox.SetMessageBoxBitmapName(tmpSz);

	margins.left = margins.top = margins.right = margins.bottom = 0;
	widget = GetDlgItem(window, IDC_MESSAGEBOX_BITMAP_MARGINS);
	GetWindowText(widget, tmpSz, 256);
	sscanf(tmpSz, "%i,%i,%i,%i", &margins.left, &margins.top, &margins.right, &margins.bottom);
	messagebox.SetMessageBoxBitmapMargins(margins);

	margins.left = margins.top = margins.right = margins.bottom = 0;
	widget = GetDlgItem(window, IDC_MESSAGEBOX_TEXT_MARGINS);
	GetWindowText(widget, tmpSz, 256);
	sscanf(tmpSz, "%i,%i,%i,%i", &margins.left, &margins.top, &margins.right, &margins.bottom);
	messagebox.SetMessageBoxTextMargins(margins);

	std::string fontStr = GetFontString(&g_MessageBoxFont);
	messagebox.SetMessageBoxFont(fontStr);

	messagebox.SetMessageBoxFontColor(g_MessageBoxFontColor);
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
				GetRainlendar()->GetCalendarWindow().GetMessageWindow().WriteSettings();
				PropSheet_QuerySiblings(GetParent(hwndDlg), 0, 0);		// This makes refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
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

/****************************************************************************\ 
**
**                        E V E N T L I S T
**
\****************************************************************************/

void UpdateEventListWidgets(HWND window)
{
	// Update font name
	char tmpSz[256];
	HWND widget = GetDlgItem(window, IDC_EVENTLIST_FONTNAME);
	std::string name = g_EventListFont.lfFaceName;
	name += " / ";
	HDC dc = GetWindowDC(window);
	int size = -MulDiv(g_EventListFont.lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	ReleaseDC(window, dc);
	itoa(size, tmpSz, 10);
	name += tmpSz;
	SetWindowText(widget, name.c_str());

	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_FONTNAME);
	name = g_EventListHeaderFont.lfFaceName;
	name += " / ";
	dc = GetWindowDC(window);
	size = -MulDiv(g_EventListHeaderFont.lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	ReleaseDC(window, dc);
	itoa(size, tmpSz, 10);
	name += tmpSz;
	SetWindowText(widget, name.c_str());
}

BOOL OnInitEventListDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	UINT state;

	CEventListWindow& eventlist = GetRainlendar()->GetCalendarWindow().GetEventListWindow();
	
	widget = GetDlgItem(window, IDC_EVENTLIST_BITMAP_BACKGROUND_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 59));
	widget = GetDlgItem(window, IDC_EVENTLIST_WIDTH_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 44));
	widget = GetDlgItem(window, IDC_EVENTLIST_BITMAP_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 11));
	widget = GetDlgItem(window, IDC_EVENTLIST_BITMAP_MARGINS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 45));
	widget = GetDlgItem(window, IDC_EVENTLIST_TEXT_MARGINS_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 46));
	widget = GetDlgItem(window, IDC_EVENTLIST_BROWSE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 3));

	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 60));
	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_ALIGN_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 9));
	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_BITMAP_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 11));
	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_OFFSET_X_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 48));
	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_OFFSET_Y_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 5));
	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_BITMAP_BROWSE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 3));
	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_SELECT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 16));
	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_FONT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 15));
	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_FONTCOLOR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 17));

	widget = GetDlgItem(window, IDC_EVENTLIST_TEXT_FRAME);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 61));
	widget = GetDlgItem(window, IDC_EVENTLIST_SELECT);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 16));
	widget = GetDlgItem(window, IDC_EVENTLIST_FONT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 15));
	widget = GetDlgItem(window, IDC_EVENTLIST_FONTCOLOR_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("SkinGUI", 17));

	widget = GetDlgItem(window, IDC_EVENTLIST_WIDTH);
	itoa(eventlist.GetEventListW(), tmpSz, 10);
	SetWindowText(widget, tmpSz);
	
	widget = GetDlgItem(window, IDC_EVENTLIST_BITMAP);
	SetWindowText(widget, eventlist.GetEventListBitmapName().c_str());

	RECT margins = eventlist.GetEventListBitmapMargins();
	sprintf(tmpSz, "%i, %i, %i, %i", margins.left, margins.top, margins.right, margins.bottom);
	widget = GetDlgItem(window, IDC_EVENTLIST_BITMAP_MARGINS);
	SetWindowText(widget, tmpSz);

	margins = eventlist.GetEventListTextMargins();
	sprintf(tmpSz, "%i, %i, %i, %i", margins.left, margins.top, margins.right, margins.bottom);
	widget = GetDlgItem(window, IDC_EVENTLIST_TEXT_MARGINS);
	SetWindowText(widget, tmpSz);

	// Fill the combos
	SendDlgItemMessage(window, IDC_EVENTLIST_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 35));
	SendDlgItemMessage(window, IDC_EVENTLIST_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 36));
	SendDlgItemMessage(window, IDC_EVENTLIST_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 37));
	SendDlgItemMessage(window, IDC_EVENTLIST_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 38));
	SendDlgItemMessage(window, IDC_EVENTLIST_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 39));
	SendDlgItemMessage(window, IDC_EVENTLIST_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 40));
	SendDlgItemMessage(window, IDC_EVENTLIST_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 41));
	SendDlgItemMessage(window, IDC_EVENTLIST_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 42));
	SendDlgItemMessage(window, IDC_EVENTLIST_ITEM_ALIGN, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("SkinGUI", 43));

	state = InitAlign(eventlist.GetEventListHeaderItemAlign());
	SendDlgItemMessage(window, IDC_EVENTLIST_ITEM_ALIGN, CB_SETCURSEL, (WPARAM)state, NULL);

	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_BITMAP);
	SetWindowText(widget, eventlist.GetEventListHeaderItemBitmapName().c_str());

	POINT offset = eventlist.GetEventListHeaderItemOffset();
	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_OFFSET_X);
	itoa(offset.x, tmpSz, 10);
	SetWindowText(widget, tmpSz);

	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_OFFSET_Y);
	itoa(offset.y, tmpSz, 10);
	SetWindowText(widget, tmpSz);

	g_EventListFontColor = eventlist.GetEventListFontColor();
	InitFont(eventlist.GetEventListFont(), &g_EventListFont);

	g_EventListHeaderFontColor = eventlist.GetEventListHeaderFontColor();
	InitFont(eventlist.GetEventListHeaderFont(), &g_EventListHeaderFont);

	UpdateEventListWidgets(window);

	return TRUE;
}

void OnOKEventListDialog(HWND window) 
{
	char tmpSz[256];
	HWND widget;
	RECT margins;
	POINT offset;

	CEventListWindow& eventlist = GetRainlendar()->GetCalendarWindow().GetEventListWindow();

	widget = GetDlgItem(window, IDC_EVENTLIST_WIDTH);
	GetWindowText(widget, tmpSz, 256);
	eventlist.SetEventListW(atoi(tmpSz));

	widget = GetDlgItem(window, IDC_EVENTLIST_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	eventlist.SetEventListBitmapName(tmpSz);

	margins.left = margins.top = margins.right = margins.bottom = 0;
	widget = GetDlgItem(window, IDC_EVENTLIST_BITMAP_MARGINS);
	GetWindowText(widget, tmpSz, 256);
	sscanf(tmpSz, "%i,%i,%i,%i", &margins.left, &margins.top, &margins.right, &margins.bottom);
	eventlist.SetEventListBitmapMargins(margins);

	margins.left = margins.top = margins.right = margins.bottom = 0;
	widget = GetDlgItem(window, IDC_EVENTLIST_TEXT_MARGINS);
	GetWindowText(widget, tmpSz, 256);
	sscanf(tmpSz, "%i,%i,%i,%i", &margins.left, &margins.top, &margins.right, &margins.bottom);
	eventlist.SetEventListTextMargins(margins);

	CRasterizer::ALIGN align = GetAlign(window, IDC_EVENTLIST_ITEM_ALIGN);
	eventlist.SetEventListHeaderItemAlign(align);

	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_BITMAP);
	GetWindowText(widget, tmpSz, 256);
	eventlist.SetEventListHeaderItemBitmapName(tmpSz);
	
	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_OFFSET_X);
	GetWindowText(widget, tmpSz, 256);
	offset.x = atoi(tmpSz);

	widget = GetDlgItem(window, IDC_EVENTLIST_ITEM_OFFSET_Y);
	GetWindowText(widget, tmpSz, 256);
	offset.y = atoi(tmpSz);
	eventlist.SetEventListHeaderItemOffset(offset);

	std::string fontStr = GetFontString(&g_EventListHeaderFont);
	eventlist.SetEventListHeaderFont(fontStr);
	eventlist.SetEventListHeaderFontColor(g_EventListHeaderFontColor);

	fontStr = GetFontString(&g_EventListFont);
	eventlist.SetEventListFont(fontStr);
	eventlist.SetEventListFontColor(g_EventListFontColor);
}

BOOL CALLBACK EventListPageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitEventListDialog(hwndDlg);

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) 
			{
			case PSN_APPLY:
				LPPSHNOTIFY pshNotify = (LPPSHNOTIFY)lParam;
				OnOKEventListDialog(hwndDlg);
				GetRainlendar()->GetCalendarWindow().GetEventListWindow().WriteSettings();
				PropSheet_QuerySiblings(GetParent(hwndDlg), 0, 0);		// This makes refresh
				PropSheet_CancelToClose(GetParent(hwndDlg));
				SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
				return TRUE;
			}
			break;

        case WM_DRAWITEM:
			if (wParam == IDC_EVENTLIST_FONTCOLOR)
			{
				return DrawItemFontColor(g_EventListFontColor, wParam, lParam);
			}
			else if (wParam == IDC_EVENTLIST_ITEM_FONTCOLOR)
			{
				return DrawItemFontColor(g_EventListHeaderFontColor, wParam, lParam);
			}
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_EVENTLIST_BROWSE:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return BrowseBitmap(hwndDlg, IDC_EVENTLIST_BITMAP);

			case IDC_EVENTLIST_ITEM_BITMAP_BROWSE:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return BrowseBitmap(hwndDlg, IDC_EVENTLIST_ITEM_BITMAP);
				
			case IDC_EVENTLIST_ITEM_SELECT:
				SelectFont(hwndDlg, &g_EventListHeaderFont);
				UpdateEventListWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_EVENTLIST_ITEM_FONTCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_EventListHeaderFontColor);

			case IDC_EVENTLIST_SELECT:
				SelectFont(hwndDlg, &g_EventListFont);
				UpdateEventListWidgets(hwndDlg);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;

			case IDC_EVENTLIST_FONTCOLOR:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return SelectFontColor(hwndDlg, &g_EventListFontColor);

			case IDC_EVENTLIST_BITMAP:
			case IDC_EVENTLIST_ITEM_BITMAP:
			case IDC_EVENTLIST_BITMAP_MARGINS:
			case IDC_EVENTLIST_TEXT_MARGINS:
			case IDC_EVENTLIST_ITEM_ALIGN:
			case IDC_EVENTLIST_ITEM_OFFSET_X:
			case IDC_EVENTLIST_ITEM_OFFSET_Y:
			case IDC_EVENTLIST_WIDTH:
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
			}
			break;

	}
    return FALSE; 
}
