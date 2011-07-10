// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Generic Calendar
// Author	: S. Carrez
// Date		: Sat Feb 25 13:37:54 1995
// Version	: $Id: XawCalendar.C,v 1.8 1996/08/04 15:34:25 gdraw Exp $
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
//
#include "config.H"
#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>

#include "Assert.H"
#include "Error.H"
#include "Date.H"
#include "XawCalendar.H"

// ----------------------------------------------------------------------
//
//			Callbacks
//
// ----------------------------------------------------------------------

    static void
DayButtonCB(Widget _w, XtPointer _tag, XtPointer)
{
    XawCalendar* xcal = (XawCalendar*) _tag;

    xcal->select(_w);
}

// ----------------------------------------------------------------------
//
//			XawCalendar
//
// ----------------------------------------------------------------------

//
// Create a Calendar object using the name `_name'. Put it in
// the form `_form' under the widget `_top'.
//
XawCalendar::XawCalendar(const char* _name, Widget _form, Widget _top)
{
    char bname[10];
    XtCallbackRec callbacks[2];

    xcYear	= 0;
    xcMonth	= 0;
    xcFirstDay	= 0;

    callbacks[0].callback = DayButtonCB;
    callbacks[0].closure  = (XtPointer) this;
    callbacks[1].closure  = (XtPointer) NULL;
    callbacks[1].callback = (XtCallbackProc) NULL;

	//
	// Create the form which holds the calendar description.
	//
    xcForm = XtVaCreateManagedWidget(_name, formWidgetClass, _form,
				     XtNfromVert, _top,
				     NULL);

	//
	// Create the top buttons to change the calendar month.
	//
    xcPrevMonth = XtVaCreateManagedWidget("pMonth", commandWidgetClass, xcForm,
					  XtNfromVert, None,
					  XtNcallback, callbacks,
					  NULL);

    xcMonthLabel = XtVaCreateManagedWidget("month", labelWidgetClass, xcForm,
					   XtNfromVert, None,
					   XtNfromHoriz, xcPrevMonth,
					   XtNborderWidth, 0,
					   NULL);

    xcNextMonth = XtVaCreateManagedWidget("nMonth", commandWidgetClass, xcForm,
					  XtNfromVert, None,
					  XtNfromHoriz, xcMonthLabel,
					  XtNcallback, callbacks,
					  NULL);

	//
	// Create the day labels.
	//
    Widget w = None;
    for (unsigned i = 1; i <= 7; i++) {
	sprintf(bname, "day%u", i);
	w = XtVaCreateManagedWidget(bname, labelWidgetClass, xcForm,
				    XtNborderWidth, 0,
				    XtNfromVert, xcPrevMonth,
				    XtNfromHoriz, w,
				    NULL);
    }


	//
	// Create one button for each day slot (7 days * 6 weeks = 42).
	// Only the valid slots will be managed (and thus visible).
	//
    Widget prevW = None;
    for (unsigned j = 0; j < 42; j++) {
	sprintf(bname, "cd%u", j);
	xcDay[j] = XtVaCreateManagedWidget(bname, commandWidgetClass, xcForm,
					   XtNborderWidth, 0,
					   XtNfromVert, w,
					   XtNfromHoriz, prevW,
					   XtNcallback, callbacks,
					   NULL);

	xcFlags[j] = XAWC_DAY_MAPPED;
	if (((j+1) % 7) == 0) {
	    prevW = None;
	    w = xcDay[j];
	} else {
	    prevW = xcDay[j];
	}
    }

	//
	// Get the foreground and background color.
	//
    XtVaGetValues(xcDay[0], XtNforeground, &xcForeground,
		  XtNbackground, &xcBackground, NULL);
}


//
// Delete the calendar object.
//
XawCalendar::~XawCalendar()
{
#ifdef HAVE_X11R6
    XtDestroyWidget(xcForm);
#endif
}


//
// Build a label to display in the calendar's corresponding day.
// The label string must be constructed in the buffer `_label'
// which is `_size' bytes length.
//
    void
XawCalendar::dayLabel(const unsigned _mday, char* _label,
		      const size_t _size) const
{
    sprintf(_label, "%u", _mday);

    ensure(strlen(_label) < _size);
}


//
// Build the label which is displayed in the month label widget.
// The label string must be constructed in the buffer `_label'
// which is `_size' bytes length.
// 
    void
