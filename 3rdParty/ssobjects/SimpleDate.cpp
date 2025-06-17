/********************************************************************
       Copyright (c) 2001, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from http://www.codeproject.com/datetime/dateclass.asp
       Copyright (c), Richard Stringer

       filename : SimpleDate.cpp

       changes  : 24-AUG-2000 Lee Patterson (lee@antws.com)
                  - Removed MFC code so it will compile under other 
                    platforms.
                  - Made more functions use const.

       notes    : Most of the date alogrithms used in this class 
                  can be found at:
                  http://www.capecod.net/~pbaum/date/date0.htm

*********************************************************************/

#ifdef WIN32
#include "stdafx.h"
#endif

#ifndef WIN32
# include <ctype.h>
#endif

#include <time.h>
#include <stdio.h>
#include "Parseit.h"
#include "SimpleDate.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//#pragma message( "Compiling " __FILE__ ) 
//#pragma message( "Last modified on " __TIMESTAMP__ ) 




using namespace ssobjects;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static int  monthtotals[]=
    { 0, 0,  31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 } ;

static char* daysofweek[] =
{
    "Sun" ,
    "Mon" ,
    "Tue" ,
    "Wed" ,
    "Thu" ,
    "Fri" ,
    "Sat" ,
} ;

static char* days_of_week[] =
{
    "Sunday" ,
    "Monday" ,
    "Tuesday" ,
    "Wednesday" ,
    "Thursday" ,
    "Friday" ,
    "Saturday" ,
} ;

static char* monthsofyear[] =
{
    "" ,
    "Jan" ,
    "Feb" ,
    "Mar" ,
    "Apr" ,
    "May" ,
    "Jun" ,
    "Jul" ,
    "Aug" ,
    "Sep" ,
    "Oct" ,
    "Nov" ,
    "Dec" ,
} ;

static char* months_of_year[] =
{
    "" ,
    "January" ,
    "February" ,
    "March" ,
    "April" ,
    "May" ,
    "June" ,
    "July" ,
    "August" ,
    "September" ,
    "October" ,
    "November" ,
    "December" ,
} ;

//--------------------------------------------------------------------
// Function name  : CSimpleDate::CSimpleDate
// Description      : Simple Constructor object is set to todays date
// Return type    : 
//--------------------------------------------------------------------
CSimpleDate::CSimpleDate(int FormatType)
        : m_Year(0),
          m_Month(0),
          m_Day(0),
          m_JulianDate(0),
          m_Format(FormatType),
          m_Hour(0),
          m_Min(0),
          m_Second(0),
          m_bPM(false)
{
  SetToday();
  m_JulianDate=ConvertToJulian(); 
  SetTime();
}


//--------------------------------------------------------------------
// Function name  : CSimpleDate::CSimpleDate
// Description      : paramaterized constructor object is set to date in string. Parsing will be done based on formattype
// Return type    : 
// Argument         : LPCSTR DateString
//--------------------------------------------------------------------

CSimpleDate::CSimpleDate(LPCSTR DateString,int FormatType)
        : m_Year(0),
          m_Month(0),
          m_Day(0),
          m_JulianDate(0),
          m_Format(FormatType),
          m_Hour(0),
          m_Min(0),
          m_Second(0),
          m_bPM(false)
{
  ParseDateString(DateString);
  m_JulianDate=ConvertToJulian(); 
  SetTime();
}



//--------------------------------------------------------------------
// Function name  : CSimpleDate::CSimpleDate
// Description      : 
// Return type    : 
// Argument         : long JD
// Argument         : int FormatType
//--------------------------------------------------------------------
CSimpleDate::CSimpleDate(long JD,int FormatType)
        : m_Year(0),
          m_Month(0),
          m_Day(0),
          m_JulianDate(JD),
          m_Format(FormatType),
          m_Hour(0),
          m_Min(0),
          m_Second(0),
          m_bPM(false)
{
  ConvertFromJulian();  // note: m_JulianDate must be set prior to this call
  SetTime();
}

//--------------------------------------------------------------------
// Function name  : CSimpleDate::~CSimpleDate
// Description      : 
// Return type    : 
//--------------------------------------------------------------------
CSimpleDate::~CSimpleDate()
{

}


