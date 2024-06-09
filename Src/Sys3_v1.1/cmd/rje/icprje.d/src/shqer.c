/*
*	shqer - executes programs on returning RJE jobs.
*
*	shqer( )
*/

#include <rje.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ustat.h>
#include <sys/dir.h>

struct sque sq;		/* Shell queuer info structure */
struct stat stb;	/* Stat structure (for shqueue files) */
struct ustat ustb;	/* Ustat structure (for file sys space) */

char timez[12], lognm[16], logdir[48];
int efd;		/* error file descriptor */
dev_t ddv;		/* Device where exec file exists */
long siz;		/* File size holder */
char fspce[2];		/* File space flag to exec file */

char *envir[4];		/* Environment variables to be passed */

main(ac,av)
int ac;
char **av;
{
	register int delay, fd;
	int qfd, status, tmout, pid, i, sigalrm(), quit();
	char buf[QUELEN+1], dir[128], *strrchr();
	register struct sque *s;

/*
*	timez gets the time zone (TZ).
*	Set up signals.
*	Change to the QUEDIR directory.
*/

	setpgrp();
	delay = QDELAY;
	if(delay < 15)
		delay = 15;
	strcpy(timez,"TZ=");
	strcpy(timez+3,getenv("TZ"));
	for(i=1; i<SIGALRM; i++)
		signal(i,quit);
	signal(SIGTERM,quit);
	if(chdir(QUEDIR) != 0) {
		efd = 2;
		error("Can't change to ",QUEDIR);
		quit(1);
	}

/*
*       Catch alarms, set up file descriptors - /dev/null (0-2).
*	Set up error file.
*	Set up log file and initialize sque structure pointer.
*/

	signal(SIGALRM,sigalrm);
	for(i=0; i<20; i++)
		close(i);
	if(open("/dev/null",O_WRONLY) != 0) {
		efd = 2;
		error("Can't open ","/dev/null");
		quit(1);
	}
	dup(0);
	dup(1);
	unlink("errors");
	efd = creat("errors",0644);
	getlog();
	if((qfd = open("log",O_RDWR)) < 0) {
		error("Can't open ","log");
		quit(1);
	}
	s = &sq;

/*
*	Loop forever reading log entries.  If
*	there are none, pause for "delay".
*/

	for(;;) {
		while((status = read(qfd,buf,QUELEN)) > 0) {
			buf[status] = '\0';
			if((fd=open(buf,O_RDONLY)) < 0) {
				error("Can't open: ",buf);
				continue;
			}
			if(read(fd,s,sizeof(struct sque)) != sizeof(struct sque)) {
				error("Size error on: ",buf);
				continue;
			}
			close(fd);
			unlink(buf);
			strcpy(lognm,"LOGNAME=");
			strcat(lognm,s->sq_login);
			strcpy(logdir,"HOME=");
			strcat(logdir,s->sq_homed);
			strcpy(dir,s->sq_exfil);
			*(strrchr(dir,'/')) = 0;
			close(0);
			if(open(s->sq_infil,O_RDONLY) != 0) {
				error("Can't open: ",s->sq_infil);
				continue;
			}
			unlink(s->sq_infil);
/*	For-ever loop is indented for readability	*/
		for(;;) {
			if((pid = fork()) == 0) {
				if(stat(s->sq_exfil,&stb) < 0) {
					error("Can't find ",s->sq_exfil);
					exit(1);
				}
				ddv = stb.st_dev;
				siz = stb.st_size >> 9;
				fspce[0] = '?';
				if(ustat(ddv,&ustb) >= 0)
					if(ustb.f_tfree < (siz + s->sq_min) || ustb.f_tinode < 50)
						fspce[0] = '1';
					else
						fspce[0] = '0';
				nice(QNICE);
				setuid(SHQUID);
				envir[0] = timez;
				envir[1] = lognm;
				envir[2] = logdir;
				envir[3] = 0;
				chdir(dir);
				execle(s->sq_exfil,s->sq_exfil,fspce,s->sq_jobnm,s->sq_pgrmr,
					s->sq_jobno,s->sq_login,0,envir);
				execle("/bin/sh","sh",s->sq_exfil,fspce,s->sq_jobnm,s->sq_pgrmr,
					s->sq_jobno,s->sq_login,0,envir);
				error("Can't exec ",s->sq_exfil);
				exit(1);
			}
			alarm(delay);
			while(wait(&status) > 0);
			tmout = alarm(0);
			if(tmout > 10) {
				alarm(tmout);
				pause();
			}
			if(pid != -1)
				break;
		}
/*	End of For-ever indented loop	*/
		}
		alarm(delay);
		while(wait(&status) > 0);
		tmout = alarm(0);
		if(tmout > 10) {
			alarm(tmout);
			pause();
		}
	}
}

/*
*	getlog rebuilds the file log from the QUEDIR directory.
*	It ignores the files "errors" and "log".
*/

getlog()
{
	char name[QUELEN];
	int fd, dfd, dsize;
	struct direct dentry;
	register struct direct *dptr;

	dptr = &dentry;
	dsize = sizeof(struct direct);
	if((fd = creat("log",0666)) < 0) {
		error("Can't create ","log file");
		quit(1);
	}
	if((dfd = open(".",O_RDONLY)) < 0) {
		error("Can't read ","dir");
		quit(1);
	}
	for(;;) {
		if(read(dfd,dptr,dsize) <= 0)
			break;
		if(dptr->d_ino == 0 || dptr->d_name[0] == '.')
			continue;
		if(strcmp(dptr->d_name,"log") == 0)
			continue;
		if(strcmp(dptr->d_name,"errors") == 0)
			continue;
		if(stat(dptr->d_name,&stb) < 0) {
			error("Can't stat",dptr->d_name);
			continue;
		}
		strcpy(name,dptr->d_name);
		write(fd,name,QUELEN);
	}
	close(dfd);
	close(fd);
}

/*
*	sigalrm catches and resets the SIGALRM signal.
*/

sigalrm()
{
	signal(SIGALRM,sigalrm);
}

/*
*	error prints the error messages.
*/

error(s1,s2)
char *s1, *s2;
{
	write(efd,s1,strlen(s1));
	write(efd,s2,strlen(s2));
	write(efd,"\n",1);
}

/*
*	quit writes an error termination error, unlinks
*	the log (shqer not running) and exits.
*/

quit(x)
{
	error("ERROR ","TERMINATION");
	chdir(QUEDIR);
	unlink(QUELOG);
	exit(1);
}
