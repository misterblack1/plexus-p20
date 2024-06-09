#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>

#define	LISTS	512

#define HOUR	3600

#define	EXACT	123
#define	ANY	124
#define	LIST	125
#define	RANGE	126
#define	EOS	127
char	crontab[]	= "/usr/lib/crontab";
char	cronlog[]	= "/usr/lib/cronlog";
time_t	itime;
struct	tm *loct;
struct	tm *localtime();
char	*malloc();
char	*realloc();
char	*ctime();
int	flag;
char	*list;
unsigned listsize;
int	rfork;
char	lorange[] = { 0,0,1,1,0 }, hirange[] = { 59,23,31,12,6 };

main()
{
	register char *cp;
	char *cmp();
	time_t filetime = 0;

	setuid(0);
	if (rfork = fork())
		if(rfork == -1) {
			write(2, "cron: cannot fork\n",18);
			exit(2);
		} else
			exit(0);
	chdir("/");
	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	time(&itime);
	itime -= localtime(&itime)->tm_sec;

	for (;; itime+=60, slp()) {
		struct stat cstat;
		if(access(cronlog,2)==0) {
			freopen(crontab, "r", stdin);
			freopen(cronlog, "a", stdout);
			setbuf(stdout, NULL);
			close(fileno(stderr));
			dup(1);
			fclose(stdin);
		}
		else {
			freopen("/dev/null", "r", stdout);
			freopen("/dev/null", "r", stderr);
		}

		if (itime%HOUR == 0)
			puts(""),puts(ctime(&itime));
		if (stat(crontab, &cstat) == -1)
			continue;
		if (cstat.st_mtime > filetime) {
			filetime = cstat.st_mtime;
			puts(""),fputs(ctime(&itime), stdout),puts("crontab read");
			init();
		}
		loct = localtime(&itime);
		loct->tm_mon++;		 /* 1-12 for month */
		for(cp = list; *cp != EOS;) {
			flag = 0;
			cp = cmp(cp, loct->tm_min);
			cp = cmp(cp, loct->tm_hour);
			cp = cmp(cp, loct->tm_mday);
			cp = cmp(cp, loct->tm_mon);
			cp = cmp(cp, loct->tm_wday);
			if(flag == 0) {
				slp();
				ex(cp);
			}
			while(*cp++ != 0)
				;
		}
	}
}

char *
cmp(p, v)
char *p;
{
	register char *cp;

	cp = p;
	switch(*cp++) {

	case EXACT:
		if (*cp++ != v)
			flag++;
		return(cp);

	case ANY:
		return(cp);

	case LIST:
		while(*cp != LIST)
			if(*cp++ == v) {
				while(*cp++ != LIST)
					;
				return(cp);
			}
		flag++;
		return(cp+1);

	case RANGE:
		if(cp[0] < cp[1]) {
			if(!(cp[0]<=v && cp[1]>=v))
				++flag;
		} else if(!(v>=cp[0] || v<=cp[1]))
			++flag;
		return(cp+2);
	}
	if(cp[-1] != v)
		flag++;
	return(cp);
}

slp()
{
	register i;
	time_t t;

	time(&t);
	i = itime - t;
	if(i > 0)
		sleep(i);
}

ex(s)
char *s;
{
	int st;

	fputs(s, stdout);
again:
	if(rfork = fork()) {
		if(rfork == -1) {
			sleep(20);
			goto again;
		}
		wait(&st);
		return;
	}
	if(rfork = fork())
		exit(0);
	if(rfork == -1)
		exit(2);
	freopen("/dev/null", "r", stdin);
	execl("/bin/sh", "sh", "-c", s, 0);
	exit(0);
}

init()
{
	register i, c;
	register char *cp;
	register char *ocp, *cp2;
	register int n;

	freopen(crontab, "r", stdin);
	if (list) {
		free(list);
		list = realloc(list, LISTS);
	} else
		list = malloc(LISTS);
	listsize = LISTS;
	cp = list;

loop:
	if(cp > list+listsize-256) {
		char *olist;
		listsize += LISTS;
		olist = list;
		free(list);
		list = realloc(list, listsize);
		cp = list + (cp - olist);
	}
	ocp = cp;
	for(i=0;; i++) {
		do
			c = getchar();
		while(c == ' ' || c == '\t')
			;
		if(c == EOF || c == '\n')
			goto ignore;
		if(i == 5)
			break;
		if(c == '*') {
			*cp++ = ANY;
			continue;
		}
		if ((n = number(c, lorange[i], hirange[i])) < 0)
			goto ignore;
		c = getchar();
		if(c == ',')
			goto mlist;
		if(c == '-')
			goto mrange;
		if(c != '\t' && c != ' ')
			goto ignore;
		*cp++ = EXACT;
		*cp++ = n;
		continue;

	mlist:
		*cp++ = LIST;
		*cp++ = n;
		do {
			if ((n = number(getchar(), lorange[i], hirange[i])) < 0)
				goto ignore;
			*cp++ = n;
			c = getchar();
		} while (c==',');
		if(c != '\t' && c != ' ')
			goto ignore;
		*cp++ = LIST;
		continue;

	mrange:
		*cp++ = RANGE;
		*cp++ = n;
		if ((n = number(getchar(), lorange[i], hirange[i])) < 0)
			goto ignore;
		c = getchar();
		if(c != '\t' && c != ' ')
			goto ignore;
		*cp++ = n;
	}

	i = 0;
	while(c != '\n') {
		if(c == EOF)
			goto ignore;
		if(c == '%') {
			if (i == 0) {
				i++;
				for (cp2 = "<< __GNOME__"; *cp++ = *cp2++;)
						;
				cp--;
			}
			c = '\n';
		}
		*cp++ = c;
		c = getchar();
	}
	*cp++ = '\n';
	if (i)
		for (cp2 = "__GNOME__\n"; *cp++ = *cp2++;)
			;
	else
		*cp++ = 0;
	goto loop;

ignore:
	cp = ocp;
	while(c != '\n') {
		if(c == EOF) {
			*cp++ = EOS;
			*cp++ = EOS;
			fclose(stdin);
			return;
		}
		c = getchar();
	}
	goto loop;
}

number(c, lowv, highv)
register c;
{
	register n = 0;

	while (isdigit(c)) {
		n = n*10 + c - '0';
		c = getchar();
	}
	ungetc(c, stdin);
	if (n<lowv || n>highv) {
		puts("value range error");
		return -1;
	}
	return(n);
}
