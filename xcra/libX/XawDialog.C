// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Dialog Boxes (C++ encapsulation)
// Author	: S. Carrez
// Date		: Sat Feb 25 13:58:01 1995
// Version	: $Id: XawDialog.C,v 1.13 2000/02/23 08:19:19 ciceron Exp $
// Project	: Xcra
// Keywords	: Dialog, Xaw
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
// class XawDialog		TopLevel application or dialog box top level
// class XawErrorMessage	Error alert box
// class XawText		Generic text/label/command/toggle item
// class XawMenu		Menu popup
// class XawMenuItem		Item in the menu popup
// class XawMenuSep		Separator line in the menu popup
// class XawConfirmDialog	Confirmation dialog box
//
#include "config.H"

#include <stdio.h>
#include <signal.h>
#include <new>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/AsciiText.h>

#include "Assert.H"
#include "Error.H"
#include "XawDialog.H"
#include "XawHelp.H"

//
// List of `XawDialog' objects. This represents the list of top level
// windows. This list is scanned to identify a window when an Xt action
// is activated (Xt actions do not have closures).
//
static dlist xDialogs;

//
// Toplevel dialog box. Only one top level dialog box can be created
// by an application. The top level dialog box only supports the
// creation of the application context. This is not very restrictive
// because in most cases only one application context is used.
//
XawDialog* xAppMainDialog = 0;

//
// WM_DELETE_WINDOW atom.
//
static Atom xWmDeleteWindow;


// ----------------------------------------------------------------------
//
//			Error handlers
//
// ----------------------------------------------------------------------

    static int
AppErrorHandler(Display* _dp, XErrorEvent* _err)
{
    char msg[80];

    if (_err) {
        if (_err->error_code == BadMatch)
            return 0;
        
        XGetErrorText(_dp, _err->error_code, msg, sizeof(msg));
    } else {
        sprintf(msg, "Unknown error code");
    }

    fprintf(stderr, "XError: code %s\n", msg);
    if (_err) {
        fprintf(stderr, "XError: XID  %ld\n", _err->resourceid);
        fprintf(stderr, "XError: Major %u Minor %u\n",
                (unsigned)_err->request_code, (unsigned)_err->minor_code);
    }
    
    XawDialog::closeAll();
    return 0;
}

    static void
AppXtErrorHandler(String _message)
{
	//
	// Notify the application here.
	//
    fprintf(stderr, "Fatal error...\n");

	//
	// Restore X-toolkit handler and call it.
	//
    XtSetErrorHandler(NULL);
    XtError(_message);

    XawDialog::closeAll();
}

    static void
AppXtErrorMsgHandler(String _name, String _type, String _clss,
		     String _defaultp, String* _params, Cardinal* _num_params)
{
    fprintf(stderr, "Fatal error...\n");

	//
	// Restore X-toolkit handler and call it.
	//
    XtSetErrorMsgHandler(NULL);
    XtErrorMsg(_name, _type, _clss, _defaultp, _params, _num_params);
    XawDialog::closeAll();
}

    static void
catchSig(int _sig)
{
    fprintf(stderr, "Caught signal %d\n", _sig);
#ifdef SIGTSTP
    if (_sig != SIGTSTP && _sig != SIGTTIN && _sig != SIGTTOU && _sig != SIGCONT)
        XawDialog::closeAll();
#endif
}

#ifdef __GNUG__
#define MSG_MEMORY	"Virtual memory exhausted...\n"

    static void
new_handler_func()
{
    static bool beenhere = false;

    (void) write(STDERR_FILENO, MSG_MEMORY, strlen(MSG_MEMORY));
    if (beenhere == false) {
	beenhere = true;
	XawDialog::closeAll();
    }
    _exit(1);
}

#if __GNUC__ >= 3
#define set_new_handler(X)
#endif

#endif

// ----------------------------------------------------------------------
//
//			Actions
//
// ----------------------------------------------------------------------

    static void
closeAC(Widget _w, XEvent*, String*, Cardinal*)
{
    XawDialog* xd = XawDialog::find(_w);

    if (xd) {
	xd->cancel();
    }
}

    static void
activateAC(Widget _w, XEvent*, String*, Cardinal*)
{
    XawDialog* xd = XawDialog::find(_w);

    if (xd) {
	xd->activate();
    }
}

    static void
helpAC(Widget _w, XEvent*, String*, Cardinal*)
{
    XawDialog* xd = XawDialog::find(_w);

    if (xd) {
	XawHelpDialog::open(xd->name());
    }
}

    static void
nextTabGroupAC(Widget _w, XEvent*, String*, Cardinal*)
{
    XawDialog* xd = XawDialog::find(_w);

    if (xd) {
	xd->nextTabGroup();
    }
}

    static void
