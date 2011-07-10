// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Task management
// Author	: S. Carrez
// Date		: Sat Sep 23 11:32:17 1994
// Version	: $Id: Task.C,v 1.13 1996/08/04 15:36:00 gdraw Exp $
// Project	: Xcra
// Keywords	: Task, Time, Report
//
// Copyright (C) 1994, 1995, 1996, 2002 Stephane Carrez
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
// class TaskCharge	Manage the time for several days for a particular task 
// class TaskList	Manage a list of tasks
// class TaskListCache	Manage a small cache of TaskList objects
//
#include "config.H"

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>

#include "Error.H"
#include "Date.H"
#include "Task.H"

static TaskCharge	tcEmpty;

// ----------------------------------------------------------------------
//
//			TaskCharge
//
// ----------------------------------------------------------------------

//
// Create an empty task-charge object.
//
TaskCharge::TaskCharge()
{
    tcTaskList	= 0;
    tcName	= 0;
    tcNotesFile = 0;
    tcReference	= 0;
    tcForecastTime = 0;
    clear();
}


//
// Return the time which corresponds to the day `_day'.
//
    time_t
TaskCharge::operator [](const Day& _day) const
{
    const unsigned d = tcTaskList->WeekConversion::operator[](_day);

    if (d == 0) {
	return 0;
    }

    Ensure(d >= 1 && d <= NR_DAYS);
    return tcTime[d - 1];
}


//
// Return the time which corresponds to the day `_day'.
//
    time_t&
TaskCharge::operator [](const Day& _day)
{
    unsigned d = tcTaskList->WeekConversion::operator[](_day);

    if (d == 0) {
	static time_t tcScratch;

	return tcScratch;
    }

    Ensure(d >= 1 && d <= NR_DAYS);
    return tcTime[d - 1];
}


//
// Add the times reported in `_t' to the current task.
//
    TaskCharge&
TaskCharge::operator +=(const TaskCharge& _t)
{
	//
	// Be smart and merge only when the two tasks are identical
	// and merge only the same days (that is, iterates on the days)
	// Merge only time values which are compatible together. When
	// time types are not compatible, an appropriate conversion
	// is made.
	//
    if (_t == *this) {
	DayIterator day(*tcTaskList);

		//
		// Left value type.
		//
	const TaskListMode left = tcTaskList->mode();

		//
		// Right value type.
		//
	const TaskListMode right= _t.tcTaskList->mode();

	do {
	    Day     d  = day;
	    time_t& t0 = (*this)[d];
	    time_t  t1 = _t[d];

	    switch (left) {
	      case TaskMonthTime :
		switch (right) {
		  case TaskMonthTime :
			//
			// If t1 is < 0, we are substracting a time `t1'
			// to the current time. This must not become a
			// negative time.
			//
		    if ((long)(t1) < 0 && t0 < - (long)(t1)) {
			tcForecastTime -= t0;
			t0 = 0;
		    } else {
			tcForecastTime += t1;
			t0 += t1;
		    }
		    break;

			//
			// Merge the remain time of the previous day
			// to the next day. So, get the time of the
			// previous day.
			//
		  case TaskMonthRemain :
		    -- d;

		    t0 += _t[d];
		    break;

			//
			// Convert back hours into seconds and merge.
			//
		  case TaskMonthHours :
		    t0 += t1 * 3600;
		    break;
	        }
	        break;

	      case TaskMonthRemain :
		switch (right) {
		  case TaskMonthTime :
		    t0 += t1 % 3600;
		    break;

		  case TaskMonthRemain :
		    t0 += t1;
		    break;

		  case TaskMonthHours :
		    // Noop
		    break;
	        }
	        break;

	      case TaskMonthHours :
		switch (right) {
		  case TaskMonthTime :
		    t0 += t1 / 3600;
		    break;

		  case TaskMonthRemain :
		    // Noop
		    break;

		  case TaskMonthHours :
		    t0 += t1;
		    break;
	        }
	        break;
	    }
	} while (day.next());
    }
    return *this;
}


//
// Return true if the task-charge object is the nil object (does not reference
// an existing task).
//
    bool
TaskCharge::isNil() const
{
    return (this == &tcEmpty) ? true : false;
}


//
// Clear all the times which are recorded.
//
    void
