# include	"trek.h"

/**
 **	move klingons around
 **/

klmove(fl)
int	fl;
{
	int			n, i;
	register KLINGONS	*k;
	double			dx, dy;
	int			nextx, nexty;
	register int		lookx, looky;
	int			motion;
	int			fudgex, fudgey;
	int			qx, qy;
	double			bigger;

#ifdef TRACE
	if (tTf(20,0))
		printf("entered klmove fl = %d, Nkling = %d\n", fl, Nkling);
#endif
	for (n = 0; n < Nkling; k && n++)
	{
		k = &Kling[n];
		i = 100;
		if (fl)
			if(Param.klingpwr != 0.)
				i = 100.0 * k->power / Param.klingpwr;
		if (ranf(i) >= Param.moveprob[2 * Move.newquad + fl])
			continue;
		/* compute distance to move */
		motion = ranf(75) - 25;
		motion =* k->avgdist * Param.movefac[2 * Move.newquad + fl];
		/* compute direction */
		dx = Sectx - k->x + ranf(3) - 1;
		dy = Secty - k->y + ranf(3) - 1;
		bigger = dx;
		if (dy > bigger)
			bigger = dy;
		if(bigger == 0.)
			bigger = 1.;
		dx = dx / bigger + 0.5;
		dy = dy / bigger + 0.5;
		if (motion < 0)
		{
			motion = -motion;
			dx = -dx;
			dy = -dy;
		}
		fudgex = fudgey = 1;
		/* try to move the klingon */
		nextx = k->x;
		nexty = k->y;
		for (; motion > 0; motion--)
		{
			lookx = nextx + dx;
			looky = nexty + dy;
			if (lookx < 0 || lookx >= NSECTS || looky < 0 || looky >= NSECTS)
			{
				/* new quadrant */
				qx = Quadx;
				qy = Quady;
				if (lookx < 0)
					qx =- 1;
				else
					if (lookx >= NSECTS)
						qx =+ 1;
				if (looky < 0)
					qy =- 1;
				else
					if (looky >= NSECTS)
						qy =+ 1;
				if(movkling(Quadx,Quady,qx,qy)) {
					printf("Klingon at %d,%d escapes to quadrant %d,%d\n",
						k->x, k->y, qx, qy);
					Nkling =- 1;
					Sect[k->x][k->y] = things[EMPTY];
					bmove(&Kling[Nkling], k, sizeof *k);
				}
				k = 0;
				break;
			}
			if (Sect[lookx][looky] != things[EMPTY])
			{
				lookx = nextx + fudgex;
				if (lookx < 0 || lookx >= NSECTS)
					lookx = nextx + dx;
				if (Sect[lookx][looky] != things[EMPTY])
				{
					fudgex = -fudgex;
					looky = nexty + fudgey;
					if (looky < 0 || looky >= NSECTS || Sect[lookx][looky] != things[EMPTY])
					{
						fudgey = -fudgey;
						break;
					}
				}
			}
			nextx = lookx;
			nexty = looky;
		}
		if (k && (k->x != nextx || k->y != nexty))
		{
			printf("Klingon at %d,%d moves to %d,%d\n",
				k->x, k->y, nextx, nexty);
			Sect[k->x][k->y] = things[EMPTY];
			Sect[k->x = nextx][k->y = nexty] = things[KLINGON];
		}
	}
	compkldist(0);
}


movkling(fqx,fqy,qx,qy)
int	fqx, fqy;
int	qx, qy;
{
	register int	qs;

	if (qx < 0 || qx >= NQUADS || qy < 0 || qy >= NQUADS ||
			Quad[qx][qy].stars < 0 || Quad[qx][qy].qkling > 8)
		return(0);
	qs = Quad[qx][qy].scanned;
	if (qs >= 0 && qs < 1000)
		Quad[qx][qy].scanned =+ 100;
	qs = Quad[fqx][fqy].scanned;
	if (qs >= 0 && qs < 1000)
		Quad[fqx][fqy].scanned =- 100;
	Quad[fqx][fqy].qkling =- 1;
	Quad[qx][qy].qkling =+ 1;
	return(1);
}
