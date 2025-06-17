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
  $Header: /home/cvsroot/Rainlendar/Server/IniFile.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: IniFile.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.2  2005/03/01 17:43:20  rainy
  Linux fixes

  Revision 1.1  2004/11/14 11:36:42  rainy
  Initial version.


*/

#if !defined(_INIFILE_H_)
#define _INIFILE_H_

#pragma warning(disable: 4786)

#include <string>
#include <map>
#include <vector>

class CIniFile
{
public:
	CIniFile(const char* filename, const char* header);
	~CIniFile();

	bool LoadFile();
	bool SaveFile();

	int GetFileVersion() { return m_FileVersion; };
	void SetFileVersion(int version) { m_FileVersion = version; };

	int GetSectionCount() { return m_Sections.size(); };
	const char* GetSectionName(int index);
	const char* GetSectionData(int index, const char* key);

	int CreateSection(const char* section);
	bool AddSectionData(int index, const char* key, const char* data);

private:
	std::string m_Filename;
	std::string m_Header;
	int m_FileVersion;

	struct Section
	{
		std::string section;
		std::map<std::string, std::string> data;
	};

	std::vector<Section*> m_Sections;
};

#endif
