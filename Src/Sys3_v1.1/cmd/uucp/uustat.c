	/*  %M% %I%  %G%  %U%  */
 
#include "uucp.h"
#ifdef UUSTAT
#include <time.h>
#include <sys/types.h>
#include "uust.h"
#define	rid	0		/* user id for super user */
 
/***	system status text	***/
char *us_stext[] = {
	"CONVERSATION SUCCEEDED",
	"BAD SYSTEM",
	"WRONG TIME TO CALL",
	"SYSTEM LOCKED",
	"NO DEVICE AVAILABLE",
	"DIAL FAILED",
	"LOGIN FAILED",
	"HANDSHAKE FAILED",
	"STARTUP FAILED",
	"CONVERSATION IN PROGRESS",
	"CONVERSATION FAILED",
	"CALL SUCCEEDED"
	};
/***	request status text	***/
char *us_rtext[] = {
	"STATUS UNKNOWN: SYSTEM ERROR",
	"COPY FAIL",
	"LOCAL ACCESS TO FILE DENIED",
	"REMOTE ACCESS TO FILE DENIED",
	"A BAD UUCP COMMAND GENERATED",
	"REMOTE CAN'T CREATE TEMP FILE",
	"CAN'T COPY TO REMOTE DIRECTORY",
	"CAN'T COPY TO LOCAL DIRECTORY - FILE LEFT IN PUBDIR/USER/FILE",
	"LOCAL CAN'T CREATE TEMP FILE",
	"CAN'T EXECUTE UUCP",
	"COPY (PARTIALLY) SUCCEEDED",
	"COPY FINISHED, JOB DELETED",
	"JOB IS QUEUED"
	};
 
short vflag = 0;
 
/*************
 *	uustat --- A command that provides uucp status.
 */
 
main(argc, argv)
char **argv;
int argc;
{
	extern char *optarg;
	extern int optind;
	struct us_rsf u;
	struct us_ssf ss;
	short cflag,jflag,sflag,kflag,uflag,oflag,yflag,mflag,iflag;
	int c;
	short ca, ja, ka, oa, ya;
	char sa[NAME7], ua[NAME7], ma[NAME7];
	char s[128], buf[BUFSIZ];
	FILE *fp, *fq, *us_open();
 
	cflag=jflag=sflag=kflag=uflag=oflag=yflag=mflag=iflag=vflag=0;
	while ((c=getopt(argc, argv, "x:c:j:s:k:u:o:y:m:v")) != EOF ) 
		switch(c) {
		case 'x':
			Debug = atoi(optarg);
			break;
		case 'c':
			if (mflag || iflag || kflag || jflag) goto error;
			cflag++;
			ca = atoi(optarg);
			break;
		case 'j':
			if (mflag || iflag || kflag || cflag) goto error;
			jflag++;
			if (strcmp(optarg, "all") == SAME) ja = -1;
			else	ja = atoi(optarg);
			break;
		case 'm':
			if (jflag || iflag || kflag || cflag) goto error;
			mflag++;
			strncpy(ma, optarg, NAME7);
			ma[NAME7] = '\0';
			break;
		case 'k':
			if (jflag || mflag || iflag || cflag) goto error;
			kflag++;
			ka = atoi(optarg);
			break;
		case 'u':
			if (jflag || mflag || kflag || cflag) goto error;
			iflag++;
			uflag++;
			strncpy(ua, optarg, NAME7);
			ua[NAME7] = '\0';
			break;
		case 'o':
			if (jflag || mflag || kflag || cflag) goto error;
			iflag++;
			oflag++;
			oa = atoi(optarg);
			break;
		case 'y':
			if (jflag || mflag || kflag || cflag) goto error;
			iflag++;
			yflag++;
			ya = atoi(optarg);
			break;
		case 's':
			if (jflag || mflag || kflag || cflag) goto error;
			iflag++;
			sflag++;
			strncpy(sa, optarg, NAME7);
			sa[NAME7] = '\0';
			break;
		case 'v':
			vflag++;
			break;
		case '?':
			error:
			 fprintf(stderr, "Usage: uustat [-j* -v]");
			 fprintf(stderr, " [-m*] [-k*] [-c*] [-v]\n");
			 fprintf(stderr, "\t\t[-u* -s* -o* -y* -v]\n");
			 exit(2);
		}
 
	guinfo(getuid(), User, s);	/* User: the current user name */
	if (cflag) {	/* remove entries in R_stat older than ca hours */
			/* used only by "uucp" or "root" */
 
		if ((strcmp(User,"uucp")!=SAME)&&(getuid()!=rid)) {
			fprintf(stderr,"Only uucp or root is allowed ");
			fprintf(stderr,"to use '-c' option\n");
			exit(1);
		}
		DEBUG(5, "enter clean mode, ca: %d\n", ca);
		fp=us_open(R_stat, "r", "dummy", 1, 1);
		if (fp==NULL) return(FAIL);
		sprintf(s, "%s/%s.%.7d",Spool,"rstat",getpid());
		DEBUG(5, "temp file: %s\n", s);
		fq=us_open(s, "w+", "dummy", 1, 1);
		if (fp==NULL) return(FAIL);
		while (fgets(buf, BUFSIZ, fp) != NULL) {
			sscanf(buf, "%*d%*s%*s%*ld%ld", &u.stime);
			if (older(u.stime, ca))	break;
			else fprintf(fq, "%s", buf);
		}
		fclose(fp);
		fclose(fq);
		if (xmv(s, R_stat) == FAIL)
			fprintf(stderr, "mv fails in uustat: %s\n", "-c");
		return(0);
	}
 
	if (mflag) {		/* print machine status */
		fp=us_open(L_stat, "r", "dummy", 1, 1);
		if (fp == NULL) return(FAIL);
		if (strcmp(ma,"all") == SAME) {
			while (fgets(buf, BUFSIZ, fp)!=NULL) {
				sscanf(buf,"%s%ld%d",ss.sysname,&ss.sti,&ss.sstat);
				sout(&ss);
			}
			return(0);
		}
		while (fgets(buf, BUFSIZ, fp) != NULL) {
			sscanf(buf,"%s%ld%d",ss.sysname,&ss.sti,&ss.sstat);
			if (strcmp(ma, ss.sysname)==SAME) {
				sout(&ss);
				return(0);
			}
		}
		fprintf(stderr, "system %s or its status unknown\n", ma);
		return(1);
	}
 
	/*
	 * Kill the job 'ka' and remove the Command file
	 * from spool directory. However, the D. files
	 * are not removed.
	 * Remove the entry from R_stat file and compact
	 * the hole. It is assumed that '-k' option
	 * will only be used occasionally.
	 */
 
	if (kflag) {
		FILE *pdirf;	/* for spool directory */
		char *name, file[100];
		int n;
 
		DEBUG(5, "enter kill loop, ka: %d\n", ka);
		fp=us_open(R_stat, "r", "dummy", 1, 1);
		if (fp==NULL) return(FAIL);
		sprintf(s, "%s/%s.%.7d",Spool,"rstat",getpid());
		DEBUG(5, "temp file: %s\n", s);
		fq=us_open(s, "w+", "dummy", 1, 1);
		if (fp==NULL) return(FAIL);
		while (fgets(buf, BUFSIZ, fp) != NULL) {
			sscanf(buf, "%d%s", &u.jobn, u.user);
			if ((u.jobn==ka) && ((strcmp(u.user,User)==SAME)
					    || (geteuid()==rid)))  {
				DEBUG(5, "Job %d is deleted\n", ka);
			/* delete the command file from spool dir. */
				chdir(Spool);	/* spool directory */
				if ((pdirf=fopen(Spool,"r"))==NULL) {
					perror(Spool);
					return(FAIL);
				}
				/* get next file name from directory 'pdirf' */
				while (gnamef(pdirf,file)) {
					if (file[0] != CMDPRE) continue;
					name = file + strlen(file) - 4;
					n = atoi(name);
					if (n == ka) {
						unlink(file);
						goto k1cont;
					}
				}
			}
			else fprintf(fq, "%s", buf);
		}
	k1cont:	/* copy the rest of R_stat file */
		while (fgets(buf, BUFSIZ, fp) != NULL)
			fprintf(fq, "%s", buf);
		fclose(fp);
		fclose(fq);
		if (xmv(s, R_stat) == FAIL)
			fprintf(stderr, "mv fails in uustat: %s\n", "-k");
		return(0);
	}
 
	fp=us_open(R_stat, "r", "dummy", 1, 1);
	if (fp == NULL) return(FAIL);
	while (fgets(buf, BUFSIZ, fp) != NULL) {
		sscanf(buf, "%d%s%s%ld%ld%o", &u.jobn, u.user,
			u.rmt, &u.qtime, &u.stime, &u.ustat);
		DEBUG(5, "user: %s ", u.user);
		DEBUG(5, " User: %s\n", User);
		if (jflag) {	/* print request status of job# 'ja' */
			if ((ja==-1) || (ja==u.jobn))	jout(&u);
		}

		else if (iflag) {
			if ((!uflag || (strcmp(u.user,ua) == SAME )) &&
			    (!sflag || (strcmp(u.rmt, sa) == SAME )) &&
			    (!oflag || older(u.qtime, oa)) &&
			    (!yflag || !older(u.qtime, ya)) )	jout(&u);
		}
 
		/* no option is given to "uustat" command,
		   print the status of all jobs isuued by
		   the current user	*/
		else if (strcmp(u.user, User) == SAME)
			jout(&u);
	}
}
 
