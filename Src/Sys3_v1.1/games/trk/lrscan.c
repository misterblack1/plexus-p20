# include	"trek.h"

/**
 **	long range scanners
 **/

lrscan()
{
	register int	i, j, j2;
	int	 scrups = 0;
	int	 ret;

	switch(Damage[LRSCAN]) {
		case 0:
			break;

		case 1:
			printf("Scotty: L. R. Scanners out\n");
			if(Status.cond == DOCKED)
				printf("; using Starbase scanners.\n");
			else {
				printf("\n");
				return;
			}
			break;

		default:
			if(!rmsgs[LRSC])
				if(randmsg("Scotty: May I remind you that L. R. scanners cannot be fully trusted\nin their present condition", 3))
					rmsgs[LRSC] = 1;
			scrups = Damage[LRSCAN];
			break;
	}
	printf(" ");
	for (j = Quady-1; j <= Quady + 1; j++)
	{
		if (j < 0 || j >= NQUADS)
			printf("    ");
		else
			printf(" -%1d-", j);
	}
	printf("\tLong range scan for quadrant %d,%d\n", Quadx, Quady);
	for (i = Quadx - 1; i <= Quadx + 1; i++) {
		if (i < 0 || i >= NQUADS) {
			printf("   *   *   *");
		} else {
			printf("%1d", i);
			for (j = Quady - 1; j <= Quady + 1; j++) {
				if (j < 0 || j >= NQUADS) {
					printf("  * ");
				} else {
					j2 = j;
					if(scrups) {
						ret = ranf(Damage[LRSCAN] - 1);
						if(ret)
							j2 += ret;
						scrups--;
					}
					prsect(1, i, j2);
				}
			}
		}
		printf("\n");
	}
	return;
}
