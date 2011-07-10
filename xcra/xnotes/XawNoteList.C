// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: NoteList
// Author	: S. Carrez
// Date		: Sat Sep 23 20:10:17 1995
// Version	: $Id: XawNoteList.C,v 1.8 1996/08/04 15:36:49 gdraw Exp $
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
#include "config.H"

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/List.h>

#include "Assert.H"
#include "Error.H"
#include "XawNoteList.H"


// ----------------------------------------------------------------------
//
//			Callbacks
//
// ----------------------------------------------------------------------

    static void
SelectCB(Widget, XtPointer _tag, XtPointer _data)
{
    XawListReturnStruct* XawLP = (XawListReturnStruct*) _data;
    XawNoteList* xl = (XawNoteList*) _tag;

    xl->select(XawLP->string);
}


// ----------------------------------------------------------------------
//
//			XawNoteList
//
// ----------------------------------------------------------------------

//
// Create the list of notes.
//
XawNoteList::XawNoteList(const char* _name, NoteList& _list,
			 Widget _form, Widget _top)
: xnList(_list), xnCurrentList(0)
{
	//
	// Create the viewport to get a scrolling window.
	//
    xnViewPort = XtVaCreateManagedWidget(_name, viewportWidgetClass,
					 _form, XtNfromVert, _top,
					 XtNallowVert, True, NULL);

    xnListW = XtVaCreateManagedWidget("list", listWidgetClass,
				      xnViewPort, NULL);

    XtAddCallback(xnListW, XtNcallback, SelectCB, (XtPointer) this);

	//
	// Fill in the list with the list of notes.
	//
    update();
}


XawNoteList::~XawNoteList()
{
#ifdef HAVE_X11R6
    XtDestroyWidget(xnViewPort);
#endif
    freeList();
}


//
// Free the memory allocated to store the list of notes.
//
    void
XawNoteList::freeList()
{
    if (xnCurrentList != (char**) NULL) {
	char** p = xnCurrentList;

	while (*p != (char*) NULL) {
	    free((free_ptr) *p++);
	}
	free((free_ptr) xnCurrentList);
	xnCurrentList = (char**) NULL;
    }
}


//
// Collect the list of notes and update the list widget.
//
    int
XawNoteList::update()
{
    long count = xnList.size();

    char** p = (char**) malloc(sizeof(char*) * (count + 1));
    if (p == (char**) NULL) {
	return -1;
    }

	//
	// Construct a list with the titles (save each title because
	// a note file can be deleted by another dialog box).
	//
    int i = 0;
    for (const Note* note = xnList.first(); note; note = note->next()) {
	p[i] = strdup(note->title());
	i++;
    }
    p[i] = 0;

    Ensure(i == xnList.size());

	//
	// Update the list widget.
	//
    XawListChange(xnListW, p, 0, 0, True);

	//
	// Save previous list if any.
	//
    freeList();

    xnCurrentList = p;
    return 0;
}


//
// One note has been selected.
//
    void
XawNoteList::select(const char*)
{
}

