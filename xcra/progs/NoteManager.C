// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Note Manager Tool
// Author	: S. Carrez
// Date		: Sun Sep 17 11:46:09 1995
// Version	: $Id: NoteManager.C,v 1.11 2000/02/23 08:24:01 ciceron Exp $
// Project	: xcra
// Keywords	: Note, Tool
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
// class NoteManager	Small Notes manager (create, remove, list, grep)
// 
//
#include "config.H"
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>

#define _REGEX_RE_COMP

#if HAVE_REGEX_H
#ifdef linux
extern "C" {
#endif
#  include <regex.h>
#ifdef linux
};
#endif
#endif
#include <regex.h>
#if HAVE_RE_COMP_H
#  include <re_comp.h>
#endif

#if HAVE_GETOPT_H
#  include <getopt.h>
#endif

#include "Assert.H"
#include "Error.H"
#include "NoteManager.H"

#ifndef HAVE_RE_EXEC
static char* rcomp;

static char*
re_comp(char* pat)
{
    rcomp = regcmp(pat, (char*) NULL);
    return 0;
}

static int
re_exec(char* match)
{
    char* r = regex(rcomp, match);
    return (r == 0) ? 0 : 1;
}
#endif

// ----------------------------------------------------------------------
//
//			NoteManager
//
// ----------------------------------------------------------------------

NoteManager::NoteManager()
{
    nmPrintTitle 	= 1;
    nmPrintGeometry	= 0;
    nmPrintFile		= 1;
    nmReadStdin		= 0;
    nmScanOperation	= NoOp;
    nmNoteTitle		= 0;
    nmNoteFile		= 0;
    nmNoteSearched	= 0;
    nmDefaultX		= 0;
    nmDefaultY		= 0;
    nmDefaultWidth  	= 0;
    nmDefaultHeight 	= 0;
    nmNoteType		= GeneralNote;
}

NoteManager::~NoteManager()
{
}


    void
NoteManager::error(const char* _msg, ...)
{
    fprintf(stderr, "NoteManager: ");

    va_list argp;
    va_start(argp, _msg);
    vfprintf(stderr, _msg, argp);
    va_end(argp);
}

//
// Print the information about the note `_note'.
//

    void
NoteManager::print(Note& _note)
{
    char buf[64];

    for (int column = 0; column < nscCurrentDepth * 2; column++) {
	fputc(' ', stdout);
    }

    if (nmPrintTitle) {
	printf("%-40.40s ", _note.title());
    }

    if (nmPrintGeometry) {
	sprintf(buf, "%dx%d+%d+%d ", _note.width(), _note.height(),
		_note.x(), _note.y());

	printf("%-20.20s ", buf);
    }

    if (nmPrintFile) {
	if (_note.type() == ExternalNote || _note.type() == SubNotes) {
	    strncpy(buf, _note.content(), sizeof(buf));
	    buf[sizeof(buf) - 1] = 0;
	} else {
	    buf[0] = 0;
	}
	printf("%s", buf);
    }

    printf("\n");
}


//
// Start a scan operation on the note. The complete content
// of the note is passed in `_buf'. The `ShowNote' operation
// will dump the note content on stdout.
//
    NoteScanResult
NoteManager::noteScanStart(Note&, const char* _buf, long _size)
{
    RequireMsg(nmScanOperation == ShowNote || nmScanOperation == GrepNote,
	       ("Invalid operation %d", nmScanOperation));

	//
	// The Grep operation needs that `apply' be called for each
	// line of the note.
	//
    if (nmScanOperation != ShowNote) {
	return NoteScanContinue;
    }

	//
	// The Show operation dumps the note content in one blow and
	// notifies that there is no need to scan each line in turn.
	//
    fwrite(_buf, 1, _size, stdout);
    return NoteScanSkip;
}


//
// Apply an operation on a single line of the note. (Grep).
//
    NoteScanResult
NoteManager::apply(Note& _note, char* _buf, int _line)
{
    RequireMsg(nmScanOperation == GrepNote,
	       ("Invalid operation %d", nmScanOperation));

    if (re_exec(_buf) == 0) {
	return NoteScanContinue;
    }

    printf("%s:%d: %s", _note.title(), _line, _buf);
    return NoteScanContinue;
}


//
// Apply an operation on a note.
//
    NoteScanResult
NoteManager::apply(Note& _note)
{
    if (nmNoteTitle != (char*) NULL
	&& strcmp(nmNoteTitle, _note.title()) != 0) {
	return NoteScanContinue;
    }

    switch (nmScanOperation) {
		//
		// Need to scan the content of the note.
		//
	case GrepNote :
	case ShowNote :
		return _note.scan(*this);

		//
		// Found the note which matches `nmNoteTitle'.
		//
	case AppendNote :
	case ReplaceNote :
	case DeleteNote :
		nmNoteSearched = &_note;
		return NoteScanFound;

		//
		// Just print the information about the note.
		//
	case ListNote :
		print(_note);
		break;

	default :
		break;
    }

    return NoteScanContinue;
}


