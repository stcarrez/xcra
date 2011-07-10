// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Note Management
// Author	: S. Carrez
// Date		: Sat Sep 16 19:46:55 1995
// Version	: $Id: Note.C,v 1.11 2000/02/23 08:22:17 ciceron Exp $
// Project	: Xcra
// Keywords	: Note
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
// class Note
// class NoteList
// class NoteScanCallback
//
#include "config.H"

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>

#include <sys/stat.h>

#if HAVE_MMAP
#  include <sys/mman.h>
#endif

#include "Assert.H"
#include "Error.H"
#include "Note.H"

// ----------------------------------------------------------------------
//
//			Note
//
// ----------------------------------------------------------------------

//
// Create a new note of type `_type' in the notes list `_list' and
// give it the title `_title'.
//
Note::Note(NoteList& _list, const char* _title, const NoteType _type)
: dlink(), nList(_list), nType(_type)
{
    dlist& d = nList;
    d.insert(this, (dlink*) NULL);

    nContent      = 0;
    nXpos	  = 0;
    nYpos         = 0;
    nWidth        = 1;
    nHeight       = 1;
    nTitle        = 0;
    nLastPosition = 0;
    nDisplayPosition = 0;
    nLength       = 0;
    nSubNotes     = 0;
    nFlags	  = 0;

    nMappedFile   = 0;
    nMappedSize   = 0;

    setTitle(_title);
}


//
// Delete the note and remove it from the list of notes.
//
Note::~Note()
{
    Require(isVisible() == 0);

    dlist& d = nList;
    d.remove(this);

    if (nContent != 0) {
	free((free_ptr) nContent);
    }

    if (nTitle != 0) {
	free((free_ptr) nTitle);
    }

	//
	// If this note is a sub-notes, we must delete the object if
	// we have no reference on it.
	//
    if (nSubNotes && --nSubNotes->nlUseCounter == 0) {
	delete nSubNotes;
    }
}


//
// Set the content of the note (or its path) to `_str'.
//
    int
Note::set(const char* _str)
{
    require(_str != 0);
    
	//
	// The note content represents a path. Make sure this path is
	// absolute by prepending the current directory if needed.
	//
    if (nType != GeneralNote && _str[0] != '/') {
	char* pwd = (char*) malloc(1024);
	if (pwd == (char*) NULL) {
	    return -1;
	}

	char* pwdRes = getwd(pwd);
	if (pwdRes == (char*) NULL) {
	    free((free_ptr) pwd);
	    return -1;
	}

	char* s = (char*) malloc(strlen(_str) + strlen(pwd) + 3);
	if (s == (char*) NULL) {
	    free((free_ptr) pwd);
	    return -1;
	}

	strcpy(s, pwd);
	strcat(s, "/");
	strcat(s, _str);
	free((free_ptr) pwd);
	_str = s;
    } else {
	_str = strdup(_str);

    }

    if (_str == (const char*) NULL) {
	return -1;
    }

    if (nContent != (char*) NULL) {
	free((free_ptr) nContent);
    }
    nContent = (char*) _str;
    nLength  = strlen(_str);
    return 0;
}


//
// Append the string `_str' to the content of the note.
//
    int
Note::append(const char* _str)
{
    require(_str != 0);
    
    if (nType != GeneralNote) {
	return -1;
    }

    int len = strlen(_str);

    char* p = (char*) malloc(len + nLength + 1);

    if (p == (char*) NULL) {
	return -1;
    }

    if (nContent != (char*) NULL) {
	memcpy(p, nContent, nLength);
	memcpy(&p[nLength], _str, len + 1);
	free((free_ptr) nContent);
	nContent = p;
	nLength  += len;
    } else {
	memcpy(p, _str, len + 1);
	nContent = p;
	nLength  = len;
    }
    return 0;
}


//
// Set the title.
//
    int
Note::setTitle(const char* _str)
{
    RequireMsg(_str != 0 && strlen(_str) < MAX_NOTE_TITLE_SIZE,
	       ("Title string `%s' is not valid.", _str));

    char* p = strdup(_str);

    if (p == (char*) NULL) {
	return -1;
    }

    if (nTitle != (char*) NULL) {
	free((free_ptr) nTitle);
    }
    nTitle = p;
    return 0;
}


