// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Date management
// Author	: S.Carrez
// Date		: Wed Feb 22 21:28:22 1995
// Version	: $Id: Date.H,v 1.7 1996/08/04 15:22:58 gdraw Exp $
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
#ifndef _DATE_H_
#define	_DATE_H_

#ifndef NR_DAYS
# define NR_DAYS	31
#endif

#include "config.H"
#include "Assert.H"

class WeekConversion;

// ----------------------------------------------------------------------
// Class :	Date
//
// Role :	Operations which are related to the date
//
class Date {
public:
	//
	// Return true if the year `_year' is a leap year.
	//
    static bool isLeapYear(const unsigned _year);

	//
	// Return the name of the month `_month'.
	//
    static const char* monthName(const unsigned _month);

	//
	// Given a year and a day of that year, return the month number
	// which correspond to the day (_day is [1..366]).
	//
    static unsigned monthOf(const unsigned _year, unsigned _day);

	//
	// Return the number of days in the month `_month' of the year `_year'.
	//
    static unsigned numberOfDays(const unsigned _year, const unsigned _month);

	//
	// Given a year and a day of that year, return the month day
	// (1..31).
	//
    static unsigned monthDayOf(const unsigned _year, unsigned _day);

	//
	// Return the day of the year (1..366) which corresponds to the
	// day `_mday/_month/_year'.
	//
    static unsigned dayOfYear(const unsigned _year, unsigned _month,
			      unsigned _mday);

	//
	// Find the week day number (1..7) of the first day of month `_month'
	// in the year `_year'.
	//
    static unsigned firstDayOf(const unsigned _year, const unsigned _month);

	//
	// Return the current week number (1..52).
	//
    static unsigned currentWeek();

	//
	// Return the current month number (1..12).
	//
    static unsigned currentMonth();

	//
	// Return the current year number (1900..).
	//
    static unsigned currentYear();

	//
	// Convert the string `_s' into a time value in `t'.
	//
    static int timeValue(const char* _s, time_t& _t);
};


// ----------------------------------------------------------------------
// Class :	Day
//
// Role :	Definition of a day of a year with some useful
//		operations to manage that day.
//
class Day {
	friend class DayIterator;
protected:
    unsigned short	dDay;	// Day  1 .. 366
    unsigned short	dYear;	// Year 1967 .. 2042 ???
public:
    inline Day();
    inline Day(const unsigned d, const unsigned y);
    Day(time_t t);

	//
	// Compares 2 days.
	//
    inline bool operator ==(const Day& d) const;
    inline bool operator <(const Day& d) const;
    inline bool operator >(const Day& d) const;
    inline bool operator <=(const Day& d) const;
    inline bool operator >=(const Day& d) const;

	//
	// Next or previous day operations.
	//
    inline Day& operator ++();
    inline Day& operator --();

    inline unsigned day() const;
    inline unsigned year() const;
    inline unsigned month() const;
};

    inline
Day::Day()
{
}

    inline
Day::Day(const unsigned d, const unsigned y)
{
    RequireMsg(d <= 366, ("Day %u\n",d));

    dDay  = (const unsigned short)d;
    dYear = (const unsigned short)y;
}

    inline bool
Day::operator ==(const Day& d) const
{
    return dDay == d.dDay && dYear == d.dYear;
}

    inline bool
Day::operator <(const Day& d) const
{
    return dYear < d.dYear || (dYear == d.dYear && dDay < d.dDay);
}

    inline bool
Day::operator <=(const Day& d) const
{
    return dYear < d.dYear || (dYear == d.dYear && dDay <= d.dDay);
}

    inline bool
Day::operator >(const Day& d) const
{
    return dYear > d.dYear || (dYear == d.dYear && dDay > d.dDay);
}

    inline bool
Day::operator >=(const Day& d) const
{
    return dYear > d.dYear || (dYear == d.dYear && dDay >= d.dDay);
}

    inline unsigned
