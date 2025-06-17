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
  $Header: /home/cvsroot/Rainlendar/Library/ItemDynamic.cpp,v 1.2 2005/10/14 17:05:41 rainy Exp $

  $Log: ItemDynamic.cpp,v $
  Revision 1.2  2005/10/14 17:05:41  rainy
  no message

  Revision 1.1  2005/09/08 16:09:54  rainy
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
#include "ItemDynamic.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemDynamic::CItemDynamic() : CItem()
{
	m_WinType = RAINWINDOW_TYPE_CALENDAR;
	m_X = 0;
	m_Y = 0;
	m_IncludeSize = false;
	m_RepeatType = REPEAT_TYPE_NO;
}

CItemDynamic::~CItemDynamic()
{
}

void CItemDynamic::Initialize()
{ 
	m_Enabled = ( (GetRasterizer() != NULL) && (GetWinType() != RAINWINDOW_TYPE_BALLOONTIP) ); 
}

void CItemDynamic::ReadSettings(const char* iniFile, const char* section)
{
	CItem::ReadSettings(iniFile, section);

	m_X = GetPrivateProfileInt( section, "X", 0, iniFile);
 	m_Y = GetPrivateProfileInt( section, "Y", 0, iniFile);

	m_WinType = (RAINWINDOW_TYPE)GetPrivateProfileInt(section, "Window", 0, iniFile);

	m_Enabled=(1==GetPrivateProfileInt( section, "Enable", 1, iniFile))?true:false;
	m_RepeatType = (REPEAT_TYPE)GetPrivateProfileInt(section, "Repeat", 0, iniFile);	
	m_IncludeSize = GetPrivateProfileInt(section, "IncludeSize", 0, iniFile) == 1;
}

void CItemDynamic::WriteSettings()
{
	char tmpSz[256];
	std::string INIPath = m_SettingsFile;

	// Only the enable/disable state need to be written since other settings cannot be edited
	sprintf(tmpSz, "%i", m_Enabled);
	WritePrivateProfileString( m_Section.c_str(), "Enable", tmpSz, INIPath.c_str() );
}
