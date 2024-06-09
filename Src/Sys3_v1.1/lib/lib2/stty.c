static char	sccsid[] = "@(#)stty.c	4.1";

# include "saio.h"
# include <sgtty.h>

stty(fildes, arg)
struct sgttyb *arg; {
	extern struct sgttyb _ttstat;

	if (fildes < 0 || fildes > 2) {
		errno = ENOTTY;
		return (-1);
	}

	strncpy((char *) &_ttstat, (char *) arg, sizeof _ttstat);
	return (0);
}
