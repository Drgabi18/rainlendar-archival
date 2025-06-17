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
  $Header: /home/cvsroot/Rainlendar/Plugin/EntryTodo.h,v 1.2 2004/12/05 18:32:37 rainy Exp $

  $Log: EntryTodo.h,v $
  Revision 1.2  2004/12/05 18:32:37  rainy
  Added profile.

  Revision 1.1  2004/11/06 13:38:58  rainy
  no message

*/

#ifndef _ENTRYTODO_H_
#define _ENTRYTODO_H_

#include <string>
#include "EntryItem.h"
#include "RainlendarAPI.h"

class CEntryTodo : public CEntryItem
{
public:
	CEntryTodo();
	CEntryTodo(CEntryItem* item);
	CEntryTodo(RainlendarItem* item);
	virtual ~CEntryTodo();
	
	RainlendarTodo* GetRainlendarTodo() { return (RainlendarTodo*)m_Item; }

	LPCTSTR GetMessage() { return GetRainlendarTodo()->message; };
	void SetMessage(LPCTSTR text);

	LPCTSTR GetProfile() { return GetRainlendarTodo()->profile; };
	void SetProfile(LPCTSTR text);

	TODO_TYPE GetTodoType() { return GetRainlendarTodo()->todoType; };
	void SetTodoType(TODO_TYPE type) { GetRainlendarTodo()->todoType = type; };

	INT GetPosition() { return GetRainlendarTodo()->position; };
	void SetPosition(INT position) { GetRainlendarTodo()->position = position; };

	bool IsChecked() { return GetRainlendarTodo()->checked; };
	void SetChecked(bool checked) { GetRainlendarTodo()->checked = checked; };
};

#endif
