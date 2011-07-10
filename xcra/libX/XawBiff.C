// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Biff
// Author	: S. Carrez
// Date		: Sat Oct  7 16:08:07 1995
// Version	: $Id: XawBiff.C,v 1.8 1996/08/04 15:34:25 gdraw Exp $
// Project	: Xcra
// Keywords	: Biff, Xaw
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
// class XawBiff		Xbiff object using Athena Widgets
// 				(from X11R5 Xaw library)
//
#include "config.H"

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>

#ifdef HAVE_X11R6
#include "Mailbox.h"
#else
#include <X11/Xaw/Mailbox.h>
#endif
#include "XawBiff.H"

// ----------------------------------------------------------------------
//
//			XawBiff
//
// ----------------------------------------------------------------------

//
// Create a Mail-Biff object.
//
XawBiff::XawBiff(const char* _name, Widget _form, Widget _top)
{
    xBiff = XtVaCreateManagedWidget(_name, mailboxWidgetClass, _form,
				    XtNfromVert, _top,
				    NULL);
}

XawBiff::~XawBiff()
{
#ifdef HAVE_X11R6
    XtDestroyWidget(xBiff);
#endif
}

