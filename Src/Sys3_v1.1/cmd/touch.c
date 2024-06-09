#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

struct	stat	stbuf;
int	status;

char	*tzname[2];
char	*cbp;
int	dmsize[];
long	timbuf;
extern int *localtime();
long	time();

struct	{
	int	hi;
	int	lo;
};

gtime()
{
	register int i;
	register int y, t;
	int d, h, m;
	long nt;

	tzset();

	t = gpair();
	if(t<1 || t>12)
		return(1);
	d = gpair();
	if(d<1 || d>31)
		return(1);
	h = gpair();
	if(h == 24) {
		h = 0;
		d++;
	}
	m = gpair();
	if(m<0 || m>59)
		return(1);
	y = gpair();
	if (y<0) {
		time(&nt);
		y = localtime(&nt)[5];
	}
	if (*cbp == 'p')
		h += 12;
	if (h<0 || h>23)
		return(1);
	timbuf = 0;
	y += 1900;
	for(i=1970; i<y; i++)
		timbuf += dysize(i);
	/* Leap year */
	if (dysize(y)==366 && t >= 3)
		timbuf += 1;
	while(--t)
		timbuf += dmsize[t-1];
	timbuf += (d-1);
	timbuf *= 24;
	timbuf += h;
	timbuf *= 60;
	timbuf += m;
	timbuf *= 60;
	return(0);
}


gpair()
{
	register int c, d;
	register char *cp;

	cp = cbp;
	if(*cp == 0)
		return(-1);
	c = (*cp++ - '0') * 10;
	if (c<0 || c>100)
		return(-1);
	if(*cp == 0)
		return(-1);
	if ((d = *cp++ - '0') < 0 || d > 9)
		return(-1);
	cbp = cp;
	return (c+d);
}
main(argc, argv)
char *argv[];
{
	register c;
	extern	 long	timezone;
	struct utimbuf {
		long actime;
		long modtime;
	} times;

	int mflg=1, aflg=1, cflg=0, nflg=0, errflg=0, optc, fd;
	extern char *optarg;
	extern int optind;

	while ((optc=getopt(argc, argv, "amc")) != EOF)
		switch(optc) {
		case 'm':
			mflg++;
			aflg--;
			break;
		case 'a':
			aflg++;
			mflg--;
			break;
		case 'c':
			cflg++;
			break;
		case '?':
			errflg++;
		}

	if(((argc-optind) < 1) || errflg) {
		fprintf(stderr, "usage: touch [-amc] [date] file ...\n");
		exit(2);
	}
	status = 0;
	if(!isnumber(argv[optind]))
		if((aflg <= 0) || (mflg <= 0))
			timbuf = time((long *) 0);
		else
			nflg++;
	else {
		cbp = (char *)argv[optind++];
		if(gtime()) {
			fprintf(stderr, "date: bad conversion\n");
			exit(2);
		}
		timbuf += timezone;
		if (localtime(&timbuf)[8])
			timbuf += -1*60*60;
	}
	for(c=optind; c<argc; c++) {
		if(stat(argv[c], &stbuf)) {
			if(cflg) {
				status++;
				continue;
			}
			else if ((fd = creat (argv[c], 0666)) < 0) {
				fprintf(stderr, "touch: %s cannot create\n", argv[c]);
				status++;
				continue;
			}
			else {
				close(fd);
				if(stat(argv[c], &stbuf)) {
					fprintf(stderr,"touch: %s cannot stat\n",argv[c]);
					status++;
					continue;
				}
			}
		}

		times.actime = times.modtime = timbuf;
		if (mflg <= 0)
			times.modtime = stbuf.st_mtime;
		if (aflg <= 0)
			times.actime = stbuf.st_atime;

		if(utime(argv[c], nflg ? NULL : &times)) {
			fprintf(stderr,"touch: cannot change times on %s\n",argv[c]);
			status++;
			continue;
		}
	}
	exit(status);
}

isnumber(s)
char *s;
{
	register c;

	while(c = *s++)
		if(!isdigit(c))
			return(0);
	return(1);
}

