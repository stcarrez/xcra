// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: File Selector
// Author	: S. Carrez
// Date		: Sat Dec  9 16:11:54 1995
// Version	: $Id: XawFileSelect.C,v 1.6 1996/08/04 15:34:22 gdraw Exp $
// Project	: Xcra
// Keywords	: File, Selector, Xaw
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
// class XawFileSelector	File Selector object
//
#include "config.H"

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>

#include "xfsel/FileSelect.h"
#include "XawFileSelect.H"


// ----------------------------------------------------------------------
//
//			XawFileSelector
//
// ----------------------------------------------------------------------

//
// Create a file selector object.
//
XawFileSelector::XawFileSelector(const char* _name, Widget _form, Widget _top)
{
    xFileSelector = XtVaCreateManagedWidget(_name, fileselectorWidgetClass,
					    _form, XtNfromVert, _top, NULL);
}


//
// Delete the file selector.
//
XawFileSelector::~XawFileSelector()
{
#ifdef HAVE_X11R6
    XtDestroyWidget(xFileSelector);
#endif
}


//
// Set the initial selected file (directory and basename).
//
    void
XawFileSelector::file(const char* _path)
{
    XtVaSetValues(xFileSelector, XtNpath, _path, NULL);
}


//
// Return the current selected file (directory and basename).
//
    const char*
XawFileSelector::file() const
{
    const char* p;
    
    XtVaGetValues(xFileSelector, XtNpath, &p, NULL);
    return p;
}

