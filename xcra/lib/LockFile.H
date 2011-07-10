// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Distributed Lock File Utility Class
// Author	: S. Carrez
// Date		: Sun Sep 24 10:03:06 1995
// Version	: $Id: LockFile.H,v 1.6 1996/08/04 15:22:56 gdraw Exp $
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
#ifndef _LOCK_FILE_H_
#define _LOCK_FILE_H_

enum LockResult {
	//
	// The current process has the lock on the file.
	//
    LockOk,

	//
	// The file is already locked.
	//
    FileLocked,

	//
	// Problem when locking the file.
	//
    FileError
};

// ----------------------------------------------------------------------
// Class :	LockFile
//
// Role :	Lock file management
//
// The class LockFile manages a lock on a particular file to
// prevent several processes to modify the file at the same time.
// The class manages a lock counter which is incremented each time
// the lock must be get and decremented each time it is released.
// The file is really locked (using lockf) when the counter is set to 1.
// It is unlocked when it reaches 0.
//
class LockFile {
protected:
    int		lfFd;
    int		lfCount;
    const char*	lfFile;

private:
    LockResult  doLock();
    void	doUnLock();
public:
    inline LockFile(const char* _file);
    inline ~LockFile();

	//
	// Lock the file (increment the lock counter).
	//
    inline LockResult lock();

	//
	// Unlock the file (really unlock it when the lock counter reaches 0).
	//
    inline void unlock();

	//
	// Return the number of times we have called `lock'.
	//
    inline int count() const;
};


    inline
LockFile::LockFile(const char* _file)
{
    lfFd    = -1;
    lfCount = 0;
    lfFile  = _file;
}

    inline
LockFile::~LockFile()
{
    if (lfCount) {
	lfCount = 0;
	doUnLock();
    }
}

    inline int
LockFile::count() const
{
    return lfCount;
}

    inline LockResult
LockFile::lock()
{
    if (lfCount == 0) {
	return doLock();
    } else {
	lfCount ++;
	return LockOk;
    }
}

    inline void
LockFile::unlock()
{
    lfCount --;
    if (lfCount <= 0) {
	doUnLock();
    }
}

#endif
