# include	"trek.h"

/**
 **	cause time to elapse
 **/

/* snapshot for time warps */
char	Snapshot[14 + sizeof Quad + sizeof Event + sizeof Base + sizeof Etc];

double randly(evname)
int	evname;
{
	return(Param.eventdly[evname] * franf());
}

double logdly(evname)
int	evname;
{
	double d;

	d = Param.eventdly[evname] * Initial.time * log(franf());
	if(evname==E_LRTB)	d =/ Status.kling;
	return(-d);
}

events()
{
	register int		i;
	int			j;
	int			restcancel;
	KLINGONS		*k;
	double			rtime;
	double			xdate;
	double			idate;
	EVENT			*ev;
	int			ix, iy;
	register QUAD		*q;
	register EVENT		*e;
	int			evnum;

	if (Move.time <= 0.0)
	{
		Status.time = Status.resource / Status.kling;
		return;
	}
	idate = Status.date;
	if (Move.time > 0.5 && Move.resting)
		schedule(E_ATTACK, 0.5, 0, 0, 0);
	while (1)
	{
		restcancel = 0;
		evnum = -1;
		xdate = idate + Move.time;
		for (i = 0; i < MAXEVENTS; i++)
		{
			e = &Event[i];
			if (e->evcode < 0)
				continue;
			if (e->date < xdate)
			{
				xdate = e->date;
				ev = e;
				evnum = i;
			}
		}
		rtime = xdate - Status.date;
		Status.resource =- Status.kling * rtime;
		Status.time = Status.resource / Status.kling;
		Status.date = xdate;
		if (Status.time <= 0.0)
			lose(L_NOTIME);
		if (evnum < 0)
			break;
		e = ev;
		switch (e->evcode)
		{
		  case E_SNOVA:			/* supernova */
			snova(-1);
			reschedule(e, logdly(E_SNOVA));
			break;

		  case E_LRTB:			/* long range tractor beam */
			if (Status.cond != DOCKED && Nkling < 3)
			{
				i = ranf(Status.kling) + 1;
				for (ix = 0; ix < NQUADS; ix++)
				{
					for (iy = 0; iy < NQUADS; iy++)
						if (Quad[ix][iy].stars >= 0)
							if ((i =- Quad[ix][iy].qkling) <= 0)
								break;
					if (i <= 0)
						break;
				}
				Quadx = ix;
				Quady = iy;
				printf("%s caught in long range tractor beam", Status.shipname);
				printf("; pulled to quadrant %d,%d\n", Quadx, Quady);
				Sectx = ranf(NSECTS);
				Secty = ranf(NSECTS);
				initquad(0);
				Move.time = xdate - idate;
			}
			reschedule(e, logdly(E_LRTB));
			break;

		  case E_KATSB:			/* Klingon attacks starbase */
			if (Status.bases <= 0)
			{
				unschedule(e);
				break;
			}
			i = 1;
			for (ix = 0; ix < NQUADS; ix++)
			{
				for (iy = 0; iy < NQUADS; iy++)
					if (Quad[ix][iy].bases > 0 && Quad[ix][iy].qkling > 0)
					{
						i = 0;
						break;
					}
				if (!i)
					break;
			}
			reschedule(e, logdly(E_KATSB));
			if (i)	break;
			e = schedule(E_KDESB, 0.5 + randly(E_KDESB), ix, iy, 0);
			if (!Damage[SSRADIO])
			{
				printf("Uhura:\tCaptain, we have recieved a distress signal\n");
				printf("\tfrom the starbase in quadrant %d,%d.\n",
					ix, iy);
				restcancel++;
			}
			else
				e->evdata = E_NREPORT;
			break;

		  case E_KDESB:			/* Klingon destroys starbase */
			unschedule(e);
			q = &Quad[e->x][e->y];
			if (q->bases <=0 || q->qkling <= 0)
				break;
			if (e->x == Quadx && e->y == Quady)
			{
				printf("\nSpock: ");
				killb(Quadx, Quady);
			}
			else
				killb(e->x, e->y);
			break;

		  case E_ISSUE:		/* issue a distress call */
			reschedule(e, logdly(E_ISSUE));
			if (Status.distressed >= MAXDISTR)
				break;
			for (i = 0; i < 100; i++)
			{
				ix = ranf(NQUADS);
				iy = ranf(NQUADS);
				q = &Quad[ix][iy];
				if (!((ix == Quadx && iy == Quady) || q->stars < 0 ||
				    q->systemname&(Q_DISTRESS|Q_GHOST) ||
				    q->systemname == 0 || q->qkling <= 0))
					break;
			}
			if (i >= 100)
				break;
			Status.distressed =+ 1;
			e = schedule(E_ENSLV, randly(E_ENSLV), ix, iy, q->systemname);
			q->systemname = (e - Event) | Q_DISTRESS;
			if (!Damage[SSRADIO])
			{
				printf("Uhura:\tCaptain, starsystem %s in quadrant %d,%d is under attack.\n",
					Systemname[e->evdata], ix, iy);
				restcancel++;
			}
			else
				e->evdata =| E_NREPORT;
			break;

		  case E_ENSLV:		/* starsystem is enslaved */
			unschedule(e);
			/* see if current distress call still active */
			q = &Quad[e->x][e->y];
			if (q->qkling <= 0)
			{
				e->evdata =& ~E_NREPORT;
				if(q->systemname&Q_DISTRESS)
					q->systemname = e->evdata;
				break;
			}
			else
				schedule(E_REPRO, randly(E_REPRO), e->x, e->y, e->evdata);
			if (!Damage[SSRADIO])
			{
				printf("Uhura:\tWe've lost contact with starsystem %s\n",
					Systemname[e->evdata]);
				printf("  in quadrant %d,%d.\n",
					e->x, e->y);
			}
			break;

		  case E_REPRO:		/* Klingon reproduces */
			/* see if distress call is still active */
			q = &Quad[e->x][e->y];
			if (q->qkling <= 0)
			{
				unschedule(e);
				if (q->systemname&Q_DISTRESS)
					q->systemname = e->evdata;
				break;
			}
			reschedule(e, randly(E_REPRO));
			/* reproduce one Klingon */
			ix = e->x;
			iy = e->y;
			if (Status.kling == 127)
				break;		/* full right now */
			if (q->qkling >= 9)
			{
				/* this quadrant not ok */
				for (i = ix - 1; i <= ix + 1; i++)
				{
					if (i < 0 || i >= NQUADS)
						continue;
					for (j = iy - 1; j <= iy + 1; j++)
					{
						if (j < 0 || j >= NQUADS)
							continue;
						q = &Quad[i][j];
						if (q->qkling >= 9 || q->stars < 0)
							continue;
						break;
					}
					if (j <= iy + 1)
						break;
				}
				if (j > iy + 1)
					/* cannot create another yet */
					break;
				ix = i;
				iy = j;
			}
			q->qkling =+ 1;
			Status.kling =+ 1;
			if (ix == Quadx && iy == Quady)
			{
				/* we must position Klingon */
				sector(&ix, &iy);
				Sect[ix][iy] = things[KLINGON];
				k = &Kling[Nkling++];
				k->x = ix;
				k->y = iy;
				k->power = Param.klingpwr;
				compkldist(Kling[0].dist == Kling[0].avgdist ? 0 : 1);
			}
			Status.time = Status.resource / Status.kling;
			break;

		  case E_SNAP:
			reschedule(e, logdly(E_SNAP));
			i = &Snapshot;
			i = bmove(&Status.bases, i, 2);
			i = bmove(&Status.date, i, 12);
			i = bmove(&Quad, i, sizeof Quad);
			i = bmove(&Event, i, sizeof Event);
			i = bmove(&Base, i, sizeof Base);
			i = bmove(&Etc, i, sizeof Etc);
			Game.snap = 1;
			break;

		  case E_ATTACK:	/* Klingons attack during rest period */
			if (!Move.resting)
			{
				unschedule(e);
				break;
			}
			attack(1);
			reschedule(e, 0.5);
			break;

		  case E_FIXDV:
			i = e->evdata;
			unschedule(e);
			Damage[i] = 0;
			printf("%s reports repair work on the %s finished.\n",
				Device[i].person, Device[i].name);
			switch (i)
			{
			  case LIFESUP:
				Status.reserves = Initial.reserves;
				break;

			  case SINS:
				if (Status.cond == DOCKED)
					break;
				printf("Spock has tried to recalibrate your Space Internal Navigation System,\n");
				printf("  but he has no standard base to calibrate to.  Suggest you get\n");
				printf("  to a starbase immediately so that you can properly recalibrate.\n");
				Status.sinsbad = 1;
				break;

			  case SSRADIO:
				restcancel = dumpssradio();
				break;

			  case SRSCAN:
				rmsgs[SRSC] = 0;
				break;

			  case LRSCAN:
				rmsgs[LRSC] = 0;
				break;
			}
			break;

		  default:
			break;
		}

		if(Move.resting && restcancel && getynpar("Spock: Shall we cancel our rest period: "))
			Move.time = xdate - idate;
	}

	if (e = Etc.eventptr[E_ATTACK])
		unschedule(e);
	if (Status.cloaked)
		Status.energy =- Param.cloakenergy * Move.time;
	rtime = 1.0 - exp(-Param.regenfac * Move.time);
	Status.shield =+ (Initial.shield - Status.shield) * rtime;
	Status.energy =+ (Initial.energy - Status.energy) * rtime;
	if (Damage[LIFESUP] && Status.cond != DOCKED)
		Status.reserves =- Move.time;
	return;
}