TaskCharge::clear()
{
    tcForecastTotalTime = 0;
    for (unsigned i = 0; i < NR_DAYS; i++) {
	tcTime[i] = 0;
    }
}


//
// Set the reference associated with the task.
//
    void
TaskCharge::setReference(const char* _ref)
{
    Require(_ref != 0);
    RequireMsg(strlen(_ref) < MAX_REF_LEN, ("Ref too long %s", _ref));

    if (tcReference) {
	free((free_ptr) tcReference);
    }
    tcReference = strdup(_ref);
}


//
// Set the notes file associated with the task.
//
    void
TaskCharge::setNoteFile(const char* _file)
{
    Require(_file != 0);
    RequireMsg(strlen(_file) < MAX_NOTE_LEN, ("Note file too long %s", _file));

    if (tcNotesFile) {
	free((free_ptr) tcNotesFile);
    }
    tcNotesFile = strdup(_file);
}


//
// Format the string `_model' in the buffer `_buf' and using the
// information stored in the task-charge object.
//
// Example of formats:
// -------------------
//
// %2tH:%02tM:%02tS	3:45:23		Hours/Minutes/Seconds for current task
// %2aH:%02aM:%02aS	23:44:34	Idem for the month (total)
// %2wH:%02wM		8:44		Idem for the week
// %2fH:%02fM		16:44		Idem for the fortnight
// %3.3tfp%%		12.4%		Percentage of current day over
//					the fortnight sum for this task.
// %3.3tFp%%		3.3%		Percentage of current day over
//					the fortnight sum for all tasks.
// %2tH			3		Hours only
// %2tM:%02tS		45:23		Remain time only
// %-20.20T %-20.20R			Task Name + Task tcReference
//
    void
