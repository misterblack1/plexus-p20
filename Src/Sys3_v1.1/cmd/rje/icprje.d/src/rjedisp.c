/*
 *	rjedisp - RJE output dispatcher
 *
 * 		rjedisp(  home , save , min )
 */

#include <stdio.h>
#include <utmp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ustat.h>
#include <rje.h>
#include <pwd.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>

/*
 * Events from xmit/recv 
 */
#define RECPRNT 0		/* received print file */
#define RECPNCH 1		/* received punch file */
#define RECMSG 2		/* received message */
#define JOBSTARTED 3		/* start of job transmision */
#define JOBSENT 4		/* completion of job transmission */

/*
 * File mode bits
 */
#define WDIR (S_IFDIR|02)	/* writable directory */
#define WREG (S_IFREG|02)	/* writable regular file */
#define XREG (S_IFREG|01)	/* executable regular file */

/*
 * Types of notification messages
 */
#define READY 0			/* output ready */
#define SENT 1			/* job transmitted */
#define SQUEUED 2		/* file queued */
#define JOBACKED 3		/* job acknowledged */
#define NOACK 4			/* job not acknowledged */
#define UNDELIV 5		/* undeliverable output */

/* etc */
#define BADMAIL "dead.letter"
#define QSIZE 6			/* xmitted job queue size */
#define NSEARCH 200		/* number of lines (cards) to search 
				   when looking for the usr card */



/*
 * Queue to hold jobs sent to host and not acknowledged
 */
struct jobque {
	int jq_front;
	int jq_rear;
	int jq_cnt;
	struct jqinfo {
		char file[NAMESZ];
		unsigned uid;
		int lvl;
	}jq_info[QSIZE];
};

/*
 * Queue full test macro. Returns 1 on
 * full else returns 0.
 */
#define qfull(x) ((jobq[x].jq_cnt == QSIZE)? 1:0)

/*
 * Job info collected from various places.  Kept
 * in a structure for convenience.
 */
struct jobdat {
	char jd_jobnm[9];	/* IBM job name */
	char jd_pgrmr[25];	/* IBM programmer's name */
	char jd_jobno[9];	/* IBM job number */
	char jd_login[9];	/* login from usr card */
	char jd_place[25];	/* place from usr card */
	short jd_lvl;		/* message level from usr card */
	char *jd_dir;		/* login directory from passwd file */
	unsigned jd_uid;	/* user id of login */
};

/*
 * Common variables
 */
FILE *respfd;			/* console resp file descr. */
FILE *acctfd;			/* acctlog file descr. */
int save;			/* save files flag */
long min;			/* minimum filesystem space */
char *home;			/* rje directory, e.g. /usr/hasp */
char jobdir[50];		/* "job" directory in home */
dev_t rjedev;			/* device where home resides */
char who[10];			/* prefix of argv[0] */
struct jobque jobq[MAXDEVS];	/* xmitted job queues */
int jobsout;			/* no. of jobs sent but not acknowledged */
struct jobdat jobdat;		/* job info */
int trunc;			/* truncation flag */

char *strrchr(),*strchr(),*ctime();
struct jqinfo *deljobq();
int sigterm(),sigalrm();

