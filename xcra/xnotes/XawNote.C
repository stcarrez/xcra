// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Dialog for the edition of a note
// Author	: S. Carrez
// Date		: Sat Sep 23 19:01:46 1995
// Version	: $Id: XawNote.C,v 1.11 2000/02/23 08:22:26 ciceron Exp $
// Project	: Xcra
// Keywords	: Note, Xaw, Interface
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
// class XawNote	Popup dialog to edit a note
// 
//
#include "config.H"
#include <stdio.h>
#include <limits.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/AsciiText.h>

#include "Assert.H"
#include "Error.H"
#include "Date.H"
#include "XawNote.H"


// ----------------------------------------------------------------------
//
//			Resources
//
// ----------------------------------------------------------------------

typedef struct {
   char*	xnPrintCommand;
} xnData;

#undef Offset
#define Offset(field) (XtOffsetOf(xnData, field))

static XtResource xnResources[] = {
  {"printCommand", "PrintCommand", XtRString, sizeof(char*),
     Offset(xnPrintCommand), XtRString, (XtPointer) PRINT_COMMAND}
};

static xnData rsrc;
static bool   rsrcInitialized = false;


// ----------------------------------------------------------------------
//
//			Callbacks
//
// ----------------------------------------------------------------------

//
// Cpp template for generation of Xaw callbacks (calling a C++ method).
//
#define XAW_EDIT_CALLBACK(NAME, TYPE, METHOD)			\
								\
    static void							\
NAME (Widget, XtPointer _tag, XtPointer)			\
{								\
    TYPE* xn = (TYPE *)_tag;					\
								\
    xn->METHOD ();						\
}

XAW_EDIT_CALLBACK(DismissCB, XawNote, dismiss)
XAW_EDIT_CALLBACK(TitleCB, XawNote, changeTitle)
XAW_EDIT_CALLBACK(ClearCB, XawNote, clearNote)
XAW_EDIT_CALLBACK(DestroyCB, XawNote, destroyNote)
XAW_EDIT_CALLBACK(SelectCB, XawNote, select)
XAW_EDIT_CALLBACK(SaveCB, XawNote, save)
XAW_EDIT_CALLBACK(PrintCB, XawNote, print)


