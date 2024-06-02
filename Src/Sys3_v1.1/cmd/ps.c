
/*
 *	ps - process status
 *	examine and print certain things about processes
 */

#include <stdio.h>
#include <a.out.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/tty.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/var.h>
#include <core.h>
#include <sys/plexus.h>

#define NTTYS	20
#define SIZ 	30
#define TSIZE	100
#define ARGSIZ	30

struct nlist nl[] = {
	{ "_proc" },
	{ "_swplo" },
	{ "_v" },
	{ 0 },
};

union {
	struct proc mprc;
	struct xproc zprc;
	} prc;
#define mproc prc.mprc
#define zproc prc.zprc
struct	var  v;
#ifdef pdp11
struct	user u;
#endif
#ifdef vax
union { struct user yy;
	int xx[128] [UPAGES];
	} zz;
#define u zz.yy
int	pagetbl[128];
int	mf;
#endif

int	retcode=1;
int	c;
int	lflg;
int	eflg;
int	uflg;
int	aflg;
int	dflg;
int	pflg;
int	fflg;
int	gflg;
int	tflg;
int	sflg;
int	errflg;
char	*gettty();
char	*ttyname();
int	mem;
int	swmem;
int	swap;
daddr_t	swplo;
char	argbuf[ARGSIZ];
char	*parg;
char	*p1;
char	*coref;
char	*memf;
long lseek();

int	ndev;
struct devl {
	char	dname[DIRSIZ];
	dev_t	dev;
} devl[256];

static	int uid_num[TSIZE];	/* for u option */
static	char *uid_name[TSIZE];
static	int uidn = 0;
int	uid_tbl[SIZ];
int	nuids = 0;
char	*tty[NTTYS];	/* for t option */
int	ntty = 0;
int	pid[SIZ];	/* for p option */
int	npid = 0;
int	grpid[SIZ];	/* for g option */
int	ngrpid = 0;

