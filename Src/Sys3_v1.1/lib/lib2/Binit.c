static char	sccsid[] = "@(#)Binit.c	4.2";

# include <stand.h>

static char	rk[] = "/dev/rk0";
static char	rp[] = "/dev/rp0";
static char	mt[] = "/dev/mt0";

_init ()
{

	if (MKNOD (rk, 1, 0, (daddr_t) 0) < 0)
		perror (rk);
	if (mount (rk, "") < 0)
		perror (rk);

	rk[7] = '1';
	if (MKNOD (rk, 1, 1, (daddr_t) 0) < 0)
		perror (rk);
	if (mount (rk, "/rk1") < 0)
		perror (rk);

	if (MKNOD (rp, 0, 0, (daddr_t) 0) < 0)
		perror (rp);
	if (mount (rp, "/rp0") < 0)
		perror (rp);

	rp[7] = '1';
	if (MKNOD (rp, 0, 0, (daddr_t) 10000) < 0)
		perror (rp);
	if (mount (rp, "/rp1") < 0)
		perror (rp);

	if (MKNOD (mt, 2, 0, (daddr_t) 0) < 0)
		perror (mt);

	mt[7] = '4';
	if (MKNOD (mt, 2, 4, (daddr_t) 0) < 0)
		perror (mt);
}
