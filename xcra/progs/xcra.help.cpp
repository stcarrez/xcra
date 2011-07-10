copyright general[General Information] bug[Bug report] author[Author]
		Copyright

Xcra is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Xcra is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

next-help
help dialogs[Dialog boxes] general[General Information]

	Oh oh! Need some help?

Help is decomposed in help topics. A help topic
gives information on a particular characteristic
of the software. Help topics are associated with
sub-topics which point to additional information,
correlated or not with the current topic. Topics
together form a possibly cyclic graph of topics.

The help dialog box gives help information on a
particular topic. It allows to walk the graph of
topics by displaying one button to go to the
previous topic and several buttons to go to the
sub-topics.

In a dialog box, activating the help via the
`Help' button, will popup the help dialog with
help information on the current dialog box.

next-help
dialogs semErrors[Semantic Errors] keys[Accelerator keys]
	    Dialog Boxes

When Xcra needs your intervention, it opens
dialog  boxes. Dialog boxes contain essentially
three kinds of buttons:

  Ok		An `Ok' button on the bottom-left of
		the dialog. Ok buttons validate the
		dialog, verify that every field is
		setup correctly and perform the action
		associated with the dialog box. When
		the dialog box is not filled correctly,
		the faulty fields are grayed: this is
		the semantic errors. The `Ok' button
		is sometimes named `Modify'.

  Help		An `Help' button, generally on the
		bottom-middle of the dialog. Help
		buttons pops up the help dialog box
		with the information concerning the
		current dialog box. 

  Cancel	A `Cancel' button on the bottom-right
		of the dialog. Cancel buttons close
		the dialog box without executing the
		action associated with the dialog.
		The `Cancel' button is sometimes
		named `Dismiss' or `Done'.

next-help
whatIs XCra[Main Window] taskMgr[Task Manager] noteMgr[Note Manager] general[General Information]

		XCRA 2.2

Xcra is a tool which will help you recording the time
spent on various tasks, and manage a set of notes
strongly or loosely coupled with the active task.
Xcra integrates two managers:

   o A task manager which helps in keeping track of the
     time spent on the different tasks,

   o A note manager which records various free-style text
     notes.

Both managers have made a peace agreement to cohabit together.

next-help
taskMgr general[General Information]
		Task Manager

Xcra  keeps  track  of  the  task  you  are working on and
records the time you spend working on it. When you  change
to another task, you simply have to notify Xcra of this by
selecting the new task.  Xcra will then  update  the  time
for  the  new  task. At the end of the day, Xcra will have
recorded for each tasks you have worked on, the time  that
you have spent for each of them. The task with their times
are saved in a file associated to the current month of the
year.   Xcra  uses the information saved in those files to
generate the report form dialog box. An external tool cra-
report(1)  also  uses the content of the files to generate
#if HAVE_LDBUPDATE
several reports, including the  report  suitable  for  the
LDBupate(1) command.
#else
several reports.
#endif

next-help
noteMgr general[General Information]
		Note Manager

Xcra manages general purpose  notes. Notes represent free text
which  is associated to a title and saved in a notes file.
The notes file is intended to be used to  save  some  free
text  information strongly correlated to the current task.
For each task, it is possible to associate a  notes  file.
In  that  case, each time the current task is changed, the
current notes file is also changed. When  a  task  is  not
associated  to a notes file, a default notes file is used.

next-help
general whatIs[What is Xcra?] copyright[Copyright] author[Author] bug[Bug report]
		General Information
		     XCra 2.2


XCra is a small tool to help people keep track of where they
spent time during the day. It also help them manage small
notes to be able not to forget complex and uninteresting
things.


next-help
author general[General Information] copyright[Copyright] bug[Bug report]
		Author

Stephane Carrez

17, rue Foucher Lepelletier
92130 Issy Les Moulineaux
FRANCE

Stephane.Carrez@worldnet.fr
next-help
bug general[General Information] copyright[Copyright] author[Author]
		Bug report

Improvements, suggestions, bug-corrections, bug-reports
are welcome. They may be sent to the following e-mail address:

	Stephane.Carrez@worldnet.fr

next-help
ref addTask[Add a task] delTask[Remove a task] dialogs[Dialog boxes]
		Update Task References

