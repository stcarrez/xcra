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
#define	FILE_SELECTOR

#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>

#include <X11/Xaw/XawInit.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Command.h>	
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Box.h>

#include "config.h"
#include "DirList.h"
#include "FileSelectP.h"

#ifdef HAVE_REGEX_H
#  include <regex.h>
#endif

#ifdef HAVE_RE_COMP_H
#  include <re_comp.h>
#endif

#include <errno.h>

#ifdef __STDC__
# define F0()			(void)
# define F1(T1,P1)		(T1 P1)
# define F2(T1,P1,T2,P2)	(T1 P1, T2 P2)
# define F3(T1,P1,T2,P2,T3,P3)	(T1 P1, T2 P2, T3 P3)
# define F4(T1,P1,T2,P2,T3,P3,T4,P4) (T1 P1, T2 P2, T3 P3, T4 P4)
# define F5(T1,P1,T2,P2,T3,P3,T4,P4,T5,P5) (T1 P1, T2 P2, T3 P3, T4 P4, T5 P5)
# define F2V(T1,P1,T2,P2)	(T1 P1, T2 P2, ...)
# ifndef __P
#   define __P(x)			x
# endif
#else
# define F0()			()
# define F1(T1,P1)		(P1) T1 P1;
# define F2(T1,P1,T2,P2)	(P1,P2) T1 P1; T2 P2;
# define F3(T1,P1,T2,P2,T3,P3)	(P1,P2,P3) T1 P1; T2 P2; T3 P3;
# define F4(T1,P1,T2,P2,T3,P3,T4,P4) (P1,P2,P3,P4) T1 P1; T2 P2; T3 P3; T4 P4;
# define F5(T1,P1,T2,P2,T3,P3,T4,P4,T5,P5) (P1,P2,P3,P4,P5) T1 P1; T2 P2; T3 P3; T4 P4;T5 P5;
# define F2V(T1,P1,T2,P2)	(P1,P2,va_alist) T1 P1; T2 P2; va_dcl
# ifndef __P
#   define __P(x)			()
# endif
# ifndef const
#  define const
# endif
#endif


/*
 * After we have set the string in the value widget we set the
 * string to a magic value.  So that when a SetValues request is made
 * on the dialog value we will notice it, and reset the string.
 */

#define MAGIC_VALUE ((char *) 3)

#define	MSG_READING_DIRECTORY	"Reading directory..."
#define	MSG_SORTING_LISTS	"Sorting lists..."
#define	MSG_FILTERING		"Filtering files..."
#define	MSG_READ_ERROR		"General read error"
#define	MSG_PERMISSION		"Permission denied"
#define	MSG_NOTADIRECTORY	"This is not a directory"
#define	MSG_NOENTRY		"This is not a valid path"
#define	MSG_MEMORY		"Virtual memory exhausted"
#define	MSG_GENERAL_ERROR	"General system error"
#define	MSG_EMPTY		""

#define streq(a,b) (strcmp( (a), (b) ) == 0)

static XtResource resources[] = {
  {XtNtitleName,  XtCTitleName, XtRString, sizeof(String),
     XtOffsetOf(FileSelectorRec, fsel.titleString), XtRString, NULL},
  {XtNfilterName, XtCFilter, XtRString, sizeof(String),
     XtOffsetOf(FileSelectorRec, fsel.filterString), XtRString, NULL},
  {XtNpath, XtCPath, XtRString, sizeof(String),
     XtOffsetOf(FileSelectorRec, fsel.pathString), XtRString, NULL},
  {XtNdirectory, XtCDirectory, XtRString, sizeof(String),
     XtOffsetOf(FileSelectorRec, fsel.directoryString), XtRString, "."},
  {XtNselectionFile, XtCSelectionFile, XtRString, sizeof(String),
     XtOffsetOf(FileSelectorRec, fsel.selectionString), XtRString, NULL},
  {XtNmessageString, XtCMessage, XtRString, sizeof(String),
     XtOffsetOf(FileSelectorRec, fsel.messageString), XtRString, NULL},
  {XtNsortLists, XtCSortLists, XtRBoolean, sizeof(Boolean),
     XtOffsetOf(FileSelectorRec, fsel.sortLists), XtRBoolean, False },
  { XtNselectCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
     XtOffsetOf(FileSelectorRec, fsel.selectCallbackList),
      XtRCallback, (XtPointer) NULL},
  {XtNupdateTime, XtCUpdateTime, XtRInt,  sizeof(int),
     XtOffsetOf(FileSelectorRec, fsel.updateTime), XtRImmediate, (XtPointer)0}
};

typedef struct {
  int	len;
  int	count;
  char*	string;
} CompletionResult_t;


/*
 * Widget methods.
 */

static void	Initialize __P((Widget request, Widget new,
				ArgList arg, Cardinal* n));
static void	Destroy __P((Widget w));
static void	Realize __P((Widget w, Mask* msk, XSetWindowAttributes* attr));
static Boolean	SetValues __P((Widget w, Widget r, Widget n,
			       ArgList args, Cardinal* N));
static void	GetValuesHook __P((Widget w, ArgList args,
				   Cardinal* num_args));


/*
 * Utility routines.
 */

static void	SetMessage __P((Widget w, const char* msg));
static void	TextSetString __P((Widget w, const char* string));
static void	SetMessage __P((Widget w, const char* msg));
static Widget	MakeLabel __P((const char* name, Widget w));
static Widget	MakeText __P((const char* name, Widget w, const char* value));

static char*	TransformPattern __P((const char** list, const char* pattern));
static int	CompareName __P((const char** s1, const char** s2));


/*
 * Private callbacsk.
 */

static void	SelectFile __P((Widget w, XtPointer tag, XtPointer data));
static void	GoSubdir __P((Widget w, XtPointer tag, XtPointer data));

static const char** GetSelectionList __P((DirList_t* dl, int** pindx,
					  const char** m));
static int	SetupFileSelectionList __P((FileSelectorWidget dw,
					    DirList_t* nList));
static char*	ReducePath __P((const char* path));

enum DirMode {
    DIR_ABSOLUTE,
    DIR_RELATIVE
};

static int	ChangeDirectory __P((FileSelectorWidget dw, const char* subD,
				     enum DirMode m));

