# include	"trek.h"

/**
 **	output hidden distress calls
 **/

dumpssradio()
{
	register EVENT		*e;
	register int		j;
	register int		printed;

	for (j = 0; j < MAXEVENTS; j++)
	{
		printed = 0;
		e = &Event[j];
		if (e->evdata&E_NREPORT)
			printed =| report(e);
	}
	return(printed);
}


report(e)
EVENT *e;
{
	register int		printed;

	printed=0;
	switch (e->evcode) {
	  case E_KDESB:
		printed++;
		printf("Starbase in quadrant %d,%d is under attack\n",
				e->x, e->y);
		e->evdata =& ~E_NREPORT;
		break;

	  case E_ENSLV:
	  case E_REPRO:
		printed++;
		e->evdata =& ~E_NREPORT;
		printf("Starsystem %s in quadrant %d,%d is distressed\n",
			Systemname[e->evdata], e->x, e->y);
		break;

	}
	return(printed);
}