//
// Save the note content and description in `_fp'.
//
    void
Note::save(FILE* _fp)
{
    char c;

    fputc(NOTE_CHAR_DELIMITER, _fp);
    switch (nType) {
	case GeneralNote :
	    c = 'G';
	    break;

	case ExternalNote :
	    c = 'E';
	    break;

	case SubNotes :
	    c = 'S';
	    break;

	default :
	    assertPrintf(1 == 0, ("Invalid note type %d", nType));
	    c = 'G';
	    break;
    }

    fputc(c, _fp);
    fprintf(_fp, "%d %d %d %d %ld %ld %s\n",
	    nXpos, nYpos, nWidth, nHeight, nLastPosition,
	    nDisplayPosition, nTitle);

    if (nContent != (char*) NULL) {
	fputs(nContent, _fp);
    }

    if (nLength == 0 || nContent[nLength-1] != '\n') {
	fputc('\n', _fp);
    }
}


//
// If the note is a sub-notes (type `SubNotes'), load the
// sub-notes in memory.
//
    int
Note::loadSubNotes()
{
    if (nSubNotes == (NoteList*) NULL) {
	nSubNotes = NoteList::findNotes(nContent);

	if (nSubNotes) {
	    nSubNotes->nlUseCounter ++;
	} else {
	    return -1;
	}
    }

    return nSubNotes->load();
}


//
// Map the external note in memory (read-only mode)
// (The note type must be `ExternalNote').
//
    int
Note::mapFile()
{
	//
	// The note represents a set of notes, there is no content for
	// such kind of notes. Deeply scanning is handled in NoteList::scan.
	//
    if (nType != ExternalNote) {
	return -1;
    }

    int fd;
    struct stat st;

	//
	// If the note content is in an external file, map the file
	// in our address space (read only). One reason to do this, is
	// to avoid to grow the size of the process by allocating
	// a large memory region and reading the file in it.
	//
    fd = open(nContent, O_RDONLY);
    if (fd < 0) {
	Error::report(MSG_LOADERROR, nContent);
	return E_OPEN_ERROR;
    }

    if (fstat(fd, &st) < 0) {
	int oerrno = errno;
	::close(fd);
	errno = oerrno;
	Error::report(MSG_LOADERROR, nContent);
	return E_OPEN_ERROR;
    }

    nMappedSize = st.st_size;

	//
	// Map the complete file in read-only mode.
	//
#if HAVE_MMAP
    nMappedFile = (char*) mmap(0, nMappedSize, PROT_READ, MAP_PRIVATE, fd, 0);
#else
    nMappedFile = malloc(st.st_size);
    if (nMappedFile) {
	if (read(fd, nMappedFile, st.st_size) != st.st_size) {
	    free((free_ptr) nMappedFile);
	    nMappedFile = 0;
	}
    }
#endif
    if (nMappedFile == (void*)(-1L) || nMappedFile == (void*) NULL) {
	int oerrno = errno;
	nMappedFile = 0;
	::close(fd);
	errno = oerrno;
	Error::report(MSG_MEMORY_ERROR, nContent);
	return E_OPEN_ERROR;
    }
    ::close(fd);

    return 0;
}


//
// Unmap the external note.
//
    int
Note::unMapFile()
{
    if (nMappedFile != 0) {
#if HAVE_MMAP
	munmap((char*) nMappedFile, nMappedSize);
#else
	free((free_ptr) nMappedFile);
#endif
	nMappedFile = 0;
	nMappedSize = 0;
    }
    return 0;
}


//
// Scan the note calling the callback function for each line.
// Stop when the callback tells to stop, or when the end of the
// note is reached.
//
    NoteScanResult
