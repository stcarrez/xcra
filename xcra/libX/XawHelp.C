// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Help Support
// Author	: S. Carrez
// Date		: Sun Nov 19 18:21:05 1995
// Version	: $Id: XawHelp.C,v 1.7 2000/02/23 08:19:34 ciceron Exp $
// Project	: xcra
// Keywords	: Help, Xaw
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
// class XawHelpDialog		Help management
//
#include "config.H"
#include <stdio.h>
#include <sys/stat.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/AsciiText.h>

#include "Assert.H"
#include "Error.H"
#include "XawDialog.H"
#include "XawHelp.H"

#if HAVE_MMAP
#  include <sys/mman.h>
#endif


//
// There is only one help dialog box which can be opened at a time.
//
static XawHelpDialog* help = 0;

// ----------------------------------------------------------------------
//
//			XawHelpTopic
//
//	Management of Sub-Help Topic Buttons
//
// ----------------------------------------------------------------------

class XawHelpTopic : public XawText {
	friend class XawHelpDialog;
protected:
    XawHelpDialog&	xHelp;
    char*		xHelpTopic;
public:
    XawHelpTopic(const char* _topic, const char* _title,
		 XawHelpDialog& _help);
    ~XawHelpTopic();

    void select(XtPointer _data);

	inline
    void setTopic(const char* _topic, const char* _title);
};


//
// Create a new help topic button. The `_topic' string is used to
// search for the topic in the help file and the `_title' string
// is printed in the topic button. A null string for `_topic' represents
// the previous-topic button.
//
XawHelpTopic::XawHelpTopic(const char* _topic, const char* _title,
			   XawHelpDialog& _help)
  : XawText("helpTopic", XAW_COMMAND, _title, _help.xSubTopicForm, None),
    xHelp(_help)
{
    if (_topic) {
	xHelpTopic = (char*) XtNewString(_topic);
    } else {
	xHelpTopic = 0;
    }
    xHelp.helpTopicList.insert(this, xHelp.helpTopicList.last());
}


//
// Delete an help topic button.
//
XawHelpTopic::~XawHelpTopic()
{
    xHelp.helpTopicList.remove(this);
    if (xHelpTopic) {
	XtFree(xHelpTopic);
    }
}


//
// Callback executed when the topic button is pressed.
//
    void
XawHelpTopic::select(XtPointer)
{
    if (xHelpTopic) {
	xHelp.showHelpTopic(xHelpTopic);
    } else {
	xHelp.popHelpTopic();
    }
}


//
// Change the topic button to refer to a new help topic.
//
    inline void
XawHelpTopic::setTopic(const char* _topic, const char* _title)
{
    if (xHelpTopic) {
	XtFree(xHelpTopic);
    }
    setLabel(_title);
    xHelpTopic = (char*) XtNewString(_topic);
}


// ----------------------------------------------------------------------
//
//			XawHelpDialog
//
//	Management of Help Dialog Box
//
// ----------------------------------------------------------------------

//
// Create the help dialog box.
//
XawHelpDialog::XawHelpDialog()
: XawDialog("help", XAW_DIALOG_OK)
{
    const char* helpFile = XawDialog::helpFile();

    helpTopicPos = 0;

    int fd = ::open(helpFile, O_RDONLY);

    if (fd < 0) {
	Error::report(MSG_NO_HELP_FILE, helpFile);
	helpAddr = 0;
	delete this;
	return;
    }

    struct stat st;
    fstat(fd, &st);

    helpSize = st.st_size;
#if HAVE_MMAP
    helpAddr = (char*) mmap(0, helpSize, PROT_READ, MAP_PRIVATE, fd, 0);
#else
    helpAddr = (char*) malloc(helpSize);
    if (helpAddr) {
	if (read(fd, helpAddr, helpSize) != helpSize) {
	    free((free_ptr) helpAddr);
	    helpAddr = 0;
	}
    }
#endif
    ::close(fd);

    if (helpAddr == (char*)(-1L) || helpAddr == (char*) NULL) {
	Error::report(MSG_NO_HELP_FILE, helpFile);
	helpAddr = 0;
	delete this;
	return;
    }

    xHelp = XtVaCreateManagedWidget("text", asciiTextWidgetClass, xForm,
				    XtNeditType, XawtextRead,
				    XtNtype, XawAsciiString,
				    XtNuseStringInPlace, True,
				    XtNdisplayCaret, True,
				    XtNstring, "",
				    XtNlength, 0,
				    NULL);

    xSubTopicForm = XtVaCreateManagedWidget("topics", boxWidgetClass, xForm,
					    XtNfromVert, xHelp,
					    XtNresizable, True,
					    NULL);
    help = this;

	//
	// Create the `prev-topic' button.
	//
    new XawHelpTopic(0, 0, *this);
}


//
// Close and delete the help dialog box.
//
XawHelpDialog::~XawHelpDialog()
{
    if (helpAddr) {
#if HAVE_MMAP
	munmap(helpAddr, helpSize);
#else
	free((free_ptr) helpAddr);
#endif
    }
    while (helpTopicPos != 0) {
	XtFree(helpTopicStack[--helpTopicPos]);
    }

    XawHelpTopic* topic;

	//
	// Delete any existing topic.
	//
    while ((topic = (XawHelpTopic*) helpTopicList.last())) {
	delete topic;
    }

    help = 0;
}


