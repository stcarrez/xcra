// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Note Manager tool
// Author	: S. Carrez
// Date		: Sun Sep 17 11:45:10 1995
// Version	: $Id: NoteManager.H,v 1.8 1996/08/04 15:37:55 gdraw Exp $
// Project	: xcra
// Keywords	: Note, Tool
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
// class NoteManager	Small Notes manager (create, remove, list, grep)
//
#ifndef _NOTE_MANAGER_H_
#define _NOTE_MANAGER_H_

#include "Note.H"

class NoteManager;

enum NoteOperation {
    NoOp,
    GrepNote,
    ShowNote,
    DeleteNote,
    ListNote,
    CreateNote,
    AppendNote,
    ReplaceNote
};


// ----------------------------------------------------------------------
// Class :	NoteManager
//
// Role :	Small notes manager
//
class NoteManager : public NoteScanCallback {
protected:
    int			nmDeepScan;
    int			nmPrintTitle;
    int			nmPrintGeometry;
    int			nmPrintFile;
    int			nmReadStdin;

    NoteOperation	nmScanOperation;

    const char*		nmNoteTitle;
    const char*		nmNoteFile;

    Note*		nmNoteSearched;

    int			nmDefaultX;
    int			nmDefaultY;
    unsigned		nmDefaultWidth;
    unsigned		nmDefaultHeight;

    NoteType		nmNoteType;

public:

    NoteManager();

    ~NoteManager();

    void print(Note& _note);

    NoteScanResult noteScanStart(Note& _note, const char* _buf, long _size);

    NoteScanResult apply(Note& _note, char* _buf, int _line);

    NoteScanResult apply(Note& _note);

    int fillNote(Note* _note, int _argc, char** _argv);

    void help();

    int main(int argc, char** argv);

    void usage();

    void error(const char* _msg, ...);
    
};

#endif