//
// Create the note editor window.
//
XawNote::XawNote(const char* _name, Note& _note, int _mode)
: XawDialog(_name, XAW_DIALOG_TOPLEVEL),
  xnNote(_note), xnDelete(false), xnClosed(false)
{
    Arg  args[20];
    int  ac;
    Widget w;
    XtCallbackRec callbacks[2];

    callbacks[0].closure  = (XtPointer) this;
    callbacks[1].closure  = (XtPointer) NULL;
    callbacks[1].callback = (XtCallbackProc) NULL;

    xnTmpPath = 0;

	//
	// Get our resources (only once).
	//
    if (rsrcInitialized == false) {
	rsrcInitialized = true;
	XtGetApplicationResources(xAppMainDialog->TopLevel(),
				  (XtPointer) &rsrc,
				  xnResources,
				  XtNumber(xnResources),
				  NULL, 0);
    }

	//
	// Create the form which holds the buttons.
	//
    xnForm = XtCreateManagedWidget(_name, formWidgetClass, form(), args, 0);

	//
	// Create the note editor control buttons.
	//
    XtSetArg(args[0], XtNtop, XawChainTop);
    XtSetArg(args[1], XtNbottom, XawChainTop);
    XtSetArg(args[2], XtNleft, XawChainLeft);
    XtSetArg(args[3], XtNright, XawChainLeft);
    XtSetArg(args[4], XtNfromVert, None);
    XtSetArg(args[5], XtNborderWidth, 0);
    XtSetArg(args[6], XtNcallback, callbacks);

    callbacks[0].callback = DismissCB;
    w = XtCreateManagedWidget("nDismiss", commandWidgetClass,
			      xnForm, args, 7);

    if (_mode == 0) {
	XtSetArg(args[7], XtNfromHoriz, w);
	callbacks[0].callback = SaveCB;
	w = XtCreateManagedWidget("nSave", commandWidgetClass,
				  xnForm, args, 8);
    }
    XtSetArg(args[7], XtNfromHoriz, w);
    callbacks[0].callback = PrintCB;
    w = XtCreateManagedWidget("nPrint", commandWidgetClass,
			      xnForm, args, 8);
    xnPrint = w;

    XtSetArg(args[7], XtNfromHoriz, w);
    callbacks[0].callback = TitleCB;
    w = XtCreateManagedWidget("nTitle", commandWidgetClass,
			      xnForm, args, 8);

    if (_mode == 0) {
	XtSetArg(args[7], XtNfromHoriz, w);
	callbacks[0].callback = ClearCB;
	w = XtCreateManagedWidget("nClear", commandWidgetClass,
				  xnForm, args, 8);
    }

    XtSetArg(args[7], XtNfromHoriz, w);
    callbacks[0].callback = DestroyCB;
    w = XtCreateManagedWidget("nDestroy", commandWidgetClass,
			      xnForm, args, 8);

    XtSetArg(args[7], XtNfromHoriz, w);
    callbacks[0].callback = SelectCB;
    w = XtCreateManagedWidget("nSelect", commandWidgetClass,
			      xnForm, args, 8);

    if (_mode != 0) {
	char sizeBuf[32];

	sprintf(sizeBuf, "Size: %ld", xnNote.mapSize());
	XtSetArg(args[6], XtNlabel, sizeBuf);
	XtSetArg(args[7], XtNfromHoriz, w);
	w = XtCreateManagedWidget("nSize", labelWidgetClass,
				  xnForm, args, 8);
    }

    XtSetArg(args[0], XtNtop, XawChainTop);
    XtSetArg(args[1], XtNbottom, XawChainBottom);
    XtSetArg(args[2], XtNleft, XawChainLeft);
    XtSetArg(args[3], XtNright, XawChainRight);

    XtSetArg(args[4], XtNfromVert, w);
    XtSetArg(args[5], XtNfromHoriz, None);
    XtSetArg(args[6], XtNdisplayCaret, True);

	//
	// If the note is mapped in memory (external), it can't be modified
	// and the text widget sees it as a read-only text.
	//
    if (_mode == 1) {
	XtSetArg(args[7], XtNtype, XawAsciiString);
	XtSetArg(args[8], XtNuseStringInPlace, True);
	XtSetArg(args[9], XtNeditType, XawtextRead);
	XtSetArg(args[10], XtNlength, xnNote.mapSize() - 1);
	XtSetArg(args[11], XtNstring, (char*)xnNote.mapAddr() + 1);
	ac = 12;

	//
	// When the note is external and can't be mapped, the path is given
	// to the text widget and edition of the note is possible.
	//
    } else if (xnNote.type() == ExternalNote) {
	XtSetArg(args[7], XtNtype, XawAsciiFile);
	XtSetArg(args[8], XtNeditType, XawtextEdit);
	XtSetArg(args[9], XtNstring, xnNote.content());
	ac = 10;

    } else {
	XtSetArg(args[7], XtNtype, XawAsciiString);
	XtSetArg(args[8], XtNeditType, XawtextEdit);
	XtSetArg(args[9], XtNstring, xnNote.content());
	ac = 10;
    }
	//
	// Also set correctly the display position (scrollbars).
	//
    XtSetArg(args[ac], XtNdisplayPosition, xnNote.displayPosition()); ac++;
    xnText = XtCreateManagedWidget("note", asciiTextWidgetClass,
				   xnForm, args, ac);
	//
	// As well as the caret position.
	//
    XawTextSetInsertionPoint(xnText, xnNote.position());

    updateTitle();
    xnNote.setVisible();

    XtRealizeWidget(xDialog);

	//
	// Catch WM_DELETE window protocol.
	//
    setupWindowProtocols();

	//
	// Set the editor's window geometry.
	//
    if (xnNote.width() > 1 && xnNote.height() > 1) {
	XtSetArg(args[0], XtNx, xnNote.x());
	XtSetArg(args[1], XtNy, xnNote.y());
	XtSetArg(args[2], XtNwidth, xnNote.width());
	XtSetArg(args[3], XtNheight, xnNote.height());
	XtSetValues(xDialog, args, 4);
    }
    XtPopup(xDialog, XtGrabNone);

    if (xnNote.width() > 1 && xnNote.height() > 1) {
	Dimension x, y;

	XtSetArg(args[0], XtNx, &x);
	XtSetArg(args[1], XtNy, &y);
	XtGetValues(xDialog, args, 2);
	xnOffsetX = x - xnNote.x();
	xnOffsetY = y - xnNote.y();
    } else {
	xnOffsetX = 0;
	xnOffsetY = 0;
    }
}


