#include	<stdio.h>

char	nout[100] = "nohup.out";
char	*getenv();

main(argc, argv)
char **argv;
{
	char	*home;
	if(argc < 2) {
		fputs("usage: nohup command arg ...\n", stderr);
		exit(2);
	}
	argv[argc] = 0;
	signal(1, 1);
	signal(3, 1);
	if(isatty(1)) {
		if(freopen(nout, "a", stdout) == NULL) {
			if((home=getenv("HOME")) == NULL) {
				fputs("nohup: cannot open/create nohup.out\n", stderr);
				exit(2);
			}
			strcpy(nout,home);
			strcat(nout,"/nohup.out");
			if(freopen(nout, "a", stdout) == NULL) {
				fputs("nohup: cannot open/create nohup.out\n", stderr);
				exit(2);
			}
		}
		fprintf(stderr, "Sending output to %s\n", nout);
	}
	if(isatty(2)) {
		close(2);
		dup(1);
	}
	execvp(argv[1], &argv[1]);
	freopen("/dev/tty", "w", stderr);
	fputs(argv[1],stderr); fputs(": not found\n",stderr);
	exit(2);
}
