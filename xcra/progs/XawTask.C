// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Interface for the Task Tool
// Author	: S. Carrez
// Date		: Sat Oct  1 13:28:01 1994
// Version	: $Id: XawTask.C,v 1.15 2000/02/23 08:24:32 ciceron Exp $
// Project	: Xcra
// Keywords	: Task, Xaw, Interface
//
// Copyright (C) 1995, 1996, 2002 Stephane Carrez
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
// class XawTaskErrorDialog	Error message dialog box.
// class XawTaskMenuItem	Item of a popup menu.
//
#include "config.H"
#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Form.h>

#include "XawDialog.H"

#include "Task.H"
#include "TaskControl.H"
#include "XawTaskMgr.H"
#include "XawTaskList.H"
#include "XawTask.H"
#include "XawNoteMgr.H"
#include "XawNote.H"


typedef struct {
    unsigned	xtcMenuSize;
    int		xtcUpdateTime;
    char*	xtcDirectory;
    char*	xtcTaskFormat;
    char*	xtcTimeFormat;
    Boolean     xtcAllowClose;
    
#ifdef HAVE_XTOOLS
    Boolean	xtcHasClock;
    Boolean	xtcHasEyes;
    Boolean	xtcHasBiff;
#endif
} xtcData;

#undef Offset
#define Offset(field) (XtOffsetOf(xtcData, field))

static XtResource xtcResources[] = {
	//
	// Max. number of task names which can appear in the task popup menu.
	// (also used by the notes popup menu).
	//
  {"taskMenuSize", "TaskMenuSize", XtRInt, sizeof(int),
     Offset(xtcMenuSize), XtRString, (XtPointer) "10"},  

	//
	// Time in seconds for the update of the task time.
	//
  {"taskUpdateTime", "TaskUpdateTime", XtRInt, sizeof(int),
     Offset(xtcUpdateTime), XtRString, (XtPointer) "60"},

	//
	// Directory where cra files must be loaded/saved.
	//
  {"taskDirectory", "TaskDirectory", XtRString, sizeof(char*),
     Offset(xtcDirectory), XtRString, (XtPointer) "."},

	//
	// Format string which must be used for the task popup button.
	//
  {"taskFormat", "ReportModel", XtRString, sizeof(char*),
     Offset(xtcTaskFormat), XtRString, (XtPointer) "%T"},

	//
	// Format string which must be used for the time popup button.
	//
  {"timeFormat", "ReportModel", XtRString, sizeof(char*),
     Offset(xtcTimeFormat), XtRString, (XtPointer) "%2tH:%02tM"},

	//
	// True if we can close the main dialog box.
	//
  {"allowClose", "AllowClose", XtRBoolean, sizeof(Boolean),
     Offset(xtcAllowClose), XtRImmediate, (XtPointer) False}

#ifdef HAVE_XTOOLS
	//
	// True if a Clock widget must be created.
	//
  , {"hasClock", "HasTools", XtRBoolean, sizeof(Boolean),
     Offset(xtcHasClock), XtRImmediate, (XtPointer) False},

	//
	// True if an Eyes widget must be created.
	//
  {"hasEyes", "HasTools", XtRBoolean, sizeof(Boolean),
     Offset(xtcHasEyes), XtRImmediate, (XtPointer) False},

	//
	// True if a Biff widget must be created.
	//
  {"hasBiff", "HasTools", XtRBoolean, sizeof(Boolean),
     Offset(xtcHasBiff), XtRImmediate, (XtPointer) False}
#endif
};

xtcData rsrc;


