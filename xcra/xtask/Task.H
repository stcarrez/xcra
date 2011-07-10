// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Task
// Author	: S. Carrez
// Date		: Sat Sep 23 10:28:07 1994
// Version	: $Id: Task.H,v 1.12 1996/08/04 15:35:53 gdraw Exp $
// Project	: Xcra
// Keywords	: Task
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
// class TaskCharge		Manage the time for a task (several days)
// class TaskList		Manage several tasks (list of TaskCharge)
// 
//
#ifndef _TASK_H_
#define	_TASK_H_

#include "Date.H"
#include "dlist.H"

class TaskList;

#define MAX_TASKS	100	// Max # of tasks (fixed limit => easier)
#define MAX_NAME_LEN	128	// Max length of a task name
#define MAX_NOTE_LEN	512	// Max length of a note file
#define	MAX_REF_LEN	32	// Max length of a task reference

#define EMPTY_TASK_REFERENCE "None"


// ----------------------------------------------------------------------
// Class :	TaskCharge
//
// Role :	Manage the time for a task
//
// The class TaskCharge represents a task with the time spent on it
// for several logical days. Each logical day is mapped to a physical
// day using a conversion object `WeekConversion'. The conversion object
// is managed by the class `TaskList' which also manages all the
// TaskCharge objects.
//
class TaskCharge {
    friend class TaskList; friend class TaskControl;
private:
//    void operator=(const TaskCharge&);
//    TaskCharge* operator&() { return this; };
protected:
		//
		// Task logical name.
		//
    const char* tcName;

		//
		// Task reference (as known by an external database).
		//
    const char* tcReference;

		//
		// The note file which is associated to this task
		// or 0 if must use the global note file.
		//
    const char* tcNotesFile;

		//
		// Charge time for each logical day.
		//
    time_t	tcTime[NR_DAYS];

		//
		// Forecast for the time spent on this task.
		//
    long        tcForecastTime;

		//
		//
		//
    long        tcForecastTotalTime;

		//
		// The TaskList this task belongs to. This is used to
		// be able to convert a logical day into a
		// physical one.
		//
    TaskList*	tcTaskList;

public:
    TaskCharge();

	//
	// Return information on the task.
	//
    inline const char* name() const;
    inline const char* reference() const;
    inline const char* notes() const;
    inline TaskList*   list() const;

	//
	// Set the forecast times.
	//
    inline void setForecasts(const long _f, const long _ft);
    
	//
	// Return True if this task must not be taken into account in sums.
	//
    inline bool skipSum() const;

	//
	// Comparison to detect when 2 TaskCharge objects refer to the
	// same task (but may be for different days).
	//
    inline bool operator ==(const TaskCharge& _t) const;
    inline bool operator ==(const char* _name) const;

	//
	// Access the time for a particular (logical) day.
	//
    inline time_t operator [](const unsigned _day) const;
    inline time_t& operator [](const unsigned _day);

	//
	// Access the time for a particular physical day.
	//
    time_t operator [](const Day& _day) const;
    time_t& operator [](const Day& _day);

	//
	// Merge two charge times.
	//
    TaskCharge& operator +=(const TaskCharge& _t);

	//
	// Return true if the object is the nil object.
	//
    bool isNil() const;

	//
	// Clear all the time values.
	//
    void clear();

	//
	// Setup a new reference for this task.
	//
    void setReference(const char* _ref);

	//
	// Setup a new note file for this task.
	//
    void setNoteFile(const char* _file);

	//
	// Format a string according to a model and using the
	// information known by the task object.
	//
    void format(char* _buf, const char* _model);

	//
	// Dump the content of this task to a file.
	//
    void print(FILE* _fp) const;

	//
	// Update the total forecast to the new time specified in `_t'.
	//
    void updateForecast(const long _t);
};

    inline const char*
TaskCharge::name() const
{
    return tcName;
}

    inline const char*
TaskCharge::reference() const
{
    return (tcReference == 0) ? "" : tcReference;
}

    inline const char*
TaskCharge::notes() const
{
    return (tcNotesFile == 0) ? "" : tcNotesFile;
}

    inline TaskList*
TaskCharge::list() const
{
    return tcTaskList;
}

    inline void
TaskCharge::setForecasts(const time_t _f, const time_t _ft)
{
    tcForecastTime      = _f;
    tcForecastTotalTime = _ft;
}

    inline bool
TaskCharge::skipSum() const
{
    return (tcReference == 0) ?
	false : strcmp(tcReference, EMPTY_TASK_REFERENCE) == 0;
}

    inline bool
TaskCharge::operator ==(const TaskCharge& _t) const
{
	//
	// Two tasks are identical if the references are identical
	// (if no reference, use the name).
	//
    if (tcReference && tcReference[0]
	&& _t.tcReference && _t.tcReference[0]) {
	return strcmp(tcReference, _t.tcReference) == 0 ? true : false;
    } else {
	return strcmp(_t.tcName, tcName) == 0 ? true : false;
    }
}

    inline bool
TaskCharge::operator ==(const char* _name) const
{
    return strcmp(tcName, _name) == 0 ? true : false;
}

    inline time_t
TaskCharge::operator [](const unsigned _day) const
{
    RequireMsg(_day <= NR_DAYS, ("Lday %u\n", _day));

    return tcTime[_day - 1];
}

    inline time_t&
TaskCharge::operator [](const unsigned _day)
{
    RequireMsg(_day <= NR_DAYS, ("Lday %u\n", _day));

    return tcTime[_day - 1];
}



//
// Time characteristics stored in the list.
//
enum TaskListMode {
    TaskMonthTime,		// Time in seconds spent for each task
    TaskMonthRemain,		// Remain time in seconds (0..3599)
    TaskMonthHours		// Time rounded down to the hour
};