selectTabGroupAC(Widget _w, XEvent*, String*, Cardinal*)
{
    XawDialog* xd = XawDialog::find(_w);

    if (xd) {
	xd->selectTabGroup(_w);
    }
}

    static void
focusViewportAC(Widget _w, XEvent*, String*, Cardinal*)
{
    XawDialog* xd = XawDialog::find(_w);

    if (xd) {
	xd->focusViewport();
    }
}

static XtActionsRec actions_table[] = {
  {"do-close",		closeAC		}, // Close the dialog box and cancel
  {"do-activate",	activateAC	}, // Activate (validates) the dialog
  {"do-help",		helpAC		}, // Give help on the current dialog
  {"focus-tab-group",	focusViewportAC	}, // Re-center viewport
  {"select-tab-group",	selectTabGroupAC}, // Next text field of dialog
  {"next-tab-group",	nextTabGroupAC	}  // Select particular text field
};


// ----------------------------------------------------------------------
//
//			Callbacks
//
// ----------------------------------------------------------------------

    static void
OkCB(Widget, XtPointer _tag, XtPointer)
{
    XawDialog* xd = (XawDialog*) _tag;

    xd->activate();
}

    static void
CancelCB(Widget, XtPointer _tag, XtPointer)
{
    XawDialog* xd = (XawDialog*) _tag;

    xd->cancel();
}

    static void
HelpCB(Widget, XtPointer _tag, XtPointer)
{
    XawDialog* xd = (XawDialog*) _tag;

    XawHelpDialog::open(xd->name());
}

    static void
ButtonCB(Widget, XtPointer _tag, XtPointer _data)
{
    XawText* xtext = (XawText*) _tag;

    xtext->select(_data);
}

    static void
menuCB(Widget, XtPointer _tag, XtPointer)
{
    XawMenuItem* item = (XawMenuItem*) _tag;

    item->activate();
}

// ----------------------------------------------------------------------
//
//			XawDialog
//
// ----------------------------------------------------------------------

typedef struct {
   char*	xHelpFile;		// Help file
   Boolean	xAutoPlace;		// Auto place dialog boxes
   Boolean	xSemErrors;		// Enable semantic errors
   Boolean	xTransientShell;	// Use transient shell
   Boolean	xRingBell;		// Ring bell on errors
} xtData;

#undef Offset
#define Offset(field) (XtOffsetOf(xtData, field))

static XtResource xtResources[] = {
	//
	// Find a place for the dialog box when opening it.
	//
  {"autoPlace", "AutoPlace", XtRBoolean, sizeof(Boolean),
     Offset(xAutoPlace), XtRImmediate, (XtPointer) True},

	//
	// Enables semantic errors.
	//
  {"semanticErrors", "SemanticErrors", XtRBoolean, sizeof(Boolean),
     Offset(xSemErrors), XtRImmediate, (XtPointer) True},

	//
	// Ring the bell if an error is detected.
	//
  {"ringBell", "RingBell", XtRBoolean, sizeof(Boolean),
     Offset(xRingBell), XtRImmediate, (XtPointer) True},

	//
	// Use a transient shell for dialog boxes.
	//
  {"transientShell", "TransientShell", XtRBoolean, sizeof(Boolean),
     Offset(xTransientShell), XtRImmediate, (XtPointer) True},

	//
	// Help file.
	//
  {"helpFile",  "HelpFile", XtRString, sizeof(char*),
     Offset(xHelpFile), XtRString, (XtPointer) "help.file"}
};

static xtData drsrc;


