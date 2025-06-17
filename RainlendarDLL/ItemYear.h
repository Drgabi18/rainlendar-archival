// ItemYear.h: interface for the CItemYear class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMYEAR_H__371A4547_F1CB_11D3_92A3_0080AD90417B__INCLUDED_)
#define AFX_ITEMYEAR_H__371A4547_F1CB_11D3_92A3_0080AD90417B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Item.h"

class CItemYear : public CItem  
{
public:
	CItemYear();
	virtual ~CItemYear();

	void Initialize();
	void Paint(CDC& dc);

	int GetX();
	int GetY();
	int GetW();
	int GetH();
};

#endif // !defined(AFX_ITEMYEAR_H__371A4547_F1CB_11D3_92A3_0080AD90417B__INCLUDED_)
