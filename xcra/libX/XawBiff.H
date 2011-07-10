// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Mail Biffer
// Author	: S. Carrez
// Date		: Sat Oct  7 18:55:36 1995
// Version	: $Id: XawBiff.H,v 1.7 1996/08/04 15:34:19 gdraw Exp $
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
#ifndef _XAW_XBIFF_H_
#define _XAW_XBIFF_H_


// ----------------------------------------------------------------------
// Class :	XawBiff
//
// Role :	Definition of the Biff widget
//
class XawBiff {
private:
    XawBiff(const XawBiff&);
    void operator=(const XawBiff&);
    void operator&();
protected:
    Widget	xBiff;
public:
	//
	// Create a Mail-Biff object.
	//
    XawBiff(const char* _name, Widget _form, Widget _top);

    ~XawBiff();

	//
	// Return the biff widget.
	//
    inline operator Widget() const;
};


    inline
XawBiff::operator Widget() const
{
    return xBiff;
}

#endif
