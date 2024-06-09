/*
**	date - with format capabilities
*/

#include	<utmp.h>
#include        <stdio.h>
#include	<time.h>

#define	MONTH	itoa(tim->tm_mon+1,cp,2)
#define	DAY	itoa(tim->tm_mday,cp,2)
#define	YEAR	itoa(tim->tm_year,cp,2)
#define	HOUR	itoa(tim->tm_hour,cp,2)
#define	MINUTE	itoa(tim->tm_min,cp,2)
#define	SECOND	itoa(tim->tm_sec,cp,2)
#define	JULIAN	itoa(tim->tm_yday+1,cp,3)
#define	WEEKDAY	itoa(tim->tm_wday,cp,1)
#define	MODHOUR	itoa(h,cp,2)

char	month[12][4] = {
	"Jan","Feb","Mar","Apr",
	"May","Jun","Jul","Aug",
	"Sep","Oct","Nov","Dec"
};

char	days[7][4] = {
	"Sun","Mon","Tue","Wed",
	"Thu","Fri","Sat"
};

char	*tzname[2];
char	*itoa();
char	cbuf[];
char	*cbp;
int	dmsize[];
long	timbuf;

struct	utmp	wtmp[2] = { {"|", "", 0}, {"{", "", 0}};

struct	{
	int	hi;
	int	lo;
};

main(argc, argv)
int	argc;
int	**argv;
{
	register char	*aptr, *cp, c;
	struct	tm	*localtime();
	extern	 long	timezone;
	int	 h, hflag, i, tfailed, wf;
	long	tbuf;
	struct	tm  *tim;
	char	 buf[200], *tzn;

	tfailed = 0;
	if(argc > 1) {
		cbp = (char *)argv[1];

		if(*cbp == '+')	{
			hflag = 0;
			for(cp=buf;cp< &buf[200];)
				*cp++ = '\0';
			time(&tbuf);
			tim = localtime(&tbuf);
			aptr = (char *)argv[1];
			aptr++;
			cp = buf;
			while(c = *aptr++) {
			if(c == '%')
			switch(*aptr++) {
			case '%':
				*cp++ = '%';
				continue;
			case 'n':
				*cp++ = '\n';
				continue;
			case 't':
				*cp++ = '\t';
				continue;
			case 'm' :
				cp = MONTH;
				continue;
			case 'd':
				cp = DAY;
				continue;
			case 'y' :
				cp = YEAR;
				continue;
			case 'D':
				cp = MONTH;
				*cp++ = '/';
				cp = DAY;
				*cp++ = '/';
				cp = YEAR;
				continue;
			case 'H':
				cp = HOUR;
				continue;
			case 'M':
				cp = MINUTE;
				continue;
			case 'S':
				cp = SECOND;
				continue;
			case 'T':
				cp = HOUR;
				*cp++ = ':';
				cp = MINUTE;
				*cp++ = ':';
				cp = SECOND;
				continue;
			case 'j':
				cp = JULIAN;
				continue;
			case 'w':
				cp = WEEKDAY;
				continue;
			case 'r':
				if((h = tim->tm_hour) >= 12)
					hflag++;
				if((h %= 12) == 0)
					h = 12;
				cp = MODHOUR;
				*cp++ = ':';
				cp = MINUTE;
				*cp++ = ':';
				cp = SECOND;
				*cp++ = ' ';
				if(hflag)
					*cp++ = 'P';
				else *cp++ = 'A';
				*cp++ = 'M';
				continue;
			case 'h':
				for(i=0; i<3; i++)
					*cp++ = month[tim->tm_mon][i];
				continue;
			case 'a':
				for(i=0; i<3; i++)
					*cp++ = days[tim->tm_wday][i];
				continue;
			default:
				fprintf(stderr, "date: bad format character - %c\n", *--aptr);
				exit(2);
			}	/* endsw */
			*cp++ = c;
			}	/* endwh */

			*cp = '\n';
			write(1,buf,(cp - &buf[0]) + 1);
			exit(0);
		}

		if(*cbp == '-') {
			fprintf(stderr,"date: no TOY clock\n");
			exit(2);
		}

		if(gtime()) {
			fprintf(stderr, "date: bad conversion\n");
			exit(2);
		}

	/* convert to Greenwich time, on assumption of Standard time. */
		timbuf += timezone;

	/* Now fix up to local daylight time. */
		if (localtime(&timbuf)->tm_isdst)
			timbuf += -1*60*60;

		time(&wtmp[0].ut_time);

		if(stime(&timbuf) < 0) {
			tfailed++;
			fprintf(stderr, "date: no permission\n");
		} else if ((wf = open("/usr/adm/wtmp", 1)) >= 0) {
			time(&wtmp[1].ut_time);
			lseek(wf, 0L, 2);
			write(wf, (char *)wtmp, sizeof(wtmp));
		}

	}
	if (tfailed==0)
		time(&timbuf);
	cbp = cbuf;
	ctime(&timbuf);
	write(1, cbuf, 20);
	tzn = tzname[localtime(&timbuf)->tm_isdst];
	if (tzn)
		write(1, tzn, 3);
	write(1, cbuf+19, 6);
	exit(tfailed?2:0);
}

gtime()
{
	register int i;
	register int y, t;
	int d, h, m;
	struct	tm *localtime();
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
		y = localtime(&nt)->tm_year;
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

char *
itoa(i,ptr,dig)
register  int	i;
register  int	dig;
register  char	*ptr;
{
	switch(dig)	{
		case 3:
			*ptr++ = i/100 + '0';
			i = i - i / 100 * 100;
		case 2:
			*ptr++ = i / 10 + '0';
		case 1:	
			*ptr++ = i % 10 + '0';
	}
	return(ptr);
}