TaskCharge::format(char* _buf, const char* _model)
{
    char fmt[10];
    char c;
    int  i;
    unsigned  vindex;
    long value[4];
    long v, hrs, mins;
    TaskSumMode mode = SumWeek;

    const char* model = _model;
    while ((c = *model++) != 0) {
	if (c == '%') {
	    fmt[0] = c;
	    i = 1;
	    *_buf = 0;
	    vindex = 0;
	    while ((c = *model++) != 0) {
		if (i == sizeof(fmt)) {
		    Error::printf("Format string is too long: `%s'\n", _model);
		    break;
		}
		if (vindex >= sizeof(value) / sizeof(long)) {
		    Error::printf("Too many values specified: `%s'\n", _model);
		    break;
		}

		switch (c) {
			//
			// Print hours.
			//
		    case 'H' :
			if (vindex == 0) {
			    Error::printf("No value specified `%s'\n", _model);
			}
			value[0] = (value[0] / 3600);
			fmt[i] = 'u'; fmt[i+1] = 0;
			sprintf(_buf, fmt, value[0]);
			break;

			//
			// Print minutes.
			//
		    case 'M' :
			if (vindex == 0) {
			    Error::printf("No value specified `%s'\n", _model);
			}
			value[0] = (value[0] % 3600) / 60;
			fmt[i] = 'u'; fmt[i+1] = 0;
			sprintf(_buf, fmt, value[0]);
			break;

			//
			// Print seconds.
			//
		    case 'S' :
			if (vindex == 0) {
			    Error::printf("No value specified `%s'\n", _model);
			}
			value[0] = (value[0] % 60);
			fmt[i] = 'u'; fmt[i+1] = 0;
			sprintf(_buf, fmt, value[0]);
			break;

			//
			// Print percentage of 2 values.
			//
		    case 'p' :
			if (vindex != 2) {
			    Error::printf("Missing values in model `%s'\n",
					   _model);
			    sprintf(_buf, "-miss-value-");
			} else if (value[1] == 0) {
			    Error::printf("Division by 0\n");
			    sprintf(_buf, "-infinity-");
			} else {
			    double d = (double)(value[0] * 100);
			    d = d / (double)(value[1]);

			    fmt[i] = 'f'; fmt[i+1] = 0;
			    sprintf(_buf, fmt, d);
			}
			break;

			//
			// Print task name.
			//
		    case 'N' :
			fmt[i] = 's'; fmt[i+1] = 0;
			sprintf(_buf, fmt, name());
			break;

			//
			// Print task reference.
			//
		    case 'R' :
			fmt[i] = 's'; fmt[i+1] = 0;
			sprintf(_buf, fmt, reference());
			break;

			//
			// Time for this task (current day).
			//
		    case 't' :
			value[vindex++] = tcTime[tcTaskList->currentDay()-1];
			continue;

			//
			// Time for all the tasks (current day).
			//
		    case 'T' :
			mode = SumDay;
			value[vindex++] = tcTaskList->sum(0, mode);
			continue;

			//
			// Sum for this task and current week.
			//
		    case 'w' :
			mode = SumWeek;
			value[vindex++] = tcTaskList->sum(tcName, mode);
			continue;

			//
			// Sum for this task and current fortnight.
			//
		    case 'f' :
			mode = SumFortnight;
			value[vindex++] = tcTaskList->sum(tcName, mode);
			continue;

			//
			// Sum for this task and current month.
			//
		    case 'a' :
			mode = SumMonth;
			value[vindex++] = tcTaskList->sum(tcName, mode);
			continue;

		    case 'W' :
			mode = SumWeek;
			value[vindex++] = tcTaskList->sum(0, mode);
			continue;

		    case 'F' :
			mode = SumFortnight;
			value[vindex++] = tcTaskList->sum(0, mode);
			continue;

		    case 'A' :
			mode = SumMonth;
			value[vindex++] = tcTaskList->sum(0, mode);
			continue;

		    case 'd' :
			value[0] = tcTaskList->currentDay();
			fmt[i] = 'd'; fmt[i+1] = 0;
			sprintf(_buf, fmt, value[0]);
			break;

		    case 'm' :
			value[0] = tcTaskList->month();
			fmt[i] = 'd'; fmt[i+1] = 0;
			sprintf(_buf, fmt, value[0]);
			break;

		    case 'y' :
			value[0] = tcTaskList->year();
			fmt[i] = 'd'; fmt[i+1] = 0;
			sprintf(_buf, fmt, value[0]);
			break;

		    case 'D' :
			sprintf(_buf, "%s", tcTaskList->directory());
			break;

		    case 'P' :
			if (tcForecastTime < 0) {
			    v = - tcForecastTime;
			} else {
			    v = tcForecastTime;
			}
			mins = v / 60;
			hrs  = mins / 60;
			mins = mins % 60;
			sprintf(_buf, ((tcForecastTime < 0)
					 ? "-%ld:%02ld" : "%ld:%02ld"),
				       hrs, mins);
			break;

		    case '%' :
			*_buf = c;
			_buf[1] = 0;
			break;

		    default :
			fmt[i++] = c;
			continue;
		}
		break;
	    }
	    _buf = &_buf[strlen(_buf)];
	    if (c == 0)
		break;
	} else {
	    *_buf++ = c;
	}
    }
    *_buf = 0;
}


//
// Print the information on the task in `_fp'.
//
    void
TaskCharge::print(FILE* _fp) const
{
    fprintf(_fp, "T=%s\nR=%s\nN=%s\nF=%ld %ld\nC= ",
	    name(), reference(), notes(), tcForecastTime,
	    tcForecastTotalTime);

    for (unsigned j = 0; j < NR_DAYS; j++ ) {
	time_t t = tcTime[j];

	fprintf(_fp, "%lu ", t);
    }
    fprintf(_fp, "\n");
}


    void
TaskCharge::updateForecast(const long _t)
{
    tcForecastTotalTime += _t - tcForecastTime;
    tcForecastTime      = _t;
}

// ----------------------------------------------------------------------
//
//			TaskList
//
// ----------------------------------------------------------------------

//
// Create a list of tasks.
//
TaskList::TaskList()
: tcEmptyTask(tcEmpty)
{
    tcMaxTasks  = MAX_TASKS;
    tcNrTasks	= 0;
    tcFile	= 0;
    tcDir	= 0;
    tcMode	= TaskMonthTime;
    tcChanged	= false;
    tcCurDay	= 0;
    tcValid     = false;

    tcList	= new TaskCharge[tcMaxTasks];
}


//
// Copy constructor for a list of tasks.
//
TaskList::TaskList(const TaskList& _tl)
: WeekConversion(_tl), tcEmptyTask(tcEmpty)
{
    tcList	= new TaskCharge[_tl.tcMaxTasks];
    tcNrTasks	= 0;
    tcMaxTasks	= MAX_TASKS;
    tcFile	= 0;
    tcDir	= 0;
    tcCurDay	= 0;
    tcValid     = false;

    *this	= _tl;
}


