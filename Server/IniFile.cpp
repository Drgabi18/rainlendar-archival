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
  $Header: /home/cvsroot/Rainlendar/Server/IniFile.cpp,v 1.2 2005/07/11 18:27:55 rainy Exp $

  $Log: IniFile.cpp,v $
  Revision 1.2  2005/07/11 18:27:55  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.2  2005/03/01 17:02:15  rainy
  Added file.h include

  Revision 1.1  2004/11/14 11:36:42  rainy
  Initial version.


*/

#include "IniFile.h"
#include <file.h>

CIniFile::CIniFile(const char* filename, const char* header)
{
	m_Filename = filename;
	m_Header = header;
	m_FileVersion = 0;
}

CIniFile::~CIniFile()
{
	for (int i = 0; i < m_Sections.size(); i++)
	{
		delete m_Sections[i];
	}
}

bool CIniFile::LoadFile()
{
	FILE* file = fopen(m_Filename.c_str(), "r");

	if (file)
	{
		// Read the whole file to memory
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		if (size > 0)
		{
			fseek(file, 0, SEEK_SET);

			char* fileBuffer = new char[size + 1];
			fread(fileBuffer, size, sizeof(char), file);
			if (ferror(file) == 0)
			{
				Section* newSection = NULL;
				fileBuffer[size] = 0;	// Just to make sure that the string ends

				char* pos = fileBuffer;
				while (pos - fileBuffer <= size)
				{
					// replace \n with \0
					char* endPos = strchr(pos, '\n');
					if (endPos)
					{
						*endPos = 0;
					}

					// Remove also \r
					endPos = strchr(pos, '\r');
					if (endPos)
					{
						*endPos = 0;
					}

					if (*pos != '#')	// This is not a comment
					{
						if (*pos == '[')	// New section
						{
							if (newSection)
							{
								m_Sections.push_back(newSection);
								newSection = NULL;
							}

							endPos = strchr(pos, ']');

							if (endPos)
							{
								*endPos = 0;

								newSection = new Section;
								newSection->section = pos + 1;
							}
						}
						else
						{
							if (newSection)
							{
								endPos = strchr(pos, '=');
								if (endPos)
								{
									*endPos = 0;

									// replace \n with newline
									std::string tmpString = endPos + 1;

									int n = tmpString.find("\\");
									while (n != -1)
									{
										if (tmpString[n + 1] == '\\') 
										{
											tmpString.replace(n, 2, "\\");
											n = tmpString.find("\\", n + 1);
										}
										else if (tmpString[n + 1] == 'n')
										{
											tmpString.replace(n, 2, "\r\n");
											n = tmpString.find("\\", n + 2);
										}
										else
										{
											n = tmpString.find("\\", n + 1);
										}
									}
									
									newSection->data[pos] = tmpString;
									*endPos = '=';
								}
							}
						}
					}

					// Goto next line
					pos += strlen(pos) + 1;
				}
				// The first section is the header

				// Store the last section
				if (newSection)
				{
					m_Sections.push_back(newSection);
					newSection = NULL;
				}
			}

			delete []  fileBuffer;
		}

		fclose(file);

		const char* version = GetSectionData(0, "Version");
		if (version != NULL)
		{
			m_FileVersion = atoi(version);

			// delete the first section (i.e. the header)
			delete m_Sections[0];
			m_Sections.erase(m_Sections.begin());
		}

		return true;
	}
	return false;
}

bool CIniFile::SaveFile()
{
	char buffer[256];
	FILE* file = fopen(m_Filename.c_str(), "w");

	if (file)
	{
		fputs("[", file);
		fputs(m_Header.c_str(), file);
		fputs("]\n", file);

		sprintf(buffer, "Version=%i\n\n", m_FileVersion);
		fputs(buffer, file);

		for (int i = 0; i < m_Sections.size(); i++)
		{
			fputs("[", file);
			fputs(m_Sections[i]->section.c_str(), file);
			fputs("]\n", file);

			std::map<std::string, std::string>::iterator iter = m_Sections[i]->data.begin();
			for ( ; iter != m_Sections[i]->data.end(); iter++)
			{
				fputs((*iter).first.c_str(), file);
				fputs("=", file);

				// Change \r\n to "\n"
				int pos;
				std::string message = (*iter).second;

				pos = message.find("\\");
				while (pos != -1)
				{
					message.replace(pos, 1, "\\\\");
					pos = message.find("\\", pos + 2);
				}

				pos = message.find("\r\n");
				while (pos != -1)
				{
					message.replace(pos, 2, "\\n");
					pos = message.find("\r\n", pos);
				}

				fputs(message.c_str(), file);
				fputs("\n", file);
			}
			fputs("\n", file);
		}

		fclose(file);
		return true;
	}
	return false;
}

const char* CIniFile::GetSectionName(int index)
{
	if (index >= 0 && index < m_Sections.size())
	{
		return m_Sections[index]->section.c_str();
	}
	return NULL;
}

const char* CIniFile::GetSectionData(int index, const char* key)
{
	if (index >= 0 && index < m_Sections.size())
	{
		// Check if the key is already in the section
		std::map<std::string, std::string>::iterator iter = m_Sections[index]->data.find(key);
		if (iter != m_Sections[index]->data.end())
		{
			return (*iter).second.c_str();
		}
	}
	return NULL;
}

int CIniFile::CreateSection(const char* section)
{
	Section* newSection = new Section;
	newSection->section = section;

	m_Sections.push_back(newSection);

	return m_Sections.size() - 1;
}

bool CIniFile::AddSectionData(int index, const char* key, const char* data)
{
	if (index >= 0 && index < m_Sections.size())
	{
		// New key
		if (data == NULL)
		{
			m_Sections[index]->data[key] = "";
		}
		else
		{
			m_Sections[index]->data[key] = data;
		}
		return true;
	}
	return false;
}
