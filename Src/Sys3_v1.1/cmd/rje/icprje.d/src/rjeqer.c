/*
*	rjeqer - Queues sent files in the joblog
*		Must have setuid bit.
*
*	rjeqer( dir , file , cnt , lvl )
*/

#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <rje.h>

struct joblog lbuf;

main(ac,av)
int ac;
char **av;
{
	struct stat stb;
	char jblog[128], filenm[24];
	int logfd;
	long lseek(),atol();
	register char *dir;
	register struct joblog *jptr;

	if(ac != 5)
		error("%s: Arg count\n","Queuer");

/*
*	Initialization of joblog path name (jblog),
*	file name to be linked to (filenm),
*	and the joblog structure (lbuf).
*/

	jptr = &lbuf;
	strcpy(jblog,*(++av));
	dir = *av;
	if(chdir(dir) < 0 || chdir("spool") < 0)
		error("Can't chdir to %s/spool",dir);
	strcat(jblog,"/joblog");
	strcpy(filenm,"../squeue/");
	strcat(filenm,*(++av));
	strncpy(jptr->j_file, *av, sizeof(jptr->j_file));
	jptr->j_uid = (unsigned) getuid();
	jptr->j_cnt = atol(*(++av));
	jptr->j_lvl = atoi(*(++av));

/*
*	Check for sent file and proper owner
*/

	if(stat(jptr->j_file,&stb) < 0)
		error("No file for queuer (%s)\n",jptr->j_file);
	if(((unsigned)stb.st_uid) != jptr->j_uid)
		error("Wrong Owner for queuer (%s)\n",jptr->j_file);

/*
*	Ignore breaks and hangups
*/

	signal(SIGHUP,SIG_IGN);
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);

/*
*	Create lock, write joblog data at end of file,
*	unlock, link/unlink, and print Queued message
*/

	if(link(jptr->j_file,filenm) < 0)
		error("Can't link %s in squeue\n",jptr->j_file);
	if((logfd = open(jblog,(O_WRONLY|O_APPEND))) < 0)
		error("Queuer: Cannot open %s\n",jblog);
	lseek(logfd,0L,2);
	if(write(logfd,&lbuf,sizeof(struct joblog)) < 0)
		error("Can't write %s\n",jblog);
	close(logfd);
	fprintf(stdout,"Queued as %s/squeue/%s\n",dir,jptr->j_file);
	if(unlink(jptr->j_file) < 0)
		error("Warning - %s not unlinked\n",jptr->j_file);
}

/*
*	Error handles queuer errors.
*/

error(str,arg)
register char *str, *arg;
{
	fprintf(stderr,str,arg);
	exit(1);
}
