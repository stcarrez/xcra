// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Dialog Boxes for the management of Notes
// Author	: S. Carrez
// Date		: Sun Sep 24 08:54:11 1995
// Version	: $Id: XawNoteMgr.C,v 1.11 2000/02/23 08:22:50 ciceron Exp $
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
// class XawNoteMode	Toggle window to select the type of the note
// class XawNoteAdd	Dialog box to create a new note
// class XawNoteRemove	Dialog box to remove a note
// class XawNoteSelect	General purpose dialog box to select a note (& edit it)
// class XawNoteGrep    Grep command in all the notes
//
#include "config.H"
#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/AsciiText.h>

#include "XawDialog.H"
#include "XawFileSelect.H"
#include "XawNoteMgr.H"
#include "XawNote.H"


// ----------------------------------------------------------------------
// Class :	XawExternalFileSelect
//
// Role  :	Dialog box to select a path for the creation of an
//		external note.
//
class XawExternalFileSelect : public XawDialog {
protected:
    XawFileSelector	xFileSelect;
    XawTaskControl&	xTask;
    NoteList&		xNotes;
    NoteType		xType;
    char*		xNoteName;
public:
	//
	// Create a dialog box with a file selector object for the
	// selection of the external path.
	//
    XawExternalFileSelect(const char* _name, NoteList& _notes,
			  XawTaskControl& _task,
			  NoteType _type, const char* _title);
    ~XawExternalFileSelect();

	//
	// Check the selected file and create the new note.
	//
    void activate();
};


XawExternalFileSelect::XawExternalFileSelect(const char* _name,
					     NoteList& _notes,
					     XawTaskControl& _task,
					     NoteType _type,
					     const char* _title)
  : XawDialog(_name),
    xFileSelect("file", xForm, None),
    xTask(_task),
    xNotes(_notes),
    xType(_type)
{
    xNoteName = (char*) XtNewString(_title);
}

XawExternalFileSelect::~XawExternalFileSelect()
{
    XtFree(xNoteName);
}

    void
XawExternalFileSelect::activate()
{
    const char* file = xFileSelect.file();
    bool fileError   = false;

	//
	// Verify the note file string (sanity check). It is necessary
	// to check because the path will be saved in the Xtask files
	// and it must be compatible with the format of the Xtask files.
	//
    if (XawText::acceptableText(file, MAX_NOTE_LEN, "Note file") == false) {
	fileError = true;

	//
	// If the file does not exist, we need a confirmation.
	//
    } else if (access(file, R_OK) < 0) {
	XawConfirmDialog confirm("createNoteFile");
	
	fileError = (confirm.result() == false) ? true : false;

	//
	// Try to load the note file so that we check its format.
	//
    } else if (xType == SubNotes
	       && NoteList::findNotes(file) == (NoteList*) NULL) {
	fileError = true;
    }

    if (fileError == false) {
	Note* note = xNotes.create(xNoteName, xType, file);
	if (note) {
	    xNotes.save();
	    xTask.mayBeUpdateNoteMenu(&xNotes);
	}
	XawDialog::activate();
    }
}


// ----------------------------------------------------------------------
//
//			XawNoteType
//
// ----------------------------------------------------------------------

//
// Create a toggle button which correspond to the note type `_type'.
//
XawNoteType::XawNoteType(const char* _name, const NoteType _type,
			 XawNoteAdd& _add)
  : XawText(_name, XAW_TOGGLE, 0, _add.form(), _add.xName),
    xAddNote(_add), xType(_type)
{
}

XawNoteType::~XawNoteType()
{
}


    void
XawNoteType::select(XtPointer _data)
{
    if ((long)(_data) == 1) {
	xAddNote.selectMode(xType);
    }
}


// ----------------------------------------------------------------------
//
//			XawNoteAdd
//
// ----------------------------------------------------------------------