Note::scan(NoteScanCallback& _callback)
{
    NoteScanResult result = NoteScanContinue;

	//
	// The note represents a set of notes, there is no content for
	// such kind of notes. Deeply scanning is handled in NoteList::scan.
	//
    if (nType == SubNotes) {
	return result;
    }

    int fd;
    char* mapAddr = 0;
    const char* lineStart;
    struct stat st;
    long len;

	//
	// If the note content is in an external file, map the file
	// in our address space (read only). One reason to do this, is
	// to avoid to grow the size of the process by allocating
	// a large memory region and reading the file in it.
	//
    if (nType == ExternalNote) {
	if (nList.scanMode() == NoteScanLocal) {
	    return result;
	}

	fd = open(nContent, O_RDONLY);
	if (fd < 0) {
	    Error::report(MSG_LOADERROR, nContent);
	    return result;
	}

	if (fstat(fd, &st) < 0) {
	    Error::report(MSG_LOADERROR, nContent);
	    ::close(fd);
	    return result;
	}

	len = st.st_size;

		//
		// Map the complete file in read-only mode.
		//
#if HAVE_MMAP
	mapAddr = (char*) mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
#else
	mapAddr = (char*) malloc(st.st_size);
	if (mapAddr) {
	    if (read(fd, mapAddr, st.st_size) != st.st_size) {
		free((free_ptr) mapAddr);
		mapAddr = 0;
	    }
	}
#endif
	if (mapAddr == (char*)(-1L) || mapAddr == (char*) NULL) {
	     Error::report(MSG_MEMORY_ERROR, nContent);
	     ::close(fd);
	     return result;
	}
	::close(fd);

#if defined(HAVE_MMAP) && defined(MADV_SEQUENTIAL)
		//
		// Optimize the mapper since we know that we access
		// the file sequentially.
		//
	if (mapAddr != 0) {
	    madvise((void*) mapAddr, st.st_size, MADV_SEQUENTIAL);
	}
#endif
	lineStart = mapAddr;
    } else {
	lineStart = nContent;
	len       = nLength;
    }

	//
	// Call the first callback before scanning each line of the note.
	// This callback gets the whole note in one blow (useful for
	// a `show-note' operation).
	//
    result = _callback.noteScanStart(*this, lineStart, len);

    int line = 0;
    long pos = 0;
    int bufSize = 1024;
    char* buf;

    if (result == NoteScanContinue) {
	buf = (char*) malloc(bufSize);
	if (buf == (char*) NULL) {
	    Error::report(MSG_MEMORY_ERROR, "");
	    pos = len;
	}
    } else {
	if (result == NoteScanSkip) {
	    result = NoteScanContinue;
	}
	buf = (char*) NULL;
	pos = len;
    }

    while (pos < len) {
	char* p = buf;
	char c;

		//
		// Copy one line in the scratch buffer. Reallocate the
		// buffer if it becomes too small.
		//
	while (pos < len && (c = *lineStart++) != '\n') {
	    if (p == &buf[bufSize-2]) {
		int bpos = bufSize - 2;
		bufSize = bufSize + 1024;
		char* nbuf = (char*) realloc(buf, bufSize);
		if (nbuf == (char*) NULL) {
		    Error::report(MSG_MEMORY_ERROR, "");
		    pos = len;
		    break;
		}
		buf = nbuf;
		p = &buf[bpos];
	    }

	    *p++ = c;
	    pos ++;
	}

	pos++;
	*p++ = '\n';
	*p++ = 0;
	line ++;

		//
		// Check if the line matches the pattern.
		//
	result = _callback.apply(*this, buf, line);
	if (result != NoteScanContinue) {
	    break;
	}
    }

    if (buf != (char*) NULL) {
	free((free_ptr) buf);
    }

    if (nType == ExternalNote) {
#if HAVE_MMAP
	munmap(mapAddr, st.st_size);
#else
	free((free_ptr) mapAddr);
#endif
    }

    return result;
}


// ----------------------------------------------------------------------
//
//			NoteList
//
// ----------------------------------------------------------------------

static dlist noteFiles;

//
// Create a notes file.
//
NoteList::NoteList(const char* _file)
: dlist(), LockFile(0)
{
    nlFile 	    = strdup(_file);
    lfFile	    = nlFile;
    nlScanMode      = NoteScanLocal;
    nlScanTraversed = false;
    nlUseCounter    = 1;

    noteFiles.insert(this);
}