main(argc, argv)
char **argv;
{
	register char **ttyp = tty;
	char *system, *name;
	char *p;
	int puid, ppid, ppgrp;
	int i, found;
	extern char *optarg;
	extern int optind;
	char	*getstr;
	char *usage="ps [ -edalf ] [ -c corefile ] [ -s swapdev ] [ -n namelist ] [ -t tlist ]";
	char *usage2="	[ -p plist ] [ -u ulist ] [ -g glist ]";
	char *malloc();
	unsigned size;

	getstr = "lfeadn:s:c:t:p:g:u:";
	system = UNIXNAME;
#ifdef vax
		coref = "/dev/kmem";
		memf = "/dev/mem";
#else
		coref = "/dev/kmem";
		memf = "/dev/mem";
#endif

	while ((c = getopt(argc,argv,getstr)) != EOF)
		switch(c) {
		case 'l':		/* long listing */
			lflg++;
			break;

		case 'f':		/* full listing */
			fflg++;
			break;

		case 'e':		/* list for every process */
			eflg++;
			tflg = uflg = pflg = gflg = 0;
			break;

		case 'a':		/* same as e except no proc grp leaders */
			aflg++; 	/* and no non-terminal processes     */
			break;

		case 'd':		/* same as e except no proc grp leaders */
			dflg++;
			break;

		case 'n':		/* alternate namelist */
			system = optarg;
			break;

		case 'c':		/* core file given */
			coref = optarg;	
			memf = coref;
			break;

		case 's':		/* swap device given */
			sflg++;
			if ((swap = open(optarg, 0)) < 0 ) {
				fprintf(stderr, "ps: cannot open %s\n",optarg);
				done(1);
			}
			break;

		case 't':		/* terminals */
			tflg++;
			p1 = optarg;
			do {
				parg = argbuf;
				if (ntty >= NTTYS)
					break;
				getarg();
				if (strncmp(parg,"tty",3) == 0)
					parg += 3;
				size = strlen(parg);
				if ((p = malloc(++size)) == 0) {
					fprintf(stderr,"ps: no memory\n");
					done(1);
				}
				strcpy(p,parg);
				*ttyp++ = p;
				ntty++;
			}
			while (*p1);
			break;

		case 'p':		/* proc ids */
			pflg++;
			p1 = optarg;
			parg = argbuf;
			do {
				if (npid >= SIZ)
					break;
				getarg();
				pid[npid++] = atoi(parg);
			}
			while (*p1);
			break;

		case 'g':		/* proc group */
			gflg++;
			p1 = optarg;
			parg = argbuf;
			do {
				if (ngrpid >= SIZ)
					break;
				getarg();
				grpid[ngrpid++] = atoi(parg);
			}
			while (*p1);
			break;

		case 'u':		/* user name or number */
			uflg++;
			p1 = optarg;
			parg = argbuf;
			do {
				getarg();
				uopt(parg);
			}
			while (*p1);
			break;

		case '?':		/* error */
			errflg++;
			break;
		}
	if ( errflg || (optind < argc)) {
		fprintf(stderr,"usage: %s\n%s\n",usage,usage2);
		done(1);
	}
	if (tflg)
		*ttyp = 0;
	/* if specifying options not used, current terminal is default */
	if ( !(aflg || eflg || dflg || uflg || tflg || pflg || gflg )) {
		name = ttyname(2);
		if (strncmp(name+5,"tty",3)==0)
			*ttyp++ = name+8;
		else
			*ttyp++ = name+5;
		*ttyp = 0;
		ntty++;
		tflg++;
	}
	if (eflg)
		tflg = uflg = pflg = gflg = aflg = dflg = 0;
	if (aflg || dflg)
		tflg = 0;
	nlist(system, nl);
	if(nl[0].n_type==0||nl[1].n_type==0||nl[2].n_type==0) {
		fprintf(stderr, "ps: no namelist\n");
		done(1);
	}
#ifdef	vax
	nl[0].n_value = ((int)nl[0].n_value & 0x3fffffff);
	nl[1].n_value = ((int)nl[1].n_value & 0x3fffffff);
	nl[2].n_value = ((int)nl[2].n_value & 0x3fffffff);
#endif
	/* info from running kernel (proc table, swplo, v) read from coref */
	if ((mem = open(coref, 0)) < 0) {
		fprintf(stderr, "ps: no mem\n");
		done(1);
	}
	/* info from in-core user process data space read from memf */
	if ((swmem = open(memf,0)) < 0) {
		fprintf(stderr, "ps: no mem\n");
		done(1);
	}
	if (chdir("/dev") < 0) {
		fprintf(stderr, "ps: cannot change to /dev\n");
		done(1);
	}
	/*
	 * Find base of swap
	 */
	l_lseek(mem, (long)nl[1].n_value, 0);
	r_read(mem, (char *)&swplo, sizeof(swplo));
	/*
	 * read to find proc table size
	 */
	l_lseek(mem, (long)nl[2].n_value, 0);
	r_read(mem, (char *)&v, sizeof(v));
	/*
	 * Locate proc table
	 */
	l_lseek(mem, (long)nl[0].n_value, 0);
	getdev();
	if (fflg && lflg )
		printf("  F S    UID   PID  PPID  C PRI NI    ADDR SZ    WCHAN   STIME TTY TIME COMD\n");
	else if (fflg)
		printf("    UID   PID  PPID  C   STIME TTY  TIME COMMAND\n");
	else if (lflg)
		printf("  F S   UID   PID  PPID  C PRI NI    ADDR SZ   WCHAN TTY  TIME COMD\n");
	else
		printf("   PID TTY TIME COMMAND\n");
	/* determine which processes to print info about */
	for (i=0; i<v.v_proc; i++) {
		found = 0;
		r_read(mem, (char *)&mproc, sizeof(mproc));
		if (mproc.p_stat == 0)	
			continue;
		puid = mproc.p_uid;
		ppid = mproc.p_pid;
		ppgrp = mproc.p_pgrp;
		if ((ppid == ppgrp) && (dflg || aflg))
			continue;
		if (eflg || dflg)
			found++;
		else if (pflg && search(pid, npid, ppid))
			found++;
		else if (uflg && ufind(puid))
			found++;
		else if (gflg && search(grpid, ngrpid, ppgrp))
			found++;
		if ( !found && !tflg && !aflg )
			continue;
		if (prcom(puid,found)) {
			printf("\n");
			retcode =0;
		}
	}
	done(retcode);
}

