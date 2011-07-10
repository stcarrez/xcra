// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Dialog Boxes for the management of Notes
// Author	: S. Carrez
// Date		: Sun Sep 24 08:50:25 1995
// Version	: $Id: XawNoteMgr.H,v 1.8 1996/08/04 15:36:49 gdraw Exp $
// Project	: Xcra
// Keywords	: Note, Xaw, Interface
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
// class XawNoteAdd	Dialog box to create a new note
// class XawNoteRemove	Dialog box to remove a note
// class XawNoteSelect	General purpose dialob box to select a note (& edit it)
// class XawNoteFileSelect
// class XawNoteGrep	Grep command on the notes (recursive)
//
#ifndef _XAW_NOTE_MANAGER_H_
#define	_XAW_NOTE_MANAGER_H_

#include "Task.H"
#include "TaskControl.H"
#include "../progs/XawTask.H"
#include "XawLauncher.H"
#include "XawFileSelect.H"
#include "XawNoteList.H"

class XawNoteAdd;

// ----------------------------------------------------------------------
// Class :	XawNoteType
//
// Role :	Toggle to select the note type (enum NoteType).
// 
class XawNoteType : public XawText {
protected:
    XawNoteAdd&	xAddNote;
    NoteType	xType;
public:
    XawNoteType(const char* _name, const NoteType _type, XawNoteAdd& _add);
    ~XawNoteType();

    void select(XtPointer _data);
};


// ----------------------------------------------------------------------
// Class :	XawNoteAdd
//
// Role :	Dialog box for the creation of a new note.
//
class XawNoteAdd : public XawDialog {
	friend class XawNoteType;
protected:
		//
		// The name of the note (or its title).
		//
    XawText		xName;

		//
		// Label and 3 toggle buttons to allow the selection
		// of the type of the note.
		//
    XawText		xNoteMode;
    XawNoteType		xNormalNote;
    XawNoteType		xExternalNote;
    XawNoteType		xSubNote;

    XawTaskControl&	xTask;
    NoteType		xType;
public:
	//
	// Create a dialog box to query the name of the note to create
	// a new note.
	//
    XawNoteAdd(const char* _name, XawTaskControl& _xtask);
    ~XawNoteAdd();

	//
	// Select the type of the note to create.
	//
    inline void selectMode(const NoteType _type);

	//
	// Check the dialog box parameters and create the new note.
	//
    void activate();
};


    inline void
XawNoteAdd::selectMode(const NoteType _type)
{
    xType = _type;
}


// ----------------------------------------------------------------------
// Class :	XawNoteSelect
//
// Role :	Creation/management of a dialog box to select a note.
//
class XawNoteSelect : public XawDialog, public XawNoteList {
protected:
    NoteList&		xNotes;
public:
    XawNoteSelect(const char* _name, NoteList& _notes);
    ~XawNoteSelect();

	//
	// Select the note whose title is `_title' and open the note
	// editor for that note.
	//
    void select(const char* _title);
};


// ----------------------------------------------------------------------
// Class :	XawNoteRemove
//
// Role :	Creation/management of a dialog box to select and delete
//		a note.
//
class XawNoteRemove : public XawNoteSelect {
    XawTaskControl& xTask;
public:
	//
	// Create a dialog box for the deletion of a note.
	//
    XawNoteRemove(const char* _name, XawTaskControl& _xtask);
    ~XawNoteRemove();

	//
	// Remove the note whose title is `_title'.
	//
    void select(const char* _title);
};


// ----------------------------------------------------------------------
// Class :	XawNoteFileSelect
//
// Role  :	Dialog box to select the current notes file (file selector).
//
class XawNoteFileSelect : public XawDialog {
protected:
    XawFileSelector	xFileSelect;
    XawTaskControl&	xTask;
public:
	//
	// Create a dialog box with a file selector object for the
	// selection of the notes file associated with the current task.
	//
    XawNoteFileSelect(const char* _name, XawTaskControl& _xtask);
    ~XawNoteFileSelect();

	//
	// Check the selected file and change the notes file of
	// the current task.
	//
    void activate();
};


// ----------------------------------------------------------------------
// Class :	XawNoteGrep
//
// Role :	Dialog box to grep in the notes.
//
class XawNoteGrep : public XawDialog, public XawLauncher {
protected:
    XawText	xFile;
    XawText	xRegex;
    Widget	xResult;
public:
	//
	// Create a dialog box to grep in the notes file.
	//
    XawNoteGrep(const char* _name, const char* _file);
    ~XawNoteGrep();

	//
	// Launch the grep command using the selected parameters.
	//
    void activate();

	//
	// The grep command is now finished, reactivate the <grep> button.
	//
    int  close();

	//
	// Print the grep result `_buf' in the result window.
	//
    void print(const char* _buf);
};

#endif