//
// Initializes the X11 toolkit and create the top level window.
//
XawDialog::XawDialog(const char* _name, XrmOptionDescList _options,
		     Cardinal _num_options, String* _fallback,
		     int* _argc, char* _argv[])
{
    require(xAppMainDialog == 0);

    xAppMainDialog = this;
    xDialogs.insert(this);

    xName	= _name;
    xOk         = None;
    xHelp	= None;
    xCancel     = None;
    xAutoDelete = true;
    xAutoPlace  = false;
    xTextFocus  = 0;

    	//
	// Catch errors raised by the Xlib and X Toolkit.
	//
    XSetErrorHandler(AppErrorHandler);
    XSetIOErrorHandler((XIOErrorHandler)AppErrorHandler);
    XtSetErrorHandler(AppXtErrorHandler);
    XtSetErrorMsgHandler(AppXtErrorMsgHandler);

#ifdef __GNUG__
	//
	// Catch C++ new error handler.
	//
    set_new_handler(new_handler_func);
#endif

	//
	// Catch every signal to abort correctly.
	//
    for (int i = 1; i < 31; i++) {
#ifdef SIGCONT
        if (i == SIGCONT) continue;
        if (i == SIGSTOP) continue;
        if (i == SIGTSTP) continue;
        if (i == SIGTTIN) continue;
        if (i == SIGTTOU) continue;
#endif
#ifdef SIGCHLD
        if (i == SIGCHLD) continue;
#endif
	signal(i, catchSig);
    }
    signal(SIGINT, SIG_IGN);
#ifdef SIGTSTP
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
#endif

#ifdef HAVE_X11R6
    XtSetLanguageProc(NULL, NULL, NULL);
#endif

    xDialog =
    xTopLevel = XtAppInitialize(&xContext, _name, _options, _num_options,
				_argc, _argv, _fallback, NULL, 0);

	//
	// Get our resources.
	//
    XtGetApplicationResources(xTopLevel, (XtPointer)&drsrc,
			      xtResources, XtNumber(xtResources),
			      NULL, 0);

    XtAppAddActions(xContext, actions_table, XtNumber(actions_table));

    xForm = XtCreateManagedWidget("tform", formWidgetClass,
				  xTopLevel, NULL, 0);

    XawSimpleMenuAddGlobalActions(xContext);

    xWmDeleteWindow = XInternAtom(XtDisplay(xTopLevel),
				  "WM_DELETE_WINDOW", False);
}


//
// Create a transcient dialog box with several buttons (Ok, Help, Cancel)
// according to `_mode'.
//
XawDialog::XawDialog(const char* _name, const int _mode)
{
    Widget tForm;

    require(xAppMainDialog != 0);

    xDialogs.insert(this);

    xName	= _name;
    xContext    = xAppMainDialog->Context();
    xTopLevel   = xAppMainDialog->TopLevel();
    xAutoDelete = true;
    xAutoPlace  = drsrc.xAutoPlace;
    xTextFocus  = 0;
    xOk         = None;
    xHelp       = None;
    xCancel     = None;

    if (xDialogs.size() > XAW_DIALOG_MAX) {
	static bool warn = false;

	if (warn == false) {
	    warn = true;
	    Error::printf(MSG_TOO_MANY_DIALOGS, xDialogs.size());
	    warn = false;
	}
    }

	//
	// Create the transient shell or top level shell.
	//	      (not iconifiable)	 (iconifiable)
	//
    xDialog = XtCreatePopupShell(_name,
				 ((drsrc.xTransientShell == True
				   && !(_mode & XAW_DIALOG_TOPLEVEL))
				   ? transientShellWidgetClass
				   : topLevelShellWidgetClass),
				 xTopLevel, NULL, 0);

    tForm = XtCreateManagedWidget("topForm", formWidgetClass,
				  xDialog, NULL, 0);

    if (_mode & (XAW_DIALOG_OK|XAW_DIALOG_CANCEL)) {
	xForm = XtCreateManagedWidget("form", formWidgetClass,
				      tForm, NULL, 0);

	tForm = XtVaCreateManagedWidget("buttonForm", formWidgetClass,
				 	tForm, XtNfromVert, xForm, NULL);

		//
		// Create the OK, Cancel buttons.
		//
	if (_mode & XAW_DIALOG_OK) {
	    xOk = XtVaCreateManagedWidget("okButton", commandWidgetClass,
					  tForm, NULL);
	    XtAddCallback(xOk, XtNcallback, OkCB, (XtPointer) this);
	}

	xHelp = XtVaCreateManagedWidget("helpButton",
					commandWidgetClass,
					tForm, XtNfromHoriz, xOk, NULL);
	XtAddCallback(xHelp, XtNcallback, HelpCB, (XtPointer) this);

	if (_mode & XAW_DIALOG_CANCEL) {
	    xCancel = XtVaCreateManagedWidget("cancelButton",
					      commandWidgetClass,
				 	      tForm, XtNfromHoriz, xHelp,
					      NULL);
	    XtAddCallback(xCancel, XtNcallback, CancelCB, (XtPointer) this);
	}
    } else {
	xForm = tForm;
    }
}


//
// Delete the dialog box.
//
XawDialog::~XawDialog()
{
    XtDestroyWidget(xDialog);

    xDialogs.remove(this);
    if (this == xAppMainDialog) {
	xAppMainDialog = 0;
    }
}


//
// Return the widget holding the different dialog items.
//
    Widget
XawDialog::form() const
{
    return xForm;
}


//
// The Ok button has been pressed. Activate the actions associated with
// the dialog box.
//
    void
XawDialog::activate()
{
    close();
    if (xAutoDelete) {
	delete this;
    }
}


