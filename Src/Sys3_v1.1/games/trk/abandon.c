# include	"trek.h"

/**
 **	abandon ship
 **/

abandon()
{
	register QUAD		*q;
	register int		i;
	int			j;
	register EVENT		*e;
	char	checkpass[PWDLEN];

	if (Status.ship == things[QUEENE]) {
		printf("Ye Fairie Queene has no shuttlecraft\n");
		return;
	}
	if (Damage[SHUTTLE]) {
		printf("Shuttlecraft damaged\n");
		return;
	}
	sleep(4);
	printf("Officers escape in shuttlecraft\n");
	/* decide on fate of crew */
	q = &Quad[Quadx][Quady];
	if (q->systemname == 0 || q->systemname&Q_GHOST || Damage[XPORTER])
	{
		printf("Entire crew of %d left to die in outer space\n",
			Status.crew);
		Game.deaths =+ Status.crew;
	}
	else
	{
		i = getqname(q);
		printf("Crew beams down to planet %s\n",
			Systemname[i]);
	}
	/* see if another ship exists */
	if (Status.bases == 0)
		lose(L_CAPTURED);
	/* re-outfit new ship */
	printf("You are hereby put in charge of an antiquated but still\n");
	printf("  functional ship, the Fairie Queene.\n");
	Status.ship = things[QUEENE];
	Status.shipname = "Fairie Queene";
	Initial.energy = Status.energy = 3000;
	Initial.torped = Status.torped = 10;
	Initial.shield = Status.shield = 1250;
	Status.shldup = 0;
	Status.cloaked = 0;
	Status.warp = 5.0;
	Status.warp2 = 25.0;
	Status.warp3 = 125.0;
	Status.cond = GREEN;
	/* clear out damages on old ship */
	for (i = 0; i < MAXEVENTS; i++)
	{
		e = &Event[i];
		if (e->evcode != E_FIXDV)
			continue;
		unschedule(e);
		Damage[e->evdata] = 0;
	}
	/* get rid of some devices and redistribute probabilities */
	i = Param.damprob[SHUTTLE] + Param.damprob[CLOAK];
	Param.damprob[SHUTTLE]  = 0;
	for (j = 0; j < NDEV && i > 0; j++) {
		if (Param.damprob[j] != 0)
		{
			Param.damprob[j] =+ 1;
			i =- 1;
		}
	}

	/* pick a starbase to restart at */
	i = ranf(Status.bases);
	Quadx = Base[i].x;
	Quady = Base[i].y;
	/* setup that quadrant */
	while (1)
	{
		initquad(1);
		Sect[Sectx][Secty] = things[EMPTY];
		for (i = 0; i < 5; i++)
		{
			Sectx = Starbase.x + ranf(3) - 1;
			if (Sectx < 0 || Sectx >= NSECTS)
				continue;
			Secty = Starbase.y + ranf(3) - 1;
			if (Secty < 0 || Secty >= NSECTS)
				continue;
			if (Sect[Sectx][Secty] == things[EMPTY])
			{
				Sect[Sectx][Secty] = things[QUEENE];
				dock();
				compkldist(0);
				return;
			}
		}
	}
}


getqname(q)
QUAD *q;
{
	register int		i;

	i = q->systemname;
	if(i&Q_DISTRESS)
		i=Event[i&Q_STARNAME].evdata;
	return(i);
}
