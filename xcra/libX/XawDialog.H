// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Dialog Boxes (C++ encapsulation)
// Author	: S. Carrez
// Date		: Sat Feb 25 13:58:01 1995
// Version	: $Id: XawDialog.H,v 1.11 2000/02/23 08:19:19 ciceron Exp $
// Project	: Xcra
// Keywords	: Dialog, Xaw
//
// Copyright (C) 1995, 1996, 2009 Stephane Carrez
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
// class XawErrorDialog		Error dialog box
// class XawText		Generic text/label/command/toggle item
// class XawMenu		Popup menu
// class XawMenuItem		Item in a popup menu
// class XawMenuSep		Separator in a popup menu
//
#ifndef _XAW_DIALOG_H_
#define	_XAW_DIALOG_H_

#include "dlist.H"

#include <X11/Intrinsic.h>


//
// C template for generation of Xaw callbacks (calling a C++ method).
//
#define XAW_CALLBACK(NAME, TYPE, METHOD)			\
								\
    static void							\
NAME (Widget _w, XtPointer _tag, XtPointer _data)		\
{								\
    TYPE* xn = (TYPE *)_tag;					\
								\
    xn->METHOD(_w);						\
}


//
// Warn limit above which the user is told about the number of dialog
// boxes it has opened. 
//
#define XAW_DIALOG_MAX	20

#define XAW_DIALOG_OK		0x01	// Create an Ok button.
#define XAW_DIALOG_CANCEL	0x02	// Create a Cancel button.
#define XAW_DIALOG_ALL		(XAW_DIALOG_OK|XAW_DIALOG_CANCEL)
#define XAW_DIALOG_TOPLEVEL	0x08	// Create a topLevel dialog.


class XawText;

// ----------------------------------------------------------------------
// Class :	XawDialog
//
// Role :	Generic and topLevel dialog box
//
// Dialog boxes are linked in a doubled linked list (class `dlink') so
// that we can find the dialog box given one of its widget. This is
// necessary for Xt actions.
//
// The top level class defines 3 virtual methods:
//
// activate	is called when the Ok button associated with the dialog
//		is pressed, or when the `do-activate' action is executed.
//		The purpose of this method is to validate the dialog and
//		execute the actions associated with it.
//
// cancel	is called when the Cancel button is pressed, when
//		the `do-close' action is executed or when a WM_DELETE_WINDOW
//		message is received (dialog closed by window manager).
//
// update	is called when the `updateAll' method is called. Its purpose
//		is to update the dialog box according to the application
//		internal data. This may be used to implement an automatic
//		refresh of dialog boxes.
//
// The constructor builds the support for the dialog box (top level shell,
// main form and buttons) and the destructor performs the necessary cleanup.
// The dialog box is realized and popped up by the `popup' method.
//
// A `Widget' conversion operator is defined for each of the different
// classes defined here. They return the most important widget of the
// object (most important in terms of functionality). These operators are
// intensively used, specially when specifying the XtNfromVert or
// XtNfromHoriz resource.
//
//
class XawDialog : private dlink {
	friend class XawText;
private:	// Next operations are forbidden.
    XawDialog(const XawDialog&);
    void operator=(const XawDialog&);
    void operator&();
protected:
    XtAppContext	xContext;
    Widget		xDialog;
    Widget		xTopLevel;

		//
		// Form widget holding the dialog box items.
		//
    Widget		xForm;

		//
		// Ok/Help/Cancel buttons.
		//
    Widget		xOk;
    Widget		xHelp;
    Widget		xCancel;

		//
		// Name of the dialog box.
		//
    const char*		xName;

		//
		// List of text widgets to manage the pseudo tab-group.
		//
    dlist		xText;

		//
		// Text item where the keyboard focus is.
		//
    XawText*		xTextFocus;

		//
		// True if the object must be deleted by cancel(), activate().
		//
    bool		xAutoDelete;

		//
		// True if the dialog box must be placed somewhere on
		// the window.
		//
    bool		xAutoPlace;

    void placeDialog();

public:
	//
	// Initialize the Xtoolkit and create the topLevel window.
	//
    XawDialog(const char* _name, XrmOptionDescList _options, Cardinal _n,
	      String* _fallback, int* _argc, char* _argv[]);

	//
	// Create a transcient (dialog box) window.
	//
    XawDialog(const char* _name, const int _mode = XAW_DIALOG_ALL);

    virtual ~XawDialog();

