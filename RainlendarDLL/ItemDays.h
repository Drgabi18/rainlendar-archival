// ItemDays.h: interface for the CItemDays class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMDAYS_H__7F453363_F122_11D3_92A3_0080AD90417B__INCLUDED_)
#define AFX_ITEMDAYS_H__7F453363_F122_11D3_92A3_0080AD90417B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Item.h"

class CItemDays : public CItem  
{
public:
	CItemDays();
	virtual ~CItemDays();

	void Initialize();
	void Paint(CDC& dc);
	int HitTest(int x, int y);

	int GetX();
	int GetY();
	int GetW();
	int GetH();

private:
	int m_DaysInMonth;
};

#endif // !defined(AFX_ITEMDAYS_H__7F453363_F122_11D3_92A3_0080AD90417B__INCLUDED_)
