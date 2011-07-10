.\" @(#)xcra.1	6/04/95
.\"" $Id: xcra.man.cpp,v 1.6 2000/02/23 08:24:48 ciceron Exp $
.\"" Copyright (C) 1994, 1995, 1996 Stephane Carrez
.\""
.\"" This file is part of Xcra.
.\""
.\"" Xcra is free software; you can redistribute it and/or modify
.\"" it under the terms of the GNU General Public License as published by
.\"" the Free Software Foundation; either version 2 of the License, or
.\"" (at your option) any later version.
.\""
.\"" Xcra is distributed in the hope that it will be useful,
.\"" but WITHOUT ANY WARRANTY; without even the implied warranty of
.\"" MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
.\"" GNU General Public License for more details.
.\""
.\"" You should have received a copy of the GNU General Public License
.\"" along with this program; if not, write to the Free Software
.\"" Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
.\""
.TH XCRA 1 "Aug 4, 1996"
.SH NAME
xcra \- Cra Recorder Automatic tool
.SH SYNOPSIS
.B xcra
[
.B \-directory
.I dir
] [
.B \-update
.I time
] [
.B \-msize
.I count
] [
.B \-allow\-close
#ifdef HAVE_XTOOLS
] [
.B \-clock
] [
.B \-digital
] [
.B \-analog
] [
.B \-eyes
] [
.B \-biff
#endif
] [
.I toolkit-options
]
.SH DESCRIPTION
.I Xcra
records the time spent on various working tasks and provides several
summary reports on a daily, weekly, fortnight\fIly\fP or monthly basis.
A special summary report allows a direct and transparent interface for
saving the report in an external
#ifdef HAVE_LDBUPDATE
database such as
.IR LDBupdate (1).
#else
database.
#endif
.PP
.I Xcra
keeps track of the task you are working on and records the time you spend
working on it. When you change to another task, you simply have to notify
.I Xcra
of this by selecting the new task.
.I Xcra
will then update the time for the new task. At the end of the day,
.I Xcra
will have recorded for each tasks you have worked on, the time that you
have spent for each of them. The task with their times are saved in a file
associated to the current month of the year.
.I Xcra
uses the information saved in those files to generate the report form
dialog box. An external tool
.IR cra-report (1)
also uses the content of the files to generate several
#ifdef HAVE_LDBUPDATE
reports, including the report suitable for the
.IR LDBupate (1)
command.
#else
reports.
#endif
.\""
.PP
In addition to the management of the task time,
.I Xcra
manages general purpose notes. Notes represent free text which is
associated to a title and saved in a notes file. The notes file
is intended to be used to save some free text information strongly
correlated to the current task. For each task, it
is possible to associate a notes file. In that case,
each time the current task is changed, the current
notes file is also changed. When a task is not associated to a notes file,
a default notes file is used.
There exists three types of note in a notes file:
.\""
.IP "\fBGeneral\fP"
The content of the note is stored in the notes file. It can be edited
by
.I Xcra
or by the note manager tool
.IR noteManager (1).
The purpose of such notes is for example to remember some complex commands,
some temporary information, some actions to perform on the current task,
some actions made on the current task, ...
.\""
.IP "\fBExternal\fP"
The content of the note is stored in an external file. The file can't
be edited by
.I Xcra
(this is a current limitation which may be removed later) but
can be viewed in read-only mode. The note manager tool is also not able
to modify the content of such external notes. External notes are intended to
simplify the reading of some external files.
#if HAVE_MMAP
External files can be very large
(> 4Mb) without impacting the size of the 
.I Xcra
process.
#endif
.\""
.IP "\fBSub notes\fP"
The sub notes represents an entry point for another notes file. This
allows the construction of graphs of notes. The construction of graphs
of notes is useful with the search option (\fI--grep\fP)
because it allows a deep scanning of all the possible notes files.
.\""
.SS OPTIONS
.\""
In addition to the X11 toolkit options,
.I Xcra
supports the following options:
.TP "\w'-digital  'u"
-directory \fIdir\fP
Specifies the directory where the cra files are saved.
.\""
.TP
-update \fItime\fP
Updates the task time each \fItime\fP seconds.
.\""
.TP
-msize \fIcount\fP
Use \fIcount\fP as the maximum number of task names which can appear
on the task popup menu.
.\""
.TP
-allow-close
By default,
.I Xcra
forbids you to close the main window to avoid mistakes. This option
allows to close the main window and exit
.IR Xcra .
.\""
#ifdef HAVE_XTOOLS
.TP
-clock
Create a Clock widget to display the current time.
.\""
.TP
-digital
This option indicates that a 24 hour  digital  clock
should be used, if the Clock widget is created. 
.\""
.TP
-analog
This option indicates that a conventional 12 hour
clock face with tick marks and hands should be used.
This is the default.
.\""
.TP
-eyes
Creates a Eyes widget to follow the position of the mouse.
.\""
-biff
Creates a Biff widget to check when new mail arrives.
.\""
.PP
Although they are available in external tools, the
.IR Clock ,
.IR Eyes ,
and
.IR Biff
widgets where introduced to optimize the environment on small X11 stations.
#endif
.\""
.SH INITIALIZATION
Before using
.IR Xcra ,
you must choose a directory where
.I Xcra
will save the list of tasks with their time. It is not necessary to create
that directory:
.IR Xcra
can do this for you. You specify the directory either by using the
.B -directory
option or by using the
.B taskDirectory
resource. The default directory is ".cra".
.\""
.\""
.SH POPUP MENUS
.\""
The main window of
.I Xcra
gives access to three popup menus: a task management popup menu,
a time management popup menu, and a notes management popup menu.
The task popup allows you to create, remove, update or switch
the active task. The time popup gives you access to the report summary form
and supports functionalities to transfer the time from one task to another
one or correct the time for a particular working day. The notes popup
allows you to create, remove, select and edit a note. It also provides
a basic search in the notes file.
.\""
.PP
The task popup menu contains the following entries:
.\""
.IP "\fBAdd Task\fP"
Create a new task.
.\""
.IP "\fBRemove Task\fP"
Remove an exising task.
.\""
.IP "\fBUpdate Task References\fP"
Update the task references associated to each of the tasks.
.\""
.IP "\fBSelect Task\fP"
Change the active task.
.\""
.IP "\fBSwitch Task\fP"
Transfer a time and change the active task.
.\""
.IP "\fI<Task>\fP"
.\""
List of most recently used tasks to quickly change the active task.
.\""
.PP
The time popup menu contains the following entries:
.\""
.IP "\fBUpdate Time\fP"
Update the time spent on tasks for a particular day.
.\""
.IP "\fBReport Form\fP"
Displays a report form which summarizes the time spent and update an
external database by launching
.IR cra-report (1).
.\""
.IP "\fBGet From\fP"
Transfer a time from a task to the current task.
.\""
.IP "\fBPut To\fP"
Transfer a time from the current task to another task.
.\""
.\""
.PP
The notes popup menu contains the following entries:
.\""
.IP "\fBAdd note\fP"
Create a new note in a notes file (by default the current one).
.\""
.IP "\fBRemove note\fP"
Remove a note from the current notes file.
.\""
.IP "\fBEdit note\fP"
List all the notes allowing the selection of one of them and
activate the editor for the selected note.
.\""
.IP "\fBSelect notes file\fP"
Changes the notes file associated to the current task.
.\""
.IP "\fBView notes tree\fP"
Display the notes tree in a dialog allowing the selection of notes.
.\""
.IP "\fBSearch in the notes\fP"
Search in the notes tree for a pattern.
.\""
.IP "\fI<Note>\fP"
.\""
List of most recently used notes to quickly edit them.
.\""
.SH DIALOG BOXES
.\""
When
.I Xcra
needs your intervention, it opens dialog boxes that you must reply or
simply close if you want to cancel the operation. Most of the dialog
boxes which have text entries, provide a semantic error checking. Semantic
error checking is activated each time you validate a dialog box, that
is either by selecting the
.B Ok
button or by hitting the
.B Return
key. In that case, if you didn't have filled correctly some text entries,
.I Xcra
will tile (gray) the label associated to the wrong text input to show you
where the errors come from. The dialog box is not closed and no action
is performed. You simply need to correct the problem or cancel the operation.
.\""
.PP
Some dialog boxes have several text input fields. Only one
text input can have the keyboard focus at a time.
.I Xcra
allows you to change the active text input by hitting the
.I Tab
key or by pressing the left mouse button on the text field that you want
to activate.
.\""
.PP