//
// Scan the help description starting at `_list' to extract the
// list of sub-topics, and build the buttons to let the user be
// able to select those sub-topics. Return the address of the
// beginning of the current help message.
//
    const char*
XawHelpDialog::gatherSubTopics(const char* _list, long _len)
{
    require(_list != 0);

    XawHelpTopic* topic;

    require(helpTopicList.first() != 0);
    topic = (XawHelpTopic*) helpTopicList.first()->next();

	//
	// Since the area pointed to by `_list' is mapped in memory,
	// we must check that the character we read are valid. We have
	// no guarantee that the area is null terminated.
	//
	// The implementation is safe if error are introduced in the help
	// file. BUT, no error is reported. This is to keep this code small.
	//

    while (_len > 0 && _list[0] != '\n') {
	char topicName[32];
	char titleName[256];
	char* p;

		//
		// Skip some spaces.
		//
	while (_len > 0 && _list[0] == ' ') {
	    _len --;
	    _list ++;
	}
		//
		// Isolate the topic name (used as an internal index).
		//
	p = topicName;
	while (_len > 0 && _list[0] != '[' && _list[0] != '\n') {
	    *p++ = *_list++;
	    _len --;

	    if (p >= &topicName[sizeof(topicName)-1])
		break;
	}
	*p = 0;
	if (_len > 0 && _list[0] == '[') {
	    _list ++;
	    _len --;
	}

		//
		// Isolate the title name (used for the button name).
		//
	p = titleName;
	while (_len > 0 && _list[0] != ']' && _list[0] != '\n') {
	    *p++ = *_list++;
	    _len --;

	    if (p >= &titleName[sizeof(titleName)-1])
		break;
	}
	*p = 0;
	if (_len > 0 && _list[0] == ']') {
	    _list ++;
	    _len --;
	}

		//
		// Update an existing topic button or create a new one.
		//
	if (topic) {
	    topic->setTopic(topicName, titleName);
	    topic = (XawHelpTopic*) topic->next();
	} else {
	    new XawHelpTopic(topicName, titleName, *this);
	}
    }

	//
	// Delete all the topic buttons which are not used.
	//
    while (topic) {
	XawHelpTopic* n = (XawHelpTopic*) topic->next();
	delete topic;
	topic = n;
    }

    return _list;
}


//
// Find the help topic `_topic' in the help file and return
// the start address of the help description as well as its length.
//
    long
XawHelpDialog::findHelpTopic(const char* _topic, char*& _start)
{
    require(_topic != 0);

    const char* p;
    size_t len;

    len = strlen(_topic);
    p   = helpAddr;

    long i = helpSize - len;
    while (i > 0) {
		//
		// Find position of next topic (find \0).
		//
	register const char* s;
	for (s = p; --i >= 0 && *s++ != '\0';) {
	     continue;
	}
	if (strncmp(p, _topic, len) == 0) {
	    _start = (char*) gatherSubTopics(&p[len], (long)(s - p));
	    if (s > _start) {
		return (long)(s - _start);
	    } else {
		return 0;
	    }
	}
	p = s;
    }
    return -1;
}


//
// Move back to display the help on the previous topic.
//
    void
XawHelpDialog::popHelpTopic()
{
    if (helpTopicPos >= 2) {
	helpTopicPos -= 2;
	char* topic = helpTopicStack[helpTopicPos];

	showHelpTopic(topic);
	XtFree(topic);
    }
}


//
// Find a help topic and show the help description.
//
    void
XawHelpDialog::showHelpTopic(const char* _topic)
{
    require(_topic != 0);

    char* helpTopic;
    char* topic = (char*) XtNewString(_topic);

		//
		// Find the topic description or abort with an error message.
		//
    long len = findHelpTopic(topic, helpTopic);
    if (len < 0) {
	Error::printf(MSG_NO_HELP, topic);
	XtFree(topic);
    } else {
		//
		// When the topic stack is full, drop the first topic
		// (oldest one).
		//
	if (helpTopicPos == MAX_HELP_TOPIC_STACK_SIZE) {
	    helpTopicPos --;
	    XtFree(helpTopicStack[0]);
	    memmove(&helpTopicStack[0], &helpTopicStack[1],
		    sizeof(helpTopicStack) - sizeof(helpTopicStack[0]));
	}
	helpTopicStack[helpTopicPos++] = topic;

	XtVaSetValues(xHelp, XtNtype, XawAsciiString,
		      XtNuseStringInPlace, True,
		      XtNeditType, XawtextRead,
		      XtNlength, len,
		      XtNstring, helpTopic,
		      NULL);
	popup();
    }
}


//
// Open the help dialog box to give help on the topic `_topic'
// (the topic may be the logical name of a dialog box).
//
    void
XawHelpDialog::open(const char* _topic)
{
	//
	// Create the help dialog box if it does not exist.
	//
    if (help == 0) {
	new XawHelpDialog;
	if (help == 0) {
	    return;
	}
    }

    help->showHelpTopic(_topic);
}