//
// Delete the list of tasks.
//
TaskList::~TaskList()
{
	//
	// Delete each task, freeing the memory.
	//
    while (tcNrTasks != 0) {
	TaskList::delTask(tcList[tcNrTasks-1].name());
    }

    if (tcFile) {
	free((free_ptr) tcFile);
    }

    delete []tcList;
}


//
// Create a new task whose name is `_name'.
//
    TaskCharge&
TaskList::addTask(const char* _name)
{
    Require(_name != 0);
    RequireMsg(strlen(_name) < MAX_NAME_LEN, ("Name too long %s", _name));

    if (tcNrTasks == tcMaxTasks) {
	Error::printf(MSG_TASKLIST_FULL);
	return tcEmpty;
    }

    tcChanged = true;

    TaskCharge& task = tcList[tcNrTasks++];

    task.tcTaskList  = this;
    task.tcName      = strdup(_name);
    task.tcReference = 0;
    task.tcNotesFile = 0;
    task.tcForecastTime = 0;
    task.clear();

    return task;
}


//
// Delete the task whose name is `_name'.
//
    int
TaskList::delTask(const char* _name)
{

    if (contains(_name) == 0) {
	Error::printf(MSG_NOTASK, _name);
	return -1;
    }

    TaskCharge& task = operator [](_name);
    if (task.tcName) {
	free((free_ptr) task.tcName);
    }
    if (task.tcReference) {
	free((free_ptr) task.tcReference);
    }
    if (task.tcNotesFile) {
	free((free_ptr) task.tcNotesFile);
    }

    size_t sz = (char*)(&tcList[--tcNrTasks]) - (char*)(&task);
    if (sz != 0) {
	memmove((char*)(&task), (char*)(&task) + sizeof(TaskCharge), sz);
    }
    return 0;
}


//
// Return the task whose name is `_name'. If the task does not exist,
// create a new task with that name.
//
    TaskCharge&
TaskList::operator [](const char* _name)
{
    for (unsigned i = 0; i < tcNrTasks; i++) {
	if (tcList[i] == _name)
	    return tcList[i];
    }

    return addTask(_name);
}


//
// Return the task whose name is `_name' but do not create a new
// task if it does not exist.
//
    const TaskCharge&
TaskList::operator [](const char* _name) const
{
    for (unsigned i = 0; i < tcNrTasks; i++) {
	if (tcList[i] == _name)
	    return tcList[i];
    }

    return tcEmptyTask;
}


//
// Copy a list of tasks in another.
//
    TaskList&
TaskList::operator =(const TaskList& _tl)
{
    if (tcFile) {
	free((free_ptr) tcFile);
    }

	//
	// Set mode so that we are compatible with `_tl' (and += will work).
	//
    tcMode    = _tl.tcMode;
    tcChanged = _tl.tcChanged;
    if (_tl.tcCurDay) {
        tcCurDay  = _tl.tcCurDay;
    }
    tcFile    = strdup(_tl.tcFile);
    tcDir     = _tl.tcDir;
    tcValid   = _tl.tcValid;

    setMonth(_tl.year(), _tl.month());

	//
	// Must delete each task not present in `_tl'. The effect is
	// that we only call the virtual `delTask', only when we
	// really need to delete the task.
	//
    int i;
    for (i = tcNrTasks; --i >= 0; ) {
	if (_tl.contains(tcList[i].name())) {
	    tcList[i].clear();

	    tcList[i] += _tl[tcList[i].name()];
	    continue;
	}

	delTask(tcList[i].name());
    }

	//
	// Then add each new task from `_tl' which is not in our list.
	// The effect is that we only call the virtual `addTask' when
	// needed.
	//
    for (i = 0; i < _tl.tcNrTasks; i++) {
	if (contains(_tl.tcList[i].name()))
	    continue;

	TaskCharge& newTask = addTask(_tl.tcList[i].name());

	if (_tl.tcList[i].tcReference)
	    newTask.setReference(_tl.tcList[i].tcReference);

	if (_tl.tcList[i].tcNotesFile)
	    newTask.setNoteFile(_tl.tcList[i].tcNotesFile);

	newTask += _tl.tcList[i];
    }
    return *this;
}