main(argc,argv)
char *argv[];
{
	struct dsplog dsplog;
	struct stat sb;
	long atol();

	/*
	 * Get params and initialize
	 */
	if(argc != 4) {
		errlog("arg count\n");
		goto badstart;
	}
	name(argv[0],"disp",who);
	home = argv[1];
	if(argv[2][0] == 'y')
		save++;
	min = atol(argv[3]);
	sprintf(jobdir,"%s/job",home);
	if((respfd=fopen("resp","r"))==NULL) {
		errlog("can't open resp file\n");
		goto badstart;
	}
	fseek(respfd,0L,2);
	if(stat("job",&sb) < 0) {
		errlog("can't stat job directory\n");
		goto badstart;
	}
	rjedev = sb.st_dev;
	if(stat("acctlog",&sb) == 0)
		acctfd = fopen("acctlog","a");
	if(chdir("rpool") < 0) {
		errlog("can't chdir to rpool\n");
		goto badstart;
	}
	umask(0);
	signal(SIGTERM,sigterm);
	/*
	 * Read and process commands
	 * from the recv/xmit procs.
	 */
	while(1) {
		if(read(DSPRD,&dsplog,sizeof(struct dsplog)) != sizeof(struct dsplog)) {
			errlog("pipe from xmit/recv broken\n");
			stoprje();
			reboot("RJE error\n",who,"../");
		}
		proccmd(&dsplog);
	}

badstart:
	stoprje();
	rjedead("RJE error\n",".");
}

/*
 * Process a command from the xmit/recv process.
 * Each command is a (struct dsplog).
 */
proccmd(dp)
register struct dsplog *dp;
{
	switch(dp->d_type) {
	case RECPRNT:
		doresp();
		trunc = dp->d_un.dspr.d_trunc;
		if(exprnt(dp->d_un.dspr.d_file) < 0) {
			acctng(dp->d_un.dspr.d_file,RJEUID,dp->d_un.dspr.d_cnt);

			dispatch(dp->d_un.dspr.d_file,"prnt");

/*
			bad(dp->d_un.dspr.d_file,"prnt");
*/
		} else {
			acctng(dp->d_un.dspr.d_file,jobdat.jd_uid,
							dp->d_un.dspr.d_cnt);
			dispatch(dp->d_un.dspr.d_file,"prnt");
		}
		break;
	case RECPNCH:
		doresp();
		trunc = dp->d_un.dspr.d_trunc;
		if(expnch(dp->d_un.dspr.d_file) < 0) {
			acctng(dp->d_un.dspr.d_file,RJEUID,dp->d_un.dspr.d_cnt);

			bad(dp->d_un.dspr.d_file,"pnch");

		} else {
			acctng(dp->d_un.dspr.d_file,jobdat.jd_uid,
							dp->d_un.dspr.d_cnt);
			dispatch(dp->d_un.dspr.d_file,"pnch");
		}
		break;
	case JOBSTARTED:
		jobstart(dp);
		break;
	case JOBSENT:
		notify((char *) 0,dp->d_un.x.d_uid,dp->d_un.x.d_lvl,SENT,dp->d_un.x.d_file);
		acctng(dp->d_un.x.d_file,dp->d_un.x.d_uid,dp->d_un.x.d_cnt);
		doresp();
		break;
	default:
		errlog("unknown command from pipe\n");
	}
}

/*
 * Extract print file info.  The file is assumed to
 * contain somewhere in the first NSEARCH lines a JOB
 * and a USR card.  If these can be found they
 * are loaded into the jobdat structure passed
 * and >= 0 is returned, otherwise -1 is returned.
 */
exprnt(file)
char *file;
{
	int jflag,uflag;
	char buf[150];
	struct passwd *pw;
	struct passwd *getpwnam();
	FILE *fd;
	int l;

	if((fd=fopen(file,"r")) == NULL) {
		errlog("can't open %s\n",file);
		return(-1);
	}
	jflag = uflag = 0;
	l = 0;
	while(l < NSEARCH && fgets(buf,150,fd) != NULL) {
		if(jflag && uflag)
			break;
		if(!jflag)
			jflag = jobcard(buf);
		if(!uflag)
			uflag = usrcard(buf);
		l++;
	}
	fclose(fd);
	if(!jflag) {
		strcpy(jobdat.jd_jobnm,":");
		strcpy(jobdat.jd_jobno,":");
		strcpy(jobdat.jd_pgrmr,":");
	}
	if(!uflag) {
		strcpy(jobdat.jd_login,RJELOGIN);
		return(-1);
	}
	if((pw=getpwnam(jobdat.jd_login)) == NULL) {
		errlog("can't find login %s\n",jobdat.jd_login);
		return(-1);
	}
	jobdat.jd_dir = pw->pw_dir;
	jobdat.jd_uid = pw->pw_uid;
	return(0);
}

