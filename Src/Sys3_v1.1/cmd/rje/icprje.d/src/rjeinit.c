/*
*	rjeinit - reboots RJE
*
*	rjeinit [ + ]
*/

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ustat.h>
#include <sys/dir.h>
#include <signal.h>
#include <errno.h>
#include <rje.h>

extern int errno;

#define ERR	-1
#define FALSE	0
#define DEV	"/dev/"
#define S_RCB	0xf0	/* RCB for control record */
#define S_SRCB	0xc1	/* Signon SRCB */
#define S_EOR	0x00	/* End of record */
#define BLNK	0x40	/* Hex space character */


char trt[256] = {	/*	Conversion tables for 8-bit ASCII to EBCDIC	*/
	 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	0x40,0x5A,0x7F,0x7B,0x5B,0x6C,0x50,0x7D,
	0x4D,0x5D,0x5C,0x4E,0x6B,0x60,0x4B,0x61,
	0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,
	0xF8,0xF9,0x7A,0x5E,0x4C,0x7E,0x6E,0x6F,
	0x7C,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
	0xC8,0xC9,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,
	0xD7,0xD8,0xD9,0xE2,0xE3,0xE4,0xE5,0xE6,
	0xE7,0xE8,0xE9,0xAD,0xE0,0xBD,0x9A,0x6D,
	0x79,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
	0xC8,0xC9,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,
	0xD7,0xD8,0xD9,0xE2,0xE3,0xE4,0xE5,0xE6,
	0xE7,0xE8,0xE9,0xC0,0x4F,0xD0,0x5F, 0  ,
	0x20,0x21,0x22,0x23,0x24,0x15,0x06,0x17,
	0x28,0x29,0x2A,0x2B,0x2C,0x09,0x0A,0x1B,
	0x30,0x31,0x1A,0x33,0x34,0x35,0x36,0x08,
	0x38,0x39,0x3A,0x3B,0x04,0x14,0x3E,0xE1,
	0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,
	0x49,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
	0x58,0x59,0x62,0x63,0x64,0x65,0x66,0x67,
	0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,
	0x76,0x77,0x78,0x80,0x8A,0x8B,0x8C,0x8D,
	0x8E,0x8F,0x90,0x6A,0x9B,0x9C,0x9D,0x9E,
	0x9F,0xA0,0xAA,0xAB,0xAC,0x4A,0xAE,0xAF,
	0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
	0xB8,0xB9,0xBA,0xBB,0xBC,0xA1,0xBE,0xBF,
	0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xDA,0xDB,
	0xDC,0xDD,0xDE,0xDF,0xEA,0xEB,0xEC,0xED,
	0xEE,0xEF,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,
	};

	/* Place holders for program/file names */

char who[12], xmit[12], recv[12], disp[12];
char device[32], home[32], joblog[40], errlog[40];
char *signon = "signon";
char *stopf = "stop";
char *deadf = "dead";

	/* Lines file parameters */

long fsmin, atol();
char *rdrs, *prnts, *pnchs;
char *save, *inhib, *phone, *min, *max;

int r_boot;
struct lines lnsbuf[MAXLNS];		/* Lines file buffers */
struct loghdr hdr;			/* joblog file header info */
struct stat stb;

