/* Controls a directory list structure
** Copyright (C) 1993, 1994, 1995, 1996 Stephane Carrez
**
** This file is part of Xcra.
**
** Xcra is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** Xcra is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
*/

#ifndef _DIRECTORY_LIST_H
#define	_DIRECTORY_LIST_H

#include <sys/types.h>

#if TIME_WITH_SYS_TIME
#  include <sys/time.h>
#  include <time.h>
#else
#  if HAVE_SYS_TIME_H
#    include <sys/time.h>
#  else
#    include <time.h>
#  endif
#endif

#include <sys/stat.h>

#ifndef __P
#ifndef HAVE_PROTOTYPE
#if defined(__cplusplus) || (__STDC__ == 1)
#define	HAVE_PROTOTYPE
#endif
#endif
#ifdef HAVE_PROTOTYPE
#define	__P(X)	X
#define __Pdefined
#else
#define	__P(X)	()
#define __Pdefined
#endif
#endif

/*
 * Indexes for the different lists.
 */
#define	DL_FILES		0	/* Regular files		   */
#define	DL_DIRS			1	/* Directories			   */
#define	DL_SPECS		2	/* Special files		   */
#define	NR_DL_LISTS		3

#define	DL_ISOLATE_DIRECTORY	0x01	/* Put directories in DL_DIRS list */
#define	DL_ISOLATE_SPECIAL	0x02	/* Put special files in DL_SPECS   */
#define	DL_ISOLATE_REGULAR	0x04	/* Put regular files in DL_FILES   */
#define	DL_SORT			0x08	/* Sort the lists		   */
#define	DL_STAT_INFO		0x40	/* Get the stat info too	   */
#define	DL_PACKED_INFO		0x80	/* Get the `FileStat_t' info	   */
#define	DL_SYMLINK_IS_REGULAR	0x100	/* Treat symbolic links as regular */

#define	DL_ISOLATE_ALL		(DL_ISOLATE_DIRECTORY | \
				 DL_ISOLATE_SPECIAL | \
				 DL_ISOLATE_REGULAR)


typedef struct {
    off_t	size;		/* Size of file			*/
    time_t	mtime;		/* Modification date		*/
    mode_t	mode;		/* Access rights + mode		*/
    uid_t	uid;		/* Owner and group of file	*/
    gid_t	gid;
} FileStat_t;

typedef int	(* cmpfile_t) __P( (const char** _name1,const char** _name2) );

struct _dir_list_ {
    char**	names;		/* List of names (null terminated)	*/
    int		count;		/* # of names in the list		*/
    union {			/* The stat/FileStat_t info or 0	*/
      struct stat* u_statInfo;
      FileStat_t*  u_packedStatInfo;
    }		u_stat;
};

typedef struct DirList {
  char*		path;		/* Path of the directory		*/
  void*		strings;
  struct stat	dirStat;	/* Stat structure of the directory	*/
  struct _dir_list_ list[NR_DL_LISTS];

  int		mode;		/* Mode used to build the lists		*/
  cmpfile_t	compare;	/* Comparison operation			*/
  int		maxLen;		/* Max length of a name			*/
} DirList_t;


#ifdef __cplusplus
extern "C" {
#endif

/*
** Role :	Scan the directory whose path is `_path' and collect
**		the different files in one or several lists, split
**		according to `_mode'. Lists may be sorted according
**		to the function `_cmp'. Up to 3 lists may be created:
**
**		- a list of regular files	(DL_ISOLATE_REGULAR)
**		- a list of sub-directories	(DL_ISOLATE_DIRECTORY)
**		- a list of special files 	(DL_ISOLATE_SPECIAL)
**
**		Lists are sorted when the flag `DL_SORT' is present.
**
**		Each list may have a `struct stat' or a packed form
**		of `struct stat' (FileStat_t) for each entry. The
**		`struct stat' entries are collected when the `DL_STAT_INFO'
**		flag is set. The `FileStat_t' entries are collected when
**		the `DL_PACKED_INFO' flag is set.
**
**		Symbolic links are treated are regular files when
**		the `DL_SYMLINK_IS_REGULAR' flag is set.
**
** Results :	The lists or 0.
*/
extern DirList_t* CreateDirectoryList __P((const char* _path,
					   cmpfile_t _cmp,
					   int _mode));

/*
** Role :	Delete the lists returned by `CreateDirectoryList' thus
**		freeing the memory allocated for them.
*/
extern void	  DestroyDirectoryList __P((DirList_t* _dlist));

#ifdef __cplusplus
}
#endif
#ifdef __Pdefined
#  undef __P
#endif

#endif
