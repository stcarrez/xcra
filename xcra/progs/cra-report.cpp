#!PERL_PATH
# ######################################################################
#
# Component :	cra-report
#
# Synopsis  :	Cra Summary Report Tool
#
# Copyright (C) 1994, 1995, 1996 Stephane Carrez
#
# This file is part of Xcra.
#
# Xcra is free software; you can redistribute it and/or modify
# It under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Xcra is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# ######################################################################
eval 'exec PERL_PATH -S $0 ${1+"$@"}'
        if $running_under_some_shell;

sub help { select(STDERR); print <<EOF;

Cra Summary Report Tool v1.5

Usage:	cra-report [-v] [-help] [-week <week>] [-week-last <week>] [-full-year]
		   [-day <day>] [-month <month>] [-year <year>]
		   [-round] [-exact] [-preserve] [-one] [-directory dir]
#ifdef HAVE_LDBUPDATE
		   [-ldb] [-report] [-oleo] [-troff]
#else
		   [-report] [-oleo] [-troff]
#endif

Report selection:
-----------------
-w <week>	Uses the week <week> instead of the current week of the day.
-week <week>	The week is the week number: 1..52. By default, the week
		contains 14 days (except for -report).

-day <day>	Specifies a day of the week instead of the week number.
-month <month>
-year <year>

-full-year	Select everything to produce a report for the complete year.

-week-last <week> Report everything starting from the week specified with
		the -week option, up to the week <week>.

List options:
------------
-r		Round each time into the closest lowest hour. The remaining
-round		time is reported to the next day and so on.

-e		Print the exact time in hours and minutes.
-exact

-one 		Print the report only for one week (7 days) instead of 14 days.


Format options:
--------------
-l		Round each time into the closest lowest hour and use an output
#ifdef HAVE_LDBUPDATE
-ldb		format suitable for LDBupdate.
#endif

-oleo		Round each time into the closest lowest hour and use an output
		format suitable for the GNU Oleo spreadsheet.

-report		Generates a report form for a week (only 7 days). Percentage
        	are computed only for that week.

-troff		Generates a report in the troff tbl format. The result must
		be processed by tbl and troff.

Misc options:
-------------
-v		Verbose flag.

-h		Prints this help message
-help

-preserve	Preserve the time remaining report file of the current week.
		By default, with the `-round' option, the time
		values which couldn't be reported are saved in a file.

-directory <dir> Specifies the directory where the cra files are saved.

EOF

    exit(0);
}
#
# History:
# --------
# 1.0 17/12/94	First release
# 1.1 21/02/95	Week report does not take into account Sunday and Saturday
#		Corrected problem with week number for LDBupdate format
# 1.2 17/03/95  Problems if the week interlaps 2 months and the next month
#		file is not yet created
# 1.3 11/04/95	Update to support -day, -month, -year options needed for
#		the integration of cra-report from xcra
# 1.4 03/11/95	Added the -oleo and ability to generate Gnu Oleo format
#		Added the -full-year report
# 1.5 04/11/95	Introduced the virtual sub rountines. Rewrote the print
#		section to be more generic. Supports the Oleo, Troff, Ldb,
#		Normal, Report classes output formats. New output formats
#		can easily be added.
#

#
# Note :	In the comments, `week' means `quinzaine' (15 days)
#
#
$CraTasks = 0;
$CraDirectory = ".";
$weekYear = 0;
$CraNoTask= "None";

# Global Tables:
# --------------
#
# @dayTime[0..31]	Indicates the total time for a given day
#
# %timeTable{<tsk>}	Records a string which represent the time for
#			each day of the month for a given task.
#
# %refTable{<tsk>}	Records a task reference for a given task
#
# @tskTable		List of tasks
#
# %reportTable{<tsk>}	Rounded values of the previous week and
#			which must be reported to the next week.
#
# %weekTime{<tsk>}	The per-day-task time for a complete fortnight
#			The format is the same as for %timeTable.
#			Day 0 is the first day of the week.
#
# $weekNumber		Number which correspond to the week
#

local(%refTable, @tskTable, %timeTable, %weekTime, @dayTime);
local(%totalTable);

$Month[1] = "Jan";
$Month[2] = "Feb";
$Month[3] = "Mar";
$Month[4] = "Apr";
$Month[5] = "Mai";
$Month[6] = "Jun";
$Month[7] = "Jul";
$Month[8] = "Aou";
$Month[9] = "Sep";
$Month[10]= "Oct";
$Month[11]= "Nov";
$Month[12]= "Dec";

sub message {
    local($msg) = @_;

    if ($verbose) {
	print "$msg";
    }
}

sub mrun {
    local($num) = @_;

    if ($verbose) {
	$num = $num % 4;
	if ($num == 1) {
	    print "\b/";
	} elsif ($num == 2) {
	    print "\b-";
	} elsif ($num == 3) {
	    print "\b\\";
	} else {
	    print "\b|";
	}
    }
}
$Indent = "";

# ######################################################################
#
# Read/Write routines
#
# ######################################################################

#
# Role :	Read a cra file
#
# Usage:	&ReadCra(<month>, <year>);
#
sub ReadCra {
     local($month, $year) = @_;
     local($file, $task, $ref, $line, $nrTasks);

	#
	# Build the file name and open it
	#
     $file = sprintf("$CraDirectory/t-%04d-%02d", $year + 1900, $month + 1);
     &message($Indent . "Reading $file...");
     if (open(CRAFILE, $file)) {

	@tskTable = keys %timeTable;
	foreach $task (@tskTable) {
	    $timeTable{$task} = "";
	}

	#
	# Read in the file filling the different tables
	#
        $nrTasks = -1;
        while (<CRAFILE>) {
	    chop;
	    $line = substr($_, 2);

	    if (/^T=/) {
		$task = $line;
	    } elsif(/^R=/) {
		$refTable{$task} = $line;

	    } elsif(/^C=/) {
		$timeTable{$task} = $line;
		#printf("$line\n");
	    }
	}
	close(CRAFILE);
    } else {
	@tskTable = keys %timeTable;
	foreach $task (@tskTable) {
	    $timeTable{$task} = "";
	}
    }

     &message("Crunching...");

     @tskTable = keys %timeTable;
	#
	# Remove all the time entries we are not interested in
	#
     foreach $task (@tskTable) {
	next if ($refTable{$task} ne $CraNoTask);

	delete $timeTable{$task};
	delete $refTable{$task};
     }
     @tskTable = keys %timeTable;

     &message("Analyzing...");
     &FindDayTime;

     &message("Done\n");
     return $nrTasks;
}


#
# Role :	Read a cra file holding rounded times of the previous week
#
# Usage:	&ReadReportTimes(<week>, <year>);
#
sub ReadReportTimes {
     local($week, $year) = @_;
     local($file, $task, $line);

	#
	# Build the file name and open it
	#
     $file = "$CraDirectory/r-$year-$week";
     if (!open(CRAFILE, $file)) {
	warn("The rounded values of the week $year$week were not computed\n");
	warn("Please check the file $file\n");
	warn("And correct the problem...\n");

	foreach $task (@tskTable) {
	    $reportTable{$task} = "0";
	}
     } else {

	foreach $task (@tskTable) {
	    $reportTable{$task} = "0";
	}
	#
	# Read in the file filling the different tables
	#
	while (<CRAFILE>) {
	  chop;
	  $line = substr($_, 2);
	  if (/^T=/) {
	     $task = $line;
	  } elsif(/^R=/) {
	     if ($refTable{$task} ne $line) {
		 $a=0;
	     }
	  } elsif(/^C=/) {
	     $reportTable{$task} = $line;
	  }
       }
       close(CRAFILE);
     }
}


#
# Role :	Save the rounded values which were computed from
#		the current week values. These values will be used
#		for the next week.
#
# Usage:	&WriteReportTimes(<week>, <year>);
#
sub WriteReportTimes {
     local($week, $year) = @_;
     local($file, $task);

	#
	# Build the file name and open it
	#
     $file = "$CraDirectory/r-$year-$week";
     die "Cannot write file $file\n"
	if (!open(CRAFILE, ">$file"));

     select(CRAFILE);

	#
	# Save the rounded time for each task
	#
     foreach $task (@tskTable) {
	print "T=$task\n";
	print "R=$refTable{$task}\n";
	print "C=$reportTable{$task}\n";
     }
     close(CRAFILE);
}



#
# Role :	Read the time values which correspond to a particular
#		week of the year. Also read in the report values of
#		the previous week.
#
sub ReadWeekTimes {
    local($week, $year) = @_;
    local($tsk, $tm, $day);
    local($needRescan) = 0;

  Rescan:
    for(;;) {

	#
	# Get the cra file for the beginning of the week
	#
    &ReadCra($weekStartMonth[$week], $year);

    $weekNumber = $week;

	#
	# Clear the time of the %weekTime table
	#
    foreach $tsk (@tskTable) {
	$weekTime{$tsk} = "";
	#printf("$tsk ");
    }
    #printf("\n");

	#
	# Loop until the last day is reached (may be in another month)
	#
    $day = $weekStartDay[$week];
  loop:
    for (;;) {

		#
		# Get and concatenate the times
		#
	foreach $tsk (@tskTable) {
	    $tm = &GetTime($day, $tsk);
	    $weekTime{$tsk} .= " $tm";
	}
		#
		# Stop when the last day is processed
		# 
	last if ($day == $weekEndDay[$week]);
	$day = &NextWeekDay($day, $week);

		#
		# Detect whether we changed the month and have to
		# read a new cra file.
		#
	if ($day == 1) {
	    &ReadCra($weekEndMonth[$week], $year);
	    $needRescan = $needRescan + 1;
	    next Rescan if ($needRescan == 1);
	}
    }
    last Rescan;
    }
#    foreach $tsk (@tskTable) {
#	printf("%s %s [", $tsk, $weekTime{$tsk});
#	for ($v = 1; $v < 16; $v = $v + 1) {
#	    printf("%d ", &GetWeekTime($v, $tsk));
#	}
#	printf("]\n");
#    }

	#
	# Get previous week report times (may be previous year too)
	#
    $week = $week - 1;
    if ($week <= 0) {
	$week = 26;
	$year = $year - 1;
    }
    &ReadReportTimes($week, $year);
}


# ######################################################################
#
# Week number computational routines and other bizarre hacks
#
# ######################################################################

#
# X Role :	Build a table which indicates the first day and last
#		day of a Chorus week (quinzaine). The result is saved
#		in global tables:
#
#		@weekStartMonth	<month>		Start of the week
#		@weekStartDay   <day>
#
#		@weekMiddleDay  <day>		The last day of the
#						month for the week, when
#						this week overlaps 2 months.
#
#		@weekStopMonth	<month>		End of the week (inclusive)
#		@weekStopDay	<day>
#
#		$weekYear			Year
#
#		The two tables are indexed by the week number (1..26)
#
# Usage:	&FindWeeks(<year>)
#
sub FindWeeks {
    local($yearToFind) = @_;
    local($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst);
    local($theTime);
    local($Week) = 0;
    local($chorusWeek) = 0;

	#
	# Find the time which correspond to `1/1/$yearToFind'
	# Note that leap years are handled by `localtime'
	# 
    $theTime = time();
    yearScan:
    for (;;) {
	($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst)
		= localtime($theTime);

	$year = $year + 1900;
	if ($year != $yearToFind) {
	    $theTime = $theTime + (($yearToFind - $year) * 365 * 24 * 3600);

	} elsif ($yday != 0) {
	    $theTime = $theTime - ($yday * 24 * 3600);
	} else {
	    last yearScan;
	}
    }

    #
    # Walk each day of the year (starting at 1/1) and compute the week
    # by observing the Monday/Sunday days
    #
    $weekYear = $yearToFind;
    while ($year == $yearToFind) {
	local($prevDay);

	$prevDay = $mday;
	($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst)
		= localtime($theTime);

	$year = $year + 1900;
	if ($mday == 1) {
	    $weekMiddleDay[$chorusWeek] = $prevDay;
	}

		#
		# This is monday or the first day of the year.
		#
	if ($wday == 1 || $chorusWeek == 0) {
	    $Week = $Week + 1;
	    if (($Week % 2) == 1) {
		$chorusWeek = $chorusWeek + 1;
		$weekStartMonth[$chorusWeek] = $mon;
		$weekStartDay[$chorusWeek] = $mday;
		$weekMiddleDay[$chorusWeek] = 0;
	    }

		#
		# Last day of the week is the current day
	        # When we switch to another week, on Monday, the last
	        # day is Sunday (I hope)
		#
	} else {
	    $weekEndMonth[$chorusWeek] = $mon;
	    $weekEndDay[$chorusWeek] = $mday;
	}

		#
		# Process next day of the year
		#
	$theTime = $theTime + (24 * 3600);
    }
}

#
# Role :	Return the next day of the week observing the
#		week start/end table
#
sub NextWeekDay {
    local($day) = @_;

		#
		# Detect whether we change the month and have to
		# read a new cra file.
		#
    if ($weekStartMonth[$weekNumber] != $weekEndMonth[$weekNumber]) {
	if ($weekMiddleDay[$weekNumber] == $day) {
	    $day = 1;	# Beginning of the month is 1 (I think)
	} else {
	    $day ++;
	}
    } else {
	$day ++;
    }
    return $day;
}


#
# Role :	Utility routine to dump the week tables
#
# Usage:	&PrintWeeks;
#
sub PrintWeeks {
    local($i);

	#
	# As far as I remember, there is only 26 `quinzaine' each year
	#
    for ($i = 1; $i <= 26; $i ++) {
	printf("%2d %s %s\n", $i, $weekStart[$i], $weekEnd[$i]);
    }
}

# &FindWeeks(1994);
# &PrintWeeks;

# &FindWeeks(1995);
# &PrintWeeks;

#
# Role :	Find the week number given the day, the month and the year
#
# Usage:	&FindWeek(<day-of-month>, <month>, <year>);
#
sub FindWeek {
    local($mday, $mon, $year) = @_;
    local($i, $week, $d, $k);

    if ($year <= 0 || $mon < 0 || $mday < 0) {
	local($curTime) = time;
	local($sec, $min, $hour, $md, $mn, $yr, $wday, $yday, $isdst);

	($sec, $min, $hour, $md, $mn, $yr, $wday, $yday, $isdst)
		= localtime($curTime);

	$year = $yr if ($year <= 0);
	$mon  = $mn if ($mon < 0);
	$mday = $md if ($mday < 0);
    }

    $curYear = $year;
    $curMonth= $mon;
    $curDay  = $mday;
    if ($weekYear != $year + 1900) {
	&FindWeeks($year + 1900);
    }
    # ? May be there exist simpler/better algorithms...
    for ($i = 1; $i <= 26; $i ++) {
	if ($weekStartMonth[$i] == $mon) {
	    next if ($weekStartDay[$i] <= $mday);

		#
		# Determine whether we are on the first week or second
		# week of the fortnight
		#
	    $week = 2 * ($i - 1) - 2;
	    $d = $weekStartDay[$i - 1];
	    #printf("X: $d $mday $week $i\n");
	    for ($k = 1; $d != $mday; $k ++) {
		if ($weekStartMonth[$i - 1] != $mon) {
		    if ($weekMiddleDay[$i - 1] == $d) {
			$d = 1;
		    } else {
			$d ++;
		    }
		} else {
		    $d ++;
		}
		if ($k > 14) {
		    $mon ++;
		    &error("Invalid date $mday/$mon/$year\n");
		    exit(1);
		}
	    }
	    $week = $week + 1
		if ($k > 7);
	    return $week + 1;

	} elsif ($weekStartMonth[$i] > $mon) {
	    $week = 2 * ($i - 1) - 2;

	    $d = $weekStartDay[$i - 1];
	    #printf("Y: $d $mday $week $i\n");
	    for ($k = 1; $d != $mday; $k ++) {
		if ($weekMiddleDay[$i - 1] == $d) {
		    $d = 1;
		} else {
		    $d ++;
		}
		if ($k > 14) {
		    $mon ++;
		    &error("Invalid date $mday/$mon/$year\n");
		    exit(1);
		}
	    }
	    $week = $week + 1
		if ($k > 7);
	    return $week + 1;
	}
    }
    return 52;
}

#
# Role :	Find the current week number according to the time
#		of the day
#
# Usage:	&FindCurrentWeek;
#
sub FindCurrentWeek {
    return &FindWeek(-1, -1, -1);
}


# ######################################################################
#
# Time and other computational routines
#
# ######################################################################

#
# Role :	Return the time for a task on a specific day
#
# Usage :	&GetTime(<day>, <task-name>)
#
sub GetTime {
    local($day, $task) = @_;
    local($timeString) = $timeTable{$task};

    return "0" if ($timeString eq "");

    local(@times) = split(/ /, $timeTable{$task});

    return "0" if ($#times < $day);

    return $times[$day];
}

#
# Role :	Return the time for a task on a specific day of the week
#
# Usage :	&GetWeekTime(<day-of-week>, <task-name>)
#
sub GetWeekTime {
    local($day, $task) = @_;

    local(@times) = split(/ /, $weekTime{$task});

    return $times[$day] if (&IsTakenIntoAccount($day));

    return 0;
}

#
# Role :	Compute the total time spent between 2 days
#
# Usage :	&TotalTask(<task>, <first-day>, <last-day>)
#
sub TotalTask {
    local($task, $firstDay, $lastDay) = @_;
    local(@times) = split(/ /, $timeTable{$task});
    local($total) = 0;

    while ($firstDay <= $lastDay) {
	$total += $times[$firstDay];
	$firstDay ++;
    }
    return $total;
}

sub IsTakenIntoAccount {
    local($day) = @_;

    return 1 if ($oneWeekFlag == 0);
    return 1 if ($weekReportFirst && $day <= 7);
    return 1 if ($weekReportFirst == 0 && $day > 7);
    return 0;
}


#
# Role :	Compute the total time spent during the week for a given task
#
# Usage :	&TotalWeekTask(<task>);
#
sub TotalWeekTask {
    local($task) = @_;
    local(@times) = split(/ /, $weekTime{$task});
    local($total) = 0;
    local($t, $day);

    $day = 0;
    foreach $t (@times) {
	$total = $total + $t
	    if (&IsTakenIntoAccount($day));
	$day ++;
    }
    return $total;
}


#
# Role :	Compute the total time spent between 2 days
#
# Usage :	&TotalDays(<first-day>, <last-day>)
#
sub TotalDays {
    local($firstDay, $lastDay) = @_;
    local($total) = 0;

    while ($firstDay <= $lastDay) {
	$total += $dayTime[$firstDay];
	$firstDay ++;
    }
    return $total;
}


#
# Role :	Compute the total time spent during the week
#
# Usage :	&TotalWeekTime;
#
sub TotalWeekTime {
    local($total) = 0;
    local($tsk);

    foreach $tsk (@tskTable) {
	$total += &TotalWeekTask($tsk);
    }
    return $total;
}


#
# Role :	Compute the `@dayTime' global table (total time / day)
#
# Usage :	&FindDayTime;
#
sub FindDayTime {
    local($day, $task);

    for ($day = 0; $day < 31; $day ++) {
	$dayTime[$day] = 0;
    }

    foreach $task (@tskTable) {

	local(@times) = split(/ /, $timeTable{$task});
	for ($day = 0; $day < 31; $day ++) {
	    $dayTime[$day] += $times[$day];
	}
    }
}


# ######################################################################
#
# Empty Print routines
#
# ######################################################################

sub EmptyPrintHeader {
}

sub EmptyPrintTailer {
}

sub EmptyPrintTaskName {
}
    
sub EmptyPrintElement {
}

sub EmptyPrintNewRow {
}

sub EmptyPrintSummary {
}

sub EmptyPrintWeekHeader {
}

sub EmptyPrintWeekTailer {
}

sub EmptyPrintMonthSeparator {
}

# ######################################################################
#
# Oleo Specific Print routines
#
# ######################################################################

sub OleoPrintHeader {
    local($cols, $rows) = @_;

    printf("# File created by cra-report 1.5\n");
    printf("#F;DGFL8\n");
    printf("F;W1 1 12\n");
    printf("F;W2 2 13\n");
    printf("O;auto;background;noa0;ticks 1\n");
    $oleoRow = 1;
    $oleoCol = 1;
}

sub OleoPrintTailer {
    printf("W;N1;A3 2;C7 0 7;Ostandout\n");
    printf("E\n");
}

sub OleoPrintTaskName {
    local($task) = @_;

    local($tskRow) = $oleoTask{$task};

    if ($tskRow == 0) {
	$tskRow = $oleoRow;
	$oleoTask{$task} = $tskRow;
	printf("C;r%d;c%d;K\"%s\"\n", $oleoRow, $oleoCol, $task);
	#printf("C;r%d;c2;K\"%s\"\n", $tskRow, $ref);
	$oleoCol ++;
    }
   
    return $tskRow;
}

sub OleoPrintNewRow {
    local($row) = @_;

    $oleoRow ++;
    $oleoCol = 1;
}

sub OleoPrintElement {
    local($val) = @_;

    printf("F;r%d;c%d;FDR\n", $oleoRow, $oleoCol);
    printf("C;K\"%s\"\n", $val);
    $oleoCol ++;
}

sub OleoPrintSummary {
    local($total, $roundedTime, $bigTotal) = @_;

    printf("F;r%d;c%d;FDR\n", $oleoRow, $oleoCol);
    printf("C;K\"%d\"\n", int($total / 3600));
    $oleoCol ++;
}

#ifdef HAVE_LDBUPDATE
# ######################################################################
#
# Ldb Specific Print routines
#
# ######################################################################

sub LdbPrintHeader {
    local($cols, $rows) = @_;

    printf("$userName\n");
    printf("%d%02d\n", $weekYear - 1900, ($weekNumber * 2) - 1);
}

sub LdbPrintTailer {
    printf("?\n\n\n\n\n");
}

sub LdbPrintSummary {
    local($total, $roundedTime, $bigTotal) = @_;

    printf("%s %d\n", $CurrentTaskRef, int($total / 3600));
}

sub LdbFindTasks {
    local($nameFlag) = 0;
    local($taskRef);
    local(%taskList);

    while (<STDIN>) {
	chop;
	if ($nameFlag) {
	    $taskList{$_} = $taskRef;
	    $nameFlag = 0;

	} elsif (/^\.LI/) {
	    $taskRef = $_;
	    $taskRef =~ s,.LI ,,;
	    $nameFlag = 1;
	}
    }
    local(@list) = sort { $a le $b } keys %taskList;
    foreach $taskRef (@list) {
	local($ref) = $taskList{$taskRef};
	print "T=$taskRef\n";
	print "R=$ref\n";
	print "C= 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0";
	print " 0 0 0 0 0 0 0 0 0\n";
    }
}
#endif

# ######################################################################
#
# Report Specific Print routines
#
# ######################################################################

sub ReportPrintHeader {
    local($cols, $rows) = @_;

    printf("$userName\n");
    printf("----------------\n");
    printf("	This Week:\n");
}

sub ReportPrintTailer {
    printf("\n	Next Week:\n");
}

sub ReportPrintSummary {
    local($total, $roundedTime, $bigTotal) = @_;

    printf("	* Task %s:	(%s) (%d%%)\n",
	   $CurrentTaskRef, $CurrentTask,
	   (($total * 100) / $bigTotal) + 0.5);
    printf("		Worked on...\n\n");
}

# ######################################################################
#
# Troff Specific Print routines
#
# &TroffHeader(<cols>, <rows>)
#
# ######################################################################

sub TroffPrintHeader {
    local($cols,$rows) = @_;
    local($i);

    printf(".TS\n| l |");
    for ($i = 1; $i <= $cols; $i++) {
	printf(" r |");
    }
    printf(" r | .\n_\n");
    $troffNeedTab = 0;
}

sub TroffPrintTailer {

    printf("\n_\n.TE\n");
}

sub TroffPrintTaskName {
    local($task) = @_;

    &TroffPrintElement($task);
}

sub TroffPrintElement {
    local($val) = @_;

    if ($troffNeedTab) {
	printf("\t%s", $val);
    } else {
	printf("%s", $val);
	$troffNeedTab = 1;
    }
}

sub TroffPrintNewRow {
    local($row) = @_;

    if ($troffNeedTab) {
	printf("\n");
	$troffNeedTab = 0;
    }
}

sub TroffPrintSummary {
    local($total, $roundedTime, $bigTotal) = @_;

	if ($bigTotal) {
	    printf("\t%3d %%", (($total * 100) / $bigTotal) + 0.5);
	} else {
	    printf("\t%3d", int($total / 3600));
	}
	if ($printRoundedTime) {
	    &Print($roundedTime);
	}
}

sub TroffPrintWeekHeader {
    local($weekDay, $day);

    printf("Task");

    $day     = $weekStartDay[$weekNumber];
    $weekDay = 1;
  loop:
    for (;; $weekDay = $weekDay + 1) {
	if (&IsTakenIntoAccount($weekDay)) {
	    printf("\t%2d", $day);
	}
	last if ($day == $weekEndDay[$weekNumber]);
	$day = &NextWeekDay($day);
    }
    printf("\n_\n");
}

sub TroffPrintWeekTailer {

    printf("\n_\nTotal");
    $troffNeedTab = 1;
}

sub TroffPrintMonthSeparator {

    if ($troffNeedTab) {
	printf("\n_\n");
    } else {
	printf("_\n");
    }
    $troffNeedTab = 0;
}

# ######################################################################
#
# Normal output Print routines
#
# &NormalHeader(<cols>, <rows>)
#
# ######################################################################

sub NormalWeekSeparators {
    local($i, $day);
    local($dayNum) = 1;

    printf("-------------------- ");

    $day = $weekStartDay[$weekNumber];
  loop:
    for (;;) {
	if (&IsTakenIntoAccount($dayNum)) {
	    if ($weekNumber == $curWeek && $day == $curDay) {
		print " ==";
		if ($printExactTime) {
		    print "===";
		}
	    } else {
		print " --";
		if ($printExactTime) {
		    print "---";
		}
	    }
	}
	$dayNum = $dayNum + 1;
	last if ($day == $weekEndDay[$weekNumber]);
	$day = &NextWeekDay($day);
    }
    print "\n";
}

sub NormalPrintHeader {
    local($cols,$rows) = @_;
}

sub NormalPrintTailer {

    printf("\n");
}

sub NormalPrintTaskName {
    local($task) = @_;

    printf("%-20.20s ", $task);
}

sub NormalPrintElement {
    local($val) = @_;

    printf(" %s", $val);
}

sub NormalPrintNewRow {
    local($row) = @_;

    printf("\n");
}

sub NormalPrintSummary {
    local($total, $roundedTime, $bigTotal) = @_;

	if ($bigTotal) {
	    printf("  %3d %%  ", (($total * 100) / $bigTotal) + 0.5);
	} else {
	    printf("  %3d ", int($total / 3600));
	}
	if ($printRoundedTime) {
	    &Print($roundedTime);
	}
}

#
# Role :	Print the header of a weekly report
#
sub NormalPrintWeekHeader {
    local($weekDay, $day);

    printf("Week : %d%02d\n", $weekYear - 1900, ($weekNumber * 2) - 1);
    printf("%-20.20s ", "Task");

    $day     = $weekStartDay[$weekNumber];
    $weekDay = 1;
  loop:
    for (;; $weekDay = $weekDay + 1) {
	if (&IsTakenIntoAccount($weekDay)) {
	    if ($printExactTime) {
		printf("  ");
	    }
	    printf(" %2d", $day);

	    if ($printExactTime) {
		printf(" ");
	    }
	}
	last if ($day == $weekEndDay[$weekNumber]);
	$day = &NextWeekDay($day);
    }
    printf("\n");

    &NormalWeekSeparators;
}

sub NormalPrintWeekTailer {

    &NormalWeekSeparators;

    printf("%-20.20s ", "Total");
}
    
sub NormalPrintMonthSeparator {
    $PrintDayFormat = "%3d";
}

# ######################################################################
#
# Pseudo Virtual Sub Routines:
# ----------------------------
#
# A virtual sub rountine is a perl sub rountine which can have several
# implementations and whose implementation can be changed dynamically.
# It is somewhat very close to C++ virtual methods (althougth there is
# no concept of inheritance and objects).
#
# A virtual sub rountine is called as follows:
#
#	&MethodCall(<vsub-rountine-name>, <args>);
#
# where <vsub-rountine-name> is the virtual sub rountine name and
# <args> represents its arguments.
#
# Virtual sub rountines are instantiated as follows:
#
#	&MethodClass(<class>, <method-list>)
#
# where <class> is the class name of the sub rountine implementation
# and <method-list> represents the list of methods. For a virtual sub
# rountine named <method>, the real perl sub rountine called is:
#
#	<class><method>
#
# Example:
#
#	&MethodClass("baseClass", "Print Dump List Save");
#	&MethodClass("intClass",  "Print List");
#	&MethodClass("longClass", "Save");
#
#	&MethodCall("Print", "(1, 2, \"string\")");
#	&MethodCall("Dump", "");
#
# and the following mapping will be made:
#
#	vsub		perl sub rountine
#	------------	-----------------
#	Print		intClassPrint
#	Dump		baseClassDump
#	List		intClassList
#	Save		longClassSave
#

local(%classMethod);	# Holds the list of virtual sub rountines


#
# Role :	Instantiate a list of virtual sub routines
#
# Usage:	&MethodClass(<class>, <method-list>)
#
sub MethodClass {
    local($class, $methodList) = @_;
    local(@methods) = split(/ /, $methodList);

    foreach $method (@methods) {
	$classMethod{$method} = $class . $method;
    }
}  

#
# Role :	Call the virtual sub rountine `method' and pass it
#		the arguments `args'.
#
# Usage:	&MethodCall(<method>, <args>)
#
# Example:	&MethodCall("print", "(1, 2, \"string\")")
#
sub MethodCall {
    local($method, $args) = @_;

    $method = $classMethod{$method};
    die if ($method eq "");
    #&message("Calling $method$args\n");
    eval "&$method$args;";
}


# ######################################################################
#
# Generic Print sub rountines
#
#
sub PrintHeader {
    local($cols,$rows) = @_;

    &MethodCall("PrintHeader", "($cols, $rows)");
}

sub PrintTailer {

    &MethodCall("PrintTailer", "");
}

sub PrintElement {
    local($val) = @_;

    &MethodCall("PrintElement", "(\"$val\")");
}

sub PrintSummary {
    local($total, $roundedTime, $bigTotal) = @_;

    &MethodCall("PrintSummary", "($total, $roundedTime, $bigTotal)");
}

sub PrintNewRow {
    local($row) = @_;

    &MethodCall("PrintNewRow", "($row)");
}

sub PrintWeekHeader {

    &MethodCall("PrintWeekHeader", "");
}

sub PrintWeekTailer {

    &MethodCall("PrintWeekTailer", "");
}

sub PrintMonthSeparator {

    &MethodCall("PrintMonthSeparator", "");
}

#
# Role :	Convert a time in seconds into a floating point number
#		expressed in hours.
#
sub Time {
    local($t) = @_;
    local($min, $hrs);

    $min = $t / 60;
    $hrs = $min / 60;
    $min = $min % 60;

    return ($hrs + ($min / 60.0));
}

#
# Role :	Print a time in hours and minutes
#
sub Convert { 
   local($tm) = @_;
   local($secs, $mins, $hrs);

   $secs=$tm % 60;
   $mins=$tm / 60;
   $hrs =$mins / 60;
   $mins=$mins % 60;

   return sprintf("%2d:%02d", $hrs, $mins);
}


#
# Role :	Print the task name or the task reference
#
sub PrintTaskName {
    local($tsk) = @_;
    local($name, $ref);

    $ref = $refTable{$tsk};
    $name = $ref;
    if ($name eq "") {
	$name = $tsk;
    }
    $CurrentTask    = $tsk;
    $CurrentTaskRef = $ref;
    &MethodCall("PrintTaskName", "(\"$name\")");
}

$PrintDayFormat = "%2d";

sub PrintDayTime {
    local($time, $rtime) = @_;
    local($val);

    if ($printRoundedTime) {
	$val = sprintf($PrintDayFormat, $rtime);

    } else {
	$val = &Convert($time);
    }
    &PrintElement($val);
}

#
# Role :	Report the time for each task and for a week
#
# Usage:	&WeeklyReport(<week>, <year>)
#
sub WeeklyReport {
    local($week, $year) = @_;
    local($tsk, $day, $weekDay, $roundCra, $tm, $t, $total);
    local($checkTotal) = 0;
    local($roundedTotal) = 0;
    local($bigTotal, $name, $weekTotalTime, $weekTaskTotal);

    if ($oneWeekFlag) {
	$weekReportFirst = int($week % 2);
	$week = int(($week + 1) / 2);
    } else {
	$weekReportFirst = 0;
	$week = int(($week + 1) / 2);
    }

	#
	# Read in the times for the week
	#
    &ReadWeekTimes($week, $year);

    if ($oneWeekFlag) {
	&PrintHeader(7,0);
    } else {
	&PrintHeader(14,0);
    }
    &PrintWeekHeader;

    $bigTotal = &TotalWeekTime;
    $weekTaskTotal = 0;

	#
	# Clear some tables
	#
    $weekDay  = 1;
    $day      = $weekStartDay[$week];

  loop:
    for(;;) {
	$dayTime[$weekDay] = 0;
	$rdayTime[$weekDay] = 0;

	last if ($day == $weekEndDay[$week]);
	$day     = &NextWeekDay($day, $week);
	$weekDay ++;
    }

	#
	# For each task, print the time for each day of the week
	#
    foreach $tsk (@tskTable) {
	$weekTotalTime = &TotalWeekTask($tsk);
	$weekTaskTotal = $weekTaskTotal + $weekTotalTime;

	#printf("Task $tsk $weekTotalTime\n");
		#
		# Spent no time during this week for this task
		#
	next if ($weekTotalTime == 0);

	&PrintTaskName($tsk);

		#
		# Get the round time of the previous set (same task)
		#
	$roundCra = $reportTable{$tsk};
	$total    = $roundCra;
	$checkTotal = $checkTotal - $roundCra;

	$weekDay  = 1;
	$day      = $weekStartDay[$week];

      loop_day:
	for (;;) {
	    if (&IsTakenIntoAccount($weekDay)) {
		$tm = &GetWeekTime($weekDay, $tsk);
		$t  = $tm;

		#
		# Round values up to the hour
		#
		$total    += $tm;
		$tm       += $roundCra;
		$roundCra = $tm % 3600;
		$tm       = int($tm / 3600);

		$dayTime[$weekDay]  += $t;
		$rdayTime[$weekDay] += $tm;

		#
		# Print something
		#
		&PrintDayTime($t, $tm);
	    }

	    last if ($day == $weekEndDay[$week]);
		#
		# Move to next day
		#
	    $day     = &NextWeekDay($day, $week);
	    $weekDay ++;
	}
	if ($total - $reportTable{$tsk} != $weekTotalTime) {
	    local($n) = $total - $reportTable{$tsk};

	    &error("Difference observed between 2 sums for task $tsk");
	    &error("Total $weekTotalTime and found $n");

	}
	$name = $refTable{$tsk};
	$name = $tsk if ($name eq "");
	&PrintSummary($total, $roundCra, $bigTotal);

	$roundedTotal	  = $roundedTotal + $roundCra;
	$reportTable{$tsk}= $roundCra;

		#
		# Compute total for final check
		#
	$checkTotal += $total;

	&PrintNewRow;
    }

    &PrintWeekTailer;

	#
	# Print a summary for each day of the week
	#
    $weekDay  = 1;
    $day      = $weekStartDay[$week];

  loop:
    for(;;) {
	if (&IsTakenIntoAccount($weekDay)) {
	    &PrintDayTime($dayTime[$weekDay], $rdayTime[$weekDay]);
	}
	last if ($day == $weekEndDay[$week]);
	$day     = &NextWeekDay($day, $week);
	$weekDay ++;
    }

    &PrintTailer;

    if ($checkTotal != $bigTotal) {
	&error("Difference observed between two sums: $checkTotal <=> $bigTotal");
    }
    if ($weekTaskTotal != $bigTotal) {
	&error("Difference observed between two other sums: $weekTaskTotal <=> $bigTotal");
    }
}


#
# Role :	Report the time for each task and for the month
#
# Usage:	&MonthReport(<month>, <year>)
#
sub MonthReport {
    local($month, $year) = @_;
    local($tsk);

    &message("Processing $month/$year\n");
    $Indent = "  ";

	#
	# Read in the times for the week
	#
    &ReadCra($month, $year);

    &message("    Computing...");

	#
	# For each task, compute the total and append the result in
	# the `totalTable' associative table.
	#
    $month ++;
    foreach $tsk (@tskTable) {
	local(@times) = split(/ /, $timeTable{$tsk});
	local($total) = 0;
	local($t);

	&message("o");
	foreach $t (@times) {
	    $total += $t;
	}

	$totalTable{$tsk} .= " $month $total";
    }
    &message("\n");
}

#
# Role :	Report the time for each task and for the month
#
# Usage:	&MonthReport(<month>, <year>)
#
sub PrintMonthReport {
    local($month, $year) = @_;
    local($tsk, $day, $weekDay, $roundCra, $tm, $t, $total);
    local($checkTotal) = 0;
    local($roundedTotal) = 0;
    local($bigTotal, $name, $weekTotalTime, $weekTaskTotal, $mon);
    local(@monTotal,@monTotalExact);

    $bigTotal = 0;

    &PrintHeader($month,0);
    &MethodCall("PrintTaskName", "(\"Task\")");
    for ($mon = 1; $mon <= $month; $mon ++) {
	$monTotal[$mon] = 0;
	$monTotalExact[$mon] = 0;
	&PrintElement($Month[$mon]);
    }
    &PrintElement("Total");
    &PrintNewRow;
    &PrintMonthSeparator;
    
	#
	# For each task, compute the total and print the result
	#
    foreach $tsk (keys %totalTable) {
	local(@times) = split(/ /, $totalTable{$tsk});
	local($total) = 0;
	local($round) = 0;
	local($i, $m, $mSet);

	&PrintTaskName($tsk);
	$m = 0;
	$mSet = 0;
	$i = 1;
	#printf("|%s|\n", $totalTable{$tsk});
	foreach $t (@times) {
	    if ($m == 0) {
		$m    = $t;
		$mSet = 1;
	    } else {
		while ($i < $m) {
		    &PrintDayTime(0,0);
		    $i ++;
		}
		$monTotal[$m] += $t;
		$total        += $t;
		$t	      += $round;
		&PrintDayTime($t, int($t / 3600));

		$round	      = int($t % 3600);
		$t	      = int($t / 3600);

		$monTotalExact[$m] += $t;
		$m = 0;
		$i ++;
	    }
	}
	while ($i <= $month) {
	    &PrintDayTime(0,0);
	    $i ++;
	}
	$bigTotal += $total;
	&PrintDayTime($total, int($total / 3600));

	&PrintNewRow;
    }
    &PrintMonthSeparator;
    &MethodCall("PrintTaskName", "(\"Rounded total\")");
    for ($mon = 1; $mon <= $month; $mon ++) {
	&PrintDayTime($monTotalExact[$mon] * 3600,
		      $monTotalExact[$mon]);
    }
    &PrintDayTime($bigTotal, int($bigTotal / 3600));
    &PrintNewRow;

    &MethodCall("PrintTaskName", "(\"Real Total\")");
    for ($mon = 1; $mon <= $month; $mon ++) {
	&PrintDayTime($monTotal[$mon], int($monTotal[$mon] / 3600));
    }
    &PrintDayTime($bigTotal, int($bigTotal / 3600));

    &PrintTailer;
}

#ifdef HAVE_PEOPLE
#
# Role :	Determine the name under which the cras are saved in the
#		ldbupdate database.
#
# Method:	Use the `people' Chorus tool to get the `*cra:' information
#
# Usage :	&CraName;
#
sub CraName {
    local($craName, $name);

    # 
    # Use an environment variable (the `people' output may be wrong,
    # and this is my personal case)
    # 
    $craName = $ENV{"CRA_USER"};
    if ($craName ne "") {
	return $craName;
    }

    $name = $ENV{"USER"};
    die "Environment variable USER not set"
	if ($name eq "");

    open(PIPE, "people $name |");
    while (<PIPE>) {
	chop;
	if (/cra\:/) {
	    @list = split("[ (]", $_);
	    $craName = $list[1];
	    #printf("CraName |$craName| $list[1] $list[0]\n");
	    break;
	}
    }
    close(PIPE);

    return $craName;
}
#endif

sub error {
    local($msg) = @_;

    print STDERR "$msg\n";
}
#
# Get current week
#
$curWeek = &FindCurrentWeek;
$week = $curWeek;

$printRoundedTime = 0;
$printExactTime   = 1;
$oneWeekFlag      = 0;

$PrintMethods = "PrintHeader PrintTailer PrintTaskName PrintElement PrintSummary PrintNewRow PrintWeekHeader PrintWeekTailer PrintMonthSeparator";

&MethodClass("Normal", "$PrintMethods");

$weekReportLast = 0;
$dayNum  = 0;
$yearNum = -1;
$monNum  = -1;
$queryReportFlag = 0;

#
# Parse some arguments
#
while (@ARGV) {
    $_ = shift;
    #printf("%s\n", $_);
    if (/^-v/) {
	$verbose = 1;
	$|=1;

    } elsif (/^-help$/ || /^-h$/) {
	&help;

    } elsif (/^-round$/ || /^-r$/) {
	$printRoundedTime = 1;
	$printExactTime   = 0;

    } elsif (/^-exact$/ || /^-e$/) {
	$printRoundedTime = 0;
	$printExactTime   = 1;

#ifdef HAVE_LDBUPDATE
    } elsif (/^-ldb$/ || /^-l$/) {
	$printRoundedTime = 0;
	$printExactTime   = 0;
	&MethodClass("Empty", "$PrintMethods");
	&MethodClass("Ldb", "PrintHeader PrintTailer PrintSummary");

    } elsif (/^-ldb-query$/) {
	$queryReportFlag = 1;
#endif

    } elsif (/^-oleo$/) {
	$printRoundedTime = 0;
	$printExactTime   = 0;
	&MethodClass("Empty", "$PrintMethods");
	&MethodClass("Oleo", "PrintHeader PrintTailer PrintTaskName PrintElement PrintSummary PrintNewRow");

    } elsif (/^-preserve$/ || /^-p$/) {
	$preserve = 1;

    } elsif (/^-report$/ || /^-R$/) {
	$printRoundedTime= 0;
	$printExactTime  = 0;
	$preserve    = 1;
	$oneWeekFlag     = 1;
	&MethodClass("Empty", "$PrintMethods");
	&MethodClass("Report", "PrintHeader PrintTailer PrintSummary");

    } elsif (/^-week$/ || /^-w$/) {
	$_ = shift;
	$week = int($_);

    } elsif (/^-week-last$/ || /^-w$/) {
	$_ = shift;
	$weekReportLast = int($_);

    } elsif (/^-day$/) {
	$_ = shift;
	$dayNum = int($_);

    } elsif (/^-month$/) {
	$_ = shift;
	$monthNum = int($_) - 1;

    } elsif (/^-year$/) {
	$_ = shift;
	$yearNum = int($_) - 1900;

    } elsif (/^-directory$/) {
	$_ = shift;
	$CraDirectory = $_;

    } elsif (/^-one$/) {
	$oneWeekFlag = 1;
	$preserve    = 1;

    } elsif (/^-full-month$/) {
	$fullMonth = 1;
	$preserve  = 1;

    } elsif (/^-full-year$/) {
	$fullYear = 1;
	$preserve  = 1;

    } elsif (/^-troff/) {
	&MethodClass("Troff", "$PrintMethods");

    } else {
	&help;
    }
}

#ifdef HAVE_PEOPLE
$userName = &CraName;
#else
$userName = "";
#endif

#ifdef HAVE_LDBUPDATE

if ($queryReportFlag) {
    &LdbFindTasks;
    exit 0;
}
#endif

if ($dayNum != 0) {
    #printf("%s %s %s\n", $dayNum, $monthNum, $yearNum);
    $week = &FindWeek($dayNum, $monthNum, $yearNum);
    #printf("%s\n", $week);

} else {
    if ($yearNum != -1) {
	$curYear = $yearNum;
    }
    if ($monthNum != -1) {
	$curMonth = $monthNum;
    }
}

if ($fullYear) {
    local($month) = 0;

    for ($month = 0; $month <= $curMonth; $month ++) {
	&MonthReport($month, $curYear);
    }

	#
	# Print a summary for each day of the week
	#
    &PrintMonthReport($month, $curYear);

} elsif ($fullMonth) {
    &PrintHeader(1,0);
    &MonthReport($curMonth, $curYear);

	#
	# Print a summary for each day of the week
	#
    &PrintTailer;

} elsif ($weekReportLast) {

    while ($week <= $weekReportLast) {
	&WeeklyReport($week, $curYear);
	if ($oneWeekFlag) {
	    $week ++;
	} else {
	    $week = $week + 2;
	}
    }

} else {
	#
	# Compute & print the report
	#
    &WeeklyReport($week, $curYear);
    $week = int(($week + 1) / 2);
    &WriteReportTimes($week, $curYear)
	if ($preserve == 0 && $printExactTime == 0);
}
