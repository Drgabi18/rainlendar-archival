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
  $Header: /home/cvsroot/Rainlendar/Library/Item.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: Item.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.8  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.7  2004/11/06 13:38:59  rainy
  no message

  Revision 1.6  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.5  2003/10/27 17:37:50  Rainy
  Config is now singleton.

  Revision 1.4  2003/10/04 14:50:18  Rainy
  Always drawn events don't count as events.

  Revision 1.3  2003/06/15 09:49:11  Rainy
  Added support for multiple calendars.

  Revision 1.2  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/
#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "Item.h"

CItem::CItem()
{
	m_Rasterizer = NULL;
	m_Section = "Rainlendar";
	m_Enabled = false;
}

CItem::~CItem()
{
	if(m_Rasterizer) delete m_Rasterizer;
}

void CItem::SetRasterizer(CRasterizer* Rasterizer)
{
	if(m_Rasterizer) delete m_Rasterizer;	// Kill the old
	m_Rasterizer=Rasterizer;
}

void CItem::ReadSettings(const char* filename, const char* section)
{
	m_Section = section;
	m_SettingsFile = filename;
}
