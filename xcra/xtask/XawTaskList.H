// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Task Selector
// Author	: S. Carrez
// Date		: Wed Mar  1 21:18:10 1995
// Version	: $Id: XawTaskList.H,v 1.9 1996/08/04 15:35:54 gdraw Exp $
// Project	: Xcra
// Keywords	: Task, List, Xaw
//
// Copyright (C) 1995, 1996, 2009 Stephane Carrez
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
// class XawTaskList		Represents a list of XawTaskCharge
//				objects (a list of tasks)
// class XawTaskSelector	Represents a task selector
//
//
#ifndef _XAW_TASK_LIST_H_
#define	_XAW_TASK_LIST_H_

#include "Task.H"
#include "XawDialog.H"
#include "XawCalendar.H"

class XawTaskList;

// ----------------------------------------------------------------------
// Class :	XawTaskCharge
//
// Role :	Defines a task (name + reference + time) to represent
//		it using widgets.
//
class XawTaskCharge : public XawText {
    friend class XawTaskList;
protected:
    TaskCharge*	 xTask;
    XawTaskList& xList;
public:
    XawTaskCharge(TaskCharge& _task, XawTaskList& _tl, Widget _top);
    ~XawTaskCharge();

	//
	// Return the name of this task.
	//
    inline const char* name() const;

	//
	// Return the task object.
	//
    inline TaskCharge& task() const;

  	//
	// Callback executed when the task item was selected.
	//
    void select(XtPointer _data);

	//
	// Convert the text value into a time and set it up in the
	// day `_day'.
	//
    int set(const unsigned _day);

	//
	// Update this object to control the task object `_task'.
	//
    void manage(TaskCharge& _task);
};

    inline const char*
XawTaskCharge::name() const
{
    return xTask->name();
}

    inline TaskCharge&
XawTaskCharge::task() const
{
    return *xTask;
}


// ----------------------------------------------------------------------
// Class :	XawTaskList
//
// Role :	Represents a list of tasks using the XawTaskCharge objects.
//		The list is displayed in a viewport to allow scrolling of
//		the window.
//
class XawTaskList {
	friend class XawTaskCharge;
protected:
    XawTaskCharge** xList;
    TaskList&	    xTaskList;
    Widget	    xForm;
    Widget	    xViewport;
    unsigned	    xDay;
    XawTaskCharge*  xSelected;
    unsigned	    xListSize;
    XawText	    xLabel;

		//
		// Model to format the label of the XawTaskCharge object.
		//
    const char*	xFormatModel;

		//
		// Mode for the XawText object to display the task charge
		// (toggle, label, text editor).
		//
    int		xDisplayMode;

		//
		// Format model to convert the time value into the editable
		// text (for the text editor).
		//
    const char* xEditModel;
public:
    XawTaskList(const char* _name, TaskList& _tl, const int _mode,
		const char* _editModel, Widget _form, Widget _top);
    virtual ~XawTaskList();

	//
	// Return the current day.
	//
    inline unsigned currentDay() const;

	//
	// Return the viewport widget.
	//
    inline operator Widget() const;

	//
	// Return the current selected task.
	//
    inline operator XawTaskCharge&() const;

	//
	// Method executed when the task object was selected (clicked)
	// by the user.
	//
    virtual void select(XawTaskCharge& _task);

	//
	// The task list may have changed. Update by creating/removing
	// some XawTaskCharge objects.
	//
    virtual void update();
};

    inline unsigned
XawTaskList::currentDay() const
{
    return xTaskList.currentDay();
}

    inline
XawTaskList::operator Widget() const
{
    return xViewport;
}

    inline
XawTaskList::operator XawTaskCharge&() const
{
    return *xSelected;
}


// ----------------------------------------------------------------------
// Class :	XawTaskSelector
//
// Role :	Generic task/day selector object.
//
//		- At top level, a calendar object allows the selection of
//		a day. It allows to switch the month.
//		- A list of tasks is displayed (XawTaskList). The content
//		associated with each task is changed according to the
//		selected day.
//
class XawTaskSelector : public TaskList, public XawCalendar,
 			public XawTaskList {
protected:
public:
    XawTaskSelector(const char* _name, const TaskList& _tl,
		    const int _mode, Widget _form, Widget _top);

    ~XawTaskSelector();

	//
	// Constructs a month label.
	//
    void monthLabel(char* _label, const size_t _size) const;

	//
	// Highlight the current selected day.
	//
    int dayFlags(const unsigned _mday) const;

	//
	// Update the calendar and the list to display the month `_month'
	// and the year `_year'.
	//
    void showCalendar(unsigned _year, unsigned _month);

	//
	// The day button `_mday' was pressed: switch to display
	// the information of tasks for this day.
	//
    void select(const unsigned _mday);
};

#endif
