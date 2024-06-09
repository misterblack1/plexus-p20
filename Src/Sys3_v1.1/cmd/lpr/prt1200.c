/*
 * prt -- TTY printer filter
 *
 * cc prt1200.c banner.c -lS
 * mv a.out /usr/lib/prt
 */

#include <stdio.h>

#define DHLINE "/dev/tty07"
#define STARTCOL  0
#define	LINELN	132
#define	EJLINE	63
#define ISPEED 9
#define OSPEED 9
#define ERASE '#'
#define KILL '@'
#define MODE 001700
#define ACCT "/usr/adm/tpracct"
#ifndef DEBUG
#define	DEBUG	0
#endif

struct sgtty {
	char sgispd,sgospd;
	char sgerase,sgkill;
	int sgflag;
} tmode;

int	anydone;
char	linebuf[LINELN+2];
int	ov;
char	ovbuf[LINELN+2];
FILE	*in = stdin;
FILE	*out;
char	*ban;
int	npages = 1;
int lineno;

main(argc, argv)
char **argv;
{
	register int i;
	int tbuf[3];

	if(DEBUG)printf("in prt1200\n");
	tmode.sgispd = ISPEED;
	tmode.sgospd = OSPEED;
	tmode.sgerase = ERASE;
	tmode.sgkill = KILL;
	tmode.sgflag = MODE;

	if ((out = fopen(DHLINE, "w")) == NULL) {
		fprintf(stderr, "Can't open printer\n");
		exit(1);
	}
	if(DEBUG)printf("dh line open\n");
		stty(out,&tmode);
		stty(fileno(out),&tmode);
	if (argc > 2 && argv[1][0]=='-' && argv[1][1]=='b') {
		argc -= 2;
		banner(out, ban = argv[2]);
		argv += 2;
	}
	if (argc<=1)
		anydone |= send();
	else while (argc>1) {
		if ((in = fopen(argv[1], "r")) == NULL) {
			fprintf(stderr, "Can't find %s\n", argv[1]);
			argv++;
			argc--;
			anydone |= 01;
			continue;
		}
		anydone |= send();
		argc--;
		argv++;
		fclose(in);
		fprintf(out, "\014");
	}
	if (anydone==0)
		exit(1);
	fprintf(out, "\004");
	if (ferror(out)) {
		fprintf(out, "Printer IO error\n");
		exit(1);
	}
	fclose(out);
	if (ban && access(ACCT, 02)>=0
	 && (out = fopen(ACCT, "a"))!=NULL) {
		fprintf(out, "%4d %s\n", npages, ban);
	}
	return(0);
}

send()
{
	register nskipped;

	lineno = 0;
	nskipped = 0;
	while (getline()) {
		if (lineno==0 && linebuf[0]==0 && nskipped<3) {
			nskipped ++;
			continue;
		}
		if (lineno >= EJLINE) {
			nskipped = 0;
			putline(1);
			lineno = 0;
		} else {
			putline(0);
			lineno++;
		}
	}
	if (lineno>0)
		npages++;
	return(1);
}

getline()
{
	register col, maxcol, c;

	ov = 0;
	for (col=0; col<LINELN; col++) {
		linebuf[col] = ' ';
		ovbuf[col] = ' ';
	}
	col = STARTCOL;
	maxcol = 0;
	for (;; (c = getc(in))>=0) switch (c) {
	case -1:
		return(0);

	default:
		if (c>=' ') {
			if (col < LINELN) {
				if (linebuf[col]=='_') {
					ov++;
					ovbuf[col] = '_';
				}
				linebuf[col++] = c;
				if (col > maxcol)
					maxcol = col;
			}
		}
		continue;

	case '\014':
		lineno = EJLINE;
		continue;
	case ' ':
		col++;
		continue;

	case '\t':
		col = (col|07) + 1;
		if (col>maxcol)
			maxcol = col;
		continue;

	case '\r':
		col = 0;
		continue;

	case '_':
		if (col>=LINELN) {
			col++;
			continue;
		}
		if (linebuf[col]!=' ') {
			ovbuf[col] = '_';
			ov++;
		} else
			linebuf[col] = c;
		col++;
		if (col>maxcol)
			maxcol = col;
		continue;

	case '\n':
		if (maxcol>=LINELN)
			maxcol = LINELN;
		linebuf[maxcol] = 0;
		ovbuf[maxcol] = 0;
		return(1);

	case '\b':
		if (col>0)
			col--;
		continue;
	}
}

putline(ff)
{
	register char *lp;
	register c, i;
	extern errno;

	errno = 0;
	lp = linebuf;
	while (c = *lp++)
		putc(c, out);
	if (ov) {
		putc('\r', out);
		fflush(out);
		lp = ovbuf;
		while(c = *lp++)
			putc(c, out);
		fflush(out);
	}
	if (ff) {
		putc('\014', out);
		npages++;
	} else
		putc('\n', out);
	if (ferror(out)) {
		printf("Printer IO error\n");
		exit(1);
	}
}