//
// Add the times in `_tl' to the current list.
//
    TaskList&
TaskList::operator +=(const TaskList& _tl)
{
    tcChanged = true;
    for (unsigned i = 0; i < _tl.tcNrTasks; i++) {
	TaskCharge  task1(_tl.tcList[i]);

	TaskCharge& task2 = operator [](task1.name());

	task2 += task1;
    }
    return *this;
}


//
// Return true if the task `_name' exists in the list.
//
    bool
TaskList::contains(const char* _name) const
{
    for (unsigned i = 0; i < tcNrTasks; i++) {
	if (tcList[i] == _name)
	    return true;
    }
    return false;
}


//
// Clear all the time values for all the tasks.
//
    void
TaskList::clear()
{
    tcChanged = true;
    for (unsigned i = 0; i < tcNrTasks; i++) {
	tcList[i].clear();
    }
}


//
// Compute the sum of the times for the task `_name' and according
// to `_mode'.
//
    unsigned long
TaskList::sum(const char* _name, const TaskSumMode _mode) const
{
    WeekConversion	week;

	//
	// Defines the range of days for the iterator.
	//
    switch (_mode) {
	case SumDay : {
		    Day d(Date::dayOfYear(year(), month(), tcCurDay), year());

		    DayIterator iter(d, d);

		    return sum(iter, 0);
		}

	case SumWeek :
		week.setWeek(*this, tcCurDay);
		break;

	case SumFortnight :
		week.setFortnight(*this, tcCurDay);
		break;

	case SumMonth :
		week.setMonth(*this);
		break;
    }

    DayIterator		diter(week);
    return sum(diter, _name);
}


//
// Compute the sum of the times for the task `_name' and using the day
// iterator `_diter'.
//
    unsigned long
TaskList::sum(DayIterator& _diter, const char* _name) const
{
    unsigned long total = 0;

    const TaskList* tk = this;
    const Day& day = _diter;
    do {

		//
		// Check whether we have the correct file.
		//
	unsigned nmonth = Date::monthOf(day.year(), day.day());
	if (tk->month() != nmonth || tk->year() != day.year()) {
	    const TaskList* newList;

		//
		// Get it from the cache.
		//
	    newList = TaskListCache::find(day.year(), nmonth, mode());
	    if (newList == 0)
		continue;

	    tk = newList;
	}

		//
		// Compute the sum.
		//
	if (_name == 0) {
	    for (unsigned i = 0; i < tk->tcNrTasks; i++) {
		if (tk->tcList[i].skipSum())
		    continue;

		total += tk->tcList[i][day];
	    }
	} else {
	    const TaskCharge& task = (*tk)[_name];

	    if (task.isNil() == 0)
		total += task[day];
	}
    } while (_diter.next());
    return total;
}


//
// Comparison operation between two tasks.
//
static TaskSortMode cmpMode;

    static int
TaskCmp(const void *_e1, const void* _e2)
{
    const TaskCharge& c1 = *(const TaskCharge*) _e1;
    const TaskCharge& c2 = *(const TaskCharge*) _e2;
    time_t t1;
    time_t t2;

    switch (cmpMode) {
	case SortName :
		return strcmp(c1.name(), c2.name());

	case SortDayTime :
		t1 = c1[c1.list()->currentDay()];
		t2 = c2[c1.list()->currentDay()];
		break;

	case SortWeekTime :
		t1 = c1.list()->sum(c1.name(), SumWeek);
		t2 = c1.list()->sum(c2.name(), SumWeek);
		break;

	case SortFortnightTime :
		t1 = c1.list()->sum(c1.name(), SumFortnight);
		t2 = c1.list()->sum(c2.name(), SumFortnight);
		break;

	case SortMonthTime :
		t1 = c1.list()->sum(c1.name(), SumMonth);
		t2 = c1.list()->sum(c2.name(), SumMonth);
		break;

	default:
		t1 = 0;	// Avoid gcc warning
		t2 = 0;
		break;
    }

	//
	// Times are equal, sort on the names
	//
    if (t1 == t2) {
	return strcmp(c1.name(), c2.name());

	//
	// Put the bigger time values first
	//
    } else if (t1 < t2) {
	return 1;
    } else {
	return -1;
    }
}


//
// Sort the list of tasks according to `_mode'.
//
    void
