// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Interface for the management of Tasks
// Author	: S. Carrez
// Date		: Sat Mar  4 13:37:28 1995
// Version	: $Id: XawTaskMgr.H,v 1.14 2000/02/23 08:23:31 ciceron Exp $
// Project	: Xcra
// Keywords	: Task, Xaw, Interface
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
// Dialog boxes:
// -------------
//   class XawTaskDialog	Generic top level for some dialogs
//   class XawTaskEditReference	Update task references
//   class XawTaskEditTimes	Update task times
//   class XawTaskSelect	Change/select a task
//   class XawTaskReport	Report the times
//   class XawTaskRemove	Remove a task
//   class XawTaskAdd		Create a new task
//   class XawTaskMoveTime	Transfert a time
//
// Misc:
// -----
//   class XawTaskReportMode	Toggle button used by XawTaskReport
//   class XawTaskSortMode	Toggle button used by XawTaskReport
//
#ifndef _XAW_TASK_MGR_H_
#define	_XAW_TASK_MGR_H_

#include "Error.H"
#include "XawDialog.H"
#include "XawLauncher.H"
#include "XawTaskList.H"

// ----------------------------------------------------------------------
// Class :	XawTaskDialog
//
// Role :	Creation/management of a dialog box to edit
//		the tasks.
// 
class XawTaskDialog : public XawDialog, public TaskList {
protected:
    TaskControl& xcTask;
public:
    XawTaskDialog(const char* _name, TaskControl& _task);
    ~XawTaskDialog();
};



// ----------------------------------------------------------------------
// Class :	XawTaskEditReference
//
// Role :	Creation/management of a dialog box to edit the
//		task references.
// 
class XawTaskEditReference : public XawTaskDialog, public XawTaskList {
public:
    XawTaskEditReference(const char* _name, TaskControl& _task);
    ~XawTaskEditReference();

	//
	// Update each task reference according to what the user filled.
	//
    void activate();
};


// ----------------------------------------------------------------------
// Class :	XawTaskEditForecasts
//
// Role :	Creation/management of a dialog box to edit the
//		task forecasts.
// 
class XawTaskEditForecasts : public XawTaskDialog, public XawTaskList {
public:
    XawTaskEditForecasts(const char* _name, TaskControl& _task);
    ~XawTaskEditForecasts();

	//
	// Update each task forecasts according to what the user filled.
	//
    void activate();
};


// ----------------------------------------------------------------------
// Class :	XawTaskEditTimes
//
// Role :	Creation/management of a dialog box to edit the task times.
// 
class XawTaskEditTimes : public XawDialog, public XawTaskSelector {
protected:
    TaskControl& xcTask;
public:
    XawTaskEditTimes(const char* _name, TaskControl& _task);
    ~XawTaskEditTimes();

	//
	// Update each task time according to what the user filled.
	//
    void activate();
};


// ----------------------------------------------------------------------
// Class :	XawTaskSelect
//
// Role :	Creation/management of a dialog box to select
//		the active task.
// 
class XawTaskSelect : public XawTaskDialog, public XawTaskList {
public:
    XawTaskSelect(const char* _name, const TaskFilterMode _mode,
		  TaskControl& _task);
    ~XawTaskSelect();

	//
	// Select the task `task' as the new task to bill.
	//
    void select(XawTaskCharge& _task);
};

class XawTaskReport;

enum TaskReportMode {
   ReportDay,
   ReportWeek,
   ReportFortnight,
   ReportMonth
};


// ----------------------------------------------------------------------
// Class :	XawTaskReportMode
//
// Role :	Toggle button to define a report mode (enum TaskReportMode).
//
class XawTaskReportMode : public XawText {
protected:
    XawTaskReport&	xReport;
    TaskReportMode	xReportMode;
public:
    XawTaskReportMode(const char* _name, const TaskReportMode _mode,
		      XawTaskReport& _report);
    ~XawTaskReportMode();

    void select(XtPointer _data);
};


// ----------------------------------------------------------------------
// Class :	XawTaskSortMode
//
// Role :	Toggle button to define a sort mode (enum TaskSortMode).
//
class XawTaskSortMode : public XawText {
protected:
    XawTaskReport&	xReport;
    TaskSortMode	xSortMode;
public:
    XawTaskSortMode(const char* _name, const TaskSortMode _mode,
		    XawTaskReport& _report);
    ~XawTaskSortMode();

