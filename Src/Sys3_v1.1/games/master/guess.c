/*
 *	Writes a guess to the terminal in English and
 *	reads the number of exact and inexact matches
 *	as given by the user. The program then translates
 *	these values to internal form and returns them
 *	indirectly through e and ival.
 */

#include <stdio.h>


#include "max.h"

guess (e, ival, x)
	int x, *e, *ival;
{
	extern int slots, colors;
	extern char *cnames [];
	int t [MAXSLOTS];
	char r[3];
	register char c;
	int ee, ii;
	register int i;

	unpack (x, t);
	for(;;)	{
		for (i = 0; i < slots; i++)
			printf ("%s ", cnames[t[i]]);
		i = 0;
		do {	while ((c = getchar()) == ' ');
			if (i < 3)
				r[i++] = c;
		} while (c != '\n');
		if (	r[0] >= '0'
		   &&	r[0] <= slots + '0'
		   &&	r[1] >= '0'
		   &&	r[1] <= slots + '0'
		   &&	r[2] == '\n')
			break;
		if (r[0] == 'y') {
			r[0] = slots + '0';
			r[1] = '0';
			break;
		}
		printf ("Incorrect response, try again.\n");
	}
	ee = r[0] - '0';
	ii = r[1] - '0';
	*e = ee;
	*ival = ee + ii;
}