.\""
.IP "\fBCreate Task\fP"
This dialog box allows you to create a new task by specifying the
task name (the name you attach to the task) and the task reference
which will be used by
.I cra-report
reports. Task names are limited to 128 characters while task
references are limited to 32 characters. Above these limits, an error
is raised.  Control characters and any other funny characters are
forbidden. The special task reference
.B None
is used by
.I Xcra
and
.I cra-report
to silently ignore the task when a summary is reported.
#ifdef HAVE_IMPORT
To help in the creation of the task,
.I Xcra
imports a list of tasks from an external database. The list of imported
tasks is read from the file
.I import.tasks
in the cra directory. This list is rebuilt by an external command (which is
configurable by a resource) if the file does not exist or seems
out-of-date (more than one hour old). In the dialog box, selecting
a task from the list will cause the task and reference of the selected
task to be placed in the text edition fields.
#endif
.\""
.IP "\fBRemove Task\fP"
This dialog box displays the list of tasks in a scrolled window and
allows you to remove a task. For security reasons, it is not possible
to remove a task which was used during the month or during the current
fortnight. It is also not possible to remove the active task.
.\""
.IP "\fBUpdate Task References\fP"
This dialog box displays the list of tasks with a text input for each
of them. Text inputs are filled with the current task reference associated
to the corresponding task.
.\""
.IP "\fBSelect Task\fP"
This dialog box allows to change the active task. It displays the list
of tasks in a scrolled window. Pressing the left mouse button on a task
will change the active task to the selected task.
.\""
.IP "\fBSwitch Task\fP"
This dialog box allows to change the active task and transfer a time
from the current task to the new active task. It is designed to be used
when you've forgotten to switch to another task and thus have updated the
time in a wrong task. It displays the list
of tasks in a scrolled window and a text field which must be filled to
specify the time which must be transfered.
.\""
.IP "\fBModify Time\fP"
This dialog box allows to update the time of tasks for a particular day.
The dialog box is decomposed in two parts: a calendar which allows you to
select the day and a scrolled window which lists the tasks with a text
input field for each of them. First, you must choose the day that you want
to operate on. Then, you must specify the time that you want to add or remove
for each of the tasks that you want to update. The modification operation
only takes place when all of the text fields have been filled correctly.
Changing the day will automatically clear all the text inputs. Once you
have filled the text entries for the tasks that you want to update,
press the \fIModify Time\fP button. The times are updated and the dialog
box remains opened to allow you to update the time in some other
configuration. Pressing the \fIDone\fP button is the only way to stop
updating the times.
.\""
.IP "\fBReport Time\fP"
This dialog box reports the time spent for each tasks. The dialog box
is decomposed in three parts. A calendar allows you to select the day,
the week, the fortnight or the month that you want to report. The
calendar automatically disables the days for which an empty time is
reported. The calendar highlights the current day, the current week,
the current fortnight or the complete month. It does so depending on
the report mode which is selected. A list of tasks displays the time
spent for each of them. The time reported by the list depends on the
report mode currently selected. A set of buttons allows to select the
format of the report. The
.B Day
button reports the time spent on a single day. The
.B Week
button reports the time spent on a week, the
.B Fortnight
button reports the time spent on a fortnight and the
.B Month
button reports the time spent for the complete month.
Another set of buttons allows to choose a sort method to list the
tasks. The
.B Names
button sorts the tasks on their names, the
.B "Day Time"
button on the time spent on the day, the
.B "Week time"
button on the time spent on the week, the
.B "Fortnight time"
button on the time spent on the fortnight and the
.B "Month time"
button on the time spent on the month.
.\""
.IP ""
The activation of the
.B "Update Database"
button updates the content of the database with the content of
.I Xcra
files. The update is only made for the fortnight in which the current
selected day belongs. That is, prior to updating the database, you
should select a day of the fortnight that you want to update.
.I Xcra
updates the database by executing the external command
.IR cra-report .
The command which is executed may be parameterized using the
.I reportCommand
resource. The
.I cra-report
tool rounds all the times down to the lowest hour boundary. The
remaining time are saved in a file and are taken into account for the
next fortnight report. It is therefore extremely important to report
the time in the fortnight increasing order (fortnight 9501, 9503, ...).
Otherwise, the remaining time of the previous fortnight cannot be
included. Anyway, a warning message is reported if
.I cra-report
does not find the remaining times for the fortnight which must be processed.
#ifdef HAVE_LDBUPDATE
Saving the times several times in the
.I LDBupdate
database is possible.
#endif
.\""
.\""
.IP "\fBAdd Time To Current Task\fP"
This dialog box transfers the time from another task to the current task.
It displays the list of tasks for which a non empty time is reported,
and allows you to select one of them from
which the time will be substracted. A text field specifies the time which
must be substracted from the selected task and then added to the current
task. Obviously, it is not possible to substract an amount of time
bigger that what is reported.
.\""
.IP "\fBSubstract Time From Current Task\fP"
This dialog box transfers the time from the current task to another task.
It displays the list of tasks and allows you to select one of them to
which the time will be transfered. A text field specifies the time which
must be substracted from the current task and then added to the selected
task. Obviously, it is not possible to substract an amount of time
bigger that what is reported.
.\""
.\""
.IP "\fBAdd a note\fP"
This dialog box queries for the creation of a new note. You must fill in the
title of the note. You must select the type of the note \fINormal\fP,
\fIExternal\fP or \fISub notes\fP. When the note is not \fINormal\fP,
an external path must be specified to indicate where the real note
content is. Pressing the \fIOk\fP button, creates the note and if
the note was of type \fINormal\fP, activates the note editor.
.\""
.\""
.IP "\fBDelete a note\fP"
This dialog box lists the notes of the current notes file and allows you
to select one of them. The note that you have selected is then deleted.
.\""
.\""
.IP "\fBEdit a note\fP"
This dialog box lists the notes of the current notes file and allows you
to select one of them. The note that you have selected is then edited.
.\""
.\""
.IP "\fBSelect notes file\fP"
This dialog box allows the selection of the notes file associated
to the current task.
.\""
.\""
.IP "\fBSearch in the notes\fP"
This dialog box offers the possibility to scan for a pattern in all
the notes. The dialog box includes two text fields which can be changed
and a result window. A text field allows the selection of the top
level notes file where the scan operation will start. The second
text field defines the pattern to search. Pressing the \fISearch\fP
button will activate the scan operation with the selected parameters.
Pressing the \fICancel\fP button will cancel the scan operation and
close the dialog box. Several search operation can be initiated through the
same dialog box.
.\""
.SH NOTE EDITOR
The note editor is based on the commonly used Athena Text widget which
supports the Emacs binding. The note editor contains several buttons
to perform some useful operation:
.\""
.IP "Dismiss"
This button saves the content of the note, the geometry of the window
and the position of the cursor. It then closes the note editor.
.\""
.IP "Save"
This button is similar to the previous one, except that it does not
close the note editor.
.\""
.IP "Print"
The note is printed using an external command. The print command can
be configured using the
.I printCommand
resource.
.\""
.IP "Title"
This button allows you to change the title associated to the note.
.\""
.IP "Clear"
This button clears the note content. A confirmation dialog box is
opened to make sure that the note is not cleared accidentally. The
note editor remains open and the note still exists.
.\""
.IP "Destroy"
This button deletes the note. A confirmation dialog box is opened
to make sure that the note is not deleted accidentally. The note
editor is closed.
.\""
.IP "Select"
This button makes the complete note content selected and ready for
paste operation with the mouse.
.\""
.SH CONFIGURATION
.\""
.I Xcra
uses its own format to customize some labels or reports. It uses a format
similar to the Unix
.IR printf (3)
primitive based on the
.I %
character modifier. The format allows to get access to the most important
characteristics of a task. Following the
.I %
character, any width or precision number may be specified.
.\""
.PP
.I Xcra
uses a small stack to get the time values which must be reported.
During the analysis of the
.I %
format, the following modifiers will fill the stack:
.TP 3
t
Engage the time for the task on the current day.
.TP 3
T
Engage the time for all the tasks on the current day.
.TP 3
w
Engage the time for the task on the current week.
.TP 3
W
Engage the time for all the tasks on the current week.
.TP 3
f
Engage the time for the task on the current fortnight.
.TP 3
F
Engage the time for all the tasks on the current fortnight.
.TP 3
a
Engage the time for the task on the current month.
.TP 3
A
Engage the time for all the tasks on the current month.
.\""
.PP
The stack of values must be filled before using some formats.
The following formats are available:
.TP 3
H
Prints the hours part of a time.
.TP 3
M
Prints the minutes part of a time.
.TP 3
S
Prints the seconds part of a time.
.TP 3
p
Prints the percentage of two values.
.TP 3
N
Prints the task name.
.TP 3
R
Prints the task reference.
.TP 3
d
Prints the current day of the month (or the current selected
day for the report dialog box).
.TP 3
m
Prints the current month of the year (or the current selected
month for the report dialog box).
.TP 3
y
Prints the current year.
.TP 3
D
Prints the directory path where the files are saved.
.TP 3
%
Prints the % character.
.\""
.PP
For example, the following formats strings could be defined:
.\""
.TP  18
%2tH:%02tM:%02tS
Prints the time for the task (current day) in hours, minutes and
seconds. For example, 3:45:23.
.TP 18
%2aH:%02aM:%02aS
Idem but use the total time for the month.
.TP 18
%2wH:%02wM
Idem but use the total time for the week.
.TP 18
%3.3tfp%%
Prints the percentage of the time spent during the current day
over the total time spent on this task during the current
fortnight. For example, 12.4%.
.TP 18
%3.3tFp%%
Prints the percentage of the time spent during the current day
over the total time spent for all the tasks during the current
fortnight. For example, 3.3%.
.TP 18
%-20.20T %-20.20R
Prints the task name and the task reference with a 20 character
width and precision.
.\""
.\""
.SH ACTIONS
.\""
In addition to the standard actions provided by the X toolkit
and the Athena Widgets, the following actions are available:
.\""
.TP 18
do-close
Closes the dialog box or the whole application (if invoked on the
main window).
.TP 18
do-help
Activates the help on the current dialog box.
.TP 18
do-activate
Validate the dialog box and execute the operation associated with
it. This is similar to hitting the bottom most left button (most
commonly an
.B Ok
button).
.TP 18
focus-tab-group
If the dialog box has a scrolled window, the window is scrolled to
make visible the text field which has the keyboard focus. If the
dialog box has no scrolled window or no text fields, this action
has no effect.
.TP 18
next-tab-group
Change the keyboard focus to the next text field of the dialog box,
updating the scroll if needed. If the dialog box has no text field
or only one, this action has no effect.
.\""
.\""
.\""
.SH "APPLICATION RESOURCES"
.I Xcra
has the following application-specific resources which allow
customizations unique to
.IR Xcra .
.PP
.TP 18
\fBtaskMenuSize\fP (Class \fBTaskMenuSize\fP)
The maximum number of task names which can be displayed in the task
popup menu. Default is 10.
.\""
.TP 18
\fBtaskUpdateTime\fP (Class \fBTaskUpdateTime\fP)
The task update time in seconds. Default is 60.
.\""
.TP 18
\fBtaskDirectory\fP (Class \fBTaskDirectory\fP)
The directory where
.I Xcra
must save its report files. Default is ".cra".
.\""
.TP 18
\fBautoPlace\fP (Class \fBAutoPlace\fP)
Whether dialog boxes should be placed by
.I Xcra
or by yourself or your window manager. Default is true.
.\""
.TP 18
\fBsemanticErrors\fP (Class \fBSemanticErrors\fP)
Boolean resource which indicates whether semantic errors are
enabled. Default is true.
.\""
.TP 18
\fBringBell\fP (Class \fBRingBell\fP)
Boolean resource which indicates whether the bell must be rung if an
error is detected. Default is true.
.\""
.TP 18
\fBallowClose\fP (Class \fBAllowClose\fP)
This resources enables the closing of the main
.I Xcra
window. When this resource is false, closing the main window is impossible.
Default is false.
.\""
.TP 18
\fBtransientShell\fP (Class \fBTransientShell\fP)
Boolean resource which indicates whether a transient shell must be
used for dialog boxes. Transient shells are used for dialog boxes and
are different from the main application window. Window managers may
decorate transient shell differently and may forbid their iconification.
When this resource is false,
.I Xcra
will create a window similar to the main application window.
Window managers will see dialog boxes as standard application windows.
Default is true (transient shell).
.\""
.TP 18
\fBtaskFormat\fP (Class \fBReportModel\fP)
Format string which must be used to display the current
active task. Default is "%N" (task name).
.\""
.TP 18
\fBtimeFormat\fP (Class \fBReportModel\fP)
Format string which must be used to display the time associated to
the current task. Default is "%2tH:%02tM" (time in hours/minutes).
.\""
.TP 18
\fBdayReportModel\fP (Class \fBReportModel\fP)
Format string used by the report dialog box to report the time for
a task when the
.I Day
report mode is selected. Default is "%-20.20N %-20.20R %2tH:%02tM".
.\""
.TP 18
\fBdayTotelModel\fP (Class \fBReportModel\fP)
Format string  used by the report dialog box to report a summary
for all the tasks when the
.I Day
report mode is selected. Default is "Total day %2TH:%02TM".
.\""
.TP 18
\fBweekReportModel\fP (Class \fBReportModel\fP)
Format string used by the report dialog box to report the time for
a task when the
.I Week
report mode is selected. Default is "%-20.20N %-20.20R %2wH:%02wM".
.\""
.TP 18
\fBweekTotalModel\fP (Class \fBReportModel\fP)
Format string  used by the report dialog box to report a summary
for all the tasks when the
.I Week
report mode is selected. Default is "Total day %2TH:%02TM   Total week %2WH:%02WM".
.\""
.TP 18
\fBfortnightReportModel\fP (Class \fBReportModel\fP)
Format string used by the report dialog box to report the time for
a task when the
.I Fortnight
report mode is selected. Default is "%-20.20N %-20.20R %2fH:%02fM".
.\""
.TP 18
\fBfortnightTotalModel\fP (Class \fBReportModel\fP)
Format string  used by the report dialog box to report a summary
for all the tasks when the
.I Fortnight
report mode is selected. Default is "Total day %2TH:%02TM   Total fortnight %2FH:%02FM".
.\""
.TP 18
\fBmonthReportModel\fP (Class \fBReportModel\fP)
Format string used by the report dialog box to report the time for
a task when the
.I Month
report mode is selected. Default is "%-20.20N %-20.20R %2aH:%02aM".
.\""
.TP 18
\fBmonthTotalModel\fP (Class \fBReportModel\fP)
Format string  used by the report dialog box to report a summary
for all the tasks when the
.I Month
report mode is selected. Default is "Total day %2TH:%02TM   Total month %2AH:%02AM".
.\""
.TP 18
\fBreportCommand\fP (Class \fBReportCommand\fP)
Format string used by the report dialog box to determine the external
command which must be executed.
Default is REPORT_COMMAND.
.\""
#ifdef HAVE_IMPORT
.TP 18
\fBimportCommand\fP (Class \fBImportCommand\fP)
Command which must be executed to build the list of imported tasks.
The command must create or update the file
.I import.tasks
stored the cra directory.
Default is IMPORT_COMMAND.
#endif
.\""
.TP 18
\fBprintCommand\fP (Class \fBPrintCommand\fP)
Command which must be executed to print a note.
When printed, the note is saved in a temporary file and the path
of that temporary file is appended to the print command.
#if HAVE_PUTENV
The note title is passed in the \fBNOTE\fP environment variable.
#endif
Default is PRINT_COMMAND.
.\""
#ifdef HAVE_XTOOLS
.TP 18
\fBhasClock\fP (Class \fBHasTools\fP)
Boolean resource which when set activates the creation of the Clock
widget to display the current time.
.\""
.TP 18
\fBhasEyes\fP (Class \fBHasTools\fP)
Boolean resource which when set activates the creation of the Eyes
widget to track the mouse position.
.\""
.TP 18
\fBhasBiff\fP (Class \fBHasTools\fP)
Boolean resource which when set activates the creation of the Biff
widget to display when new mail arrives.
#endif
.\""
.SH PROBLEMS
.\""
.I Xcra
saves its files regularly, each time the time associated to a task is
changed. If it encounters a problem, it opens an error dialog box and
prints some error messages in it. In addition, it sends you a mail with
the content of the file.
.\""
.\""
.SH LIMITATIONS
.\""
.I Xcra
can only manage 100 tasks.
.\""
.SH BUGS



.I Xcra
is not smart enought to detect when you are outside, sleeping, eating,
or in holidays.

There is no
.I previous-tab-group
associated key (opposite of Tab key).

.SH AUTHOR
St\('ephane Carrez
ciceron@chorus.fr
#ifdef HAVE_XTOOLS
.br
Xeyes widget by Keith Packard, MIT X Consortium
.br
Xbiff widget by Jim Fulton, MIT X Consortium
#endif
.\""
.\""
.\""
.SH "SEE ALSO"
.IR X (1),
.IR cra-report (1)
.IR noteManager (1)