static FileSelectorWidget	FindFileSelectorWidget __P((Widget w));
static void	FindCompletion __P((CompletionResult_t* completionAdd,
				    const char* string, const char** list));
static char*	Dirname __P((const char* path, const char** baseName));
static int	DirectoryCompletion __P((FileSelectorWidget dw,
					 CompletionResult_t* Cres));
static int	FileCompletion __P((FileSelectorWidget dw,
				    CompletionResult_t* Cres));

/*
 * Actions
 */
#define	ACTION_PARAMS	(Widget w, XEvent* ev, String* str, Cardinal* n)

static void	SelectFileAction	__P(ACTION_PARAMS);
static void	CompletionNameAction	__P(ACTION_PARAMS);
static void	RescanAction		__P(ACTION_PARAMS);
static void	ChangeFilterAction	__P(ACTION_PARAMS);

static void UpdateDirectory __P((XtPointer closure, XtIntervalId* id));


static XtActionsRec actions_table[] = {
  { "Select",		SelectFileAction		},
  { "Completion",	CompletionNameAction		},
  { "RescanDirectory",	RescanAction			},
  { "RefilterList",	ChangeFilterAction		}
};

static char	defaultTranslations[] =
  "<Key>Return:		Select()		\n\
   Ctrl<Key>L:		RescanDirectory()";


