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
  $Header: /home/cvsroot/Rainlendar/Library/ItemTime.h,v 1.3 2005/10/14 17:05:29 rainy Exp $

  $Log: ItemTime.h,v $
  Revision 1.3  2005/10/14 17:05:29  rainy
  no message

  Revision 1.2  2005/09/08 16:09:12  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.2  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.1  2004/11/06 13:38:59  rainy
  no message

*/

#ifndef __ITEMTIME_H__
#define __ITEMTIME_H__

#include "ItemDynamic.h"

class CFileTime;

class CItemTime : public CItemDynamic
{
public:
	CItemTime();
	virtual ~CItemTime();

	virtual void Initialize();
	virtual void ReadSettings(const char* filename, const char* section);
	virtual void Paint(CImage& background, POINT offset);
	virtual bool NeedsUpdating() { return true; };

	int GetW();
	int GetH();

    virtual LPCTSTR ToString();
    
protected:
	std::string m_Format;
	int m_NumOfComponents;
	std::string m_Location;	// Location - From Control Panel's Date & Time
};

#endif