//
// Create of a dialog box for the creation of a new note.
//
XawNoteAdd::XawNoteAdd(const char* _name, XawTaskControl& _xtask)
	//
	// Create the top level dialog box.
	//
  : XawDialog(_name),

	//
	// Create the _name label & text edition.
	//
    xName("note", XAW_TEXT | XAW_LABEL, 0, xForm, None),

	//
	// Create 3 toggle buttons to select the type of the note.
	//
    xNoteMode("nLabel", XAW_LABEL | XAW_FIRST_GROUP, 0, xForm, xName),
	xNormalNote("normNote", GeneralNote, *this),
	xExternalNote("extNote", ExternalNote, *this),
	xSubNote("subNote", SubNotes, *this),

    xTask(_xtask)
{
    xType = GeneralNote;
}

XawNoteAdd::~XawNoteAdd()
{
}


//
// Check the dialog box fields and create the new note.
//
    void
XawNoteAdd::activate()
{
    const char* title   = xName;
    bool noteError	= false;
    NoteList& notes = xTask.currentNotes();

	//
	// Verify the note title. It must not be empty.
	//
    if (!XawText::acceptableText(title, MAX_NOTE_TITLE_SIZE, "Note title")) {
	noteError = true;

    } else if (notes.find(title) != (Note*) NULL) {
	Error::printf(MSG_NOTE_EXIST, title);
	noteError = true;
    }

    xName.setSemanticError(noteError);

	//
	// No error detected, create the new note.
	//
    if (noteError == False) {

	if (xType != GeneralNote) {
	    XawExternalFileSelect* dg =
		new XawExternalFileSelect("FileSelect", notes, xTask,
					  xType, title);
	    dg->popup();
	} else {

		//
		// Try to lock the note file.
		//
	    if (notes.lock() != LockOk) {
	        Error::printf(MSG_FILE_LOCKED, notes.file());
	    } else {

		Note* note = notes.create(title, xType, "");
		if (note != (Note*) NULL) {
			//
			// Raise the window to edit the note content.
			//
		    xTask.mayBeUpdateNoteMenu(&notes);
		    XawNote::edit(*note);
		}
		notes.unlock();
	    }
	}
	XawDialog::activate();
    }
}


// ----------------------------------------------------------------------
//
//			XawNoteSelect
//
// ----------------------------------------------------------------------

//
// Create a dialog box with the list of notes and allow the selection
// of one of them for opening the note editor.
//
XawNoteSelect::XawNoteSelect(const char* _name, NoteList& _notes)
	//
	// Create the top level dialog box.
	//
  : XawDialog(_name, XAW_DIALOG_CANCEL),

	//
	// Create the list widget holding all the note titles.
	//
    XawNoteList(_name, _notes, xForm, None),

    xNotes(_notes)
{
}

XawNoteSelect::~XawNoteSelect()
{
}


//
// Select the note whose title is `_title' and open the note
// editor for that note.
//
    void
XawNoteSelect::select(const char* _title)
{
    Note* note = xNotes.find(_title);
    if (note != (Note*) NULL) {
	XawNote::edit(*note);
    }
    activate();
}


// ----------------------------------------------------------------------
//
//			XawNoteRemove
//
// ----------------------------------------------------------------------

//
// Create a dialog box with the list of notes and allow the selection
// of one of them for deleting the note.
//
XawNoteRemove::XawNoteRemove(const char* _name, XawTaskControl& _xtask)
  : XawNoteSelect(_name, _xtask.currentNotes()),
    xTask(_xtask)
{
}

XawNoteRemove::~XawNoteRemove()
{
}


//
// Remove the note whose title is `_title'.
//
    void
XawNoteRemove::select(const char* _title)
{
    if (xNotes.delNote(_title) == 0) {
	xTask.mayBeUpdateNoteMenu(&xNotes);
    } else {
	Error::printf(MSG_CANT_DELETE_NOTE, _title);
    }
    activate();
}


// ----------------------------------------------------------------------
//
//			XawNoteFileSelect
//
// ----------------------------------------------------------------------

//
// Create a dialog box with a file selector object for the
// selection of the notes file associated with the current task.
//
XawNoteFileSelect::XawNoteFileSelect(const char* _name,
				     XawTaskControl& _xtask)
  : XawDialog(_name),
    xFileSelect("file", xForm, None),
    xTask(_xtask)
{
    xFileSelect.file(_xtask.currentNotes().file());
}

