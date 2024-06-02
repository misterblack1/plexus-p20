# include	"trek.h"

/**
 **	compute klingon distances
 **/

compkldist(f)
int	f;		/* set if new quadrant */
{
	register int		i, dx, dy;
	double			d;
	double			temp;

	if (Nkling == 0)
		return;
	for (i = 0; i < Nkling; i++)
	{
		/* compute distance to the Klingon */
		dx = Sectx - Kling[i].x;
		dy = Secty - Kling[i].y;
		d = dx*dx + dy*dy; d = sqrt(d);
		/* computer average of new and old distances to Klingon */
		if (!f)
		{
			temp = Kling[i].dist;
			Kling[i].avgdist = 0.5 * (temp + d);
		}
		else
			Kling[i].avgdist = d;
		Kling[i].dist = d;
	}
	sortkl();
}


/**
 **	sort klingons
 **		bubble sort on ascending distance
 **/

sortkl()
{
	KLINGONS		t;
	register int		f, i, m;

	m = Nkling - 1;
	f = 1;
	while (f)
	{
		f = 0;
		for (i = 0; i < m; i++)
			if (Kling[i].dist > Kling[i+1].dist)
			{
				bmove(&Kling[i], &t, sizeof t);
				bmove(&Kling[i+1], &Kling[i], sizeof t);
				bmove(&t, &Kling[i+1], sizeof t);
				f = 1;
			}
	}
	return;
}
