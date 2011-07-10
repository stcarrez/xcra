// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Generic Command Launcher
// Author	: S. Carrez
// Date		: Sun Oct  8 10:31:52 1995
// Version	: $Id: XawLauncher.C,v 1.10 2000/02/23 08:21:00 ciceron Exp $
// Project	: Xcra
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
#include "config.H"
#include <stdio.h>

#include "Assert.H"
#include "Error.H"

#include "XawLauncher.H"


// ----------------------------------------------------------------------
//
//			Callbacks
//
// ----------------------------------------------------------------------

    static void
reportInput(XtPointer _data, int*, XtInputId*)
{
    XawLauncher* launcher = (XawLauncher*) _data;
    char buf[1024];

    int result = read(fileno(launcher->file()), buf, sizeof(buf) - 1);
    if (result <= 0) {
	(void) launcher->close();
    } else {
	buf[result] = 0;
	launcher->print(buf);
    }
}


// ----------------------------------------------------------------------
//
//			XawLauncher
//
// ----------------------------------------------------------------------

//
// Create an empty launcher.
//
XawLauncher::XawLauncher()
: xlReportFp((FILE*) NULL), xlStatus(0)
{
}


//
// Delete the launcher.
//
XawLauncher::~XawLauncher()
{
    (void) XawLauncher::close();
}


//
// Launch an external command `_cmd' and get all its output
// and call `print' with each message (groups of lines) reported
// by the command.
//
    int
XawLauncher::launch(const char* _cmd)
{
    if (xlReportFp != (FILE*) NULL) {
	Error::printf(MSG_LAUNCH_IN_ACTION);
	return -1;
    }

    char* cmd = (char*) malloc(strlen(_cmd) + sizeof(" 2>&1"));
    if (cmd == (char*) NULL) {
        Error::printf(MSG_MEMORY_ERROR);
        return -1;
    }
    
    sprintf(cmd, "%s 2>&1", _cmd);
    xlReportFp = popen(cmd, "r");
    free(cmd);
    
    if (xlReportFp == 0) {
	Error::report(MSG_LAUNCH_ERROR, _cmd);
	return -1;
    }

    xlReportInput = XtAppAddInput(xAppMainDialog->Context(),
				  fileno(xlReportFp),
				  (XtPointer)XtInputReadMask,
				  ::reportInput, (XtPointer)this);

    return 0;
}


//
// Close the pipe and return the exit status of the command.
//
    int
XawLauncher::close()
{
    if (xlReportFp == (FILE*) NULL) {
	return 0;
    }

    xlStatus = pclose(xlReportFp);
    XtRemoveInput(xlReportInput);
    xlReportFp = 0;

    return xlStatus;
}


//
// The output message `_buf' was read, print it somewhere in
// a text widget (default: use Error::printf).
//
    void
XawLauncher::print(const char* _buf)
{
    Error::printf("%s", _buf);
}