    inline XawDialog* next() const;
    inline XawDialog* prev() const;

	//
	// Return the name of the dialog box.
	//
    inline const char* name() const;
    
    inline XtAppContext Context() const;
    inline Widget TopLevel() const;
    inline operator Widget() const;

	//
	// Returns xForm (Needed due to a bug in g++ 2.6.3: it does
	// not access it correctly).
	//
    Widget form() const;

	//
	// Method executed when the user selects the Ok button.
	//
    virtual void activate();

	//
	// Method executed when the user selects the Cancel button.
	//
    virtual void cancel();

	//
	// Update the content of the dialog box (when something changes,
	// all the `update' methods are called so that dialog boxes can
	// reflect the change).
	//
    virtual void update();

	//
	// Close the dialog (ie pop it down) but do not delete or release
	// the widgets.
	//
    void close();

	//
	// Popup the dialog.
	//
    void popup();

	//
	// Setup the window protocols (WM_DELETE_WINDOW, ...).
	//
    void setupWindowProtocols();

	//
	// Enters in the main loop of the application.
	//
    void mainLoop();

	//
	// Recenter the viewport (if any) so that the text widget
	// which has the focus is now visible.
	//
    void focusViewport();

	//
	// Change the focus of the text widget to the widget `_w'.
	// (Used from an action, `_w' is the text widget which was clicked)
	//
    void selectTabGroup(XawText* _t);
    void selectTabGroup(Widget _w);

	//
	// Change the focus of the text widget to the next text widget.
	//
    void nextTabGroup();

	//
	// Inserts the text item in the text group of the dialog.
	// (Find the dialog in which `_textItem' is, and insert it in
	// the tab-group list of that dialog)
	//
    static void insertTabGroup(XawText* _textItem);
    static void removeTabGroup(XawText* _textItem);

	//
	// Find the dialog object which holds the widget `w'.
	//
    static XawDialog* find(Widget _w);

	//
	// Emergency exit, close everything.
	//
    static void closeAll();

	//
	// Something has changed in the environment. Update all the
	// dialog boxes.
	//
    static void updateAll();

	//
	// Return the help file (configured from application resources).
	//
    static const char* helpFile();
};

extern XawDialog* xAppMainDialog;

    inline XtAppContext
XawDialog::Context() const
{
    return xContext;
}

    inline Widget
XawDialog::TopLevel() const
{
    return xTopLevel;
}

    inline
XawDialog::operator Widget() const
{
    return xDialog;
}

    inline XawDialog*
XawDialog::next() const
{
    return (XawDialog*) dlink::next();
}


    inline XawDialog*
XawDialog::prev() const
{
    return (XawDialog*) dlink::prev();
}

    inline const char*
XawDialog::name() const
{
    return xName;
}


// ----------------------------------------------------------------------
// Class :	XawErrorDialog
//
// Role :	General purpose error dialog popup
//
class XawErrorDialog : public XawDialog {
protected:
    Widget	xText;		// Text widget holding the error messages.
public:
    XawErrorDialog(const char* _name);
    ~XawErrorDialog();

	//
	// Report the error message `_msg' (already formatted).
	//
    void report(const char* _msg);
};


// ----------------------------------------------------------------------
// Class :	XawConfirmDialog
//
// Role :	Confirmation popup dialog box
//
//		The confirmation dialog box is an anti-social dialog box
//		which prevents any action in other dialog boxes, until
//		the user has confirmed or not the operation. The constructor
//		initializes the dialog box, opens it and waits for the
//		user to validate or cancel. Once the constructor has
//		returned, it is possible to check the result selected by
//		the user. The dialog box is popped down by the destructor.
//
class XawConfirmDialog : private XawDialog {
protected:
    Widget	xText;

    bool	xcdResult;
    bool	xcdDone;
public:
	//
	// Create a confirmation dialog box. The confirmation message is
	// define in the application resources.
	//
    XawConfirmDialog(const char* _name, const int _mode = XAW_DIALOG_ALL);
    ~XawConfirmDialog();

	//
	// Return `true' if the operation was confirmed. `false' otherwise.
	//
    inline bool result() const;

	//
	// Method executed when the user selects the Ok button.
	//
    void activate();

	//
	// Method executed when the user selects the Cancel button.
	//
    void cancel();
};


    inline bool
XawConfirmDialog::result() const
{
    return xcdResult;
}


