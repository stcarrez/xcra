// -*- C++ -*-
//////////////////////////////////////////////////////////////////////
// Title	: Athena Widget Help
// Author	: S. Carrez
// Date		: Sun Nov 19 18:18:43 1995
// Version	: $Id: XawHelp.H,v 1.5 1996/08/04 15:34:17 gdraw Exp $
// Project	: Xcra
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
#ifndef _XAW_HELP_H
#define _XAW_HELP_H

#include "XawDialog.H"

#ifndef MAX_HELP_TOPIC_STACK_SIZE
#  define MAX_HELP_TOPIC_STACK_SIZE	32
#endif

// ----------------------------------------------------------------------
// Class :	XawHelpDialog
//
// Role  :	Controls the help dialog box. Only one instance of this
//		class is created at a time (and its creation is private).
//		The help dialog contains a text widget which holds the
//		description of the help message (read-only).
//
//		An help file has a very simple layout. It begins with the
//		help topic name and is followed by the help description.
//		The help description is terminated by \0. Next topics then
//		follows. Because it is never changed, the complete help
//		file is mapped in memory.
//
//		To search for a particular topic, the complete help file
//		is scanned, starting at the begining, and until the topic
//		is found (this is the simpler implementation which is in
//		fact quite good if help files are small).
//
//		When an help description is found, the text widget is
//		updated and required to display the region of the mapped
//		help file.
//
class XawHelpDialog : private XawDialog {
	friend class XawHelpTopic;
private:
		//
		// The help file is mapped in memory using `mmap'.
		// The region starts at `helpAddr' and contains `helpSize'
		// bytes. It is mapped in read-only mode. If `mmap' is
		// not available, the help file is just read in memory.
		//
    char*	helpAddr;
    long	helpSize;

		//
		// The text widget holding the help description.
		//
    Widget	xHelp;

		//
		// The box holding the sub-topics.
		//
    Widget	xSubTopicForm;

		//
		// To implement a `prev-topic' button, we maintain a stack
		// of topic names. Selecting a sub-topic means pushing the
		// current topic name on the stack. Activating the `prev-topic'
		// option just means popping from the stack.
		//
    char*	helpTopicStack[MAX_HELP_TOPIC_STACK_SIZE];
    int		helpTopicPos;

    dlist	helpTopicList;

    XawHelpDialog();

	//
	// Find the help topic `_topic' in the help file and return
	// the start address of the help description as well as its length.
	//
	long
    findHelpTopic(const char* _topic, char*& _start);

	//
	// Find a help topic and show the help description.
	//
	void
    showHelpTopic(const char* _topic);

	//
	// Move back to display the help on the previous topic.
	//
	void
    popHelpTopic();

	//
	// Scan the help description starting at `_list' to extract the
	// list of sub-topics, and build the buttons to let the user be
	// able to select those sub-topics. Return the address of the
	// beginning of the current help message.
	//
	const char*
    gatherSubTopics(const char* _list, long _len);

private:protected:// Keep gcc happy, but you can't use it anyway :-)
    ~XawHelpDialog();
public:
	//
	// Open the help dialog box to give help on the topic `_topic'
	// (the topic may be the logical name of a dialog box).
	//
    static void open(const char* _topic);
};

#endif
