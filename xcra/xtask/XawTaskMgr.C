// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Interface for the management of Tasks
// Author	: S. Carrez
// Date		: Sat Feb 25 13:28:01 1995
// Version	: $Id: XawTaskMgr.C,v 1.15 2000/02/23 08:23:31 ciceron Exp $
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
// Contents :
// ----------
// class XawTaskDialog
// class XawTaskEditReference
// class XawTaskEditForecasts
// class XawTaskEditTimes
// class XawTaskSelect
// class XawTaskRemove
// class XawTaskAdd
// class XawTaskMoveTime
// class XawTaskReport
// class XawTaskReportMode
// class XawTaskSortMode
// 
#include "config.H"
#include <stdio.h>

#include "Task.H"
#include "TaskControl.H"

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include "XawDialog.H"
#include "XawTaskMgr.H"
#include "XawTaskList.H"

typedef struct {
   char*	xtcDayReport;
   char*	xtcWeekReport;
   char*	xtcFortnightReport;
   char*	xtcMonthReport;

   char*	xtcDayTotal;
   char*	xtcWeekTotal;
   char*	xtcFortnightTotal;
   char*	xtcMonthTotal;

   char*	xtcReportCommand;
#ifdef HAVE_IMPORT
   char*	xtcImportTasks;
#endif
} xtcData;

#undef Offset
#define Offset(field) (XtOffsetOf(xtcData, field))

static XtResource xtcResources[] = {
  {"dayReportModel",  "ReportModel", XtRString, sizeof(char*),
     Offset(xtcDayReport), XtRString, 
    (XtPointer) "%-20.20N %-20.20R %2tH:%02tM"},

  {"weekReportModel", "ReportModel", XtRString, sizeof(char*),
     Offset(xtcWeekReport), XtRString, 
    (XtPointer) "%-20.20N %-20.20R %2wH:%02wM"},

  {"fortnightReportModel", "ReportModel", XtRString, sizeof(char*),
     Offset(xtcFortnightReport), XtRString, 
    (XtPointer) "%-20.20N %-20.20R %2fH:%02fM"},

  {"monthReportModel", "ReportModel", XtRString, sizeof(char*),
     Offset(xtcMonthReport), XtRString,
    (XtPointer) "%-20.20N %-20.20R %2aH:%02aM"},


	//
	// Label models to report totals in the report form.
	//
  {"dayTotalModel",  "ReportModel", XtRString, sizeof(char*),
     Offset(xtcDayTotal), XtRString,
     (XtPointer) "Total day %2TH:%02TM"},

  {"weekTotalModel", "ReportModel", XtRString, sizeof(char*),
     Offset(xtcWeekTotal), XtRString,
    (XtPointer) "Total day %2TH:%02TM   Total week %2WH:%02WM"},

  {"fortnightTotalModel", "ReportModel", XtRString, sizeof(char*),
     Offset(xtcFortnightTotal), XtRString,
     (XtPointer) "Total day %2TH:%02TM   Total fortnight %2FH:%02FM"},

  {"monthTotalModel", "ReportModel", XtRString, sizeof(char*),
     Offset(xtcMonthTotal), XtRString,
     (XtPointer) "Total day %2TH:%02TM   Total month %2AH:%02AM"},

  {"reportCommand", "ReportCommand", XtRString, sizeof(char*),
     Offset(xtcReportCommand), XtRString,
       (XtPointer) REPORT_COMMAND }
#ifdef HAVE_IMPORT
,

  {"importCommand", "ImportCommand", XtRString, sizeof(char*),
     Offset(xtcImportTasks), XtRString,
       (XtPointer) IMPORT_COMMAND }
#endif
};


static xtcData rsrc;
static bool    rsrcInitialized = false;

 
// ----------------------------------------------------------------------
//
//			XawTaskDialog
//
// Top level for dialog boxes which need to refer to the list of tasks.
//
// ----------------------------------------------------------------------

XawTaskDialog::XawTaskDialog(const char* _name, TaskControl& _task)
	//
	// Create the top level dialog box.
	//
 : XawDialog(_name),

	//
	// Get a copy of the TaskList (so that another dialog can add/remove
	// some tasks without problems).
	//
   TaskList(_task),

   xcTask(_task)
{
}

XawTaskDialog::~XawTaskDialog()
{
}


// ----------------------------------------------------------------------
//
//			XawTaskEditReference
//
// Creation/management of a dialog box to edit the task references.
//
// ----------------------------------------------------------------------

