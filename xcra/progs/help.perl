#!/usr/local/bin/perl
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
eval 'exec /usr/local/bin/perl -S $0 ${1+"$@"}'
        if $running_under_some_shell;

#
# This perl script just replaces occurences of `next-help' by a \0.
# (this can be done by hand with Emacs: Esc % next-help^Q^J <ret> ^Q^@ <ret>!)
#
while (<>) {
    chop;
    if (/^[ 	]*next-help[ 	]*$/) {
	print("\0");
    } else {
	print("$_\n");
    }
}
