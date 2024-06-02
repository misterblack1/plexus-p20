/*
	L. R. Satz 5/18/79
	acctcom [-bhkmrtv] [-l name] [-u uid] [-s time] [-e time] 
		[-g gid] [-n pattern] [-C cpuvalue] [-H hogvalue]
		[-O sysvalue] [file...]
 *	usually used to read /usr/adm/pacct
 *	reads file... (acct.h format), printing selected records
 *	reads files in order given, but may read individual files
 *	forward or backward; reads std. input if no files given
 *	and std. input is not a tty.
 *	If std. input is a tty reads /usr/adm/pacct.
 *	-b	read files backward (for lastcom effect)
 *	-d mo/dy all time args following occur on given
		month and day rather than last 24 hours
 *	-g	group	print only records with specified gid
 *	-h	show "hog factor (cpu/elapsed)
 *	-i	show I/O
 *	-k	show kcore-minutes
 *	-m	show mean core size (the default)
 *	-r	show cpu factor (user/(sys+user))
 *	-t	show separate sys and user cpu times
 *	-v	verbose (turns off initial 2-line heading)
 *	-l name	print only records with specified linename (/dev/linename)
 *	-u user	print only records with specified uid (or name)
	 if name given is ? prints processes without login
	 if name is # prints proceses using super-user privilege
 *	-s time	print only records of processes active on or after time
 *	-e time	print only records of processes active on or before time
 *	if interrupted, shows line it was looking at
 *	-C prints processes exceeding specified CPU time (sec.)
 *	-H print processes with hog factors exceeding cut-off.
 *	-I print processes transferring more characters than cut-off
 *	-O print proceses with system time exceeding specified value (sec.)
 */

#include <sys/types.h>
#include "acctdef.h"
#include <grp.h>
# include "macros.h"
#include <stdio.h>
#include <sys/acct.h>
#include <pwd.h>

#define MYKIND(flag)   ((flag & ACCTF) == 0)
#define SU(flag)   ((flag & ASU) == ASU)
#define PACCT "/usr/adm/pacct"
#define MEANSIZE	01
#define KCOREMIN	02
#define HOGFACTOR	04
#define	SEPTIME		010
#define	CPUFACTOR	020
#define IORW		040

struct	acct ab;
long	elapsed, sys, user, cpu, mem, io, rw;
char	command_name[16];
time_t	end_time;
int	backward;
int	flag_field,average,quiet;
double	cpucut,realcut,memcut,syscut,usercut,hogcut,factorcut,iocut;
double	realtot,cputot,usertot,systot,kcoretot,iotot,rwtot;
long cmdcount;
int	option;
int	verbose = 1;
dev_t	linedev	= -1;
uid_t	uidval;
uid_t	gidval; /*should create typedef for group*/
int	uidflag,gidflag;
int noid ; /*user doesn't have login on this machine*/
int su_user;
time_t	tstart	= 0;
time_t	tend	= 0;
int	nfiles;
int	sig2;
char	obuf[BUFSIZ];
char	*cname = NULL; /* command name pattern to match*/

long	ftell();
char	*ctime();
char	*devtolin();
long	convtime();
uid_t	namtouid();
char	*uidtonam();
struct group *getgrname(),*grp_ptr;
dev_t	lintodev();
struct	tm *localtime();
long	tmsecs();
int	catch2();
long	expand();
int	isdevnull();

