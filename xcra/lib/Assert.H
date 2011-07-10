// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Assertion macros
// Author	: S. Carrez
// Date		: Sun Sep 25 22:02:56 1994
// Version	: $Id: Assert.H,v 1.7 1996/08/04 15:22:58 gdraw Exp $
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
#ifndef _PRIVATE_ASSERT_H
#define _PRIVATE_ASSERT_H

#ifdef DEBUG
# define assert(ex)	(!(ex) ? (__assert__printf("Assertion failed"	\
				" at line"				\
				" %d in file %s\nassertion: " #ex " \n",\
				__LINE__, __FILE__), abort(), 0): 1)

# define assertPrintf(ex,msg)	(!(ex) ? (__assert__printf("Assertion failed"\
				" at line"				\
				" %d in file %s\nassertion: " #ex " \n",\
				__LINE__, __FILE__),			\
					  __assert__printf msg,		\
					  abort(), 0): 1)

# define Assert(ex)	(!(ex) ? (__assert__printf("Assertion failed"	\
				" at line"				\
				" %d in file %s\nassertion: " #ex " \n"	\
				"Still continuing...\n",		\
				__LINE__, __FILE__), 0): 1)

# define AssertPrintf(ex,msg) (!(ex) ? (__assert__printf("Assertion failed"\
				" at line"				\
				" %d in file %s\nassertion: " #ex " \n"	\
				"Still continuing...\n",		\
				__LINE__, __FILE__),			\
				       __assert__printf msg, 0): 1)


extern void __assert__printf(const char* msg, ...);

#else
# define assert(x)
# define Assert(x)
# define assertPrintf(x,y)
# define AssertPrintf(x,y)
#endif

//
// Assertions programming:
//
// Pre-condition	require		Check consistency of input params
// Post-condition	ensure		Check consistency of computed results
// Invariant		Invariant	Check invariant expressions/vars...
//
// The difference between `assert' and `Assert' is that the former
// aborts when an error is detected and the later continues.
//

#define require(x) 	assert(x)
#define	Require(x)	Assert(x)
#define requireMsg(x,y)	assertPrintf(x,y)
#define RequireMsg(x,y)	AssertPrintf(x,y)
#define	ensure(x) 	assert(x)
#define ensureMsg(x,y)	assertPrintf(x,y)
#define	Ensure(x)	Assert(x)
#define EnsureMsg(x,y)	AssertPrintf(x,y)
#define	invariant(x)	assert(x)
#define	Invariant(x)	Assert(x)

#endif
