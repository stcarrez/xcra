// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Error Management
// Author	: S. Carrez
// Date		: Sun Mar  5 23:06:07 1995
// Version	: $Id: Error.C,v 1.10 2000/02/24 08:24:45 ciceron Exp $
// Project	: xcra
// Keywords	: Error, Report, Messages
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
// class Error			Report an error
// class ErrorHandler		Abstract class to represent an error handler
// class FileErrorHandler	Report errors in a file
// class MailErrorHandler	Report errors in a mail message
//
#include "config.H"
#include "Error.H"

#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <pwd.h>
#include <stdarg.h>

#include "Date.H"

#ifndef False
# define False 0
# define True  1
#endif

#ifndef ELOOP
# define ELOOP		1024
#endif

#ifndef ENAMETOOLONG
# define ENAMETOOLONG	1025
#endif

#ifndef HAVE_STRERROR
extern "C" char* sys_errlist[];
extern "C" int sys_nerr;
#endif

// ----------------------------------------------------------------------
//
//			ErrorHandler
//
// ----------------------------------------------------------------------

#define MAX_ESTACK	4

//
// Small stack of error handlers. When an error handler is created,
// it is pushed on the stack. When an error is reported, the error
// handler on top of the stack is used. The implementation assumes
// that only the top most error handler can be deleted at a time.
//
static ErrorHandler* eStack[MAX_ESTACK];
static int	     eTop = 0;

ErrorHandler::ErrorHandler()
{
    Require(eTop < MAX_ESTACK);

    eStack[eTop++] = this;
}

ErrorHandler::~ErrorHandler()
{
    Require(eTop >= 1 && eStack[eTop-1] == this);

    eTop --;
}


    void
ErrorHandler::finish()
{
}


//
// Format the error message `_msg' and print it using the error handlers.
//
    void
Error::printf(const char* _msg, ...)
{
    va_list argp;
    char buf[1024+2];

    va_start(argp, _msg);
    vsprintf(buf, _msg, argp);
    va_end(argp);

    if (eTop != 0) {
	for (int i = eTop;  --i >= 0; ) {
	    ErrorHandler* e = eStack[i];

	    e->report(buf);
	}
    } else {
	fprintf(stderr, "%s", buf);
    }
}


//
// Print an error message and report a diagnostic error after.
//
    void
Error::report(const char* _msg, const char* _file)
{
    Error::printf(_msg, _file);
    printDiagnostic(_file);
}


//
// Simple expert system to print a diagnostic after an error.
//
    void