static XrmOptionDescRec options[] = {
{"-directory", "taskDirectory",		XrmoptionSepArg,	NULL},
{"-update",    "taskUpdateTime",	XrmoptionSepArg,	NULL},
{"-msize",     "taskMenuSize",     	XrmoptionSepArg,	NULL},
{"-allow-close","allowClose",		XrmoptionNoArg,		"TRUE"}
#ifdef HAVE_XTOOLS
,{"-chime",	"*clock.chime",		XrmoptionNoArg,		"TRUE"},
{"-highlight",	"*clock.highlight",	XrmoptionSepArg,	NULL},
{"-digital",	"*clock.analog",	XrmoptionNoArg,		"FALSE"},
{"-analog",	"*clock.analog",	XrmoptionNoArg,		"TRUE"},
{"-clock",	"hasClock",		XrmoptionNoArg,		"TRUE"},
{"-eyes",	"hasEyes",		XrmoptionNoArg,		"TRUE"},
{"-biff",	"hasBiff",		XrmoptionNoArg,		"TRUE"}
#endif
};

static String fallback_resources[] = {
	// Label resources
"*Form*resizable:	false",
"*Label.height:		24",
"*Label.justify:	left",
"*Label.resize:		false",
"*Label.width:		240",
"*MenuButton.width:	150",
"*MenuButton.resize:	false",
"*MenuButton.left:	ChainLeft",
"*MenuButton.right:	ChainLeft",
"*SimpleMenu.width:	170",
"*SmeBSB.width:		140",
"*SmeLine.width:	140",
"*Text.width:		100",
"*Text.horizDistance:	10",

"*timesButton.fromHoriz:	taskButton",
"*notesButton.fromHoriz:	timesButton",
"*buttonForm*Command.width:	120",
"*buttonForm*Command.vertDistance: 5",
"*buttonForm*Command.height:	20",

"*timeFormat:	%2tH:%02tM",
"*taskFormat:	%N",

"*cal*Label.width:	66",
"*cal*Label.height:	18",
"*cal*Label.resize:	false",
"*cal*Label.justify:	center",
"*cal*Command.height:	18",
"*cal*Command.width:	66",
"*cal*Command.resize:	false",

"*cal*vertDistance:	2",

"*togtask.justify:	left",
"*togtask.resize:	false",
"*togtask.bottom:	ChainTop",
"*togtask.top:		ChainTop",

"*labeltask.resize:	false",
"*labeltask.bottom:	ChainTop",
"*labeltask.top:	ChainTop",

"*task.top:		ChainTop",
"*task.bottom:		ChainTop",

"*togweekReport.fromHoriz:	togdayReport",
"*togfortReport.fromHoriz:	togweekReport",
"*togmonthReport.fromHoriz:	togfortReport",

"*togdaySort.fromHoriz:	tognameSort",
"*togweekSort.fromHoriz:	togdaySort",
"*togfortSort.fromHoriz:	togweekSort",
"*togmonthSort.fromHoriz:	togfortSort",

	NULL
};

// ----------------------------------------------------------------------
//
//			Callbacks
//
// ----------------------------------------------------------------------

    static void
updateTimeCB(XtPointer _data, XtIntervalId*)
{
    XawTaskControl* xtc = (XawTaskControl*) _data;
    
    xtc->timer();
}

    static Boolean
reportErrors(XtPointer _data)
{
    XawTaskControl* xtc = (XawTaskControl*) _data;

    return xtc->workProc();
}


// ----------------------------------------------------------------------
//
//			XawTaskMenuItem
//
// ----------------------------------------------------------------------

//
// Create a new menu item.
//
XawTaskMenuItem::XawTaskMenuItem(const char* _name, XawMenu& _menu,
				 XawTaskControl& _tk, const DialogType _which)
  : XawMenuItem(_name, _menu),
    xTask(_tk),
    xDialogKind(_which)
{
}

XawTaskMenuItem::~XawTaskMenuItem()
{
}


//
// Execute the action associated with the menu item.
// That action is determined by `xDialogKind'.
//
    void
