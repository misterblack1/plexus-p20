#define	PRF_ON	01
#define	PRF_VAL	02

int	buf[4096];
int	prfmax;

main(argc, argv)
	int	argc;
	char	**argv;
{
	register  int  prf, log;
	long	tvec, lseek();

	if(argc != 2)
		error("usage: prfsnap  logfile");
	if((prf = open("/dev/prf", 0)) < 0)
		error("cannot open /dev/prf");
	if((log = open(argv[1], 1)) < 0)
		if((log = creat(argv[1], 0666)) < 0)
			error("cannot creat log file");

	lseek(log, (long) 0, 2);
	if(ioctl(prf, 3, PRF_ON))
		error("cannot activate profiling");
	prfmax = ioctl(prf, 2, 0);
	time(&tvec);
	read(prf, buf, prfmax * 6 + 4);
	if(lseek(log, (long) 0, 1) == (long) 0) {
		write(log, &prfmax, sizeof prfmax);
		write(log, buf, prfmax * 2);
	}
	write(log, &tvec, sizeof tvec);
	write(log, &buf[prfmax], prfmax * 4 + 4);
}

error(s)
	char	*s;
{
	printf("error: %s\n", s);
	exit(1);
}
