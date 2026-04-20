/*
 * Copyright (C) 1989 by Kenneth Almquist.  All rights reserved.
 * This file is part of ash, which is distributed under the terms specified
 * by the Ash General Public License.  See the file named LICENSE.
 */

/* flags passed to redirect */
#define REDIR_PUSH 01		/* save previous values of file descriptors */
#define REDIR_BACKQ 02		/* save the command output in memory */

#ifdef __STDC__
void redirect(union node *, int);
void popredir(void);
void clearredir(void);
int copyfd(int, int);
#else
void redirect();
void popredir();
void clearredir();
int copyfd();
#endif
