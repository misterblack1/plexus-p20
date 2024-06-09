/*
 *	prfstat - change and/or report profiler status
 */

#define	PRF_ON	 1
#define	PRF_VAL	 2

main(argc, argv)
	char	**argv;
{
	register  int  prf;

	if((prf = open("/dev/prf", 2)) < 0) {
		printf("cannot open /dev/prf\n");
		exit(1);
	}
	if(argc > 2) {
		printf("usage: prfstat  [ on ]  [ off ]\n");
		exit(1);
	}

	if(argc == 2) {
		if(strcmp("off", argv[1]) == 0)
			ioctl(prf, 3, 0);
		else if(strcmp("on", argv[1]) == 0)
			ioctl(prf, 3, PRF_ON);
		else {
			printf("eh?\n");
			exit(1);
		}
	}
	printf("profiling %s\n", ioctl(prf, 1, 0) & PRF_ON ?
	    "enabled" : "disabled");
	if(ioctl(prf, 1, 0) & PRF_VAL)
		printf("%d kernel text addresses\n", ioctl(prf, 2, 0));
}
