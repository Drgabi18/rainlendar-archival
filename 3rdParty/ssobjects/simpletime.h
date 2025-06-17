/********************************************************************
       Copyright (c) 2001, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from http://www.codeproject.com/datetime/dateclass.asp
       Copyright (c), Richard Stringer
       
       Original source copyright got lost. 

       filename :  simpletime.h
       author   :  Lee Patterson (lee@antws.com)
*********************************************************************/

#ifndef SIMPLETIME_H
#define SIMPLETIME_H

namespace ssobjects
{

typedef const char* PTIMESTRING;

#define SECS_PER_MIN 60
#define SECS_PER_HOUR 3600
#define SECS_PER_DAY (86400)    //SECSperHOUR*24

class CSimpleTime
{
  public:     //attributes
    unsigned    m_currentTime;      //current time of this day in seconds
    unsigned    m_hour;
    unsigned    m_minute;
    unsigned    m_second;
    char        m_szFormated[80];

  public:     //construction
    CSimpleTime();
    CSimpleTime(const char* szTime);
    CSimpleTime(unsigned seconds);

  protected:  //methods
    void FormatOutput();
    void SetTime();
        
        
  public:     //methods
    static unsigned GetCurrentTime();
    static unsigned Convert(unsigned hour,unsigned minute,unsigned second);
    static bool     IsValid(unsigned time) {return time < SECS_PER_DAY ? true:false;}

    virtual unsigned GetHour();
    virtual unsigned GetMinute();
    virtual unsigned GetSecond();

//        operator long() const {return m_currentTime;}
    operator unsigned() const {return m_currentTime;}
    operator PTIMESTRING();

    CSimpleTime& operator = (const char* time);
    CSimpleTime& operator = (const unsigned time);

    //lee: I changed this to return by value instead of ref. g++ gave a warning. not tested yet
    CSimpleTime operator += (const unsigned time);
    CSimpleTime operator -= (const unsigned time);

    bool operator == (const unsigned time) const;
    bool operator < (const unsigned time) const;
    bool operator > (const unsigned time) const;
    bool operator <= (const unsigned time) const;
    bool operator >= (const unsigned time) const;

    bool operator == (const CSimpleTime& time) const;
    bool operator < (const CSimpleTime& time) const;
    bool operator > (const CSimpleTime& time) const;
    bool operator <= (const CSimpleTime& time) const;
    bool operator >= (const CSimpleTime& time) const;
};

};

#endif