    void select(XtPointer _data);
};


// ----------------------------------------------------------------------
// Class :	XawTaskReport
//
// Role :	Dialog box to report the time spent for each task.
//
class XawTaskReport : public XawDialog, public XawTaskSelector,
		      public XawLauncher {
	friend class XawTaskSortMode;
protected:
		//
		// Below the list of tasks (from XawTaskSelector):
		//
		// 1 label to summarize the report.
		//
    XawText		xTotalLabel;

		//
		// 4 toggle buttons to select the time to report.
		//
    XawText		xTimeLabel;
    XawTaskReportMode	xDayReport;
    XawTaskReportMode   xWeekReport;
    XawTaskReportMode   xFortnightReport;
    XawTaskReportMode   xMonthReport;

		//
		// 5 toggle buttons to select the sort mode.
		//
    XawText		xSortLabel;
    XawTaskSortMode     xNameSort;
    XawTaskSortMode     xDaySort;
    XawTaskSortMode     xWeekSort;
    XawTaskSortMode     xFortSort;
    XawTaskSortMode     xMonthSort;

		//
		// Current report mode.
		//
    TaskReportMode      xMode;

		//
		// Current sort mode.
		//
    TaskSortMode        xSortMode;
    Boolean		xDisableEmpty;
    char*		xTotalModel;
    TaskControl&        xcTask;

public:
    XawTaskReport(const char* _name, TaskControl& _task);
    ~XawTaskReport();

	//
	// Return the topLevel form widget.
	//
    inline Widget form() const;

	//
	// Launch an external command.
	//
    void activate();

	//
	// Returns the flags associated to the month of the day.
	//
    int dayFlags(const unsigned _mday) const;

	//
	// Update the list of tasks.
	//
    void update();

	//
	// Redefine XawLauncher::close to be notified when the command
	// has finished.
	//
    int close();

	//
	// Change the mode of the report and update the dialog box.
	//
    void selectMode(const TaskReportMode _mode);

	//
 	// Change the sort method and update the dialog box.
	//
    void selectSort(const TaskSortMode _mode);

    Widget list() const;
};

    inline Widget
XawTaskReport::form() const
{
    return XawDialog::xForm;
}


// ----------------------------------------------------------------------
// Class :	XawTaskRemove
//
// Role :	Dialog box to remove a task.
//
class XawTaskRemove : public XawTaskSelect {
public:
    XawTaskRemove(const char* _name, TaskControl& _task);
    ~XawTaskRemove();

	//
	// Remove the task `task'.
	//
    void select(XawTaskCharge& _task);
};


// ----------------------------------------------------------------------
// Class :	XawTaskAdd
//
// Role :	Dialog box holding 2 text fields to create a new task.
//
#ifdef HAVE_IMPORT
class XawTaskAdd : public XawDialog,
		   public TaskList,
  		   public XawTaskList {
#else
class XawTaskAdd : public XawDialog {
#endif
protected:
    XawText		xName;
    XawText		xReference;
    XawText		xForecast;
    XawText		xDate;
    TaskControl&	xTask;

#ifdef HAVE_IMPORT
	//
	// Import a list of tasks by reading the file `_file'. The
	// The list of tasks is then displayed in the dialog box.
	// Tasks are sorted on their name.
	//
    void importTasks(const char* _file);
#endif
public:
    XawTaskAdd(const char* _name, TaskControl& _task);
    ~XawTaskAdd();

    void activate();
#ifdef HAVE_IMPORT
	//
	// Remove the task `task'.
	//
    void select(XawTaskCharge& _task);
#endif
};


// ----------------------------------------------------------------------
// Class :	XawTaskMoveTime
//
// Role :	Dialog box to transfert the time from/to another task.
//
class XawTaskMoveTime : public XawTaskSelect, private XawText {
protected:
    int			xMoveMode;
    XawTaskCharge*	xSelected;
public:
    XawTaskMoveTime(const char* _name, const int mode, TaskControl& _task);
    ~XawTaskMoveTime();

    void select(XawTaskCharge& _task);

    void activate();
};

#endif
