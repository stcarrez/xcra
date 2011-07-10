// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Task Control
// Author	: S. Carrez
// Date		: Sat Feb 25 13:03:37 1995
// Version	: $Id: TaskControl.H,v 1.8 1996/08/04 15:35:58 gdraw Exp $
// Project	: Xcra
// Keywords	: Task, Monitor
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
// class TaskControl	Controls a list of tasks and update the time
// 
//
#ifndef _TASK_CONTROL_H_
#define	_TASK_CONTROL_H_

//
// Minimum time value above which deleting a task is refused.
//
#define	MIN_DELETE_CHARGE	(30)


// ----------------------------------------------------------------------
// Class :	TaskControl
//
// Role :	Task monitor. Update the time for the current task
//		and provide some methods to switch the active task.
//
class TaskControl : public TaskList {
protected:
	//
	// Last update time.
	//
    time_t	tcLastTime;

	//
	// First update time (startup time).
	//
    time_t	tcStartTime;

	//
	// Check that only one controller is active at a time.
	//
    void checkStartup();

	//
	// Close.
	//
    void close();

#define	tcCurTask	tcList[0]
public:
    TaskControl();
    ~TaskControl();

	//
	// Load the file and setup everything.
	//
    virtual int	load();

	//
	// Update the time for the current task.
	//
    virtual int	updateTime();

	//
	// Charge the task whose name is `_name'. The task object
	// is moved at beginning of the list.
	//
    virtual int	charge(const char* _name);

	//
	// Move a time from the task `_name' to the current task
	// The time is specified in `_t'. The sign `_sign'
	// indicates whether we must substract or add.
	//
    virtual int	moveTime(const char* _name, time_t _t, int _sign);

	//
	// Delete a task.
	//
    int delTask(const char* _name);

	//
	// Add a new task and assign it a reference.
	//
    void addTask(const char* _name, const char* _ref, const long _forecast);

	//
	// Setup the directory where the cra files are saved.
	//
    void setDirectory(char* dir);
};

#endif