//--------------------------------------------------------------------
// Function name  : CSimpleDate::SetToday
// Description      : 
// Return type    : bool 
//--------------------------------------------------------------------
bool CSimpleDate::SetToday()
{
  long time_val ;
   struct tm *tm_ptr ;

  time( (time_t *)&time_val ) ;
  tm_ptr = localtime( (time_t *)&time_val ) ;

  m_Year=1900 + tm_ptr->tm_year;
  m_Month=tm_ptr->tm_mon + 1;
  m_Day=tm_ptr->tm_mday;                 
  return true;
}



//--------------------------------------------------------------------
// Function name  : CSimpleDate::IsValid
// Description      : Hope that this is right and Y2K OK
// Return type    : bool 
//--------------------------------------------------------------------
bool CSimpleDate::IsValid() const
{
     int is_leap, month_days ;

     is_leap =  ( m_Year%4 == 0 && m_Year%100 != 0 || m_Year%400 == 0 ) ?  1 : 0 ;
    if(m_Month > 12 || m_Day > 31)
      return false;
     month_days = monthtotals[ m_Month+1 ] -  monthtotals[ m_Month] ;
     if ( m_Month == 2 )  month_days += is_leap ;

     if ( m_Year  < 0  ||
      m_Month < 1  ||  m_Month > 12  ||
      m_Day  < 1  ||  m_Day   > month_days )
      return false ;        
     return true;
}


//--------------------------------------------------------------------
// Function name  : CSimpleDate::ParseDateString
// Description      : 
// Return type    : bool 
// Argument         : LPCSTR TheDate
//--------------------------------------------------------------------
bool CSimpleDate::ParseDateString(LPCSTR TheDate)
{
  ParseDateString(TheDate,m_Month,m_Day,m_Year);
  return true;
}


//--------------------------------------------------------------------
// Function name  : CSimpleDate::ParseDateString
// Description      : See CParseIt class in Parseit.cpp for details
// Return type    : bool 
// Argument         : LPCSTR date
// Argument         : int& m
// Argument         : int& d
// Argument         : int& y
//--------------------------------------------------------------------
bool CSimpleDate::ParseDateString(LPCSTR date,int& m,int& d,int& y)
{
  CParseIt ParseIt(date,"/.-");      
  ParseIt.Parse();
  int one,two,three;           
  one=two=three=0;
  int N=ParseIt.GetNumFields();
  one=(int)ParseIt.GetField(1); 
  if(N > 1)
    two=(int)ParseIt.GetField(2); 
  if(N > 2)
    three=(int)ParseIt.GetField(3); 
  switch(m_Format)
  {
  case MMDDYY:
    m=one;d=two;y=three;
    if(y < 100)     // ajust two digit year to the range of 1900-1999 
      y+=1900;
    break;
  case DDMMYY:
    d=one;m=two;y=three;
    if(y < 100)
      y+=1900;
    break;
  case YYMMDD:
    y=one;m=two;d=three;
    if(y < 100)
      y+=1900;
    break;
  case MMDDYYYY:
    m=one;d=two;y=three;
    if(y < 100)
      y+=1900;
    break;
  case DDMMYYYY:
    d=one;m=two;y=three;
    if(y < 100)
      y+=1900;
    break;
  case YYYYMMDD:
    y=one;m=two;d=three;
    if(y < 100)
      y+=1900;
    break;
  default:
    m=y=d=0;
  }

  return true;

}

//--------------------------------------------------------------------
//  JULIAN DATE CONVERSION BELOW
//--------------------------------------------------------------------
// Function name  : CSimpleDate::ConvertToJulian
// Description      : 
// Return type    : long 
//--------------------------------------------------------------------
long CSimpleDate::ConvertToJulian()
{
  return ConvertToJulian(m_Month,m_Day,m_Year);

}
//--------------------------------------------------------------------
// Function name  : CSimpleDate::ConvertToJulian
// Description      : go to http://www.capecod.net/~pbaum/date/date0.htm for details
// Return type    : long 
// Argument         :  int month
// Argument         : int day
// Argument         : int year
//--------------------------------------------------------------------
long CSimpleDate::ConvertToJulian( int m,int d,int y)
{
  if(m < 3)
  {
      m = m + 12;
      y=y-1 ;
  }
  long jd = d + (153 * m - 457) / 5 + 365 * y + (y / 4) - (y / 100) + (y / 400) + 1721119;
  return jd;
}

