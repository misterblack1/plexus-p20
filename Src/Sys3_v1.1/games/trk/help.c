# include	"trek.h"

/**
 **	call starbase for help
 **/

char	*Cntvect[3]
{"first", "second", "third"};

help()
{
	register int		i;
	double			dist, x;
	register int		dx, dy;
	int			j, l;

	if (Status.cond == DOCKED) {
		printf("Uhura: But Captain, we're already docked\n");
		return;
	}
	if (Damage[SSRADIO]) {
		printf("Uhura: Sorry Captain, but the subspace radio is out\n");
		return;
	}
	if (Status.bases <= 0) {
		printf("Uhura: I'm not getting any response from starbase\n");
		randmsg("Maybe nobody's home!", 53);
		return;
	}

	Game.helps =+ 1;
	if((l=findbase(&dist))>=0) {
		Quadx=Base[l].x;
		Quady=Base[l].y;
		initquad(1);
	}
	/* dematerialize the Enterprise */
	Sect[Sectx][Secty] = things[EMPTY];
	printf("Starbase in %d,%d responds\n", Quadx, Quady);
	x = pow(1.0 - pow(0.94, dist), 0.3333333);
	/* attempt to rematerialize */
	for (i = 0; i < 3; i++)
	{
		sleep(2);
		printf("%s attempt to rematerialize ", Cntvect[i]);
		if (franf() > x)
		{
			for (j = 0; j < 5; j++)
			{
				dx = Starbase.x + ranf(3) - 1;
				if (dx < 0 || dx >= NSECTS)
					continue;
				dy = Starbase.y + ranf(3) - 1;
				if (dy < 0 || dy >= NSECTS || Sect[dx][dy] != things[EMPTY])
					continue;
				break;
			}
			if (j < 5)
			{
				printf("succeeds\n");
				Sectx = dx;
				Secty = dy;
				Sect[dx][dy] = Status.ship;
				dock();
				compkldist(0);
				return;
			}
		}
		printf("fails\n");
	}
	lose(L_NOHELP);
}

findbase(d)
double *d;
{
	register int i, dx, dy;
	int l;
	double dist;
	double x;

	/* find the closest base */
	dist = 1.0e38;
	if (Quad[Quadx][Quady].bases <= 0)
	{
		for (i = 0; i < Status.bases; i++)
		{
			dx = Base[i].x - Quadx;
			dy = Base[i].y - Quady;
			x = dx*dx + dy*dy; x = sqrt(x);
			if (x < dist)
			{
				dist = x;
				l = i;
			}
		}
	}
	else
	{
		dist = 0.0;
		l = -1;
	}
	*d=dist;
	return(l);
}