NoteList::~NoteList()
{
    Note *note;

    while ((note = first())) {
	delete note;
    }
    noteFiles.remove(this);

    if (nlFile) {
	free((free_ptr) nlFile);
    }
}


//
// Determine whether the notes file must be re-loaded (ie it
// has changed on the disk).
//
    bool
NoteList::needLoad() const
{
    struct stat st;

    if (stat(nlFile, &st) < 0) {
	return false;
    }

    if (st.st_mtime == nlLoadTime) {
	return false;
    }
    return true;
}


//
// Load the notes file, updating the list of notes.
//
    int
NoteList::load()
{
    int fd;
    struct stat st;

    fd = open(nlFile, O_RDONLY);
    if (fd < 0) {
	return E_OPEN_ERROR;
    }

    if (fstat(fd, &st) < 0) {
	int oerrno = errno;
	::close(fd);
	errno = oerrno;
	return E_OPEN_ERROR;
    }

    if (st.st_mtime == nlLoadTime) {
	::close(fd);
	return 0;
    }

	//
	// Read the file in one blow.
	//
    char* p = (char*) malloc(st.st_size + 1);
    if (p == (char*) NULL) {
	::close(fd);
	return E_OPEN_ERROR;
    }

    int res = ::read(fd, p, st.st_size);
    int oerrno = errno;
    ::close(fd);
    errno = oerrno;
    if (res != st.st_size) {
	free((free_ptr) p);
	return E_OPEN_ERROR;
    }

    p[st.st_size] = 0;

    int errors = 0;
    int line = 1;
    for (char* s = p; *s; ) {
	char c = *s++;
	NoteType type;

	if (c == '\n') {
	    line ++;
	}

	if (c != '\f') {
	    Error::printf(MSG_MISSING_NOTE, nlFile, line);
	    errors ++;
	    break;
	}

	c = *s++;
	switch (c) {
	    case 'G' :
		type = GeneralNote;
		break;

	    case 'E' :
		type = ExternalNote;
		break;

	    case 'S' :
		type = SubNotes;
		break;

	    default :
		Error::printf(MSG_BAD_NOTE_TYPE, nlFile, line, c);
		errors ++;
		continue;
	}

	char* t = strchr(s, '\n');
	if (t == (char*) NULL) {
	    Error::printf(MSG_BAD_NOTE_SPEC, nlFile, line);
	    errors ++;
	    break;
	}

	char title[MAX_NOTE_TITLE_SIZE * 2];
	if ((size_t) (t - s) > sizeof(title)) {
	    Error::printf(MSG_BAD_NOTE_SPEC, nlFile, line);
	    errors ++;
	    break;
	}

	int x, y;
	unsigned width, height;
	long pos, dpos;
	res = sscanf(s, "%d %d %d %d %ld %ld %[^\n]",
		     &x, &y, &width, &height, &pos, &dpos, title);

	if (res != 7 || strlen(title) > MAX_NOTE_TITLE_SIZE) {
	    Error::printf(MSG_BAD_NOTE_SPEC, nlFile, line);
	    errors ++;
	    break;
	}

	line ++;

	s = t + 1;
	t = strchr(s, '\f');
	if (t != (char*) NULL) {
	    *t = 0;
	}

	if (type != GeneralNote) {
	    char* p = s;

	    p = strchr(s, '\n');
	    if (p)
		*p = 0;
	}

		//
		// If a note existed with the title, remove it.
		//
	Note* n = find(title);
	if (n == (Note*) NULL) {
		//
		// Create the new note.
		//
	    n = new Note(*this, title, type);
	}

	if (n->set(s) != 0) {
	    // delete n;
	    errors ++;
	    Error::report(MSG_MEMORY_ERROR, "");
	    break;
	}

	n->setGeometry(x, y, width, height);
	n->setPosition(pos, dpos);

		//
		// Backtrack and continue with the next note.
		//
	if (t == (char*) NULL) {
	    break;
	}
	*t = '\f';
	s = t;
    }

    free((free_ptr) p);

    if (errors) {
	Error::printf(MSG_LOADERROR, nlFile);

	//
	// Since some notes could have been deleted, invalidate the
	// complete note list.
	//
	nlLoadTime = 0;
	return E_LOAD_ERROR;
    } else {
	nlLoadTime = st.st_mtime;
	return 0;
    }
}