/* getdev reads in the open devices (terminals) and stores */
/* info in the devl structure.  /dev/swap is opened */
getdev()
{
#include <sys/stat.h>
	register FILE *df;
	struct stat sbuf;
	struct direct dbuf;

	if ((df = fopen("/dev", "r")) == NULL) {
		fprintf(stderr, "ps: cannot open /dev\n");
		done(1);
	}
	ndev = 0;
	while (fread((char *)&dbuf, sizeof(dbuf), 1, df) == 1) {
		if(dbuf.d_ino == 0)
			continue;
		if(stat(dbuf.d_name, &sbuf) < 0)
			continue;
		if ((sbuf.st_mode&S_IFMT) != S_IFCHR)
			continue;
		strcpy(devl[ndev].dname, dbuf.d_name);
		devl[ndev].dev = sbuf.st_rdev;
		ndev++;
	}
	fclose(df);
	if ( !sflg ) {
		if ((swap = open("/dev/swap", 0)) < 0) {
			fprintf(stderr, "ps: cannot open /dev/swap\n");
			done(1);
		}
	}
}

/* getarg finds next argument in list and copies arg into argbuf  */
/* p1 first pts to arg passed back from getopt routine.  p1 is then */
/* bumped to next character that is not a comma or blank - p1 null */
/* indicates end of list    */

getarg()
{
	char *parga;
	parga = argbuf;
	while(*p1 && *p1 != ',' && *p1 != ' ')
		*parga++ = *p1++;
	*parga = '\0';

	while( *p1 && ( *p1 == ',' || *p1 == ' ') )
		p1++;
}

/* gettty returns the user's tty number or ? if none  */
char *gettty()
{
	register i;
	register char *p;

	if (u.u_ttyp==0)
		return("?");
	for (i=0; i<ndev; i++) {
		if (devl[i].dev == u.u_ttyd) {
			p = devl[i].dname;
			if (strncmp(p,"tty",3) == 0)
				p += 3;
			return(p);
		}
	}
	return("?");
}

#ifdef pdp11
long	round(a,b)
	long		a, b;
{
	long		w = ((a+b-1)/b)*b;

	return(w);
}
#endif

typedef unsigned pos;
struct map {
	long	b1, e1; long f1;
	long	b2, e2; long f2;
};
struct map datmap;

int	file;

