// ItemEvent.h: interface for the CItemEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMEVENT_H__C0B109F3_3CA7_4DFE_9152_69C1C479DE2B__INCLUDED_)
#define AFX_ITEMEVENT_H__C0B109F3_3CA7_4DFE_9152_69C1C479DE2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Item.h"
#include "Event.h"

class CCalendarWindow;

class CItemEvent : public CItem  
{
public:
	CItemEvent();
	virtual ~CItemEvent();

	void Initialize();
	void Paint(CDC& dc);

	CEvent* GetEvent(int Index) { return m_Events[Index]; };

	void AddToolTips(CCalendarWindow* CalendarWnd);

protected:
	void AddToolTip(CCalendarWindow* CalendarWnd, RECT* Rect, int Day);
	void RemoveToolTip(CCalendarWindow* CalendarWnd, int Day);

	void ReadEvents();

	CEvent* m_Events[32];
};

#endif // !defined(AFX_ITEMEVENT_H__C0B109F3_3CA7_4DFE_9152_69C1C479DE2B__INCLUDED_)
