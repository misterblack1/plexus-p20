# include	"trek.h"

/**
 **	ram a klingon
 **/

collide(ix, iy)
int	ix, iy;
{
	register int		i;
	register char		c;

	printf(" *** RED ALERT: collision imminent\n");
	c = Sect[ix][iy];
	switch (c)
	{

	case KL:
		printf("%s rams Klingon at %d,%d\n", Status.shipname, ix, iy);
		hitkling(ix, iy, i = Status.warp * 100);
		break;

	case ST:
	case IN:
		sleep(2);
		printf("Spock: Hull temperature approaching 550 Degrees Kelvin.\n");
		lose(L_STAR);

	case BA:
		printf("You ran into the starbase at %d,%d\n", ix, iy);
		if(!Damage[SINS])
			Game.killb++;
		killb(Quadx,Quady);
		break;
	}
	sleep(2);
	printf("%s extensively damaged.\n", Status.shipname);
	i = 15 + ranf(10 * Game.skill);
	Game.deaths =+ i;
	Status.crew =- i;
	printf("McCoy: Take it easy Jim; we had %d casualties.\n", i);
	for (i = 0; i < NDEV; i++)
	{
		if (ranf(100) < 50)
			continue;
		damage(i, (Status.warp * (franf() + 1.0)) * Param.damfac[i] * 0.1, 1);
	}
	Status.shldup = 0;
	dcrept();
}