//
// The Cancel button has been pressed. Cancel the dialog box and close it.
//
    void
XawDialog::cancel()
{
    close();
    if (xAutoDelete) {
	delete this;
    }
}


//
// Update the content of the dialog items.
//
    void
XawDialog::update()
{
}


//
// Close the dialog box (pop it down).
//
    void
XawDialog::close()
{
    if (xDialog != xTopLevel) {
	XtPopdown(xDialog);
    }
}


//
// Popup the dialog box. If the dialog box is not realized, it it first
// realized and we find a place where to put it on the screen (unless the
// auto-place feature is disabled).
//
    void
XawDialog::popup()
{
    if (XtIsRealized(xDialog) == False) {
	XtRealizeWidget(xDialog);

	setupWindowProtocols();

	if (xAutoPlace && xDialog != xTopLevel) {
	    placeDialog();
	}
    }
    if (xDialog != xTopLevel) {
	XtPopup(xDialog, XtGrabNone);
    }
}


//
// Setup the window protocol for this dialog box to use the do-close action.
//
    void
XawDialog::setupWindowProtocols()
{
	//
	// Handle the WM_DELETE_WINDOW protocol
	//
    XtOverrideTranslations(xDialog,
		XtParseTranslationTable("<Message>WM_PROTOCOLS: do-close()"));

    (void)XSetWMProtocols(XtDisplay(xDialog), XtWindow(xDialog),
			  &xWmDeleteWindow, 1);
}


//
// Find a place on the screen where to put the new dialog box.
// The algorithm is not always very smart but simple. We just try
// to put the new dialog box below/above or on the left/right side
// of the previous dialog box. We make sure that the window is
// completely inside the screen. (hacked from the bitmap application).
//
    void
XawDialog::placeDialog()
{
    Position popup_x, popup_y, top_x, top_y;
    Dimension popup_width, popup_height, top_width, top_height, border_width;
    Widget w;
    
	//
	// Get toplevel window dimension.
	//
    if (prev() != 0) {
	w = prev()->xDialog;
    } else if (next() != 0 && next()->next() != 0) {
	w = next()->xDialog;
    } else {
	w = xTopLevel;
    }
    XtVaGetValues(w, XtNx, &top_x, XtNy, &top_y,
		  XtNwidth, &top_width, XtNheight, &top_height,
		  NULL);

	//
	// Get dialog window dimension.
	//
    XtVaGetValues(xDialog, XtNwidth, &popup_width, XtNheight, &popup_height,
		  XtNborderWidth, &border_width,
		  NULL);

    Display* dp = XtDisplay(xDialog);
    unsigned displayWidth = DisplayWidth(dp, DefaultScreen(dp));

    popup_x = top_x - ((popup_width - top_width) / 2);
    if ((Dimension) (popup_x + popup_width) > displayWidth) {
	popup_x = displayWidth - popup_width;
    }

    popup_y = top_y + top_height;
    if ((Dimension) (popup_y + popup_height) > DisplayHeight(dp, DefaultScreen(dp))) {
	popup_y = top_y - popup_height;
	if (popup_y < 0) {
	    popup_y = top_y;
	    if ((Dimension) (top_x + top_width + popup_width) > displayWidth) {
		popup_x = top_x - popup_width;
	    } else {
		popup_x = top_x + top_width;
	    }
	}
    }
    if (popup_y < 0) {
	popup_y = 0;
    }
    if (popup_x < 0) {
	popup_x = 0;
    }

    XtVaSetValues(xDialog, XtNx, popup_x, XtNy, popup_y, NULL);

    XWarpPointer(dp, XtWindow(xTopLevel), XtWindow(xDialog), 
		 0, 0, top_width, top_height,
	         popup_width / 2, popup_height / 2);
}


//
// Enter in the X11 application main loop.
//
    void
XawDialog::mainLoop()
{
    XtAppMainLoop(xContext);
}


//
// Scroll the viewport so that the text widget which has the focus is
// completely visible. (We can have to scroll forward or backward).
// This is called each time the text input focus is changed.
//
    void