sout(s)		/* print a record of us_ssf in L_stat file */
struct us_ssf *s;
{
	struct tm *tp, *localtime();
 
	tp = localtime(&s->sti);
	printf("%.7s\t%02.2d/%02.2d-%02.2d:%02.2d\t%s\n",s->sysname,tp->tm_mon+1,
		tp->tm_mday, tp->tm_hour, tp->tm_min, us_stext[s->sstat]);
	return(0);
}
 
jout(u)		/* print one line of job status in "u" */
struct us_rsf *u;
{
	register i, j;
	struct tm *tp, *localtime();
 
	tp = localtime(&u->qtime);
	printf("%-4.4d\t%.7s\t%.7s",u->jobn,u->user,u->rmt);
	printf("\t%02.2d/%02.2d-%02.2d:%02.2d", tp->tm_mon+1, tp->tm_mday,
		tp->tm_hour, tp->tm_min);
	tp = localtime(&u->stime);	/* status time */
	printf("\t%02.2d/%02.2d-%02.2d:%02.2d", tp->tm_mon+1, tp->tm_mday,
		tp->tm_hour, tp->tm_min);
	if (vflag)
		for (j=1, i=u->ustat; i>0; j++, i=i>>1) {
			if (i&01) printf("\n%s",us_rtext[j]);
		}
	else	printf("\t%o", u->ustat);
	printf("\n");
	return(0);
}
 
 
/* older(rt,t) rt: request time;	t: hours.
   return 1 if rt older than current time by t hours or more. */
older(rt,t)
short t;
time_t rt;
{
	time_t ct;		/* current time */
	time(&ct);
	return ((ct-rt) > (t*3600L));
}
 
#else
main(argc, argv)
char **argv;
{
	fprintf(stderr,"Uustat command does not exist on your system\n");
}
#endif
