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
#include "config.h"
#include "DirList.h"

#if HAVE_STDLIB_H
#  include <stdlib.h>
#endif

#if HAVE_STRING_H
#  include <string.h>
#endif

#if HAVE_DIRENT_H
#  include <dirent.h>
#  define NAMELEN(dirent) strlen((dirent)->d_name)
#else
#  define dirent direct
#  define NAMELEN(dirent) ((dirent)->d_namlen)
#  if HAVE_SYS_NDIR_H
#    include <sys/ndir.h>
#  endif
#  if HAVE_SYS_DIR_H
#    include <sys/dir.h>
#  endif
#  if HAVE_NDIR_H
#    include <ndir.h>
#  endif
#endif

#include <assert.h>
#include <stdio.h>

#ifdef __STDC__
# define F0()			(void)
# define F1(T1,P1)		(T1 P1)
# define F2(T1,P1,T2,P2)	(T1 P1, T2 P2)
# define F3(T1,P1,T2,P2,T3,P3)	(T1 P1, T2 P2, T3 P3)
# define F4(T1,P1,T2,P2,T3,P3,T4,P4) (T1 P1, T2 P2, T3 P3, T4 P4)
# define F2V(T1,P1,T2,P2)	(T1 P1, T2 P2, ...)
# ifndef __P
#   define __P(x)			x
# endif
#else
# define F0()			()
# define F1(T1,P1)		(P1) T1 P1;
# define F2(T1,P1,T2,P2)	(P1,P2) T1 P1; T2 P2;
# define F3(T1,P1,T2,P2,T3,P3)	(P1,P2,P3) T1 P1; T2 P2; T3 P3;
# define F4(T1,P1,T2,P2,T3,P3,T4,P4) (P1,P2,P3,P4) T1 P1; T2 P2; T3 P3; T4 P4;
# define F2V(T1,P1,T2,P2)	(P1,P2,va_alist) T1 P1; T2 P2; va_dcl
# ifndef __P
#   define __P(x)			()
# endif
# ifndef const
#  define const
# endif
#endif

#if HAVE_ALLOCA_H
#  include <alloca.h>
#endif

#ifdef _GNUC_
#  define alloca	__builtin_alloca
#endif

typedef struct strList {
  struct strList	*next;
  char*			oldAddr;
  int			size;
  char			buf[1];
} strList_t;

typedef struct block {
  strList_t	*strBuf;
  int		strSize;
  int		strPos;
  int		totSize;
  int		bsize;
} Block_t;

#define	MAX_STRBUF_SIZE		(10240)
#define	MIN_STRBUF_SIZE		(sizeof(struct stat))
#define	ROUND_SIZE(SZ)		((((SZ)+sizeof(struct stat)-1)		\
				 /sizeof(struct stat))*sizeof(struct stat))

#define	RATE_TO_SIZE(RT)	ROUND_SIZE((MAX_STRBUF_SIZE * (RT)) / 100)

static const int BlockSizeTable[NR_DL_LISTS] = {
  RATE_TO_SIZE(80),	/* 80% of regular files	*/
  RATE_TO_SIZE(15),	/* 15% of directories	*/
  RATE_TO_SIZE(5)	/* 5% of specials	*/
};

#define	InitBlock(CTLBLOCK,BSIZE)					\
  		do {							\
		   (CTLBLOCK)->strBuf = 0;				\
		   (CTLBLOCK)->strSize = 0;				\
		   (CTLBLOCK)->strPos = 0;				\
		   (CTLBLOCK)->totSize = 0;				\
		   (CTLBLOCK)->bsize = (BSIZE);				\
		} while (0)


/*
** Role :	Shrink the memory block if not all of its memory is used.
** 		This operation may move the block. We keep track of
**		the previous block address to be able to adjust the pointers
**		that refered to this block (see `FixPointers').
*/
static void
ShrinkBlock F1(Block_t*, sBuf)
{
    if (sBuf->strBuf != 0 && sBuf->strPos < MAX_STRBUF_SIZE) {
        strList_t* nBuf;
        
	nBuf = (strList_t*) realloc (sBuf->strBuf, sBuf->strPos
						+ sizeof(strList_t));

	if (nBuf) {
	    sBuf->strBuf = nBuf;
	}
    }
}


/*
** Role :	Destroy the chain of memory blocks holding strings.
*/
static	void
DestroyBuffer F1(strList_t*, hd)
{
    while (hd != 0) {
	strList_t *hn = hd->next;

	free (hd);
	hd = hn;
    }
}