XawDialog::focusViewport()
{
    Position x, y, x0, y0, h0, w0;
    Position xabs = 0;
    Position yabs = 0;
    Position bw;

    Widget childViewport;
    Widget viewport;

    if (xTextFocus == 0)
	return;

    viewport = *xTextFocus;

	//
	// Get the dimension of the text widget
	//
    XtVaGetValues(viewport, XtNheight, &h0, XtNwidth, &w0,
		  XtNborderWidth, &bw, NULL);
    h0 += bw + 2;
    w0 += bw + 2;

	//
	// Loop until we find a viewport widget in the ancestor's of
	// the text widget. Then, check that the text widget is visible
	// and if not scroll the viewport to make it visible either
	// at top or at bottom of the viewport.
	//
    while (1) {
	childViewport = viewport;

		//
		// Get relative coordinate of the current widget
		// within its parent.
		//
	XtVaGetValues(childViewport, XtNx, &x, XtNy, &y, NULL);

		//
		// Check whether the parent is a viewport.
		//
	viewport = XtParent(viewport);
	if (viewport == None)
	    return;

	if (XtClass(viewport) == viewportWidgetClass)
	    break;

		//
		// Compute the absolute coordinate of the text widget
		// within the child of the viewport widget.
		//
	xabs = xabs + x;
	yabs = yabs + y;
    }

	//
	// Get the view port dimension.
	//
    Position width, height;
    XtVaGetValues(viewport, XtNheight, &height, XtNwidth,  &width, NULL);

	//
	// Verify/correct the placement of the text widget so that
	// it is visible.
	//
    x0 = xabs + x;
    if (x0 < 0) {
	x = x - x0;
    } else if (x0 + w0 > width) {
	x = x - (x0 + w0 - width);
    }

    y0 = yabs + y;
    if (y0 < 0) {
	y = y - y0;
    } else if (y0 + h0 > height) {
	y = y - (y0 + h0 - height);
    }

	//
	// Update the scroll positions.
	//
    XawViewportSetCoordinates(viewport, -x, -y);
}


//
// Select the text field `_nextFocus' as the new text field which has
// the input focus.
//
    void
XawDialog::selectTabGroup(XawText* _nextFocus)
{
	//
	// Find the next text item and undisplay the caret (visual feedback).
	//
    if (xTextFocus && xTextFocus != _nextFocus) {
	XtVaSetValues(*xTextFocus, XtNdisplayCaret, False, NULL);
    }

	//
	// Setup the keyboard focus to the new text widget.
	// Display the caret to provide a visual feedback.
	//
    if (_nextFocus != xTextFocus) {
	ensure(_nextFocus != 0);

	Widget w = *_nextFocus;
	xTextFocus = _nextFocus;

        XtSetKeyboardFocus(xForm, w);
        XtVaSetValues(w, XtNdisplayCaret, True, NULL);

	if (XtIsRealized(xForm)) {
	    focusViewport();
	}
    }
}


//
// Select the widget `_w' has having the text input focus. If `_w' is
// not a text widget, nothing is done.
//
    void
XawDialog::selectTabGroup(Widget _w)
{
    for (XawText* t = (XawText*) xText.first(); t; t = t->next()) {
	if (_w == (Widget) *t) {
	    selectTabGroup(t);
	    break;
	}
    }
}


//
// Change the input focus to the next text field of the dialog box.
//
    void
XawDialog::nextTabGroup()
{
    XawText* nextFocus;

	//
	// Find the next text item and undisplay the caret (visual feedback).
	//
    if (xTextFocus) {
	nextFocus = xTextFocus->next();
	if (nextFocus == 0) {
	    nextFocus = (XawText*) xText.first();
	}
    } else {
	nextFocus = (XawText*) xText.first();
    }

    selectTabGroup(nextFocus);
}



//
// Insert the text field `_text' in the list of text fields used to
// manage tab groups.
//
    void
XawDialog::insertTabGroup(XawText* _text)
{
    require(_text != 0);
    
    XawDialog* xd = XawDialog::find(*_text);

    ensure(xd != 0);

    xd->xText.insert(_text, xd->xText.last());
    if (xd->xTextFocus == 0) {
	xd->nextTabGroup();
    }
}


//
// Remove the text field `_text' from the list of text fields used to
// manage tab groups.
//
    void
XawDialog::removeTabGroup(XawText* _text)
{
    require(_text != 0);
    
    XawDialog* xd = XawDialog::find(*_text);

    ensure(xd != 0);

	//
	// Move the keyboard focus to the next tab group.
	// (Disable focus if last text item).
	//
    if (_text == xd->xTextFocus) {
	xd->nextTabGroup();
	if (_text == xd->xTextFocus) {
	    xd->xTextFocus = 0;
	}
    }

    xd->xText.remove(_text);
}



//
// Find a dialog given one of its widgets (whichever).
//
    XawDialog*
XawDialog::find(Widget _w)
{
    XawDialog* d;

	//
	// Find the dialog box which contains the widget `_w'.
	// Check the parent widget and go on until we reach the top level
	// widget.
	//
    do {
	for (d = (XawDialog*) xDialogs.first(); d; d = d->next()) {
	    if (d->xDialog == _w) {
		return d;
	    }
	}
	_w = XtParent(_w);
    } while (_w != None);

    return (XawDialog*) 0;
}