/*
 * Test if buf is a jobcard. If so save the
 * job name, job number, and programmers name
 * in jobdat and return 1, else return 0
 */
jobcard(buf)
char *buf;
{
	char tbuf[164];
	static char job[] = {
		"-//$1  JOB 'JOB ' $2\n"
	};
	static char jes3no[] = {
		"'IAT2000' JOB $1 "
	};
	static char jes3nm[] = {
		"-//$1  JOB  ("
	};
	static char nprgmrnm[] = {
		"-),$1\n"
	};

	if(nmatch(job,buf,jobdat.jd_jobnm,jobdat.jd_jobno) == 2) {
		locase(jobdat.jd_jobnm);
		strcpy(jobdat.jd_pgrmr,":");
		if(nmatch(nprgmrnm,buf,tbuf) == 1) {
			if(setupname(tbuf) > 0) {
				strcpy(jobdat.jd_pgrmr,tbuf);
				locase(jobdat.jd_pgrmr);
			}
		}
		return(1);
	}
	if(nmatch(jes3no,buf,jobdat.jd_jobno) == 1)
		return(0);
	if(nmatch(jes3nm,buf,jobdat.jd_jobnm) == 1) {
		locase(jobdat.jd_jobnm);
		strcpy(jobdat.jd_pgrmr,":");
		return(1);
	}
	return(0);
}

/*
 * Test if buf is a usrcard. If so save the place,
 * login, and message level in jobdat and return
 * 1, else return 0.
 */
usrcard(buf)
char *buf;
{
	static char usr[] = {
		"'[Uu][Ss][Rr]=('$1,$2[,)]?)$3[,)]"
	};
	int n;
	char lvl[10];

	if((n=nmatch(usr,buf,jobdat.jd_login,jobdat.jd_place,lvl)) >= 2) {
		if(n==3)
			jobdat.jd_lvl = atoi(lvl);
		else
			jobdat.jd_lvl = 54;
		locase(jobdat.jd_login);
		locase(jobdat.jd_place);
		return(1);
	}
	return(0);
}

/*
 * Extract punch file info.  Search the first NSEARCH
 * lines for a USR card.  If it is found load the jobdat
 * info and return >=0, else return -1.
 */
expnch(file)
char *file;
{
	char buf[150];
	FILE *fd;
	int l,uflag;
	struct passwd *pw;
	struct passwd *getpwname();

	if((fd=fopen(file,"r")) == NULL) {
		errlog("can't open %s\n",file);
		return(-1);
	}
	uflag = 0;
	l = 0;
	while(l < 200 && getcard(fd,buf) >= 0 && !uflag) {
		if(!uflag)
			uflag = usrcard(buf);
		l++;
	}
	fclose(fd);
	strcpy(jobdat.jd_jobnm,":");
	strcpy(jobdat.jd_jobno,":");
	strcpy(jobdat.jd_pgrmr,":");
	if(!uflag) {
		strcpy(jobdat.jd_login,RJELOGIN);
		return(-1);
	}
	if((pw=getpwnam(jobdat.jd_login)) == NULL) {
		errlog("can't find login %s\n",jobdat.jd_login);
		return(-1);
	}
	jobdat.jd_dir = pw->pw_dir;
	jobdat.jd_uid = pw->pw_uid;
	return(0);
}

/*
 * Read a `card' image from FILE fd into buf
 */
getcard(fd,buf)
register FILE *fd;
register char *buf;
{
	static char etoa[] = {
#		include "../util/etoa.h"
	};
	register int n;

	if((n=getc(fd)) == EOF)
		return(-1);
	while(n--)
		*buf++ = etoa[getc(fd)];
	*buf++ = '\0';
	return(0);
}