FileSelectorClassRec fileselectorClassRec = {
  { /* core_class fields */
    /* superclass         */    (WidgetClass) &formClassRec,
    /* class_name         */    "FileSelector",
    /* widget_size        */    sizeof(FileSelectorRec),
    /* class_initialize   */    XawInitializeWidgetSet,
    /* class_part init    */    NULL,
    /* class_inited       */    FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook    */    NULL,
    /* realize            */    Realize,
    /* actions            */    actions_table,
    /* num_actions        */    XtNumber(actions_table),
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    TRUE,
    /* compress_exposure  */    TRUE,
    /* compress_enterleave*/    TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    Destroy,
    /* resize             */    XtInheritResize,
    /* expose             */    XtInheritExpose,
    /* set_values         */    SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */    GetValuesHook,
    /* accept_focus       */    NULL,
    /* version            */    XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    defaultTranslations,
    /* query_geometry     */	XtInheritQueryGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  { /* composite_class fields */
    /* geometry_manager   */   XtInheritGeometryManager,
    /* change_managed     */   XtInheritChangeManaged,
    /* insert_child       */   XtInheritInsertChild,
    /* delete_child       */   XtInheritDeleteChild,
    /* extension          */   NULL
  },
  { /* constraint_class fields */
    /* subresourses       */   NULL,
    /* subresource_count  */   0,
    /* constraint_size    */   sizeof(FileSelectorConstraintsRec),
    /* initialize         */   NULL,
    /* destroy            */   NULL,
    /* set_values         */   NULL,
    /* extension          */   NULL
  },
  { /* form_class fields */
    /* layout             */   XtInheritLayout
  },
  { /* fileselector_class fields */
    /* empty              */   0
  }
};

WidgetClass fileselectorWidgetClass = (WidgetClass)&fileselectorClassRec;

/* ----------------------------------------------------------------------
**
**		Widget methods
**
** -------------------------------------------------------------------- */

/*
** Role :	Initialize method.
*/
static void Initialize F4(Widget, request, Widget, new, ArgList, arg,
			  Cardinal*, n)
{
    FileSelectorPart*		p;
    FileSelectorWidget		dw = (FileSelectorWidget) new;
    Arg				args[6];
    Widget			wd = None;
    Widget			listForm;
    Widget			filterForm;
    Widget			dirResult;
    Widget			msgForm;
    Cardinal			ac;

    p = &dw->fsel;
    p->dirList  = 0;
    p->fileList = 0;
    p->fileIndex= 0;
    p->titleW   = None;
    p->updateTimer = 0;

    if (p->titleString) {
	p->titleString = XtNewString(p->titleString);
    }
    if (p->directoryString) {
	p->directoryString = XtNewString(p->directoryString);
    }
    if (p->pathString) {
	p->pathString = XtNewString(p->pathString);
    }
    if (p->selectionString) {
	p->selectionString = XtNewString(p->selectionString);
    }
    if (p->filterString) {
	p->filterString = XtNewString(p->filterString);
    }
    if (p->messageString) {
	p->messageString = XtNewString(p->messageString);
    }

	/*
	** Create the title.
	*/
    if (p->titleString) {
	XtSetArg (args[0], XtNtop, XtChainTop);
	XtSetArg (args[1], XtNright, XtChainRight);
	XtSetArg (args[2], XtNleft, XtChainLeft);
	XtSetArg (args[3], XtNbottom, XtRubber);
	wd = XtCreateManagedWidget ("title", formWidgetClass, new, args, 4);
	p->titleW = MakeLabel ("titleLabel", wd);

	XtSetArg (args[0], XtNlabel, dw->fsel.titleString);
	XtSetValues (p->titleW, args, 1);
    }

	/*
	** Create the filter box.
	*/
    XtSetArg (args[0], XtNright, XtChainRight);
    XtSetArg (args[1], XtNleft, XtChainLeft);
    XtSetArg (args[2], XtNbottom, XtRubber);
    if (p->titleString) {
	XtSetArg (args[3], XtNfromVert, wd);
	XtSetArg (args[4], XtNtop, XtRubber);
	ac = 5;
    } else {
	XtSetArg (args[3], XtNtop, XtChainTop);
	ac = 4;
    }
    filterForm = XtCreateManagedWidget ("filter", formWidgetClass,
					new, args, ac);

    p->filterName = MakeLabel ("filterName", filterForm);
    p->filterValue= MakeText ("filterValue", filterForm, p->filterString);

    XtSetKeyboardFocus (filterForm, p->filterValue);
  
	/*
	** Create the form holding the two scrolling lists.
	*/
    XtSetArg (args[0], XtNtop, XtRubber);
    XtSetArg (args[1], XtNright, XtChainRight);
    XtSetArg (args[2], XtNleft, XtChainLeft);
    XtSetArg (args[3], XtNbottom, XtRubber);
    XtSetArg (args[4], XtNfromVert, filterForm);
    listForm = XtCreateManagedWidget ("list", formWidgetClass, new, args, 5);

	/*
	** Scrolled list of directories.
	*/
    XtSetArg (args[0], XtNtop, XtChainTop);
    XtSetArg (args[1], XtNright, XtRubber);
    XtSetArg (args[2], XtNleft, XtChainLeft);
    XtSetArg (args[3], XtNbottom, XtChainBottom);
    XtSetArg (args[4], XtNallowVert, True);
    p->viewDirs = XtCreateManagedWidget ("viewDirs", viewportWidgetClass,
					 listForm, args, 5);

    p->dirW = XtCreateManagedWidget ("dirs", listWidgetClass,
				     p->viewDirs, NULL, 0);

    XtAddCallback (p->dirW, XtNcallback, GoSubdir, (XtPointer) new);


	/*
	** Scrolled list of files.
	*/
    XtSetArg (args[0], XtNtop, XtChainTop);
    XtSetArg (args[1], XtNright, XtChainRight);
    XtSetArg (args[2], XtNleft, XtRubber);
    XtSetArg (args[3], XtNbottom, XtChainBottom);
    XtSetArg (args[4], XtNfromHoriz, p->viewDirs);
    XtSetArg (args[5], XtNallowVert, True);
    p->viewFiles = XtCreateManagedWidget ("viewFiles", viewportWidgetClass,
					  listForm, args, 6);


    p->fileW = XtCreateManagedWidget ("files", listWidgetClass,
				      p->viewFiles, NULL, 0);

    XtAddCallback (p->fileW, XtNcallback, SelectFile, (XtPointer) new);

	/*
	** Create the selection form.
	*/
    XtSetArg (args[0], XtNtop, XtRubber);
    XtSetArg (args[1], XtNright, XtChainRight);
    XtSetArg (args[2], XtNleft, XtChainLeft);
    XtSetArg (args[3], XtNbottom, XtRubber);
    XtSetArg (args[4], XtNfromVert, listForm);
    dirResult = XtCreateManagedWidget ("selection", formWidgetClass,
				       new, args, 5);

    p->dirLabel  = MakeLabel ("dirName", dirResult);
    p->dirValue  = MakeText ("dirValue", dirResult, p->directoryString);

    p->fileLabel = MakeLabel ("fileName", dirResult);
    p->fileValue = MakeText ("fileValue", dirResult, p->selectionString);

	/*
	** Create the message form.
	*/
    XtSetArg (args[0], XtNtop, XtRubber);
    XtSetArg (args[1], XtNright, XtChainRight);
    XtSetArg (args[2], XtNleft, XtChainLeft);
    XtSetArg (args[3], XtNbottom, XtChainBottom);
    XtSetArg (args[4], XtNfromVert, dirResult);
    msgForm = XtCreateManagedWidget ("messages", formWidgetClass,
				     new, args, 5);

    p->msgLabel = MakeLabel ("msgLabel", msgForm);
    p->msgText  = MakeLabel ("msgText", msgForm);
}


/*
** Role :	Destroy widget method.
*/
static void
Destroy F1(Widget, w)
{
    FileSelectorPart*	p;
    FileSelectorWidget	dw = (FileSelectorWidget) w;

    p = &dw->fsel;

    if (dw->fsel.updateTimer)
      XtRemoveTimeOut (dw->fsel.updateTimer);

    if (p->dirList)
      DestroyDirectoryList (p->dirList);

    if (p->fileList)
      XtFree ((char*)p->fileList);

    if (p->fileIndex)
      XtFree ((char*)p->fileIndex);

    if (p->selectionString)
      XtFree (p->selectionString);

    if (p->directoryString)
      XtFree (p->directoryString);

    if (p->pathString)
      XtFree (p->pathString);

    if (p->filterString)
      XtFree (p->filterString);
}


/*
** Role :	Realize widget method.
*/
static void 
Realize F3(Widget, w, Mask*, valueMask, XSetWindowAttributes*, attributes)
{
    FileSelectorWidget	dw = (FileSelectorWidget) w;

    (*fileselectorClassRec.core_class.superclass->core_class.realize)
      (w, valueMask, attributes);
  
    (void)ChangeDirectory (dw, dw->fsel.directoryString, DIR_ABSOLUTE);

    if (dw->fsel.updateTime > 0) {
	XtAppContext app =  XtWidgetToApplicationContext (w);
	dw->fsel.updateTimer = XtAppAddTimeOut (app,
						dw->fsel.updateTime * 1000,
						UpdateDirectory, w);
    }
}


/* ARGSUSED */
static Boolean
SetValues F5(Widget, current, Widget, request, Widget, new, ArgList, in_args,
	     Cardinal*, in_num_args)
{
    FileSelectorWidget w   = (FileSelectorWidget) new;
    FileSelectorWidget old = (FileSelectorWidget) current;
    int		     need_rescan = False;
    int		     need_update = False;

	/*
	** Setup new title.
	*/
    if (w->fsel.titleString != old->fsel.titleString) {
	const char* p;

	p = w->fsel.titleString;
	if (p == 0)
	  p = "";
	if (w->fsel.titleW != None)
	  XtVaSetValues (w->fsel.titleW, XtNlabel, p, NULL);
    }

	/*
	** Change path string.
	*/
    if (w->fsel.pathString != old->fsel.pathString) {
	const char* basename;
	if (old->fsel.pathString)
	  XtFree ((char *) old->fsel.pathString);
	if (old->fsel.directoryString)
	  XtFree ((char *) old->fsel.directoryString);
	if (old->fsel.selectionString)
	  XtFree ((char *) old->fsel.selectionString);

	w->fsel.directoryString = Dirname(w->fsel.pathString, &basename);
	w->fsel.selectionString = XtNewString (basename);
	w->fsel.pathString      = XtNewString (w->fsel.pathString);
	w->fsel.directoryString = XtNewString (w->fsel.directoryString);
	need_rescan = True;

	TextSetString (w->fsel.dirValue, w->fsel.directoryString);
	TextSetString (w->fsel.fileValue, w->fsel.selectionString);
    }

	/*
	** Change directory string.
	*/
    if (w->fsel.directoryString != old->fsel.directoryString) {
	if (old->fsel.directoryString)
	  XtFree ((char *) old->fsel.directoryString);

	w->fsel.directoryString = XtNewString (w->fsel.directoryString);
	need_rescan = True;

	TextSetString (w->fsel.dirValue, w->fsel.directoryString);
    }

	/*
	** Change filter string.
	*/
    if (w->fsel.filterString != old->fsel.filterString) {
	if (old->fsel.filterString)
	  XtFree ((char *) old->fsel.filterString);

	w->fsel.filterString = XtNewString (w->fsel.filterString);
	need_update = True;

	TextSetString (w->fsel.filterValue, w->fsel.filterString);
    }

	/*
	** Change message string.
	*/
    if (w->fsel.messageString != old->fsel.messageString) {
	if (old->fsel.messageString)
	  XtFree ((char *) old->fsel.messageString);

	w->fsel.messageString = XtNewString (w->fsel.messageString);
	SetMessage (w->fsel.msgText, w->fsel.messageString);
    }

	/*
	** Change file selection string.
	*/
    if (w->fsel.selectionString != old->fsel.selectionString) {
	if (old->fsel.selectionString)
	  XtFree ((char *) old->fsel.selectionString);

	w->fsel.selectionString = XtNewString (w->fsel.selectionString);

	TextSetString (w->fsel.fileValue, w->fsel.selectionString);
    }


	/*
	** Change the update time.
	*/
    if (w->fsel.updateTime != old->fsel.updateTime) {
	if (old->fsel.updateTimer)
	  XtRemoveTimeOut (old->fsel.updateTimer);

	w->fsel.updateTimer = 0;
	if (w->fsel.updateTime > 0) {
	    XtAppContext app = XtWidgetToApplicationContext(new);
	    w->fsel.updateTimer = XtAppAddTimeOut (app,
						   w->fsel.updateTime * 1000,
						   UpdateDirectory, current);
	}
    }

    if (w->fsel.sortLists != old->fsel.sortLists)
      need_rescan = True;

    if (w->fsel.dirList == 0)
      need_rescan = need_update = False;

    if (need_rescan) {
	(void)ChangeDirectory (w, w->fsel.directoryString, DIR_ABSOLUTE);
    } else if (need_update) {
	SetupFileSelectionList (w, w->fsel.dirList);
    }

    return False;
}


/*	Function Name: GetValuesHook
 *	Description: This is a get values hook routine that gets the
 *                   values in the dialog.
 *	Arguments: w - the Text Widget.
 *                 args - the argument list.
 *                 num_args - the number of args.
 *	Returns: none.
 */

static void
GetValuesHook F3(Widget, w, ArgList, args, Cardinal*, num_args)
{
    Arg 	     a[1];
    String 	     s;
    FileSelectorWidget src = (FileSelectorWidget) w;
    int 	     i;
  
    for (i = 0; i < *num_args; i++) {

	/*
	** Get filter string.
	*/
	if (streq (args[i].name, XtNfilterName)) {
	    XtSetArg (a[0], XtNstring, &s);
	    XtGetValues (src->fsel.filterValue, a, 1);
	    *((char **) args[i].value) = s;

	/*
	** Get message string.
	*/
	} else if (streq (args[i].name, XtNmessageString)) {
	    XtSetArg (a[0], XtNlabel, &s);
	    XtGetValues (src->fsel.msgText, a, 1);
	    *((char **) args[i].value) = s;

	/*
	** Get current directory which is visited.
	*/
	} else if (streq (args[i].name, XtNdirectory)) {
	    s = (src->fsel.dirList == 0)
	      ? src->fsel.directoryString :
		src->fsel.dirList->path;
	    *((char **) args[i].value) = s;

	/*
	** Get selection string.
	*/
	} else if (streq(args[i].name, XtNselectionFile)) {
	    XtSetArg (a[0], XtNstring, &s);
	    XtGetValues (src->fsel.fileValue, a, 1);
	    *((char **) args[i].value) = s;

	/*
	** Get complete path.
	*/
	} else if (streq(args[i].name, XtNpath)) {
	    const char* dir = (src->fsel.dirList == 0)
	      ? src->fsel.directoryString :
		src->fsel.dirList->path;

	    XtSetArg (a[0], XtNstring, &s);
	    XtGetValues (src->fsel.fileValue, a, 1);
	    if (src->fsel.pathString)
	      XtFree ((char*) src->fsel.pathString);

	    src->fsel.pathString = XtMalloc (strlen(dir)
					     + strlen(s) + 2);
	    strcpy(src->fsel.pathString, dir);
	    strcat(src->fsel.pathString, "/");
	    strcat(src->fsel.pathString, s);
	    *((char **) args[i].value) = src->fsel.pathString;

	/*
	** Get title string.
	*/
	} else if (streq(args[i].name, XtNtitleName)) {
	    if (src->fsel.titleW != None) {
		XtSetArg (a[0], XtNlabel, &s);
		XtGetValues (src->fsel.titleW, a, 1);
		*((char **) args[i].value) = s;
	    } else {
		*((char **) args[i].value) = 0;
	    }
	}
    }
}



/* ----------------------------------------------------------------------
**
**		Utility routines
**
** -------------------------------------------------------------------- */

/*
** Role :	Display an information or an error message in the label.
*/
static void
SetMessage F2(Widget, w, const char*, msg)
{
    Arg	args[1];

    XtSetArg (args[0], XtNlabel, msg);
    XtSetValues (w, args, 1);

	/*
	** Hack:	The label widget must be updated right now. It must
	**		be made visible immediately because this may be a
	**		temporary message which only indicates what's going
	**		on (Reading directory, Filtering,...). If the expose
	**		method is not called explicitly, the message does
	**		not appear. I don't know how to do another way.
	*/
    if (XtIsRealized (w)) {
	XEvent ev;

	ev.type = Expose;
	ev.xexpose.serial = 0;
	ev.xexpose.send_event = True;
	ev.xexpose.display = XtDisplay (w);
	ev.xexpose.window  = XtWindow (w);
	ev.xexpose.x = 0;
	ev.xexpose.y = 0;
	ev.xexpose.width = w->core.width;
	ev.xexpose.height= w->core.height;

	/*
	** Force redraw of the widget and flush the X11 queue.
	*/
	(* XtClass(w)->core_class.expose) (w, &ev, NULL);
	XFlush (XtDisplay (w));
    }
}


/*
** Role :	Setup the text value of an asciiText widget and set the
**		caret at end of the text.
*/
static void
TextSetString F2(Widget, w, const char*, string)
{
    XawTextPosition	pos;

    XtVaSetValues (w, XtNstring, string, NULL);

    pos = (XawTextPosition) (strlen (string) + 1);
    XawTextSetInsertionPoint (w, pos);
}


/*
** Role :	Create a label widget of name `name' and put it in the
**		parent widget `formW'.
*/
static Widget
MakeLabel F2(const char*, name, Widget, formW)
{
    Arg	args[1];

    XtSetArg (args[0], XtNborderWidth, 0);
    return XtCreateManagedWidget (name, labelWidgetClass, formW, args, 1);
}


/*
** Role :	Create an asciiText widget of name `name' and insert it
**		in the form widget `formW'. Give it value `value'.
*/
static Widget
MakeText F3(const char*, name, Widget, formW, const char*, value)
{
    int		ac;
    Arg		args[4];

    ac = 0;
    XtSetArg (args[0], XtNeditType, XawtextEdit ); ac++;
    XtSetArg (args[1], XtNresizable, False ); ac++;
    XtSetArg (args[2], XtNresize, XawtextResizeBoth ); ac++;
    if (value) {
	XtSetArg (args[3], XtNstring, value ); ac ++;
    }

    return XtCreateManagedWidget (name, asciiTextWidgetClass,
				  formW, args, ac);
}

#ifndef HAVE_RE_EXEC
static char* rcomp;

static char*
re_comp F1(char*,pat)
{
    rcomp = regcmp (pat, (char*) NULL);
    return 0;
}

static int
re_exec F1(char*,match)
{
    char* r = regex (rcomp, match);
    return (r == 0) ? 0 : 1;
}
#endif

/*
** Role :	Transform the pseudo regex `regex' in a real regular
**		expression and compile it.
**
**		(Shell reg.) ->	(regex)
** Example :	*.c		^.*\.c$
**		*.[ch]		^.*\.[ch]$
**
*/
static int
CompilePattern F1(const char*, regex)
{
    char	*p;
    char*	buf;
    char	c;

    buf = XtMalloc (strlen (regex) * 2 + 3);
    p = buf;
    *p++ = '^';
    while ((c = *regex++)) {
	if (c == '*') {
	    *p++ = '.';
	    *p++ = c;
	} else if (c == '.') {
	    *p++ = '\\';
	    *p++ = c;
	} else if (c == '[') {
	    do {
		*p++ = c;
		c = *regex++;
	    } while (c != 0 && c != ']');
	    if (c == 0)
	      break;
	    *p++ = c;
	} else if (c == '(' || c == '!' || c == '^' || c == '|') {
	    *p++ = '\\';
	    *p++ = c;
	} else {
	    *p++ = c;
	}
    }
    *p++ = '$';
    *p = 0;

    if (re_comp (buf) != 0) {
	XtFree (buf);
	return 1;
    }
    XtFree (buf);
    return 0;
}


/*
** Role :	Split the fill pattern in several pseudo-regex.
*/
static char*
TransformPattern F2(const char**, list, const char*, pattern)
{
    char*	p;
    char*	result;

    while (*pattern == ' ' || *pattern == '\t')
      pattern++;

    p = XtNewString (pattern);

    *list++ = p;
    for (result = p; p[0]; p++) {
	if (*p == ' ' || *p == '\t') {
	    *p++ = 0;
	    while (*p == ' ' || *p == '\t')
	      p++;
	    if (*p == 0)
	      break;
	    *list++ = p;
	}
    }
    *list = 0;
    return result;
}


/*
** Role :	Compare two names for the sort routine.
*/
static int
CompareName F2(const char**, s1, const char**, s2)
{
    return strcmp (*s1, *s2);
}


/* ----------------------------------------------------------------------
**
**		Private Callbacks
**
** -------------------------------------------------------------------- */

/*
** Role :	A file item has been selected in the file list widget.
**		Update the editable text widget (fileValue widget).
**
** Invoked:	From selection of an item in the `files' List widget.
*/
static void
SelectFile F3(Widget, w, XtPointer, tag, XtPointer, data)
{
    FileSelectorWidget 	 dw   = (FileSelectorWidget) tag;
    XawListReturnStruct* XawP = (XawListReturnStruct *) data;

    TextSetString (dw->fsel.fileValue, XawP->string);
}



/*
** Role :	Callback executed to enter in the subdirectory when
**		the user selects a directory
**
** Invoked:	From selection of an item in the `dirs' List widget.
*/
static void
GoSubdir F3(Widget, w, XtPointer, tag, XtPointer, data)
{
    FileSelectorWidget	 dw    = (FileSelectorWidget) tag;
    XawListReturnStruct* XawLP = (XawListReturnStruct *) data;

    (void) ChangeDirectory (dw, XawLP->string, DIR_RELATIVE);
}


/*
** Role :	Build a table of pointers that correspond to the selection
**		of the files (DL_FILES) in `dl'.
**
*/
static const char**
GetSelectionList F3(DirList_t*, dl, int**, pindex, const char**, matchList)
{
    int*	 index;
    const char** list;
    int		 i;
    int		 last = 0;
    char*	 okList;
    const char** lp;

	/*
	** Get an area at least as large as the original (+1 for null).
	*/
    list = (const char **) XtMalloc ((dl->list[DL_FILES].count+1)
				     * sizeof(const char*));

    index = (int*) XtMalloc ((dl->list[DL_FILES].count) * sizeof(int));
    okList = (char*) XtCalloc (dl->list[DL_FILES].count, sizeof(char));

    for (lp = matchList; lp[0]; lp++) {
	int re_comp_failed;

	re_comp_failed = CompilePattern (lp[0]);
	for (i = 0; i < dl->list[DL_FILES].count; i++) {
	    if (okList[i])
	      continue;

	    if (re_comp_failed || re_exec (dl->list[DL_FILES].names[i]) == 1) {
		okList[i] = 1;
	    }
	}
    }

    for (i = 0; i < dl->list[DL_FILES].count; i++) {
	const char* name = dl->list[DL_FILES].names[i];
	if (okList[i]) {
	    list[last] = name;
	    index[last] = i;
	    last++;
	}
    }
    XtFree (okList);

    list[last] = 0;
    *pindex = index;
    return list;
}


/*
** Role :	Update the list of files and directories inside the widgets
**		Destroy the previous directory list.
**
** Results :	0 if Ok, -1 if memory pb
*/
static int
SetupFileSelectionList F2(FileSelectorWidget, dw, DirList_t*, newList)
{
    FileSelectorPart* fsl = &dw->fsel;
    const char**    nlist;
    char*	    curFilter;
    Arg		    args[1];
    int*	    pindex;
    const char**    matchList;

    SetMessage (fsl->msgText, MSG_FILTERING);

    XtSetArg (args[0], XtNstring, &curFilter);
    XtGetValues (fsl->filterValue, args, 1);

    matchList = (const char**) XtMalloc ((2 + (strlen(curFilter) / 2)) *
					 sizeof(const char*) );
    curFilter = TransformPattern (matchList, curFilter);

	/*
	** Build the list of files according to selection.
	*/
    nlist = GetSelectionList (newList, &pindex, matchList);
    XtFree ((char*)matchList);
    XtFree (curFilter);

	/*
	** Change the list of files.
	*/
    XawListChange (fsl->fileW, (char**)nlist, 0, 0, True);

	/*
	** Detects a modification of the list of directories and update.
	*/
    if (fsl->dirList != newList) {
	XawListChange (fsl->dirW, newList->list[DL_DIRS].names, 0, 0, True);

	TextSetString (fsl->dirValue, newList->path);

	if (fsl->dirList != 0)
	  DestroyDirectoryList (fsl->dirList);

	fsl->dirList = newList;
    }

    if (fsl->fileList != 0)
      XtFree ((char*) fsl->fileList);

    if (fsl->fileIndex != 0)
      XtFree ((char*) fsl->fileIndex);

    fsl->fileIndex = pindex;
    fsl->fileList  = (char**) nlist;

	/*
	** Clear the error message (since successful).
	*/
    SetMessage (fsl->msgText, MSG_EMPTY);

    return 0;
}


/*
** Role :	Isolate directory and basename of a path.
*/
static char*
Dirname F2(const char*, path, const char**, baseName)
{
    char*	p;

    p = strrchr (path, '/');

	/*
	** Implicit directory .
	*/
    if (p == 0) {
	*baseName = path;
	p = (char*) XtMalloc (2);

	strcpy (p, ".");

    } else {
	size_t len = (size_t) (p - path);

	*baseName = &p[1];
	p = (char*) XtMalloc (len + 1);

	if (len > 0) {
	    memcpy (p, path, len);
	    p[len] = 0;
	} else {
	    strcpy (p, "/");
	}
    }
    return p;
}


#define	IsSameFile(A,B)	(((A)->st_dev == (B)->st_dev) &&	\
			 ((A)->st_ino == (B)->st_ino))

