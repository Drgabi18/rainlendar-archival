/*
  Copyright (C) 2004 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Library/ItemTime.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: ItemTime.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.3  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.2  2004/12/05 18:28:21  rainy
  Fixed a crash bug when rasterizer was not set.

  Revision 1.1  2004/11/06 13:38:59  rainy
  no message

*/

#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "ItemTime.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemTime::CItemTime() : CItem()
{
	m_WinType = RAINWINDOW_TYPE_CALENDAR;
	m_X = 0;
	m_Y = 0;
}

CItemTime::~CItemTime()
{
}

void CItemTime::ReadSettings(const char* iniFile, const char* section)
{
	CItem::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	m_X = GetPrivateProfileInt( section, "X", 0, iniFile);
 	m_Y = GetPrivateProfileInt( section, "Y", 0, iniFile);

	m_WinType = (RAINWINDOW_TYPE)GetPrivateProfileInt(section, "Window", 0, iniFile);

	if (GetPrivateProfileString(section, "Format", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_Format = tmpSz;
	}

	if (GetPrivateProfileString(section, "Rasterizer", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		 if(0==strcmp(tmpSz, "BITMAP")) 
		 {
			CRasterizerBitmap* BMRast;

			BMRast = new CRasterizerBitmap;

			if (GetPrivateProfileString(section, "BitmapName", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				std::string name = tmpSz;
				BMRast->Load(name);
			}

			CRasterizer::ALIGN align = (CRasterizer::ALIGN)GetPrivateProfileInt(section, "Align", 0, iniFile);
			BMRast->SetAlign(align);

			int separation = GetPrivateProfileInt(section, "Separation", 0, iniFile);
			BMRast->SetSeparation(separation);

			int digits = GetPrivateProfileInt(section, "Digits", 0, iniFile);
			BMRast->SetDigits(digits);

			int components = GetPrivateProfileInt(section, "NumOfComponents", 10, iniFile);
			BMRast->SetNumOfComponents(components);

			SetRasterizer(BMRast);
		} 
		else if(0==strcmp(tmpSz, "FONT")) 
		{
			CRasterizerFont* FNRast;

			FNRast = new CRasterizerFont;

			if (GetPrivateProfileString(section, "Font", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				std::string name = tmpSz;
				FNRast->SetFont(name);
			}

			CRasterizer::ALIGN align = (CRasterizer::ALIGN)GetPrivateProfileInt(section, "Align", 0, iniFile);
			FNRast->SetAlign(align);

			if(GetPrivateProfileString(section, "FontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				FNRast->SetColor(CConfig::ParseColor(tmpSz));
			}

			time_t aclock;
			struct tm* newtime;
			::time(&aclock);
			newtime = localtime(&aclock);
			strftime(tmpSz, MAX_LINE_LENGTH, m_Format.c_str(), newtime);

			FNRast->UpdateDimensions(tmpSz);
			SetRasterizer(FNRast);
		}
	}

	m_Enabled=(1==GetPrivateProfileInt( section, "Enable", 1, iniFile))?true:false;
}

void CItemTime::WriteSettings()
{
	char tmpSz[256];
	std::string INIPath = m_SettingsFile;

	// Only the enable/disable state need to be written since other settings cannot be edited
	sprintf(tmpSz, "%i", m_Enabled);
	WritePrivateProfileString( m_Section.c_str(), "Enable", tmpSz, INIPath.c_str() );
}

/* 
** Initialize
**
** Initializes the item
**
*/
void CItemTime::Initialize()
{
	if (!GetRasterizer())
	{
		m_Enabled = false;
	}
}

/* 
** Paint
**
** Paints the items
**
*/
void CItemTime::Paint(CImage& background, POINT offset)
{
	char tmpSz[MAX_LINE_LENGTH];
	time_t aclock;
	struct tm* newtime;
	::time(&aclock);
	newtime = localtime(&aclock);

	int x = m_X;
	int y = m_Y;

	if (x < 0)
	{
		x += background.GetWidth();
	}

	if (y < 0)
	{
		y += background.GetHeight();
	}

	strftime(tmpSz, MAX_LINE_LENGTH, m_Format.c_str(), newtime);

	if (GetRasterizer())
	{
		if (GetRasterizer()->GetType() == CRasterizer::TYPE_FONT)
		{
			((CRasterizerFont*)GetRasterizer())->WriteText(background, x, y, 0, 0, tmpSz, false, false);
		}
		else
		{
			GetRasterizer()->Paint(background, x, y, 0, 0, atoi(tmpSz));
		}
	}
}
