// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Task Control
// Author	: S. Carrez
// Date		: Sat Feb 25 12:59:45 1995
// Version	: $Id: TaskControl.C,v 1.11 1996/08/04 15:35:55 gdraw Exp $
// Project	: Xcra
// Keywords	: Task, Time, Control, Report
//
// Copyright (C) 1994, 1995, 1996 Stephane Carrez
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
// class TaskControl	Manages the update of charge for the tasks
// 
//
#include "config.H"

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>

#ifdef HAVE_SYS_UTSNAME_H
# include <sys/utsname.h>
#endif

#include "Error.H"
#include "Date.H"
#include "Task.H"
#include "TaskControl.H"


// ----------------------------------------------------------------------
//
//			TaskControl
//
// ----------------------------------------------------------------------

//
// Create the task monitor object.
//
TaskControl::TaskControl()
{
    time(&tcLastTime);
    tcStartTime = tcLastTime;
    Day d(tcLastTime);

    tcCurDay = Date::monthDayOf(d.year(), d.day());
}


//
// Delete the task monitor object.
//
TaskControl::~TaskControl()
{
    close();
}


//
// Load the file and setup everything.
//
    int
TaskControl::load()
{
    time_t	curTime;
    int		result;

    bool	newFile = false;
    unsigned	year    = 0;
    unsigned	month   = 0;
    unsigned    curDay  = 0;

        //
        // Get the time to load the newest charge file.
        //
    time(&curTime);

    for (unsigned loop = 0; ; loop++) {
	Day d(curTime);

	result = loadFile(d.year(), d.day(), TaskMonthTime);
	if (result != E_OPEN_ERROR)
	    break;

	        //
	        // See whether 28 days before today we had a charge file.
	        //
        curTime -= 28 * 24 * 3600;
        if (loop > ((365 + 27) / 28)) {
	    Error::printf(MSG_NOCRAFILE, tcDir);
	    addTask("EmptyTask", "", 0);
	    break;
        }

		//
		// In that case, we must not take into account the charge of
		// the tasks: they  correspond to the previous month. This is
		// just used to read in the task names and references.
		//
	if (newFile == 0) {
	    year  = d.year();
	    month = d.month();
	    curDay= Date::monthDayOf(year, d.day());
	    newFile = true;
	}
    }
    

	//
	// We have loaded a file which correspond to the previous month.
	// Clear the times and set the new month/year and file.
	//
    if (newFile && (result == 0 || result == E_OPEN_ERROR)) {
	clear();

	(void) setMode(year, month, TaskMonthTime);

	tcCurDay = curDay;
	result = 0;
    }
    return result;
}


//
// Update the time for the current task.
//
    int
TaskControl::updateTime()
{
    time_t	curTime;

    time(&curTime);

    tcCurTask[tcCurDay] += curTime - tcLastTime;
    tcCurTask.tcForecastTime -= curTime - tcLastTime;
    tcLastTime = curTime;
    
    tcChanged  = true;

	//
	// Detect when the day changes.
	//
    Day d(curTime);

    const unsigned day = Date::monthDayOf(d.year(), d.day());

    int result = 0;
    if (day != (unsigned) tcCurDay) {

		//
		// Save the current times.
		//
	result = save();
	if (result == 0) {
		//
		// Reload the times (another day and may be another month).
		//
	    result = load();
	}
    }
    return result;
}


//
// Charge the task whose name is `_name'. The task object
// is moved at beginning of the list.
//
    int
TaskControl::charge(const char* _name)
{
	//
	// Find the task object.
	//
    if (contains(_name) == 0) {
	Error::printf(MSG_NOTASK, _name);
	return E_NO_TASK;
    }

    TaskCharge& task = operator [](_name);

    int result = updateTime();

	//
	// Move it at front of the list
	// (Just maintain the list of tasks in an LRU order).
	//
    size_t sz = (char*)(&task) - (char*)(tcList);
    if (sz != 0) {
	TaskCharge t(task);

	memmove(&tcList[1], tcList, sz);
	tcList[0] = t;
    }
    
    return result;
}


//
// Move a time from the task `_name' to the current task
// The time is specified in `_t'. The sign `_sign'
// indicates whether we must substract or add.
//
    int
TaskControl::moveTime(const char* _name, time_t _value, int _sign)
{
    if (contains(_name) == 0) {
	Error::printf(MSG_NOTASK, _name);
	return E_NO_TASK;
    }
    TaskCharge& task = operator [](_name);

    if (_sign < 0) {
	if (tcCurTask[tcCurDay] < _value) {
	    Error::printf(MSG_CANT_SUBSTRACT);
	    return E_CANT_MODIFY;
	}
	task[tcCurDay] += _value;
	task.tcForecastTime -= _value;
	_value = -_value;
    } else {
	if (task[tcCurDay] < _value) {
	    Error::printf(MSG_CANT_SUBSTRACT);
	    return E_CANT_MODIFY;
	}
	task[tcCurDay] -= _value;
	task.tcForecastTime += _value;
    }
    tcCurTask[tcCurDay] += _value;
    tcCurTask.tcForecastTime -= _value;
    return 0;
}