/*
 * Process the job transmitted event
 */
jobstart(dp)
register struct dsplog *dp;
{
	register struct jqinfo *ip;

	if(dp->d_un.x.d_rdr < 0 || dp->d_un.x.d_rdr >= MAXDEVS)
		return;
	if(qfull(dp->d_un.x.d_rdr)) {
		ip = deljobq(dp->d_un.x.d_rdr);
		notify((char *)0,ip->uid,ip->lvl,NOACK,ip->file);
	}
	addjobq(dp->d_un.x.d_rdr, dp->d_un.x.d_file, dp->d_un.x.d_uid, dp->d_un.x.d_lvl);
}

/*
 * Process resp file messages
 */
doresp()
{
	register struct jqinfo *ip;
	char buf[150];
	int n;

	while(jobsout && fgets(buf,150,respfd) != NULL) {
		if((n=onrdr(buf)) >= 0) {
			ip = deljobq(n);
			notify((char *)0,ip->uid,ip->lvl,JOBACKED,ip->file);
		} else if((n=skipped(buf)) >= 0) {
			ip = deljobq(n);
			notify((char *)0,ip->uid,ip->lvl,NOACK,ip->file);
		}
	}
}

/*
 * Determine if buf is a job acknowledgement.
 * If so: pick up the job name and number and
 * return the reader number (0-6).  If not return -1.
 */
onrdr(buf)
char *buf;
{
	register int n;
	char rdrno[10];

	if(nmatch("-JOB $1 -HASP100 $2  ON R-RD$3 ",buf,jobdat.jd_jobno,
	jobdat.jd_jobnm,rdrno) < 3)
		return(-1);
	n = atoi(rdrno) - 1;
	if(n < 0 || n >= MAXDEVS)
		return(-1);
	locase(jobdat.jd_jobnm);
	return(n);
}

/*
 * Determine if buf is a job skipped message.
 * If so return the reader number (0-6) else
 * return -1.
 */
skipped(buf)
char *buf;
{
	register int n;
	char rdrno[10];

	if(nmatch("-HASP125 R-RD$1 ",buf,rdrno) < 1)
		return(-1);
	n = atoi(rdrno) - 1;
	if(n < 0 || n > MAXDEVS)
		return(-1);
	return(n);
}

/*
 * Dispatch files
 */
dispatch(file,prefix)
	char *file;
	char *prefix;
{
	char path[150];		/* login directory + jd_place */
	char parent[150];	/* parent of path */
	char dst[200];		/* destination file name */
	struct stat sb;

	if(jobdat.jd_place[0] != '/') {
		sprintf(path,"%s/%s",jobdat.jd_dir,jobdat.jd_place);
		strcpy(parent,path);
		*strrchr(parent,'/') = '\0';
	} else
		if(stat(jobdat.jd_place,&sb)>=0 && (sb.st_mode&XREG)==XREG) {
			sque(jobdat.jd_place,file);
			return;
		}

	if(stat(path,&sb) >= 0) {
		if((sb.st_mode&WDIR) == WDIR) {
			makefil(dst,path,prefix);
			move(file,prefix,dst,sb.st_dev);
			return;
		}
		else if((sb.st_mode&XREG) == XREG) {
			sque(path,file);
			return;
		}
		else if((sb.st_mode&WREG) == WREG) {
			move(file,prefix,path,sb.st_dev);
			return;
		}
		else {

			bad(file,prefix);
			return;
		}
	}
	else if(stat(parent,&sb)>=0 && (sb.st_mode&WDIR)==WDIR) {
		move(file,prefix,path,sb.st_dev);
		return;
	}
	else {

		bad(file,prefix);
		return;
	}
}

/*
 * Generate and send notification messages
 */
