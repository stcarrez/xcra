// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Note Management
// Author	: S. Carrez
// Date		: Sat Sep 16 19:23:47 1995
// Version	: $Id: Note.H,v 1.8 1996/08/04 15:36:49 gdraw Exp $
// Project	: Xcra
// Keywords	: Note
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
// class Note			Management of a note (inside a notes file)
// class NoteList		Management of a notes file (set of notes)
// class NoteScanCallback	Callback object to scan the notes
//
//
#ifndef _NOTE_H_
#define _NOTE_H_

#include <stdio.h>
#include "dlist.H"
#include "LockFile.H"

#define MAX_NOTE_TITLE_SIZE	80
#define NOTE_CHAR_DELIMITER	'\f'

#define NOTE_VISIBLE	0x01

enum NoteType {
		//
		// The note content is part of the note file
		// (the nContent member holds the note content).
		//
    GeneralNote,

		//
		// The note content is in an external file whose path
		// is provided by the user. (the nContent member holds
		// the path).
		//
    ExternalNote,

		//
		// The note content is a set of notes. The set of sub
		// notes is a note file whose path is provided by the
		// user.
		//
    SubNotes
};


enum NoteScanResult {
		//
		// Continue to scan the notes.
		//
    NoteScanContinue,

		//
		// Stop the scan operation assuming that the note was found.
		//
    NoteScanFound,

		//
		// Stop the scan operation with an error.
		//
    NoteScanAbort,

		//
		// The note couldn't be scanned due to an error.
		//
    NoteScanError,

		//
		// Don't scan the note (or sub-notes) but continue with
		// the next one.
		//
    NoteScanSkip
};

enum NoteScanMode {
		//
		// Scan only the notes defined in the notes file.
		//
    NoteScanLocal,

		//
		// Also scan the external notes.
		//
    NoteScanExternal,

		//
		// Also scan the sub-notes (recursively).
    NoteScanDeep
};


class NoteList;
class Note;
class NoteScanCallback;


// ----------------------------------------------------------------------
// Class :	Note
//
// Role :	Management of a note inside a notes file
//
// The Note class manages a single note in a note file (set of notes).
// The note content can be part of the note file (GeneralNote) or 
// may be saved in an external file (ExternalNote) whose path is given
// by the user. The class Note records everything that can be saved
// in the note file (geometry, title, content).
//
class Note : private dlink {
	friend class NoteList;
private:
    Note(const Note&);
    void operator=(const Note&);
protected:
		//
		// The list this note belongs to.
		//
    NoteList&   nList;

		//
		// Note content or path (null-terminated raw buffer).
		//
    char*	nContent;
    int		nLength;

		//
		// Window geometry of the note when popped up.
		//
    int		nXpos;
    int		nYpos;
    unsigned	nWidth;
    unsigned	nHeight;

		//
		// Window associated title.
		//
    const char*	nTitle;

		//
		// Last position when looking at the note.
		//
    long	nLastPosition;
    long	nDisplayPosition;

		//
		// The type of the note.
		//
    NoteType    nType;

		//
		// The sub-notes if the current note is a sub-notes entry point
		//
    NoteList*	nSubNotes;

		//
		// Flags (popped-up, modified,...).
		//
    int		nFlags;

		//
		// Area where external files are mapped in memory
		// in read-only mode (useful to consult large files).
		//
    void*	nMappedFile;
    long	nMappedSize;

	//
	// Save the note content and description in `_fp'.
	//
    void save(FILE* _fp);

	//
	// Create a new note.
	//
    Note(NoteList& _list, const char* _title, const NoteType _type);
    ~Note();

    inline void clearSubTraversal();

public:
    inline Note* next() const;

	//
	// Return the type of the note.
	//
    inline NoteType type() const;

	//
	// Return the window geometry when the note is popped up.
	//
    inline int x() const;
    inline int y() const;
    inline unsigned width() const;
    inline unsigned height() const;
    inline long position() const;
    inline long displayPosition() const;

