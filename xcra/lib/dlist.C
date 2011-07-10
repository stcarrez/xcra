// -*- C++ -*-
////////////////////////////////////////////////////////////////////////
// Title	: Doubled linked list management
// Author	: S. Carrez
// Date		: Fri Dec 27 15:23:17 1991
// Version	: $Id: dlist.C,v 1.7 1996/08/04 15:22:58 gdraw Exp $
// Project	: xcra
// Keywords	: abstract data type, list.
//
// Copyright (C) 1991, 1992, 1993, 1994, 1995, 1996 Stephane Carrez
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
////////////////////////////////////////////////////////////////////////
//
// Content:
//
// class dlist		Controls a doubled linked list.
//
#include "config.H"
#include "Assert.H"
#include "dlist.H"


//
// Move the slot referenced by `_slot' at beginning of the list.
//
    void
dlist::raise(dlink* _slot)
{
    require(is_member(_slot));

    if (_slot != d_head) {

		// Builtin remove
	if (_slot == d_tail) {
	    d_tail = _slot->p_prev;
	    if (_slot->p_next == 0) {
		_slot->p_prev->p_next = 0;
	    } else {
		_slot->remove_ring();
	    }
	} else {
	    _slot->remove_ring();
	}

    		// Builtin insert
	if (d_head->p_prev == 0) {
	    d_head = _slot->insert_head(d_head);
	} else {
	    d_head->p_prev->insert(_slot);
	    d_head = _slot;
	}
    }

    ensure(_slot == first());
}


//
// Move the slot referenced by `_slot' at end of the list.
//
    void
dlist::unraise(dlink* _slot)
{
    require(is_member(_slot));

    if (_slot != d_tail) {

		// Builtin remove
	if (_slot == d_head) {
	    d_head = _slot->p_next;
	    if (_slot->p_prev == 0) {
		_slot->p_next->p_prev = 0;
	    } else {
		_slot->remove_ring();
	    }
	} else {
	    _slot->remove_ring();
	}	

		// Builtin insert
	if (d_tail->p_next == 0) {
	    d_tail = _slot->insert_tail(d_tail);
	} else {
	    d_tail->insert(_slot);
	    d_tail = _slot;
	}
    }

    ensure(_slot == last());
}


//
// Role :	
//
// Results :	
//
dlist::dlist(dlink* _a_slot)
{
    if (_a_slot == 0) {
	d_head = 0;
	d_tail = 0;
    } else {
	d_head = _a_slot->first();
	d_tail = _a_slot->last();
    }

    ensure(_a_slot == 0 || is_member(_a_slot));
}


//
// Insert slot pointed to by `_new' after slot pointed to by
// `_prev'. When `_prev' is null, insert at beginning of the list.
//
    void
dlist::insert(dlink* _new, dlink* _prev)
{
    require(_prev == 0 || is_member(_prev));
    require(!is_member(_new));

    if (_prev == 0) {
		//
		// Insert first slot of the list
		//
	if ((_prev = d_head) == 0) {
	    d_head = _new;
	    d_tail = _new;
	    _new->p_prev = 0;
	    _new->p_next = 0;

		//
		// Insert in a non-ring list
		//
	} else if (_prev->p_prev == 0) {
	    _prev->p_prev = _new;
	    _new->p_prev = 0;
	    _new->p_next = _prev;
	    d_head = _new;

     		//
     		// Insert in a ring-list
     		//
	} else {
	    _prev->p_prev->insert(_new);
	    d_head = _new;
	}
    } else if (_prev == d_tail) {
	if (d_tail->p_next == 0) {
	    d_tail = _new->insert_tail(_prev);
	} else {
	    d_tail->insert(_new);
	    d_tail = _new;
	}
    } else {
	_prev->insert(_new);
    }

    ensure(is_member(_new));
}


//
// Remove the slot pointed to by `_slot' and update head and tail
// references.
//
    void
dlist::remove(dlink* _slot)
{
    require(is_member(_slot));

    if (d_head == _slot) {
	d_head = _slot->next();
    }

    if (d_tail == _slot) {
	d_tail = _slot->prev();
    }

    _slot->remove();

    ensure(!is_member(_slot));
}


//
// Return true if the `_slot' is a member of this list.
//
     bool
dlist::is_member(const dlink* _slot) const
{
    const dlink* l;
    const dlink* t;

    l = d_head;
    if (l != 0) {
	t = d_tail;
	while (1) {
	    require(l != 0);

	    if (l == _slot) return true;
	    if (l == t) break;
	    l = l->next();
	}
    }

    return false;
}


//
// Return # of slots in the list.
//
    long
dlist::size() const
{
    const dlink* l;
    const dlink* t;
    long cnt = 0;

    l = d_head;
    if (l != 0) {
	t = d_tail;
	cnt++;
	while (l != t) {
	    cnt++;

	    require(l != 0);

	    l = l->next();
	}
    }
    return cnt;
}


//
// Return the slot at position `_n' in the list.
//
    dlink*
dlist::pos(unsigned _n) const
{
    const dlink* l;
    const dlink* t;

    l = d_head;
    if (l != 0) {
	t = d_tail;
	_n++;
	while (--_n != 0 && l != t) {
	    require(l != 0);

	    l = l->next();
	}

	if (_n) l = 0;
    }

    ensure(l == 0 || is_member(l));
    return (dlink*) l;
}

