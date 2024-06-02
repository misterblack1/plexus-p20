/*
 *	rjestat - rje status info and status console
 *
 *		rjestat [host]... [-shost] [-chost cmd]...
 */

#include <stdio.h>
#include <rje.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
 * common
 */
struct lines line[MAXLNS];	/* internal version of lines file */
int nlines;			/* number of lines in the line table */
char *sys;			/* PWB system name */
struct lines *host;		/* Current host system */
FILE *respfd;			/* RJE resp file descriptor */
FILE *outfd;			/* Output file descriptor */
jmp_buf stenv;			/* environment for returns */

/*
 * etc common
 */
int uid, retu();
struct lines *findhost();
char *sysname();
FILE *popen();
char *checkstat();

/*
 * Process arguments.  If no arguments are given
 * the status of every line hooked to this
 * system is printed. Valid arguments are:
 *	host - give the status of host, e.g. "A"
 *	-shost - start an interactive status terminal to host.
 *	-chost cmd - send cmd to host.
 */
main(argc,argv)
char *argv[];
{
	register struct lines *lp;	/* lines table pointer */
	register struct lines *end;	/* pointer past end of lines table */
	int streq;			/* status term. request flag */

	uid = getuid();
	sys = sysname();
	if((nlines=getlines(line,MAXLNS)) <= 0)
		error("can't read lines file\n");

	if(argc == 1) {
		/*
		 * Print status of every RJE connection
		 * on this PWB system.
		 */
		end = &line[nlines];
		for(lp=line; lp < end; lp++)
			if(strcmp(sys,lp->l_sys) == 0)
				prstat(lp);
	} else {
		streq = 0;
		/*
		 * Scan argument list
		 */
		for(argv++;--argc;argv++) {
			if(argv[0][0] == '-')
			switch(argv[0][1]) {
			case 's':
				streq++;
				if((host=findhost(argv[0]+2,1)) == NULL)
					error("%s bad host\n",argv[0]+2);
				break;
			case 'c':
				if((host=findhost(argv[0]+2,1)) == NULL)
					error("%s bad host\n",argv[0]+2);
				argc--;
				argv++;
				signal(SIGINT,retu);
				if(setjmp(stenv) != 0)
					exit(1);
				docmd(*argv);
				break;
			default:
				error("%s bad arg\n",argv[0]);
			}
			else {
				if((lp=findhost(*argv,0)) != NULL)
					prstat(lp);
				else
					error("%s bad host\n",argv[0]);
			}
		}
		if(streq)
			staterm();
	}
}

/*
 * Print the status of the RJE line passed
 */
prstat(lp)
register struct lines *lp;
{
	char *statmsg;
	char buf[150];
	FILE *fd;

	sprintf(buf,"%s/status",lp->l_dir);
	if((fd=fopen(buf,"r")) != NULL) {
		while(fgets(buf,150,fd) != NULL)
			fputs(buf,stdout);
		fclose(fd);
	}
	if((statmsg=checkstat(lp)) == NULL)
		printf("RJE to %s operating normally.\n",lp->l_host);
	else
		printf("RJE to %s down, reason: %s",lp->l_host,statmsg);
}

/*
 * Try to determine whether or not the
 * RJE line passed is operating.  Return
 * NULL if it seems OK otherwise return a
 * short messge that desribes why its sick
 */
char *
checkstat(lp)
register struct lines *lp;
{
	char file[100];
	FILE *fd;
	static char buf[100];
	struct stat sb;

	sprintf(file,"%s/dead",lp->l_dir);
	if((fd=fopen(file,"r")) != NULL) {
		if(fgets(buf,100,fd) == NULL)
			strcpy(buf,"Unknown\n");
		return(buf);
	}
	sprintf(file,"%s/stop",lp->l_dir);
	if(stat(file,&sb) == 0)
		return("Stopped by operator\n");
	return(NULL);
}

/*
 * Status terminal
 */
staterm()
{
	char buf[256];

	if(checkstat(host) != NULL)
		error("RJE to %s down, try later\n",host->l_host);
	/*
	 * Return after signal hit
	 */
	if(setjmp(stenv) != 0)
		putc('\n',stdout);
	signal(SIGINT,retu);

	for(;;) {
		printf("%s: ",host->l_host);
		if(gets(buf) == NULL)
			exit(0);
		docmd(buf);
	}
}

/*
 * Process status terminal commands.
 * There are three types of commands:
 *	1) !shell_line
 *	2) rjestat_internal_command (q for quit)
 *	3) host_command
 */