//
// Close all the dialog boxes (emergency exit).
//
    void
XawDialog::closeAll()
{
    XawDialog* d;

	//
	// Close the dialogs. We only close the toplevel dialog box
	// because `closeAll' is called in case of serious error.
	//
    d = (XawDialog*) xDialogs.last();
    if (d != (XawDialog*) NULL) {
	ensure(d->xDialog == d->xTopLevel); // This is the top level dialog.

		//
		// Cancel the dialog (this should activate the destructor).
		//
	d->cancel();

		//
		// Verify that it is really closed or hard kill it.
		//
	if (d == (XawDialog*) xDialogs.last()) {
	    delete d;
	}
    }

    _exit(1);
}


//
// Update all the dialog box items (not used yet).
//
    void
XawDialog::updateAll()
{
    for (XawDialog* d = (XawDialog*) xDialogs.first(); d; d = d->next()) {
	d->update();
    }
}


//
// Return the help file (configure by resource) used by this application.
//
    const char*
XawDialog::helpFile()
{
    return drsrc.xHelpFile;
}


// ----------------------------------------------------------------------
//
//			XawErrorDialog
//
// ----------------------------------------------------------------------

//
// Create and open the error dialog box. The error dialog box only contains
// a text window and an Ok button (used to close the dialog box).
//
XawErrorDialog::XawErrorDialog(const char* _name)
: XawDialog(_name, XAW_DIALOG_OK)
{
    xText = XtVaCreateManagedWidget("error", asciiTextWidgetClass, xForm, 
				    XtNeditType, XawtextAppend,
				    XtNtype, XawAsciiString,
				    XtNuseStringInPlace, False,
				    XtNdisplayCaret, False,
				    XtNstring, "",
				    NULL);
    popup();
}


//
// Delete the error dialog box.
//
XawErrorDialog::~XawErrorDialog()
{
}


//
// Report the error message `_msg' in the error dialog box.
//
    void
XawErrorDialog::report(const char* _msg)
{
    require(_msg != 0);
    
    XawTextBlock    textBlock;
    XawTextPosition lastPos;

    textBlock.firstPos = 0;
    textBlock.length   = strlen(_msg);
    textBlock.ptr      = (char*) _msg;

    lastPos = XawTextGetInsertionPoint(xText);
    XawTextReplace(xText, lastPos, lastPos, &textBlock);
    XawTextSetInsertionPoint(xText, lastPos + textBlock.length);
}


// ----------------------------------------------------------------------
//
//			XawConfirmDialog
//
// ----------------------------------------------------------------------

//
// Create a confirmation dialog box. The confirmation message is
// define in the application resources.
//
XawConfirmDialog::XawConfirmDialog(const char* _name, const int _mode)
 : XawDialog(_name, _mode), xcdDone(false)
{
    xAutoDelete = false;
    xText = XtCreateManagedWidget("confirm", labelWidgetClass,
		 		  xForm, NULL, 0);

    XtRealizeWidget(xDialog);

    setupWindowProtocols();

    if (xAutoPlace) {
	placeDialog();
    }
    XtPopup(xDialog, XtGrabExclusive);

	//
	// Wait until the dialog is validated and canceled.
	//
    while (xcdDone == false) {
	XtAppProcessEvent(xAppMainDialog->Context(), XtIMAll);
    }
}


//
// Delete the confirmation box.
//
XawConfirmDialog::~XawConfirmDialog()
{
}


//
// The operation is confirmed by the user.
//
    void
XawConfirmDialog::activate()
{
    xcdResult = true;
    xcdDone   = true;
    XawDialog::activate();
}


//
// The operation is canceled.
//
    void
XawConfirmDialog::cancel()
{
    xcdResult = false;
    xcdDone   = true;
    XawDialog::cancel();
}


// ----------------------------------------------------------------------
//
//			XawText
//
// ----------------------------------------------------------------------

#ifdef HAVE_X11R5
static bool      xDestroyWidgets = true;

    void
XawText::dontDestroyWidgets(const bool _f)
{
    xDestroyWidgets = _f;
}
#endif

static Widget	xRadioGroup = None;


