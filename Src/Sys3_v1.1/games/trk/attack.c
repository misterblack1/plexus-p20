# include	"trek.h"

/**
 **	Klingon attack routine
 **/

attack(resting)
int	resting;	/* set if attack while resting */
{
	register int		hit, i, l;
	int			maxhit, tothit, shldabsb, percent;
	double			chgfac, propor, extradm;
	double			dustfac, tothe;
	int			cas, sev;
	int			hitflag;

	if (Move.free)
		return;
	if (Nkling <= 0 || Quad[Quadx][Quady].stars < 0)
		return;
	if (Status.cloaked && Status.cloakdate != Status.date)
		return;
	/* move before attack */
	klmove(0);
	if (Status.cond == DOCKED)
	{
		if (!resting)
			printf("Star Base shields protect the %s\n", Status.shipname);
		return;
	}
	/* setup shield effectiveness */
	chgfac = 1.0;
	if (Move.shldchg)
		chgfac = 0.25 + 0.50 * franf();
	maxhit = tothit = 0;
	hitflag = 0;

	/* let each Klingon do his damndest */
	for (i = 0; i < Nkling; i++)
	{
		/* if he's low on power he won't attack */
		if (Kling[i].power < 20)
			continue;
		if (!hitflag)
		{
			printf("%.2f: Klingon attack.\n",
				Status.date);
			hitflag++;
		}
		/* complete the hit */
		dustfac = 0.90 + 0.01 * franf();
		tothe = Kling[i].avgdist;
		hit = Kling[i].power * pow(dustfac, tothe) * Param.hitfac;
		/* deplete his energy */
		dustfac = Kling[i].power;
		Kling[i].power = dustfac * Param.phasfac * (1.0 + (franf() - 0.5) * 0.2);
		/* see how much of hit shields will absorb */
		shldabsb = 0;
		if (Status.shldup || Move.shldchg)
		{
			propor = Status.shield;
			propor =/ Initial.shield;
			shldabsb = propor * chgfac * hit;
			if (shldabsb > Status.shield)
				shldabsb = Status.shield;
			Status.shield =- shldabsb;
		}
		/* actually do the hit */
		printf("HIT: %3d units from %d,%d", hit, Kling[i].x, Kling[i].y);
		percent = (shldabsb*100)/hit;
		hit =- shldabsb;
		if (shldabsb > 0)
			printf(" shields absorb %3d%%, effective hit %d\n", percent, hit);
		else
			printf("\n");
		tothit =+ hit;
		if (hit > maxhit)
			maxhit = hit;
		Status.energy =- hit;
		/* see if damages occurred */
		if (hit >= (15 - Game.skill) * (25 - ranf(12)))
		{
			printf("\tCRITICAL --- ");
			/* select a device from probability vector */
			cas = ranf(1000);
			for (l = 0; cas >= 0; l++)
				cas =- Param.damprob[l];
			l =- 1;
			/* compute amount of damage */
			extradm = (hit * Param.damfac[l]) / (75 + ranf(50)) + 0.5;
			/* select severity on certain devices */
			switch(l) {
				case WARP:
				case IMPULSE:
				case PHASER:
				case TORPED:
				case CLOAK:
				case XPORTER:
				case SRSCAN:
				case LRSCAN:
					sev = ranf(3) + 1;
					break;

				default:
					sev = 1;
			}
			/* damage the device */
			damage(l, extradm, sev);
			printf("\t%s damaged\n", Device[l].name);
			if (Damage[SHIELD])
			{
				if (Status.shldup)
					printf("Sulu: Shields knocked down, captain.\n");
				Status.shldup = 0;
				Move.shldchg = 0;
			}
		}
		if (Status.energy <= 0)
			lose(L_DSTRYD);
	}

	/* see what our casualities are like */
	if (maxhit >= 200 || tothit >= 500)
	{
		cas = tothit * 0.015 * franf();
		if (cas >= 2)
		{
			printf("McCoy: We suffered %d casualties in that attack.\n",
				cas);
			if(cas > 5)
				randmsg("Take it easy Jim!", 3);
			Game.deaths =+ cas;
			Status.crew =- cas;
			if(Status.crew < 50 && !rmsgs[ALONE])
				if(randmsg("Better watch it or you'l find yourself alone!", 2))
					rmsgs[ALONE]++;
		}
	}

	/* allow Klingons to move after attacking */
	klmove(1);

	return;
}
