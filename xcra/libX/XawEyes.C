// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Eyes
// Author	: S. Carrez
// Date		: Sat Oct  7 16:08:07 1995
// Version	: $Id: XawEyes.C,v 1.8 1996/08/04 15:34:25 gdraw Exp $
// Project	: Xcra
// Keywords	: Eyes, Xaw
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
// class XawEyes		Xeyes object using Athena Widgets
// 				(from X11R5 xeyes demo)
//
#include "config.H"

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>

#include "Eyes.h"
#include "XawEyes.H"


// ----------------------------------------------------------------------
//
//			XawEyes
//
// ----------------------------------------------------------------------

//
// Create an Eyes object.
//
XawEyes::XawEyes(const char* _name, Widget _form, Widget _top)
{
    xEyes = XtVaCreateManagedWidget(_name, eyesWidgetClass, _form,
				    XtNfromVert, _top,
				    XtNshapeWindow, False,
				    NULL);
}

XawEyes::~XawEyes()
{
#ifdef HAVE_X11R6
    XtDestroyWidget(xEyes);
#endif
}