The `Update Task References' dialog box allows to update
the task references associated with each of the tasks.
The list of tasks is displayed in a scrolling window.
For each task, the corresponding reference can be edited
via a text box. Once the different task references are
updated, activating the dialog box will actually change
them to the new values.

next-help
listTask switchTask[Switch to a task] dialogs[Dialog boxes]
		Select Task

The `Select Task' dialog box  lists in a scrolling  list
the available tasks allowing  to choose  the active one.
Once a task has been selected, it becomes the new active
task.

next-help
switchTask listTask[Select a task] specTime[Time Format] dialogs[Dialog boxes]
		Switch to a Task

The `Switch To Task' dialog box allows you to change the
active task and transfer a time from the current task
to the new active task. It is useful when you discover
that you are working on the wrong task, and want to
correct the problem by transferring the time and changing
the active task.

The `Time value' text field allows you to specified the
time that must be transfered.

Fields:
-------
Time value	The time which must be transfered.

next-help
addTask delTask[Delete a task] dialogs[Dialog boxes]
		Add a new Task

The `Create Task' dialog box allows you to create a new task.
The task is associated with a logical name and a reference name.
The logical name is printed in dialog boxes and the
reference name is the task identification used by external
tools.
#ifdef HAVE_IMPORT
A list of imported tasks is displayed and selecting one
task of the list will put the name and reference of the
selected task in the text edition fields. It is then possible
validate the dialog box or to modify the task or
reference name.

The list of imported tasks is read from the file `import.tasks'
which is automatically updated by an external command when
this is needed.
#endif

Fields:
-------
Task name		Logical name of the task
			(must not be empty)
Task reference		Task reference name
Creation date		The month when the task is created. By
			default this is the current month. The
			date is specified as: <month>/<year>

next-help
keys
		Key Bindings

Several key-bindings are defined to accelerate and
ease the operations on dialog boxes.


XK_Tab			Edit next text field
XK_Return		Validate the dialog box
F1			Give help on the current dialog box
F2			Cancel the dialog box

The file selector dialog box integrates a file
name and directory name completion mechanism activated
when the `XK_space' key is hit.

next-help
semErrors keys[Accelerator keys]
		Semantic Errors

Most of the dialog boxes  which  have  text entries,
provide a semantic error checking. Semantic error
checking is activated each time you validate a dialog
box, that is either by selecting the `Ok' button or by
hitting the `XK_Return' key.

In that case, if you  didn't  have  filled correctly
some text  entries,  Xcra will tile (gray) the label
associated with the wrong text input to show you where
the errors come from. The dialog box is not closed
and no action is performed. You simply need to correct
the  problem or cancel the operation.

next-help
delTask addTask[Adding a task] dialogs[Dialog boxes]
		Remove Task

The `Remove Task' dialog box allows you to remove an existing
task. The list of tasks is displayed in a scrolling list.
Choosing one of them will remove it.

 o It is not possible to remove a task for which
   a time is reported.

 o It is not possible to delete the current task.

next-help
addTime specTime[Time Format] dialogs[Dialog boxes]
		Modify Time

The `Modify Time' dialog box allows you to modify the
time associated to several tasks for a particular day.

At top of the dialog box, a calendar allows the
selection of the day. A day is selected by pressing
the <left> mouse button on the corresponding day.

Changing the current month can be made by pressing
the <left> mouse button either on the left or right
top arrows.

Once the month and the day are selected, it is possible
to specify a time for several tasks. The time can
be added or substracted. To substract a time, the '-'
character must be present.

When all the text fields are filled correctly, the
time of the different tasks is updated. The text
field are cleared and the dialog box remains open.
It is then possible to update another day. The dialog
box is closed by pressing the `Done' button.

next-help
specTime semErrors[Semantic Errors]
		Time Specification

The specification of a time is as follows:

	0..59		   minutes
	0..23:0..59	   hours:minutes
	0..23:0..59:0..59  hours:minutes:seconds

When a time value is preceded by the `-' sign (minus),
the time is negative: it is subtracted from the actual
task time value. Otherwise, the time value
is added to the actual task time value. If a time value is
not correct, the validation of the dialog box will cause
the label associated with that value to be grayed. This
indicates that the text field is not filled correctly and
must be corrected.

next-help
report addTime[Modify Time] dialogs[Dialog boxes]
		Report Time

The `Report Time' dialog box presents various summary
reports of the time spent on the different tasks. It also
provide an export method of the Xcra time database to
an external database.

