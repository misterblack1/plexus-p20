	/* us_crs	1.0	6/21/79	*/
#include "uucp.h"
#include <sys/types.h>
#include "uust.h"
 
/*********
 *	us_crs(cfile)	create request status
 *	char cfile[NAMESIZE];	 e.g. C.mhtsvn1234 
 *
 *	Whenever a command file (i.e. C.*****) file is spooled by uucp,
 *	this routine creates an entry in the beginning of "R_stat" file. 
 *	Future expansion: An R_stat entry may be created by, e.g.
 *	  uux, rmail, stock, or any command using uucp.
 *	return - 0 ok  | FAIL
 */
 
us_crs(cfile)
char cfile[NAMESIZE];
{
	char *name, s[30], buf[BUFSIZ];
	struct us_rsf u;
	FILE *fp, *fq, *us_open();
	long time();
 
	DEBUG(6, "Enter us_crs, cfile: %s\n", cfile);
	sprintf(s, "%s/%s.%.7d", Spool, "rstat", getpid());
	fq = us_open(s, "w+", "dummy", 1, 1);
	if (fq==NULL) return(FAIL);
	/* manufacture a new entry */
	name = cfile + strlen(cfile) - 4;	/* extract the last 4 digits */
	u.jobn = atoi(name);			/* convert to digits */
	u.qtime = u.stime = time((long *) 0);	/* store current time */
	fprintf(fq, "%04d\t%7.7s\t%7.7s\t", u.jobn, User, Rmtname);
	fprintf(fq, "%09ld\t%09ld\t%7o\n", u.qtime,u.stime,USR_QUEUE);
/*	printf(" Job %s is spooled\n", name);
*/
	fp=us_open(R_stat, "r", "dummy", 1, 1);
	if (fp != NULL ) {
		fseek(fp, 0L, 0);	/* rewind */
		while (fgets(buf, BUFSIZ, fp) != NULL )	/* read a line */
			fprintf(fq, "%s", buf);		/* write a line */
		fclose(fp);
		unlink(R_stat);		/* put spl6 and spl0 around */
	}
	fclose(fq);
	if (link(s, R_stat) < 0)
		DEBUG(1, "link %s fails in us_crs.c\n", R_stat);
	unlink(s);
	return(0);
}
