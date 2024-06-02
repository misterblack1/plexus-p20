#include "trek.h"

/**
 **	get status info
 **/

char	*Color[4]
{
	"GREEN",
	"DOCKED",
	"YELLOW",
	"RED"
};

CVNTAB	Infotab[]
{
	"st",		"ardate",
	"c",		"ondition",
	"p",		"osition",
	"w",		"arp",
	"e",		"nergy",
	"to",		"rpedo",
	"sh",		"ields",
	"k",		"lingons",
	"t",		"ime",
	"l",		"ife",
	"cr",		"ew",
	"br",		"ig",
	0
};

info()
{
	register int	i;

	if((i=getcodpar("Request: ", Infotab))<0) return;
	getinfo(i);
}

getinfo(i)
{
	register char	*s;

	switch (i)
	{
	  case 0:
		printf("stardate      %.2f", Status.date);
		break;
	  case 1:
		printf("condition     %s", Color[Status.cond]);
		if (Status.cloaked)
			printf(", CLOAKED");
		break;
	  case 2:
		printf("position      %d,%d/%d,%d",Quadx, Quady, Sectx, Secty);
		break;
	  case 3:
		printf("warp factor   %.1f", Status.warp);
		break;
	  case 4:
		printf("total energy  %d", Status.energy);
		break;
	  case 5:
		printf("torpedoes     %d", Status.torped);
		break;
	  case 6:
		s = "down";
		if (Status.shldup)
			s = "up";
		if (Damage[SHIELD])
			s = "damaged";
		printf("shields       %s, %d%%", s, effshld());
		break;
	  case 7:
		printf("Klingons left %d", Status.kling);
		break;
	  case 8:
		printf("time left     %.2f", Status.time);
		break;
	  case 9:
		printf("life support  ");
		if (Damage[LIFESUP])
		{
			printf("damaged, reserves = %.2f", Status.reserves);
			break;
		}
		printf("active");
		break;

	  case 10:
		printf("crew\t%d", Status.crew);
		break;

	  case 11:
		printf("brig space\t%d", Status.brigfree);
		break;
	}
	printf("\n");
}