/*
** Role :	Given the path `path', reduce the representation of
**		that path by removing extra "./" and "../" when this
**		is possible.
**
** Example :	Asterix/Gaston/../Obelix	->	Asterix/Obelix
**		Asterix/Obelix/../..		->	.
**		../../..			->	../../..
**
** Note:	Reducing the path is not a matter of performance but
**		rather a matter of human representation.
**
** Results :	The reduced path.
*/
static char*
ReducePath F1(const char*, path)
{
    const char* base_name;
    char* dir_path;
    char* smaller_path;

    dir_path = Dirname (path, &base_name);

    /*
     * Remove extra ./
     */
    if (strcmp (base_name, ".") == 0) {
        if (strcmp (dir_path, ".") == 0) {
	    return dir_path;
	}
	smaller_path = ReducePath (dir_path);
	XtFree (dir_path);
	return smaller_path;
    }

    /*
     * Check whether .. can be removed (ie: A/B/.. -> A)
     */
    if (strcmp (base_name, "..") == 0) {
	struct stat stOrig;
	struct stat stNew;
	const char* dir;
	int result;

	smaller_path = Dirname (dir_path, &dir);
	result = stat (smaller_path, &stNew);
	if (result == 0) {
	    result = stat (path, &stOrig);
	    if (result == 0 && IsSameFile (&stOrig, &stNew) == True) {
		XtFree (dir_path);
		dir_path = ReducePath (smaller_path);
		XtFree (smaller_path);
		return dir_path;
	    }
	}
	XtFree (smaller_path);
    }

    /*
     * Try to reduce the parent directory and we reconstruct the new path.
     */
    smaller_path = ReducePath (dir_path);
    XtFree (dir_path);

    dir_path = XtMalloc (strlen (smaller_path) + strlen (base_name) + 2);
    strcpy (dir_path, smaller_path);
    strcat (dir_path, "/");
    strcat (dir_path, base_name);

    return dir_path;
}


