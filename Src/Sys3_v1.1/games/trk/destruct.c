# include	"trek.h"

/**
 **	self destruct sequence
 **/

destruct()
{
	char		checkpass[PWDLEN];
	register int	i, j;
	float		zap;

	if (Damage[COMPUTER]) {
		printf("Computer damaged; cannot execute destruct sequence.\n");
		return;
	}
	printf("COUNT DOWN ... \n");
	sleep(3);
	for (i = 10; i > 5; i--)
	{
		for (j = 10;  j > i; j--)
			printf("   ");
		printf("%d\n", i);
		sleep(1);
	}
	getpasswd(checkpass);
	sleep(2);
	if (cf(checkpass, Game.passwd)) {
		printf("Self destruct sequence aborted\n");
		return;
	}
	printf("Password verified; self destruct sequence continues:\n");
	for (i = 5; i >= 0; i--)
	{
		sleep(1);
		for (j = 5; j > i; j--)
			printf("   ");
		printf("%d\n", i);
	}
	sleep(2);
	printf("\032***** %s destroyed *****\n", Status.shipname);
	Game.killed = 1;
	zap = 20.0 * Status.energy;
	for (i = 0; i < Nkling; )
	{
		if (Kling[i].power * Kling[i].dist <= zap)
			killk(Kling[i].x, Kling[i].y);
		else
			i =+ 1;
	}
	lose(L_DSTRCT);
}