main(argc, argv)
char **argv;
{

	"@(#)acctcom.c	1.6";
	if (signal(2, 1) != 1)
		signal(2, catch2);
	setbuf(stdout,obuf);
	while (--argc > 0) {
		if (**++argv == '-')
			switch(*++*argv) {
			case 'C':
				if(--argc>0) {
					sscanf(*++argv,"%f",&cpucut);
				};
				continue;
			case 'O':
				if(--argc>0) {
					sscanf(*++argv,"%f",&syscut);
				};
				continue;
			case 'H':
				if(--argc) {
					sscanf(*++argv,"%f",&hogcut);
				};
				continue;
			case 'I':
				if(--argc) {
					sscanf(*++argv,"%f",&iocut);
				};
				continue;
			case 'a':
				average++;
				continue;
			case 'b':
				backward++;
				continue;
			case 'g':
				if(--argc > 0) {
					if(sscanf(*++argv,"%d",&gidval)!=1)
						if((grp_ptr=getgrname(*argv))==0)
							fprintf(stderr,"No such group\n");
						else
							gidval=grp_ptr->gr_gid;
					gidflag++;
				};
				continue;
			case 'h':
				option |= HOGFACTOR;
				continue;
			case 'i':
				option |= IORW;
				continue;
			case 'k':
				option |= KCOREMIN;
				continue;
			case 'm':
				option |= MEANSIZE;
				continue;
			case 'n':
				if(--argc>0)
					cname=(char *)cmset(*++argv);
				continue;
			case 't':
				option |= SEPTIME;
				continue;
			case 'r':
				option |= CPUFACTOR;
				continue;
			case 'v':
				verbose=0;
				continue;
			case 'l':
				if (--argc > 0)
					linedev = lintodev(*++argv);
				continue;
			case 'u':
				if (--argc > 0) {
					++argv;
					if(**argv == '?') {
						noid++;
						continue;
					};
					if(**argv == '#') {
						su_user++;
						continue;
					};
					uidflag++;
					if (sscanf(*argv, "%d", &uidval) != 1)
						uidval = namtouid(*argv);
					continue;
				}
			case 'd':
				if (--argc > 0)
					convday(*++argv);
				continue;
			case 'q':
				quiet++;
				verbose=0;
				average++;
				continue;
			case 's':
				if (--argc > 0) {
					tstart = convtime(*++argv);
					printf("START: %s", ctime(&tstart));
				}
				continue;
			case 'f':
				flag_field++;
				continue;
			case 'e':
				if (--argc > 0) {
					tend = convtime(*++argv);
					printf("END:   %s", ctime(&tend));
				}
				continue;
			}
		else {
			nfiles++;
			dofile(*argv);
		}
	}
	if(nfiles==0) {  
		if(isatty(0))
			dofile(PACCT);
		else if(isdevnull())
			dofile(PACCT);
		else
			dofile(NULL);
	};
	doexit(0);
}

dofile(fname)
char *fname;
{

	if (fname != NULL)
		if (freopen(fname, "r", stdin) == NULL) {
			fprintf(stderr,  "acctcom: cannot open %s\n", fname);
			return;
		}
	if (backward)
		fseek(stdin, (long)sizeof(ab), 2);

	while (aread(&ab)) {
		if (!MYKIND(ab.ac_flag))
			continue;
		if(su_user && !SU(ab.ac_flag))
			continue;
		elapsed = expand(ab.ac_etime);
		if(elapsed == 0)
			elapsed++;
		sys = expand(ab.ac_stime);
		user = expand(ab.ac_utime);
		cpu = sys + user;
		if(cpu == 0)
			cpu = 1;
		mem = expand(ab.ac_mem);
		substr(ab.ac_comm,command_name,0,8);
		end_time = ab.ac_btime + SECS(elapsed +30);
		io=expand(ab.ac_io);
		rw=expand(ab.ac_rw);
		if(cpucut != 0 && cpucut >=  SECS(cpu))
			continue;
		if(syscut != 0 && syscut >=  SECS(sys))
			continue;
		if(hogcut !=0 && hogcut >= (double)cpu/(double)elapsed)
			continue;
		if (linedev != -1 && ab.ac_tty != linedev)
			continue;
		if (uidflag  && ab.ac_uid != uidval)
			continue;
		if(gidflag && ab.ac_gid != gidval)
			continue;
		if (tstart && end_time < tstart)
			continue;
		if (tend && (ab.ac_btime > tend))
			continue;
		if(cname && !cmatch(ab.ac_comm,cname))
			continue;
		if(iocut && iocut > io)
			continue;
		if(noid && uidtonam(ab.ac_uid)[0] != '?')
			continue;
		if (verbose) {
			printhd();
			verbose = 0;
		}
		println();
		if(average) {
			cmdcount++;
			realtot += (double)elapsed;
			usertot += (double)user;
			systot +=  (double)sys;
			kcoretot += (double)mem;
			iotot += (double)io;
			rwtot += (double)rw;
		};
	}
}