/*
** Role :	Move to a subdirectory and update the lists.
**
** Results :	0 if Ok, -1 if error
*/
static int
ChangeDirectory F3(FileSelectorWidget, dw, const char*, subDir,
		   enum DirMode, mode)
{
    FileSelectorPart*	fsl = &dw->fsel;
    char		*path;
    int			n;
    int			res;
    int			scan_mode;
    DirList_t*		dl;
    DirList_t*		dlNew;

    dl = fsl->dirList;

	/*
	** Normal case. Concatenate the current path and the sub-dir name.
	*/
    if (mode == DIR_RELATIVE) {
	char* old;
        if (dl == 0)
            return -1;
        
	path = dl->path;
	n = strlen (path) + strlen (subDir) + 2;
	old = path = (char *) XtMalloc (n);

	strcpy (path, dl->path);
	strcat (path, "/");
	strcat (path, subDir);

	path = ReducePath (path);
	XtFree (old);
    } else {
	/*
	** Treat the subDir as an absolute path (used by XtSetValues).
	*/
	path = ReducePath (subDir);
    }

    SetMessage (fsl->msgText, MSG_READING_DIRECTORY);

	/*
	** Create a new list of files using same parameters as old one.
	*/
    if (fsl->sortLists)
      scan_mode = DL_SORT | DL_ISOLATE_DIRECTORY;
    else
      scan_mode = DL_ISOLATE_DIRECTORY;