	//
	// Return true if a popup window is associated with this note.
	//
    inline bool isVisible() const;
    inline void setVisible();
    inline void clearVisible();

	//
	// Set the window geometry and editor's positions.
	//
    inline void setGeometry(int _x, int _y, unsigned _w, unsigned _h);
    inline void setPosition(long _pos, long _dpos);

	//
	// Return the notes list.
	//
    inline NoteList& notes();

	//
	// Return the title associated with the note.
	//
    inline const char* title() const;

	//
	// Return the content of the note (or its path).
	//
    inline const char* content() const;

	//
	// Set the content of the note (or its path) to `_str'.
	//
    int set(const char* _str);

	//
	// Append the string `_str' to the content of the note.
	//
    int append(const char* _str);

	//
	// Set the title.
	//
    int setTitle(const char* _title);

	//
	// Scan the note calling the callback function for each line.
	// Stop when the callback tells to stop, or when the end of the
	// note is reached.
	//
    NoteScanResult scan(NoteScanCallback& _callback);

	//
	// If the note is a sub-notes (type `SubNotes'), load the
	// sub-notes in memory.
	//
    int loadSubNotes();

	//
	// Save the complete note file which holds this note.
	//
    inline int saveList();

	//
	// Map the external note in memory (read-only mode)
	// (The note type must be `ExternalNote').
	//
    int mapFile();

	//
	// Unmap the external note.
	//
    int unMapFile();

	//
	// Return the map-address and map-region size.
	//
    inline void* mapAddr() const;
    inline long mapSize() const;
};


    inline Note*
Note::next() const
{
    return (Note*) dlink::next();
}

    inline NoteType
Note::type() const
{
    return nType;
}

    inline int
Note::x() const
{
    return nXpos;
}

    inline int
Note::y() const
{
    return nYpos;
}

    inline unsigned
Note::width() const
{
    return nWidth;
}

    inline unsigned
Note::height() const
{
    return nHeight;
}

    inline long
Note::position() const
{
    return nLastPosition;
}

    inline long
Note::displayPosition() const
{
    return nDisplayPosition;
}

    inline bool
Note::isVisible() const
{
    return (nFlags & NOTE_VISIBLE) ? true : false;
}

    inline void
Note::setVisible()
{
    nFlags |= NOTE_VISIBLE;
}

    inline void
Note::clearVisible()
{
    nFlags &= ~NOTE_VISIBLE;
}

    inline void
Note::setGeometry(int _x, int _y, unsigned _w, unsigned _h)
{
    nXpos   = _x;
    nYpos   = _y;
    nWidth  = _w;
    nHeight = _h;
}

    inline void
Note::setPosition(long _pos, long _dpos)
{
    nLastPosition    = _pos;
    nDisplayPosition = _dpos;
}

    inline NoteList&
Note::notes()
{
    return nList;
}

    inline const char*
Note::title() const
{
    return nTitle;
}

    inline const char*
Note::content() const
{
    return nContent;
}

    inline void*
Note::mapAddr() const
{
    return nMappedFile;
}

    inline long
Note::mapSize() const
{
    return nMappedSize;
}


// ----------------------------------------------------------------------
// Class :	NoteList
//
// Role :	Manages a notes file. A notes file represents a list of notes.
//
//
class NoteList : public dlist, private dlink, public LockFile {
	friend class Note;
protected:
		//
		// The file which records the descriptions and
		// contents of all the notes.
		//
    const char*		nlFile;

		//
		// The scan mode used by the scan operation.
		//
    NoteScanMode	nlScanMode;

		//
		// true if the notes file was scanned already.
		//
    bool		nlScanTraversed;

		//
		// Number of `Note' object which use this notes list.
		//
    int			nlUseCounter;

		//
		// Modification time of the note file when it was loaded
		// (used to detect when the note file must be re-loaded).
		//
    time_t		nlLoadTime;
private:
    inline NoteList* next();

public:
	//
	// Create a notes file.
	//
    NoteList(const char* _file);