//--------------------------------------------------------------------
// Function name  : CSimpleDate::ConvertFromJulian
// Description      : 
// Return type    : void 
//--------------------------------------------------------------------
void CSimpleDate::ConvertFromJulian()
{
  ConvertFromJulian(m_Month,m_Day,m_Year);
}
//--------------------------------------------------------------------
// Function name  : CSimpleDate::ConvertFromJulian
// Description      : goto http://www.capecod.net/~pbaum/date/date0.htm for details
// Return type    : void 
// Argument         : int Month
// Argument         : int DAy
// Argument         : int Year
//--------------------------------------------------------------------
void CSimpleDate::ConvertFromJulian(int& Month,int& Day,int& Year)
{

  long L = m_JulianDate + 68569;
    long N = (long) ((4*L)/146097) ;
    L = L - ((long)((146097 * N + 3)/4) );
    long I = (long) ((4000 *(L + 1)/1461001)) ;
    L = L - (long)((1461*I)/4) + 31; 
    long J = (long)((80*L)/2447); 
    Day = L - (long)((2447*J)/80);
    L=(long)(J/11) ;
    Month = J + 2 - 12*L;
    Year = 100*(N-49) + I + L ;
}

//--------------------------------------------------------------------
//  JULIAN DATE CONVERSION ABOVE
//--------------------------------------------------------------------
//--------------------------------------------------------------------
// Function name  : CSimpleDate::GetDayOfWeek
// Description      : goto http://www.capecod.net/~pbaum/date/date0.htm for details
// Return type    : int 
//--------------------------------------------------------------------
int CSimpleDate::GetDayOfWeek()
{
  if(!IsValid())
    return 0;
  int res= ((int) (m_JulianDate + 1.5)) % 7;
  return res;
}

//--------------------------------------------------------------------
// Function name  : CSimpleDate::GetFullDateString
// Description      : 
// Return type    : LPCSTR 
//--------------------------------------------------------------------
const char* 
CSimpleDate::GetFullDateString()
{
  if(!IsValid())
    return NULL;
  sprintf(m_DateString,"%s %s %02d %04d", daysofweek[GetDayOfWeek()],monthsofyear[m_Month],m_Day,m_Year);
  return m_DateString;
}

//--------------------------------------------------------------------
// Function name  : CSimpleDate::GetFullDateStringLong
// Description      : 
// Return type    : LPCSTR 
//--------------------------------------------------------------------
const char*
 CSimpleDate::GetFullDateStringLong()
{
  if(!IsValid())
    return NULL;
  sprintf(m_DateString,"%s %s %02d %04d", days_of_week[GetDayOfWeek()],months_of_year[m_Month],m_Day,m_Year);
  return m_DateString;
}


//--------------------------------------------------------------------
// Function name  : CSimpleDate::GetJulianDate
// Description      : 
// Return type    : long 
//--------------------------------------------------------------------
long CSimpleDate::GetJulianDate()
{
  if(!IsValid())
    return 0L;
  return m_JulianDate;
}


//--------------------------------------------------------------------
//  overloaded operators and copy constructors here
//--------------------------------------------------------------------
// Function name  : LPCSTR
// Description      : 
// Return type    : CSimpleDate::operator return LPCSTR
//--------------------------------------------------------------------
CSimpleDate::operator const char*() 
{
  if(!IsValid())
    return (LPCSTR)0;
  switch(m_Format)
  {
  case MMDDYY:
    sprintf(m_DateString,"%02d/%02d/%02d",m_Month,m_Day,m_Year > 99 ? m_Year-1900:m_Year);    
    return (LPCSTR)m_DateString;
  case DDMMYY:
    sprintf(m_DateString,"%02d/%02d/%02d",m_Day,m_Month,m_Year > 99 ? m_Year-1900:m_Year);    
    return (LPCSTR)m_DateString;
  case YYMMDD:
    sprintf(m_DateString,"%02d/%02d/%02d",m_Year > 99 ? m_Year-1900:m_Year,m_Month,m_Day);    
    return (LPCSTR)m_DateString;
  case MMDDYYYY:
    sprintf(m_DateString,"%02d/%02d/%04d",m_Month,m_Day,m_Year );   
    return (LPCSTR)m_DateString;
  case DDMMYYYY:
    sprintf(m_DateString,"%02d/%02d/%04d",m_Day,m_Month,m_Year);    
    return (LPCSTR)m_DateString;
  case YYYYMMDD:
    sprintf(m_DateString,"%04d/%02d/%02d",m_Year ,m_Month,m_Day);   
    return (LPCSTR)m_DateString;
  default:
    return (LPCSTR)NULL;
  }
}


