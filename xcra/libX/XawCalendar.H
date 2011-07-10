// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Generic Calendar
// Author	: S. Carrez
// Date		: Wed Mar  1 22:09:54 1995
// Version	: $Id: XawCalendar.H,v 1.6 1996/08/04 15:34:22 gdraw Exp $
// Project	: xcra
// Keywords	: Calendar, Xaw
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
// class XawCalendar		Calendar object using Athena Widgets
// 
#ifndef _XAW_CALENDAR_H_
#define	_XAW_CALENDAR_H_

#define	XAWC_DAY_HIGHLIGHTED	0x01	// Day is highlighted
#define	XAWC_DAY_DISABLED	0x02	// Day is not selectable
#define XAWC_DAY_MAPPED		0x04	// Day button is mapped

// ----------------------------------------------------------------------
// Class :	XawCalendar
//
// Role :	Calendar object
//
// The class `XawCalendar' defines a generic calendar object which
// looks like the xcalendar(1) tool. It manages a form with several
// buttons:
//
// - A pMonth button which changes the calendar to display the
//   previous month,
// - A nMonth button which changes for the next month,
// - One button for each day of the current month.
//
// Day buttons are arranged so that their layout looks like the output
// of the cal(1) tool. For example, the XawCalendar layout could be:
//
//
//	    March 1995
//	 S  M Tu  W Th  F  S
//	          1  2  3  4
//	 5  6  7  8  9 10 11
//	12 13 14 15 16 17 18
//	19 20 21 22 23 24 25
//	26 27 28 29 30 31
//
//
// The class provides two feedback mechanism from user's selection:
//
// - a `select' virtual method which is activated when a day button
//   is pressed,
// - a `showCalendar' virtual method which is activated when a
//   pMonth or nMonth button (change month) is pressed.
//
//
// Each day button is assigned several flags which are queried by
// the virtual method `dayFlags'. The purpose is to highlight or
// disable some days to provide various feedback effects. The possible
// flags are:
//
//	XAWC_DAY_HIGHLIGHTED	The day button is highlighted.
//				(Foreground and background colors are switched)
//	XAWC_DAY_DISABLED	The day button is disabled. The button
//				is not selectable and is tiled with a pixmap.
//
class XawCalendar {
private:
    XawCalendar(const XawCalendar&);
    void operator=(const XawCalendar&);
    void operator&();
protected:
    Widget	xcForm;

		//
		// Next/Prev month buttons.
		//
    Widget	xcPrevMonth;
    Widget	xcNextMonth;

		//
		// Middle label.
		//
    Widget	xcMonthLabel;

		//
		// Year/Month and current day.
		//
    unsigned	xcYear;
    unsigned	xcMonth;
    unsigned	xcFirstDay;

		//
		// Colors of the day buttons.
		//
    Pixel	xcForeground;
    Pixel	xcBackground;

		//
		// Button description (6 lines * 7 columns).
		//
    Widget	xcDay[42];
    char	xcFlags[42];
private: public:	// Hack for private method.
    void select(Widget _w);
public:
	//
	// Create a Calendar object using the name `_name'. Put it in
	// the form `_form' under the widget `_top'.
	//
    XawCalendar(const char* _name, Widget _form, Widget _top);

    virtual ~XawCalendar();

	//
	// Build a label to display in the calendar's corresponding day.
	// The label string must be constructed in the buffer `_label'
	// which is `_size' bytes length.
	// The default prints the day number.
	//
    virtual void dayLabel(const unsigned _mday, char* _label,
			  const size_t _size) const;

	//
	// Build the label which is displayed in the month label widget.
	// The label string must be constructed in the buffer `_label'
	// which is `_size' bytes length.
	// The default prints the month name followed by the year.
	// 
    virtual void monthLabel(char* _label, const size_t _size) const;

	//
	// Returns the flags associated to the month of the day.
	// The default returns 0.
	//
    virtual int  dayFlags(const unsigned _mday) const;

	//
	// Callback executed when the user changes the current
	// month. The default updates the Calendar object to
	// actually show the new month.
	//
    virtual void showCalendar(unsigned _year, unsigned _month);

	//
	// Callback executed when the user presses the button
	// which correspond to the day of month. The default action
	// does nothing.
	//
    virtual void select(const unsigned _day);

	//
	// Force an update of the month title.
	//
    void updateMonthLabel();

	//
	// Return the Calendar widget object.
	//
    inline Widget widget() const;
};


    inline Widget
XawCalendar::widget() const
{
    return xcForm;
}

#endif