/*
** Role :	Given the list of buffers `sBuf', builds a single area
**		that will contain all the information.
**
*/
static	char*
FixBuffer F1(Block_t*, sBuf)
{
    char	*p;
    strList_t	*sb;
    char	*s;

    if (sBuf->totSize == 0) {
	return 0;
    }

    p = (char *)malloc (sBuf->totSize);
    if (p == 0) {
	DestroyBuffer (sBuf->strBuf);
	return p;
    }

    s = &p[sBuf->totSize];
    for (sb = sBuf->strBuf; sb; sb = sb->next) {
        s -= sb->size;
	memcpy (s, &sb->buf[0], sb->size);
	sBuf->totSize -= sb->size;
    }
    assert (sBuf->totSize == 0);

    DestroyBuffer (sBuf->strBuf);

    return p;
}


/*
** Role :	Adjust the pointers in the list `list' if this is
**		necessary. The list `list' is assumed to have pointers
**		within the buffer list `sBuf'.
*/
static void
FixPointers F3(char**, list, int, count, Block_t*, sBuf)
{
    strList_t	*sb;

    sb = sBuf->strBuf;
    
    while (--count >= 0) {
	/*
	 * If the string pointer does not refer to something in the block,
	 * we can move to the next block (strong assumsion on how the
	 * memory is allocated in the blocks).
	 */
	if (list[count] < sb->oldAddr
	    || list[count] > &sb->oldAddr[sb->size]) {
	    sb = sb->next;
	}
	list[count] += (size_t)(sb->buf - sb->oldAddr);
    }
}


/*
** Role :	Build a single table to store all the information.
*/
static int
FixList F4(struct _dir_list_*, list, Block_t*, names,
	   Block_t*, stInfo, int, count)
{
    int	res = 0;

    list->count = count;

    if (names->totSize) {
	list->names = (char **) FixBuffer (names);
	if (list->names == 0) {
	    res = -1;
	}
    }

    if (stInfo->totSize) {
	list->u_stat.u_statInfo = (struct stat *) FixBuffer (stInfo);
	if (list->u_stat.u_statInfo == 0) {
	    res = -1;
	}
    }

    return res;
}


/*
** Role :	Save the region pointed to by `str' and holding `len'
**		bytes in the block list pointed to by `sBuf'. The
**		region is saved in a contiguous memory area.
*/
static	char*
SaveRegion F3(Block_t*, sBuf, const void*, str, int, len)
{
    char	*p;

	/*
	** The current block is not large enough. Get another one.
	*/
    if (sBuf->strSize < len) {
	strList_t	*nlist;

	ShrinkBlock (sBuf);

	while (1) {
	    /*
	     * Be sure the block will be big enough to hold new data.
	     */
	    if (sBuf->bsize < len) {
		sBuf->bsize += len;
	    }

	    nlist = (strList_t *) malloc (sBuf->bsize + sizeof(strList_t));
	    if (nlist != 0) {
		break;
	    }
     
	    /*
	     * If there is not enough memory, try another block size
	     * We stop when the block size becomes too small.
	     */

	    if (sBuf->bsize <= MIN_STRBUF_SIZE) {
		return 0;
	    }

	    sBuf->bsize -= (sBuf->bsize >> 2) + MIN_STRBUF_SIZE;
	    if (sBuf->bsize < len) {
		return 0;
	    }
	}

	nlist->next  = sBuf->strBuf;
	nlist->size  = 0;
	nlist->oldAddr = nlist->buf;
	sBuf->strBuf = nlist;
	sBuf->strSize= sBuf->bsize;
	sBuf->strPos = 0;
    }

    /*
     * Save the region and update the position pointer.
     */
    p = &sBuf->strBuf->buf[ sBuf->strPos ];
    memcpy (p, str, len);
    sBuf->strPos += len;
    sBuf->totSize += len;
    sBuf->strSize -= len;
    sBuf->strBuf->size += len;
    return p;
}