main(ac,av)
char **av;
int ac;
{
	struct ustat ustb;
	int t, acu, sig15();
	int xmtpipe[2], dspipe[2], pid, alarmsig();
	long lseek();

/*
*	Set the process group.
*	Reset umask for proper file creation.
*	Initialize, close all but error file descriptor (2),
*	and ignore all signals.
*/

	setpgrp();
	umask(0);
	r_boot = t = 0;
	close(0);
	close(1);
	signal(SIGHUP,SIG_IGN);
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	signal(SIGALRM,alarmsig);
	for(t = 3; t < 20; t++)
		close(t);

/*
*	Put init prefix into "who". Initval sets up the values
*	for program/file names and Lines table entries. If
*	arg1 is a "+" this is a reboot, but wait for children.
*/

	name(*av,"init",who);
	initval();
	if(ac > 1)
		if(**(++av) == '+') {
			r_boot = 1;
			while(wait(&t) != -1);
		}

/*
*	Check the home directory. Check the uid and setuid.
*	Check the file system free blocks and inodes.
*/

	if(chdir(home) < 0 || stat(".",&stb) < 0)
		error("Can't chdir to %s\n",home,1);
	if((stb.st_uid != geteuid()) && setuid(stb.st_uid) < 0)
		error("Wrong User Id\n","",0);
	if(ustat(stb.st_dev,&ustb) >= 0) {
		if(ustb.f_tfree < 100 || ustb.f_tinode < 10)
			error("File System %s exhausted\n",ustb.f_fname,0);
		if(ustb.f_tfree < (fsmin + fsmin/2) || ustb.f_tinode < 100)
			if(!r_boot)
				fprintf(stderr,
				"%sinit: Warning - only %ld blocks, %d inodes free\n",
				who,ustb.f_tfree,ustb.f_tinode);
	}

/*
*	If reboot and stopfile, exit.  Otherwise unlink stop file.
*	Open the RJE device. Sleep 1 minute if reboot, dial-up.
*/

	if(r_boot)
		sleep(60);
	if(stat(stopf,&stb) >= 0) {
		if(r_boot)
			exit(0);
		unlink(stopf);
	}
	if(open(device,O_RDWR) != DEVFD) {
		if(errno == EBUSY)
			error("Can't open %s (already open)\n",device,0);
		if(errno == EACCES)
			error("Can't open %s (script not running)\n",device,0);
		error("Can't open %s\n",device,0);
	}
	if(*phone) {
		strcat(phone,"-<");
		if((acu = open(RJECU,O_WRONLY)) < 0)
			error("Can't open %s\n",RJECU,1);
		if(write(acu,phone,strlen(phone)) < 0)
			error("Can't write %s\n",RJECU,1);
		close(acu);
		fprintf(stderr,"Number dialed and %s closed\n",RJECU);
	}

/*
*	Send signon card.
*	Create "resp" file.
*	Create the xmit and disp pipes.
*	unlink the dead file.
*	fork and set the new process group.
*	set up the error logging file.
*	rebuild the joblog (getjobs).
*	Open the joblog for reading, skip header.
*	fork and exec xmit, disp, and recv.
*	redisp notifies disp of files to be done.
*	This program execs recv.
*/

	unlink("resp");
	if((t=creat("resp",0644)) < 0)
		error("Can't create %s\n","resp",1);
	close(t);
	if(pipe(xmtpipe) < 0)
		error("Can't pipe (%s)\n",xmit,1);
	if(xmtpipe[0] != XMTRD || xmtpipe[1] != XMTWR)
		error("Bad pipe (%s)\n",xmit,1);
	if(pipe(dspipe) < 0)
		error("Can't pipe (%s)\n",disp,1);
	if(dspipe[0] != DSPRD || dspipe[1] != DSPWR)
		error("Bad pipe (%s)\n",disp,1);
	unlink(deadf);
	if((pid = fork()) == 0) {
		setpgrp();
		hdr.h_pgrp = getpgrp();
		close(ERRFD);
		if(r_boot) {
			if(open(errlog,(O_WRONLY|O_APPEND)) != ERRFD)
				error("Can't open %s\n",errlog,1);
			lseek(ERRFD,0L,2);
		} else {
			unlink(errlog);
			if(creat(errlog,0644) != ERRFD)
				error("Can't create %s\n",errlog,1);
		}

		sinon();
		getjobs();
		if(open(joblog,O_RDONLY) != JBLOG)
			error("Can't open %s\n",joblog,1);
		lseek(JBLOG,(long)sizeof(struct loghdr),0);
		if((pid = fork()) == 0) {
			close(DSPRD);
			close(XMTWR);
			execl(xmit,xmit,home,rdrs,0);
			error("Can't execute %s\n",xmit,1);
		}
		if(pid == ERR)
			error("Can't fork %s\n",xmit,1);
		if((pid = fork()) == 0) {
			close(DEVFD);
			close(XMTRD);
			close(XMTWR);
			close(DSPWR);
			execl(disp,disp,home,save,min,0);
			error("Can't execute %s\n",disp,1);
		}
		if(pid == ERR)
			error("Can't fork %s\n",disp,1);
		close(XMTRD);
		close(DSPRD);
		close(JBLOG);
		redisp();
		execl(recv,recv,min,max,0);
		error("Can't exec %s\n",recv,1);
	}
	if(pid == ERR)
		error("Cannot fork %sinit\n",who,1);
	alarm(5);
	if(wait(&t) > 0) {
		alarm(0);
		while(wait(&t) > 0);
		fprintf(stderr,"%s not started\n",who);
		exit(1);
	}
	alarm(0);
	fprintf(stderr,"%s started\n",who);
	exit(0);
}

