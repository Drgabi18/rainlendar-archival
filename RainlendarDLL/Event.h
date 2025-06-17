// Event.h: interface for the CEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENT_H__3B699F74_B75F_4CDC_ACCA_F98A33D0D58B__INCLUDED_)
#define AFX_EVENT_H__3B699F74_B75F_4CDC_ACCA_F98A33D0D58B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEvent  
{
public:
	CEvent();
	~CEvent();

	CString& GetMessage() { return m_Message; };
	void SetMessage(CString& Message ) { m_Message=Message; };
	void SetMessage(char* Message ) { m_Message=Message; };

private:
	CString m_Message;

};

#endif // !defined(AFX_EVENT_H__3B699F74_B75F_4CDC_ACCA_F98A33D0D58B__INCLUDED_)
