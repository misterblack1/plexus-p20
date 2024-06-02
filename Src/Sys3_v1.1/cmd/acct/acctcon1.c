/*
 *	acctcon1 [-p] [-t] [-l file] [-o file] <wtmp-file >ctmp-file
 *	-p	print input only, no processing
 *	-t	test mode: use latest time found in input, rather than
 *		current time when computing times of lines still on
 *		(only way to get repeatable data from old files)
 *	-l file	causes output of line usage summary
 *	-o file	causes first/last/reboots report to be written to file
 *	reads input (normally /usr/adm/wtmp), produces
 *	list of sessions, sorted by ending time in ctmp.h/ascii format
 *	TSIZE is max # distinct ttys
 */

#include <sys/types.h>
#include "acctdef.h"
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <utmp.h>
#include "ctmp.h"

#define PREDATE	'|'
#define POSTDATE	'{'
#define BOOTSHUT	'~'

int	tsize	= -1;
struct  utmp	wb;
struct	ctmp	cb;

struct tbuf {
	char	tline[LSZ];	/* dev/* */
	char	tname[NSZ];	/* user name */
	time_t	ttime;		/* start time */
	dev_t	tdev;		/* device */
	int	tlsess;		/* # complete sessions */
	int	tlon;		/* # times on (non-null name) */
	int	tloff;		/* # times off (null name) */
	long	ttotal;		/* total time used on this line */
} tbuf[TSIZE];

#define NSYS	10
int	nsys;
struct sys {
	char	sname[NSZ];	/* system name for reboot report */
	char	snum;		/* number of times encountered */
} sy[NSYS];

time_t	datetime;
time_t	firstime;
time_t	lastime;
int	ndates;
int	exitcode;
char	*report	= NULL;
char	*replin = NULL;
int	printonly;
int	tflag;

char	*ctime();
long	ftell();
uid_t	namtouid();
dev_t	lintodev();

main(argc, argv) 
char **argv;
{

	while (--argc > 0 && **++argv == '-')
		switch(*++*argv) {
		case 'l':
			if (--argc > 0)
				replin = *++argv;
			continue;
		case 'o':
			if (--argc > 0)
				report = *++argv;
			continue;
		case 'p':
			printonly++;
			continue;
		case 't':
			tflag++;
			continue;
		}

	if (printonly) {
		while (wread()) {
			if (valid()) {
				printf("%.8s\t%.8s\t%lu",
					wb.ut_line,
					wb.ut_name,
					wb.ut_time);
				printf("\t%s", ctime(&wb.ut_time));
			} else
				fixup(stdout);
			
		}
		exit(exitcode);
	}

	while (wread()) {
		if (firstime == 0)
			firstime = wb.ut_time;
		if (valid())
			loop();
		else
			fixup(stderr);
	}
	wb.ut_name[0] = '\0';
	wb.ut_line[0] = BOOTSHUT;
	if (tflag)
		wb.ut_time = lastime;
	else
		time(&wb.ut_time);
	loop();
	if (report != NULL)
		printrep();
	if (replin != NULL)
		printlin();
	exit(exitcode);
}

#ifdef	V6
#include "utmp6.h"
/*
 * wread: fill in wb, with V6 conversion
 */
wread()
{
	struct utmp6 wb6;

	if (fread(&wb6, sizeof(wb6), 1, stdin) != 1)
		return(0);
/* NOTE: further validation (of fill?) may be needed; see old c1acct pgm. */
	CPYN(wb.ut_name, wb6.ut_name6);
	if (wb6.ut_tty == 'x' || wb6.ut_tty == BOOTSHUT) {
		CPYN(wb6.ut_line, "");
		wb.ut_line[0] = BOOTSHUT;
	} else if ( wb6.ut_tty == PREDATE || wb6.ut_tty == POSTDATE ) {
		CPYN(wb6.ut_line, "");
		wb6.ut_line[0] = wb6.ut_tty;
	} else {
		strncpy(wb.ut_line, "tty", 3);
		wb.ut_line[3] = wb6.ut_tty;
	}
	wb.ut_time = wb6.ut_time6;
	return(1);
}
#else
wread()
{
	return(fread(&wb, sizeof(wb), 1, stdin) == 1);
}
#endif

/*
 * valid: check input wtmp record, return 1 if looks OK
 * NOTE: this may falsely return 0 if local system has tty names
 * starting with characters other than [tcu]
 */
valid()
{
	register i, c, fl;

	fl = 0;
	for (i = 0; i < NSZ; i++) {
		c = wb.ut_name[i];
		if (isdigit(c) || isalpha(c) || c == '$' ) {
			if (fl)
				return(0);
		} else if (c == ' ' || c == '\0') {
			fl++;
			wb.ut_name[i] = '\0';
		} else
			return(0);
	}

	switch(wb.ut_line[0]) {
	case 't':
	case 'c':
	case 'u':
	case PREDATE:
	case POSTDATE:
	case BOOTSHUT:
		return(1);
	}
	return(0);
}

/*
 *	fixup assumes that V6 wtmp (16 bytes long) is mixed in with
 *	V7 records (20 bytes each)
 */
