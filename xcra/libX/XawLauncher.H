// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Generic Command Launcher
// Author	: S. Carrez
// Date		: Sun Oct  8 10:33:27 1995
// Version	: $Id: XawLauncher.H,v 1.7 1996/08/04 15:34:19 gdraw Exp $
// Project	: XCra
// Keywords	: Launch, pipe, Xaw
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
// class XawLauncher		Generic command launcher
// 				(Launch a command through a pipe and print
//				the result using Error::printf or whatever)
// 
#ifndef _XAW_LAUNCHER_H_
#define	_XAW_LAUNCHER_H_

#include <X11/Intrinsic.h>
#include "XawDialog.H"


// ----------------------------------------------------------------------
// Class :	XawLauncher
//
// Role :	Generic command launcher
//
class XawLauncher {
private:
    XawLauncher(const XawLauncher&);
    void operator=(const XawLauncher&);
    void operator&();
protected:
    FILE*	xlReportFp;
    XtInputId	xlReportInput;
    int		xlStatus;
private:public:
    inline FILE* file() const;
public:
	//
	// Create an empty launcher.
	//
    XawLauncher();
    virtual ~XawLauncher();

	//
	// Launch an external command `_cmd' and get all its output
	// and call `print' with each message (groups of lines) reported
	// by the command.
	//
    int launch(const char* _cmd);

	//
	// Close the pipe and return the exit status of the command.
	//
    virtual int close();

	//
	// The output message `_buf' was read, print it somewhere in
	// a text widget (default: use Error::printf).
	//
    virtual void print(const char* _buf);

	//
	// Return the exit status of the previous command.
	//
    inline int status() const;
};


    inline int
XawLauncher::status() const
{
    return xlStatus;
}


    inline FILE*
XawLauncher::file() const
{
    return xlReportFp;
}

#endif