//--------------------------------------------------------------------
// OPERATORS
//--------------------------------------------------------------------
// Function name  : long
// Description      : 
// Return type    : CSimpleDate::operator return long
//--------------------------------------------------------------------
CSimpleDate::operator long() const
{
  if(!IsValid())
    return 0L;
  return m_JulianDate;
}

//--------------------------------------------------------------------
// Function name  : =
// Description      : 
// Return type    : CSimpleDate::operator 
//--------------------------------------------------------------------
const CSimpleDate& CSimpleDate::operator =(const CSimpleDate& Date)
{
  if(*this==Date)
    return *this; //  self assignment
  m_Year=Date.m_Year;
  m_Month=Date.m_Month;
  m_Day=Date.m_Day;
  m_JulianDate=Date.m_JulianDate;
  m_Format=Date.m_Format;
  return *this;
}

//--------------------------------------------------------------------
// Function name  : =
// Description      : 
// Return type    : CSimpleDate::operator 
//--------------------------------------------------------------------
const CSimpleDate& CSimpleDate::operator =(LPCSTR Date)
{
  ParseDateString(Date);
  m_JulianDate=ConvertToJulian(); 
  return *this;
}
//--------------------------------------------------------------------
bool CSimpleDate::operator > (const CSimpleDate& Date) const 
{                        
  if(!IsValid())
    return false;
  return m_JulianDate > Date.m_JulianDate;
}     
//--------------------------------------------------------------------
bool CSimpleDate::operator >= (const CSimpleDate& Date) const
{                        
  if(!IsValid())
    return false;
  return m_JulianDate >= Date.m_JulianDate;
}     
//--------------------------------------------------------------------
bool CSimpleDate::operator < (const CSimpleDate& Date) const
{                        
  if(!IsValid())
    return false;
  return m_JulianDate < Date.m_JulianDate;
}     
//--------------------------------------------------------------------
bool CSimpleDate::operator <= (const CSimpleDate& Date) const
{                        
  if(!IsValid())
    return false;
  return m_JulianDate <= Date.m_JulianDate;
}     
//--------------------------------------------------------------------
bool CSimpleDate::operator == (const CSimpleDate& Date) const
{                        
  if(!IsValid())
    return false;
  return m_JulianDate == Date.m_JulianDate;
}     
//--------------------------------------------------------------------
bool CSimpleDate::operator != (const CSimpleDate& Date) const
{                        
  if(!IsValid())
    return false;
  return m_JulianDate != Date.m_JulianDate;
}     
//--------------------------------------------------------------------
bool CSimpleDate::operator > (LPCSTR Date) const
{                        
  if(!IsValid())
    return false;
  CSimpleDate TheDate(Date);
  if(!TheDate.IsValid())
    return false;
  return m_JulianDate > TheDate.m_JulianDate;
}     
//--------------------------------------------------------------------
bool CSimpleDate::operator < (LPCSTR Date) const
{                        
  if(!IsValid())
    return false;
  CSimpleDate TheDate(Date);
  if(!TheDate.IsValid())
    return false;
  return m_JulianDate < TheDate.m_JulianDate;
}     
//--------------------------------------------------------------------
bool CSimpleDate::operator >= (LPCSTR Date) const
{                        
  if(!IsValid())
    return false;
  CSimpleDate TheDate(Date);
  if(!TheDate.IsValid())
    return false;
  return m_JulianDate >= TheDate.m_JulianDate;
}     
//--------------------------------------------------------------------
bool CSimpleDate::operator <= (LPCSTR Date) const
{                        
  if(!IsValid())
    return false;
  CSimpleDate TheDate(Date);
  if(!TheDate.IsValid())
    return false;
  return m_JulianDate <= TheDate.m_JulianDate;
}     
//--------------------------------------------------------------------
bool CSimpleDate::operator == (LPCSTR Date) const
{                        
  if(!IsValid())
    return false;
  CSimpleDate TheDate(Date);
  if(!TheDate.IsValid())
    return false;
  return m_JulianDate == TheDate.m_JulianDate;
}     
//--------------------------------------------------------------------
bool CSimpleDate::operator != (LPCSTR Date) const     
{                        
  if(!IsValid())
    return false;
  CSimpleDate TheDate(Date);
  if(!TheDate.IsValid())
    return false;
  return m_JulianDate != TheDate.m_JulianDate;
}     


