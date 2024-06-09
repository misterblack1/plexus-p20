#ifndef SRBHDR
#include	"defs.h"
#endif


STRUCT fileblk	FILEBLK;
STRUCT filehdr	FILEHDR;
STRUCT fileblk	*FILE;
#define NIL	0

/* the following nonsense is required
 * because casts turn an Lvalue
 * into an Rvalue so two cheats
 * are necessary, one for each context.
 */
union { int _cheat;};
#define Lcheat(a)	((a)._cheat)
#define Rcheat(a)	((int)(a))

#define	BUFSIZ	64
struct fileblk {
	UFD	fdes;
	POS	flin;
	BOOL	feof;
	CHAR	fsiz;
	STRING	fnxt;
	STRING	fend;
	FILE	fstak;
	CHAR	fbuf[BUFSIZ];
};

/* for files not used with file descriptors */
struct filehdr {
	UFD	fdes;
	POS	flin;
	BOOL	feof;
	CHAR	fsiz;
	STRING	fnxt;
	STRING	fend;
	FILE	fstak;
	CHAR	_fbuf[1];
};

#define EOF	(-1)
INT	eof;
