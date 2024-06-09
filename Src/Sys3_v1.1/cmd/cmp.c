/*
**	compare two files
*/

#include	<stdio.h>
#include	<ctype.h>

FILE	*file1,*file2;

char	*arg;

int	eflg;
int	lflg = 1;

long	line = 1;
long	chr = 0;
long	skip1;
long	skip2;

main(argc, argv)
char **argv;
{
	register c1, c2;

	if(argc < 3)
		narg();
	arg = argv[1];
	if(arg[0] == '-' && arg[1] == 's') {
		lflg--;
		argv++;
		argc--;
	}
	arg = argv[1];
	if(arg[0] == '-' && arg[1] == 'l') {
		lflg++;
		argv++;
		argc--;
	}
	if(argc != 3)
		narg();
	arg = argv[1];
	if( arg[0]=='-' && arg[1]==0 )
		file1 = stdin;
	else if((file1 = fopen(arg, "r")) == NULL)
		barg();
	arg = argv[2];
	if((file2 = fopen(arg, "r")) == NULL)
		barg();
	if (argc>3)
		skip1 = otoi(argv[3]);
	if (argc>4)
		skip2 = otoi(argv[4]);
	while (skip1) {
		if ((c1 = getc(file1)) == EOF) {
			arg = argv[1];
			earg();
		}
		skip1--;
	}
	while (skip2) {
		if ((c2 = getc(file2)) == EOF) {
			arg = argv[2];
			earg();
		}
		skip2--;
	}

	while(1) {
		chr++;
		c1 = getc(file1);
		c2 = getc(file2);
		if(c1 == c2) {
			if (c1 == '\n')
				line++;
			if(c1 == EOF) {
				if(eflg)
					exit(1);
				exit(0);
			}
			continue;
		}
		if(lflg == 0)
			exit(1);
		if(c1 == EOF) {
			arg = argv[1];
			earg();
		}
		if(c2 == EOF)
			earg();
		if(lflg == 1) {
			printf("%s %s differ: char %ld, line %ld\n", argv[1], arg,
				chr, line);
			exit(1);
		}
		eflg = 1;
		printf("%6ld %3o %3o\n", chr, c1, c2);
	}
}

otoi(s)
char *s;
{
	long v;
	int base;

	v = 0;
	base = 10;
	if (*s == '0')
		base = 8;
	while(isdigit(*s))
		v = v*base + *s++ - '0';
	return(v);
}

narg()
{
	fprintf(stderr, "usage: cmp [-l] [-s] file1 file2\n");
	exit(2);
}

barg()
{
	if (lflg)
		fprintf(stderr, "cmp: cannot open %s\n", arg);
	exit(2);
}

earg()
{
	fprintf(stderr, "cmp: EOF on %s\n", arg);
	exit(2);
}
