/*
**	Time a command
*/

#include	<stdio.h>
#include	<signal.h>
#include	<errno.h>
#ifdef STOCKIII
#else
#include	<sys/types.h>
#include	<sys/times.h>
#endif

main(argc, argv)
char **argv;
{
	struct {
		long user;
		long sys;
		long childuser;
		long childsys;
	} buffer;

	register p;
	extern	errno;
	extern	char	*sys_errlist[];
	int	status;
	long	before, after;
	extern long times();

	before = times(&buffer);
	if(argc<=1)
		exit(0);
	p = fork();
	if(p == -1) {
		fprintf(stderr,"time: cannot fork -- try again.\n");
		exit(2);
	}
	if(p == 0) {
/*		close(1);	lem commented this out	*/
		execvp(argv[1], &argv[1]);
	        fprintf(stderr, "%s: %s\n", sys_errlist[errno], argv[1]);
		exit(2);
	}
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, 1);
	while(wait(&status) != p);
	if((status & 0377) != '\0')
		fprintf(stderr,"time: command terminated abnormally.\n");
	after = times(&buffer);
	fprintf(stderr,"\n");
#ifdef STOCKIII
	printt("real", (after-before));
	printt("user", buffer.childuser);
	printt("sys ", buffer.childsys);
#else
	printt("real", cv50to60(after-before));
	printt("user", cv50to60(buffer.childuser));
	printt("sys ", cv50to60(buffer.childsys));
#endif
	exit(status >> 8);
}

char quant[] = { 6, 10, 10, 6, 10, 6, 10, 10, 10 };
char *pad  = "000      ";
char *sep  = "\0\0.\0:\0:\0\0";
char *nsep = "\0\0.\0 \0 \0\0";

printt(s, a)
char *s;
long a;
{
	register i;
	char	digit[9];
	char	c;
	int	nonzero;

	for(i=0; i<9; i++) {
		digit[i] = a % quant[i];
		a /= quant[i];
	}
	fprintf(stderr,s);
	nonzero = 0;
	while(--i>0) {
		c = digit[i]!=0 ? digit[i]+'0':
		    nonzero ? '0':
		    pad[i];
		if (c != '\0')
			putc (c, stderr);
		nonzero |= digit[i];
		c = nonzero?sep[i]:nsep[i];
		if (c != '\0')
			putc (c, stderr);
	}
	fprintf(stderr,"\n");
}
