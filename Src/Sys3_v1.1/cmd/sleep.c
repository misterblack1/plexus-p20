/*
**	sleep -- suspend execution for an interval
**
**		sleep time
*/

#include	<stdio.h>

main(argc, argv)
char **argv;
{
	int	c, n;
	char	*s;

	n = 0;
	if(argc < 2) {
		fprintf(stderr, "usage: sleep time\n");
		exit(2);
	}
	s = argv[1];
	while(c = *s++) {
		if(c<'0' || c>'9') {
			fprintf(stderr, "sleep: bad character in argument\n");
			exit(2);
		}
		n = n*10 + c - '0';
	}
	sleep(n);
}
