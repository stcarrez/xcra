// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: File Selector
// Author	: S. Carrez
// Date		: Sat Dec  9 16:07:55 1995
// Version	: $Id: XawFileSelect.H,v 1.5 1996/08/04 15:34:17 gdraw Exp $
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
//
#ifndef _XAW_FILE_SELECTOR_H_
#define _XAW_FILE_SELECTOR_H_


// ----------------------------------------------------------------------
// Class :	XawFileSelector
//
// Role :	Definition of the File Selector object.
//
class XawFileSelector {
private:
    XawFileSelector(const XawFileSelector&);
    void operator=(const XawFileSelector&);
    void operator&();
protected:
    Widget	xFileSelector;
public:
    XawFileSelector(const char* _name, Widget _form, Widget _top);

    ~XawFileSelector();

	//
	// Return the current selected file (directory and basename).
	//
	const char*
    file() const;
  
	//
	// Set the initial selected file (directory and basename).
	//
	void
    file(const char* _path);

	//
	// Return the file selector widget.
	//
    inline operator Widget() const;
};


    inline
XawFileSelector::operator Widget() const
{
    return xFileSelector;
}

#endif
