// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Task Selector
// Author	: S. Carrez
// Date		: Sat Feb 25 13:37:54 1995
// Version	: $Id: XawTaskList.C,v 1.10 1996/08/04 15:35:59 gdraw Exp $
// Project	: Xcra
// Keywords	: Task, List, Xaw
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
// class XawTaskCharge		Represents a task in a dialog box
// class XawTaskList		Represents a list of tasks in a scrolled view
// class XawTaskSelector	Represents a calendar and the list of tasks
// 
//
#include "config.H"
#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Viewport.h>

#include "Date.H"
#include "Error.H"
#include "Task.H"
#include "TaskControl.H"
#include "XawTaskList.H"


// ----------------------------------------------------------------------
//
//			XawTaskCharge
//
// ----------------------------------------------------------------------

//
// Create the widgets to represent the task `_task'.
//
XawTaskCharge::XawTaskCharge(TaskCharge& _task, XawTaskList& _tl, Widget _top)
    : XawText("task", _tl.xDisplayMode, 0, _tl.xForm, _top),
      xList(_tl)
{
	//
	// Hack here! To be able to configure the representation of the
	// task using the X11 resources, we read in the label which is
	// initialized by the X11 tookit. This label is the model which
	// must be used. This must be done only the first time.
	//
    if (_tl.xFormatModel == 0) {
	_tl.xFormatModel = strdup(getLabel());
    }

    manage(_task);
}


//
// Delete the task item.
//
XawTaskCharge::~XawTaskCharge()
{
}


//
// Update the task item so that it now represents the task `_task'.
//
    void
XawTaskCharge::manage(TaskCharge& _task)
{
    xTask = &_task;

    char msg[512];

	//
	// Format the label which is displayed and update it.
	//
    xTask->format(msg, xList.xFormatModel);
    setLabel(msg);

	//
	// Fill the text field with the current time.
	//
    if (xList.xDisplayMode & XAW_TEXT) {
	xTask->format(msg, xList.xEditModel);

	XawText* t = this;

	(*t) = msg;
    }
}


//
// The task item has been selected.
//
    void
XawTaskCharge::select(XtPointer _data)
{
    if ((long)(_data) == 1) {
	xList.select(*this);
    }
}


//
// Check the value of the text field and convert it into a time.
// If the value was correct, update the task time in the day `_day'
// with that value.
//
    int
XawTaskCharge::set(const unsigned _day)
{
    time_t t;

    int result = Date::timeValue(*this, t);
    if (result == 0) {
	(*xTask)[_day] = t;
    }
    return result;
}


// ----------------------------------------------------------------------
//
//			XawTaskList
//
// ----------------------------------------------------------------------

//
// Create a representation box for a list of tasks.
//
XawTaskList::XawTaskList(const char* _name, TaskList& _tl, const int _mode,
			 const char* _editModel,
			 Widget _form, Widget _top)
    : xTaskList(_tl),

      xLabel(_name, XAW_LABEL, 0, _form, _top)
{
    _top = xLabel;

    xSelected    = 0;
    xFormatModel = 0;
    xListSize    = 0;
    xEditModel   = _editModel;
    xDisplayMode = _mode;

	//
	// Create a viewport (scrolled window).
	//
    xViewport = XtVaCreateManagedWidget(_name, viewportWidgetClass, _form,
				        XtNallowVert, True,
					XtNforceBars, True,
					XtNfromVert, _top,
					NULL);

	//
	// Put a form widget in the scrolled window.
	//
    xForm = XtCreateManagedWidget("form", formWidgetClass, xViewport, NULL, 0);

    xList = (XawTaskCharge**) XtCalloc(sizeof(XawTaskCharge*), MAX_TASKS);

	//
	// Create one XawTaskCharge item for each task.
	//
    update();
}


//
// Delete the representation of the list of tasks.
//
XawTaskList::~XawTaskList()
{
#ifdef HAVE_X11R5
    XawText::dontDestroyWidgets(false);
#endif

	//
	// Delete each task representation (from the end, due to a pb in Form).
	//
    for (unsigned i = 0; i < xListSize; i++) {
	delete xList[xListSize - 1 - i];
    }
    if (xFormatModel) {
	free((free_ptr) xFormatModel);
    }

    XtFree((char*) xList);

#ifdef HAVE_X11R5
    XawText::dontDestroyWidgets(true);
#else
    XtDestroyWidget(xViewport);
#endif
}


