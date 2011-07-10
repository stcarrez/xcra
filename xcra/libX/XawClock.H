// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Clock
// Author	: S. Carrez
// Date		: Sat Sep 16 18:37:14 1995
// Version	: $Id: XawClock.H,v 1.6 1996/08/04 15:34:22 gdraw Exp $
// Project	: Xcra
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
#ifndef _XAW_CLOCK_H_
#define _XAW_CLOCK_H_


// ----------------------------------------------------------------------
// Class :	XawClock
//
// Role :	Definition of the Clock widget
//
class XawClock {
private:
    XawClock(const XawClock&);
    void operator=(const XawClock&);
    void operator&();
protected:
    Widget	xClock;
public:
	//
	// Create a Clock object.
	//
    XawClock(const char* _name, Widget _form, Widget _top);

    ~XawClock();

	//
	// Return the clock widget.
	//
    inline operator Widget() const;
};


    inline
XawClock::operator Widget() const
{
    return xClock;
}

#endif
