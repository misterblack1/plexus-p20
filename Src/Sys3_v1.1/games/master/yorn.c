/*
 *	This program prompts with the string given as its
 *	argument and requests a reply of y or n. It returns
 *	1 for y and 0 for n.
 */

#include <stdio.h>

yorn (s)
	char *s;
{
	char reply[2];
	int i;
	char c;
	do {	printf ("%s", s);
		i = 0;
		do {	while ((c = getchar()) == ' ');
			if (i < 2)
				reply [i++] = c;
		} while (c != '\n');
	} while (i != 2 || reply[0] != 'y' && reply[0] != 'n');
	return reply[0] == 'y';
}