/*
** Role :	Gather the stat information for each file and
**		build the 3 lists of files.
**
*/
static	int
GetStatInformation F3(DirList_t*, dl, char**, files, int, nTotal)
{
    /*
     * Set of memory blocks to build to different lists.
     */
    Block_t	namesList[NR_DL_LISTS];
    Block_t	stList[NR_DL_LISTS];
    int		count[NR_DL_LISTS];

    struct stat	st;
    FileStat_t	pst;
    void	*stInfo;
    int		stInfoSize;

    int		plen;
    char	*pathBuf;
    char	*file;

    int		i;

    int		err = 0;	/* Count # of errors found (memory pb)	*/

    for (i = 0; i < NR_DL_LISTS; i++) {
	InitBlock (&namesList[i], BlockSizeTable[i]);
	InitBlock (&stList[i], BlockSizeTable[i]);
	count[i] = 0;
    }

	/*
	** Sort the list of files.
	*/
    if ((dl->mode & DL_SORT) && (nTotal > 1)) {
	qsort (files, nTotal, sizeof (char *), dl->compare);
    }

	/*
	** Get and initialize buffer which holds the complete path.
	*/
    plen = strlen (dl->path);
    pathBuf = alloca (plen + 2 + dl->maxLen);

    memcpy (pathBuf, dl->path, plen);
    file = &pathBuf[ plen ];
    *file++ = '/';


	/*
	** Setup pointer to local stat info (packed/normal/none).
	*/
    if (dl->mode & DL_PACKED_INFO) {
	stInfo = &pst;
	stInfoSize = sizeof (pst);
    } else if (dl->mode & DL_STAT_INFO) {
	stInfo = &st;
	stInfoSize = sizeof (st);
    } else {
	stInfo = 0;
	stInfoSize = 0;
    }

	/*
	** Loop on the files:
	**	1) Build the complete path of that file,
	**	2) Get stat information on it,
	**	3) Select the appropriate list and save its name and
	**	   its stat information
	*/
    for (i = 0; i < nTotal; i++) {

	char *nfile = files[i];
	int res;
	int which;

	/*
	** Build path and get stat info.
	*/
	strcpy (file, nfile);

	res = stat (pathBuf, &st);
	if (res < 0)
	  continue;	/* Assume not an error but file does not exist */

	/*
	** Build the packed information.
	*/
	if (dl->mode & DL_PACKED_INFO) {
	    pst.size = st.st_size;
	    pst.mtime= st.st_mtime;
	    pst.mode = st.st_mode;
	    pst.uid  = st.st_uid;
	    pst.gid  = st.st_gid;
	}

	/*
	** Select the appropriate list according to file type and spread mode.
	*/
	if (S_ISDIR(st.st_mode) && (dl->mode & DL_ISOLATE_DIRECTORY)) {
	    which = DL_DIRS;
	} else if (!S_ISREG(st.st_mode) && (dl->mode & DL_ISOLATE_SPECIAL)) {
	    which = DL_SPECS;
	} else {
	    which = DL_FILES;
	}

	/*
	** Save the name.
	*/
	nfile = SaveRegion (&namesList[which], &nfile, sizeof (char *));
	if (nfile && stInfo) {
	    nfile = SaveRegion (&stList[which], stInfo, stInfoSize);
	}

	/*
	** Check result of `SaveRegion'.
	*/
	if (nfile == 0) {
	    err++;
	    break;
	}

	count[which] ++;
    }

    for (i = 0; i < NR_DL_LISTS; i++) {
	/*
	** Terminate the table by a Null (may be useful for some kinds of
	** programs).
	*/
	if (count[i] > 0) {
	    char *nfile = 0;
	    if (SaveRegion (&namesList[i], &nfile, sizeof (char *)) == 0) {
		err++;
	    }
	}
	if (FixList (&dl->list[i], &namesList[i], &stList[i], count[i]) != 0) {
	    err++;
	}
    }

    return err;
}


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
DirList_t*
CreateDirectoryList F3(const char*, _path, cmpfile_t, _cmp, int, _mode)
{
    DIR*		dirp;
    struct dirent*	dp;
    struct stat		st;
    int			res;
    Block_t		strings;
    Block_t		lists;
    DirList_t*		dl;
    int			nrFiles;
    int			maxLen;
    char**		list;
    long		sz;

	/*
	** Get information on `_path' (mode & date are needed).
	*/
    res = stat (_path, &st);
    if (res < 0) {
	return 0;
    }

    if (!S_ISDIR(st.st_mode)) {
	return 0;
    }

    dl = (DirList_t *) calloc (1, sizeof (DirList_t));
    if (dl == 0) {
	return 0;
    }

    dl->path = strdup (_path);
    if (dl->path == 0) {
	free (dl);
	return 0;
    }

    dl->mode    = _mode;
    dl->compare = _cmp;
    dl->dirStat = st;

    dirp = opendir (_path);
    if (dirp == 0) {
	DestroyDirectoryList (dl);
	return 0;
    }

	/*
	** Initialize the buffer holding strings.
	*/
    sz = st.st_size;
    if (sz < MIN_STRBUF_SIZE)
      sz = MIN_STRBUF_SIZE;

    InitBlock (&strings, sz);
    InitBlock (&lists, sz/4);

    nrFiles = 0;
    maxLen  = 0;
    while ((dp = readdir (dirp)) != 0) {
	char *nfile;
	int len = NAMELEN(dp);

	/*
	** Save the file name or abort.
	*/
	nfile = SaveRegion (&strings, dp->d_name, len + 1);
	if (nfile == 0)
	  break;

	/*
	** Save the pointer or abort.
	*/
	nfile = SaveRegion (&lists, &nfile, sizeof (char *));
	if (nfile == 0)
	  break;

	if (maxLen < len)
	  maxLen = len;

	nrFiles ++;
    }
    closedir (dirp);

    dl->strings = strings.strBuf;
    dl->maxLen  = maxLen;

	/*
	** Check for an error (in that case it's a memory allocation pb).
	*/
    if (dp != 0) {
	DestroyBuffer (lists.strBuf);
	DestroyDirectoryList (dl);
	return 0;
    }
    ShrinkBlock (&strings);
    ShrinkBlock (&lists);

        /*
        ** The string block might have moved after ShrinkBlock().
        */
    dl->strings = strings.strBuf;

    list = (char **) FixBuffer (&lists);
    if (list == 0) {
	DestroyDirectoryList (dl);
	return 0;
    }

    FixPointers (list, nrFiles, &strings);

    if (GetStatInformation (dl, list, nrFiles) != 0) {
	free (list);
	DestroyDirectoryList (dl);
	return 0;
    }
    free (list);

    return dl;
}


