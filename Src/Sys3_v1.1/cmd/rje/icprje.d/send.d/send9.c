/*	send9.c		Trap handling code	*/

#include	"send.h"

struct	trap	*tchain;

rstrp(strp)
struct	trap	*strp;
{
	tchain = strp->str;
}

dotrp()
{
	if(tchain)
		longjmp(tchain->save);
}
