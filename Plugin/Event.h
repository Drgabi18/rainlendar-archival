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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Event.h,v 1.7 2002/11/25 17:10:21 rainy Exp $

  $Log: Event.h,v $
  Revision 1.7  2002/11/25 17:10:21  rainy
  Added SetID()

  Revision 1.6  2002/08/24 11:12:19  rainy
  Added some trimming support.

  Revision 1.5  2002/08/03 16:17:44  rainy
  Added new variables to support repeating events.

  Revision 1.4  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.3  2002/01/27 16:03:53  rainy
  Changed CEvent to CEventMessage to avoid name clash

  Revision 1.2  2002/01/10 16:46:56  rainy
  Now holds the bitmap and color too.

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef __EVENT_H__
#define __EVENT_H__

#include <string>

enum EVENT_TYPE 
{
	EVENT_SINGLE,
	EVENT_DAILY,
	EVENT_WEEKLY,
	EVENT_MONTHLY,
	EVENT_ANNUALLY,
};

class CEventMessage  
{
public:
	CEventMessage();
	CEventMessage(const CEventMessage& event);
	~CEventMessage();

	const std::string& GetMessage() { return m_Message; };
	void SetMessage(const std::string& Message );

	const std::string& GetProfile() { return m_Profile; };
	void SetProfile(const std::string& Profile ) { m_Profile=Profile; };
	EVENT_TYPE GetType() { return m_Type; };
	void SetType(EVENT_TYPE Type ) { m_Type=Type; };
	int GetEveryNth() { return m_EveryNth; };
	void SetEveryNth(int EveryNth ) { m_EveryNth=EveryNth; };
	int GetValidUntil() { return m_ValidUntil; };
	void SetValidUntil(int ValidUntil ) { m_ValidUntil=ValidUntil; };
	int GetDate() { return m_Date; };
	void SetDate(int Date ) { m_Date=Date; };
	int GetCount() { return m_Count; };
	void SetCount(int Count ) { m_Count=Count; };
	bool IsDeleted() { return m_Deleted; };
	void SetDeleted() { m_Deleted=true; };
	int GetID() { return m_ID; };
	void SetID(int ID) { m_ID = ID; };

	const char* GetTypeText();

	CEventMessage& operator=(const CEventMessage& event);

	static int DateToValue(int day, int month, int year);
	static void ValueToDate(int value, int* day, int* month, int* year);
	static int CalculateNumOfDays(int startValue, int endValue);

private:
	std::string m_Message;
	std::string m_Profile;
	int m_ValidUntil;
	int m_EveryNth;
	int m_Date;
	int m_Count;
	int m_ID;
	EVENT_TYPE m_Type;
	bool m_Deleted;

	static int c_CurrentID;
};

#endif