    dlNew = CreateDirectoryList (path, CompareName, scan_mode);
    XtFree (path);

    if (dlNew == 0) {
	const char* msg;

	switch (errno){
	  case EACCES :
	  case EPERM :	msg = MSG_PERMISSION;	break;
	  case ENOTDIR :msg = MSG_NOTADIRECTORY;break;
	  case ELOOP :
	  case ENAMETOOLONG :
	  case ENOENT :	msg = MSG_NOENTRY;	break;
	  case ENOMEM :	msg = MSG_MEMORY;	break;
	  default :	msg = MSG_GENERAL_ERROR;break;
	}
	SetMessage (fsl->msgText, msg);
	return -1;
    }

    res = SetupFileSelectionList (dw, dlNew);
    if (res != 0)
      DestroyDirectoryList (dlNew);

    return res;
}



/*
** Role :	Find the file selector widget.
*/
static FileSelectorWidget
FindFileSelectorWidget F1(Widget, w)
{
	/*
	** Can't do anything if the widget is not realized.
	*/
    if (!XtIsRealized (w))
      return (FileSelectorWidget) None;

	/*
	** Find the FileSelectorWidget widget.
	*/
    while (XtClass (w) != (WidgetClass) &fileselectorClassRec) {
	w = XtParent (w);
	if (w == None)
	  break;
    }
    return (FileSelectorWidget) w;
}



