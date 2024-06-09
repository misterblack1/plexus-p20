static char	sccsid[] = "@(#)chmod.c	4.1";

# include "saio.h"

chmod() {

	errno = EPERM;
	return (-1);
}