//--------------------------------------------------------------------
// Date Math routines
//--------------------------------------------------------------------
// Function name  : CSimpleDate::YearsOld
// Description      : 
// Return type    : int 
//--------------------------------------------------------------------
int CSimpleDate::YearsOld()
{
  if(!IsValid())
    return 0;
  CSimpleDate ToDay;
  if(ToDay <= *this)
    return 0;
  long t=ToDay.GetJulianDate();
  long b=GetJulianDate();
  return (int)((t-b)/365.2425);

}


//--------------------------------------------------------------------
// Function name  : CSimpleDate::AddMonths
// Description      : 
// Return type    : const CSimpleDate& 
// Argument         : int Mon
//--------------------------------------------------------------------
const CSimpleDate& CSimpleDate::AddMonths(int Mon)
{
  if(!IsValid())
    return *this;
  m_Month+=Mon;
  if(m_Month > 12)
  {
    while(m_Month > 12)
    {
      m_Month-=12;
      ++m_Year;
    }
  }
  AdjustDays();
    m_JulianDate=ConvertToJulian();
  return *this;
}


//--------------------------------------------------------------------
// Function name  : CSimpleDate::SubtractMonths
// Description      : 
// Return type    : const CSimpleDate& 
// Argument         : int Mon
//--------------------------------------------------------------------
const CSimpleDate& CSimpleDate::SubtractMonths(int Mon)
{
  if(!IsValid())
    return *this;
  if(Mon > 12)
  {
    int t= Mon/12;
    m_Year-=t;
    Mon=Mon-(t*12);
  }
  m_Month-=Mon;
  if(m_Month < 1)
  {
    --m_Year;
    m_Month+=12;
  }
  AdjustDays();
    m_JulianDate=ConvertToJulian();
  return *this;
}

//--------------------------------------------------------------------
// Function name  : CSimpleDate::AddYears
// Description      : 
// Return type    : const CSimpleDate& 
// Argument         : int Yrs
//--------------------------------------------------------------------
const CSimpleDate& CSimpleDate::AddYears(int Yrs)
{
  if(!IsValid())
    return *this;
  m_Year+=Yrs;
  AdjustDays();
  m_JulianDate=ConvertToJulian();
  return *this;

}


//--------------------------------------------------------------------
// Function name  : CSimpleDate::SubtractYears
// Description      : 
// Return type    : const CSimpleDate& 
// Argument         : int Yrs
//--------------------------------------------------------------------
const CSimpleDate& CSimpleDate::SubtractYears(int Yrs)
{
  if(!IsValid())
    return *this;
  m_Year-=Yrs;
  AdjustDays();
  m_JulianDate=ConvertToJulian();
  return *this;

}

//--------------------------------------------------------------------
// Function name  : CSimpleDate::AddDays
// Description      : 
// Return type    : const CSimpleDate& 
// Argument         : int Days
//--------------------------------------------------------------------
const CSimpleDate& CSimpleDate::AddDays(int Days)
{
  if(!IsValid())
    return *this;
  m_JulianDate+=Days;
  ConvertFromJulian(m_Month,m_Day,m_Year);
  return *this;
}


//--------------------------------------------------------------------
// Function name  : CSimpleDate::SubtractDays
// Description      : 
// Return type    : const CSimpleDate& 
// Argument         : int Days
//--------------------------------------------------------------------
const CSimpleDate& CSimpleDate::SubtractDays(int Days)
{
  if(!IsValid())
    return *this;
  m_JulianDate-=Days;
  ConvertFromJulian(m_Month,m_Day,m_Year);
  return *this;

}

//--------------------------------------------------------------------
// Function name  : CSimpleDate::AdjustDays
// Description      : 
// Return type    : void 
//--------------------------------------------------------------------
void CSimpleDate::AdjustDays()
{
   int is_leap, month_days ;
   is_leap =  ( m_Year%4 == 0 && m_Year%100 != 0 || m_Year%400 == 0 ) ?  1 : 0 ;
   month_days = monthtotals[ m_Month+1 ] -  monthtotals[ m_Month] ;
   if ( m_Month == 2 )  month_days += is_leap ;
  while(m_Day   > month_days)
    --m_Day;

}

