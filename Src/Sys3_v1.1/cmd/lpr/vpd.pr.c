/*
 * vpr -- Versatek printer filter
 */

#include <stdio.h>
#include <sys/ioctl.h>

#define	LINELN	132
#define	EJLINE	62

int	anydone;
char	linebuf[LINELN+2];
int	sppmode	= 0400;
int	pltmode	= 0200;
int	prtmode	= 0100;
int	ov;
char	ovbuf[2*LINELN];
FILE	*in	= stdin;
FILE	*out;
char	*ban;
int	npages	= 1;
int	lineno;
char	*ctime();
char	eofsw = 0;

main(argc, argv)
char **argv;
{

	if ((out = fopen("/dev/vp", "w")) == NULL) {
		fprintf(stderr, "Can't open printer\n");
		exit(1);
	}
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
		fclose(in);
		argv++;
		if(--argc > 1)
			fprintf(out, "\014");
	}
	if (anydone==0)
		exit(1);
	if (ferror(out)) {
		fprintf(out, "Printer IO error\n");
		exit(1);
	}
	fclose(out);
	if (ban && access("/usr/adm/vpacct", 02)>=0
	 && (out = fopen("/usr/adm/vpacct", "a"))!=NULL) {
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

	if(eofsw){
		eofsw = 0;
		return(0);
	}
	ov = 0;
	for (col=0; col<LINELN; col++) {
		linebuf[col] = ' ';
		ovbuf[2*col] = ovbuf[2*col+1] = 0;
	}
	col = 8;
	maxcol = 0;
	for (;;) switch (c = getc(in)) {

	case EOF:
		eofsw = 1;
	case '\n':
		if (maxcol>=LINELN)
			maxcol = LINELN;
		linebuf[maxcol] = 0;
		return(1);

	default:
		if (c>=' ') {
			if (col < LINELN) {
				if (linebuf[col]=='_') {
					ov++;
					ovbuf[2*col] = 0377;
					ovbuf[2*col+1] = 0377;
				}
				linebuf[col++] = c;
				if (col > maxcol)
					maxcol = col;
			}
		}
		continue;

	case '\f':
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
			ovbuf[2*col] = 0377;
			ovbuf[2*col+1] = 0377;
			ov++;
		} else
			linebuf[col] = c;
		col++;
		if (col>maxcol)
			maxcol = col;
		continue;

	case '\b':
		if (col>0)
			col--;
		continue;
	}
}

putline(ff)
{
	register char *lp;
	register c;
	extern errno;

	errno = 0;
	lp = linebuf;
	while (c = *lp++)
		putc(c, out);
	if (ov) {
		putc('\n', out);
		fflush(out);
		ioctl(fileno(out), LIOCSETS, pltmode);
		for (lp=ovbuf; lp < &ovbuf[2*LINELN]; )
			putc(*lp++, out);
		fflush(out);
		ioctl(fileno(out), LIOCSETS, prtmode);
	}
	if (ff) {
		putc('\014', out);
		npages++;
	} else if (ov==0)
		putc('\n', out);
	if (ferror(out)) {
		printf("Printer IO error\n");
		exit(1);
	}
}