//
// Save the notes file.
//
    int
NoteList::save()
{
    FILE* fp;
    char* tempName;

    if (lock() != LockOk) {
	Error::printf(MSG_FILE_LOCKED, nlFile);
	return -1;
    }

	//
	// The notes file is written in a temporary file (<file>~).
	// It will be moved to the final notes file when the save
	// operation succeeds.
	//
    tempName = (char*) malloc (strlen(nlFile) + 2);
    if (tempName == (char*) NULL) {
	Error::report(MSG_CANT_CREATE, nlFile);
	unlock();
	return -1;
    }

    sigset_t oset;
    sigset_t set;
    
    sigfillset(&set);
    sigprocmask(SIG_BLOCK, &set, &oset);

    sprintf(tempName, "%s~", nlFile);
    fp = fopen(tempName, "w");
    if (fp == (FILE*) NULL) {
	Error::report(MSG_CANT_CREATE, nlFile);
	sigprocmask(SIG_SETMASK, &oset, 0);
	free((free_ptr) tempName);
	unlock();
	return E_OPEN_ERROR;
    }

	//
	// Save each note in turn.
	//
    for (Note* note = first(); note; note = note->next()) {
	note->save(fp);
    }

	//
	// If this was correct, replace the original with the
	// temporary file (this should be atomic if `rename' exists).
	//
    int result = fclose(fp);
    if (result == 0) {
	result = rename(tempName, nlFile);
	if (result != 0) {
	    Error::printf(MSG_RENAME_FAILED, tempName, nlFile);
	    Error::printDiagnostic(nlFile);
	}
    }
    if (result != 0) {
	int oerrno = errno;
	(void) unlink(tempName);
	errno = oerrno;
    }
    sigprocmask(SIG_SETMASK, &oset, 0);
    unlock();
    free((free_ptr) tempName);

    return result;
}


//
// Find a note given its title.
//
    Note*
NoteList::find(const char* _title)
{
    for (Note* note = first(); note; note = note->next()) {
	if (strcmp(note->title(), _title) == 0) {
	    return note;
	}
    }
    return (Note*) NULL;
}


//
// Create a new note, checking if a previous note with the title
// existed before, and assign it a content string. If the note
// type is a file, the current directory is prepended to the
// path `_content' if it does not start with /.
//
    Note*
NoteList::create(const char* _title, NoteType _type, const char* _content)
{

    int result = load();

    if (result != 0 && result != E_OPEN_ERROR) {
	return (Note*) NULL;
    }

    Note* note = find(_title);
    if (note != (Note*) NULL) {
	return (Note*) NULL;
    }

    note = new Note(*this, _title, _type);

    if (_content != (const char*) NULL && note->set(_content) != 0) {
	delete note;
	return (Note*) NULL;
    }
    return note;
}


//
// Delete the note whose title is `_title'
// and save the notes file.
//
    int
NoteList::delNote(const char* _title)
{
    int result = 0;

	//
	// Be sure the file is locked.
	//
    if (lock() != LockOk) {
	Error::printf(MSG_FILE_LOCKED, nlFile);
	return -1;
    }

	//
	// See if the file has changed. If we already have a lock,
	// no change was possible.
	//
    if (LockFile::count() == 1) {
	result = load();
	if (result != 0) {
	    unlock();
	    return result;
	}
    }

	//
	// Find the Note object and delete it.
	//
    Note* note = find(_title);
    if (note != (Note*) NULL) {
	if (note->isVisible() != 0) {
	    Error::printf(MSG_NOTE_BEING_EDITED, _title);
	    Error::printf(MSG_CANT_DELETENOTE, _title);
	    result = -1;
	} else {
	    delete note;

	    result = save();
	}
    }
    unlock();
    return result;
}