XawTaskEditReference::XawTaskEditReference(const char* _name,
					   TaskControl& _task)
	//
	// Create the top level dialog box (with a backup copy of the tasks).
	//
  : XawTaskDialog(_name, _task),

	//
	// Fill the dialog box with the copied list of tasks.
	//
    XawTaskList(_name, *this, XAW_LABEL | XAW_TEXT,
	        "%R", XawDialog::xForm, None)
{
}

XawTaskEditReference::~XawTaskEditReference()
{
}


//
// Update each task reference according to what the user specified.
// Each text field is checked and the task references are updated
// only when all of them are valid.
//
    void
XawTaskEditReference::activate()
{
    bool errors = false;

	//
	// Verify each text field.
	//
    unsigned i;
    for (i = 0; i < xListSize; i++) {
	XawTaskCharge& xc = xList[i][0];
	const char* r = xc;

	if (r[0]
	    && !XawText::acceptableText(r, MAX_REF_LEN, "Task reference")) {
	    errors = true;
	    xc.setSemanticError(true);
	} else {
	    xc.setSemanticError(false);
	}
    }
    if (errors)
	return;

	//
	// Update each task reference.
	//
    for (i = 0; i < xListSize; i++) {
	XawTaskCharge& xc = xList[i][0];

		//
		// Verify that the task exists (it may have been deleted from
		// another dialog box).
		//
	if (xcTask.contains(xc.name())) {
	    TaskCharge& task = xcTask[xc.name()];

	    task.setReference(xc);
	}
    }
    XawDialog::activate();
}


// ----------------------------------------------------------------------
//
//			XawTaskEditForecasts
//
// Creation/management of a dialog box to edit the task forecasts.
//
// ----------------------------------------------------------------------

XawTaskEditForecasts::XawTaskEditForecasts(const char* _name,
					   TaskControl& _task)
	//
	// Create the top level dialog box (with a backup copy of the tasks).
	//
  : XawTaskDialog(_name, _task),

	//
	// Fill the dialog box with the copied list of tasks.
	//
    XawTaskList(_name, *this, XAW_LABEL | XAW_TEXT,
	        "%P", XawDialog::xForm, None)
{
}

XawTaskEditForecasts::~XawTaskEditForecasts()
{
}


//
// Update each task reference according to what the user specified.
// Each text field is checked and the task references are updated
// only when all of them are valid.
//
    void
XawTaskEditForecasts::activate()
{
    bool errors = false;

	//
	// Verify each text field.
	//
    unsigned i;
    for (i = 0; i < xListSize; i++) {
	XawTaskCharge& xc = xList[i][0];
	const char* r = xc;
	time_t t = 0;

	if (r[0] && Date::timeValue(r, t) != 0) {
	    errors = true;
	    Error::printf(MSG_BADTIME_FORMAT, r);
	    xc.setSemanticError(true);
	} else {
	    xc.setSemanticError(false);
	}
    }
    if (errors)
	return;

	//
	// Update each task forecast.
	//
    for (i = 0; i < xListSize; i++) {
	XawTaskCharge& xc = xList[i][0];
	const char* r = xc;

		//
		// Verify that the task exists (it may have been deleted from
		// another dialog box).
		//
	if (xcTask.contains(xc.name())) {
	    TaskCharge& task = xcTask[xc.name()];
	    time_t t;

	    if (r[0] != 0 && Date::timeValue(r, t) == 0) {
		task.updateForecast((long) t);
	    }
	}
    }
    xcTask.updateTime();
    xcTask.save();
    XawDialog::activate();
}


// ----------------------------------------------------------------------
//
//			XawTaskEditTimes
//
// Creation/management of a dialog box to edit the time values.
//
// ----------------------------------------------------------------------

XawTaskEditTimes::XawTaskEditTimes(const char* _name, TaskControl& _task)
	//
	// Create the top level dialog box.
	//
  : XawDialog(_name),

	//
	// Create the task selector with a backup copy of the tasks and
	// a calendar selection object.
	//
    XawTaskSelector(_name, _task, XAW_LABEL | XAW_TEXT,
		    XawDialog::xForm, None),

    xcTask(_task)
{
}

XawTaskEditTimes::~XawTaskEditTimes()
{
}


//
// Modify the time values for each task and for a particular day.
// Each text field is checked and converted into a time which is
// put in the task list (and in the selected day). When all the
// text fields are validated (correct), the task list is added
// to the task manager list.
//
    void