//
// Create a new text/toggle/label/button item.
//
XawText::XawText(const char* _name, const int _mode, const char* _value,
		 Widget _form, Widget _top)
{
    unsigned ac;
    Arg  args[10];
    char labelName[64];
    Widget prev;

    require(strlen(_name) < sizeof(labelName) - 10);

    if (_mode & XAW_FIRST_GROUP) {
	xRadioGroup = None;
    }

	//
	// Create a label.
	//
    if (_mode & XAW_LABEL) {
	sprintf(labelName, "label%s", _name);

	ac = 2;
	XtSetArg(args[0], XtNfromVert, _top);
	XtSetArg(args[1], XtNborderWidth, 0);
	if (_value != 0) {
	    XtSetArg(args[2], XtNlabel, _value);
	    ac = 3;
	}

	ensure(ac < XtNumber(args));
	xLabel = XtCreateManagedWidget(labelName, labelWidgetClass,
				       _form, args, ac);
	prev = xLabel;
    } else {
	xLabel = None;
	prev   = None;
    }

	//
	// Create a button and bind the callback.
	//
    if (_mode & XAW_COMMAND) {
	sprintf(labelName, "cmd%s", _name);

	ac = 1;
	XtSetArg(args[0], XtNfromVert, _top);
	if (xLabel) {
	    XtSetArg(args[1], XtNfromHoriz, xLabel);
	    ac = 2;
	}
	if (_value) {
	    XtSetArg(args[ac], XtNlabel, _value); ac++;
	}
	ensure(ac < XtNumber(args));
	xCommand = XtCreateManagedWidget(labelName, commandWidgetClass,
					 _form, args, ac);
	XtAddCallback(xCommand, XtNcallback, ButtonCB, (XtPointer)this);

	prev = xCommand;
    } else {
	xCommand = None;
    }

	//
	// Create a toggle widget.
	//
    if (_mode & XAW_TOGGLE) {
	sprintf(labelName, "tog%s", _name);

	ac = 2;
	XtSetArg(args[0], XtNfromVert, _top);
	XtSetArg(args[1], XtNradioData, (XtPointer)this);
	if (prev != None) {
	    XtSetArg(args[ac], XtNfromHoriz, prev); ac++;
	}
	if (xRadioGroup != None) {
	    XtSetArg(args[ac], XtNstate, False); ac++;
	    XtSetArg(args[ac], XtNradioGroup, xRadioGroup); ac++;
	} else {
	    XtSetArg(args[ac], XtNstate, True); ac++;
	}

	if (_value) {
	    XtSetArg(args[ac], XtNlabel, _value); ac++;
	}
	ensure(ac < XtNumber(args));
	xToggle = XtCreateManagedWidget(labelName, toggleWidgetClass,
					_form, args, ac);
	XtAddCallback(xToggle, XtNcallback, ButtonCB, (XtPointer)this);

	if (xRadioGroup == None) {
	    xRadioGroup = xToggle;
	}
	prev = xToggle;
    } else {
	xToggle = None;
    }

	//
	// Create the text widget.
	//
    if (_mode & XAW_TEXT) {
	ac = 7;

	XtSetArg(args[0], XtNfromVert, _top);
	XtSetArg(args[1], XtNeditType, XawtextEdit);
	XtSetArg(args[2], XtNtype, XawAsciiString);
	XtSetArg(args[3], XtNresize, XawtextResizeBoth);
	XtSetArg(args[4], XtNstring, "");
	XtSetArg(args[5], XtNuseStringInPlace, False);
	XtSetArg(args[6], XtNdisplayCaret, False);
	if (prev != None) {
            XtSetArg(args[7], XtNfromHoriz, prev);
	    ac = 8;
	}

	ensure(ac < XtNumber(args));
	xText = XtCreateManagedWidget(_name, asciiTextWidgetClass,
				      _form, args, ac);

	XawDialog::insertTabGroup(this);
    } else {
	xText = None;
    }
}    


//
// Delete the text/label/toggle/button item.
//
XawText::~XawText()
{
    if (xText != None) {
	XawDialog::removeTabGroup(this);
#ifdef HAVE_X11R5
	if (xDestroyWidgets)
#endif
	XtDestroyWidget(xText);
    }
#ifdef HAVE_X11R5
    if (xDestroyWidgets == 0)
	return;
#endif
    if (xLabel != None) {
	XtDestroyWidget(xLabel);
    }
    if (xCommand != None) {
	XtDestroyWidget(xCommand);
    }
    if (xToggle != None) {
	XtDestroyWidget(xToggle);
    }
}


//
// Method called when the toggle or button are pressed.
//
    void
XawText::select(XtPointer)
{
}


//
// Set the value of the text field to edit (if any).
//
    XawText&
XawText::operator= (const char* _text)
{
    if (xText != None) {
	XtVaSetValues(xText, XtNstring, _text, NULL);
    }
    return *this;
}


//
// Return the value of the text field (or 0 if none).
//
XawText::operator char*()
{
    char* s;

    if (xText != None) {
	XtVaGetValues(xText, XtNstring, &s, NULL);
    } else {
	s = 0;
    }
    return s;
}


