#

/*
 * wtmpfix - adjust wtmp file and remove date changes.
 *
 *	wtmpfix <wtmp1 >wtmp2
 *
 *	R.M. Kofoed, Jr.		Bell Laboratories
 *					Piscataway, N. J.
 */

# include <stdio.h>
# include <ctype.h>
# include <sys/types.h>
# include <utmp.h>

# define BOOT	"~"
# define ODATE	"|"
# define NDATE	"{"
# define DAILY	""

FILE	*Wtmp, *Opw;

char	*Ofile	={ "/tmp/wXXXXXX" };

struct	dtab
{
	long	d_off1;		/* file offset start */
	long	d_off2;		/* file offset stop */
	long	d_adj;		/* time adjustment */
	struct dtab *d_ndp;	/* next record */
};

struct	dtab	*Fdp;		/* list header */
struct	dtab	*Ldp;		/* list trailer */


long	ftell();

intr()
{

	signal(2,1);
	unlink(Ofile);
	exit(1);
}

main(argc, argv)
char	**argv;
{

	struct	utmp	wrec, wrec2;
	static long	recno = 0;
	register struct dtab *dp;

	if(argc < 2){
		argv[argc] = "-";
		argc++;
	}
	if((signal(2,1)&01)==0)
		signal(2,intr);
	mktemp(Ofile);
	if((Opw=fopen(Ofile,"w"))==NULL)
		err("cannot make temporary: %s", Ofile);
	while(--argc > 0){
		argv++;
		if(strcmp(*argv,"-")==0)
			Wtmp = stdin;
		else if((Wtmp = fopen(*argv,"r"))==NULL)
			err("Cannot open: %s", *argv);
		while(winp(Wtmp,&wrec)){
			if(recno == 0 || strcmp(wrec.ut_line,BOOT)==0){
				mkdtab(recno,&wrec);
			}
			if(strcmp(wrec.ut_line,ODATE)==0){
				if(!winp(Wtmp,&wrec2))
					err("Input truncated at offset %ld",recno);
				if(strcmp(wrec2.ut_line,NDATE)!=0)
					err("New date expected at offset %ld",recno);
				setdtab(recno,&wrec,&wrec2);
				recno += sizeof(struct utmp);
				recno += sizeof(struct utmp);
				wout(Opw,&wrec);
				wout(Opw,&wrec2);
				continue;
			}
			wout(Opw,&wrec);
			recno += sizeof(struct utmp);
		}
		if(Wtmp!=stdin)
			fclose(Wtmp);
	}
	fclose(Opw);
	if((Opw=fopen(Ofile,"r"))==NULL)
		err("Cannot read from temp: %s", Ofile);
	recno = 0;
	while(winp(Opw,&wrec)){
		adjust(recno,&wrec);
		recno += sizeof(struct utmp);
		if(strcmp(wrec.ut_line,ODATE)==0 || strcmp(wrec.ut_line,NDATE)==0)
			continue;
		wout(stdout,&wrec);
	}
	fclose(Opw);
	unlink(Ofile);
	exit(0);
}
err(f,m1,m2,m3)
{

	fprintf(stderr,f,m1,m2,m3);
	fprintf(stderr,"\n");
	intr();
}
winp(f,w)
register FILE *f;
register struct utmp *w;
{

	if(fread(w,sizeof(struct utmp),1,f)!=1)
		return 0;
	switch(w->ut_line[0]){
	case 't':
	case 'l':
	case 'c':
	case '~':
	case '|':
	case '{':
	case '\0':
		return ((unsigned)w);
	}
	if(isalpha(w->ut_line[0]) || isdigit(w->ut_line[0]))
		return ((unsigned)w);
	fprintf(stderr,"Bad file at offset %ld\n",ftell(f)-sizeof(struct utmp));
	fprintf(stderr,"%-8s %-8s %lu %s",w->ut_line,w->ut_name,w->ut_time,ctime(&w->ut_time));
	intr();
}
wout(f,w)
{

	fwrite(w,sizeof(struct utmp),1,f);
}
mkdtab(p,w)
long	p;
register struct utmp *w;
{

	register struct dtab *dp;

	dp = Ldp;
	if(dp == NULL){
		dp = (struct dtab *)calloc(sizeof(struct dtab),1);
		if(dp == NULL)
			err("out of core");
		Fdp = Ldp = dp;
	}
	dp->d_off1 = p;
}
setdtab(p,w1,w2)
long	p;
register struct utmp *w1, *w2;
{

	register struct dtab *dp;

	if((dp=Ldp)==NULL)
		err("no dtab");
	dp->d_off2 = p;
	dp->d_adj = w2->ut_time - w1->ut_time;
	if((Ldp=(struct dtab *)calloc(sizeof(struct dtab),1))==NULL)
		err("out of core");
	Ldp->d_off1 = dp->d_off1;
	dp->d_ndp = Ldp;
}
adjust(p,w)
long	p;
register struct utmp *w;
{

	long pp;
	register struct dtab *dp;

	pp = p;

	for(dp=Fdp;dp!=NULL;dp=dp->d_ndp){
		if(dp->d_adj==0)
			continue;
		if(pp>=dp->d_off1 && pp < dp->d_off2)
			w->ut_time += dp->d_adj;
	}
}
