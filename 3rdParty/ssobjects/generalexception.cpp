/********************************************************************
        Copyright (c) 2001, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        created  :  03/21/2000 9:00pm
        filename :  generalexception.cpp
        author   :  Lee Patterson (lee@antws.com)
        
        purpose  :  base exception class
*********************************************************************/

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "generalexception.h"

using namespace ssobjects;

GeneralException::GeneralException(const char* pszMessage,const char* pszFname,const int iLine)
		: m_sMessage(pszMessage),m_sFullMessage(),m_nError(0)
{
	m_sFullMessage.format("%s in file %s line %d",pszMessage,pszFname,iLine);
}

