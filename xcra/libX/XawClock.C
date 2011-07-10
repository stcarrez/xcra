// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Clock
// Author	: S. Carrez
// Date		: Sat Sep 16 18:43:35 1995
// Version	: $Id: XawClock.C,v 1.7 1996/08/04 15:34:19 gdraw Exp $
// Project	: xcra
// Keywords	: Clock, Xaw
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
// class XawClock		Clock object using Athena Widgets
// 
//
#include "config.H"

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>

#ifdef HAVE_X11R6
#include "Clock.h"
#else
#include <X11/Xaw/Clock.h>
#endif
#include "XawClock.H"


// ----------------------------------------------------------------------
//
//			XawClock
//
// ----------------------------------------------------------------------

//
// Create a Clock object.
//
XawClock::XawClock(const char* _name, Widget _form, Widget _top)
{
    xClock = XtVaCreateManagedWidget(_name, clockWidgetClass, _form,
				     XtNfromVert, _top,
				     NULL);
}

XawClock::~XawClock()
{
#ifdef HAVE_X11R6
    XtDestroyWidget(xClock);
#endif
}