/* print info about the process */
prcom(puid,found)
int puid,found;
{
	int abuf[BSIZE/sizeof(int)];
	long addr;
	register int *ip;
	register char *cp, *cp1;
	char *cp2;
	char *ctime();
	time_t time();
	time_t *clock, *tloc;
	time_t tim;
	char timbuf[26];
	char *curtim = timbuf;
	char *sttim, *s1;
	long tm;
	int c, nbad, badflg;
	register char *tp;
	long	txtsiz, datsiz, stksiz;
	int	septxt;
	int	match, i;
	int	nbytes;
	int	uzero = 0;
	register char **ttyp, *str;
	int	lw=(lflg?35:80);

	/* if process is zombie, call print routine and return */
	if (mproc.p_stat==SZOMB) {
		if ( tflg && !found)
			return(0);
		else {
			przom(puid);
			return(1);
		}
	}
	/* Determine if process is in memory or swap space */
	/* and read in user block */
#ifdef vax
	if ((mproc.p_flag& (SLOAD | SSPART)) == 0) {
		addr = (mproc.p_swaddr+swplo)<<9;
		mf = swap;
		l_lseek(mf, addr, 0);
		if ((nbytes = read(mf, (char *)&u, sizeof(u))) != sizeof(u)) {
			if (( nbytes == 0) && sflg) {
				uzero++;
				for ( i = 0, tp = (char *) &u; i < sizeof(u); i++)
					*tp++ = '\0';
			}
			else
				return(0);
		}
	} else {
		for(c=0; c<UPAGES; c++) {
			l_lseek(swmem,(long)mproc.p_addr[c]<<9,0);
			if (read(swmem,(char *)(((int *)&u)+128*c),512) != 512)	/* get u page */
				return(0);
		}
	}
#endif
#ifdef pdp11
	if (mproc.p_flag&SLOAD) {
		addr = ctob((long)mproc.p_addr);
		file = swmem;
	} else {
		addr = (mproc.p_addr+swplo)<<9;
		file = swap;
	}
	l_lseek(file, addr, 0);
	if ((nbytes = read(file, (char *)&u, sizeof(u))) != sizeof(u))
		if (( nbytes == 0) && sflg && (file == swap)) {
			uzero++;
			for ( i = 0, tp = (char *) &u; i < sizeof(u); i++)
				*tp++ = '\0';
		} else 
			return(0);
#endif

	/* get current terminal - if none (?) and aflg is set */
	/* then don't print info - if tflg is set, check if term */
	/* is in list of desired terminals and print if so   */
	tp = gettty();
	if ( aflg && (*tp == '?' ))
		return(0);
	if(tflg && !found) {	/* the t option */
		for (ttyp=tty, match=0; (str = *ttyp) !=0 && !match; ttyp++)
			if (strcmp(tp,str) == 0)
				match++;
		if(!match)
			return(0);
	}

	if (lflg)
		printf("%3o %c", mproc.p_flag&0377, "OSWRIZT"[mproc.p_stat]);	/* F S */
	if (fflg) {
		i = getunam(puid);
		if (i >= 0)
			printf("%7s", uid_name[i]);
		else
			printf("%7d", puid);
	}
	else if (lflg)
		printf("%6d", puid);
	printf("%6u",mproc.p_pid);	/* PID */
	if (lflg || fflg)
		printf("%6u%3d", mproc.p_ppid, mproc.p_cpu&0377);	/* PPID  CPU */
	if (lflg) {
		printf("%4d%3d",mproc.p_pri, mproc.p_nice);	/* PRI  NICE */
#ifdef vax
		printf("%8x%3d", mproc.p_addr[0], mproc.p_size);	/* ADDR  SZ */
		if (mproc.p_wchan)
			printf("%9x",mproc.p_wchan);	/* WCHAN */
		else
			printf("         ");
#endif
#ifdef pdp11
		printf("%8o%3d", mproc.p_addr, ctod(mproc.p_size));
		if (mproc.p_wchan)
			printf("%9o", mproc.p_wchan);
		else
			printf("         ");
#endif
	}
	if (uzero) 		/* u-block zeroed out so return */
		return(1);
	if (fflg) {		  /* STIME*/
		clock = &u.u_start;
		tim = time((time_t *) 0);
		tloc = &tim;
		s1 = ctime(tloc);
		strcpy(curtim,s1);
		sttim = ctime(clock);
		prtim(curtim, sttim);
	}
	printf(" %2.2s", tp);	/* TTY */
	tm = (u.u_utime + u.u_stime + (HZ/2))/HZ;	/* TIME */
	printf(" %2ld:", tm/60);
	tm %= 60;
	printf(tm<10?"0%ld":"%ld", tm);
	if (mproc.p_pid==0) {
		printf(" swapper");
		return(1);
	}
	/* if fflg not set, print command from u_block */
	if (!fflg) {		/* CMD */
		printf(" %.8s", u.u_comm);
		return(1);
	}
	/* set up address maps for user pcs */
#ifdef pdp11
	txtsiz = ctob(u.u_tsize);
	datsiz = ctob(u.u_dsize);
	stksiz = ctob(u.u_ssize);
	septxt = u.u_sep;
	datmap.b1 = (septxt ? 0 : round(txtsiz,TXTRNDSIZ));
	datmap.e1=datmap.b1+datsiz;
	datmap.f1 = ctob(USIZE)+addr;
	datmap.b2 = stackbas(stksiz);
	datmap.e2 = stacktop(stksiz);
	datmap.f2 = ctob(USIZE)+(datmap.e1-datmap.b1)+addr;

	/* determine if process is a shell or not */ 
	/* if last word is null then shell */
	
	addr += ctob(mproc.p_size) - BSIZE;
	l_lseek(file, addr+BSIZE-sizeof(int), 0);
	if (read(file, (char *)abuf, sizeof(int)) != sizeof(int))
		return(1);
	if (abuf[0]&0177400) {
		char b[82];
		char *bp = b;
		char **ap = abuf[0];
		char *cp;
	
		*bp++ = ' ';
		badflg = 0;
		while((cp=(char *)getword(ap++)) != -1 && cp && (bp < b+lw)){
			nbad = 0;
			while((c = getbyte(cp++)) && (int)(cp) != 0177777 && (bp < b + lw)){
				badflg++;
				if (c < ' ' || c > '~') {
					if (nbad++ > 3)
						break;
					continue;
				}
				*bp++ = c;
			}
			*bp++ = ' ';
		}
		*bp++ = '\0';
		if ( badflg == 0 || nbad != 0 || *b == '\0' )
			printf(" [ %.8s ]",u.u_comm);
		else
			printf("%.20s",b);
		return(1);
	}
	l_lseek(file, addr, 0);
	if (read(file, (char *)abuf, sizeof(abuf)) != sizeof(abuf))
		return(1);
#endif
#ifdef vax
	c = mproc.p_size - btoc(512);
	if ((mproc.p_flag & SLOAD) == 0) {
		addr += ctob(c);
		l_lseek(mf, addr, 0);
		if (read(mf, (char *)abuf, sizeof(abuf)) != sizeof(abuf))
			return(1);
	} else {
		if (u.u_pcb.pcb_szpt<1 || u.u_pcb.pcb_szpt>20)
			return(1);
		c = ctob((u.u_ptable[u.u_pcb.pcb_szpt-1] & 0x1fffff));
		l_lseek(swmem,(long)c,0);
		if (read(swmem,(char *)pagetbl,512) != 512)	/* get last page table */
			return(1);
		l_lseek(swmem,ctob((pagetbl[127] & 0x1fffff)),0);
		if (read(swmem,(char *)abuf,sizeof(abuf)) != sizeof(abuf))
			return(1);
	}
#endif
	badflg = 0;
	for (ip = &abuf[BSIZE/sizeof(int)-sizeof(int)]; ip > abuf;) {
		if (*--ip == -1 || *ip == 0) {
			cp = (char *)(ip+1);
			if (*cp==0)
				cp++;
			nbad = 0;
			for (cp2 = cp1 = cp; cp1 < (char *)&abuf[BSIZE/sizeof(int)]; cp1++) {
				badflg++;
				c = *cp1&0177;
				if (c==0) {
					*cp1 = ' ';
					cp2 = cp1;
				}
				else if (c < ' ' || c > '~') {
					if (++nbad >= 3) {
						*cp1++ = ' ';
						break;
					}
					*cp1 = '?';
				}
				if (c == '=') {
					cp1 = cp2;
					*cp1++ = 0;
					break;
				}
			}
			while (*--cp1==' ')
				*cp1 = 0;
			if ( badflg == 0 || nbad != 0 || *cp == '\0')
				printf(" [ %.8s ]",u.u_comm);
			else
				printf(lflg?" %.35s":" %.80s", cp);
			return(1);
		}
	}
	printf(" [ %.8s ]",u.u_comm);
	return(1);
}


