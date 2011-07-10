#!/usr/bin/perl
# ######################################################################
#
# Component :	help.perl
#
# Synopsis  :	XCra Help Builder
#
# Copyright (C) 1995, 1996 Stephane Carrez
#
# This file is part of Xcra.
#
# Xcra is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
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
eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
        if $running_under_some_shell;

$remove=0;
while (<>) {
    chop;
    if (/^#ifdef HAVE_XTOOLS[ 	]*$/) {
	$remove=1;
    } elsif ($remove != 0 && /^#endif/) {
	$remove=0;
    } elsif ($remove == 1 && /^#else/) {
	$remove=2;
    } elsif ($remove != 1) {
	print("$_\n");
    }
}
