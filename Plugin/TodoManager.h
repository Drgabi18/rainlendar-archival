/*
  Copyright (C) 2003 Kimmo Pekkola

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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/TodoManager.h,v 1.1 2003/10/27 17:34:54 Rainy Exp $

  $Log: TodoManager.h,v $
  Revision 1.1  2003/10/27 17:34:54  Rainy
  Initial version.

*/

#ifndef __TODOMANAGER_H__
#define __TODOMANAGER_H__

#pragma warning(disable: 4786)

#include <string>
#include <vector>

enum TODO_TYPE
{
	TODO_TYPE_TODO,
	TODO_TYPE_DONE,
	TODO_TYPE_NOTE
};

class CTodoManager
{
public:
    CTodoManager();
    ~CTodoManager();

	void Initialize(const char* file);

    bool Read();
    bool Write();

	std::vector<std::string>& GetTodos() { return m_Todos; }
	std::vector<std::string>& GetDones() { return m_Dones; }
	std::vector<std::string>& GetNotes() { return m_Notes; }

private:
	std::string m_Filename;
	std::vector<std::string> m_Todos;
	std::vector<std::string> m_Dones;
	std::vector<std::string> m_Notes;
};

#endif