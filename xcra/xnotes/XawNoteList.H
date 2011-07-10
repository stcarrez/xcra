// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: NoteList
// Author	: S. Carrez
// Date		: Sat Sep 23 20:08:52 1995
// Version	: $Id: XawNoteList.H,v 1.7 1996/08/04 15:36:47 gdraw Exp $
// Project	: Xcra
// Keywords	: Notes, Xaw, Interface
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
// class XawNoteList		Display a list of notes and allow the
//				selection of one of them
// 
#ifndef _XAW_NOTELIST_H_
#define	_XAW_NOTELIST_H_

#include "Note.H"


// ----------------------------------------------------------------------
// Class :	XawNoteList
//
// Role :	Representation of a list of notes
//
// The `XawNoteList' class represents the list of notes in a scrolling
// list. It defines a virtual method `select' which is executed when
// a particular note of the list is selected.
//
class XawNoteList {
private:
    XawNoteList(const XawNoteList&);
    void operator=(const XawNoteList&);
    void operator&();
protected:
		//
		// Viewport holding the list of notes.
		//
    Widget	xnViewPort;

		//
		// List widget.
		//
    Widget	xnListW;

		//
		// The notes list being displayed.
		//
    NoteList&	xnList;

		//
		// List of notes titles as passed to the List widget.
		//
    char**	xnCurrentList;

private:
    void freeList();
public:
	//
	// Create a List object using the name `_name'. Put it in
	// the form `_form' under the widget `_top'. The list is filled
	// with a backup copy of the notes stored in `_list'.
	//
    XawNoteList(const char* _name, NoteList& _list, Widget _form, Widget _top);

    virtual ~XawNoteList();

	//
	// Return the Viewport widget holding the list widget.
	//
    inline operator Widget() const;

	//
	// Return the notes list.
	//
    inline NoteList& notes() const;

	//
	// Callback executed when the user presses the button
	// which correspond to the note `_title'. The default action
	// does nothing.
	//
    virtual void select(const char* _title);

	//
	// Update the list.
	//
    int update();
};

    inline NoteList&
XawNoteList::notes() const
{
    return xnList;
}

    inline
XawNoteList::operator Widget() const
{
    return xnViewPort;
}

#endif