XawNoteFileSelect::~XawNoteFileSelect()
{
}

    void
XawNoteFileSelect::activate()
{
    const char* file = xFileSelect.file();
    bool fileError   = false;

	//
	// Verify the note file string (sanity check). It is necessary
	// to check because the path will be saved in the Xtask files
	// and it must be compatible with the format of the Xtask files.
	//
    if (XawText::acceptableText(file, MAX_NOTE_LEN, "File name") == false) {
	fileError = true;

	//
	// If the file does not exist, we need a confirmation.
	//
    } else if (access(file, R_OK) < 0) {
	XawConfirmDialog confirm("createNoteFile");
	
	fileError = (confirm.result() == false) ? true : false;

	//
	// Try to load the note file so that we check its format.
	//
    } else if (NoteList::findNotes(file) == (NoteList*) NULL) {
	fileError = true;
    }

    if (fileError == false) {
	xTask.setNotesFile(file);
	XawDialog::activate();
    }
}


// ----------------------------------------------------------------------
//
//			XawNoteGrep
//
// ----------------------------------------------------------------------

//
// Create a dialog box to grep in the notes file.
//
XawNoteGrep::XawNoteGrep(const char* _name, const char* _file)
  : XawDialog(_name),
    XawLauncher(),
    xFile("file", XAW_TEXT | XAW_LABEL, 0, xForm, None),
    xRegex("regex", XAW_TEXT | XAW_LABEL, 0, xForm, xFile)
{
    xFile = _file;

    xResult = XtVaCreateManagedWidget("result", asciiTextWidgetClass, xForm,
				      XtNeditType, XawtextAppend,
				      XtNtype, XawAsciiString,
				      XtNuseStringInPlace, False,
				      XtNdisplayCaret, False,
				      XtNstring, "",
				      XtNfromVert, (Widget) xRegex,
				      XtNfromHoriz, None, NULL);
}

XawNoteGrep::~XawNoteGrep()
{
}


//
// Launch the grep command using the selected parameters.
//
    void
XawNoteGrep::activate()
{
    const char* file = xFile;
    const char* reg  = xRegex;
    bool fileError   = false;
    bool regexError  = false;

    if (XawText::acceptableText(reg, 1024, "Regular expression") == false) {
	regexError = true;
    }

	//
	// Verify the note file string (sanity check).
	//
    if (XawText::acceptableText(file, 1024, "File name") == False) {
	fileError = true;
    } else {
		//
		// Try to load the note file. If the file does not
		// exist, raise an error.
		//
	NoteList* notes = NoteList::findNotes(file);

	if (notes == (NoteList*) NULL) {
	    fileError = true;
	    Error::printf(MSG_LOADERROR, file);
	}
    }

    xFile.setSemanticError(fileError);
    xRegex.setSemanticError(regexError);

	//
	// Launch the external grep command and deactivate the <grep> button.
	//
    if (fileError == false && regexError == false) {
	char* cmd = (char*) XtMalloc(strlen(file) + strlen(reg) + 80);

	sprintf(cmd, "noteManager -f '%s' -g '%s' -R",
		file, reg);

	if (launch(cmd) == 0) {
	    XtSetSensitive(xOk, False);
	}
	XtFree(cmd);
    }
}


//
// The grep command is now finished, reactivate the <grep> button.
//
    int
XawNoteGrep::close()
{
    int result = XawLauncher::close();

    XtSetSensitive(xOk, True);
    return result;
}


//
// Print the grep result `_buf' in the result window.
//
    void
XawNoteGrep::print(const char* _msg)
{
    XawTextBlock    textBlock;
    XawTextPosition lastPos;

    textBlock.firstPos = 0;
    textBlock.length   = strlen(_msg);
    textBlock.ptr      = (char*)_msg;

    lastPos = XawTextGetInsertionPoint(xResult);
    XawTextReplace(xResult, lastPos, lastPos, &textBlock);
    XawTextSetInsertionPoint(xResult, lastPos + textBlock.length);
}