#define	XAW_TEXT	0x01
#define	XAW_LABEL	0x02
#define	XAW_COMMAND	0x04
#define	XAW_TOGGLE	0x08
#define XAW_FIRST_GROUP 0x10

// ----------------------------------------------------------------------
// Class :	XawText
//
// Role :	Definition of a Text/Label/Toggle/Button item [with a label]
//
class XawText : public dlink {
private:
    XawText(const XawText&);
    void operator=(const XawText&);
// void operator&(); this restriction is removed because too hard to cope with.
protected:
    Widget	xLabel;
    Widget	xText;
    Widget	xCommand;
    Widget	xToggle;
public:
	//
	// Create a text edition field with a label.
	//
    XawText(const char* name, const int mode, const char* value,
	    Widget form, Widget top);
    virtual ~XawText();

    inline XawText* next() const;
    inline XawText* prev() const;

	//
	// Method executed when a command/toggle button is selected.
	//
    virtual void select(XtPointer _data);

	//
	// Setup the value of the text which is edited.
	//
    XawText& operator=(const char* _text);

	//
	// Get the current text value (filled by user).
	//
    operator char*();

	//
	// Return the text widget or the label widget or the command widget.
	//
    operator Widget() const;

	//
	// Set the label.
	//
    void setLabel(const char* _label);

	//
	// Return the label string.
	//
    const char* getLabel();

	//
	// Set/clear the semantic error mark associated to the text widget.
	//
    void setSemanticError(const bool _value);

	//
	// Return True if the string pointed to by `s' only contains.
	// Ascii characters (without control chars).
	//
    static bool acceptableText(const char* _text, const size_t _max,
			       const char* _param_name);

	//
	// Clear the radio group leader before the creation of a new
	// toggle item.
	//
    static void resetRadioGroup();

#ifdef HAVE_X11R5
    static void dontDestroyWidgets(const bool _flag);
#endif
};

    inline XawText*
XawText::next() const
{
    return (XawText*)dlink::next();
}

    inline XawText*
XawText::prev() const
{
    return (XawText*)dlink::prev();
}


class XawMenuItem;

// ----------------------------------------------------------------------
// Class :	XawMenu
//
// Role :	Manage a popup dialog with a menu button
//
class XawMenu : public dlist {
private:
    XawMenu(const XawMenu&);
    void operator=(const XawMenu&);
    void operator&();
protected:
    Widget	 xmMenu;
    Widget	 xmButton;
public:
	//
	// Create a popup menu.
	//
    XawMenu(const char* _name, Widget _form, Widget _topLevel);
    ~XawMenu();

	//
	// Return the `_which' th entry of the menu.
	//
    inline XawMenuItem* operator [](unsigned _which) const;

	//
	// Return the popup menu widget.
	//
    inline operator Widget() const;

	//
	// Return the button widget.
	//
    inline Widget Button() const;
};

    inline
XawMenu::operator Widget() const
{
    return xmMenu;
}

    inline Widget
XawMenu::Button() const
{
    return xmButton;
}

// ----------------------------------------------------------------------
// Class :	XawMenuItem
//
// Role :	Manage a single item of a popup menu
//
class XawMenuItem : public dlink {
private:
    XawMenuItem(const XawMenuItem&);
    void operator=(const XawMenuItem&);
    void operator&();
protected:
    Widget	xItem;
    XawMenu&	xMenu;
public:
	//
	// Add an entry in a popup menu.
	//
    XawMenuItem(const char* _name, XawMenu& _menu);
    virtual ~XawMenuItem();

	//
	// Callback executed when the item is selected.
	//
	virtual void
    activate() = 0;

	//
	// Return the item widget.
	//
    inline operator Widget() const;
};

    inline
XawMenuItem::operator Widget() const
{
    return xItem;
}

    inline XawMenuItem*
XawMenu::operator [](unsigned _which) const
{
    return (XawMenuItem*) dlist::pos(_which);
}

// ----------------------------------------------------------------------
// Class :	XawMenuSep
//
// Role :	Definition of a separator in a popup menu
//
class XawMenuSep : private dlink {
private:
    XawMenuSep(const XawMenuSep&);
    void operator=(const XawMenuSep&);
    void operator&();
protected:
    Widget	xItem;
    XawMenu&	xMenu;
public:
	//
	// Add a separator in a popup menu.
	//
    XawMenuSep(const char* _name, XawMenu& _menu);
    ~XawMenuSep();
};

#endif