notify(login,uid,level,type,arg)
	char *login;	/* login name (or NULL) */
	unsigned uid;	/* login uid if login==NULL */
	int level;	/* message level from USR card */
	int type;	/* type of message */
	char *arg;	/* argument for message */
{
	long tsec;	/* clock time */
	char times[10];	/* time string */
	char rjemesg[256];
	struct tm *tp,*localtime();
	struct passwd *pw,*getpwuid();
	register int n;

	time(&tsec);
	tp = localtime(&tsec);
	sprintf(times,"%02d:%02d:%02d",tp->tm_hour,tp->tm_min,tp->tm_sec);
	switch(type) {
	case READY:
		sprintf(rjemesg,"\n%s %s job %s -- %s ready%s\n\n",
		times, jobdat.jd_jobnm, jobdat.jd_jobno, arg, 
		trunc? " (truncated)" : "");
		n = 6;
		break;
	case SENT:
		sprintf(rjemesg,"\n%s %s transmitted\n\n",times,arg);
		n = 3;
		break;
	case SQUEUED:
		sprintf(rjemesg,"\n%s %s job %s -- %s queued for execution\n\n",
		times,jobdat.jd_jobnm,jobdat.jd_jobno,arg);
		n = 6;
		break;
	case JOBACKED:
		sprintf(rjemesg,"\n%s %s job %s -- %s acknowledged\n\n",
		times,jobdat.jd_jobnm,jobdat.jd_jobno,arg);
		n = 5;
		break;
	case NOACK:
		sprintf(rjemesg,"\n%s %s not acknowledged\n\n",times,arg);
		n = 5;
		break;
	case UNDELIV:
		sprintf(rjemesg,"\n%s job %s -- %s undeliverable\n\n",jobdat.jd_jobnm,jobdat.jd_jobno,arg);
		n = 6;
	}
	if(login == (char *) 0) {
		if((pw=getpwuid(uid)) == NULL) {
			errlog("can't find uid %d\n",uid);
			mail(rjemesg,RJELOGIN);
			return;
		}
		login = pw->pw_name;
	}
	if(n >= (level%10) && ptty(rjemesg,login) >= 0)
		return;
	if(n >= (level/10))
		if(mail(rjemesg,login) < 0) {
			unlink(BADMAIL);
			errlog("can't mail to %s\n",login);
			mail(rjemesg,RJELOGIN);
		}
}

/*
 * Write mesg to user's terminal.  Return -1
 * if mesg can't be sent, 0 otherwise.
 */
ptty(mesg,user)
char *mesg,*user;
{
	char *tty,*findtty();
	FILE *fd;

	if((tty=findtty(user))==0)
		return(-1);
	if((fd=fopen(tty,"w"))==NULL)
		return(-1);
	signal(SIGALRM,sigalrm);
	alarm(10);
	fputc('\07',fd);
	fflush(fd);
	sleep(2);
	fputc('\07',fd);
	fputs(mesg,fd);
	alarm(0);
	fclose(fd);
	return(0);
}

/*
 * Find the first tty that user is logged onto.
 */
char *
findtty(user)
char *user;
{
	struct utmp utmp;
	static char tty[15];
	int fd;

	if((fd=open("/etc/utmp",O_RDONLY)) >= 0) {
		while(read(fd,&utmp,sizeof(struct utmp)) == sizeof(struct utmp))
			if(strncmp(utmp.ut_name,user,8)==0) {
				sprintf(tty,"/dev/%s",utmp.ut_line);
				close(fd);
				return(tty);
			}
		close(fd);
	}
	return(0);
}


/*
 * Mail mesg to login.  Return 0 on
 * success, -1 on failure.
 */
mail(mesg,login)
char *mesg,*login;
{
	int p[2],status,pid,i;

	if(pipe(p) < 0)
		return(-1);
	if((pid=fork()) == 0) {
		close(0);
		dup(p[0]);
		for(i=1; i < 20; i++)
			close(i);
		execl("/bin/mail","mail",login,0);
		exit(1);
	}
	if(pid < 0) {
		errlog("can't fork\n");
		close(p[0]);
		close(p[1]);
		return(-1);
	}
	close(p[0]);
	write(p[1],mesg,strlen(mesg));
	close(p[1]);
	wait(&status);
	return((status==0)? 0:-1);
}