//
// The task list may have changed. Update by creating/removing
// some XawTaskCharge objects.
//
//
    void
XawTaskList::update()
{
    Widget top = None;

    XawText::resetRadioGroup();

    if (XtIsRealized(xForm)) {
	XawFormDoLayout(xForm, False);
    }
    unsigned i;
    for (i = 0; i < xTaskList.size(); i++) {
	if (xList[i] == 0) {
	    xList[i] = new XawTaskCharge(xTaskList[i], *this, top);
	} else {
	    xList[i]->manage(xTaskList[i]);
	}
	top = *xList[i];
    }

    if (xListSize != i) {
	for (unsigned j = xListSize; j >= i; j--) {
	    delete xList[j];
	    xList[j] = 0;

		//
		// This happens if the new list is empty! Bizarre bizarre...
		//
	    if (j == 0)
		break;
	}
    }

    if (XtIsRealized(xForm)) {
	XawFormDoLayout(xForm, True);
    }
    xListSize = xTaskList.size();
}


//
// Method executed when the task object was selected (clicked)
// by the user.
//
    void
XawTaskList::select(XawTaskCharge& _task)
{
    xSelected = &_task;
}


// ----------------------------------------------------------------------
//
//			XawTaskSelector
//
// ----------------------------------------------------------------------

//
// Create a generic task/day selector object.
//
XawTaskSelector::XawTaskSelector(const char* _name, const TaskList& _tl,
				 const int _mode, Widget _form, Widget _top)
	: TaskList(_tl),
	  XawCalendar("cal", _form, _top),
	  XawTaskList(_name, *this, _mode, "", _form, XawCalendar::xcForm)
{
	//
	// Must redraw the calendar because the vtbl is now up to date.
	//
    XawCalendar::showCalendar(year(), month());
}

//
// Delete the generic task/day selector.
//
XawTaskSelector::~XawTaskSelector()
{
}


//
// Construct a month label.
//
    void
XawTaskSelector::monthLabel(char* _label, const size_t) const
{
    sprintf(_label, "%s %u, %u", Date::monthName(xcMonth), tcCurDay, xcYear);
}


//
// Highlight the current selected day.
//
    int
XawTaskSelector::dayFlags(const unsigned _mday) const
{
    if (tcCurDay == (int) _mday) {
	return XAWC_DAY_HIGHLIGHTED;
    } else {
        return 0;
    }
}


//
// Update the calendar and the list to display the month `_month'
// and the year `_year'.
//
    void
XawTaskSelector::showCalendar(unsigned _year, unsigned _month)
{
    bool backward = (xcYear > _year || (xcYear == _year && xcMonth > _month));

    const unsigned curYear  = Date::currentYear();
    const unsigned curMonth = Date::currentMonth();
 
	//
	// Check/Load the charge file.
	//
    for (int i = 0; ; ) {

		//
		// Get the year-day of the first day of the new month.
		//
	const unsigned yday = Date::dayOfYear(_year, _month, 1);

	int result = loadFile(_year, yday, mode());
	if (result == 0) {
	    break;
	}
	if (result != E_OPEN_ERROR) {
	    return;
	}
	if (backward) {
	    if (_month == 1) {
		_month = 12;
		_year --;
	    } else {
		_month --;
	    }
	    i++;
	    if (i > 24) {
		return;
	    }
	} else {
	    if (_month == 12) {
		_month = 1;
		_year ++;
	    } else {
		_month ++;
	    }

	    if (_year > curYear || (_year == curYear && _month > curMonth)) {
		return;
	    }
	}
    }

	//
	// This succeeded, we can change the calendar.
	//
    XawCalendar::showCalendar(_year, _month);
    update();
}


//
// The day button `_mday' was pressed: switch to display
// the information of tasks for this day.
//
    void
XawTaskSelector::select(const unsigned _mday)
{
	//
	// Convert the month-day into a year-day.
	//
    const unsigned yday = Date::dayOfYear(year(), month(), _mday);

	//
	// Check that the charges are loaded (load them).
	//
    if (loadFile(year(), yday, mode()) != 0) {
	return;
    }

	//
	// Update the calendar (this may change the current highlighted day).
	//
    XawCalendar::showCalendar(year(), month());
    update();
}


