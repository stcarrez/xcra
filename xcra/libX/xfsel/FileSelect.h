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
#ifndef _FILE_SELECTOR_H
#define	_FILE_SELECTOR_H

/* Inherited Parameters (from Form widget):

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 defaultDistance     Thickness		int		4
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	computed at realize
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	computed at realize
 x		     Position		Position	0
 y		     Position		Position	0

*/

/* Constraint parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 bottom		     Edge		XtEdgeType	XtRubber
 fromHoriz	     Widget		Widget		(left edge of form)
 fromVert	     Widget		Widget		(top of form)
 horizDistance	     Thickness		int		defaultDistance
 left		     Edge		XtEdgeType	XtRubber
 resizable	     Boolean		Boolean		False
 right		     Edge		XtEdgeType	XtRubber
 top		     Edge		XtEdgeType	XtRubber
 vertDistance	     Thickness		int		defaultDistance

*/

/* Specific parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 titleName	     TitleName		String		NULL
 filterName          FilterName		String		"*"
 messageString	     Message		String		""
 directory           Directory          String		"."
 selectionFile       SelectionFile      String		NULL
 path                Path               String		NULL
 sortLists           SortLists          Boolean		False
 selectCallback      SelectCallback     XtRCallback	NULL
 updateTime          UpdateTime         int		0

*/

#define	XtNtitleName		"titleName"
#define	XtCTitleName		"TitleName"
#define	XtNfilterName		"filterName"
#define	XtCFilter		"FilterName"
#define	XtNpath			"path"
#define	XtCPath			"Path"
#define	XtNdirectory		"directory"
#define	XtCDirectory		"Directory"
#define	XtNselectionFile	"selectionFile"
#define	XtCSelectionFile	"SelectionFile"
#define	XtNsortLists		"sortLists"
#define	XtCSortLists		"SortLists"
#define	XtNselectCallback	"selectCallback"
#define	XtCSelectCallback	"SelectCallback"
#define	XtNupdateTime		"updateTime"
#define	XtCUpdateTime		"UpdateTime"
#define XtNmessageString	"messageString"
#define XtCMessage		"Message"

typedef struct _FileSelectorClassRec*	FileSelectorWidgetClass;
typedef struct _FileSelectorRec*	FileSelectorWidget;

typedef struct {
  char*		directory;
  char*		file_name;
} XawFileSelectionResult;

#ifndef FILE_SELECTOR
externalref WidgetClass fileselectorWidgetClass;
#endif

#endif