A calendar allows you to select the day, the  week,
the fortnight or the month that you want to report.
The calendar automatically disables  the  days  for
which an empty time is reported. The calendar high-
lights the current day, the current week, the  cur-
rent  fortnight  or  the complete month. It does so
depending on the report mode which is  selected.

A list of tasks displays the time spent for each of
them. The time reported by the list depends on  the
report  mode  currently  selected. A set of buttons
allows to select the format of the report:

Day		report the time spent on a single day,
Week		report the time spent on a week,
Fortnight	report the time spent on a fortnight,
Month		report the time spent for the complete
		month.

Another set of buttons allows to choose a sort method
to list  the  tasks:

Names	   	sort on task name,
Day Time	sort on the time spent on the day,
Week time	sort on the time spent on the week,
Fortnight time	sort on the time spent on the fortnight,
Month		sort on the time spent on the month.

The  activation  of  the  `Update  Database'   button
updates  the  content of an external database with the
content of Xcra files. The update is only made for the
fortnight   in   which  the  current  selected  day
belongs. That is, prior to updating  the  database,
you  should  select a day of the fortnight that you
want to update.  Xcra updates the database by  executing
the  external command `cra-report'.  The command which is
executed may be  parameterized  using the  `reportCommand'
resource.

The  cra-report tool rounds all the times down to the
lowest hour boundary. The remaining time are saved in a file
and are taken into account for the next  fortnight  report.
It  is  therefore extremely important to report the
time in the fortnight in increasing  order  (fortnight
9501, 9503, ...).  Otherwise, the remaining time of
the previous fortnight cannot be included.  Anyway,
a  warning  message  is reported if cra-report does
not find the  remaining  times  for  the  fortnight.
#if HAVE_LDBUPDATE
Saving the times several times in the LDBupdate database
is  possible.
#endif

next-help
decTime addTime[Modify Time] specTime[Time Format] dialogs[Dialog boxes]
		Transfer Time

The `Add Time To Current Task' dialog box allows you to
transfer a time from a task to another, in the current
day.

This is a shortcut of the `Modify Time' dialog box
in which you would subtract a time for a task
and add the same value to the current task.

Fields:
-------
Time value		The time which must be subtracted
			from the selected task, and then
			added to the current task.
next-help
incTime addTime[Modify Time] specTime[Time Format] dialogs[Dialog boxes]
		Transfer Time

The `Subtract Time From Current Task' dialog box
allows you to transfer a time from the current task
to another task.

This is a shortcut of the `Modify Time' dialog box
in which you would add a time for a task and subtract
the same value from the current task.

Fields:
-------
Time value		The time which must be added
			to the selected task, and then
			subtracted from the current task.
next-help
error dialogs[Dialog boxes]
		Error Dialog Box

The `Xcra Error Message' dialog box is open each
time an error occurs. Error messages are reported
in that dialog box while they occur. Closing the dialog
box will clear all the error messages history.

next-help
		Search in the notes

The `Search in the notes' dialog box allows you to
search for some information in several notes. The
search operation uses the external tool noteManager(1)
with the option --grep. When the dialog box is
validated, the external tool is launched and its
output saved in the text window.

Fields:
-------
Notes file		Specifies the path of the notes
			file on which the search must be
			started.

Regular expression	Specifies the regular expression
			to search for.

next-help
addNote delNote[Delete a note] editNote[Edit a note] dialogs[Dialog boxes]
		Create a Note

The `Create Note' dialog box allows you to create a
new note description in a note file. Each note
description is assigned a category:

Normal
   The content of the note  is  stored  in  the  notes
   file.  It can be edited by Xcra or by the note man-
   ager tool `noteManager'. The purpose of such  notes
   is  for  example to remember some complex commands,
   some temporary information, some actions to perform
   on  the current task, some actions made on the cur-
   rent task, ...

External
   The content of the note is stored  in  an  external
   file.  The  file can't be edited by Xcra (this is a
   current limitation which may be removed later)  but
   can  be  viewed in read-only mode. The note manager
   tool is also not able to modify the content of such
   external notes. External notes are intended to sim-
   plify the reading of some external files.  External
   files  can  be very large (> 4Mb) without impacting
   the size of the Xcra process.

Sub notes
   The sub notes represents an entry point for another
   notes  file. This allows the construction of graphs
   of notes. The construction of graphs  of  notes  is
   useful  with  the search option (--grep) because it
   allows a deep scanning of all  the  possible  notes
   files.

Once the dialog is validated, the note is created.
If the new note is of type `Normal', the note editor
is started on the current note.

