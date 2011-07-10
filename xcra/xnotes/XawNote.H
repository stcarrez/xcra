// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Dialog for the edition of a note
// Author	: S. Carrez
// Date		: Sat Sep 23 18:56:09 1995
// Version	: $Id: XawNote.H,v 1.9 1996/08/04 15:36:51 gdraw Exp $
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
// class XawNote		Note editor
// 
#ifndef _XAW_NOTE_H_
#define	_XAW_NOTE_H_


#include "Note.H"
#include "XawDialog.H"
#include "XawLauncher.H"


// ----------------------------------------------------------------------
// Class :	XawNote
//
// Role :	Note editor
//
// The note editor is a simple text widget with a set of buttons to
// perform some basic actions. It derives from `XawDialog' to get
// the toplevel window and from `XawLauncher' to be able to launch
// a print command transparently.
//
// When a note editor is opened, it is popped up at the place it was
// the previous time. The scrollbar position as well as the cursor
// position are the same. The information to do this is saved in
// each note (see Note.C).
//
// The note editor is not created directly. It is opened by
// the static operation `edit'.
//
class XawNote : public XawDialog, public XawLauncher {
protected:
		//
		// The note being edited (It can't be deleted until
		// the note editor is opened).
		//
    Note&	xnNote;

    Widget	xnForm;
    Widget	xnText;
    Widget	xnPrint;

    char*	xnTmpPath;
    int		xnOffsetX;
    int		xnOffsetY;

    bool	xnDelete;
    bool	xnClosed;

    void updateTitle();

	//
	// Create a Note editor object using the name `_name' to
	// edit the note `_note'.
	//
    XawNote(const char* _name, Note& _note, int _mode);

    ~XawNote();
public:
	//
	// Save the note and close the note editor.
	//
    void dismiss();

	//
	// Save the note.
	//
    int save();

	//
	// Print the note (using lpr or enscript or ...).
	//
    void print();
    
	//
	// Open a dialog box to change the title associated with the note.
	//
    void changeTitle();

	//
	// Clear the note content.
	//
    void clearNote();

	//
	// Delete the note and close the note editor.
	//
    void destroyNote();

    	//
	// Select the complete note content (for pasting).
	//
    void select();

	//
	// Redefines `XawLauncher::close' to be notified when the print
	// command has terminated.
	//
    int close();

	//
	// Redefined `XawDialog::cancel' to prevent the object from
	// being deleted if the note is being printed.
	//
    void cancel();
    
	//
	// Start the note editor to edit the note `_note'.
	//
    static int edit(Note& _note);
};

#endif