XawTaskMenuItem::activate()
{
    XawDialog* dg = 0;

	//
	// Check whether we have enough memory. Since the X-toolkit
	// calls exit() when no memory is available, it is preferable
	// not to open dialog boxes...
	//
    if (XawTaskControl::checkMemory() != 0) {
	return;
    }

    switch (xDialogKind) {
	case XTASK_LIST:
		dg = new XawTaskSelect("listTask", FilterNone, xTask);
		break;

	case XTASK_REMOVE:
		dg = new XawTaskRemove("delTask", xTask);
		break;

	case XTASK_REFERENCE:
		dg = new XawTaskEditReference("ref", xTask);
		break;

	case XTASK_FORECASTS:
		dg = new XawTaskEditForecasts("forcasts", xTask);
		break;

	case XTASK_ADDTIME:
		dg = new XawTaskEditTimes("addTime", xTask);
		break;

	case XTASK_REPORT:
		dg = new XawTaskReport("report", xTask);
		break;

	case XTASK_ADD:
		dg = new XawTaskAdd("addTask", xTask);
		break;

	case XTASK_SWITCH_TO:
		dg = new XawTaskMoveTime("switchTask", 2, xTask);
		break;

	case XTIME_ADD_FROM:
		dg = new XawTaskMoveTime("incTime", 1, xTask);
		break;

	case XTIME_SUB_FROM:
		dg = new XawTaskMoveTime("decTime", 0, xTask);
		break;

	case XNOTE_ADD :
		dg = new XawNoteAdd("addNote", xTask);
		break;

	case XNOTE_REMOVE :
		dg = new XawNoteRemove("delNote", xTask);
		break;

	case XNOTE_CHOOSE :
		dg = new XawNoteSelect("selNote", xTask.currentNotes());
		break;

	case XNOTE_CHANGE :
		dg = new XawNoteFileSelect("selNoteFile", xTask);
		break;

	case XNOTE_GREP :
		dg = new XawNoteGrep("grepNotes", xTask.currentNotes().file());
		break;

	case XNOTE_TREE :
		{
		    XawConfirmDialog x("missTree", XAW_DIALOG_CANCEL);
		}
		break;

	default :
		if (xDialogKind & XTASK_PROJECT) {
		    xTask.charge(xDialogKind & (~XTASK_PROJECT));
		} else {
		    Note* note = xTask.currentNotes()[xDialogKind &
					    (~XNOTE_SELECT)];

		    if (note != (Note*) NULL) {
			XawNote::edit(*note);
		    }
		}
		return;
    }

    if (dg) {
	dg->popup();
    }
}


// ----------------------------------------------------------------------
//
//			XawTaskErrorDialog
//
// Creation/management of an error dialog box.
//
// ----------------------------------------------------------------------

XawTaskErrorDialog::XawTaskErrorDialog(const char* _name,
				       XawTaskControl& _xtask) 
   : XawErrorDialog(_name),
     xTask(_xtask)
{
    xTask.xtcErrorDialog = this;
}

XawTaskErrorDialog::~XawTaskErrorDialog()
{
	//
	// We need to detect when the error dialog box is closed.
	// In that case, it will be re-created (see XawTaskControl::report).
	//
    xTask.xtcErrorDialog = 0;
}


// ----------------------------------------------------------------------
//
//			XawTaskControl
//
// ----------------------------------------------------------------------

