/*
 *	This program plays one half of "Mastermind", in which the
 *	machine has to guess the person's configuration.
 *
 *	play1 returns the score as its value.
 */

#include <stdio.h>

play1()
{
	extern int slots, colors;
	int i, j, g;
	int score;
	int seed;
	long timeval, time();
	extern int tcount;

	printf ("Select your configuration, hit return...");
	while (getchar() != '\n');
	timeval = time ((long *) 0);
	seed = (timeval + (timeval >> 15)) & 077777;
	for(;;)	{
		gentab (slots, colors);
		score = 0;
		srand (seed);
		do {	i = randint (tcount);
			g = -1;
			do	g = next(g);
			while (i--);
			if (tcount == 1)
				printf ("Then it must be ");
			guess (&i, &j, g);
			prune (i, j, g);
			score++;
		} while	(tcount > 0 && (i != slots || j != slots));
		if (i == slots && j == slots) {
			printf ("I guessed your configuration in %d %s.\n",
				score, score == 1 ? "try" : "tries");
			break;
		} else
			printf ("Inconsistent...let's start again!\n");
	}
	return score;
}
