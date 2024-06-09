/*
**	Concatenate files.
*/

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>

char	stdbuf[BUFSIZ];

main(argc, argv)
char **argv;
{
	register FILE *fi;
	register int c;
	int	fflg = 0;
	int	silent = 0;
	int	status = 0;
	int	dev, ino = -1;
	struct	stat	statb;

#ifdef STANDALONE
	if (argv[0][0] == '\0')
		argc = getargv ("cat", &argv, 0);
#endif
	setbuf(stdout, stdbuf);
	for( ; argc>1 && argv[1][0]=='-'; argc--,argv++) {
		switch(argv[1][1]) {
		case 0:
			break;
		case 'u':
			setbuf(stdout, (char *)NULL);
			continue;
		case 's':
			silent++;
			continue;
		}
		break;
	}
	if(fstat(fileno(stdout), &statb) < 0) {
		if(!silent)
			fprintf(stderr, "cat: Cannot stat stdout\n");
		exit(2);
	}
	statb.st_mode &= S_IFMT;
	if (statb.st_mode!=S_IFCHR && statb.st_mode!=S_IFBLK) {
		dev = statb.st_dev;
		ino = statb.st_ino;
	}
	if (argc < 2) {
		argc = 2;
		fflg++;
	}
	while (--argc > 0 && !ferror (stdout)) {
		if ((*++argv)[0]=='-' && (*argv)[1]=='\0' || fflg)
			fi = stdin;
		else {
			if ((fi = fopen(*argv, "r")) == NULL) {
				if (!silent)
					fprintf(stderr, "cat: cannot open %s\n", *argv);
				status = 2;
				continue;
			}
		}
		if(fstat(fileno(fi), &statb) < 0) {
			if(!silent)
				fprintf(stderr, "cat: cannot stat %s\n", *argv);
			status = 2;
			continue;
		}
		if (statb.st_dev==dev && statb.st_ino==ino) {
			if(!silent)
				fprintf(stderr, "cat: input %s is output\n",
				   fflg?"-": *argv);
			if (fclose(fi) != 0 ) 
				fprintf(stderr, "cat: close error\n");
			status = 2;
			continue;
		}
		while ((c = getc(fi)) != EOF)
			if (putchar (c) == EOF && ferror (stdout)) {
				if (!silent)
					fprintf (stderr, "cat: output error\n");
				status = 2;
				break;
			}
		if (fi!=stdin)
			if (fclose(fi) != 0) 
				fprintf(stderr, "cat: close error\n");
	}
	exit(status);
}
