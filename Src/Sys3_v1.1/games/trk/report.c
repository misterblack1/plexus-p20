# include	"trek.h"

/**
 **	damage control report
 **/

dcrept()
{
	register int		i, f;
	int			sev;
	float			x;
	float			m1, m2;
	register EVENT		*e;

	if (Status.cond == DOCKED)
	{
		m1 = 1.0 / Param.dockfac;
		m2 = 1.0;
	}
	else
	{
		m1 = 1.0;
		m2 = Param.dockfac;
	}
	printf("Damage control report:\t");
	f = 1;
	for (i = 0; i < MAXEVENTS; i++)
	{
		e = &Event[i];
		if (e->evcode != E_FIXDV)
			continue;
		if (f)
		{
			printf("  repair times\t  severity level\n");
			printf("			in flight  docked\n");
			f = 0;
		}
		x = e->date - Status.date;
		printf("%-24s%7.2f  %7.2f",
			Device[e->systemname].name, x * m1 + 0.005, x * m2 + 0.005);
		sev = Damage[getdev(Device[e->systemname].name)];
		printf("\t%d\n", sev);
		if (!Damage[e->systemname]) {
			printf("Damage discrepancy device %d\n",
				e->systemname);
			syserr();
		}
	}
	if (f)
		printf("all devices functional\n");
}

char *eout[] {
	"snova",
	"lrtb",
	"katsb",
	"kdesb",
	"issue",
	"enslv",
	"repro",
	"fixdv",
	"attk",
	"snap"
};

eventpr()
{
	register int		i;
	register EVENT		*e;

	if(!adm()) {
		printf("cannot comply!\n");
		return;
	}
	Game.tourn=0;
	for(i = 0; i < MAXEVENTS; i++) {
		e = &Event[i];
		if(e->evcode<0)
			continue;
		printf("@ %.2f %s\t%d,%d,%d\n", e->date, eout[e->evcode&~E_NREPORT], e->x, e->y, e->evdata);
	}
}