/*
*	error prints fatal errors, creates a stop file if sflag
*	is non-zero, and exits.
*/

error(str,arg,sflg)
char *str, *arg;
int sflg;
{
	fprintf(stderr,"%sinit failed: ",who);
	fprintf(stderr,str,arg);
	if(sflg) {
		chdir(home);
		creat(stopf,0644);
		rjedead("Init failed\n",home);
	}
	kill(0,SIGTERM);
	exit(1);
}

/*
*	initval initializes the names of programs and files,
*	and sets the arguments from the "lines" file.
*/

initval()
{
	register struct lines *lnptr=FALSE;
	register int cnt;
	register char *ptr;
	int rds, prs, pns;
	char *advance();

	strcpy(xmit,who);
	strcpy(recv,who);
	strcpy(disp,who);
	strcat(xmit,"xmit");
	strcat(recv,"recv");
	strcat(disp,"disp");
	if((cnt = getlines(lnsbuf,MAXLNS)) == ERR)
		error("Can't read lines file\n","",1);
	while(cnt > 0)
		if(strcmp(lnsbuf[--cnt].l_prefix,who) == 0) {
			lnptr = &lnsbuf[cnt];
			break;
		}
	if(lnptr == FALSE)
		error("No entry for %s in lines file\n",who,1);
	strcpy(device,DEV);
	strcat(device,lnptr->l_dev);
	strcpy(home,lnptr->l_dir);
	strcpy(joblog,home);
	strcpy(errlog,home);
	strcat(errlog,"/errlog");
	strcat(joblog,"/joblog");
	rdrs = ptr = lnptr->l_peri;
	if((rds = atoi(rdrs)) > MAXDEVS)
		error("RJE cannot handle %d readers\n",rds,0);
	prnts = ptr = advance(ptr,':');
	if((prs = atoi(prnts)) > MAXDEVS)
		error("RJE cannot handle %d printers\n",prs,0);
	pnchs = ptr = advance(ptr,':');
	if((pns = atoi(pnchs)) > MAXDEVS)
		error("RJE cannot handle %d punches\n",pns,0);
	min = ptr = lnptr->l_parm;
	fsmin = atol(ptr);
	max = ptr = advance(ptr,':');
	ptr = advance(ptr,':');
	save = ptr;
	ptr = advance(ptr,':');
	inhib = ptr;
	ptr = advance(ptr,':');
	phone = ptr;
	save = (*save) ? save : "n";
	inhib = (*inhib) ? inhib : "n";
}

/*
*	getjob rebuilds the joblog file from the squeue directory.
*/

