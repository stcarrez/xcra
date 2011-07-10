// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Eyes
// Author	: S. Carrez
// Date		: Sat Oct  7 16:11:33 1995
// Version	: $Id: XawEyes.H,v 1.7 1996/08/04 15:34:19 gdraw Exp $
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
#ifndef _XAW_EYES_H_
#define _XAW_EYES_H_



// ----------------------------------------------------------------------
// Class :	XawEyes
//
// Role :	Definition of the Eyes widget
//
class XawEyes {
private:
    XawEyes(const XawEyes&);
    void operator=(const XawEyes&);
    void operator&();
protected:
    Widget	xEyes;
public:
	//
	// Create an Eyes object.
	//
    XawEyes(const char* _name, Widget _form, Widget _top);

    ~XawEyes();

	//
	// Return the eyes widget.
	//
    inline operator Widget() const;
};


    inline
XawEyes::operator Widget() const
{
    return xEyes;
}

#endif