/*
 * Copy/link files, space permitting
 */
move(src,prefix,dst,ddev)
	char *src;		/* source file name */
	char *prefix;		/* "prnt" or "pnch" */
	char *dst;		/* destination file name */
	dev_t ddev;		/* destination device */
{
	char buf[512];
	struct stat sb;
	struct ustat usb;
	int sfd,dfd,n;

	if(rjedev == ddev) {
		unlink(dst);
		link(src,dst);
		chmod(dst,0454);
		unlink(src);
	} else {
		stat(src,&sb);
		ustat(ddev,&usb);
		if(usb.f_tfree - (sb.st_size>>9) < min) {
			makefil(dst,jobdir,prefix);
			link(src,dst);
			chmod(buf,0454);
			unlink(src);
		} else {
			sfd = open(src,O_RDONLY);
			dfd = creat(dst,0454);
			while((n=read(sfd,buf,512)) != 0)
				write(dfd,buf,n);
			close(sfd);
			close(dfd);
			unlink(src);
		}
	}
	notify(jobdat.jd_login,0,jobdat.jd_lvl,READY,dst);
}


/*
 * Queue files for the shell queuer
 * to execute.
 */
sque(prog,infil)
char *prog;
char *infil;
{
	struct sque qent;		/* queue entry */
	char tmpfil[150];		/* temporary file */
	char logfil[150];		/* shqer log file */
	int fd;
	int pid;

	strcpy(qent.sq_exfil,prog);
	sprintf(qent.sq_infil,"%s/rpool/%s",home,infil);
	strcpy(qent.sq_jobnm,jobdat.jd_jobnm);
	strcpy(qent.sq_pgrmr,jobdat.jd_pgrmr);
	strcpy(qent.sq_jobno,jobdat.jd_jobno);
	strcpy(qent.sq_login,jobdat.jd_login);
	strcpy(qent.sq_homed,jobdat.jd_dir);
	qent.sq_min = min;
	makefil(tmpfil,QUEDIR,"tmp");
	if((fd=creat(tmpfil,0644)) < 0) {
		errlog("can't create file squeue\n");
		return;
	}
	write(fd,&qent,sizeof(struct sque));
	close(fd);
	sprintf(logfil,"%s/%s",QUEDIR,QUELOG);
	if((fd=open(logfil,O_WRONLY|O_APPEND)) < 0) {
		errlog("restarting shqer\n");
		if((pid=fork()) == 0) {
			execl("/usr/rje/shqer","shqer",0);
			errlog("can't exec shqer\n");
		}
		if(pid < 0)
			errlog("can't fork\n");
		return;
	}
	write(fd,tmpfil,QUELEN);
	close(fd);
	notify(jobdat.jd_login,0,jobdat.jd_lvl,SQUEUED,prog);
	return;
}

/*
 * Save or remove undeliverable files
 */
bad(file,prefix)
	char *file;		/* received file name */
	char *prefix;		/* "prnt" or "pnch" */
{
	char dst[150];

	if(save) {
		makefil(dst,jobdir,prefix);
		link(file,dst);
		chmod(dst,0454);
		notify(jobdat.jd_login,0,0,UNDELIV,dst);
		unlink(file);
		return;
	}
	notify(jobdat.jd_login,0,0,UNDELIV,"");
	unlink(file);
}

/*
 * Generate a filename in directory dir,
 * with prefix prefix returned in buf
 */
makefil(buf,dir,prefix)
char *buf,*dir,*prefix;
{
	int count;
	struct stat sb;

	count = 0;
	do {
		sprintf(buf,"%s/%s%d",dir,prefix,count);
		count++;
	} while(stat(buf,&sb) >= 0);
	return;
}

