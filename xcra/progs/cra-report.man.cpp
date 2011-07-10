.\" @(#)cra-report.1	6/04/95
.\"" $Id: cra-report.man.cpp,v 1.4 1996/08/04 15:37:51 gdraw Exp $
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
.ds CH C\s-2HORUS\s+2
.TH cra-report 1 "Aug 4, 1995"
.SH NAME
cra-report \- Cra Summary Report Tool
.SH SYNOPSIS
.B cra-report
[
.B \-directory
.I dir
] [
.B \-week
.I week
] [
.B \-week-last
.I week
] [
.B \-full-year
] [
.B \-day
.I day
.B \-month
.I month
.B \-year
.I year
] [
.B \-round
] [
.B \-exact
] [
.B \-preserve
] [
.B \-one
#if HAVE_LDBUPDATE
] [
.B \-ldb
#endif
] [
.B \-report
] [
.B \-oleo
] [
.B \-troff
] [
.B \-v
] [
.B \-help
]
.SH DESCRIPTION
.I cra-report
uses the log files produced by
.IR xcra (1)
to generate various summary reports showing the time spent on the different
activities. Most summaries will list the different tasks with the time
spent on them for the different days of a week.
.\""
.PP
.I cra-report
generates several kinds of summaries:
.\""
.TP "\w'fortnight  'u"
week
A week summary reports the time spent on the different tasks for a
particular week. The report indicates the week number, the time for
each task and for each of the seven days of the week. A total for
each task and each day is also computed.
.\""
.TP
fortnight
A fortnight summary is similar to a week summary except that it is
applied to 2 weeks.
.\""
.TP
year
A year summary reports the time spent on the different tasks for each
month of the year.
.\""
.TP
report
The report summary is suitable to produce weekly reports and indicate
the progress of the project.
.\""
#ifdef HAVE_LDBUPDATE
.TP
ldb
The \fBldb\fP report is suitable for the
.IR LDBupdate (1)
external database. The output of
.I cra-report
can be directly passed to
.IR LDBudpate .
#endif
.\""
.PP
.I cra-report
supports several output formats for the generation of the summaries.
Changing the output format of
.I cra-report
is only possible with the
.IR week ,
.IR fortnight ,
or
.I year
summary reports.
.\""
.TP "\w'normal  'u"
normal
The summary is in the ASCII format and does not need any further processing.
This format is the default format.
.\""
.TP
oleo
The summary is generated in the GNU Oleo 1.6 spreadsheet format.
This format is suitable to import the different times in a datasheet and
perform some computations on them.
.\""
.TP
troff
The output consists of a troff table holding the different values.
The result can be included in a troff document which must be
formatted using
.IR tbl (1)
and
.IR troff (1).
.\""
.PP
.I cra-report
can report the time values in two forms: an exact value showing the hours
and minutes and a rounded value giving the hours part only.
When it rounds all the times down to the lowest hour boundary, the
remaining time are saved in a file and are taken into account for the
next fortnight report. It is therefore extremely important to report
the time in the fortnight increasing order (fortnight 9501, 9503, ...).
Otherwise, the remaining time of the previous fortnight cannot be
included. Anyway, a warning message is reported if
.I cra-report
does not find the remaining times for the fortnight.
.\""
.SS OPTIONS
.\""
.I cra-report
supports the following options:
.TP "\w'-full-year  'u"
-directory \fIdir\fP
Specifies the directory where the cra files are saved.
.\""
.TP
-week \fIweek\fP
Specifies the week number of the first week which must be reported.
.\""
.TP
-week-last \fIweek\fP
Specifies the week number of the last week which must be reported.
.\""
.TP
-day \fIday\fP
.TP
-month \fImonth\fP
.TP
-year \fIyear\fP
Specifies a day of the year which represents the first week which
must be reported.
.\""
.TP
-one
Use the
.I week
summary to produce a report for one week at a time.
The default is to report two weeks at a time.
.\""
.TP
-full-year
Use the
.I year
summary to produce a full-year report. 
.\""
.TP
-round
Round the time values to report only the hour part.
.\""
.TP
-exact
Print the time in hours and minutes.
.\""
.\""
.TP
-report
Produces a weekly summary report.
.\""
.TP
-oleo
Use the GNU Oleo datasheet output format.
.\""
.TP
-troff
Use the troff tbl table output format.
.\""
#ifdef HAVE_LDBUPDATE
.TP
-ldb
Use the LDBupdate output format.
#endif
.\""
.TP
-help
Print a help description.
.\""
.\""
.\""
.SH AUTHOR
St\('ephane Carrez
ciceron@chorus.fr
.\""
.\""
.\""
.SH "SEE ALSO"
.IR tbl (1),
.IR troff (1),
#ifdef HAVE_LDBUPDATE
.IR LDBupdate (1),
.IR people (1),
#endif
.IR xcra (1)
