/* -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Configuration and strange portability hacks
// Author	: S. Carrez
// Date		: Wed Mar  8 22:19:44 1995
// Version	: $Id: config.H,v 1.13 1996/08/04 15:38:57 gdraw Exp $
// Project	: xcra
// Keywords	: Port, Config
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
// Nothing
**/
#ifndef _CONFIG_H
#define _CONFIG_H

#include "config.h"

/*
 * Put 1 here to debug Xcra; -g and assertion macros on.
 */
#if 1
#ifndef DEBUG
# define DEBUG
#endif

#if 0
#ifndef HAVE_PROFILE
# define HAVE_PROFILE
#endif
#endif
#endif

#define HAVE_XTOOLS

#ifndef free_ptr
# define free_ptr	void*
#endif

#ifndef min
#  define min(x, y)                     (((x) < (y)) ? (x) : (y))
#endif

#ifndef max
#  define max(x, y)                     (((x) > (y)) ? (x) : (y))
#endif

#ifndef ProjectX

#ifndef __GNUG__
typedef int bool;
#  define true 1
#  define false 0
#endif

#ifdef sun3
#include <sys/types.h>

extern "C" void bcopy(const void*,void*,size_t);
extern "C" int gethostname(char*, int);

extern "C" int madvise(void*,long,int);
extern "C" int munmap(void*,long);
extern "C" long sigsetmask(long);
extern "C" char* getwd(char*);
extern "C" const char* re_comp(const char*);
extern "C" int re_exec(const char*);
extern "C" int lockf(int, int, off_t);

#endif

#ifdef sparc
extern "C" char* strdup (const char*);
#endif

extern "C" void set_new_handler(...);

#include <sys/types.h>

#if TIME_WITH_SYS_TIME
#  include <sys/time.h>
#  include <time.h>
#else
#  if HAVE_SYS_TIME_H
#    include <sys/time.h>
#  else
#    include <time.h>
#  endif
#endif

#if HAVE_STDLIB_H
#  include <stdlib.h>
#endif

#if HAVE_STRING_H
#  include <string.h>
#endif

#if HAVE_MEMORY_H
#  include <memory.h>
#endif

#if HAVE_UNISTD_H
#  include <unistd.h>
#endif

#if HAVE_FCNTL_H
#  include <fcntl.h>
#endif

#ifndef HAVE_STRCHR
#  define strchr		index
#  define strrchr		rindex
#endif

#ifndef HAVE_MEMCPY
#  define memcpy(d,s,n)		bcopy(s,d,n)
#  define memset(d,c,n)		((c == 0) ? bzero(d,n) : abort())
#endif

#ifndef HAVE_MEMMOVE
#  define memmove(d,s,n) 	bcopy(s,d,n)
#endif

#ifndef HAVE_STRDUP
#  define strdup(s)		strcpy(malloc(strlen(s)+1), s, strlen(s)+1)
#endif

#ifdef HAVE_GETCWD
#  define getwd(PATH)		getcwd((PATH),1024)
#endif

#ifndef HAVE_SIGSETMASK
#  define sigsetmask(MASK)      (0)
#endif

#ifndef HAVE_GETHOSTNAME
#  define gethostname(PATH,SIZE) (strcpy(PATH,"unknown host"),0)
#endif

/* Either we have both <getopt.h> and getopt_long available or we have none */
#if HAVE_GETOPT_H
#  ifndef HAVE_GETOPT_LONG
#      undef HAVE_GETOPT_H
#  endif
#else
#  ifdef HAVE_GETOPT_LONG
#      undef HAVE_GETOPT_LONG
#  endif
#endif
#endif
#endif