docmd(buf)
register char *buf;
{
	register char c;

	while(nmatch("[ \t]",buf) == 0)
		buf++;
	if((c = *buf) == '\0')
		return;
	else if(c  == '!')
		syscmd(++buf);
	else if(c == 'q')
		exit(0);
	else {
		hostcmd(buf);
	}
}


/*
 * Process host commands.  They are of the form:
 *	command [ pipeline ]
 */
hostcmd(buf)
char *buf;
{
	char cmd[256];		/* command to send */
	char pipeline[256];	/* output file or pipeline */
	int type;		/* type of pipeline */
	char resp[100];		/* RJE resp filename */

	/*
	 * Separate the command into pieces
	 * and check for legaliness.
	 */
	if((type=decode(buf,cmd,pipeline)) < 0 || !legalcmd(cmd)) {
		warning("%s illegal command\n",cmd);
		return;
	}
	/*
	 * Check for and open resp file
	 */
	sprintf(resp,"%s/resp",host->l_dir);
	if((respfd=fopen(resp,"r"))==NULL) {
		warning("RJE error: can't open resp file\n");
		return;
	}
	fseek(respfd,0L,2);
	/*
	 * Send the command
	 */
	if(checkstat(host) != NULL) {
		fclose(respfd);
		respfd = NULL;
		error("RJE to %s down, try later\n",host->l_host);
	}
	if(send(cmd) < 0) {
		warning("can't send\n");
		fclose(respfd);
		respfd = NULL;
		return;
	}
	/*
	 * Get response
	 */
	if(getresp(type,pipeline) < 0)
		warning("bad output file/pipe (%s)\n",pipeline);
	fclose(respfd);
	respfd = NULL;
}

/*
 * Output types (used in getresp and decode)
 */
#define PTTY 0
#define PIPE 1
#define AFILE 2
#define WFILE 3

/*
 * Break buf into cmd and pipeline.
 * Return the type of pipeline.
 */
decode(buf,cmd,pipeline)
	register char *buf;	/* input line */
	char *cmd;		/* place to save command */
	char *pipeline;		/* place to save IO redirection */
{
	register char *p;
	int type;

	/*
	 * Save cmd until a delimiter is seen
	 */
	p = cmd;
	while(nmatch("[>|\0]",buf) < 0)
		*p++ = *buf++;
	/*
	 * Back over trailing white space
	 */
	while(nmatch("[ \t]",--p) == 0)
		;
	*(++p) = '\0';
	/*
	 * Check IO redirect character
	 */
	if(*buf == '\0')
		return(PTTY);
	else if(*buf == '|' || *buf == '^') {
		type = PIPE;
		buf++;
	} else if(*buf == '>') {
		if(buf[1] == '>') {
			type = AFILE;
			buf += 2;
		} else {
			type = WFILE;
			buf++;
		}
	} else
		return(-1);
	/*
	 * Skip white space
	 */
	while(nmatch("[ \t]",buf) == 0)
		buf++;
	/*
	 * Save pipeline until end of string
	 */
	p = pipeline;
	while(*buf != '\0')
		*p++ = *buf++;
	*p = '\0';
	return(type);
}

/*
 * Process console output.
 */
getresp(type,pipeline)
char *pipeline;
{
	char buf[150];		/* line buffer */
	char ids[25];		/* id string */

	/*
	 * Open output file
	 */
	switch(type) {
	case PTTY:
		/* print response at tty */
		outfd = stdout;
		break;
	case PIPE:
		/* pipe into shell line */
		if((outfd=popen(pipeline,"w"))==NULL)
			return(-1);
		signal(SIGPIPE,retu);
		break;
	case AFILE:
		/* append to file */
		if((outfd=fopen(pipeline,"a"))==NULL)
			return(-1);
		break;
	case WFILE:
		/* Write to a file */
		if((outfd=fopen(pipeline,"w"))==NULL)
			return(-1);
		break;
	default:
		return(-1);
	}
	/*
	 * Look forward in resp file
	 * for our output
	 */
	sprintf(ids,"-UX%d",getpid());
	do {
		if(fgets(buf,150,respfd)==NULL) {
			sleep(3);
			if(checkstat(host) != NULL)
				error("RJE to %s has gone down, try later\n",host->l_host);
			continue;
		}
	} while(nmatch(ids,buf) != 0);
	/*
	 * Write it
	 */
	for(;;) {
		if(fgets(buf,150,respfd)==NULL) {
			sleep(1);
			continue;
		}
		if(nmatch(ids,buf) == 0)
			break;
		fputs(buf,outfd);
	}
	if(type == PIPE)
		pclose(outfd);
	else if(type != PTTY)
		fclose(outfd);
	outfd = NULL;
	return(0);
}