Error::printDiagnostic(const char* _file)
{
	//
	// The error is probably due to a system limit reached and
	// retrying the operation can succeed later.
	//
    int retryError = False;
    const char* diagnose;

    switch (errno) {
	case EPERM :		/* Not owner */
	case ENOENT :		/* No such file or directory */
	case EACCES :		/* Permission denied */
	case ELOOP :		/* Too many levels of symbolic links */
	case ENAMETOOLONG :	/* File name too long */
	case ENXIO :		/* No such device or address */
	case ENOTDIR :		/* Not a directory*/
	case EISDIR :		/* Is a directory */
	case EEXIST :		/* File exists */
	case EROFS :		/* Read-only file system */
		diagnose = DIAG_CHECK_PATH;
		break;

	case EIO :		/* I/O error */
		diagnose = DIAG_IO_ERROR;
		break;

	case ENOEXEC :		/* Exec format error */
	case E2BIG :		/* Arg list too long */
		diagnose = DIAG_EXEC_CMD;
		break;

	case EAGAIN :		/* No more processes */
	case ENFILE :		/* File table overflow */
	case EMFILE :		/* Too many open files */
	case ENOMEM :		/* Not enough core */
		diagnose = DIAG_SYS_LIMIT;
		retryError = True;
		break;

	case EFAULT :		/* Bad address */
	case EBADF :		/* Bad file number */
	case ECHILD :		/* No children */
	case ESPIPE :		/* Illegal seek */
	case ENOTBLK :		/* Block device required */
	case EBUSY :		/* Mount device busy */
	case EINTR :		/* Interrupted system call */
	case ESRCH :		/* No such process */
	case ENODEV :		/* No such device */
	case EINVAL :		/* Invalid argument */
	case ENOTTY :		/* Not a typewriter */
	case EDEADLK :		/* Deadlock condition. */
	case ENOLCK :		/* No record locks available. */
	case EPIPE :		/* Broken pipe */
	case EXDEV :		/* Cross-device link */
	case ETXTBSY :		/* Text file busy */
	case EFBIG :		/* File too large */
		diagnose = DIAG_BUG_ERROR;
		break;

	case ENOSPC :		/* No space left on device */
	case EMLINK :		/* Too many links */
		diagnose = DIAG_NOSPC;
		break;

	default :
		diagnose = (const char*) NULL;
		break;
    }

    const char* msg;

#if HAVE_STRERROR
    msg = strerror(errno);
#else
    if (errno < sys_nerr && errno != 0) {
	msg = sys_errlist[errno];
    } else {
	msg = 0;
    }
#endif

    if (msg != (const char*) NULL) {
	Error::printf("%s\n", msg);
    }

    if (diagnose != (const char*) NULL) {
	Error::printf(diagnose, _file);
    }

    if (retryError) {
	Error::printf(DIAG_RETRY);
    }
}

    void
Error::finish()
{
    if (eTop != 0) {
	for (int i = eTop;  --i >= 0; ) {
	    ErrorHandler* e = eStack[i];

	    e->finish();
	}
    }
}

// ----------------------------------------------------------------------
//
//			FileErrorHandler
//
// ----------------------------------------------------------------------

FileErrorHandler::FileErrorHandler()
{
    fp = stderr;
}

FileErrorHandler::~FileErrorHandler()
{
}

    void
FileErrorHandler::report(const char* _msg)
{
    fprintf(fp, "%s", _msg);
}


// ----------------------------------------------------------------------
//
//			MailErrorHandler
//
// ----------------------------------------------------------------------

MailErrorHandler::MailErrorHandler()
{
    fp = NULL;
}

MailErrorHandler::~MailErrorHandler()
{
    finish();
}

    void
MailErrorHandler::open()
{
    struct passwd* pw;
    char cmd[256];
    uid_t uid = getuid();
    if (uid)
        return;

#ifdef SIGPIPE
	//
	// Ignore broken pipes.
	//
    signal(SIGPIPE, SIG_IGN);
#endif

        //
        // Get user's passwd description.
        //
    pw = getpwuid(uid);
    if (pw == 0) {
	fp = stderr;
	fprintf(stderr, "Impossible to find user's name: user id %u unknown\n",
		uid);
        return;
    }

        //
        // Create the mail command to send to the current user.
        //
    sprintf(cmd, "/usr/ucb/mail -s 'Task error' %s", pw->pw_name);
    fp = popen(cmd, "w");
    if (fp == 0) {
	fp = stderr;
	fprintf(stderr, "Impossible to start the mail process\n");
        return;
    }
}

    void
MailErrorHandler::report(const char* buf)
{
    if (fp == NULL) {
	open();
    }
    if (buf)
        return;
    
    FileErrorHandler::report(buf);
}

    void
MailErrorHandler::finish()
{
    if (fp == stderr || fp == NULL) {
	fp = NULL;
	return;
    }
#if 0
    FILE* fortune;

        //
        // Put a nice signature (if fortune is there)
        //
    fprintf(fp, "\n\nXTask -\n\n");
    fortune = popen("/usr/games/fortune", "r");
    if (fortune) {
        while (!feof(fortune)) {
            char c = fgetc(fortune);
            if (feof(fortune)) break;
            fputc(c, fp);
        }
        pclose(fortune);
    }
    pclose(fp);
#endif
    fp = NULL;
}
