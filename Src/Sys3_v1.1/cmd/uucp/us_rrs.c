	/* us_rrs	1.0	7/2/79	*/
#include "uucp.h"
#include <sys/types.h>
#include "uust.h"
 
/*********
 *	us_rrs(cfilel, stat) 	record request status
 *	char *cfilel;	e.g. /usr/spool/uucp/C.mhtsvn1234
 *	short stat;	the "Usrf" flag.
 *
 *	We get the job number from a command file "cfile".
 *	using the jobn as the key to search thru "R_stat"
 *	file and modify the corresponding status as indicated
 *	in "stat".	"Stat" is defined in "uust.h".
 *	return - 0 ok  | FAIL
 */
 
us_rrs(cfilel,stat)
char *cfilel;
short stat;
{
	struct us_rsf u;
	FILE *fp, *us_open();
	short n;
	char cfile[20], *lastpart(), *name, buf[BUFSIZ];
	long time();
 
	strcpy(cfile, lastpart(cfilel));	/* strip path info */
	DEBUG(6, "\nenter us_rrs, cfile: %s", cfile);
	DEBUG(6, "  request status: %o\n", stat);
	name = cfile + strlen(cfile) - 4;  /* extract the last 4 digits */
	n = atoi(name);			/* convert to digits */
	fp = us_open(R_stat, "a+", LCKRSTAT, 10, 1);
	if (fp == NULL) return(FAIL);
	fseek(fp, 0L, 0);	/* rewind */
	while (fscanf(fp, "%d%*s%*s%*ld", &u.jobn)!=NULL) {
		if (u.jobn == n) {
			DEBUG(6, " jobn : %d\n", u.jobn);
			fseek(fp, 0L, 1);	/* no effect: for printf */
			fprintf(fp, "\t%09ld\t%7o\n", time((long *) 0), stat);
			break;
		}
		if(fgets(buf, BUFSIZ, fp)==NULL)	/* skip the line */
			break;
	}
	fclose(fp);
	unlink(LCKRSTAT);
	return(FAIL);
}
