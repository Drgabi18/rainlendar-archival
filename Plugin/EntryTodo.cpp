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
  $Header: /home/cvsroot/Rainlendar/Plugin/EntryTodo.cpp,v 1.2 2004/12/05 18:32:37 rainy Exp $

  $Log: EntryTodo.cpp,v $
  Revision 1.2  2004/12/05 18:32:37  rainy
  Added profile.

  Revision 1.1  2004/11/06 13:38:58  rainy
  no message

*/
#pragma warning(disable: 4996)

#include "EntryTodo.h"

CEntryTodo::CEntryTodo() : CEntryItem()
{
	m_Item = new RainlendarTodo;
	memset(m_Item, 0, sizeof(RainlendarTodo));
	m_Item->size = sizeof(RainlendarTodo);
	m_Item->type = RAINLENDAR_TYPE_TODO; 

	CoCreateGuid(&m_Item->guid);	// Create unique guid for the event
	UpdateTimeStamp();
}

CEntryTodo::CEntryTodo(CEntryItem* item)
{
	m_Item = new RainlendarTodo;
	m_Item->type = RAINLENDAR_TYPE_TODO;
	Rainlendar_CopyItem(item->GetRainlendarItem(), m_Item);
	m_PluginID = item->GetPluginID();
}

CEntryTodo::CEntryTodo(RainlendarItem* item)
{
	m_Item = new RainlendarTodo;
	m_Item->type = RAINLENDAR_TYPE_TODO;
	Rainlendar_CopyItem(item, m_Item);
	m_PluginID = 0;
}

CEntryTodo::~CEntryTodo()
{
}

void CEntryTodo::SetMessage(LPCTSTR text)
{
	if (GetRainlendarTodo()->message)
	{
		free(GetRainlendarTodo()->message);
		GetRainlendarTodo()->message = NULL;
	}
	if (text)
	{
		GetRainlendarTodo()->message = strdup(text);
	}
}

void CEntryTodo::SetProfile(LPCTSTR text)
{
	if (GetRainlendarTodo()->profile)
	{
		free(GetRainlendarTodo()->profile);
		GetRainlendarTodo()->profile = NULL;
	}
	if (text)
	{
		GetRainlendarTodo()->profile = strdup(text);
	}
}