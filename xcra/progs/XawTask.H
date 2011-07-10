// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Interface for the Task Tool
// Author	: S. Carrez
// Date		: Sat Oct  1 13:37:28 1994
// Version	: $Id: XawTask.H,v 1.12 1996/08/04 15:37:48 gdraw Exp $
// Project	: Xcra
// Keywords	: Task, Xaw, Interface
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
// class XawTaskControl		Top level dialog box (main dialog & monitor).
//
// Dialog boxes:
// -------------
//   class XawTaskErrorDialog	Error message dialog box.
//
// Popup menu:
// ----------
//   class XawTaskMenuItem	Item of a popup menu.
//
//
#ifndef _XAW_TASK_H_
#define	_XAW_TASK_H_


#include "Error.H"
#include "XawDialog.H"
#include "XawTaskList.H"
#include "XawTaskMgr.H"
#include "Note.H"

#ifdef HAVE_XTOOLS
#  include "XawClock.H"
#  include "XawEyes.H"
#  include "XawBiff.H"
#endif


//
// Identifier values for the dialog box.
//
enum DialogType {
    XTASK_ADD,		// Add a new task
    XTASK_REMOVE,	// Remove a task
    XTIME_ADD_FROM,	// Move a time value from a task to another
    XTIME_SUB_FROM,	// Move a time value from a task to another
    XTASK_ADDTIME,	// Add/Substract a time value to several tasks
    XTASK_SWITCH_TO,	// Substract a time from the current task to
			// another task and then switch to it.
    XTASK_LIST,		// List the available tasks
    XTASK_ERROR,	// Error dialog box
    XTASK_REFERENCE,	// List the tasks to edit their references
    XTASK_FORECASTS,	// List the tasks to edit the forecasts
    XTASK_REPORT,	// Report the time values

    XNOTE_ADD,		// Add a new note entry
    XNOTE_REMOVE,	// Remove a note entry
    XNOTE_CHOOSE,	// Choose and edit a note entry
    XNOTE_CHANGE,	// Change the current note file
    XNOTE_TREE,		// Show the complete note tree
    XNOTE_GREP,		// Grep in all the notes

    XTASK_PROJECT = 0x1000,
    XNOTE_SELECT  = 0x2000
};

class XawTaskControl;

// ----------------------------------------------------------------------
// Class :	XawTaskMenuItem
//
// Role :	Defines an item of a popup menu.
//
class XawTaskMenuItem : public XawMenuItem {
protected:
    XawTaskControl&	xTask;
    DialogType		xDialogKind;
public:
	XawTaskMenuItem(const char* _name, XawMenu& _menu,
			XawTaskControl& _xtask, const DialogType _which);
	~XawTaskMenuItem();

	//
	// Execute the action associated with the menu item.
	// That action is determined by `xDialogKind'.
	//
	void
    activate();
};


// ----------------------------------------------------------------------
// Class :	XawTaskErrorDialog
//
// Role :	Error dialog box.
//
class XawTaskErrorDialog : public XawErrorDialog {
protected:
    XawTaskControl&	xTask;
public:
    XawTaskErrorDialog(const char* _name, XawTaskControl& _xtask);
    ~XawTaskErrorDialog();
};


// ----------------------------------------------------------------------
// Class :	XawTaskControl
//
// Role :	Main application.
//
class XawTaskControl : public TaskControl,
		       public XawDialog,
		       public ErrorHandler {
	friend class XawTaskErrorDialog;
protected:
	//
	// One popup menu for the task manager.
	//
    XawMenu		xtcTaskMenu;
	XawTaskMenuItem	xtcAddTask;
	XawTaskMenuItem xtcDelTask;
	XawTaskMenuItem	xtcRefTask;
	XawTaskMenuItem	xtcForTask;
	XawTaskMenuItem	xtcSelTask;
	XawTaskMenuItem	xtcSwiTask;
	XawMenuSep	xtcSeparator;

	//
	// One popup menu for the management of the time associated with tasks.
	//
    XawMenu		xtcTimesMenu;
	XawTaskMenuItem	xtcAddTime;
	XawTaskMenuItem	xtcRepTime;
	XawTaskMenuItem	xtcIncTime;
	XawTaskMenuItem	xtcDecTime;

	//
	// One popup menu for the notes manager.
	//
    XawMenu		xtcNotesMenu;
	XawTaskMenuItem xtcAddNote;
	XawTaskMenuItem xtcDelNote;
	XawTaskMenuItem xtcSelNote;
	XawTaskMenuItem xtcChgNote;
//	XawTaskMenuItem xtcTreeNote;
	XawTaskMenuItem xtcSearchNote;
	XawMenuSep	xtcSeparatorNote;

#ifdef HAVE_XTOOLS
    XawClock*		xtcClock;
    XawEyes*		xtcEyes;
    XawBiff*		xtcBiff;
#endif

    XawTaskErrorDialog*	xtcErrorDialog;

    int			xtcMenuSize;
    int			xtcNoteMenuSize;
    int			xtcUpdateTime;

		//
		// Label which is currently displayed in the task button.
		//
    char*		xtcTaskLabel;

		//
		// Label which is currently displayed in the times button.
		//
    char*		xtcTimeLabel;

		//
		// Label associated with the icon.
		//
    char*		xtcIconName;

		//
		// Timer used to be notified when the task time must be
		// incremented (each minute in most cases).
		//
    XtIntervalId	xtcTimerId;

		//
		// Work proc to do some background job.
		//
    XtWorkProcId	xtcWorkProc;

		//
		// The current list of notes associated with the active
		// task.
		//
    NoteList*		xtcNotesList;

	void
    updateProjectMenu();

	void
    updateNoteMenu();

	TaskCharge&
    addTask(const char* _name);

public:
    XawTaskControl(int _argc, char* _argv[]);
    ~XawTaskControl();

	//
	// Return the notes which must be used according to the active task.
	//
	inline NoteList&
    currentNotes() const;

	//
	// Delete the task whose name is `_name'.
	//
    int delTask(const char* _name);

	//
	// Move the time value specified in `_value' to/from the task
	// whose name is `_name'.
	//
    int moveTime(const char* _name, time_t _value, int _sign);

	//
	// Update the time for this task (update the labels).
	//
    int updateTime();

	//
	// Load the file (update the labels).
	//
    int load();

	//
	// Charge the task whose name is `_name'.
	//
    int charge(const char* _name);

	//
	// Change the active task to the `_which' th (in the LRU).
	//
    void charge(int _which);

	//
	// Catch cancel on the window to make sure the user does not
	// exit accidentally.
	//
    void cancel();
    
	//
	// Report an error message in a dialog box.
	//
    void report(const char* _msg);

	//
	// Timer callback executed each minute to update the task time.
	// The timer is restarted each time this operation is called.
	//
    void timer();

	//
	// Setup the new labels when the time/task changes.
	//
    void setup();

	//
	// Change the notes file associated to the current task.
	//
    int setNotesFile(const char* _file);

	//
	// The notes file identified by `_notes' was changed. Check
	// if the note menu must be updated.
	//
	void
    mayBeUpdateNoteMenu(NoteList* _notes);

	void
    setWorkProc();

	Boolean
    workProc();

	//
	// Check that we have enough memory before opening a dialog box.
	//
	static int
    checkMemory();
};


    inline NoteList&
XawTaskControl::currentNotes() const
{
    return *xtcNotesList;
}

#endif