XawTaskEditTimes::activate()
{
    bool errors = false;

    clear();
    for (unsigned i = 0; i < xListSize; i++) {
	XawTaskCharge& xc = xList[i][0];

	if (xc.set(tcCurDay) == 0) {
	    xc.setSemanticError(false);
	} else {
	    Error::printf(MSG_WRONG_TIME_SPEC);
	    xc.setSemanticError(true);
	    errors = true;
	}
    }

	//
	// If an error is detected, the current task-list must be re-loaded
	// because it contains invalid values.
	//
    if (errors) {
	TaskList::invalidate();
	TaskList::loadFile(year(),
			   Date::dayOfYear(year(), month(), tcCurDay),
			   mode());
	return;
    }

	//
	// Check if we must add directly to the task manager.
	//
    if (xcTask.month() == month() && xcTask.year() == year()) {
	xcTask += *this;
	xcTask.updateTime();
	xcTask.save();
    
    } else {
	TaskList tlist(xcTask);

	//
	// Load the file and update.
	//
	if (tlist.loadFile(year(), Date::dayOfYear(year(), month(), tcCurDay),
			mode()) == 0) {
	    tlist += *this;
	    tlist.save();
	}
    }

	//
	// Reset the dialog box to be ready to modify something
	// in another day. We need to re-load the list to get the new values.
	//
    clear();
    TaskList::invalidate();
    int curDay = tcCurDay;
    TaskList::loadFile(year(), Date::dayOfYear(year(), month(), tcCurDay),
		       mode());
    tcCurDay = curDay;
    XawTaskSelector::update();
}


// ----------------------------------------------------------------------
//
//			XawTaskSelect
//
// Creation/management of a dialog box to select a task.
//
// ----------------------------------------------------------------------

XawTaskSelect::XawTaskSelect(const char* _name, const TaskFilterMode _mode,
			     TaskControl& _task)
	//
	// Create the top level dialog box (with a backup copy of the tasks).
	//
  : XawTaskDialog(_name, _task),

	//
	// Fill the dialog box with the copied list of tasks.
	//
    XawTaskList(_name, *this, XAW_TOGGLE, "", XawDialog::xForm, None)
{
    TaskList::filter(_mode);
    TaskList::sort(SortDayTime);
    XawTaskList::update();
}

XawTaskSelect::~XawTaskSelect()
{
}


//
// Select the task `_task' as being the current task.
//
    void
XawTaskSelect::select(XawTaskCharge& _task)
{
    xcTask.charge(_task.name());
    activate();
}


// ----------------------------------------------------------------------
//
//			XawTaskRemove
//
// Creation/management of a dialog box to remove a task.
//
// ----------------------------------------------------------------------

XawTaskRemove::XawTaskRemove(const char* _name, TaskControl& _task)
  : XawTaskSelect(_name, FilterNone, _task)
{
}

XawTaskRemove::~XawTaskRemove()
{
}


//
// Delete the selected task `_task'.
//
    void
XawTaskRemove::select(XawTaskCharge& _task)
{
    xcTask.delTask(_task.name());
    activate();
}


// ----------------------------------------------------------------------
//
//			XawTaskMoveTime
//
// Creation/management of a dialog box to move a time from/to another task.
//
// ----------------------------------------------------------------------

XawTaskMoveTime::XawTaskMoveTime(const char* _name, const int _mode,
				 TaskControl& _task)
	//
	// Create the top level dialog box (with a backup copy of the tasks).
	//
  : XawTaskSelect(_name, (_mode == 0) ? FilterDayEmpty : FilterNone, _task),

    XawText("time", XAW_LABEL | XAW_TEXT, 0, form(), XawTaskList::xViewport)
{
    if (xListSize) {
	xSelected = xList[0];
    } else {
	xSelected = 0;
    }
    xMoveMode = _mode;
}

XawTaskMoveTime::~XawTaskMoveTime()
{
}


//
// Select the task onto which the operation must be applied.
//
    void
XawTaskMoveTime::select(XawTaskCharge& _task)
{
    xSelected = &_task;
}


//
// Transfer the time to/from the selected task and the current task.
//
    void
