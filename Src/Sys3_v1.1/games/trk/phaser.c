# include	"trek.h"

/**
 **	phaser control
 **/

CVNTAB	Matab[]
{
	"a",		"utomatic",
	"m",		"anual",
	0
};

phaser()
{
	register int		i, k, sev;
	int			manual, flag, units, extra;
	int			hit, reqd, n;
	double			tot;
	int			hits[MAXKLQUAD], hitreqd[MAXKLQUAD];
	double			dustfac, tothe;

	if (Status.cond == DOCKED) {
		printf("Phasers cannot fire through starbase shields.\n");
		return;
	}
	if (sev = Damage[PHASER]) 
		if(sev == 1) {
			printf("Phaser control damaged.\n");
			return;
		}
		else
			printf("Sulu: Phasers showing %d%% efficiency\n",
				(sev -1) * 25);
	if(Status.shldup) {
		printf("Scotty: We will have to divert power from the shields.\n");
		Move.shldchg = 1;
	}
	if (Status.cloaked && Nkling > 0) {
		printf("Sulu: Federation regulations do not permit attack while cloaked\n");
		if(!getynpar("Are you willing to violate regulations?"))
			return;
		violations++;
		randmsg("Aye aye, sir!", 1);
	}
	manual = 0;
	if (Damage[COMPUTER])
		if(lineended()) {
			printf("Computer damaged, manual mode selected\n");
			manual = 1;
		}

	if (!manual)
	{
		if((manual=(getcodpar("Manual or automatic: ", Matab))) < 0) return;
	}
	if (!manual && Damage[COMPUTER])
	{
		printf("Computer damaged, manual selected\n");
		manual = 1;
	}

	/* set up hits[] with amount for each klingon */
	flag = 1;
	n = Nkling;
	if (manual)
	{
		while (flag)
		{
			printf("%d units available\n", Status.energy);
			extra = 0;
			for (i = 0; i < n; i++)
			{
				printf("Klingon at %d,%d:",
					Kling[i].x, Kling[i].y);
				if(getintpar(" units to fire", &units)==0 || units < 0) return;
				hits[i] = units;
				extra =+ units;
				if (extra > Status.energy)
				{
					printf("Available energy exceeded. ");
					break;
				}
			}
			flag = i < n;
			if (extra <= 0)
				return;
		}
		Status.energy =- extra;
		extra = 0;
	}
	else
	{
		/* automatic distribution of power */
		printf("Phasers locked on target.  ");
		while (flag)
		{
			printf("%d units available\n", Status.energy);
			if(getintpar("Units to fire", &units)==0 || units < 0) return;
			if (units > Status.energy)
			{
				printf("Available energy exceeded.  ");
				continue;
			}
			flag = 0;
			Status.energy =- units;
			extra = units;
			if (Nkling != 0)
			{
				tot = Nkling * (Nkling + 1) / 2;
				for (i = 0; i < Nkling; i++)
				{
					hits[i] = ((Nkling - i) / tot) * extra;
					extra =- hits[i];
					dustfac = 0.90;
					tothe = Kling[i].dist;
					hitreqd[i] = Kling[i].power / pow(dustfac, tothe) + 0.5;
					if (hits[i] > hitreqd[i])
					{
						extra =+ hits[i] - hitreqd[i];
						hits[i] = hitreqd[i];
					}
				}
				if (extra != 0)
				{
					for (i = 0; i < Nkling; i++)
					{
						reqd = hitreqd[i] - hits[i];
						if (reqd <= 0)
							continue;
						if (reqd >= extra)
						{
							hits[i] =+ extra;
							extra = 0;
							break;
						}
						hits[i] = hitreqd[i];
						extra =- reqd;
					}
				}
			}
		}
	}

	/* actually fire the shots */
	Move.free = 0;
	k = 0;
	for (i = 0; i < n; i++)
	{
		if(sev)
			if(!ranf(sev))
				hits[i] *= (sev-1) * .25;
		if (hits[i] == 0)
		{
			k =+ 1;
			continue;
		}
		dustfac = 0.90 + 0.01 * franf();
		tothe = Kling[k].dist;
		hit = hits[i] * pow(dustfac, tothe) + 0.5;
		Kling[k].power =- hit;
		printf("%d unit hit\t", hit);
		if (Kling[k].power <= 0) {
			extra =- Kling[k].power;
			killk(Kling[k].x, Kling[k].y);
		} else {
			printf("on Klingon at %d,%d\n",	Kling[k].x, Kling[k].y);
			k =+ 1;
		}
	}
	if (extra > 0)
		printf("%d units expended on empty space\n", extra);
}