    ~NoteList();

	//
	// Return the first note of the notes file.
	//
    inline Note* first() const;

	//
	// Return the `_which' th note of the list.
	//
    inline Note* operator [](unsigned _which);

	//
	// Return the mode which must be used when scanning the notes.
	//
    inline NoteScanMode scanMode() const;

	//
	// Return true if this notes file was traversed during the last
	// scan operation. This is used to detect cycles in the notes
	// files.
	//
    inline bool wasTraversed() const;

	//
	// Return the path of the file holding the notes descriptions.
	//
    inline const char* file() const;

	//
	// Return the number of notes in the notes file.
	//
    inline long size() const;

	//
	// Determine whether the notes file must be re-loaded (ie it
	// has changed on the disk).
	//
    bool needLoad() const;

	//
	// Load the notes file, updating the list of notes.
	//
    int load();

	//
	// Save the notes file.
	//
    int save();

	//
	// Find a note given its title.
	//
    Note* find(const char* _title);

	//
	// Create a new note, checking if a previous note with the title
	// existed before, and assign it a content string. If the note
	// type is a file, the current directory is prepended to the
	// path `_content' if it does not start with /.
	//
    Note* create(const char* _title, NoteType _type, const char* _content);

	//
	// Delete the note whose title is `_title'
	// and save the notes file.
	//
    int delNote(const char* _title);

	//
	// Scan the notes file calling the callback functions for each note.
	// Stop when the callback tells to stop, or when the end of the
	// notes file is reached.
	//
    NoteScanResult scan(NoteScanMode _mode, NoteScanCallback& _callback);

	//
	// Recursively clear the traveral mark for each notes file.
	// The traversal marks are used to detect cycles by the scan
	// operation. Before a scan operation can be executed, all the
	// traversal marks should be cleared.
	//
    void clearTraversal();

	//
	// Find a list of notes given its path.
	//
    static NoteList* findNotes(const char* _file);
};

    inline NoteList*
NoteList::next()
{
    return (NoteList*) dlink::next();
}

    inline Note*
NoteList::first() const
{
    return (Note*) dlist::first();
}

    inline Note*
NoteList::operator [](unsigned _which)
{
    return (Note*) dlist::pos(_which);
}

    inline NoteScanMode
NoteList::scanMode() const
{
    return nlScanMode;
}

    inline bool
NoteList::wasTraversed() const
{
    return nlScanTraversed;
}

    inline const char*
NoteList::file() const
{
    return nlFile;
}

    inline long
NoteList::size() const
{
    return dlist::size();
}

    inline void
Note::clearSubTraversal()
{
    Require(nType == SubNotes);

    if (nSubNotes != (NoteList*) NULL) {
	nSubNotes->clearTraversal();
    }
}

    inline int
Note::saveList()
{
    return nList.save();
}


// ----------------------------------------------------------------------
// Class :	NoteScanCallback
//
// Role :	Callback object used when scanning the different notes
//
class NoteScanCallback {
protected:
    int		nscCurrentDepth;
public:
    NoteScanCallback();

    virtual ~NoteScanCallback();

	//
	// First callback executed on the note `_note' and with the
	// complete note content passed in `_buf' and `_size'. External
	// notes are mapped in memory (if possible).
	//
    virtual NoteScanResult noteScanStart(Note& _note, const char* _buf,
					 long _size);

	//
	// Callback executed on each line of the note (if `noteScanStart'
	// returned `NoteScanContinue'). Each line are passed in
	// `_buf' (null terminated). The line number is passed in `_line'.
	//
    virtual NoteScanResult apply(Note& _note, const char* _buf, int _line);

	//
	// Apply an operation on the note.
	//
    virtual NoteScanResult apply(Note& _note);

	//
	// Start a deep scan of the note file.
	//
    virtual NoteScanResult deepScanStart(Note& _note);

	//
	// End a deep scan initiated on the note file.
	//
    virtual NoteScanResult deepScanStop(Note& _note);
};


#endif
