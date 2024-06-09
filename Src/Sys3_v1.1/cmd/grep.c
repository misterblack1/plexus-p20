/*
 * grep -- print lines matching (or not matching) a pattern
 *
 *	status returns:
 *		0 - ok, and some matches
 *		1 - ok, but no matches
 *		2 - some error
 */

#include <stdio.h>

/*
 * define some macros for rexexp.h
 */

#define INIT	register char *sp = instring;	/* First arg points to RE string */
#define GETC()		(*sp++)
#define PEEKC()		(*sp)
#define UNGETC(c)	(--sp)
#define RETURN(c)	return;
#define ERROR(c)	regerr()

#include <regexp.h>

#define	LBSIZE	512
#define ESIZE	256
#define BSIZE	512

char	*strchr();
long	lnum;
char	linebuf[LBSIZE+1];
char	expbuf[ESIZE];
int	nflag;
int	bflag;
int	lflag;
int	cflag;
int	vflag;
int	sflag;
int	nfile;
long	tln;
int	nsucc;

main(argc, argv)
char **argv;
{
	register c;
	int errflg = 0;
	extern int optind;

	while((c=getopt(argc, argv, "blcnsv")) != EOF) switch(c) {
		case 'v':
			vflag++;
			break;


		case 'c':
			cflag++;
			break;

		case 'n':
			nflag++;
			break;

		case 'b':
			bflag++;
			break;

		case 's':
			sflag++;
			break;

		case 'l':
			lflag++;
			break;

		case '?':
			errflg++;
		}
	argc -= optind;
	if(errflg || (argc<=0)) errexit("usage: grep -blcnsv pattern file . . .\n", (char *)NULL);
	argv = &argv[optind];
	if (strchr(*argv,'\n'))
		regerr();
	compile(*argv, expbuf, &expbuf[ESIZE], '\0');
	nfile = --argc;
	if (argc<=0)
		execute((char *)NULL);
	else while (--argc >= 0) {
		argv++;
		execute(*argv);
	}
	exit(nsucc == 2 ? 2 : nsucc == 0);
}


execute(file)
char *file;
{
	register char *p1;
	register c;

	if (file) {
		if (freopen(file, "r", stdin) == NULL) {
			if (!sflag)
				fprintf(stderr,"grep: can't open %s\n", file);
			nsucc = 2;
			return;
			}
	}
	lnum = 0;
	tln = 0;
	for (;;) {
		lnum++;
		p1 = linebuf;
		while ((c = getchar()) != '\n') {
			if (c == EOF) {
				if (cflag) {
					if (nfile>1)
						printf("%s:", file);
					printf("%ld\n", tln);
				}
				return;
			}
			*p1++ = c;
			if (p1 >= &linebuf[LBSIZE-1])
				break;
		}
		*p1++ = '\0';
		if(step(linebuf, expbuf) ^ vflag)
			succeed(file);
	}
}

succeed(f)
char *f;
{
	long tell();
	nsucc = (nsucc == 2) ? 2 : 1;
	if (cflag) {
		tln++;
		return;
	}
	if (lflag) {
		printf("%s\n", f);
		fseek(stdin, 0l, 2);
		return;
	}
	if (nfile > 1)
		printf("%s:", f);
	if (bflag)
		printf("%ld:", (ftell(stdin)-1)/BSIZE);
	if (nflag)
		printf("%ld:", lnum);
	printf("%s\n", linebuf);
}

regerr()
{
	errexit("RE error\n", (char *)NULL);
}

errexit(s, f)
char *s, *f;
{
	fprintf(stderr, s, f);
	exit(2);
}