catch2()
{
	sig2++;
}


aread(ab)
struct acct *ab;
{
	if (sig2) {
		printf("\n");
		println();
		doexit(2);
	}
	if (backward) {
		if (ftell(stdin) < 2*sizeof(*ab))
			return(0);
		fseek(stdin, (long)-2*sizeof(*ab), 1);
		fread(ab, sizeof(*ab), 1, stdin);
		return(1);
	} else
		return(fread(ab, sizeof(*ab), 1, stdin) == 1);
}

printhd()
{
	printf("COMMAND                      START    END          REAL");
	ps("CPU");
	if (option & SEPTIME)
		ps("(SECS)");
	if (option & IORW){
		ps("CHARS");
		ps("BLOCKS");
	}
	if (option & CPUFACTOR)
		ps("CPU");
	if (option & HOGFACTOR)
		ps("HOG");
	if (!option || (option & MEANSIZE))
		ps("MEAN");
	if (option & KCOREMIN)
		ps("KCORE");
	printf("\n");
	printf("NAME         USER   TTYNAME  TIME     TIME       (SECS)");
	if (option & SEPTIME) {
		ps("SYS");
		ps("USER");
	} else
		ps("(SECS)");
	if (option & IORW) {
		ps("TRNSFD");
		ps("READ");
	}
	if (option & CPUFACTOR)
		ps("FACTOR");
	if (option & HOGFACTOR)
		ps("FACTOR");
	if (!option || (option & MEANSIZE))
		ps("SIZE(K)");
	if (option & KCOREMIN)
		ps("MIN");
	if(flag_field)
		printf("  F STAT");
	printf("\n");
	fflush(stdout);
}

ps(s)
char	*s;
{
	printf("%8.8s", s);
}

println()
{

	extern char *lookname();
	char name[32];
	if(quiet)
		return;
	if(!SU(ab.ac_flag))
		cat(name,command_name,0);
	else
		cat(name,"#",command_name,0);
	printf("%-9.9s", name);
	copy(lookname(ab.ac_uid,ab.ac_tty,ab.ac_btime),name);
	if(*name != '?')
		printf("  %-8.8s", name);
	else
		printf("  %-8d",ab.ac_uid);
	printf(" %-8.8s",ab.ac_tty != -1? devtolin(ab.ac_tty):"?");
	printf("%.9s", &ctime(&ab.ac_btime)[10]);
	printf("%.9s ", &ctime(&end_time)[10]);
	pf((double)SECS(elapsed));
#ifdef STOCKIII
	if (option & SEPTIME) {
		pf((double)sys / 60);
		pf((double)user / 60);
	} else
		pf((double)cpu / 60);
#else
	if (option & SEPTIME) {
		pf((double)SECS(sys));
		pf((double)SECS(user));
	} else
		pf((double)SECS(cpu));
#endif
	if (option & IORW)
		printf("%8ld%8ld",io,rw);
	if (option & CPUFACTOR)
		pf((double)user / cpu);
	if (option & HOGFACTOR)
		pf((double)cpu / elapsed);
	if (!option || (option & MEANSIZE))
		pf(KCORE(mem / cpu));
	if (option & KCOREMIN)
		pf(MINT(KCORE(mem)));
	if(flag_field)
		printf("  %1o %3o", (int)ab.ac_flag, (unsigned)ab.ac_stat);
	printf("\n");
	fflush(stdout);
}

char *
lookname(uid,tty,start)
uid_t	uid;
dev_t	tty;
time_t	start;
{
	return(uidtonam(uid));
}

pf(v)
double	v;
{
	printf("%8.2f", v);
}

/*
 * return uid of name, -1 if not found
 */
