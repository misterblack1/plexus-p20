# include	"trek.h"

/**
 **	print out loser messages
 **/

lose(why)
int	why;
{
	Game.killed = 1;
	sleep(1);
	printf("\n");
	switch (why)
	{

	  case L_NOTIME:
		printf("You ran out of time\n");
		Game.killed = 0;
		break;

	  case L_NOENGY:
		printf("You ran out of energy\n");
		break;

	  case L_DSTRYD:
		if(randmsg("Say your prayers...", 5))
			sleep(5);
		printf("You have been destroyed\n");
		break;

	  case L_NEGENB:
		printf("You ran into the negative energy barrier\n");
		break;

	  case L_SUICID:
		printf("You destroyed yourself by nova'ing that star\n");
		break;

	  case L_SNOVA:
		printf("You have been caught in a supernova\n");
		break;

	  case L_NOLIFE:
		if(randmsg("Spock: Sir, it's getting a bit stuffy in here", 5))
			sleep(5);
		printf("You just suffocated in outer space\n");
		break;

	  case L_NOHELP:
		printf("You could not be rematerialized\n");
		break;

	  case L_TOOFAST:
		/* try to clear the screen */
		printf("\n\032 *** Ship's hull has imploded ***\n");
		break;

	  case L_STAR:
		printf("You have burned up in a star\n");
		break;

	  case L_DSTRCT:
		printf("Well, you destroyed yourself, but it didn't do any good\n");
		break;

	  case L_CAPTURED:
		printf("You have been captured by Klingons and mercilessly tortured\n");
		break;

	  case L_NOCREW:
		if(randmsg("Haven't you noticed its been quiet around here lately...", 5))
			sleep(5);
		printf("Your last crew member died\n");
		break;

	  default:
		printf("I don't know why, but you lost [%d]\n", why);
	}
	Move.endgame = -1;
	writelog(why);
	reset();
}