//
// Append to the content of the note `_note', either
// the arguments passed in `_argc, _argv', or the
// standard input.
//
    int
NoteManager::fillNote(Note* _note, int _argc, char* _argv[])
{
    if (_argc != 0) {
	for (int i = 0; i < _argc; i++) {
	    if (_note->append(_argv[i]) != 0) {
		return -1;
	    }

	    if (i + 1 < _argc) {
		if (_note->append(" ") != 0) {
		    return -1;
		}
	    }
	}
    } else if (_note->type() == GeneralNote) {
	int bufSize = 1024;
	char* buf = (char*) malloc(bufSize);

	int pos = 0;
	while (!feof(stdin)) {
	    char c = fgetc(stdin);
	    if (c == EOF)
		break;

	    if (pos == bufSize - 1) {
		buf[pos] = 0;
		if (_note->append(buf) != 0) {
		    free((free_ptr) buf);
		    return -1;
		}
		pos = 0;
	    }
	    buf[pos++] = c;
	}
	buf[pos] = 0;
	if (_note->append(buf) != 0) {
	    free((free_ptr) buf);
	    return -1;
	}
	free((free_ptr) buf);
    }

    return 0;
}


    void
NoteManager::usage()
{

    fprintf(stderr, "Usage: %s [-acrlhsxGR] [-d title] [-f file] [-t title] [-g regex]\n"
#if HAVE_GETOPT_LONG
	"	[--create title] [--append title] [--replace title] [--delete title]\n"
	"	[--list] [--show] [--grep regex] [--recursive] [--file file]\n"
	"	[--title title] [--external title] [--subnotes] [--help]\n"
	"       [--geometry]"
#endif
	"[args]", "noteManager");

    exit(1);
}


    void
NoteManager::help()
{
#if HAVE_GETOPT_LONG
    fputs("Choose one of the following option:\n"
"-c title,\n"
"--create title	 Create a new note entry point and fill it\n"
"-a title,\n"
"--append title  Append a text to an existing note\n"
"-r title,\n"
"--replace title Replace the text of an existing note\n"
"-S title,\n"
"--subnotes title Insert a subnote entry\n"
"-x title,\n"
"--external title Insert an external file entry\n"
"-d title,\n"
"--delete title  Delete a note given its title\n"
"-l, --list      List the available notes\n"
"-s, --show      Show all or a specific note\n", stdout);

    fputs("\nOther options:\n"
"-R, --recursive Recursively lists/shows/greps all the note files\n"
"-G, --geometry  List the geometry of the note\n"
"-h, --help      Print some help\n"
"-t title,\n"
"-title title	 Specifies the title of the note file\n"
"-f file,\n"
"--file file     Specifies the notes file\n",

	stdout);
#else
    fputs("Choose one of the following option:\n"
"-c title   Create a new note entry point and fill it\n"
"-a title   Append a text to an existing note\n"
"-r title   Replace the text of an existing note\n"
"-S title   Insert a subnote entry\n"
"-x title   Insert an external file entry\n"
"-d title   Delete a note given its title\n"
"-l         List the available notes\n"
"-s         Show all or a specific note\n", stdout);

    fputs("\nOther options:\n"
"-R         Recursively lists/shows/greps all the note files\n"
"-G         List the geometry of the note\n"
"-h         Print some help\n"
"-t title   Specifies the title of the note file\n"
"-f file    Specifies the notes file\n",

	stdout);
#endif

    exit(1);
}


#if HAVE_GETOPT_LONG
struct option long_options[] = {
 { "create",	required_argument,	0,	'c'	},
 { "append",	required_argument,	0,	'a'	},
 { "replace",	required_argument,	0,	'r'	},
 { "delete",	required_argument,	0,	'd'	},
 { "grep",	required_argument,	0,	'g'	},
 { "list",	no_argument,		0,	'l'	},
 { "show",	no_argument,		0,	's'	},
 { "external",	required_argument,	0,	'x'	},
 { "subnotes",	required_argument,	0,	'S'	},

 { "recursive",	no_argument,	   	0,	'R'	},

 { "file",	required_argument, 	0,	'f'	},
 { "title",	required_argument, 	0,	't'	},
 { "geometry",	no_argument, 		0,	'G'	},
 { "help",	no_argument, 		0,	'h'	}
};
#endif

    int
