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
  $Header: /home/cvsroot/Rainlendar/Plugin/Language.h,v 1.4 2004/11/06 13:38:59 rainy Exp $

  $Log: Language.h,v $
  Revision 1.4  2004/11/06 13:38:59  rainy
  no message

  Revision 1.3  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.2  2003/08/09 16:34:53  Rainy
  Added new strings.
  Profiles can be translated.

  Revision 1.1  2003/06/15 09:38:03  Rainy
  no message


*/

#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__

#pragma warning(disable: 4786)

#include <string>
#include <vector>
#include <map>

class CLanguage
{
public:
    CLanguage();
    ~CLanguage();

	void Clear();

    bool ScanLanguages(const char* path);
    const std::vector<std::string>& GetLanguages();
    bool ChangeLanguage(int index);
    const char* GetString(const char* section, int index);
    void SetDefaultLanguage();

    const char* GetTranslatedProfile(const char* profile);
    const char* GetOriginalProfile(const char* profile);

private:
    void SetString(const char* section, int index, const char* string);

    std::string m_Path;
    std::vector<std::string> m_Languages;
    std::map<std::string, int> m_IndexMap;
    std::vector< std::vector<std::string>* > m_Strings; 
    std::map<std::string, std::string> m_ProfileMap;
};

#endif