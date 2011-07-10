// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Date management
// Author	: S.Carrez
// Date		: Wed Feb 22 21:32:54 1995
// Version	: $Id: Date.C,v 1.10 2000/02/24 08:24:35 ciceron Exp $
// Project	: Xcra
// Keywords	: Date, Conversion
//
// Copyright (C) 1995, 1996 Stephane Carrez
//
// This file is part of Xcra.
//
// Xcra is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Xcra is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////
//
// Contents :
// ----------
// class Date		Management of date conversion
// class Day		Representation of a day (year, day)
// class DayIterator	Iterator to operate on an interval of days
// class WeekConversion	Logical to physical date conversion for a
//			week, fortnight, month.
// 
//
//
#include "Date.H"

// ----------------------------------------------------------------------
//
//			Date
//
// ----------------------------------------------------------------------
static int FirstDay(int, int);

static const unsigned char monthDays[13] = {
  0,
  31, 28, 31, 30, 31, 30, 31,
  31, 30, 31, 30, 31
};

static const unsigned char monthDaysLeap[13] = {
  0,
  31, 29, 31, 30, 31, 30, 31,
  31, 30, 31, 30, 31
};

static const char* const monthNames[12] = {
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};


//
// Return true if the year `_year' is a leap year.
//
    bool
Date::isLeapYear(const unsigned _year)
{
    static int prevYear = -1;
    static bool prevRes = false;
    
    if (prevYear == (int) _year) {
	return prevRes;
    }
    prevYear = (int) _year;
    if ((_year % 1000) == 0) {
	return prevRes = true;
    }
    if ((_year % 100) == 0) {
	return prevRes = false;
    }
    return prevRes = (((_year % 4) == 0) ? true : false);
}


//
// Return the name of the month `_month'.
//
    const char*
Date::monthName(const unsigned _month)
{
    RequireMsg(_month >= 1 && _month <= 12, ("Month %u\n", _month));

    return monthNames[_month - 1];
}


//
// Given a year and a day of that year, return the month number
// which correspond to that day (_day is [1..366]).
//
    unsigned
Date::monthOf(const unsigned _year, unsigned _day)
{
    const unsigned char* months;

    if (_day == 0)
      _day = 1;
    
    if (Date::isLeapYear(_year)) {
	RequireMsg(_day >= 1 && _day <= 366, ("Day %u\n", _day));

	months = monthDaysLeap;
    } else {
	RequireMsg(_day >= 1 && _day <= 365, ("Day %u\n", _day));

	months = monthDays;
    }

    unsigned i;
    for (i = 0; i < 12 && _day > *months; i++) {
	_day -= *months++;
    }

    Ensure(i >= 1 && i <= 12 && _day <= NR_DAYS);
    return i;
}


//
// Return the number of days in the month `_month' of the year `_year'.
//
    unsigned
Date::numberOfDays(const unsigned _year, const unsigned _month)
{
    RequireMsg(_month >= 1 && _month <= 12, ("Month %u\n", _month));

    if (Date::isLeapYear(_year)) {
	return monthDaysLeap[_month];
    } else {
	return monthDays[_month];
    }
}


//
// Given a year and a day of that year, return the month day
// (1..31).
//
    unsigned
Date::monthDayOf(const unsigned _year, unsigned _day)
{
    const unsigned char* months;

    if (Date::isLeapYear(_year)) {
	RequireMsg(_day >= 1 && _day <= 366, ("Day %u\n", _day));

	months = monthDaysLeap;
    } else {
	RequireMsg(_day >= 1 && _day <= 365, ("Day %u\n", _day));

	months = monthDays;
    }

    for (unsigned i = 0; i < 12 && _day > *months; i++) {
	_day -= *months++;
    }

    Ensure(_day >= 1 && _day <= 366);
    return _day;
}


//
// Return the day of the year (1..366) which corresponds to the
// day `_mday/_month/_year'.
//
    unsigned
Date::dayOfYear(const unsigned _year, unsigned _month, unsigned _day)
{
   const unsigned char* months;

   RequireMsg(_month >= 1 && _month <= 12,  ("Month %u\n", _month));
   RequireMsg(_day >= 1 && _day <= NR_DAYS, ("Day %u\n", _day));

   if (Date::isLeapYear(_year)) {
	months = &monthDaysLeap[1];
   } else {
	months = &monthDays[1];
   }

   while (-- _month != 0) {
	_day += *months++;
   }

   Ensure(_day >= 1 && _day <= 366);
   return _day;
}


//
// Find the week day number (1..7) of the first day of month `_month'
// in the year `_year'.
//
    unsigned
