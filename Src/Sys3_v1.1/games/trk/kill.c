# include	"trek.h"

/**
 **	Handle a Klingon's death
 **/

killk(ix, iy)
int	ix, iy;
{
	register int		i;

	printf("   *** Klingon at %d,%d destroyed ***\n", ix, iy);
	Status.kling =- 1;
	Sect[ix][iy] = things[EMPTY];
	Quad[Quadx][Quady].qkling =- 1;
	Quad[Quadx][Quady].scanned =- 100;
	Game.gkillk =+ 1;
	for (i = 0; i < Nkling; i++)
		if (ix == Kling[i].x && iy == Kling[i].y)
		{
			Nkling =- 1;
			for (; i < Nkling; i++)
				bmove(&Kling[i+1], &Kling[i], sizeof Kling[i]);
			break;
		}
	if (Status.kling <= 0)
		win();
	Status.time = Status.resource / Status.kling;
}


/**
 **	handle a starbase's death
 **/

killb(qx, qy)
int	qx, qy;
{
	register QUAD		*q;
	register XY		*b;

	q = &Quad[qx][qy];

	if (q->bases <= 0)
		return;
	q->bases = 0;
	Status.bases =- 1;
	for (b = Base; ; b++)
		if (qx == b->x && qy == b->y)
			break;
	bmove(&Base[Status.bases], b, sizeof *b);
	if (qx == Quadx && qy == Quady)
	{
		Sect[Starbase.x][Starbase.y] = things[EMPTY];
		if (Status.cond == DOCKED)
			undock();
		printf("Spock: Starbase at %d,%d destroyed\n", Starbase.x, Starbase.y);
	} else if(!Damage[SSRADIO]) {
			printf("Uhura:\tStarfleet command reports that the starbase in\n");
			printf("\tquadrant %d,%d has been destroyed\n", qx, qy);
	}
	if(!Damage[SSRADIO]) {
		/* then update starchart */
		if (q->scanned < 1000)
			q->scanned =- 10;
		else
			if (q->scanned > 1000)
				q->scanned = -1;
	}
	if(Status.bases == 0 && !rmsgs[BASES])
		if(randmsg("Spock: I believe we will find it difficult obtaining further supplies in this galaxy", 4))
			rmsgs[BASES]++;
}


/**
 **	kill an inhabited starsystem
 **/

kills(x, y, f)
int	x, y;	/* quad coords if f == 0, else sector coords */
int	f;	/* f != 0 -- this quad;  f < 0 -- Enterprise's fault */
{
	register QUAD		*q;
	register EVENT		*e;
	register int		i;

	if (f)
	{
		/* current quadrant */
		q = &Quad[Quadx][Quady];
		Sect[x][y] = things[EMPTY];
		i = getqname(q);
		if(i==0) return;
		printf("Spock: Inhabited starsystem %s at %d,%d destroyed\n",
			Systemname[i], x, y);
		randmsg("I should say our aim was a bit off!", 21);
		if (f < 0)
			Game.killinhab =+ 1;
	}
	else
	{
		/* different quadrant */
		q = &Quad[x][y];
	}
	if (q->systemname&Q_DISTRESS)
	{
		/* distressed starsystem */
		e = &Event[q->systemname&Q_STARNAME];
		printf("Uhura: Distress call for %s invalidated\n",
			Systemname[e->evdata]);
		unschedule(e);
	}
	q->systemname = 0;
	q->stars =- 1;
}


/**
 **	"kill" a distress call
 **/

killd(x, y)
int	x, y;		/* quadrant coordinates */
{
	register EVENT		*e;
	register int		i;
	register QUAD		*q;

	q = &Quad[x][y];
	for (i = 0; i < MAXEVENTS; i++)
	{
		e = &Event[i];
		if (e->x != x || e->y != y)
			continue;
		switch (e->evcode)
		{
		  case E_KDESB:
			printf("Uhura: Distress call for starbase in %d,%d nullified\n",
					x, y);
			unschedule(e);
			break;

		  case E_ENSLV:
		  case E_REPRO:
			printf("Distress call for %s in quadrant %d,%d nullified\n",
					Systemname[e->evdata], x, y);
			q->systemname = e->evdata;
			unschedule(e);
		}
	}
}