Fields:
-------
Note title		Defines the name of the note.

Note type		Defines the category of the
			note.

next-help
delNote addNote[Create a note] dialogs[Dialog boxes]
		Delete a Note

The `Delete Note' dialog box allows you to delete
an existing note in the current note file. The dialog
lists in a scrolling list the available notes,
allowing the selection of one of them.

Once selected, the note is deleted. Its content is
no longer accessible.

next-help
selNote editNote[Edit a Note] dialogs[Dialog boxes]
		Select a Note

The `Select Note for Edition' dialog box allows you
to select a note for modifying its content. The list
of available notes is displayed in a scrolling list.

Once selected, the note editor is started on the
selected note.

next-help
selNoteFile dialogs[Dialog boxes]
		Select Notes File

The `Select Notes File' dialog box allows you to
select the notes file associated to the current task.
The dialog box is a file selector object showing
two separate lists and three text fields.

The `Directory' and `File' text fields represent
the file which is currently selected.

o A file list on the right displays the files which
  match one or several patterns defined in the
  `Filter' text field. A single click using the
  left mouse button inserts the file in the `File'
  text field. A double click using the left mouse
  button inserts the file in the `File'
  text fields and validates the dialog box.

o A directory list on the left allows to change the
  directory. The selection of one directory of the
  list will change the directory and update the list
  of files.

A completion mode, a la Emacs, is available in the
two text fields `Directory' and `File'. The completion
mode is activated by hitting the <Space> key.

next-help
destroyNote dialogs[Dialog boxes]
		Delete Note

The `Destroy Note' dialog box is a confirmation
dialog box to make sure that you really want to
delete the note.

At this time, it is not possible for you to give
the control to any other open dialog box, including
the help dialog box. This, until you accepts or not
the confirmation box.

next-help
FileSelect dialogs[Dialog boxes]
	Select the Note External Path

The `FileSelect' dialog box allows you to
select the path of the external note. Once the
external note is selected, the new note is
created. The dialog box is a file selector object
showing two separate lists and three text fields.

The `Directory' and `File' text fields represent
the file which is currently selected.

o A file list on the right displays the files which
  match one or several patterns defined in the
  `Filter' text field. A single click using the
  left mouse button inserts the file in the `File'
  text field. A double click using the left mouse
  button inserts the file in the `File'
  text fields and validates the dialog box.

o A directory list on the left allows to change the
  directory. The selection of one directory of the
  list will change the directory and update the list
  of files.

A completion mode, a la Emacs, is available in the
two text fields `Directory' and `File'. The completion
mode is activated by hitting the <Space> key.

next-help
clearNote dialogs[Dialog boxes]
		Clear Note

The `Clear Note' dialog box is a confirmation
dialog box to make sure that you really want to
clear the note content.

At this time, it is not possible for you to give
the control to any other opened dialog box, including
the help dialog box. This, until you accepts or not
the confirmation box.

next-help
editNote dialogs[Dialog boxes]
		Note Editor

The note editor is based on the Athena widget text
editor. That editor supports the main Emacs key
binding.

The note editor window is always popped-up at the
same place where it was visible the last time it
was opened. The title of the window is not
yet configurable.

At top level of the note content, several buttons
allow to perform some actions:

Dismiss		Close the note editor. The note is saved.
Save		Save the note without closing the note
		editor.
Print		Print the note using an external command.
Title		Change the title of the note editor.
Clear		Clear the entire note content.
Destroy		Delete the current note
Select		Select the entire note content. The note
		content is ready to be paste using the
		X11 copy/paste mechanism.

next-help
XCra whatIs[What is Xcra?] dialogs[Dialog boxes]
		Main Window

The XCra main window displays three popup menu buttons:

 o A task popup button.

   The name of the current task is displayed in the button.
   The task popup menu allows you to create, remove, update
   or switch the active task.

 o A time popup button.

   The name of the button is a formatted string which
   can indicate the time spent on the task. The time
   popup menu gives you access to the report summary form,
   and supports functionalities to edit the time values.

 o A note popup button.

   The name of the current notes file is displayed in button.
   The notes popup menu allows you to create, remove, select
   and edit a note. It also provides a basic search in the
   notes file.

By default, it is not possible to close the main window.
Closing the main window is only enabled when the -allow-close
option is specified or when the allowClose resource is set
to true.

next-help
toto