//
// Create and initialize the main application window.
//
XawTaskControl::XawTaskControl(int _argc, char* _argv[])
  : TaskControl(),

	//
	// Initialize the X toolkit and create the toplevel window.
	//
    XawDialog("XCra", options, XtNumber(options),
	      fallback_resources, &_argc, _argv),

	//
	// Create the Task popup menu.
	//
    xtcTaskMenu("task", xForm, xTopLevel),
	xtcAddTask("addTask", xtcTaskMenu, *this, XTASK_ADD),
	xtcDelTask("delTask", xtcTaskMenu, *this, XTASK_REMOVE),
	xtcRefTask("refTask", xtcTaskMenu, *this, XTASK_REFERENCE),
	xtcForTask("forTask", xtcTaskMenu, *this, XTASK_FORECASTS),
	xtcSelTask("selTask", xtcTaskMenu, *this, XTASK_LIST),
	xtcSwiTask("swiTask", xtcTaskMenu, *this, XTASK_SWITCH_TO),
	xtcSeparator("line", xtcTaskMenu),

	//
	// Create the Time popup menu.
	//
    xtcTimesMenu("times", xForm, xTopLevel),
	xtcAddTime("addTime", xtcTimesMenu, *this, XTASK_ADDTIME),
	xtcRepTime("repTime", xtcTimesMenu, *this, XTASK_REPORT),
	xtcIncTime("incTime", xtcTimesMenu, *this, XTIME_SUB_FROM),
	xtcDecTime("decTime", xtcTimesMenu, *this, XTIME_ADD_FROM),

	//
	// Create the Notes popup menu.
	//
    xtcNotesMenu("notes", xForm, xTopLevel),
	xtcAddNote("addNote", xtcNotesMenu, *this, XNOTE_ADD),
	xtcDelNote("delNote", xtcNotesMenu, *this, XNOTE_REMOVE),
	xtcSelNote("selNote", xtcNotesMenu, *this, XNOTE_CHOOSE),
	xtcChgNote("chgNote", xtcNotesMenu, *this, XNOTE_CHANGE),
//	xtcTreeNote("treeNote", xtcNotesMenu, *this, XNOTE_TREE),
	xtcSearchNote("grepNote", xtcNotesMenu, *this, XNOTE_GREP),
	xtcSeparatorNote("line", xtcNotesMenu)

{
    xtcNotesList  = 0;
    xtcErrorDialog= 0;
    xtcTaskLabel  = 0;
    xtcTimeLabel  = 0;
    xtcIconName   = 0;

	//
	// Get our resources.
	//
    XtGetApplicationResources(xTopLevel, (XtPointer) &rsrc,
			      xtcResources, XtNumber(xtcResources),
			      NULL, 0);

    xtcMenuSize     = rsrc.xtcMenuSize;
    xtcNoteMenuSize = rsrc.xtcMenuSize;
    xtcUpdateTime   = rsrc.xtcUpdateTime * 1000;

    if (xtcMenuSize < 1 || xtcUpdateTime < 1000) {
	fprintf(stderr, "xcra: Invalid resources\n");
	exit(1);
    }

#ifdef HAVE_XTOOLS
    xtcClock      = 0;
    xtcEyes	  = 0;
    xtcBiff	  = 0;

    Widget w = None;
    if (rsrc.xtcHasClock == True) {
	xtcClock = new XawClock("clock", xForm, xtcNotesMenu.Button());
	w = *xtcClock;
    }

    if (rsrc.xtcHasEyes == True) {
	xtcEyes = new XawEyes("eyes", xForm, xtcNotesMenu.Button());
	if (w != None) {
	    XtVaSetValues(*xtcEyes, XtNfromHoriz, w, NULL);
	}
	w = *xtcEyes;
    }

    if (rsrc.xtcHasBiff == True) {
	xtcBiff = new XawBiff("biff", xForm, xtcNotesMenu.Button());
	if (w != None) {
	    XtVaSetValues(*xtcBiff, XtNfromHoriz, w, NULL);
	}
	w = *xtcBiff;
    }
#endif

	//
	// Set the directory where the cra files are located.
	//
    setDirectory(rsrc.xtcDirectory);

    if (XawTaskControl::checkMemory() != 0) {
	fprintf(stderr, "xcra: Virtual memory exhausted\n");
	exit(1);
    }

	//
	// Setup the timer and update the labels
	//
    xtcTimerId = XtAppAddTimeOut(xContext, xtcUpdateTime,
				 updateTimeCB, (XtPointer) this);

	//
	// Setup the work proc. to report errors during idle time.
	//
    setWorkProc();

    popup();
}

XawTaskControl::~XawTaskControl()
{
    TaskControl::close();
    _exit(0);
}


//
// Update the task popup menu to make sure that the most recently used
// tasks appear in the popup menu. This operation must be called each
// time a task is deleted, created or when a task becomes the active
// task.
//
    void
