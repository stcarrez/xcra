// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Assertion macros
// Author	: S. Carrez
// Date		: Sun Sep 25 22:12:57 1994
// Version	: $Id: Assert.C,v 1.7 1996/08/04 15:22:58 gdraw Exp $
// Project	: Xcra
// Keywords	: Assert, preconditions, postconditions
//
// Copyright (C) 1994, 1995, 1996 Stephane Carrez
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
//
#include <stdio.h>
#include <stdarg.h>
#include "Assert.H"

#ifdef DEBUG

void __assert__printf(const char* msg, ...)
{
    va_list argp;

    va_start(argp, msg);
    vfprintf(stderr, msg, argp);
    va_end(argp);
}
#endif
