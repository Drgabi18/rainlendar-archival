// Item.h: interface for the CItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEM_H__7F453361_F122_11D3_92A3_0080AD90417B__INCLUDED_)
#define AFX_ITEM_H__7F453361_F122_11D3_92A3_0080AD90417B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Rasterizer.h"

// DayTypes
#define NORMAL 0
#define TODAY 1
#define EVENT 2

class CItem  
{
public:
	CItem();
	virtual ~CItem();

	virtual void Initialize() = 0;
	virtual void Paint(CDC& dc) = 0;

	CRasterizer* GetRasterizer() { return m_Rasterizer; };
	void SetRasterizer(CRasterizer* Rasterizer);

	virtual int GetX() { return 0; };
	virtual int GetY() { return 0; };
	virtual int GetW() { return 0; };
	virtual int GetH() { return 0; };

	static int GetDayType(int Index) { return c_DayTypes[Index]; };
	static void SetDayType(int Index, int DayType) { c_DayTypes[Index]|=DayType; };		// Note the OR !!

protected:
	CRasterizer* m_Rasterizer;

	static int c_DayTypes[32];
};

#endif // !defined(AFX_ITEM_H__7F453361_F122_11D3_92A3_0080AD90417B__INCLUDED_)
