static char	sccsid[] = "@(#)tell.c	4.1";

# include "saio.h"

off_t
tell(fildes)
{
	extern off_t lseek();

	return (lseek(fildes, (off_t) 0, 1));
}
