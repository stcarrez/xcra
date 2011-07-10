// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Distributed Lock File Utility Class
// Author	: S. Carrez
// Date		: Sun Sep 24 10:17:47 1995
// Version	: $Id: LockFile.C,v 1.7 1996/08/04 15:22:58 gdraw Exp $
// Project	: xcra
// Keywords	: lock, distributed, file
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
// class LockFile	Lock a file
//
#include "config.H"
#include <stdio.h>
#include <errno.h>

#include "Assert.H"
#include "LockFile.H"

#if !HAVE_LOCKF
	// Locking of file is not available...
#  define lockf(fd,mode,size)	(0)
#endif

// ----------------------------------------------------------------------
//
//			LockFile
//
// ----------------------------------------------------------------------

//
// Lock the file using `lockf'. If the file does not exist,
// we assume that the lock is get.
//
    LockResult
LockFile::doLock()
{
    lfFd = open(lfFile, O_RDWR);
    if (lfFd < 0) {
	if (errno == ENOENT) {
	    lfCount = 1;
	    return LockOk;
	}
	return FileError;
    }

    int result = lockf(lfFd, F_TLOCK, 0);
    if (result != 0) {
	close(lfFd);
	lfFd = -1;
	return FileError;
    }

    lfCount = 1;
    return LockOk;
}


//
// Unlock the file.
//
    void
LockFile::doUnLock()
{
    RequireMsg(lfCount == 0,
	       ("unlock called with invalid lock counter %d\n", lfCount));

    if (lfFd >= 0) {
	lockf(lfFd, F_ULOCK, 0);
	close(lfFd);
	lfFd = -1;
    }
}