TaskList::sort(const TaskSortMode _mode)
{
    if (tcNrTasks > 1) {
	cmpMode = _mode;
	qsort(tcList, tcNrTasks, sizeof(TaskCharge), TaskCmp);
    }
}


//
// Filter the list of tasks according to `_mode'. Tasks which do not
// match the criteria `_mode' are removed: all the tasks which have
// a null reported time for the day/week/fortnight/month are removed.
//
    void
TaskList::filter(const TaskFilterMode _mode)
{
    if (_mode == FilterNone) {
	return;
    }

    for (int i = tcNrTasks; --i > 0; ) {
	TaskCharge* t = &tcList[i];
	time_t t0;

	switch (_mode) {
	    case FilterDayEmpty :
		t0 = (*t)[tcCurDay];
		break;

	    case FilterWeekEmpty :
		t0 = sum(t->name(), SumWeek);
		break;

	    case FilterFortnightEmpty :
		t0 = sum(t->name(), SumFortnight);
		break;

	    case FilterMonthEmpty :
		t0 = sum(t->name(), SumMonth);
		break;

	    default:
		continue;	// Avoid gcc warning t0 not initialized
	}
	if (t0 != 0)
	    continue;

	    //
	    // Remove all the tasks with a null time.
	    //
	delTask(t->name());
    }
}


//
// Load the task list and time values which correspond to the
// day `_day' of the year `_year'. `_day' starts from 1 up to 366.
// The list of tasks is get from the cache.
//
    int
TaskList::loadFile(const unsigned _year, const unsigned _day,
		   const TaskListMode _mode)
{
    unsigned	nmonth;

    nmonth   = Date::monthOf(_year, _day);
    tcCurDay = Date::monthDayOf(_year, _day);

    const unsigned oldYear     = year();
    const unsigned oldMonth    = month();
    const TaskListMode oldMode = tcMode;

    if (setMode(_year, nmonth, _mode) == 0) {
	return 0;
    }

	//
	// Look in the cache if we have already loaded this file.
	//
    const TaskList* tl = TaskListCache::find(_year, nmonth, _mode);
    if (tl != 0) {
	*this = *tl;
	return 0;
    }

	//
	// The file does not exist, backtrack and restore what we can.
	//
    if (oldYear != 0) {
	(void) setMode(oldYear, oldMonth, oldMode);
    }

    return E_OPEN_ERROR;
}


#ifndef O_SYNC
#  define O_SYNC	0
#endif


//
// Save the task list is its associated file.
//
    int
TaskList::save()
{
    char	tempName[1024];
    FILE*	fp;
    int		fd;

    MailErrorHandler info;

    sprintf(tempName, "%s%%", tcFile);

    sigset_t oset;
    sigset_t set;
    
    sigfillset(&set);
    sigprocmask(SIG_BLOCK, &set, &oset);

	//
	// Open the file for writing with synchronous (no cache) flag.
	//
    fd = open(tempName, O_WRONLY | O_CREAT | O_SYNC, 0600);
    if (fd < 0) {
	Error::report(MSG_CANT_CREATE, tempName);
	sigprocmask(SIG_SETMASK, &oset, 0);
	return E_OPEN_ERROR;
    }
    
    fp = fdopen(fd, "w");
    if (fp == 0) {
	Error::printf(MSG_CANT_REOPEN, tempName);
	sigprocmask(SIG_SETMASK, &oset, 0);
	return E_OPEN_ERROR;
    }

    print(fp);

    int res = fclose(fp);

    if (res == 0) {
        res = rename(tempName, tcFile);
	if (res == 0) {
	    TaskListCache::update(*this);
	    tcChanged = false;
	} else {
	    Error::printf(MSG_RENAME_FAILED, tempName, tcFile);
	    Error::printDiagnostic(tcFile);
	}
    }

    if (res != 0) {
	int oerrno = errno;
	(void) unlink(tempName);
	errno = oerrno;
    }
    
    sigprocmask(SIG_SETMASK, &oset, 0);

    if (res != 0) {
	Error::report(MSG_SAVE_ERROR, tcFile);
	return E_WRITE_ERROR;
    } else {
	return 0;
    }
}


//
// Check the mode and date reference associated with the task list.
//
    int