/* ----------------------------------------------------------------------
**
**	File name and Directory name completion
**
** -------------------------------------------------------------------- */


/*
** Role :	Find the completion name for `string' using the list of
**		names `list'. The completion addendum (the string that must
**		be appended to `string') is returned in `completionAdd'.
*/
static void
FindCompletion F3(CompletionResult_t*, completionAdd, const char*, string,
		  const char**, list)
{
    const char*	p;
    const char*	Match = 0;
    int		MatchLen = 0;
    int		MatchCount = 0;

    int len = strlen (string);

    while ((p = *list++) != 0) {
	/*
	** Possible completion name must match `len' first characters.
	*/
	if (strncmp (p, string, len) == 0) {
	    MatchCount ++;

		/*
		** First completion name found.
		*/
	    if (Match == 0) {
		Match = p;
		MatchLen = strlen (p);
	    } else {
		/*
		** Another completion name. Get the minimum of all match
		** lengths.
		*/
		int j = len;
		while (j < MatchLen && Match[j] == p[j]) {
		    j++;
		}
		MatchLen = j;
	    }
	}
    }

    MatchLen = MatchLen - len;
    if (MatchLen > 0) {
	completionAdd->string = (char*) XtMalloc (MatchLen + 1);

	memcpy (completionAdd->string, &Match[len], MatchLen);
	completionAdd->string[MatchLen] = 0;
    }
    completionAdd->count = MatchCount;
    completionAdd->len   = MatchLen;
}


/*
** Role :	Perform the file name completion on a directory.
*/
static int
DirectoryCompletion F2(FileSelectorWidget, dw, CompletionResult_t*, Cres)
{
    const char**	list;
    const char*		file;
    Arg			args[1];
    XawTextPosition	pos = 0;
    XawTextBlock	textBlock;
    int			res;
    const char*		bname;

	/*
	** Get the list.
	*/
    list = (const char**) dw->fsel.dirList->list[DL_DIRS].names;

	/*
	** No list yet or list is empty.
	*/
    if (list == 0 || list[0] == 0) {
	Cres->count = 0;
	return 0;
    }

  /*
  ** Loop until the directory that is visible within the widget lists
  ** really correspond to the directory specified in the completion text.
  ** This may envolves no more than one trip with a scanning of the directory.
  ** In case of error, abort and return to caller.
  */
    while (1) {
	char*		ndir;
	struct stat	st;

	/*
	** Get current string (This is the reference).
	*/
	XtSetArg (args[0], XtNstring, &file);
	XtGetValues (dw->fsel.dirValue, args, 1);

	ndir = Dirname (file, &bname);

	/*
	** Directory wasn't changed.
	*/
	res = stat (file, &st);
	if (res == 0 && IsSameFile (&st, &dw->fsel.dirList->dirStat)) {
	    XtFree (ndir);
	    break;
	}

	/*
	** Get stat info to detect whether this is the same directory.
	*/
	res = stat (ndir, &st);
	if (res < 0) {
	    XtFree (ndir);
	    return -1;
	}

	if (IsSameFile (&st, &dw->fsel.dirList->dirStat)) {
	    XtFree (ndir);
	    break;	/* EXIT point of while() loop */
	}

	/*
	** Get back the / 
	*/
	if (bname > &file[1])
	  bname--;

	/*
	** Save the path (it will be changed later by ChangeDirectory).
	*/
	bname = XtNewString (bname);

	/*
	** Scan the directory, update the lists.
	*/
	res = ChangeDirectory (dw, ndir, DIR_ABSOLUTE);
	if (res != 0) {
	    XtFree ((char*)bname);
	    XtFree (ndir);
	    return -1;
	}

	list = (const char**) dw->fsel.dirList->list[DL_DIRS].names;

	/*
	** The directory string has changed. Append the name that must
	** be completed.
	*/
	textBlock.length = strlen (bname);
	if (textBlock.length > 0) {
	    pos = strlen (ndir);
	    textBlock.ptr = (char*) bname;
	    textBlock.firstPos = 0;
	    XawTextReplace (dw->fsel.dirValue, pos, pos, &textBlock);
	    pos += textBlock.length;
	    XawTextSetInsertionPoint (dw->fsel.dirValue, pos + 1);
	}
	XtFree ((char*)bname);
	XtFree (ndir);
    }

	/*
	** Build the completion addendum in `Cres'.
	*/
    FindCompletion (Cres, bname, list);

	/*
	** Append the completion string.
	*/
    if (Cres->len > 0) {
	pos = strlen (file);
	textBlock.firstPos = 0;
	textBlock.length   = Cres->len;
	textBlock.ptr      = Cres->string;
	XawTextReplace (dw->fsel.dirValue, pos, pos, &textBlock);
	pos += Cres->len + 1;
    }

	/*
	** Exact match with a directory. Enter that directory by showing
	** its content. Be smart and append a / for the next completion.
	*/
    if (Cres->count == 1) {
		/*
		** Get current directory string (Since it has changed).
		*/
	XtSetArg (args[0], XtNstring, &file);
	XtGetValues (dw->fsel.dirValue, args, 1);

	pos = strlen (file);
	res = ChangeDirectory (dw, file, DIR_ABSOLUTE);
	if (res == 0) {
	    textBlock.firstPos = 0;
	    textBlock.length = 1;
	    textBlock.ptr = "/";
	    XawTextReplace (dw->fsel.dirValue, pos, pos, &textBlock);
	}
    }

	/*
	** Set caret position if string was modified.
	*/
    if (Cres->len > 0 || Cres->count == 1) {
	XawTextSetInsertionPoint (dw->fsel.dirValue, pos + 1);
    }

    return 0;
}


