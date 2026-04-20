/*
 * System V directory routines.  The BSD ones are almost the same except
 * that the structure tag "direct" is used instead of "dirent" and opendir
 * is replaced with myopendir (which checks that the file being opened is
 * a directory).  If we don't have the BSD ones, we use our own code which
 * assumes an old style directory format.  This file requires sys/types.h.
 *
 * Copyright (C) 1989 by Kenneth Almquist.  All rights reserved.
 * This file is part of ash, which is distributed under the terms specified
 * by the Ash General Public License.  See the file named LICENSE.
 */

#if DIRENT		/* System V directory routines available */
#include <dirent.h>
#else
#ifdef BSD		/* 4.2 BSD directory routines available */
#include <sys/dir.h>
#ifdef __STDC__
DIR *myopendir(char *);
#else
DIR *myopendir();
#endif
#define dirent direct
#define opendir myopendir
#else			/* Use our own directory access routines */
#include <sys/dir.h>

struct dirent {				/* data from readdir */
      long d_ino;			/* inode number of entry */
      char d_name[DIRSIZ+1];		/* name of file */	/* non-POSIX */
};

#define DIRBUFENT 64

typedef struct {
      struct dirent dd_entry;		/* directory entry */
      int	    dd_fd;		/* file descriptor */
      int	    dd_nleft;		/* amount of valid data */
      struct direct *dd_loc;		/* location in block */
      struct direct dd_buf[DIRBUFENT];	/* -> directory block */
} DIR;					/* stream data from opendir() */

#ifdef __STDC__
DIR *opendir(char *);
struct dirent *readdir(DIR *);
int closedir(DIR *);
#else
DIR *opendir();
struct dirent *readdir();
int closedir();
#endif

#endif /* BSD */
#endif /* DIRENT */