fixup(stream)
register FILE *stream;
{
	fprintf(stream, "bad wtmp: offset %lu.\n", ftell(stdin)-sizeof(wb));
	fprintf(stream, "bad record is:  %.8s\t%.8s\t%lu",
		wb.ut_line,
		wb.ut_name,
		wb.ut_time);
	fprintf( stream, "\t%s", ctime(&wb.ut_time));
	fseek(stdin, (long)-4, 1);
	exitcode = 1;
}

loop()
{
	register i;
	register struct tbuf *tp;
	char c;

	if(wb.ut_line[0] == PREDATE) {
		datetime = wb.ut_time;
		return;
	}
	if(wb.ut_line[0] == POSTDATE) {
		if(datetime == 0)
			return;
		for (tp = tbuf; tp <= &tbuf[tsize]; tp++)
			tp->ttime += wb.ut_time-datetime;
		datetime = 0;
		ndates++;
		return;
	}
	lastime = wb.ut_time;
	if (wb.ut_line[0] == BOOTSHUT) {
		c = wb.ut_name[0];
		wb.ut_name[0] = '\0';
		upall();
		if ((wb.ut_name[0] = c) != '\0')
			bootshut();
		return;
	}
	update(&tbuf[iline()]);
}

/*
 * bootshut: record reboot (or shutdown)
 * bump count, looking up wb.ut_name in sy table
 */
bootshut()
{
	register i;

	for (i = 0; i < nsys && !EQN(wb.ut_name, sy[i].sname); i++)
		;
	if (i >= nsys) {
		if (++nsys > NSYS)
			return;
		CPYN(sy[i].sname, wb.ut_name);
	}
	sy[i].snum++;
}

/*
 * iline: look up/enter current line name in tbuf, return index
 * (used to avoid system dependencies on naming)
 */
iline()
{
	register i;

	for (i = 0; i <= tsize; i++)
		if (EQN(wb.ut_line, tbuf[i].tline))
			return(i);
	if (++tsize >= TSIZE) {
		fprintf(stderr, "acctcon1: RECOMPILE WITH LARGER TSIZE\n");
		exit(2);
	}

	CPYN(tbuf[tsize].tline, wb.ut_line);
	tbuf[tsize].tdev = lintodev(wb.ut_line);
	return(tsize);
}

upall()
{
	register struct tbuf *tp;

	for (tp = tbuf; tp <= &tbuf[tsize]; tp++)
		update(tp);
}

/*
 * update tbuf with new time, write ctmp record for end of session
 */
update(tp)
struct tbuf *tp;
{
	long told, tnew;

	told = tp->ttime;
	tnew = wb.ut_time;
	if (told > tnew) {
		fprintf(stderr, "acctcon1: bad times: old: %s", ctime(&told));
		fprintf(stderr, "new: %s", ctime(&tnew));
		exitcode = 1;
		return;
	}
	tp->ttime = tnew;
	if (tp->tname[0] != '\0') {
		tp->tlsess++;
		CPYN(cb.ct_name, tp->tname);
		tp->tname[0] = '\0';
		cb.ct_start = told;
		pnpsplit(cb.ct_start, tnew-told, cb.ct_con);
		cb.ct_tty = tp->tdev;
		cb.ct_uid = namtouid(cb.ct_name);
		prctmp(&cb);
		tp->ttotal += tnew-told;
	}
	if (wb.ut_name[0] != '\0')
		tp->tlon++;
	else
		tp->tloff++;
	CPYN(tp->tname, wb.ut_name);
}

printrep()
{
	register i;

	freopen(report, "w", stdout);
	printf("from %s", ctime(&firstime));
	printf("to   %s", ctime(&lastime));
	if (ndates)
		printf("%d date changes\n", ndates);
	for (i = 0; i < nsys; i++)
		printf("%d\t%.8s\n", sy[i].snum, sy[i].sname);
}

/*
 *	print summary of line usage
 *	accuracy only guaranteed for wtmp file started fresh
 */
printlin()
{
	register struct tbuf *tp;
	double timet, timei;
	double ttime;
	int tsess, ton, toff;

	freopen(replin, "w", stdout);
	ttime = 0.0;
	tsess = ton = toff = 0;
	timet = MINS(lastime-firstime);
	printf("TOTAL DURATION IS %.0f MINUTES\n", timet);
	printf("LINE\tMINUTES\tPERCENT\t# SESS\t# ON\t# OFF\n");
	for (tp = tbuf; tp <= &tbuf[tsize]; tp++) {
		timei = MINS(tp->ttotal);
		ttime += timei;
		tsess += tp->tlsess;
		ton += tp->tlon;
		toff += tp->tloff;
		printf("%.8s\t%.0f\t%.0f\t%d\t%d\t%d\n",
			tp->tline,
			timei,
			(timet > 0.)? 100*timei/timet : 0.,
			tp->tlsess,
			tp->tlon,
			tp->tloff);
	}
	printf("TOTALS\t%.0f\t--\t%d\t%d\t%d\n", ttime, tsess, ton, toff);
}

prctmp(t)
register struct ctmp *t;
{

	printf("%u\t%u\t%.8s\t%lu\t%lu\t%lu",
		t->ct_tty,
		t->ct_uid,
		t->ct_name,
		t->ct_con[0],
		t->ct_con[1],
		t->ct_start);
	printf("\t%s", ctime(&t->ct_start));
}
