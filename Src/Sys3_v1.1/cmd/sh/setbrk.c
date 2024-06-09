#
/*
 *	UNIX shell
 *
 *	S. R. Bourne
 *	Bell Telephone Laboratories
 *
 */

#include	"defs.h"

setbrk(incr)
{
	REG BYTPTR	a=(STRING)(sbrk(incr));
	brkend=a+incr;
	return((INT)(a));
}
