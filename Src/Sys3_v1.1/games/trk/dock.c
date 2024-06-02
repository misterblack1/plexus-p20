# include	"trek.h"

/**
 **	dock to starbase
 **/

dock()
{
	register int		i, j;
	int			ok;
	register EVENT		*e;

	if (Status.cond == DOCKED) {
		printf("Chekov: But Captain, we are already docked\n");
		randmsg("Do you know where you are, sir?", 25);
		return;
	}
	/* check for ok to dock */
	ok = 0;
	for (i = Sectx - 1; i <= Sectx + 1 && !ok; i++)
	{
		if (i < 0 || i >= NSECTS)
			continue;
		for (j = Secty - 1; j <= Secty + 1; j++)
		{
			if (j  < 0 || j >= NSECTS)
				continue;
			if (Sect[i][j] == things[BASE])
			{
				ok = 1;
				break;
			}
		}
	}
	if (!ok) {
		printf("Chekov: But Captain, we are not adjacent to a starbase.\n");
		if(!Damage[SRSCAN])
			randmsg("Perhaps another peek at this quadrant is in order!", 15);
		return;
	}

	Status.energy = Initial.energy;
	Status.torped = Initial.torped;
	Status.shield = Initial.shield;
	Status.crew = Initial.crew;
	Status.brigfree = Initial.brigfree;
	Status.shldup = 0;
	Status.cloaked = 0;
	Status.cond = DOCKED;
	Status.reserves = Initial.reserves;
	Status.sinsbad = 0;
	dumpssradio();
	/* reschedule any device repairs */
	for (i = 0; i < MAXEVENTS; i++)
	{
		e = &Event[i];
		if (e->evcode != E_FIXDV)
			continue;
		reschedule(e, (Status.date - e->date)*(1 - Param.dockfac));
	}
	return;
}


/**
 **	leave a starbase
 **/

undock()
{
	register EVENT		*e;
	register int		i;

	if (Status.cond != DOCKED)
	{
		printf("Sulu: Pardon me captain, but we are not docked.\n");
		randmsg("Are you feeling ok, sir?", 31);
		return;
	}
	Status.cond = GREEN;
	Move.free = 0;
	/* reschedule device repair times (again) */
	for (i = 0; i < MAXEVENTS; i++)
	{
		e = &Event[i];
		if (e->evcode != E_FIXDV)
			continue;
		reschedule(e, (Status.date - e->date)*(1 - 1 / Param.dockfac));
	}
	return;
}
