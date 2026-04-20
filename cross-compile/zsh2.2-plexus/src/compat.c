/*
 * compat.c - SVR2 compatibility functions for zsh
 * Provides opendir/readdir/closedir and other missing functions
 * for AT&T UNIX System V Release 2 on Plexus P/20
 */

#include "zsh.h"

#ifdef PLEXUS_SVR2

/* opendir/readdir/closedir implementation for SVR2
 * Uses struct direct from sys/dir.h (16-byte fixed entries)
 */

static struct direct _de;

DIR *opendir(name)
char *name;
{
	int fd;
	DIR *dirp;
	char *malloc();

	fd = open(name, 0);
	if (fd < 0)
		return (DIR *)0;
	dirp = (DIR *)malloc(sizeof(DIR));
	if (!dirp) {
		close(fd);
		return (DIR *)0;
	}
	dirp->dd_fd = fd;
	dirp->dd_loc = 0;
	dirp->dd_size = 0;
	return dirp;
}

struct direct *readdir(dirp)
DIR *dirp;
{
	struct direct dbuf;
	int n;

	for (;;) {
		n = read(dirp->dd_fd, (char *)&dbuf, sizeof(dbuf));
		if (n <= 0)
			return (struct direct *)0;
		if (dbuf.d_ino == 0)
			continue;
		_de.d_ino = dbuf.d_ino;
		strncpy(_de.d_name, dbuf.d_name, DIRSIZ);
		return &_de;
	}
}

void closedir(dirp)
DIR *dirp;
{
	close(dirp->dd_fd);
	free((char *)dirp);
}

void rewinddir(dirp)
DIR *dirp;
{
	lseek(dirp->dd_fd, 0L, 0);
}

int dup2(old, new)
int old, new;
{
	close(new);
	return fcntl(old, 0, new); /* F_DUPFD */
}

#endif /* PLEXUS_SVR2 */