NoteManager::main(int argc, char** argv)
{
#ifndef HAVE_GETOPT_LONG
    extern char* optarg;
    extern int optind;
#endif

    int c;
    char* regex = 0;
    const char* msg;
    NoteScanMode nmScanMode = NoteScanExternal;

#if HAVE_GETOPT_LONG
    int ind = -1;

    while ((c = getopt_long(argc, argv, "hlsGRa:c:d:e:f:g:r:t:x:S:",
			    long_options, &ind)) != EOF) {
#else
    while ((c = getopt(argc, argv, "hlsGRa:c:d:e:f:g:r:t:x:S:")) != EOF) {
#endif
	switch (c) {
	    case 0 :	// long option that set a single flag.
		break;

	    case 'a' :
		nmScanOperation = AppendNote;
		nmNoteTitle = optarg;
		break;

	    case 'c' :
		nmScanOperation = CreateNote;
		nmNoteTitle = optarg;
		break;

	    case 'd' :
		nmScanOperation = DeleteNote;
		nmNoteTitle = optarg;
		break;

	    case 'f' :
		nmNoteFile = optarg;
		break;

	    case 'g' :
	    case 'e' :
		nmScanOperation = GrepNote;
		regex = optarg;
		break;
	
	    case 'r' :
		nmScanOperation = ReplaceNote;
		nmNoteTitle = optarg;
		break;

	    case 't' :
		nmNoteTitle = optarg;
		break;

	    case 'x' :
		nmScanOperation = CreateNote;
		nmNoteType = ExternalNote;
		nmNoteTitle = optarg;
		break;

	    case 'S' :
		nmScanOperation = CreateNote;
		nmNoteType = SubNotes;
		nmNoteTitle = optarg;
		break;

	    case 'h' :
		help();

	    case 'l' :
		nmScanOperation = ListNote;
		break;

	    case 's' :
		nmScanOperation = ShowNote;
		break;

	    case 'G' :
		nmPrintGeometry = 1;
		break;

	    case 'R' :
		nmScanMode = NoteScanDeep;
		break;

	    default :
		usage();
	}
    }

	//
	// An operation option must be specified.
	//
    if (nmScanOperation == NoOp) {
	usage();
    }

	//
	// If no note file was specified, use the default one in the
	// home directory.
	//
    if (nmNoteFile == (char*) NULL) {
	const char* home = getenv("HOME");

	if (home == (const char*) NULL) {
	    error("Impossible to get the home directory\n");
	    return 1;
	}

	char* s = (char*) malloc(strlen(home) + sizeof("/.notes"));
	sprintf(s, "%s/.notes", home);
	nmNoteFile = s;
    }

    NoteList notes(nmNoteFile);

	//
	// Create a new note
	//
    if (nmScanOperation == CreateNote) {
	if (notes.lock() != LockOk) {
	     error("Impossible to lock the notes file\n");
	     return 1;
	}
	Note* note = notes.create(nmNoteTitle, nmNoteType, argv[optind]);
	if (note == (Note*) NULL) {
	     error("Cannot create the note `%s'\n", nmNoteTitle);
	     notes.unlock();
	     return 1;
	}

	note->setGeometry(nmDefaultX, nmDefaultY,
			  nmDefaultWidth, nmDefaultHeight);

		//
		// Fill the note content
		//
	optind ++;
	if (fillNote(note, argc - optind, &argv[optind]) != 0) {
	     error("Not enough memory\n");
	     notes.unlock();
	     return 1;
	}

	if (notes.save() != 0) {
	     error("Cannot save note file\n");
	     notes.unlock();
	     return 1;
	}
	notes.unlock();
	return 0;
    }

    if (nmScanOperation == DeleteNote || nmScanOperation == AppendNote
	|| nmScanOperation == ReplaceNote) {
	if (notes.lock() != LockOk) {
	    error("Impossible to lock the note file\n");
	    return 1;
	}
    }

    if (notes.load() != 0) {
	error("Impossible to load the note file\n");
	return 1;
    }

    if (nmScanOperation == GrepNote) {
	msg = re_comp(regex);
	if (msg != (char*) NULL) {
	    error("%s\n", msg);
	    return 1;
	}
    }

    notes.scan(nmScanMode, *this);

    switch (nmScanOperation) {
	case DeleteNote :
		if (nmNoteSearched == (Note*) NULL) {
		    error("Note `%s' not found\n", nmNoteTitle);
		    return 1;
		}

		if (notes.delNote(nmNoteSearched->title()) != 0) {
		    error("Impossible to delete the note.\n");
		}
		notes.unlock();
		break;

	case AppendNote :
	case ReplaceNote :
		if (nmNoteSearched == (Note*) NULL) {
		    error("Note `%s' not found\n", nmNoteTitle);
		    return 1;
		}

		if (nmScanOperation == ReplaceNote) {
		    nmNoteSearched->set("");
		}
		if (fillNote(nmNoteSearched, argc - optind,
			     &argv[optind]) != 0) {
		    error("Not enough memory\n");
		    return 1;
		}

		if (notes.save() != 0) {
		    error("Impossible to save\n");
		    return 1;
		}
		notes.unlock();
		break;

	default :
		break;
    }

    return 0;
}

    int
main(int argc, char* argv[])
{
    NoteManager mgr;

    return mgr.main(argc, argv);
}