TaskList::setMode(const unsigned _year, const unsigned _month,
		  const TaskListMode _mode)
{
    RequireMsg(_month != 0 && _month <= 12, ("Month %u\n", _month));
    RequireMsg(_year >= 1970, ("Year %u\n", _year));

	//
	// Check whether we are in the correct mode.
	//
    if (year() == _year && month() == _month && tcMode == _mode) {
	return tcValid == false ? 1 : 0;
    }

    char bname[12];

	//
	// Build the file name for this new mode
	//
    switch (_mode) {
	case TaskMonthTime :
	    sprintf(bname, "t-%04u-%02u", _year, _month);
	    break;

	case TaskMonthRemain:
	    sprintf(bname, "r-%04u-%02u", _year, _month);
	    break;

        case TaskMonthHours:
	    sprintf(bname, "h-%04u-%02u", _year, _month);
	    break;
    }
    if (tcFile) {
	free((free_ptr)tcFile);
    }

    Require(tcDir != 0);

    char* p = (char*)malloc(sizeof(bname) + strlen(tcDir) + 2);

    sprintf(p, "%s/%s", tcDir, bname);

    tcFile = p;
    tcMode = _mode;
    tcChanged = true;

    setMonth(_year, _month);

    return 1;
}


//
// Print the list of tasks on `_fp'.
//
    void
TaskList::print(FILE* _fp) const
{
    fprintf(_fp, "# Task v2.2 Format 4.0\n");
    fprintf(_fp, "# %d tasks\n", tcNrTasks);

    for (unsigned i = 0; i < tcNrTasks; i++ ){
	tcList[i].print(_fp);
    }
}


//
// Load the task list and times from the file.
//
    int
TaskList::load()
{
    FILE*       fp;

    fp = fopen(tcFile, "r");
    if (fp == 0) {
	return E_OPEN_ERROR;
    }

	//
	// Since loading a file may merge the tasks (due to operator []),
	// be sure all the times are cleared.
	//
    clear();

    int errors = 0;
    int line = 1;

    char name[MAX_NAME_LEN];
    char ref[MAX_REF_LEN];
    char noteFile[MAX_NOTE_LEN];
    long forecast = 0;
    long forecastTotal = 0;

    name[0]     = 0;
    ref[0]      = 0;
    noteFile[0] = 0;

    while (1) {
	int res;
	char c;

	c = fgetc(fp);
	if (c == '\n') {
	    line ++;
	    continue;
	}

	if (feof(fp))
	    break;

	    //
	    // Comments are only present in version 2.0 files.
	    //
	if (c == '#') {
	    do {
		c = fgetc(fp);
	    } while (!feof(fp) && c != '\n');
	    line ++;
	    continue;
	}

	    //
	    // Format:
	    //
	    // T=<name>
	    // R=<name>
	    // N=<name>
	    // F=<t1> <t2>
	    // C=<day-1> <day-2> .. <day-31>
	    //
	char* p;

	if (fgetc(fp) != '=') {
	    Error::printf(MSG_MISSING_EQUAL, tcFile, line);
	    errors ++;
	    break;
	}
	if (c == 'R') {
	    p = fgets(ref, sizeof(ref) - 1, fp);
	    if (p == (char*) NULL) {
		Error::printf(MSG_BAD_REFERENCE, tcFile, line);
		errors ++;
		break;
	    }
	    p[strlen(p) - 1] = 0;
	    continue;

	} else if (c == 'T') {
	    p = fgets(name, sizeof(name) - 1, fp);
	    if (p == (char*) NULL) {
		Error::printf(MSG_BAD_NAME, tcFile, line);
		errors++;
		break;
	    }
	    p[strlen(p) - 1] = 0;
	    continue;

	} else if (c == 'N') {
	    p = fgets(noteFile, sizeof(noteFile) - 1, fp);
	    if (p == (char*) NULL) {
		Error::printf(MSG_BAD_NOTE, tcFile, line);
		errors++;
		break;
	    }
	    p[strlen(p) - 1] = 0;
	    continue;

	} else if (c == 'F') {
	    res = fscanf(fp, "%ld %ld ", &forecast, &forecastTotal);
	    if (res != 2) {
		Error::printf(MSG_BAD_NOTE, tcFile, line);
		errors++;
		break;
	    }
	    continue;

	} else if (c != 'C') {
	    Error::printf(MSG_UNKNOWN_DIRECTIVE, tcFile, line, c);
	    Error::printf(MSG_SKIP_ENDLINE, tcFile, line);
	    errors ++;
	    while (!feof(fp) && c != '\n') {
		c = fgetc(fp);
	    }
	    line ++;
	    continue;
	}

	TaskCharge& task = operator [](name);
	if (task.isNil()) {
	    Error::printf(MSG_CANT_ADD_TASK, tcFile, line);
	    errors ++;
	    break;
	}
	task.setReference(ref);
	task.setNoteFile(noteFile);
	task.setForecasts(forecast, forecastTotal);

	int j;
	for (j = 1; j <= NR_DAYS; j++ ) {
	    time_t t;
	    res = fscanf(fp, "%ld ", &t);
	    task[j] = t;
	    if (res != 1) break;
	}
	if (j != NR_DAYS + 1) {
	    Error::printf(MSG_BAD_TIME_FORMAT, tcFile, line);
	    errors ++;
	    break;
	}

	name[0]	      = 0;
	ref[0]	      = 0;
	noteFile[0]   = 0;
	forecast      = 0;
	forecastTotal = 0;
    }
    fclose(fp);

    if (errors) {
	Error::printf(MSG_LOADERROR, tcFile);
        return E_LOAD_ERROR;
    } else {
	tcChanged = false;
	tcValid   = true;
        return 0;
    }
}


