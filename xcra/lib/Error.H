// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Error Management
// Author	: S. Carrez
// Date		: Sun Mar  5 23:05:08 1995
// Version	: $Id: Error.H,v 1.13 2000/03/18 10:56:08 ciceron Exp $
// Project	: xcra
// Keywords	: Errors, Report
//
// Copyright (C) 1995, 1996, 2002 Stephane Carrez
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
#ifndef _ERROR_H
#define _ERROR_H

#include <stdio.h>

#define MSG_MISSING_EQUAL	"%s:%d: Missing `='\n"
#define MSG_BAD_REFERENCE 	"%s:%d: Missing reference name\n"
#define MSG_BAD_NAME		"%s:%d: Missing task name\n"
#define MSG_BAD_NOTE		"%s:%d: Missing note file path\n"
#define MSG_UNKNOWN_DIRECTIVE	"%s:%d: Unknown directive `%c'\n"
#define MSG_SKIP_ENDLINE	"%s:%d: Skipping to end of line\n"
#define MSG_CANT_ADD_TASK	"%s:%d: Too many tasks, cannot create task\n"
#define MSG_BAD_TIME_FORMAT	"%s:%d: Bad format for the time\n"
#define MSG_LOADERROR		"Failed to load the file `%s'\n"

#define MSG_CANT_CREATE		"Impossible to create the file `%s'\n"
#define	MSG_CANT_REOPEN		"Impossible to reopen the file `%s'\n"
#define	MSG_RENAME_FAILED	"Cannot rename file `%s' into `%s'\n"
#define MSG_SAVE_ERROR		"Failed to save the file `%s'\n"
#define MSG_TASK_FILE_MISSING	"Task file for %s %d does not exist\n"

#define MSG_WRONG_TIME_SPEC	"Wrong time specification"
#define MSG_TASK_EXISTS		"This task exists already.\n"
#define MSG_WRONG_DATE		"This date is not valid.\n"
#define MSG_EMPTY_FIELD		"%s is empty.\n"
#define MSG_WRONG_FIELD		"%s contains invalid characters.\n"
#define MSG_LONG_FIELD		"%s is too long.\n"
#define MSG_IMPORTING_TASKS	"Importing the tasks, please wait...\n"

#define MSG_TASKLIST_FULL	"Too many tasks\n"
#define MSG_NOTASK		"No task with such a name: %s\n"

#define MSG_NOCRAFILE		"No cra file found in the directory `%s'\n"
#define MSG_BADTIME_FORMAT	"Bad time specification `%s'\n"
#define MSG_CANT_SUBSTRACT	"Cannot substract\n"
#define	MSG_HOSTNAME_ERROR	"Impossible to get the host name\n"
#define MSG_TOO_MANY_DIALOGS	"Take care, you have opened %ld dialog boxes\n"

#define MSG_SUM_MONTH_NON_EMPTY	"The task `%s' cannot be deleted.\n\
It has one or several days in the month which report a time.\n\
The sum reported is %ld seconds."

#define MSG_SUM_FORTNIGHT_NON_EMPTY "The task `%s' cannot be deleted.\n\
It has one or several days in the fortnight which report a time.\n\
The sum reported is %ld seconds."

#define MSG_CANT_DELETE_CURRENT	"Impossible to delete the current task"

#define MSG_LAUNCH_IN_ACTION	"A command is currently running\n"
#define	MSG_LAUNCH_ERROR	"Problems while launching the command:\n%s\n"

#define MSG_MISSING_NOTE	"%s:%d: Missing note separator\n"
#define MSG_BAD_NOTE_TYPE	"%s:%d: Bad note type `%c'\n"
#define MSG_BAD_NOTE_SPEC	"%s:%d: Bad note specification\n"
#define MSG_MEMORY_ERROR	"Virtual memory exhausted.\n"

#define MSG_NOTE_EXIST		"The note `%s' exists already.\n"
#define MSG_FILE_LOCKED		"File `%s' is locked.\n"
#define MSG_CANT_DELETE_NOTE	"Impossible to remove the note `%s'\n"
#define MSG_NOTE_BEING_EDITED	"The note `%s' is being edited.\n"
#define MSG_CANT_DELETENOTE	"Impossible to remove the note `%s'.\n"
#define MSG_NO_HELP_FILE	"No help file found `%s'.\n"
#define MSG_NO_HELP		"No help found on `%s'.\n"

#define DIAG_CHECK_PATH		"You should probably check the following path:\n`%s'\n"
#define DIAG_EXEC_CMD		"You should probably check the command that"\
"you wanted Xcra to execute for you.\n"
#define DIAG_SYS_LIMIT		"Some system resources are exceeded.\n"
#define DIAG_BUG_ERROR		"Please submit a bug report to `Stephane.Carrez@worldnet.fr'.\n"
#define DIAG_IO_ERROR		"Please, report this error to the system administrator.\n"
#define DIAG_RETRY		"Wait a little and try the operation again.\n"
#define DIAG_NOSPC		"Do some cleanup on the file system.\n"

//
// Error codes
//
#define	E_NO_TASK	-1	// No task of given name
#define	E_EXIST		-2	// Task exists
#define	E_FULL		-3	// Too many tasks
#define	E_CURRENT	-4	// Current task
#define	E_NOT_EMPTY	-5	// Task charge not empty
#define	E_OPEN_ERROR	-6	// Open error
#define	E_WRITE_ERROR	-7	// Write error (or saving error)
#define	E_LOAD_ERROR	-8	// Read error (or load error)
#define	E_LOAD_DONE	-9	// Load already done
#define	E_BAD_TIME	-10	// Bad time value string
#define	E_CANT_MODIFY	-11	// Cant modify time

// ----------------------------------------------------------------------
// Class :	Error
//
// Role :	Management of errors.
//
class Error {
public:
	//
	// Print a message using the current error handler.
	//
   static void printf(const char* _msg, ...);

	//
	// Report an error message with a diagnostic.
	//
   static void report(const char* _msg, const char* _file);

	//
	// Find and print a diagnostic message after an error
	// (Well, this is not far from an expert system).
	//
   static void printDiagnostic(const char* _file);
   
	//
	// Notify that the operation is finished and that it's now
	// time to report (flush) the messages (if needed).
	//
   static void finish();
};


// ----------------------------------------------------------------------
// Class :	ErrorHandler
//
// Role :	Abstract class to represent an error handler.
//
class ErrorHandler {
public:
    ErrorHandler();
    virtual ~ErrorHandler();

	//
	// Report a message.
	//
    virtual void report(const char* _msg) = 0;

    virtual void finish();
};


// ----------------------------------------------------------------------
// Class :	FileErrorHandler
//
// Role :	Error handler to report errors in a file.
//
class FileErrorHandler : public ErrorHandler {
protected:
    FILE*	fp;
public:
    FileErrorHandler();
    ~FileErrorHandler();

    void report(const char* _msg);
};


// ----------------------------------------------------------------------
// Class :	MailErrorHandler
//
// Role :	Error handler to report errors via e-mail.
//
class MailErrorHandler : public FileErrorHandler {
protected:
    void open();
public:
    MailErrorHandler();
    ~MailErrorHandler();

    void report(const char* _msg);

    void finish();
};

#endif
