#include	"time.h"
#define	PRF_ON	01
#define	PRF_VAL	02
#define	SIGALRM	14

int	buf[4096];
int	prfmax;

sigalrm()
{
	signal(SIGALRM, sigalrm);
}

main(argc, argv)
	int	argc;
	char	**argv;
{
	register  int  rate, prf, log;
	int	first = 1;
	int	toff = 17;
	long	tvec;
	struct	tm	*localtime();

	rate = 10;

	switch(argc) {
		default:
			error("usage: prfdc  logfile  [ rate  [ off_hour ] ]");
		case 4:
			toff = atoi(argv[3]);
		case 3:
			rate = atoi(argv[2]);
		case 2:
			;
	}
	if(rate <= 0)
		error("invalid sampling rate");
	if((prf = open("/dev/prf", 0)) < 0)
		error("cannot open /dev/prf");
	if(open(argv[1], 0) >= 0)
		error("existing file would be truncated");
	if((log = creat(argv[1], 0666)) < 0)
		error("cannot creat log file");

	if(ioctl(prf, 3, PRF_ON))
		error("cannot activate profiling");
	if(fork())
		exit(0);
	setpgrp();
	sigalrm();

	prfmax = ioctl(prf, 2, 0);
	write(log, &prfmax, sizeof prfmax);

	for(;;) {
		alarm(60 * rate);
		time(&tvec);
		read(prf, buf, prfmax * 6 + 4);
		if(first) {
			write(log, buf, prfmax * 2);
			first = 0;
		}
		write(log, &tvec, sizeof tvec);
		write(log, &buf[prfmax], prfmax * 4 + 4);
		if(localtime(&tvec)->tm_hour == toff)
			exit(0);
		pause();
	}
}

error(s)
	char	*s;
{
	printf("error: %s\n", s);
	exit(1);
}
