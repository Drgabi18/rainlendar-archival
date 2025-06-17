// ItemWeekdays.h: interface for the CItemWeekdays class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMWEEKDAYS_H__371A4545_F1CB_11D3_92A3_0080AD90417B__INCLUDED_)
#define AFX_ITEMWEEKDAYS_H__371A4545_F1CB_11D3_92A3_0080AD90417B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Item.h"

class CItemWeekdays : public CItem  
{
public:
	CItemWeekdays();
	virtual ~CItemWeekdays();

	void Initialize();
	void Paint(CDC& dc);
};

#endif // !defined(AFX_ITEMWEEKDAYS_H__371A4545_F1CB_11D3_92A3_0080AD90417B__INCLUDED_)