//
// Mode when computing a sum of time.
//
enum TaskSumMode {
    SumDay,			// Current day
    SumWeek,			// Current week
    SumFortnight,		// Current fortnight
    SumMonth			// All month
};

//
// Mode when sorting the list of tasks.
//
enum TaskSortMode {
    SortName,			// Sort on the task names
    SortDayTime,		// Sort on the sum of the day
    SortWeekTime,		// Sort on the sum of the week
    SortFortnightTime,		// Sort on the sum of the fortnight
    SortMonthTime		// Sort on the sum of the month
};

//
// Mode when filtering the list of tasks.
//
enum TaskFilterMode {
    FilterNone,			// Don't filter
    FilterDayEmpty,		// Remove tasks with empty days
    FilterWeekEmpty,		// Remove tasks with empty weeks
    FilterFortnightEmpty,	// Remove tasks with empty fortnights
    FilterMonthEmpty		// Remove tasks with empty months
};


// ----------------------------------------------------------------------
// Class :	TaskList
//
// Role :	Manage a list of tasks
//
class TaskList : public WeekConversion {
	friend class TaskListCache;
protected:
    TaskCharge*	    tcList;	// List of tasks (in LRU order)
    unsigned short  tcNrTasks;	// Number of tasks
    unsigned short  tcMaxTasks;	// Maximum size of the list
    TaskListMode    tcMode;
    bool	    tcChanged;
    bool	    tcValid;
    short	    tcCurDay;	// Current day of month
    const char*	    tcFile;
    const char*     tcDir;

		//
		// Scratch and empty task object.
		//
    TaskCharge&	    tcEmptyTask;

	//
	// Check the date and mode associated with the task list.
	//
    int setMode(const unsigned _year, const unsigned _month,
		const TaskListMode _mode);

	//
	// Print all the tasks on `_fp'.
	//
    void print(FILE* _fp) const;

protected:
	//
	// Load the content of the list from a file.
	//
    int load();

public:
    TaskList();
    TaskList(const TaskList& _tl);
    virtual ~TaskList();

	//
	// Return the # of tasks.
	//
    inline unsigned size() const;

	//
	// Current day of month.
	//
    inline unsigned currentDay() const;

	//
	// Directory where the files are saved.
	//
    inline const char* directory() const;

	//
	// Mode to represent the time values.
	//
    inline TaskListMode mode() const;

	//
	// Invalidate the content of the list.
	// This is used to force the next `loadFile' to reinitialize
	// the list (from the cache).
	//
    inline void invalidate();

	//
	// Add a new task whose name is `_name'.
	//
    virtual TaskCharge& addTask(const char* _name);

	//
	// Delete the task whose name is `_name'.
	//
    virtual int delTask(const char* _name);

	//
	// Access to the TaskCharge.
	//
    TaskCharge& operator [](const char* _name);
    const TaskCharge& operator [](const char* _name) const;
    inline TaskCharge& operator [](const unsigned _which);

	//
	// Assignment (copy of lists to optimize and detect deleted
	// tasks or new tasks).
	//
    TaskList& operator =(const TaskList& _tl);

	//
	// Add two list of times.
	//
    TaskList& operator +=(const TaskList& _tl);

	//
	// Returns True if the task `_name' exists.
	//
    bool contains(const char* _name) const;

	//
	// Clear all the times.
	//
    void clear();

	//
	// Compute the sum of the time for a given task and in
	// a range of days.
	//
    unsigned long sum(DayIterator& _diter, const char* _name) const;

	//
	// Compute various sums of time for a given task.
	//
    unsigned long sum(const char* name, const TaskSumMode mode) const;

	//
	// Sort the list of tasks.
	//
    void sort(const TaskSortMode _mode);

	//
	// Filter the list of tasks to remove non-interesting entries
	// (entries with a null time value).
	//
    void filter(const TaskFilterMode _mode);

	//
	// Make sure that the current list contains the values for
	// the day of year `_year', `_day' in the format `_mode'.
	// Load the file if this is needed (from the cache).
	//
    int loadFile(const unsigned _year, const unsigned _day,
		 const TaskListMode _mode);

	//
	// Save the content of the list in a file (the name of the
	// file depends on the type of the list and the date it refers to).
	//
    int save();

    void forecast(const TaskList& _tl);
};

    inline unsigned
TaskList::size() const
{
    return tcNrTasks;
}

    inline unsigned
TaskList::currentDay() const
{
    return tcCurDay;
}

    inline const char*
TaskList::directory() const
{
    return tcDir;
}

    inline TaskListMode
TaskList::mode() const
{
    return tcMode;
}

    inline void
TaskList::invalidate()
{
    tcValid = false;
}

    inline TaskCharge&
TaskList::operator [](const unsigned _which)
{
    RequireMsg(_which < tcNrTasks, ("Task %u\n", _which));

    return tcList[_which];
}


// ----------------------------------------------------------------------
// Class :	TaskListCache
//
// Role :	Manage a cache of TaskList objects
//
class TaskListCache : private dlink, private TaskList {
private:
    TaskListCache(const TaskList& _tl);
protected:
    ~TaskListCache();
public:

	//
	// Set the directory where the task files must be located.
	//
    static void setDirectory(const char* _dir);

	//
	// Return that directory.
	//
    static const char*const getDirectory();

	//
	// Find in the cache the list of tasks which correspond to the month
	// `_month' of the year `_year'. If the cache does not contain such
	// list, it is read from the associated file.
	//
    static const TaskList* find(const unsigned _year, const unsigned _month,
			 	const TaskListMode _mode);

	//
	// Update the cache by inserting the list `_tl' in it.
	//
    static void update(const TaskList& _tl);
};

#endif
