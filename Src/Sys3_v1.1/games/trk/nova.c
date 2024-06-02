# include	"trek.h"

/**
 **	cause a nova to occur
 **/

nova(x, y)
int	x, y;
{
	int		i, j;
	int		se;

	if (Sect[x][y] != things[STAR] || Quad[Quadx][Quady].stars < 0)
		return;
	if (ranf(100) < 15)
	{
		printf("Spock: Star at %d,%d failed to nova\n", x, y);
		return;
	}
	if (ranf(100) < 5) {
		snova(x, y);
		return;
	}
	printf("Spock: Star at %d,%d gone nova\n", x, y);

	if(ranf(3)) {
		Sect[x][y] = things[EMPTY];
	} else {
		Sect[x][y] = things[BLACKHOLE];
		Quad[Quadx][Quady].holes =+ 1;
	}
	Quad[Quadx][Quady].stars =- 1;
	Game.kills =+ 1;
	for (i = x - 1; i <= x + 1; i++)
	{
		if (i < 0 || i >= NSECTS)
			continue;
		for (j = y - 1; j <= y + 1; j++)
		{
			if (j < 0 || j >= NSECTS)
				continue;
			se = Sect[i][j];
			switch (se)
			{
				case EM:
				case BL:
				break;
				case KL:
				killk(i, j);
				break;
			  case ST:
				nova(i, j);
				break;
				case IN:
				kills(i, j, -1);
				break;
				case BA:
				killb(i, j); Game.killb++;
				break;
				case EN:
				case QU:
				se = 2000;
				if (Status.shldup)
					if (Status.shield >= se)
					{
						Status.shield =- se;
						se = 0;
					}
					else
					{
						se =- Status.shield;
						Status.shield = 0;
					}
				Status.energy =- se;
				if (Status.energy <= 0)
					lose(L_SUICID);
				break;

			  default:
				printf("Unknown object %c at %d,%d destroyed\n",
					se, i, j);
				Sect[i][j] = things[EMPTY];
				break;
			}
		}
	}
	return;
}