XawTaskMoveTime::activate()
{
    time_t v;

	//
	// Verify the time value.
	//
    if (Date::timeValue(*this, v) != 0) {
        const char* p = *this;
	setSemanticError(true);
	Error::printf(MSG_BADTIME_FORMAT, p);
	return;
    }

    if (xSelected == 0)
	return;

	//
	// Transfert the time value.
	//
    int result = 0;
    switch (xMoveMode) {
	case 0 :
	    result = xcTask.moveTime(xSelected->name(), v, 0);
	    break;

	case 1 :
	    result = xcTask.moveTime(xSelected->name(), v, -1);
	    break;

	case 2 :
	    result = xcTask.moveTime(xSelected->name(), v, -1);
	    if (result == 0) {
		xcTask.charge(xSelected->name());
	    }
	    break;
    }

    if (result != 0) {
	setSemanticError(true);
    } else {
	XawDialog::activate();
    }
}


// ----------------------------------------------------------------------
//
//			XawTaskAdd
//
// Creation/management of a dialog box to create a new task.
//
// ----------------------------------------------------------------------

XawTaskAdd::XawTaskAdd(const char* _name, TaskControl& _task)
	//
	// Create the top level dialog box.
	//
  : XawDialog(_name),

#ifdef HAVE_IMPORT
    TaskList(),

    XawTaskList(_name, *this, XAW_TOGGLE, "", XawDialog::xForm, None),

	//
	// Create the _name label & text edition.
	//
    xName("task", XAW_TEXT | XAW_LABEL, 0, XawDialog::xForm,
	  XawTaskList::xViewport),
#else
	//
	// Create the _name label & text edition.
	//
    xName("task", XAW_TEXT | XAW_LABEL, 0, XawDialog::xForm, None),
#endif
	//
	// Create the task reference label & text.
	//
    xReference("refName", XAW_TEXT | XAW_LABEL, 0, XawDialog::xForm, xName),

	//
	// Create the forecast for this task.
	//
    xForecast("forcast", XAW_TEXT | XAW_LABEL, 0, XawDialog::xForm,
	      xReference),

	//
	// Create the creation-date for this task.
	//
    xDate("date", XAW_TEXT | XAW_LABEL, 0, XawDialog::xForm, xForecast),

    xTask(_task)
{
    char buf[1024];

    sprintf(buf, "%d/%d", xTask.month(), xTask.year());
    xDate = buf;

#ifdef HAVE_IMPORT
	//
	// Get our resources (only once).
	//
    if (rsrcInitialized == false) {
	rsrcInitialized = true;
	XtGetApplicationResources(xAppMainDialog->TopLevel(),
				  (XtPointer) &rsrc,
				  xtcResources,
				  XtNumber(xtcResources),
				  NULL, 0);

    }

    sprintf(buf, "%s/import.tasks", TaskListCache::getDirectory());
    struct stat st;
    time_t t;

    time(&t);
    if (stat(buf, &st) != 0 || (t - st.st_mtime > 60 * 3600)
	&& rsrc.xtcImportTasks && rsrc.xtcImportTasks[0]) {
	Error::printf(MSG_IMPORTING_TASKS);
	system(rsrc.xtcImportTasks);
    }
    importTasks(buf);
#endif
}

XawTaskAdd::~XawTaskAdd()
{
}

#ifdef HAVE_IMPORT
//
// Import a list of tasks by reading the file `_file'. The
// The list of tasks is then displayed in the dialog box.
// Tasks are sorted on their name.
//
    void
XawTaskAdd::importTasks(const char* _file)
{
    tcFile = _file;
    switch (TaskList::load()) {
	case E_OPEN_ERROR :
	    Error::report(MSG_LOADERROR, tcFile);
	    tcFile = 0;
	    return;

	case 0 :
	    tcFile = 0;
	    break;

	default :
	    tcFile = 0;
	    return;
    }

    TaskList::sort(SortName);
    XawTaskList::update();

	// TODO: Be smart and remove all the tasks which are already
	// present in our list.
}


//
// The task `_task' was selected from the list of imported tasks.
// Update the Task Name and Task Reference fields (but do not validate
// the dialog box so that it is possible to change the task name).
//
    void
XawTaskAdd::select(XawTaskCharge& _task)
{
    xName      = _task.name();
    xReference = _task.task().reference();
}
#endif


//
// Create the new task given its name and reference.
//
    void
