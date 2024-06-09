# include	"trek.h"

/**
 **	cause supernova to occur
 **/

snova(x, y)
int	x, y;
{
	int			qx, qy;
	register int		ix, iy;
	int			f;
	int			dx, dy;
	int			n;
	register QUAD		*q;

	f = 0;
	ix = x;
	if (ix < 0)
	{
		/* choose a quadrant */
		while (1)
		{
			qx = ranf(NQUADS);
			qy = ranf(NQUADS);
			q = &Quad[qx][qy];
			if (q->stars > 0)
				break;
		}
		if (Quadx == qx && Quady == qy)
		{
			/* select a particular star */
			n = ranf(q->stars);
			for (ix = 0; ix < NSECTS; ix++)
			{
				for (iy = 0; iy < NSECTS; iy++)
					if (Sect[ix][iy] == things[STAR] || Sect[ix][iy] == things[INHABIT])
						if ((n =- 1) <= 0)
							break;
				if (n <= 0)
					break;
			}
			f = 1;
		}
	}
	else
	{
		/* current quadrant */
		iy = y;
		qx = Quadx;
		qy = Quady;
		q = &Quad[qx][qy];
		f = 1;
	}
	if (f)
	{
		/* supernova is in same quadrant as Enterprise */
		printf(" *** RED ALERT: supernova occuring at %d,%d", ix, iy);
		dx = ix - Sectx;
		dy = iy - Secty;
		if (dx * dx + dy * dy <= 2)
		{
			printf(" *** Emergency override attempt");
			sleep(4);
			printf("\n");
			lose(L_SNOVA);
		}
		printf("\n");
		q->scanned = 1000;
		kills(ix, iy, x >= 0 ? -1 : 1);
		Nkling = 0;
	}
	else
	{
		if (!Damage[SSRADIO])
		{
			q->scanned = 1000;
			printf("Uhura: Starfleet Command reports a supernova in quadrant %d,%d\n", qx, qy);
		}
		kills(qx, qy, 0);
	}

	/* clear out the supernova'ed quadrant */
	dx = q->qkling;
	dy = q->stars;
	Status.kling =- dx;
	if (x >= 0)
	{
		/* Enterprise caused supernova */
		Game.kills =+ dy;
		if (q->bases) {
			killb(qx, qy); Game.killb++;
		}
		Game.gkillk =+ dx;
	}
	else
		if (q->bases) {
			killb(qx, qy);
		}
	q->stars = -1;
	q->qkling = 0;
	if (Status.kling <= 0)
	{
		printf("Lucky devil, that supernova destroyed the last klingon\n");
		win();
	}
	return;
}
