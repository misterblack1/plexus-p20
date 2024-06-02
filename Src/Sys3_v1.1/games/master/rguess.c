/*
 *	This program reads a guess (as a collection of colors)
 *	from the keyboard, and returns the packed representation
 *	of the guess as its (integer) result.
 */

#include <stdio.h>


#include "max.h"

rguess()
{
	extern char *cnames[];
	extern int slots, colors;
	char c;
	int gval [MAXSLOTS+1], gp, ip, i;
	char name[10];
	for(;;)	{
		while ((c = getchar()) == ' ');
		gp = 0;
		do {	i = 0;
			while (c != ' ' && c != '\n') {
				if (i < 9)
					name[i++] = c;
				c = getchar();
			}
			name[i] = '\0';
			gval[gp] = -1;
			for (i = 0; i < colors; i++)
				if (abbrev (name, cnames[i])) {
					if (gval[gp] >= 0) {
						gval[gp] = -1;
						break;
					} else
						gval[gp] = i;
				}
			if (gp < MAXSLOTS)
				gp++;
			while (c == ' ')
				c = getchar();
		} while (c != '\n');
		ip = 0;
		for (i = 0; i < slots; i++)
			ip |= gval [i];
		if (ip >= 0 && gp == slots)
			break;
		if (gp == 1 && equal (name, "review"))
			review();
		else
			printf ("Please re-enter your guess.\n");
	}
	return pack (gval);
}
