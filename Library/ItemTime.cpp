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
  $Header: /home/cvsroot/Rainlendar/Library/ItemTime.cpp,v 1.3 2005/10/14 17:05:29 rainy Exp $

  $Log: ItemTime.cpp,v $
  Revision 1.3  2005/10/14 17:05:29  rainy
  no message

  Revision 1.2  2005/09/08 16:09:12  rainy
  no message

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
#include "TimeZones.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemTime::CItemTime() : CItemDynamic()
{
	m_NumOfComponents = 1;
}

CItemTime::~CItemTime()
{
}

void CItemTime::Initialize()
{ 
	m_Enabled = ( (GetRasterizer() != NULL) || (GetWinType() == RAINWINDOW_TYPE_BALLOONTIP) ); 
}

void CItemTime::ReadSettings(const char* iniFile, const char* section)
{
	CItemDynamic::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	if (GetPrivateProfileString(section, "Format", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_Format = tmpSz;
	}
	if(GetPrivateProfileString(section, "Location", m_Location.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_Location=tmpSz;
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

			m_NumOfComponents = GetPrivateProfileInt(section, "NumOfComponents", 10, iniFile);
			BMRast->SetNumOfComponents(m_NumOfComponents);

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

			FNRast->UpdateDimensions(ToString());
			SetRasterizer(FNRast);

			m_NumOfComponents = 1;
		}
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

	int x = m_X;
	int y = m_Y;
	const char* tmpSz;
	tmpSz = ToString();

	if ( (m_WinType == RAINWINDOW_TYPE_CALENDAR) && (m_RepeatType != REPEAT_TYPE_NO) )
	{
		if (x < 0)
		{
			x += background.GetWidth() / CConfig::Instance().GetHorizontalCount();
		}

		if (y < 0)
		{
			y += background.GetHeight() / CConfig::Instance().GetVerticalCount();
		}
	}
	else
	{
		if (x < 0)
		{
			x += background.GetWidth();
		}

		if (y < 0)
		{
			y += background.GetHeight();
		}
	}

	x += offset.x;
	y += offset.y;
	
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

LPCTSTR CItemTime::ToString()
{
	static char tmpSz[MAX_LINE_LENGTH];
	std::string text = m_Format;

	if (!m_Location.empty())
	{
		std::string::size_type start = text.find("%NM");
		if (start != -1)
		{
			std::string st = CTimeZones::Instance().GetTimeZoneName(m_Location.c_str());
			std::string::size_type start = text.find("%NM");
			while (start != -1)
			{
				text.replace (start,3,st.c_str());
				start = text.find("%NM");
			}
		}
	}

	CFileTime ft;
	ft.SetToLocalTime();
	if (!m_Location.empty())
		ft.SetWorldTimeAt(m_Location.c_str());
	struct tm newtime;
	ft.Get_tm(newtime);
	strftime(tmpSz, MAX_LINE_LENGTH, text.c_str(), &newtime);

	return tmpSz;
}

int CItemTime::GetW()
{
	if(m_Rasterizer) 
	{
		if(m_Rasterizer->GetHeight() > m_Rasterizer->GetWidth()) 
		{
			return m_Rasterizer->GetWidth();
		} 
		else 
		{
			return m_Rasterizer->GetWidth() / m_NumOfComponents;
		}
	}

	return 0;
}

int CItemTime::GetH()
{
	if(m_Rasterizer) 
	{
		if(m_Rasterizer->GetHeight() > m_Rasterizer->GetWidth()) 
		{
			return m_Rasterizer->GetHeight() / m_NumOfComponents;
		} 
		else 
		{
			return m_Rasterizer->GetHeight();
		}
	}

	return 0;
}