getjobs()
{
	struct direct dentry;
	struct joblog lbuf;
	register struct direct *dptr;
	register struct joblog *jptr;
	int dfd, jfd, dsize, jsize;

	dptr = &dentry;
	jptr = &lbuf;
	dsize = sizeof(struct direct);
	jsize = sizeof(struct joblog);
	if((jfd = creat("joblog",0644)) < 0)
		error("Can't create %s\n","joblog",1);
	if(chdir("squeue") < 0)
		error("Can't chdir to %s\n","squeue",1);
	if((dfd = open(".",O_RDONLY)) < 0)
		error("Can't read %s\n","squeue",1);
	write(jfd,&hdr,sizeof(struct loghdr));
	for(;;) {
		if(read(dfd,dptr,dsize) <= 0)
			break;
		if(dptr->d_ino == 0 || dptr->d_name[0] == '.')
			continue;
		if(stat(dptr->d_name,&stb) < 0) {
			fprintf(stderr,"Can't stat %s\n",dptr->d_name);
			continue;
		}
		strcpy(jptr->j_file,dptr->d_name);
		jptr->j_uid = (unsigned) stb.st_uid;
		jptr->j_cnt = 99;
		jptr->j_lvl = 0;
		write(jfd,jptr,jsize);
	}
	chdir("..");
	close(dfd);
	close(jfd);
}

/*
*	redisp requeues the recieved jobs in the disp pipe (DSPWR)
*	from the rpool (recv-pool) directory.
*/

redisp()
{
	struct direct dentry;
	struct dsplog dlog;
	register struct direct *dptr;
	register int dsize, rfd;

	dptr = &dentry;
	dsize = sizeof(struct direct);
	if(chdir("rpool") < 0)
		error("Can't chdir to %s\n","rpool",1);
	if((rfd = open(".",O_RDONLY)) < 0)
		error("Can't read %s\n","rpool",1);
	for(;;) {
		if(read(rfd,dptr,dsize) <= 0)
			break;
		if(dptr->d_ino == 0 || dptr->d_name[0] == '.')
			continue;
		if(strncmp(dptr->d_name,"pr",2) == 0)
			dlog.d_type = 0;
		else
			if(strncmp(dptr->d_name,"pu",2) == 0)
				dlog.d_type = 1;
			else
				continue;
		strcpy(dlog.d_un.dspr.d_file,dptr->d_name);
		dlog.d_un.dspr.d_cnt = -1;
		write(DSPWR,&dlog,sizeof(struct dsplog));
	}
	chdir("..");
	close(rfd);
}

/*
*	advance returns the address following "c" from "p".
*	It is used to parse the parameters field from "lines".
*/

char *
advance(p,c)
register char *p, c;
{
	while((*p) && *p != c)
		p++;
	if(*p)
		*p++ = '\0';
	return(p);
}

/*
*	sinon signs onto the remote system if there is a signon
*	file available.
*/

sinon()
{
	char sbufa[80], sbufe[86];
	register char *pe, *pa;
	register int i;
	int sfd, t;

	pe = sbufe;
	pa = sbufa;
	*pe++ = S_RCB;
	*pe++ = S_SRCB;
	i = 0;
	if ((sfd = open(signon,O_RDONLY)) > 0) {
		t = read(sfd,sbufa,80);
		for(; i < t; i++) {
			if(*pa != '\n')
				*pe++ = trt[(unsigned)*pa++];
			else
				break;
		}
		close(sfd);
	}
	for(;i < 80; i++)
		*pe++ = BLNK;
	*pe++ = S_EOR;
	*pe++ = S_EOR;
	t = pe - sbufe;
	sfd = write(DEVFD,sbufe,t);
	if(sfd != t) {

		fprintf(stderr,"t=%x\n", t);
		fprintf(stderr, "count sent=%x\n", sfd);

		if(stat(stopf,&stb) >= 0)
			exit(0);
		if(!r_boot)
			fprintf(stderr,"Signon error - rebooting\n");
		reboot("Signon err - rebooting\n",who,home);
		exit(1);
	}
}
/*
*	alarmsig catches SIGALRM and resets it.
*/

alarmsig()
{
	signal(SIGALRM,alarmsig);
}