//
// Delete the note editor object.
//
XawNote::~XawNote()
{
    require(xnTmpPath == 0);

    xnNote.clearVisible();
    xnNote.unMapFile();
    xnNote.notes().unlock();
    if (xnDelete == true) {
	NoteList& notes = xnNote.notes();

	notes.delNote(xnNote.title());
    }
}


//
// Update the title and icon associated to the note editor.
//
    void
XawNote::updateTitle()
{
    XtVaSetValues(xDialog, XtNiconName, xnNote.title(),
		  XtNtitle, xnNote.title(), NULL);
}


//
// Save the note and close the note editor.
//
    void
XawNote::dismiss()
{
    save();
    cancel();
}


//
// Save the note.
//
    int
XawNote::save()
{
    String str;
    Dimension top_x, top_y, top_width, top_height;

	//
	// Get window geometry and save it in the note.
	//
    XtVaGetValues(xDialog, XtNx, &top_x, XtNy, &top_y,
		  XtNwidth, &top_width, XtNheight, &top_height, NULL);

    top_x -= xnOffsetX;
    top_y -= xnOffsetY;
    xnNote.setGeometry(top_x, top_y, top_width, top_height);

    long dpos;
    XtVaGetValues(xnText, XtNdisplayPosition, &dpos, NULL);
    xnNote.setPosition((long) XawTextGetInsertionPoint(xnText), dpos);

    if (xnNote.type() == GeneralNote) {

	XtVaGetValues(xnText, XtNstring, &str, NULL);
	if (xnNote.set(str) != 0) {
	    return -1;
	}
    } else if (xnNote.mapAddr() == (void*) NULL) {
	XawAsciiSave(XawTextGetSource(xnText));
    }

    return xnNote.saveList();
}


//
// Print the current note. The note is saved in a temporary file. The
// print command is get from the `printCommand' resource and the temp.
// file is passed to that command. The note title is passed via the
// `NOTE' environment variable. The command is launched through a pipe,
// and the `Print' button is deactivated. It is still possible to
// do any other action while the note is being printed (including
// deleting the note). If the note editor must be closed while the
// note is being printed, the dialog box is simply popped down but
// the `XawNote' object is not deleted (see XawNote::cancel).
//
    void
XawNote::print()
{
    char tmpPath[PATH_MAX];
    const char* tmp;
    
    tmp = getenv("TMPDIR");
    if (tmp == 0)
       tmp = P_tmpdir;

       //
       // Create temporary file.
       //
    snprintf(tmpPath, PATH_MAX - 1, "%s/xcraXXXXXX", tmp);
    if (mkstemp(tmpPath) != 0) {
	Error::report(MSG_SAVE_ERROR, tmpPath);
	return;
    }
    xnTmpPath = strdup(tmpPath);

	//
	// Save the note in a temporary file.
	//
    if (xnTmpPath
	&& XawAsciiSaveAsFile(XawTextGetSource(xnText), xnTmpPath) != True) {
	Error::report(MSG_SAVE_ERROR, xnTmpPath);
	(void) unlink(xnTmpPath);
        free((free_ptr) xnTmpPath);
        xnTmpPath = 0;
	return;
    }

	//
	// The note title is passed in the `NOTE' environment variable.
	//
    char* nEnv = (char*) malloc((strlen(xnNote.title()) + sizeof("NOTE=")));
    if (nEnv) {
#if HAVE_PUTENV
	sprintf(nEnv, "NOTE=%s", xnNote.title());
#else
	sprintf(nEnv, "%s", xnNote.title());
#endif
    }
    char* cmd = (char*) malloc((strlen(rsrc.xnPrintCommand)
				+ strlen(xnTmpPath) + 2));
    if (cmd) {
	sprintf(cmd, "%s %s", rsrc.xnPrintCommand, xnTmpPath);
    }
    if (xnTmpPath == 0 || nEnv == 0 || cmd == 0
#if HAVE_PUTENV
	|| putenv(nEnv) != 0
#elif HAVE_SETENV
	|| setenv("NOTE", nEnv, 1) != 0
#endif
						) {
	Error::printf(MSG_MEMORY_ERROR);
	if (xnTmpPath) {
	    (void) unlink(xnTmpPath);
	    free((free_ptr) xnTmpPath);
	}
	if (nEnv) {
	    free((free_ptr) nEnv);
	}
	if (cmd) {
	    free((free_ptr) cmd);
	}
	xnTmpPath = 0;
	return;
    }

	//
	// Launch the print command.
	//
    int result = launch(cmd);

    free((free_ptr) cmd);
#if HAVE_PUTENV
    (void) putenv("NOTE=");
#elif HAVE_SETENV
    (void) setenv("NOTE", "", 1);
#endif
    free((free_ptr) nEnv);

    if (result != 0) {
	(void) unlink(xnTmpPath);
	free((free_ptr) xnTmpPath);
	xnTmpPath = 0;
	return;
    }

	//
	// Disable the Print button until the print command is finished.
	//
    XtSetSensitive(xnPrint, False);
}


