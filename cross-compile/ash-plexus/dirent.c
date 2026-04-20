/*
 * Copyright (C) 1989 by Kenneth Almquist.  All rights reserved.
 * This file is part of ash, which is distributed under the terms specified
 * by the Ash General Public License.  See the file named LICENSE.
 */

#include "shell.h"	/* definitions for pointer, NULL, DIRENT, and BSD */

#if ! DIRENT

#include "myerrno.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mydirent.h"

#ifndef S_ISDIR				/* macro to test for directory file */
#define	S_ISDIR(mode)		(((mode) & S_IFMT) == S_IFDIR)
#endif

#ifdef BSD

#ifdef __STDC__
int stat(char *, struct stat *);
#else
int stat();
#endif


/*
 * The BSD opendir routine doesn't check that what is being opened is a
 * directory, so we have to include the check in a wrapper routine.
 */

#undef opendir

DIR *
myopendir(dirname)
      char *dirname;			/* name of directory */
      {
      struct stat statb;

      if (stat(dirname, &statb) != 0 || ! S_ISDIR(statb.st_mode)) {
	    errno = ENOTDIR;
	    return NULL;		/* not a directory */
      }
      return opendir(dirname);
}

#else /* not BSD */

/*
 * Dirent routines for old style file systems.
 */

#ifdef __STDC__
pointer malloc(unsigned);
void free(pointer);
int open(char *, int, ...);
int close(int);
int fstat(int, struct stat *);
#else
pointer malloc();
void free();
int open();
int close();
int fstat();
#endif


DIR *
opendir(dirname)
      char		*dirname;	/* name of directory */
      {
      register DIR	*dirp;		/* -> malloc'ed storage */
      register int	fd;		/* file descriptor for read */
      struct stat	statb;		/* result of fstat() */

#ifdef O_NDELAY
      fd = open(dirname, O_RDONLY|O_NDELAY);
#else
      fd = open(dirname, O_RDONLY);
#endif
      if (fd < 0)
	    return NULL;		/* errno set by open() */

      if (fstat(fd, &statb) != 0 || !S_ISDIR(statb.st_mode)) {
	    (void)close(fd);
	    errno = ENOTDIR;
	    return NULL;		/* not a directory */
      }

      if ((dirp = (DIR *)malloc(sizeof(DIR))) == NULL) {
	    (void)close(fd);
	    errno = ENOMEM;
	    return NULL;		/* not enough memory */
      }

      dirp->dd_fd = fd;
      dirp->dd_nleft = 0;		/* refill needed */

      return dirp;
}



int
closedir(dirp)
      register DIR *dirp;		/* stream from opendir() */
      {
      register int fd;

      if (dirp == NULL) {
	    errno = EFAULT;
	    return -1;			/* invalid pointer */
      }

      fd = dirp->dd_fd;
      free((pointer)dirp);
      return close(fd);
}



struct dirent *
readdir(dirp)
      register DIR *dirp;		/* stream from opendir() */
      {
      register struct direct *dp;
      register char *p, *q;
      register int i;

      do {
	    if ((dirp->dd_nleft -= sizeof (struct direct)) < 0) {
		  if ((i = read(dirp->dd_fd,
				(char *)dirp->dd_buf,
				DIRBUFENT*sizeof(struct direct))) <= 0) {
			if (i == 0)
			      errno = 0;	/* unnecessary */
			return NULL;		/* EOF or error */
		  }
		  dirp->dd_loc = dirp->dd_buf;
		  dirp->dd_nleft = i - sizeof (struct direct);
	    }
	    dp = dirp->dd_loc++;
      } while (dp->d_ino == 0);
      dirp->dd_entry.d_ino = dp->d_ino;

      /* now copy the name, nul terminating it */
      p = dp->d_name;
      q = dirp->dd_entry.d_name;
      i = DIRSIZ;
      while (--i >= 0 && *p != '\0')
	    *q++ = *p++;
      *q = '\0';
      return &dirp->dd_entry;
}

#endif /* BSD */
#endif /* DIRENT */