/*
 * Send cmd to host
 */
send(cmd)
char *cmd;
{
	char quefil[100];	/* file in squeue directory */
	char poolfil[100];	/* file in spool directory */
	char joblog[100];	/* joblog file */
	struct joblog jblr;	/* joblog record */
	char ids[25];		/* invalid command to identify output */
	register FILE *fd;
	register int n;

	/*
	 * Initialize strings
	 */
	jblr.j_uid = uid;
	jblr.j_lvl = (unsigned) 0;
	jblr.j_cnt = (long) 0;
	n = getpid();
	sprintf(joblog,"%s/joblog",host->l_dir);
	sprintf(quefil,"%s/squeue/co%d",host->l_dir,n);
	sprintf(poolfil,"%s/spool/co%d",host->l_dir,n);
	sprintf(jblr.j_file,"co%d",n);
	sprintf(ids,"$UX%d",n);
	/*
	 * Create file to be sent in spool
	 */
	if((fd=fopen(poolfil,"w"))==NULL) {
		warning("RJE error: can't write in spool\n");
		return(-1);
	}
	n = strlen(ids);
	putcard(fd,n,ids);
	putcard(fd,strlen(cmd),cmd);
	putcard(fd,n,ids);
	fclose(fd);
	/*
	 * Link into squeue and write joblog
	 */
	signal(SIGINT,SIG_IGN);
	if(link(poolfil,quefil) < 0) {
		warning("RJE error: can't link into squeue\n");
		unlink(poolfil);
		signal(SIGINT,retu);
		return(-1);
	}
	if((n=open(joblog,O_WRONLY|O_APPEND)) < 0) {
		warning("RJE error: can't open joblog\n");
		unlink(poolfil);
		signal(SIGINT,retu);
		return(-1);
	}
	write(n,&jblr,sizeof(struct joblog));
	close(n);
	unlink(poolfil);
	signal(SIGINT,retu);
	return(0);
}

/*
 * Write card image contained in buf, n
 * characters long, to fd out.
 */
putcard(out,n,buf)
register FILE *out;
register char *buf;
register int n;
{
	static char atoe[] = {	/* translation table */
#		include "atoe.h"
	};

	putc(n,out);
	while(n--)
		putc(atoe[*buf++],out);
}

/*
 * Return to staterm
 */
retu()
{
	alarm(0);
	if(respfd != NULL)
		fclose(respfd);
	if(outfd != NULL && outfd != stdout)
		fclose(outfd);
	longjmp(stenv,1);
}

/*
 * Determine if the command is legal
 * or not.  All commands are legal to RJE UID
 * and super user.  All other commands must
 * be display or inquiry commands If legal
 * return 1 else return 0.
 */
legalcmd(buf)
char *buf;
{
	if(uid==0 || uid==RJEUID)
		return(1);
	if(nmatch("\\$[dD]",buf) == 0)
		return(1);
	if(nmatch("*[iI]",buf) == 0)
		return(1);
	return(0);
}

/*
 * Search lines file for entry with
 * name as l_host connected to sys.
 * If flag is set check for inhibited
 * status terminal.  Return a pointer
 * to the line or NULL on failure.
 */
struct lines *
findhost(name,flag)
register char *name;
{
	register struct lines *lp,*end;

	end = &line[nlines];
	for(lp=line; lp < end; lp++ )
		if(strcmp(name,lp->l_host)==0) {
			if(strcmp(sys,lp->l_sys) != 0)
				continue;
			if(flag && nmatch("-:-:-: i",lp->l_parm)==0)
				continue;
			return(lp);
		}
	return(NULL);
}

/*
 * Give buf to a shell to execute
 */
syscmd(buf)
char *buf;
{
	int status,pid;
	int (*istat)();

	if((pid = fork()) == 0) {
		setuid(getuid());
		execl("/bin/sh", "sh", "-c", buf, 0);
		warning("can't exec /bin/sh\n");
		exit(1);
	}
	if(pid < 0) {
		error("can't fork, try again\n");
		return;
	}
	istat = signal(SIGINT,SIG_IGN);
	wait(&status);
	signal(SIGINT, istat);
}

/*
 * Print fatal error message and exit
 */
/*VARARGS1*/
error(s,a)
char *s,*a;
{
	fprintf(stderr,"rjestat: ");
	fprintf(stderr,s,a);
	exit(1);
}

/*
 * Print warning (diagnostic)
 */
/*VARARGS1*/
warning(s,a)
char *s,*a;
{
	fprintf(stderr,"rjestat: ");
	fprintf(stderr,s,a);
}
