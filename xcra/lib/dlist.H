// -*- C++ -*-
////////////////////////////////////////////////////////////////////////
// Title	: Doubled linked list management
// Author	: S. Carrez
// Date		: Fri Dec 27 15:23:58 1991
// Version	: $Id: dlist.H,v 1.6 1996/08/04 15:22:56 gdraw Exp $
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
// Content	:
//
//	This file defines the class `dlink' used to represent a doubled
// linked list. The class `dlist' controls head and tail of that list.
//
// Description	:
//
//	o Slots must be linked altogether with the class `dlink'.
//	  This class provides methods to move forward and backward as
//	  well as to insert or delete slots.
//
//	o Head and tail references of the list can be controlled by
//	  the class `dlist'. Insertion and deletion are available but
//	  there is no concept of a cursor position.
//
//
#ifndef _OBJECTS_LIST_H
#define	_OBJECTS_LIST_H

// Some systems define raise in <signal.h>. This conflicts with the
// dlist::raise() operation.
#ifdef raise
#  undef raise
#endif

// Some other systems define remove (as unlink). This also conflicts
// with several methods.
#ifdef remove
#  undef remove
#endif

// ----------------------------------------------------------------------
// Class :	dlink
//
// Role :	This class was designed to managed doubled linked list.
//
// Bugs  :	When a doubled linked list is initialized to work as
//		a ring, there is no way to tell where the beginning or
//		the end of the ring is. Therefore, the `last' and `first'
//		methods should never be used on theses lists or the program
//		may hang up.
//
class dlink {
	friend class dlist;
private:
    dlink(const dlink&);
    void operator=(const dlink&);
    
    dlink*	p_next;	// Next slot in list
    dlink*	p_prev;	// Previous slot in list
public:
	//
	// Create a list with 1 slot (this).
	//
    dlink()
			{ p_next = p_prev = (dlink*) 0;	}

	//
	// Create a list with 1 slot (this).
	//
    dlink(dlink* _nxt, dlink* _prv)
			{
			  p_next = _nxt;
			  p_prev = _prv;
			}

	//
	// Delete slot, unlink from list.
	//
    ~dlink()
			{
			  if (p_prev)
			     p_prev->p_next = p_next;
			  if (p_next)
			     p_next->p_prev = p_prev;
			}

	//
	// Return next or previous slot of the list.
	//
    dlink* next() const
			{ return p_next;		}
    dlink* prev() const
			{ return p_prev;		}

	//
	// Return first or last slot of the list.
	//
    inline dlink* first() const;
    inline dlink* last() const;

	//
	// Boolean functions to check whether:
	//	o we are at end of the list (is_last)
	//	o 	 at beginning	    (is_first)
	//	o this list contain one slot (is_alone)
	//
    bool is_last() const
			{ return p_next == 0;		}
    bool is_first() const
			{ return p_prev == 0;		}
    bool is_alone() const
			{ return p_next == 0 && p_prev == 0;	}


////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////

protected:
	//
	// Insert current slot at beginning of the list whose
	// head is pointed to by `_hd'. Return new head of
	// the list.
	//
    dlink* insert_head(dlink* _hd)
			{
			  if (_hd != (dlink*) 0) {
			     _hd->p_prev = this;
			  }
			  p_prev = (dlink*) 0;
			  p_next = _hd;
			  return this;
			}

	//
	// Insert current slot at end of the list whose
	// tail is pointed to by `_tl'. Return new tail of
	// the list.
	//
    dlink* insert_tail(dlink* _tl)
			{
			  if (_tl != (dlink*) 0) {
			     _tl->p_next = this;
			  }
			  p_next = (dlink*) 0;
			  p_prev = _tl;
			  return this;
			}

	//
	// Insert slot pointed to by `_slot' after current slot.
	//
    void insert(dlink* _nxt)
			{
			  _nxt->p_next	= p_next;
			  _nxt->p_prev	= this;
			  p_next->p_prev= _nxt;
			  p_next	= _nxt;
			}

	//
	// Remove this slot.
	//
    void remove()
			{
			   if (p_prev)
			     p_prev->p_next = p_next;
			   if (p_next)
			     p_next->p_prev = p_prev;
			   p_next = (dlink*) 0;
			   p_prev = (dlink*) 0;
			}

	//
	// Remove current slot from ring.
	//
    void remove_ring()
 			{
			  p_next->p_prev = p_prev;
			  p_prev->p_next = p_next;
			}
};


    inline dlink*
dlink::first() const
{
    const dlink* slot;

    slot = this;
    while (slot->p_prev != (dlink*) 0) {
	slot = slot->p_prev;
    }
    
    return (dlink*) slot;
}


    inline dlink*
dlink::last() const
{
    const dlink* slot;

    slot = this;
    while (slot->p_next != (dlink*) 0) {
	slot = slot->p_next;
    }

    return (dlink*) slot;
}


// ----------------------------------------------------------------------
// Class :	dlist
//
// Role :	Head and tail references of a double linked list are controlled
//		by this class. 
//
class dlist {
private:
    dlist(const dlist&);
    void operator=(const dlist&);
protected:
    dlink* d_head;
    dlink* d_tail;
public:
	dlist()
		{ d_head = 0; d_tail = 0;		}

	dlist(dlink* _a_slot);

	//
	// Insert slot pointed to by `_new' after slot `_prev'.
	// If `_prev' is null, insert at beginning of the list.
	//
    void insert(dlink* _new, dlink* _prev = 0);

	//
	// Remove slot.
	//
    void remove(dlink* _slot);

	//
	// Move slot `_slot' at beginning of the list.
	//
    void raise(dlink* _slot);

	//
	// Move slot `_slot' at end of the list.
	//
    void unraise(dlink* _slot);

	//
	// Return true if `_slot' is a member of this list.
	//
    bool is_member(const dlink* _slot) const;

	//
	// Return true if list is empty.
	//
    bool is_empty() const
		{ return d_head == 0;					}

	//
	// Return # of slots in the list.
	//
    long size() const;

	//
	// Return slot at position `_n' in the list.
	//
    dlink* pos(unsigned _n) const;

	//
	// Return first slot of the list.
	//
    dlink* first() const
		{ return d_head;				}

	//
	// Return last slot of the list.
	//
    dlink* last() const
		{ return d_tail;				}
};

#endif