Day::day() const
{
    return dDay;
}

    inline unsigned
Day::year() const
{
    return dYear;
}

    inline unsigned
Day::month() const
{
    return Date::monthOf(dYear, dDay);
}

    inline Day&
Day::operator ++()
{
    dDay ++;
    if (dDay > 365) {
	if (dDay > 366 || Date::isLeapYear(dYear) == 0) {
	    dDay = 1;
	    dYear ++;
	}
    }
    return *this;
}

    inline Day&
Day::operator --()
{
    dDay --;
    if (dDay == 0) {
	dYear --;
	if (Date::isLeapYear(dYear) == 0) {
	    dDay = 365;
	} else {
	    dDay = 366;
	}
    }
    return *this;
}


// ----------------------------------------------------------------------
// Class :	DayIterator
//
// Role :	Day iterator to perform some operations on several
//		consecutive days.
//
class DayIterator {
protected:
    Day	diFirst;
    Day	diLast;
    Day diCurrent;
public:
    inline DayIterator(const Day& first, const Day& last);
    DayIterator(const WeekConversion& cv);

	//
	// Return the current day.
	//
    inline operator const Day&() const;

	//
	// Advance to the next day. Return false if none.
	//
    inline bool next();

	//
	// Advance to the previous day. Return false if none.
	//
    inline bool prev();

	//
	// Move to first/last day of the iterator (reset iterator).
	//
    inline void first();
    inline void last();
};

    inline void
DayIterator::first()
{
    diCurrent = diFirst;
}

    inline void
DayIterator::last()
{
    diCurrent = diLast;
}

    inline
DayIterator::DayIterator(const Day& first, const Day& last)
    : diFirst(first), diLast(last), diCurrent(first)
{
}

    inline
DayIterator::operator const Day&() const
{
    return diCurrent;
}

    inline bool
DayIterator::next()
{
    if (diCurrent >= diLast) {
	return 0;
    } else {
	++ diCurrent;
	return 1;
    }
}

    inline bool
DayIterator::prev()
{
    if (diCurrent <= diFirst) {
	return 0;
    } else {
	-- diCurrent;
	return 1;
    }
}


// ----------------------------------------------------------------------
// Class :	WeekConversion
//
// Role :	Convert a day into a physical position (see task manager).
//
class WeekConversion {
protected:
    Day		wcFirst;
    Day		wcLast;
public:
    WeekConversion();

	//
	// Return the first/last day of the week/set.
	//
    inline const Day& firstDay() const;
    inline const Day& lastDay() const;

	//
	// Return the current year and month.
	//
    inline unsigned year() const;
    inline unsigned month() const;

	//
	// Return true if the month day `_mday' is valid in the set.
	//
    bool isInside(const unsigned _month, const unsigned _mday) const;

	//
	// Convert the day `_day' into an index.
	//
    unsigned operator [](const Day& _day) const;

	//
	// Clear the conversion object.
	//
    void clear();

	//
	// Setup the conversion for one week (starting at Monday).
	//
    void setWeek(unsigned _year, unsigned _week);
    void setWeek(const WeekConversion& _wc, unsigned _curDay);

	//
	// Setup the conversion for 2 weeks.
	//
    void setFortnight(unsigned _year, unsigned _week);
    void setFortnight(const WeekConversion& _wc, unsigned _curDay);

	//
	// Setup the conversion for a full month.
	//
    void setMonth(unsigned _year, unsigned _month);
    void setMonth(const WeekConversion& _wc);
};


    inline const Day&
WeekConversion::firstDay() const
{
    return wcFirst;
}


    inline const Day&
WeekConversion::lastDay() const
{
    return wcLast;
}


    inline unsigned
WeekConversion::year() const
{
    return wcFirst.year();
}


    inline unsigned
WeekConversion::month() const
{
    return wcFirst.month();
}

#endif
