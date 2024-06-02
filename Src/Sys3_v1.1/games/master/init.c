/*
 *	This function initializes "slots" and "colors",
 *	and scrambles the random number generator.
 */

#include "max.h"

init()
{
	extern int slots, colors;
	extern char *cnames[];
	int i;
	long time();

	/*
	 * 	Scramble the random number generator
	 */
	srand ((int) (time ((long *) 0) & (long)(unsigned)-1));

	do {	printf ("Enter number of slots (1-%d): ", MAXSLOTS);
		slots = read1();
	} while (slots < 1 || slots > MAXSLOTS);

	do {	printf ("Enter number of colors (1-%d): ", MAXCOLORS);
		colors = read1();
	} while (colors < 1 || colors > MAXCOLORS);

	printf ("The list of available colors is:\n");
	for (i = 0; i < colors; i++)
		printf ("%s ", cnames [i]);
	putchar('\n');
}
