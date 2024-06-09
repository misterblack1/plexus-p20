/***
 *	setline - optimize line setting for sending or receiving files
 *
 *	return code - none
 */

#include "uucp.h"
#include <termio.h>

#define PACKSIZE	64
#define SNDFILE	'S'
#define RCVFILE 'R'
#define RESET	'X'

setline(type)
char type;
{
	struct termio tbuf, sbuf;
	int set = 0;

	DEBUG(2, "setline - %c\n", type);
	switch(type) {
	case SNDFILE:
		break;
	case RCVFILE:
		ioctl(Ifn, TCGETA, &tbuf);
		sbuf = tbuf;
		tbuf.c_cc[VMIN] = PACKSIZE;
		ioctl(Ifn, TCSETAW, &tbuf);
		set++;
		break;
	case RESET:
		if (set == 0) break;
		ioctl(Ifn, TCSETAW, &sbuf);
		break;
	}
}