/*
** Role :	Free all the list of files of `dl' (DL_FILES,DL_DIRS,DL_SPECS)
*/
static	void
DestroyDirectoryLists F1(DirList_t*, dl)
{
    int	i;

    for (i = 0; i < NR_DL_LISTS; i++) {
	if (dl->list[i].names) {
	    free (dl->list[i].names);
	    dl->list[i].names = 0;
	}

	if (dl->list[i].u_stat.u_statInfo) {
	    free (dl->list[i].u_stat.u_statInfo);
	    dl->list[i].u_stat.u_statInfo = 0;
	}
    }
}


/*
** Role :	Delete the lists returned by `CreateDirectoryList' thus
**		freeing the memory allocated for them.
*/
void
DestroyDirectoryList F1(DirList_t*, _dl)
{
    DestroyBuffer (_dl->strings);
    DestroyDirectoryLists (_dl);
    if (_dl->path) {
	free (_dl->path);
    }
    free (_dl);
}


#ifdef TEST
#include <stdio.h>

int
NameCompare F2(const char**, s1, const char**, s2)
{
    return strcmp( *s1, *s2 );
}

void
PrintDirectoryList F1(DirList_t*, dl)
{
    int	i;

#define packedStatInfo	u_stat.u_packedStatInfo
#define statInfo	u_stat.u_statInfo

    printf ("Listing %s\n", dl->path);
    for (i = 0; i < NR_DL_LISTS; i++) {
	int j;

	printf ("List %d:\n", i);
	for (j = 0; j < dl->list[i].count; j++) {
	    printf ("%-30.30s", dl->list[i].names[j]);
	    if (dl->mode & DL_PACKED_INFO) {
		printf (" %x %ld (%d.%d) %ld\n",
			dl->list[i].packedStatInfo[j].mode,
			dl->list[i].packedStatInfo[j].size,
			dl->list[i].packedStatInfo[j].uid,
			dl->list[i].packedStatInfo[j].gid,
			dl->list[i].packedStatInfo[j].mtime);
	    } else if (dl->mode & DL_STAT_INFO) {
		printf (" %x %ld (%d.%d) %ld I%ld\n",
			dl->list[i].statInfo[j].st_mode,
			dl->list[i].statInfo[j].st_size,
			dl->list[i].statInfo[j].st_uid,
			dl->list[i].statInfo[j].st_gid,
			dl->list[i].statInfo[j].st_mtime,
			dl->list[i].statInfo[j].st_ino);
	    } else {
		printf ("\n");
	    }
	}
    }
}

int
main F2(int, argc, char**, argv)
{
    DirList_t*	dl;
    int		mode = 0;
    int		i;
    cmpfile_t	compare = 0;
    int		c;

    extern int	optind;

	/*
	** Check argument and build scanning mode.
	*/
    while ((c = getopt (argc,argv,"siSp")) != EOF) {
	switch(c){
	  case 's':
	    compare = NameCompare;
	    mode |= DL_SORT;
	    break;
	  case 'i':
	    mode |= DL_ISOLATE_DIRECTORY | DL_ISOLATE_SPECIAL;
	    break;
	  case 'p':
	    mode |= DL_PACKED_INFO;
	    break;
	  case 'S':
	    mode |= DL_STAT_INFO;
	    break;
	  default :
	    fprintf (stderr, "Usage: %s [-si] dir...\n", argv[0]);
	    printf (" -s  Sort the files\n");
	    printf (" -i  Isolate directories/regular/others\n");
	    printf (" -p  Build packed information table\n");
	    printf (" -S  Build stat information table\n");
	    exit (1);
	}
    }

    printf ("Mode = %d\n", mode);
    for (i = optind; i < argc; i++) {
	dl = CreateDirectoryList (argv[i], compare, mode);
	if (dl == 0) {
	    fprintf (stderr,"Virtual memory exhausted or problem reading %s\n",
		     argv[i]);
	} else {
	    PrintDirectoryList (dl);
	    DestroyDirectoryList (dl);
	}
    }
    return 0;
}
#endif
