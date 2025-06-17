// ItemMonth.h: interface for the CItemMonth class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMMONTH_H__371A4546_F1CB_11D3_92A3_0080AD90417B__INCLUDED_)
#define AFX_ITEMMONTH_H__371A4546_F1CB_11D3_92A3_0080AD90417B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Item.h"

class CItemMonth : public CItem  
{
public:
	CItemMonth();
	virtual ~CItemMonth();

	void Initialize();
	void Paint(CDC& dc);

	int GetX();
	int GetY();
	int GetW();
	int GetH();
};

#endif // !defined(AFX_ITEMMONTH_H__371A4546_F1CB_11D3_92A3_0080AD90417B__INCLUDED_)