XawCalendar::monthLabel(char* _label, const size_t _size) const
{
    sprintf(_label, "%s %u", Date::monthName(xcMonth), xcYear);

    ensure(strlen(_label) < _size);
}


//
// Returns the flags associated to the month of the day.
//
    int
XawCalendar::dayFlags(const unsigned) const
{
    return 0;
}


//
// Callback executed when the user changes the current
// month. The default updates the Calendar object to
// actually show the new month.
//
    void
XawCalendar::showCalendar(unsigned _year, unsigned _month)
{
    require(_month <= 12);
    
    bool updateLabel;
    
    updateLabel = (_year != xcYear || _month != xcMonth) ? true : false;
    

	//
	// Determine the number of days in this month and
	// the first day of the week.
	//
    unsigned firstDay = Date::firstDayOf(_year, _month);
    unsigned numDays  = Date::numberOfDays(_year, _month);

    if (firstDay == 7) {
	firstDay = 0;
    }

    xcYear     = _year;
    xcMonth    = _month;
    xcFirstDay = firstDay;

    Arg  args[4];
    char label[32];

	//
	// Scan the day buttons:
	//
	// - Unmap the buttons which do not correspond to a valid day
	//   (they will not appear)
	// - Map the others
	// - Set the label (day number)
	//
    for (unsigned i = 0; i < 42; i++) {
	int ac = 0;

	if (i < firstDay || i > firstDay + numDays - 1) {
	    if (xcFlags[i] & XAWC_DAY_MAPPED) {
		XtSetMappedWhenManaged(xcDay[i], False);
		xcFlags[i] &= ~XAWC_DAY_MAPPED;
	    }
	} else {
	    if (updateLabel) {
		dayLabel(i - firstDay + 1, label, sizeof(label));
		XtSetArg(args[ac], XtNlabel, label); ac++;
	    }

	    int flags = dayFlags(i - firstDay + 1);
	    if ((flags ^ xcFlags[i]) & XAWC_DAY_DISABLED) {
		if (flags & XAWC_DAY_DISABLED) {
		    XtSetArg(args[ac], XtNsensitive, False); ac++;
		} else {
		    XtSetArg(args[ac], XtNsensitive, True); ac++;
		}
	    }
	    if ((flags ^ xcFlags[i]) & XAWC_DAY_HIGHLIGHTED) {
		if (flags & XAWC_DAY_HIGHLIGHTED) {
		    XtSetArg(args[ac], XtNforeground, xcBackground); ac++;
		    XtSetArg(args[ac], XtNbackground, xcForeground); ac++;
		} else {
		    XtSetArg(args[ac], XtNforeground, xcForeground); ac++;
		    XtSetArg(args[ac], XtNbackground, xcBackground); ac++;
		}
	    }
	    if (!(xcFlags[i] & XAWC_DAY_MAPPED)) {
		XtSetMappedWhenManaged(xcDay[i], True);
	    }
	    xcFlags[i] = flags | XAWC_DAY_MAPPED;
	    if (ac) {
		XtSetValues(xcDay[i], args, ac);
	    }
	}
    }

	//
	// Update the month label.
	//
    updateMonthLabel();
}


//
// Callback executed when the user presses the button
// which correspond to the day of month.
//
    void
XawCalendar::select(const unsigned)
{
}


//
// Force an update of the month title.
//
    void
XawCalendar::updateMonthLabel()
{
    char label[512];

	//
	// Update the month label.
	//
    monthLabel(label, sizeof(label));
    XtVaSetValues(xcMonthLabel, XtNlabel, label, NULL);
}


//
// Identify the widget `_w\' and execute the action associated with it.
//
    void
XawCalendar::select(Widget _w)
{
	//
	// See if it's a change button widget.
	//
    if (_w == xcPrevMonth) {
	if (xcMonth == 1) {
	    showCalendar(xcYear - 1, 12);
	} else {
	    showCalendar(xcYear, xcMonth - 1);
	}
    } else if (_w == xcNextMonth) {
	if (xcMonth == 12) {
	    showCalendar(xcYear + 1, 1);
	} else {
	    showCalendar(xcYear, xcMonth + 1);
	}

	//
	// Otherwise, look each day button. If the widget is wrong,
	// do nothing.
	//
    } else {
	for (unsigned i = 0; i < 42; i++) {
	    if (xcDay[i] == _w) {
		select(i - xcFirstDay + 1);
		break;
	    }
	}
    }
}