//
// Return the widget of this item.
//
XawText::operator Widget() const
{
    return (xText != None) ? xText
		: ((xLabel != None) ? xLabel
			: ((xCommand != None) ? xCommand
				: xToggle));
}


//
// Set the new label value. This does not affect the text field but
// the label/toggle/command widgets.
//
    void
XawText::setLabel(const char* _value)
{
    Widget w;

    w = (xLabel != None) ? xLabel
		: ((xCommand != None) ? xCommand
			: xToggle);

    if (w != None) {
	XtVaSetValues(w, XtNlabel, _value, NULL);
    }
}


//
// Return the label string.
//
    const char*
XawText::getLabel()
{
    const char* s = "";
    Widget w;

    w = (xLabel != None) ? xLabel
		: ((xCommand != None) ? xCommand
			: xToggle);

    if (w != None) {
	XtVaGetValues(w, XtNlabel, &s, NULL);
    }
    return s;
}



//
// Mark this item with the error mark `_value'. A true value means that
// the item is not field correctly. The label associated with it is
// tiled (greyed) to produce a user feedback. A false value means that
// the item is (may-be) correctly field.
//
    void
XawText::setSemanticError(const bool _value)
{
    if (xLabel != None && drsrc.xSemErrors == True) {
	if (_value == true) {
	    XtSetSensitive(xLabel, False);
	    if (drsrc.xRingBell == True) {
		XBell(XtDisplay(xLabel), 100);
	    }
	} else {
	    XtSetSensitive(xLabel, True);
	}
    }
}


//
// Look at the string `p' to see whether this is an acceptable string.
// (no control characters).
//
    bool
XawText::acceptableText(const char* _text, const size_t _max,
			const char* _param_name)
{
    require(_text != 0);

	//
	// The string must not be empty and it is bounded by `_max'.
	//
    if (_text[0] == 0) {
	Error::printf(MSG_EMPTY_FIELD, _param_name);
	return false;
    }
    if (strlen(_text) > _max) {
	Error::printf(MSG_LONG_FIELD, _param_name);
	return false;
    }

	//
	// Verify that there are not special character which
	// would break loading/saving of files
	//
    for (; _text[0]; _text++) {
	if (_text[0] >= ' ' && _text[0] < 0x7F)
	    continue;

	Error::printf(MSG_WRONG_FIELD, _param_name);
	return false;
    }
    return true;
}


//
// Clear the radio group leader before the creation of a new toggle item.
//
    void
XawText::resetRadioGroup()
{
    xRadioGroup = None;
}


// ----------------------------------------------------------------------
//
//			XawMenu
//
// ----------------------------------------------------------------------

//
// Create a popup menu.
//
XawMenu::XawMenu(const char* _name, Widget _form, Widget _topLevel)
{
    char mName[128];
    char bName[128];

    require(strlen(_name) < sizeof(mName) - 10);

    sprintf(mName, "%sMenu", _name);

    xmMenu = XtVaCreatePopupShell(mName, simpleMenuWidgetClass,
				  _topLevel, NULL);

    sprintf(bName, "%sButton", _name);
    xmButton = XtVaCreateManagedWidget(bName, menuButtonWidgetClass, _form,
				       XtNmenuName, XtNewString(mName), NULL);
}


//
// Delete a popup menu.
//
XawMenu::~XawMenu()
{
}


// ----------------------------------------------------------------------
//
//			XawMenuItem
//
// ----------------------------------------------------------------------

//
// Add an entry in a popup menu.
//
XawMenuItem::XawMenuItem(const char* _name, XawMenu& _menu)
  : xMenu(_menu)
{
    xItem = XtCreateManagedWidget(_name, smeBSBObjectClass, _menu, NULL, 0);
    XtAddCallback(xItem, XtNcallback, menuCB, (XtPointer) this);

    xMenu.insert(this, xMenu.last());
}


//
// Remove an entry from a popup menu.
//
XawMenuItem::~XawMenuItem()
{
    xMenu.remove(this);
    XtDestroyWidget(xItem);
}


// ----------------------------------------------------------------------
//
//			XawMenuSep
//
// ----------------------------------------------------------------------

//
// Add a separator in a popup menu.
//
XawMenuSep::XawMenuSep(const char* _name, XawMenu& _menu)
: xMenu(_menu)
{
    xItem = XtCreateManagedWidget(_name, smeLineObjectClass, _menu, NULL, 0);
    xMenu.insert(this, xMenu.last());
}


//
// Delete the separator.
//
XawMenuSep::~XawMenuSep()
{
    xMenu.remove(this);
    XtDestroyWidget(xItem);
}