//
// Open a dialog box to ask for a new title associated with the note.
//
    void
XawNote::changeTitle()
{
    XawConfirmDialog x("missTree", XAW_DIALOG_CANCEL);
}


//
// Clears the note content if the operation is confirmed by the user.
//
    void
XawNote::clearNote()
{
    XawConfirmDialog confirm("clearNote");

    if (confirm.result() == true) {
	XtVaSetValues (xnText, XtNstring, NULL, NULL);
    }
}


//
// Physically deletes the note (and closes the note editor) if the
// operation is confirmed by the user.
//
    void
XawNote::destroyNote()
{
    XawConfirmDialog confirm("destroyNote");

    if (confirm.result() == true) {
	xnDelete = true;
	cancel();
    }
}


//
// Select the entire note so that it can be pasted using the mouse.
//
    void
XawNote::select()
{
    String str;
    int    len;

    if (xnNote.mapAddr() != (void*) NULL) {
	str = (String) xnNote.mapAddr();
	len = (int) xnNote.mapSize();
    } else {
	XtVaGetValues(xnText, XtNstring, &str, NULL);
	len = strlen(str);
    }

    if (len != 0) {
	XawTextSetSelection(xnText, 0, len);
	XStoreBytes(XtDisplay(xnText), str, len);
    }
}


//
// Finish the print command.
//
    int
XawNote::close()
{
    int result;

    result = XawLauncher::close();

	//
	// Activate back the Print button.
	//
    XtSetSensitive(xnPrint, True);

    (void) unlink(xnTmpPath);
    free((free_ptr) xnTmpPath);
    xnTmpPath = 0;

	//
	// If meanwhile the note editor was closed, delete the `XawNote'
	// object. The `XawNote' object was not deleted because it was
	// needed by the external print command (see XawLauncher).
	//
    if (xnClosed) {
	delete this;
    }
    return result;
}


//
// Close the note editor. If a print command is running, the `XawNote'
// object must not be deleted. It is marked as `closed' and the dialog
// box is popped down.
//
    void
XawNote::cancel()
{
    xnClosed = true;
    if (xnTmpPath) {
	XawDialog::close();
    } else {
	XawDialog::cancel();
    }
}



//
// Start the note editor to edit the note `_note'.
//
    int
XawNote::edit(Note& _note)
{
	//
	// Can't edit a subnote.
	//
    if (_note.type() == SubNotes) {
	return -1;
    }

	//
	// Only one edition window.
	//
    if (_note.isVisible() != 0) {
	Error::printf(MSG_NOTE_BEING_EDITED, _note.title());
	return -1;
    }

	//
	// And get the lock first.
	//
    if (_note.notes().lock() != LockOk) {
	return -1;
    }

    _note.notes().load();
    
    int mode = 0;
    if (_note.type() == ExternalNote) {
	mode = _note.mapFile() == 0 ? 1 : 0;
    }

    new XawNote("editNote", _note, mode);

    return 0;
}