Date::firstDayOf(const unsigned _year, const unsigned _month)
{
    RequireMsg(_month >= 1 && _month <= 12, ("Month %u\n", _month));

    return FirstDay(_month, _year);
}


//
// Return the current week number (1..52).
//
    unsigned
Date::currentWeek()
{
    time_t t;

    time(&t);

    Day d(t);

    return (d.day() + 6) / 7;
}


//
// Return the current month number (1..12).
//
    unsigned
Date::currentMonth()
{
    time_t t;

    time(&t);

    Day d(t);

    return d.month();
}


//
// Return the current year number (1900..).
//
    unsigned
Date::currentYear()
{
    time_t t;

    time(&t);

    Day d(t);

    return d.year();
}


//
// Convert the string `_s' into a time value in `t'.
//
    int
Date::timeValue(const char* s, time_t& t)
{
    int mins = 0;
    int hours = 0;
    int secs = 0;
    int state = 0;
    int n;
    int sign = 1;
//
// Formats:			State
//
//	hh:mm			1
//	hh:mm:ss		2
//	mm			0
//	hh h			3
//	mm m			4
//	ss s			5
//
    for (n = 0; s[0]; s++) {
	switch (s[0]) {
	case '-':
	   sign = -1;
	   break;

	case '+':
	   sign = 1;
	   break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	    n = (n * 10) + (s[0] - '0');
	    if (state >= 2)
		return -1;
	    break;
	case ':':
	    switch (state) {
	    case 0:
		hours = n;
		state = 1;
		break;
	    case 1:
		mins = n;
		state = 2;
		break;
	    default :
		return -1;
	    }
	    n = 0;
	    break;
	case 's':
	case 'S':
	    if (state != 0 || state == 6)
		return -1;
	    secs = n;
	    state = 5;
	    n = 0;
	    break;
	case 'm':
	case 'M':
	    if (state != 0 || state == 6)
		return -1;
	    mins = n;
	    state = 4;
	    n = 0;
	    break;
	case 'h':
	case 'H':
	    if (state != 0 || state == 6)
		return -1;
	    hours = n;
	    state = 3;
	    n = 0;
	    break;
	case ' ':
	    if (state != 0)
		return -1;
	    state = 6;
	    break;
	default:
	    return -1;
	}
    }
    switch (state) {
    case 0 :
    case 1 :
	mins = n;
	break;
    case 2 :
	secs = n;
	break;
    default :
	break;
    }

    if (hours < 0 || hours >= 24)
	return -1;
    if (mins < 0 || mins >= 60)
	return -1;
    if (secs < 0 || secs >= 60)
	return -1;

    t = sign * ((hours * 3600) + (mins * 60) + secs);
    return 0;
}

/* taken from xcalendar.c */

/**/

/* taken from cal.c */

static char	mon[] = {
   0,
   31, 29, 31, 30,
   31, 30, 31, 31,
   30, 31, 30, 31,
};

static int calInit = 0;

/*
 *	return day of the week
 *	of jan 1 of given year
 */

static int jan1(int yr)
{
	register int y, d;

/*
 *	normal gregorian calendar
 *	one extra day per four years
 */

	y = yr;
	d = 4+y+(y+3)/4;

/*
 *	julian calendar
 *	regular gregorian
 *	less three days per 400
 */

	if(y > 1800) {
		d -= (y-1701)/100;
		d += (y-1601)/400;
	}

/*
 *	great calendar changeover instant
 */

	if(y > 1752)
		d += 3;

	return(d%7);
}

/* should be called first */
static int FirstDay(int m, int y)
{
   register int d, i;
   
   calInit = y;
   d = jan1(y);
   mon[2] = 29;
   mon[9] = 30;
   
   switch((jan1(y+1)+7-d)%7) {
      
      /*
       *	non-leap year
       */
    case 1:
      mon[2] = 28;
      break;
      
      /*
       *	1752
       */
    default:
      mon[9] = 19;
      break;

      /*
       *	leap year
       */
    case 2:
      ;
   }
   
   for(i=1; i<m; i++)
     d += mon[i];
   d %= 7;
   
   d = d>0 ? d : 7;		/* returns 1-7, not 0-6 */
   return d;
}


// ----------------------------------------------------------------------
//
//			Day
//
// ----------------------------------------------------------------------

Day::Day(time_t _t)
{
    struct tm* tmP;

    tmP   = localtime(&_t);
    dDay  = tmP->tm_yday + 1;
    dYear = tmP->tm_year + 1900;

    Ensure(dDay >= 1 && dDay <= 366);
}