XawTaskControl::updateProjectMenu()
{
    for (int i = 0; i < xtcMenuSize; i++) {
	XawMenuItem* item = xtcTaskMenu[(unsigned) (i + 7)];

	if (i < tcNrTasks) {
	    if (item == 0) {
		DialogType t = (DialogType)(XTASK_PROJECT | i);
                
		item = new XawTaskMenuItem("project", xtcTaskMenu,
                                           *this, t);
	    }
	    XtVaSetValues(*item, XtNlabel, tcList[i].name(), NULL);

	} else if (item != 0) {
	    delete item;
	    i--;
	    
	} else {
	    break;
	}
    }
}


//
// Update the notes popup menu to make sure that the most recently used
// notes appear in the popup menu. This operation must be called each
// time the notes file is changed or when a note is created or deleted.
//
    void
XawTaskControl::updateNoteMenu()
{
    const char* noteFile = tcCurTask.notes();
    char buf[512];

    if (noteFile == 0 || noteFile[0] == 0) {
	sprintf(buf, "%s/.notes", rsrc.xtcDirectory);
	noteFile = buf;
    }

    xtcNotesList = NoteList::findNotes(noteFile);
    if (xtcNotesList == (NoteList*) NULL) {
	xtcNotesList = new NoteList(noteFile);
    }

    xtcNotesList->load();

    for (int i = 0; i < xtcNoteMenuSize; i++) {
	XawMenuItem* item = xtcNotesMenu[(unsigned) (i + 6)];

	if (i < xtcNotesList->size()) {
	    if (item == 0) {
		DialogType t = (DialogType)(XNOTE_SELECT | i);

		item = new XawTaskMenuItem("note", xtcNotesMenu,
					   *this, t);
	    }
	    XtVaSetValues(*item, XtNlabel, (*xtcNotesList)[i]->title(), NULL);

	} else if (item != 0) {
	    delete item;
	    i --;

	} else {
	    break;
	}
    }

    const char* name = strrchr(noteFile, '/');
    if (name == (const char*) NULL) {
	name = noteFile;
    } else {
	name ++;
    }

    XtVaSetValues(xtcNotesMenu.Button(), XtNlabel, name, NULL);
}


//
// Create a new task. This virtual method is redefined to be able to
// update the task popup menu with the new task (may be).
//
    TaskCharge&
XawTaskControl::addTask(const char* _name)
{
    TaskCharge& task = TaskList::addTask(_name);

    if (!task.isNil()) {
	updateProjectMenu();
    }
    return task;
}


//
// Delete a task. This virtual method is redefined to be able to take
// into account the deleted task in the task popup menu.
//
    int
XawTaskControl::delTask(const char* _name)
{
    int result = TaskControl::delTask(_name);

    if (result == 0) {
	updateProjectMenu();
    }
    return result;
}


//
// Move the time value specified in `_value' to/from the task
// whose name is `_name'.
//
    int
XawTaskControl::moveTime(const char* _name, time_t _value, int _sign)
{
    int result = TaskControl::moveTime(_name, _value, _sign);

    if (result == 0) {
	setup();
    }
    return result;
}


//
// Update the time for this task (update the labels).
//
    int
XawTaskControl::updateTime()
{
    int result = TaskControl::updateTime();

	//
	// See if the notes file has changed and re-load it to update
	// the notes popup menu
	//
    if (xtcNotesList->needLoad()) {
	updateNoteMenu();
    }

    setup();
    return result;
}


//
// Load the file (update the labels).
//
    int
XawTaskControl::load()
{
    int result = TaskControl::load();

    if (result != 0) {
	return result;
    }

    setup();
    updateProjectMenu();
    updateNoteMenu();
    return result;
}


//
// Charge the task whose name is `_name'.
//
    int
XawTaskControl::charge(const char* _name)
{
    int result = TaskControl::charge(_name);

    if (result == 0) {
	setup();
	updateProjectMenu();
	updateNoteMenu();
    }

    return result;
}


//
// Change the active task to the `_which' th (in the LRU).
//
    void