//--------------------------------------------------------------------
//  Time functions
//--------------------------------------------------------------------
// Function name  : CSimpleDate::SetTime
// Description      : The only time function: Sets the variables for later use
// Return type    : void 
//--------------------------------------------------------------------
void CSimpleDate::SetTime()
{
    long time_val ;
    struct tm *tm_ptr ;

    time( (time_t *)&time_val) ;
    tm_ptr = localtime( (time_t *)&time_val) ;

  m_Hour=tm_ptr->tm_hour;
  m_Min=tm_ptr->tm_min;
  m_Second=tm_ptr->tm_sec;
  if(m_Hour > 12)
  {
    m_bPM=true;
    m_Hour-=12;
  }
  else
    m_bPM=false;

}

//--------------------------------------------------------------------
// Function name  : CSimpleDate::GetTimeString
// Description      : 
// Return type    : void 
// Argument         : LPSTR s
// Argument         : int nLen
// Argument         : bool AmPm
//--------------------------------------------------------------------
void CSimpleDate::GetTimeString(LPSTR s,int nLen,bool AmPm)
{
  if(!IsValid())
    return;

  SetTime();
  int Flag=0;
  if(AmPm==true)
  {
    if(m_bPM)
      Flag=1;
    else
      Flag=2;
  }
  snprintf(s,nLen,"%02d:%02d:%02d   %s",m_Hour==0 ? 12:m_Hour,m_Min,m_Second,Flag==0 ? "":Flag==1 ? "PM":"AM");
}


//--------------------------------------------------------------------
// Function name  : CSimpleDate::GetTimeStringShort
// Description      : 
// Return type    : void 
// Argument         : LPSTR s
// Argument         : int nLen
// Argument         : bool AmPm
//--------------------------------------------------------------------
void CSimpleDate::GetTimeStringShort(LPSTR s,int nLen,bool AmPm)
{
  if(!IsValid())
    return;

  SetTime();
  int Flag=0;
  if(AmPm==true)
  {
    if(m_bPM)
      Flag=1;
    else
      Flag=2;
  }
  snprintf(s,nLen,"%02d:%02d %s",m_Hour==0 ? 12:m_Hour,m_Min,Flag==0 ? "":Flag==1 ? "PM":"AM");
}


//--------------------------------------------------------------------
//  Static member functions
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Function name  : CSimpleDate::FixDateFormat
// Description      : 
// Return type    : bool 
// Argument         : LPSTR str
//--------------------------------------------------------------------
bool CSimpleDate::FixDateFormat(LPSTR str)
{
  char  Buff[20];
  char  Temp[20];

  if(strlen(str) < 6 || strlen(str) > 11)
    return false;   
  CParseIt ParseIt(str,"/.-");      
  ParseIt.Parse();
  int N=ParseIt.GetNumFields();
  if(N > 1)
    return false;
  memset(Buff,0,sizeof(Buff));
  memset(Temp,0,sizeof(Temp));
  strncpy(Buff,str,2);
  strcat(Buff,"/");
  strncpy(Temp,str+2,2);
  strcat(Buff,Temp);
  strcat(Buff,"/");   
  strncpy(Temp,str+4,strlen(str)==6 ? 2:4); //either "YY" or "YYYY"
  strcat(Buff,Temp);
  strcpy(str,Buff);   
  return true;
}
//--------------------------------------------------------------------
// Function name  : CSimpleDate::VerifyDateFormat
// Description      : 
// Return type    : bool 
// Argument         : LPCSTR date
//--------------------------------------------------------------------
bool CSimpleDate::VerifyDateFormat(LPCSTR date)
{
  bool  HasSeperators=false;
  char  TheDate[9];
  //  allow blank dates
  if(strlen(date)==0)
    return true;
  //  shortest possible date
  if(strlen(date) < 6)
    return false;
  //  look for unacceptable characters
    int x;
  for(x=0;x < (signed)strlen(date);++x)           
  {
    if( date[x] !='/' && date[x] !='-' && date[x] !='.' && !isdigit(date[x]))
        return false;
  }       
  for(x=0;x < (signed)strlen(date);++x)           
  {
    if( date[x] =='/' || date[x] =='-' || date[x] =='.')
        HasSeperators=true;
  }       
  strcpy(TheDate,date);
  if(!HasSeperators)
  {
    if(!FixDateFormat(TheDate))
      return false;
  }
  CSimpleDate s(TheDate);
  return s.IsValid();   
}