// ----------------------------------------------------------------------
//
//			DayIterator
//
// ----------------------------------------------------------------------

DayIterator::DayIterator(const WeekConversion& _cvt)
: diFirst(_cvt.firstDay()), diLast(_cvt.lastDay())
{
    first();
}


// ----------------------------------------------------------------------
//
//			WeekConversion
//
// ----------------------------------------------------------------------

//
// Create a conversion object to convert a logical date into a physical one.
//
WeekConversion::WeekConversion()
{
    clear();
}


//
// Convert the day `_day' into an index.
//
    unsigned
WeekConversion::operator [](const Day& _day) const
{
    if (wcFirst > _day)
        return 0;

    if (wcLast < _day)
	return 0;

    unsigned d = (_day.day() - wcFirst.day()) + 1;
    return d;
}


//
// Return true if the month day `_mday' is valid in the set.
//
    bool
WeekConversion::isInside(const unsigned _month, const unsigned _mday) const
{
    Day d(Date::dayOfYear(wcFirst.year(), _month, _mday), wcFirst.year());

    return (*this)[d] != 0 ? true : false;
}


//
// Clear the conversion object.
//
    void
WeekConversion::clear()
{
    wcFirst = Day(0, 0);
    wcLast  = wcFirst;
}


//
// Setup the conversion for a full month.
//
    void
WeekConversion::setMonth(unsigned _year, unsigned _month)
{
    unsigned nrDays;

    RequireMsg(_month <= 12, ("Month %u\n", _month));

    if (_month == 0) {
	_month = Date::currentMonth();
    }

    if (Date::isLeapYear(_year)) {
	nrDays = monthDaysLeap[_month];
    } else {
	nrDays = monthDays[_month];
    }

    unsigned first = Date::dayOfYear(_year, _month, 1);
    wcFirst = Day(first, _year);
    wcLast  = Day(first + nrDays - 1, _year);
}


    void
WeekConversion::setMonth(const WeekConversion& _wc)
{
    setMonth(_wc.year(), _wc.month());
}


//
// Setup the conversion for one week (starting at Monday).
//
    void
WeekConversion::setWeek(unsigned _year, unsigned _week)
{
    RequireMsg(_week <= 52, ("Week %u\n", _week));

    unsigned curDay = 1;

	//
	// Get the day number of 1/1/year
	//
    unsigned day = Date::firstDayOf(_year, 1);

    if (_week == 0) {
	_week = Date::currentWeek();
    }

	//
	// Hack for years starting a Saturday or a Sunday: the first week
	// starts on next Monday.
	//
    if (day >= 6) {
	_week ++;
    }

    while (--_week != 0) {
		//
		// Align the week on the next monday
		//
	unsigned delta = (7 - day) + 1;
	curDay = curDay + delta;
	day = 1;
    }

    wcFirst = Day(curDay, _year);
    wcLast  = Day(curDay + 7 - day, _year);
}

    void
WeekConversion::setWeek(const WeekConversion& _wc, unsigned _curDay)
{
    Day d(_wc.firstDay());

	//
	// Get the day number of 1/1/year.
	//
    unsigned first = Date::firstDayOf(_wc.year(), 1);    

	//
	// Hack if year starts on Saturday or Sunday.
	//
    if (first < 6) {
	_curDay = _curDay + first - 1;
    } else if (first == 7) {
	_curDay --;
    } else if (first == 6) {
	_curDay -= 2;
    }
    _curDay = ((d.day() + _curDay - 1) + 6) / 7;
    setWeek(_wc.year(), _curDay);
}


    void
WeekConversion::setFortnight(unsigned _year, unsigned _week)
{
    if (_week == 0) {
	_week = Date::currentWeek();
    }

    if (!(_week & 1))
	_week --;

    setWeek(_year, _week);
    Day d(wcFirst);
    setWeek(_year, _week + 1);
    wcFirst = d;
}


    void
WeekConversion::setFortnight(const WeekConversion& _wc, unsigned _curDay)
{
    Day d(_wc.firstDay());
    
	//
	// Get the day number of 1/1/year.
	//
    unsigned first = Date::firstDayOf(_wc.year(), 1);    

	//
	// Hack if year starts on Saturday or Sunday.
	//
    if (first < 6) {
	_curDay = _curDay + first - 1;
    } else if (first == 7) {
	_curDay --;
    } else if (first == 6) {
	_curDay -= 2;
    }
    _curDay = ((d.day() + _curDay - 1) + 6) / 7;
    setFortnight(_wc.year(), _curDay);
}


