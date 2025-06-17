// ItemToday.h: interface for the CItemToday class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMTODAY_H__371A4544_F1CB_11D3_92A3_0080AD90417B__INCLUDED_)
#define AFX_ITEMTODAY_H__371A4544_F1CB_11D3_92A3_0080AD90417B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Item.h"

class CItemToday : public CItem  
{
public:
	CItemToday();
	virtual ~CItemToday();

	void Initialize();
	void Paint(CDC& dc);

};

#endif // !defined(AFX_ITEMTODAY_H__371A4544_F1CB_11D3_92A3_0080AD90417B__INCLUDED_)
