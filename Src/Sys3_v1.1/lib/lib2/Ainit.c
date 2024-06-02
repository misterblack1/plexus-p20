static char	sccsid[] = "%W%";

# include "saio.h"

static char	rp[] = "/dev/rp0";
static char	mt[] = "/dev/mt0";

_init ()
{

	if (MKNOD (rp, 0, 0, (daddr_t) 0) < 0)
		perror (rp);
	if (mount (rp, "") < 0)
		perror (rp);

	rp[7] = '1';
	if (MKNOD (rp, 0, 1, (daddr_t) 18392) < 0)
		perror (rp);
	if (mount (rp, "/usr") < 0)
		perror (rp);

	if (MKNOD (mt, 1, 0, (daddr_t) 0) < 0)
		perror (mt);

	mt[7] = '4';
	if (MKNOD (mt, 1, 4, (daddr_t) 0) < 0)
		perror (mt);
}