/* file handling and access routines */

getbyte(adr)
	pos	adr;
{
	return((int)access(adr,1));
}

getword(adr)
	pos	adr;
{
	return((int)access(adr,sizeof(int*)));
}

access(aadr,size)
pos aadr;
int size;
{
	int *word = 0;
	register struct map *amap = &datmap;
	long adr = aadr;

	if(!within(aadr,amap->b1,amap->e1)) {
		if(within(aadr,amap->b2,amap->e2)) 
			adr += (amap->f2)-(amap->b2);
		else
			return(0);
	}
	else {
		adr += (amap->f1)-(amap->b1);
	}
	if(lseek(file,adr,0)==-1 || read(file,(char *)&word,size)<size) {
		return(0);
	}

	return(word);
}


within(adr,lbd,ubd)
	pos	adr;
	long	lbd, ubd;
{
	return(adr>=lbd && adr<ubd);
}

done(exitno)
{
	exit(exitno);
}

/* search returns 1 if arg is found in array arr */
/* which has length num, and returns 0 if not found */

search(arr, num, arg)
int arr[];
int num;
int arg;
{
	int i;
	for (i = 0; i < num; i++)
		if (arg == arr[i])
			return(1);
	return(0);
}

/* uopt is used for the fancy version of the u option where either */
/* user name or number may be an argument.  The arrays uid_name & */
/* uid_num store the associated names and numbers.  ( these arrays are */
/* later used for printing UID ). The array uid_tbl stores */
/* ptrs (index) into the name & number arrays for arguments */
/* after the u option */