//
// Delete the task `_name'. The task is deleted only if the time
// reported during the complete month does not exceed 30 seconds per day.
//
    int
TaskControl::delTask(const char* _name)
{
	//
	// Sanity checks.
	//
    if (contains(_name) == 0) {
	Error::printf(MSG_NOTASK, _name);
	return -1;
    }

    if (tcCurTask == _name) {
	Error::printf(MSG_CANT_DELETE_CURRENT);
	return -1;
    }

	//
	// To prevent someone to loose a task which report some time,
	// the task cannot be deleted if the sum for the month is not
	// empty or the sum for the fortnight is not empty.
	//
    unsigned long t;

    t = sum(_name, SumMonth);
    if (t > MIN_DELETE_CHARGE * NR_DAYS) {
	Error::printf(MSG_SUM_MONTH_NON_EMPTY, _name, t);
	return -1;
    }

    t = sum(_name, SumFortnight);
    if (t > MIN_DELETE_CHARGE * (NR_DAYS / 2)) {
	Error::printf(MSG_SUM_FORTNIGHT_NON_EMPTY, _name, t);
	return -1;
    }

    return TaskList::delTask(_name);
}


//
// Add a new task and assign it a reference.
//
    void
TaskControl::addTask(const char* _name, const char* _ref, const long _forecast)
{
    TaskCharge& task = operator [](_name);

    task.setReference(_ref);
    task.setForecasts(_forecast, _forecast);
}


//
// Setup the directory where the cra files are saved.
//
    void
TaskControl::setDirectory(char* _dir)
{
    tcDir = strdup(_dir);
    TaskListCache::setDirectory(_dir);

	//
	// Check that the directory exists, try to create it
	//
    struct stat st;
    int res = ::stat(tcDir, &st);
    if (res != 0) {
	MailErrorHandler info;

	Error::printf("Welcome,\n"
		"It seems that this is the first time you are using xcra.\n"
		"I need a directory where to put all the report files.\n"
		"The tool is configured to use `%s' as directory.\n"
		"An X resource and a command line option are available\n"
		"if you want to change to another directory.\n",
		tcDir);

	res = mkdir(tcDir, 0700);
	if (res != 0) {
	    Error::report("\nHum...It is impossible to create that directory.",
			  tcDir);
	} else {
	    Error::printf("\nThe directory was created successfully.\n");
	}
    }
    checkStartup();
}


//
// Check that only one controller is active at a time.
//
    void
TaskControl::checkStartup()
{
    char file[1024];
    char host[512];
    FILE* fp;

#ifdef HAVE_SYS_UTSNAME_H
    struct utsname ut;

    if (uname(&ut) < 0) {
	Error::report(MSG_HOSTNAME_ERROR, "");
        exit(1);
    }
    strcpy(host, ut.nodename);
#else
    if (gethostname(host, sizeof(host) - 1) != 0) {
	Error::report(MSG_HOSTNAME_ERROR, "");
	exit(1);
    }
#endif
    
    sprintf(file, "%s/.xcra-startup", tcDir);
    fp = fopen(file, "r");
    if (fp != 0) {
	int pid, n;

	n = fscanf(fp, "%d %s", &pid, file);
	fclose(fp);
	if (n == 2) {
	    if (strcmp(host, file) == 0) {
		//
		// Check whether the process is still.
		//
		if (kill(pid, 0) == 0) {
		    {
		      MailErrorHandler info;

		      Error::printf("Dear user, you have already started"
		      		    " XCra on the current host.\n"
		  		    "It is not possible to start it twice.\n");
		    }
		    exit(1);
		}
	    } else {
		fprintf(stderr, "Dear user, you have already started"
				" XCra on %s.\nIt is not possible to "
				"start it twice.\n", file);
		exit(1);
	    }
	}
    }

    sprintf(file, "%s/.xcra-startup", tcDir);
    fp = fopen(file, "w");
    if (fp == 0) {
	fprintf(stderr, "Cannot create startup file\n");
	exit(1);
    }
    fprintf(fp, "%d %s\n", getpid(), host);
    if (fclose(fp) != 0) {
	unlink(file);
	fprintf(stderr, "Cannot create startup file\n");
	exit(1);
    }
}


//
// Close.
//
    void
TaskControl::close()
{
    char file[1024];

    save();

    sprintf(file, "%s/.xcra-startup", tcDir);
    unlink(file);
}   