/*
** Role :	Perform file name completion for a file name.
*/
static int
FileCompletion F2(FileSelectorWidget, dw, CompletionResult_t*, Cres)
{
    const char**	list;
    const char*		file;
    Arg			args[1];
    XawTextPosition	pos;
    XawTextBlock	textBlock;

	/*
	** Get the list.
	*/
    list = (const char**) dw->fsel.fileList;

	/*
	** No list yet or list is empty.
	*/
    if (list == 0 || list[0] == 0) {
	Cres->count = 0;
	return 0;
    }

	/*
	** Get current string (This is the reference).
	*/
    XtSetArg (args[0], XtNstring, &file);
    XtGetValues (dw->fsel.fileValue, args, 1);

	/*
	** Build the completion addendum in `Cres'.
	*/
    FindCompletion (Cres, file, list);

	/*
	** Append the completion string and set caret at end of string.
	*/
    if (Cres->len > 0) {
	pos = strlen (file);
	textBlock.firstPos = 0;
	textBlock.length   = Cres->len;
	textBlock.ptr      = Cres->string;
	XawTextReplace (dw->fsel.fileValue, pos, pos, &textBlock);
	pos += Cres->len + 1;
	XawTextSetInsertionPoint (dw->fsel.fileValue, pos);
    }

    return 0;
}


/* ----------------------------------------------------------------------
**
**		Public actions
**
** -------------------------------------------------------------------- */

/*
** Role :	Perform a file name or a directory name completion.
**
** Name :	Completion().
*/
static void
CompletionNameAction F4(Widget, w, XEvent*, event, String*,params, Cardinal*,n)
{
    FileSelectorWidget	dw;
    CompletionResult_t	Cres;
    int			res;

	/*
	** Get the file selector widget (We may have been called from text
	** widget).
	*/
    dw = FindFileSelectorWidget (w);
    if (dw == 0)
      return;

    Cres.string = 0;
    Cres.len = 0;
    Cres.count = 0;

	/*
	** Get the widget on which the file name completion is to be realized.
	*/
    if (w == dw->fsel.dirValue) {
	res = DirectoryCompletion (dw, &Cres);
    } else {
	res = FileCompletion (dw, &Cres);
    }

    if (Cres.string)
      XtFree (Cres.string);

	/*
	** Completion not found.
	*/
    if (res != 0 || Cres.count == 0 || (Cres.len == 0 && Cres.count > 1)) {
	XBell (XtDisplay (dw), 100);
	return;
    }
}


/*
** Role :	The filter string has been changed and the user wants to
**		show the items according to the filter. Recompute the file
**		list.
**
** Name :	RefilterList()
*/
static void
ChangeFilterAction F4(Widget, w, XEvent*, event, String*, params, Cardinal*, n)
{
    FileSelectorWidget 	dw;

    dw = FindFileSelectorWidget (w);
    if (dw == 0)
      return;

	/*
	** Update the selection (without scanning the directory).
	*/
    SetupFileSelectionList (dw, dw->fsel.dirList);
}


/*
** Role :	Select the file. Get the selected file and directory.
**		Execute all the select callbacks.
**
** Name :	Select()
*/
static void
SelectFileAction F4(Widget, w, XEvent*, event, String*, params, Cardinal*, n)
{
    FileSelectorWidget		dw;
    XawFileSelectionResult	reply;
    Arg				args[1];

    dw = FindFileSelectorWidget (w);
    if (dw == 0)
      return;

    XtSetArg (args[0], XtNstring, &reply.file_name);
    XtGetValues (dw->fsel.fileValue, args, 1);

    XtSetArg (args[0], XtNstring, &reply.directory);
    XtGetValues (dw->fsel.dirValue, args, 1);

    XtCallCallbackList (w, dw->fsel.selectCallbackList, (XtPointer)&reply);
}


/*
** Role :	Rescan the directory and update the two lists.
**
** Name :	RescanDirectory()
*/
static void
RescanAction F4(Widget, w, XEvent*, event, String*, params, Cardinal*, n)
{
    FileSelectorWidget	dw;
    Arg			args[1];
    char*		dir;

	/*
	** Get the file selector widget (We may have been called from text
	** widget).
	*/
    dw = FindFileSelectorWidget (w);
    if (dw == 0)
      return;

	/*
	** Get current directory string (This is the reference).
	*/
    XtSetArg (args[0], XtNstring, &dir);
    XtGetValues (dw->fsel.dirValue, args, 1);

	/*
	** Rescan directory using an absolute path.
	*/
    (void)ChangeDirectory (dw, dir, DIR_ABSOLUTE);
}


/*
** Role :	Timer action executed to check whether the directory
**		has changed or not.
*/
static void
UpdateDirectory F2(XtPointer, closure, XtIntervalId*, id)
{
    FileSelectorWidget	dw = (FileSelectorWidget)(closure);

	/*
	** Only possible if widget is realized and a list is there.
	*/
    if (XtIsRealized ((Widget) dw) && dw->fsel.dirList) {
	struct stat	st;

	int res = stat (dw->fsel.dirList->path, &st);

		/*
		** Update if - time has changed
		**	     - not the same directory (strange case)
		*/
	if (res == 0
	    && (st.st_mtime != dw->fsel.dirList->dirStat.st_mtime
		|| ! IsSameFile (&st, &dw->fsel.dirList->dirStat))) {

	    (void)ChangeDirectory (dw, dw->fsel.dirList->path, DIR_ABSOLUTE);
	}
    }

    dw->fsel.updateTimer = 0;

	/*
	** Restart the timer if still enabled.
	*/
    if (dw->fsel.updateTime > 0) {
	XtAppContext app = XtWidgetToApplicationContext((Widget) dw);
	dw->fsel.updateTimer = XtAppAddTimeOut (app,
						dw->fsel.updateTime * 1000,
						UpdateDirectory, closure);
    }
}