#include <pwd.h>
uopt(oarg)
char *oarg;
{
	int found = -1;

	struct passwd *pwd, *getpwuid(), *getpwname();
	char *pwname;
	char *p, *malloc();
	unsigned size;
	int pwuid;
	int i;

	/* search thru name array for oarg */
	for (i=0; i<uidn; i++) {
		if (strcmp(oarg,uid_name[i])==0) {
			found = i;
			break;
		}
	}
	/* if not found then search through number array */
	if (found < 0) {
		pwuid = atoi(oarg);
		for (i=0; i<uidn; i++) {
			if (pwuid == uid_num[i]) {
				found = i;
				break;
			}
		}
	}
	/* if found then enter found index into tbl array */
	if ( found != -1 ) {
		if (nuids >= SIZ) return;
		uid_tbl[nuids++] = found;
		return;
	}
	/* oarg was not found in uid arrays so search through /etc/passwd */
	/* for name and number.  If found, enter name and number in arrays */
	/* and then put values in table */

	pwname = oarg;
	if(pwd = getpwname(pwname)) {	/* not null so found */
		pwuid = pwd->pw_uid;
	}
	else {
		pwuid = atoi(oarg);
	/* rewrite an atoi - this is kludge */
		if (( pwuid == 0) && (strcmp(oarg,"0") != 0))
			return;
		if (pwd = getpwuid(pwuid)) {
			pwname = pwd->pw_name;
		}
		else {
		/* not found so name or number is invalid */
		return;
		}
	}

	/* put nuid name and number in table */
	if (( uidn >= TSIZE) || (nuids >= SIZ))
		return;
	uid_num[uidn] = pwuid;
	size = strlen(pwname);
	if ((p = malloc(++size)) == 0) {
		fprintf(stderr,"ps: no memory\n");
		done(1);
	}
	strcpy(p,pwname);
	uid_name[uidn] = p;
	uid_tbl[nuids++] = uidn;
	uidn++;
	return;
}