//
// Print the list of tasks on `_fp'.
//
    void
TaskList::forecast(const TaskList& _tl)
{
    for (unsigned i = 0; i < tcNrTasks; i++) {
	TaskCharge& task = tcList[i];

	if (_tl.contains(task.name())) {
	    const TaskCharge& otask = _tl[task.name()];

	    task.tcForecastTotalTime = otask.tcForecastTotalTime;
	    task.tcForecastTime      = otask.tcForecastTime;
	    task.tcForecastTime     -= _tl.sum(task.name(), SumMonth);
	}
    }
}


// ----------------------------------------------------------------------
//
//			TaskListCache
//
// ----------------------------------------------------------------------

static dlist tcache;
static char* tcDirectory;

//
// Create a cache entry holding a list of tasks associated with a file.
//
TaskListCache::TaskListCache(const TaskList& _tl)
: TaskList(_tl)
{
    tcache.insert(this);
}


//
// Delete a cache entry.
//
TaskListCache::~TaskListCache()
{
    tcache.remove(this);
}


//
// Set the directory where the task files must be located.
//
    void
TaskListCache::setDirectory(const char* _dir)
{
   tcDirectory = strdup(_dir);
}

//
// Get the directory where the task files must be located.
//
    const char*const
TaskListCache::getDirectory()
{
   return tcDirectory;
}


//
// Find in the cache the list of tasks which correspond to the month
// `_month' of the year `_year'. If the cache does not contain such
// list, it is read from the associated file.
//
    const TaskList*
TaskListCache::find(const unsigned _year, const unsigned _month,
		    const TaskListMode _mode)
{
    TaskListCache* tc;

	//
	// Look if we find an entry with the corresponding name.
	//
    tc = (TaskListCache*) tcache.first();
    while (tc != 0) {
	if (tc->year() == _year && tc->month() == _month
	    && tc->mode() == _mode) {

	    tcache.raise(tc);
	    return tc;
	}

	tc = (TaskListCache*) tc->next();
    }

	//
	// Create a new cache entry and load the file.
	//
    TaskList tl;

    tl.tcDir = tcDirectory;
    tl.setMode(_year, _month, _mode);

    int result = tl.load();
    if (result != 0) {
	return 0;
    }

    tc = new TaskListCache(tl);
    if (tc == 0) {
	return 0;
    }

    return tc;
}


//
// Update the cache by inserting the list `_tl' in it.
//
    void
TaskListCache::update(const TaskList& _tl)
{
    TaskListCache* tc;

	//
	// Find the cache entry and remove it.
	//
    tc = (TaskListCache*) tcache.first();
    while (tc != 0) {
	if (tc->year() == _tl.year() && tc->month() == _tl.month()
	    && tc->mode() == _tl.mode()) {

	    delete tc;
	    break;
	}

	tc = (TaskListCache*) tc->next();
    }

    new TaskListCache(_tl);
}
