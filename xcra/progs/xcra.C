// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Main
// Author	: S. Carrez
// Date		: Fri Mar 31 21:32:19 1995
// Version	: $Id: xcra.C,v 1.9 1996/08/04 15:37:49 gdraw Exp $
// Project	: xcra
// Keywords	: xcra
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
// main
//
#include "config.H"
#include "Error.H"
#include "Task.H"
#include "TaskControl.H"
#include "XawTask.H"

#ifdef HAVE_PROFILE
// Profiling with X11R6 shared libraries
extern "C" void monstartup(...);
extern "C" void _mcleanup();
extern long etext;
#endif

    int
main(int argc, char* argv[])
{
#ifdef HAVE_PROFILE
    monstartup(main, &etext);
    on_exit(_mcleanup, 0);
#endif

    XawTaskControl xTask(argc, argv);

    (void)xTask.load();

    xTask.mainLoop();
    return 0;
}