/* for full command (-f flag) print user name instead of number */
/* search thru existing table of userid numbers and if puid is found, */
/* return corresponding name.  Else search thru /etc/passwd */

getunam(puid)
int puid;
{
	struct passwd *pwd, *getpwuid();
	char *p, *malloc();
	unsigned size;
	char *uname;
	int i;

	for (i=0; i<uidn; i++)
		if (uid_num[i] == puid)
			return(i);
	/* not found in table so search thru /etc/passwd for number & return name */
	if (pwd = getpwuid(puid))
		uname = pwd->pw_name;
	else
		return(-1);
	if (uidn >= TSIZE) return(-1);
	uid_num[uidn] = puid;
	size = strlen(uname);
	if ((p = malloc(++size)) == 0) {
		fprintf(stderr,"ps: no memory\n");
		done(1);
	}
	strcpy(p,uname);
	uid_name[uidn++] = p;
	return(uidn-1);
}

/* ufind will return 1 if puid is in table ; if not return 0 */
ufind(puid)
int puid;
{
int	i, j;
	for (i=0; i<nuids; i++){
		j = uid_tbl[i];
		if (uid_num[j] == puid)
			return(1);
	}
	return(0);
}

/* lseek with error checking */
l_lseek(fd, offset, whence)
int fd, whence;
long	offset;
{
	if (lseek(fd, offset, whence) == -1) {
		fprintf(stderr, "ps: error on lseek\n");
		done(1);
	}
}

/* read with error checking */
r_read (fd, buf, nbytes)
int	fd, nbytes;
char	*buf;
{
	if (read(fd, buf, nbytes) != nbytes) {
		fprintf(stderr, "ps: error on read\n");
		done(1);
	}
}
/* print starting time of process unless process started more */
/* than 24 hours ago in which case date is printed   */
/* sttim is start time and it is compared to curtim (current time ) */

prtim(curtim, sttim)
char *curtim, *sttim;
{
	char *p1, *p2;
	char dayst[3], daycur[3];
	if ( strncmp(curtim, sttim, 11) == 0) {
		p1 = sttim + 11;
		p2 = p1 + 8;
	}
	else {
		p1 = sttim + 4;
		p2 = p1 + 7;
		/* if time is < 24 hours different, then print time */
		if (strncmp(curtim+4, sttim+4, 3) == 0) {
			strncpy(dayst,sttim+8, 2);
			strcat(dayst,"");
			strncpy(daycur,curtim+8,2);
			strcat(daycur,"");
			if ((atoi(dayst) +1 == atoi(daycur)) &&
				 (strncmp(curtim+11,sttim+11,8)<=0)) {
				p1 = sttim + 11;
				p2 = p1 + 8;
			}
		}
	}
	*p2 = '\0';
	printf("%9s",p1);
}
przom(puid)
/* print zombie process - zproc overlays mproc */
int puid;
{
	int i;
	long tm;

	if (lflg)
		printf("%3o %c", zproc.xp_flag&0377, "OSWRIZT"[zproc.xp_stat]);	/* F S */
	if (fflg) {
		i = getunam(puid);
		if (i >= 0)
			printf("%7s", uid_name[i]);
		else
			printf("%7d", puid);
	}
	else if (lflg)
		printf("%6d", puid);
	printf("%6u",zproc.xp_pid);	/* PID */
	if (lflg || fflg)
		printf("%6u%3d", zproc.xp_ppid, zproc.xp_cpu&0377);	/* PPID  CPU */
	if (lflg)
		printf("%4d%3d",zproc.xp_pri, zproc.xp_nice);	/* PRI  NICE */
	if (fflg) printf("         ");
	if (lflg) printf("                    ");
	tm = (zproc.xp_utime + zproc.xp_stime + (HZ/2))/HZ;	/* TIME */
	printf("    %2ld:", tm/60);
	tm %= 60;
	printf(tm<10?"0%ld":"%ld", tm);
	printf(" <defunct>");
	return;
}