/*
 * Convert string to lower case
 */
locase(s)
register char *s;
{
	register char c;

	while(c = *s)
		if(c >= 'A' && c<='Z')
			*s++ = c + 'a' - 'A';
		else
			s++;
}

/*
 * Log an error in the errlog file
 */
/*VARARGS1*/
errlog(s,a)
char *s,*a;
{
	char buf[100];

	sprintf(buf,s,a);
	write(ERRFD,buf,strlen(buf));
}

/*
 * Add the info passed to the rear of jobq no. (0-6).
 * Return -1 on overflow, 0 otherwise.
 */
addjobq(no,file,uid,lvl)
char *file;
unsigned uid;
{
	register struct jobque *jqp;
	register struct jqinfo *ip;

	jqp = &jobq[no];
	if(jqp->jq_cnt == QSIZE)
		return(-1);
	ip = &(jqp->jq_info[jqp->jq_rear]);
	strcpy(ip->file,file);
	ip->uid = uid;
	ip->lvl = lvl;
	jqp->jq_cnt++;
	jqp->jq_rear = ++jqp->jq_rear % QSIZE;
	jobsout++;
	return(0);
}

/*
 * Delete the jqinfo structure from the front of
 * jobq no. (0-6).  Return a pointer to it or NULL
 * on underflow.
 */
struct jqinfo *
deljobq(no)
{
	register struct jobque *jqp;
	register struct jqinfo *ip;

	jqp = &jobq[no];
	if(jqp->jq_cnt == 0)
		return((struct jqinfo *)0);
	ip = &(jqp->jq_info[jqp->jq_front]);
	jqp->jq_cnt--;
	jqp->jq_front = ++jqp->jq_front % QSIZE;
	jobsout--;
	return(ip);
}

/*
 * Stop this RJE subsystem
 */
stoprje()
{
	signal(SIGTERM,SIG_IGN);
	kill(0,SIGTERM);
}

/*
 * Clean termination
 */
sigterm()
{
	struct dsplog dsplog;

	signal(SIGTERM,SIG_IGN);
	while(read(DSPRD,&dsplog,sizeof(struct dsplog)) == sizeof(struct dsplog))
		proccmd(&dsplog);
	exit(0);
}

/*
 * Catch SIGALRM
 */
sigalrm()
{
}

/*
 * If FILE acctfd is open write an accounting
 * record into.  Format:
 *	month/day   hour:min:sec   file   uid   cnt
 */
acctng(file,uid,cnt)
	char *file;		/* file received/transmitted */
	unsigned uid;		/* sender/receiver */
	long cnt;		/* number of records */
{
	long t;			/* clock time */
	struct tm *tp;		/* pointer to time struct */
	struct tm *localtime();

	if(acctfd == NULL)
		return;
	time(&t);
	tp = localtime(&t);
	fprintf(acctfd,"%02d/%02d  %02d:%02d:%02d  %s  %u  %ld\n",
	tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec,file,uid,cnt);
	fflush(acctfd);
}

/*
 *	This routine goes through the grossness needed
 *	to extract the programmer name from the job card.
 */

setupname(p)
register char *p;
{
	register char c;
	int flag, quoted=0;
	char *p0;

	p0 = p;
	for(flag=0;;) {
		if((c=(*p++)) == '\'') {
			if((c=(*p++)) != '\'') {
				flag = ~flag;
				quoted++;
			}
		}
		if(c == '\0')
			break;
		if(flag == 0) {
			if(c == ' ' || c == ',')
				break;
			if(c == '=')
				return(-1);
		}
	}
	if(((--p)-p0) > 20)
		return(-1);
	if(quoted) {
		*(--p) = '\0';
		strcpy(p0,p0 + 1);
	} else
		*p = '\0';
	return(1);
}