XawTaskAdd::activate()
{
    const char* task     = xName;
    const char* ref      = xReference;
    const char* forecast = xForecast;
    const char* date     = xDate;
    time_t t = 0;
    bool errors;

	//
	// Verify the task name.
	//
    if (XawText::acceptableText(task, MAX_NAME_LEN, "Task name") == false) {
	errors = true;
    } else if (xTask.contains(task)) {
	errors = true;
	Error::printf(MSG_TASK_EXISTS);
    } else {
	errors = false;
    }

    xName.setSemanticError(errors);

	//
	// Verify the reference string.
	//
    if (ref[0]
	&& !XawText::acceptableText(ref, MAX_REF_LEN, "Reference name")) {
	errors = true;
	xReference.setSemanticError(true);
    } else {
	xReference.setSemanticError(false);
    }

	//
	// Verify the forecast string.
	//
    if (forecast[0] != 0 && Date::timeValue(forecast, t) != 0) {
	errors = true;
	xForecast.setSemanticError(true);
    } else {
	xForecast.setSemanticError(false);
    }

	//
	// Verify the date.
	//
    unsigned month = xTask.month();
    unsigned year  = xTask.year();
    if (date[0] != 0 && sscanf(date, "%u/%u", &month, &year) != 2
	|| month == 0 || month > 12 || year < 1970 || year > xTask.year()
	|| (month > xTask.month() && year == xTask.year())) {
	errors = true;
	xDate.setSemanticError(true);
	Error::printf(MSG_WRONG_DATE);
    } else {
	xDate.setSemanticError(false);
    }
    if (errors == false) {
	bool oneCreated = false;

		//
		// Create the new task in all the task-month file
		// from the date specified in `xDate' up to the
		// previous month.
		//
	while (year != xTask.year() || month != xTask.month()) {
	    TaskList tlist(xTask);

	    if (tlist.loadFile(year, Date::dayOfYear(year, month, 1),
				xTask.mode()) != 0) {
		if (oneCreated == false) {
		    xDate.setSemanticError(true);
		    Error::printf(MSG_WRONG_DATE);
		    return;
		}
	    } else {
		TaskCharge& newTask = tlist.addTask(task);
		newTask.setReference(ref);
		newTask.setForecasts(t, t);

		tlist.save();
		oneCreated = true;
	    }
	    if (month == 12) {
		month = 1;
		year ++;
	    } else {
		month ++;
	    }
	}
	xTask.addTask(task, ref, (long) t);
	XawDialog::activate();
    }
}


// ----------------------------------------------------------------------
//
//			XawTaskReport
//
// Creation/management of a report dialog box.
//
// ----------------------------------------------------------------------

XawTaskReportMode::XawTaskReportMode(const char* _name,
				     const TaskReportMode _mode,
				     XawTaskReport& _report)
  : XawText(_name, XAW_TOGGLE, 0, _report.form(), _report.list()),
    xReport(_report)
{
    xReportMode = _mode;
}

XawTaskReportMode::~XawTaskReportMode()
{
}

    void
XawTaskReportMode::select(XtPointer _data)
{
    if ((long)(_data) == 1) {
	xReport.selectMode(xReportMode);
    }
}

XawTaskSortMode::XawTaskSortMode(const char* _name, const TaskSortMode _mode,
				 XawTaskReport& _report)
  : XawText(_name, XAW_TOGGLE, 0, _report.form(), _report.xSortLabel),
    xReport(_report)
{
    xSortMode = _mode;
}

XawTaskSortMode::~XawTaskSortMode()
{
}

    void
XawTaskSortMode::select(XtPointer _data)
{
    if ((long)(_data) == 1) {
	xReport.selectSort(xSortMode);
    }
}

XawTaskReport::XawTaskReport(const char* _name, TaskControl& _task)
	//
	// Create the top level dialog box.
	//
  : XawDialog(_name),

	//
	// Create a calendar with the list of tasks.
	//
    XawTaskSelector(_name, _task, XAW_LABEL, XawDialog::xForm, (Widget)None),

    XawLauncher(),

	//
	// Create the total summary label.
	//
    xTotalLabel("totalLabel", XAW_LABEL | XAW_FIRST_GROUP,
		0, XawDialog::xForm, xViewport),

	//
	// Create 4 radio buttons to change the format of the report.
	//
    xTimeLabel("tLabel", XAW_LABEL | XAW_FIRST_GROUP,
		0, XawDialog::xForm, xTotalLabel),

	xDayReport("dayReport", ReportDay, *this),
	xWeekReport("weekReport", ReportWeek, *this),
	xFortnightReport("fortReport", ReportFortnight, *this),
	xMonthReport("monthReport", ReportMonth, *this),

	//
	// Create 5 radio buttons to select the sort mode of the report.
	//
    xSortLabel("sLabel", XAW_LABEL | XAW_FIRST_GROUP,
		0, XawDialog::xForm, xDayReport),

	xNameSort("nameSort", SortName, *this),
	xDaySort("daySort", SortDayTime, *this),
	xWeekSort("weekSort", SortWeekTime, *this),
	xFortSort("fortSort", SortFortnightTime, *this),
	xMonthSort("monthSort", SortMonthTime, *this),

    xcTask(_task)
{
    if (xFormatModel) {
	free((free_ptr) xFormatModel);
	xFormatModel = 0;
    }

	//
	// Get our resources (only once).
	//
    if (rsrcInitialized == false) {
	rsrcInitialized = true;
	XtGetApplicationResources(xAppMainDialog->TopLevel(),
				  (XtPointer) &rsrc,
				  xtcResources,
				  XtNumber(xtcResources),
				  NULL, 0);

    }
    xSortMode     = SortName;
    xDisableEmpty = True;
    selectMode(ReportDay);
}


