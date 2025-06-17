/********************************************************************
       Copyright (c) 2001, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from http://www.codeproject.com/datetime/dateclass.asp
       Copyright (c), Richard Stringer

       filename :  simpletime.h
       author   :  Lee Patterson (lee@antws.com)
*********************************************************************/

#ifdef WIN32
#include "stdafx.h"
#endif

#include <stdio.h>
#include <time.h>

#include "simpletime.h"
#include "Parseit.h"

using namespace ssobjects;

CSimpleTime::CSimpleTime()
  : m_currentTime(0),m_hour(0),m_minute(0),m_second(0)
{
  memset(m_szFormated,0,sizeof(m_szFormated));
  m_currentTime = GetCurrentTime();
  SetTime();
}

CSimpleTime::CSimpleTime(unsigned seconds)
  : m_currentTime(seconds),m_hour(0),m_minute(0),m_second(0)
{
  memset(m_szFormated,0,sizeof(m_szFormated));
  SetTime();
}

CSimpleTime::CSimpleTime(const char* time)
  : m_currentTime(0),m_hour(0),m_minute(0),m_second(0)
{
  memset(m_szFormated,0,sizeof(m_szFormated));
  CParseIt ParseIt(time,"/.-:");      
  ParseIt.Parse();

  unsigned hour=0,minute=0,second=0;           
  unsigned N=ParseIt.GetNumFields();

  hour=(int)ParseIt.GetField(1);  
  if(N > 1)
    minute=(int)ParseIt.GetField(2);    
  if(N > 2)
    second=(int)ParseIt.GetField(3);    

  m_currentTime = Convert(hour,minute,second);
  SetTime();
}

//
// Generate a string based on current time settings
//
void CSimpleTime::FormatOutput()
{
  sprintf(m_szFormated,"%02d:%02d:%02d",GetHour(),GetMinute(),GetSecond());
}

//
// fill in hour, minute, second and formated string for time
//
void CSimpleTime::SetTime()
{
  if(IsValid(m_currentTime))
  {
    m_hour   = m_currentTime/SECS_PER_HOUR;
    m_minute = (m_currentTime-m_hour*SECS_PER_HOUR)/60;
    m_second = m_currentTime-m_hour*SECS_PER_HOUR-m_minute*SECS_PER_MIN;
    FormatOutput();
  }
}

unsigned CSimpleTime::GetCurrentTime()
{
  long time_val ;
  struct tm *tm_ptr ;
  unsigned hour,minute,second;

  time( (time_t *)&time_val) ;
  tm_ptr = localtime( (time_t *)&time_val) ;
    
  hour=tm_ptr->tm_hour;
  minute=tm_ptr->tm_min;
  second=tm_ptr->tm_sec;
    
  return Convert(hour,minute,second);
}

unsigned CSimpleTime::Convert(unsigned hour,unsigned minute,unsigned second)
{
  return ( hour*SECS_PER_HOUR + minute*SECS_PER_MIN + second );
}

unsigned CSimpleTime::GetHour()
{
  return m_hour;
}

unsigned CSimpleTime::GetMinute()
{
  return m_minute;
}

unsigned CSimpleTime::GetSecond()
{
  return m_second;
}

CSimpleTime::operator PTIMESTRING()
{
  return m_szFormated;
}

CSimpleTime& CSimpleTime::operator = (const unsigned time)
{
  if(IsValid(time))
  {
    m_currentTime = time;
    SetTime();
  }
  return *this;
}

CSimpleTime& CSimpleTime::operator = (const char* time)
{
  CParseIt ParseIt(time,"/.-:");      
  ParseIt.Parse();

  unsigned hour=0,minute=0,second=0;           
  unsigned N=ParseIt.GetNumFields();

  hour=(int)ParseIt.GetField(1);  
  if(N > 1)
    minute=(int)ParseIt.GetField(2);  
  if(N > 2)
    second=(int)ParseIt.GetField(3);  

  m_currentTime = Convert(hour,minute,second);
  SetTime();
  return *this;
}

CSimpleTime CSimpleTime::operator += (const unsigned time)
{
  m_currentTime+=time;
  while(m_currentTime >= SECS_PER_DAY)
    m_currentTime -= SECS_PER_DAY;    //wrap past midnight 
  SetTime();
  return *this;
}

CSimpleTime CSimpleTime::operator -= (const unsigned time)
{
  m_currentTime-=time;
  while(m_currentTime >= SECS_PER_DAY)
    m_currentTime -= SECS_PER_DAY;    //wrap past midnight 
  SetTime();
  return *this;
}

bool CSimpleTime::operator == (const unsigned time) const
{
  return m_currentTime == time;
}

bool CSimpleTime::operator < (const unsigned time) const
{
  return m_currentTime < time;
}

bool CSimpleTime::operator > (const unsigned time) const
{
  return m_currentTime > time;
}

bool CSimpleTime::operator <= (const unsigned time) const
{
  return m_currentTime <= time;
}

bool CSimpleTime::operator >= (const unsigned time) const
{
  return m_currentTime >= time;
}


bool CSimpleTime::operator == (const CSimpleTime& time) const
{
  return m_currentTime == time.m_currentTime;
}

bool CSimpleTime::operator < (const CSimpleTime& time) const
{
  return m_currentTime < time.m_currentTime;
}

bool CSimpleTime::operator > (const CSimpleTime& time) const
{
  return m_currentTime > time.m_currentTime;
}

bool CSimpleTime::operator <= (const CSimpleTime& time) const
{
  return m_currentTime <= time.m_currentTime;
}

bool CSimpleTime::operator >= (const CSimpleTime& time) const
{
  return m_currentTime >= time.m_currentTime;
}
