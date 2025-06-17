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
  $Header: /home/cvsroot/Rainlendar/Plugin/Event.h,v 1.12 2004/04/24 11:19:35 rainy Exp $

  $Log: Event.h,v $
  Revision 1.12  2004/04/24 11:19:35  rainy
  SetEveryNth doesn't return 0 anymore.

  Revision 1.11  2004/01/10 15:16:05  rainy
  The start time is parsed from the message.

  Revision 1.10  2003/05/26 18:44:40  Rainy
  Added more consts.

  Revision 1.9  2003/05/25 18:09:20  Rainy
  Gets are now consts

  Revision 1.8  2003/05/07 19:13:49  rainy
  Added permanent and timestamp.

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
	CEventMessage(bool Permanent = true);
	CEventMessage(const CEventMessage& event);
	~CEventMessage();

	const std::string& GetMessage() const { return m_Message; };
	void SetMessage(const std::string& Message);

	void SetProfile(const std::string& Profile ) { m_Profile=Profile; };
	void SetType(EVENT_TYPE Type ) { m_Type=Type; };
	void SetEveryNth(int EveryNth ) { m_EveryNth = max(1, EveryNth); };
	void SetValidUntil(int ValidUntil ) { m_ValidUntil=ValidUntil; };
	void SetDate(int Date ) { m_Date=Date; };
    void SetCount(int Count ) { m_Count=Count; };
	void SetDeleted() { m_Deleted=true; };
	void SetID(int ID) { m_ID = ID; };
	void SetTimeStamp(time_t timeStamp) { m_TimeStamp = timeStamp; };

    const std::string& GetProfile() const { return m_Profile; };
	EVENT_TYPE GetType() const { return m_Type; };
	int GetEveryNth() const { return m_EveryNth; };
	int GetValidUntil() const { return m_ValidUntil; };
	int GetDate() const { return m_Date; };
	int GetCount() const { return m_Count; };
	bool IsDeleted() const { return m_Deleted; };
	int GetID() const { return m_ID; };
	int IsPermanent() const { return m_Permanent; };
	int GetTimeStamp() const { return m_TimeStamp; };

	// No setter for this since it's parsed from the message
	int GetStartTime() const { return m_StartTime; };

	bool IsShown() { return m_Shown; }
	void SetShown(bool shown) { m_Shown = shown; }

	const char* GetTypeText() const;
	const char* GetDateText() const;

	CEventMessage& operator=(const CEventMessage& event);

	static int DateToValue(int day, int month, int year);
	static void ValueToDate(int value, int* day, int* month, int* year);
	static int CalculateNumOfDays(int startValue, int endValue);
    static void GetDate(double dtSrc, int *year, int *month, int *day, int *hour, int *minute, int *second);

private:
    void UpdateTimeStamp();

	std::string m_Message;
	std::string m_Profile;
	int m_ValidUntil;
	int m_EveryNth;
	int m_Date;
	int m_Count;
	int m_ID;
	int m_StartTime;
	EVENT_TYPE m_Type;
	bool m_Deleted;
	bool m_Permanent;
    time_t m_TimeStamp;

	bool m_Shown;

	static int c_CurrentID;
};

#endif