XawTaskReport::~XawTaskReport()
{
    xFormatModel = 0;
}


//
// Launch an external command.
//
    void
XawTaskReport::activate()
{
    if (tcNrTasks != 0) {	// Limitation: must have at least one task.
	char cmd[512];

	tcList[0].format(cmd, rsrc.xtcReportCommand);

	if (launch(cmd) == 0) {
	    XtSetSensitive(xOk, False);
	    return;
	}	
    }
    XawDialog::activate();
}


//
// Returns the flags associated to the month of the day.
//
    int
XawTaskReport::dayFlags(const unsigned _mday) const
{
    int flags = 0;

    WeekConversion week;
    switch (xMode) {
	case ReportDay :
	    if (tcCurDay == (int) _mday)
		flags |= XAWC_DAY_HIGHLIGHTED;
	    break;

	case ReportWeek :
	    week.setWeek(*this, tcCurDay);
	    if (week.isInside(month(), _mday))
		flags |= XAWC_DAY_HIGHLIGHTED;
	    break;
	
	case ReportFortnight :
	    week.setFortnight(*this, tcCurDay);
	    if (week.isInside(month(), _mday))
		flags |= XAWC_DAY_HIGHLIGHTED;
	    break;
	
	case ReportMonth :
	    flags = XAWC_DAY_HIGHLIGHTED;
	    break;
    }

	//
	// If no time has been recorded for this day, then the day button
	// is not activated and can't be selected.
	//
    if (xDisableEmpty) {
	Day d(Date::dayOfYear(year(), month(), _mday), year());

	DayIterator iter(d, d);
	if (sum(iter, 0) == 0) {
	    flags |= XAWC_DAY_DISABLED;
	}
    }
    return flags;
}


//
// Update the list of tasks.
//
    void
XawTaskReport::update()
{
    TaskList::sort(xSortMode);

    XawTaskSelector::update();

    char buf[512];

    if (tcNrTasks != 0) {
	tcList[0].format(buf, xTotalModel);
	xTotalLabel.setLabel(buf);
    }
}


//
// Redefine XawLauncher::close to be notified when the command
// has finished.
//
    int
XawTaskReport::close()
{
    int result = XawLauncher::close();

    XawDialog::cancel();

    return result;
}	


//
// Change the mode of the report and update the dialog box.
//
    void
XawTaskReport::selectMode(const TaskReportMode _mode)
{
	//
	// Update the report format.
	//
    xMode = _mode; 
    switch (_mode) {
	case ReportDay :
	    xFormatModel = rsrc.xtcDayReport;
	    xTotalModel  = rsrc.xtcDayTotal;
	    break;

	case ReportWeek :
	    xFormatModel = rsrc.xtcWeekReport;
	    xTotalModel  = rsrc.xtcWeekTotal;
	    break;

	case ReportFortnight :
	    xFormatModel = rsrc.xtcFortnightReport;
	    xTotalModel  = rsrc.xtcFortnightTotal;
	    break;

	case ReportMonth :
	    xFormatModel = rsrc.xtcMonthReport;
	    xTotalModel  = rsrc.xtcMonthTotal;
	    break;
    }

	//
	// Redisplay the calendar (highlight of days may have changed).
	//
    XawCalendar::showCalendar(year(), month());

	//
	// Update the list of tasks (redraw each label with the new format).
	//
    XawTaskReport::update();
}


//
// Change the sort method and update the dialog box.
//
    void
XawTaskReport::selectSort(const TaskSortMode _mode)
{
	//
	// Update the report format.
	//
    xSortMode = _mode;

	//
	// Update the list of tasks (redraw each label with the new format).
	//
    XawTaskReport::update();
}



    Widget
XawTaskReport::list() const
{
    return xTimeLabel;
}