uid_t
namtouid(name)
char *name;
{
	struct passwd *getpwnam();
	register struct passwd *pp;

	setpwent();
	if ((pp = getpwnam(name)) == NULL)
		return((uid_t)-1);
	return((uid_t)pp->pw_uid);
}
/*
 * convtime converts time arg to internal value
 * arg has form hr:min:sec, min or sec are assumed to be 0 if omitted
 * times assumed to be within last 24 hours, unless -d mo/dy given before
 */
#include <time.h>
long	daydiff;


/*
 * convday computes number of seconds to be subtracted
 * because of -d mon/day argument
 */
convday(str)
register char *str;
{
	struct tm *cur;
	long tcur;
	int mday, mon;
	register i;

	if (sscanf(str, "%d/%d", &mon, &mday) != 2) {
		fprintf(stderr, "acctcom: can't scan -d %s\n", str);
		return;
	}
	mon--;
	time(&tcur);
	daydiff = tcur;
	for (i = 0; i < 100; i++) {
		cur = localtime(&tcur);
		if (mday == cur->tm_mday && mon == cur->tm_mon) {
			daydiff -= tcur;
			return;
		}
		tcur -= SECSINDAY;
	}
	daydiff = 0;
	fprintf(stderr, "acctcom: bad -d %s\n", str);
}

long
convtime(str)
char *str;
{
	struct tm *cur, arg;
	long tcur;

	arg.tm_min = 0;
	arg.tm_sec = 0;
	if (sscanf(str, "%d:%d:%d", &arg.tm_hour, &arg.tm_min,
		&arg.tm_sec) < 1) {
		fprintf(stderr, "acctcom: bad time: %s\n", str);
		return(0);
	}
	time(&tcur);
	cur = localtime(&tcur);
	if (tmless(&arg, cur))
		tcur -= (daydiff+tmsecs(&arg, cur));
	else {
		tcur -= SECSINDAY-tmsecs(cur, &arg);
		if (daydiff)
			tcur -= daydiff-SECSINDAY;
	}
	return(tcur);
}
cmatch(comm, cstr)
register char	*comm, *cstr;
{

	char	xcomm[9];
	register  i;

	for(i=0;i<8;i++){
		if(comm[i]==' '||comm[i]=='\0')
			break;
		xcomm[i] = comm[i];
	}
	xcomm[i] = '\0';

	return(regex(cstr,xcomm));
}

cmset(pattern)
register char	*pattern;
{

	if((pattern=(char *)regcmp(pattern,0))==NULL){
		fprintf(stderr,"pattern syntax\n");
		exit(1);
	}

	return((unsigned)pattern);
}

doexit(status)
{
	if(!average)
		exit(status);
	if(cmdcount != 0) {
		printf("\cmds=%ld ",cmdcount);
		printf("Real=%-6.2f ",SECS(realtot)/cmdcount);
		cputot = systot + usertot;
		printf("CPU=%-6.2f ",SECS(cputot)/cmdcount);
		printf("USER=%-6.2f ",SECS(usertot)/cmdcount);
		printf("SYS=%-6.2f ",SECS(systot)/cmdcount);
		printf("CHAR=%-8.2f ",iotot/cmdcount);
		printf("BLK=%-8.2f ",rwtot/cmdcount);
		printf("USR/TOT=%-4.2f ",usertot/cputot);
		printf("HOG=%-4.2f ",cputot/realtot);
		printf("\n");
	}
	else
		printf("\nNo commands matched\n");
	exit(status);
}
isdevnull()
{
	struct stat	filearg;
	struct stat	devnull;

	if(fstat(0,&filearg) == -1) {
		fprintf(stderr,"acctcom: cannot stat stdin\n");
		return(NULL);
	}
	if(stat("/dev/null",&devnull) == -1) {
		fprintf(stderr,"acctcom: cannot stat /dev/null\n");
		return(NULL);
	}

	if(filearg.st_rdev == devnull.st_rdev) return(1);
	else return(NULL);
}
