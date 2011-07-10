/* File selector widget
** Copyright (C) 1993, 1994, 1995, 1996 Stephane Carrez
**
** This file is part of Xcra.
**
** Xcra is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** Xcra is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
*/
#ifndef _FILE_SELECTORP_h
#define _FILE_SELECTORP_h

#include "FileSelect.h"
#include <X11/Xaw/FormP.h>

typedef struct {
  int	empty;
} FileSelectorClassPart;

typedef struct _FileSelectorClassRec {
    CoreClassPart		core_class;
    CompositeClassPart		composite_class;
    ConstraintClassPart		constraint_class;
    FormClassPart		form_class;
    FileSelectorClassPart	selector_class;
} FileSelectorClassRec;

extern FileSelectorClassRec filedialogClassRec;

struct DirList;

typedef struct _FileSelectorPart {
  /*
  ** Public resources
  */
  String	titleString;
  String	directoryString;
  String	pathString;
  String	selectionString;
  String	filterString;	/* Current filter string	*/
  String	messageString;

  int		updateTime;

  Boolean	sortLists;
  Boolean	showSize;
  Boolean	showOwner;

  XtCallbackList selectCallbackList;

  /*
  ** Private data
  */
  XtIntervalId	updateTimer;
  struct DirList* dirList;
  char**	fileList;
  int*		fileIndex;
  Widget	titleW;
  Widget	dirW;
  Widget	fileW;
  Widget	viewDirs;
  Widget	viewFiles;
  Widget	dirValue;
  Widget	fileValue;
  Widget	filterValue;
  Widget	accessValue;
  Widget	sizeValue;
  Widget	formW;
  Widget	filterName;
  Widget	dirLabel;
  Widget	fileLabel;
  Widget	msgLabel;
  Widget	msgText;
} FileSelectorPart;

typedef struct _FileSelectorRec {
    CorePart		core;
    CompositePart	composite;
    ConstraintPart	constraint;
    FormPart		form;
    FileSelectorPart	fsel;
} FileSelectorRec;

typedef struct {int empty;} FileSelectorConstraintsPart;

typedef struct _FileSelectorConstraintsRec {
    FormConstraintsPart	  	form;
    FileSelectorConstraintsPart selector;
} FileSelectorConstraintsRec, *FileSelectorConstraints;

#endif
