/*
 *	Reads a 1-digit integer from the keyboard
 */

#include <stdio.h>

read1()
{
	char s[2];
	register int i;
	register char c;

	for(;;)	{
		i = 0;
		do {	while ((c = getchar()) == ' ');
			if (i < 2)
				s [i++] = c;
		} while (c != '\n');
		if (s[0] >= '0' && s[0] <= '9' && s[1] == '\n')
			break;
		printf ("Incorrect input; please re-enter.\n");
	}
	return s[0] - '0';
}