//
// Scan the notes file calling the callback functions for each note.
// Stop when the callback tells to stop, or when the end of the
// notes file is reached.
//
    NoteScanResult
NoteList::scan(NoteScanMode _mode, NoteScanCallback& _callback)
{
	//
	// This note file was already scanned, don't scan it another
	// time (The note files altogether may represent a cyclic graph).
	//
    if (nlScanTraversed == true) {
	return NoteScanContinue;
    }

    nlScanMode      = _mode;
    nlScanTraversed = true;

	//
	// Scan each note in turn, stopping when we are told to do so.
	//
    for (Note* note = first(); note; note = note->next()) {

		//
		// Apply the callback on the current note.
		//
	NoteScanResult result = _callback.apply(*note);
	if (result != NoteScanContinue) {
	    return result;
	}

		//
		// Deeply scan the set of notes if needed.
		//
	if (note->type() == SubNotes && _mode == NoteScanDeep
	    && note->loadSubNotes() == 0) {

	    result = _callback.deepScanStart(*note);
	    if (result == NoteScanSkip) {
		continue;
	    }
	    if (result != NoteScanContinue) {
		return result;
	    }
	    result = note->nSubNotes->scan(_mode, _callback);
	    if (result != NoteScanContinue) {
		return result;
	    }

	    result = _callback.deepScanStop(*note);
	    if (result != NoteScanContinue) {
		return result;
	    }
	}
    }

    return NoteScanContinue;
}


//
// Recursively clear the traveral mark for each notes file.
// The traversal marks are used to detect cycles by the scan
// operation. Before a scan operation can be executed, all the
// traversal marks should be cleared.
//
    void
NoteList::clearTraversal()
{
    nlScanTraversed = false;

	//
	// Scan each note in turn, stopping when we are told to do so.
	//
    for (Note* note = first(); note; note = note->next()) {
	if (note->type() != SubNotes)
	    continue;

	note->clearSubTraversal();
    }
}


//
// Find a list of notes given its path.
//
    NoteList*
NoteList::findNotes(const char* _file)
{
	//
	// Find the note file in core first.
	//
    NoteList* notes;
    for (notes = (NoteList*)noteFiles.first();
	 notes; notes = notes->next()) {

	if (strcmp(notes->file(), _file) == 0) {
	    return notes;
	}
    }

	//
	// If the file does not exist, don't try to create it
	// but return an error
	//
    if (access(_file, R_OK) != 0) {
	return (NoteList*) NULL;
    }

	//
	// Load the new note file
	//
    notes = new NoteList(_file);
    if (notes->load() != 0) {
	delete notes;
	return (NoteList*) NULL;
    }

    return notes;
}


// ----------------------------------------------------------------------
//
//			NoteScanCallback
//
// ----------------------------------------------------------------------

NoteScanCallback::NoteScanCallback()
{
    nscCurrentDepth = 0;
}


NoteScanCallback::~NoteScanCallback()
{
}


//
// First callback executed on the note `_note' and with the
// complete note content passed in `_buf' and `_size'. External
// notes are mapped in memory (if possible).
//
    NoteScanResult
NoteScanCallback::noteScanStart(Note&, const char*, long)
{
    return NoteScanContinue;
}


//
// Callback executed on each line of the note (if `noteScanStart'
// returned `NoteScanContinue'). Each line are passed in
// `_buf' (null terminated). The line number is passed in `_line'.
//
    NoteScanResult
NoteScanCallback::apply(Note&, const char*, int)
{
    return NoteScanContinue;
}


//
// Apply an operation on the note.
//
    NoteScanResult
NoteScanCallback::apply(Note& _note)
{
    return _note.scan(*this);
}


//
// Start a deep scan of the note file.
//
    NoteScanResult
NoteScanCallback::deepScanStart(Note&)
{
    nscCurrentDepth ++;
    return NoteScanContinue;
}


//
// End a deep scan initiated on the note file.
//
    NoteScanResult
NoteScanCallback::deepScanStop(Note&)
{
    nscCurrentDepth --;
    return NoteScanContinue;
}