XawTaskControl::charge(int _which)
{
    if (_which >= 0 && _which < tcNrTasks) {
	charge(tcList[_which].name());
    }
}


//
// Catch cancel on the window to make sure the user does not
// exit accidentally (unless an option allows exit).
//
    void
XawTaskControl::cancel()
{
    if (rsrc.xtcAllowClose == True) {
        TaskControl::close();
        _exit(0);
    }
}


//
// Report an error message in a dialog box.
//
    void
XawTaskControl::report(const char* _msg)
{
    if (xtcErrorDialog == 0) {
	xtcErrorDialog = new XawTaskErrorDialog("error", *this);
    }

    xtcErrorDialog->report(_msg);

    setWorkProc();
}


//
// Timer callback executed each minute to update the task time.
// The timer is restarted each time this operation is called.
//
    void
XawTaskControl::timer()
{
    updateTime();
    save();

    xtcTimerId = XtAppAddTimeOut(xContext, xtcUpdateTime,
				 updateTimeCB, (XtPointer) this);
}


//
// Setup the new labels when the time/task changes.
//
    void
XawTaskControl::setup()
{
    char buf[512];
    Arg  args[1];

    if (tcNrTasks == 0)
        return;

	//
	// Get the new task label (update if changed).
	//
    tcCurTask.format(buf, rsrc.xtcTaskFormat);
    if (xtcTaskLabel == 0 || strcmp(xtcTaskLabel, buf) != 0) {
	XtSetArg(args[0], XtNlabel, buf);
	XtSetValues(xtcTaskMenu.Button(), args, 1);

	if (xtcTaskLabel) {
	    free((free_ptr)xtcTaskLabel);
	}
	xtcTaskLabel = strdup(buf);
    }

	//
	// Get the new icon name.
	//
    if (xtcIconName == 0 || strcmp(xtcIconName, tcCurTask.name()) != 0) {
	XtSetArg(args[0], XtNiconName, tcCurTask.name());
	XtSetValues(xTopLevel, args, 1);

	if (xtcIconName) {
	    free((free_ptr)xtcIconName);
	}
	xtcIconName = strdup(tcCurTask.name());
    }

	//
	// Get the new time string.
	//
    tcCurTask.format(buf, rsrc.xtcTimeFormat);
    if (xtcTimeLabel == 0 || strcmp(xtcTimeLabel, buf) != 0) {
	XtSetArg(args[0], XtNlabel, buf);
	XtSetValues(xtcTimesMenu.Button(), args, 1);

	if (xtcTimeLabel) {
	    free((free_ptr)xtcTimeLabel);
	}
	xtcTimeLabel = strdup(buf);
    }
}


//
// Change the notes file associated to the current task.
//
    int
XawTaskControl::setNotesFile(const char* _file)
{
    tcCurTask.setNoteFile(_file);
    updateNoteMenu();
    return 0;
}


//
// The notes file identified by `_notes' was changed. Check
// if the note menu must be updated.
//
    void
XawTaskControl::mayBeUpdateNoteMenu(NoteList* _notes)
{
    if (_notes == xtcNotesList) {
	updateNoteMenu();
    }
}


    void
XawTaskControl::setWorkProc()
{
    if (xtcWorkProc == 0) {
	xtcWorkProc = XtAppAddWorkProc(xContext, reportErrors,
				       (XtPointer) this);
    }
}


    Boolean
XawTaskControl::workProc()
{
    Error::finish();

    xtcWorkProc = 0;

    return True;
}


//
// Check that we have enough memory before opening a dialog box.
//
    int
XawTaskControl::checkMemory()
{
	//
	// Allocate a large block and free it immediately
	// This just checks that we have at least that many bytes of
	// memory. The X toolkit is protected against NULL returned by
	// malloc. The operator `new' is also protected. So, this will
	// never be checked within the code.
	//
    char* p = (char*)malloc(100 * 1024);
    if (p == 0) {
	return -1;
    }
    free(p);
    return 0;
}


