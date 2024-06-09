	/*  us_sst.c 3.4  4/4/80  15:04:28  */
#include "uucp.h"
#ifdef UUSTAT
#include <sys/types.h>
#include "uust.h"
 
/*********
 *	us_sst(flag)	record system status
 *	short flag;
 *
 *	This routine searches thru L_stat file
 *	using "rmtname" as the key.
 *	If the entry is found, then modify the system
 * 	status as indicated in "flag" and return.
 *
 *	return - 0 ok  | FAIL
 */
 
us_sst(flag)
short flag;
{
	struct us_ssf s;
	FILE *fp, *us_open();
	char buf[BUFSIZ];
	long time();
 
	DEBUG(6, " enter us_sst, status is : %02d\n", flag);
	fp = us_open(L_stat, "a+", LCKLSTAT, 10, 1);
	if (fp == NULL) return(FAIL);
 
	fseek(fp, 0L, 0);	/* rewind */
	while (fscanf(fp, "%7s", s.sysname) == 1) {
		DEBUG(6, "s.sysname : %s\n", s.sysname);
		if (strncmp(s.sysname, Rmtname, 7) == SAME) {
			fseek(fp,0L,1);		/* for stdio */
			fprintf(fp, "\t%09ld\t%02d\n", time((long *) 0), flag);
			goto out;
		}
		else  fgets(buf, BUFSIZ, fp);	/* skip a line */
	}
 
	/** system name is not known, create an entry **/
	fseek(fp, 0L, 2);		/* for stdio */
	fprintf(fp, "%s\t%09ld\t%02d\n", Rmtname, time((long *) 0), flag);
  out:	unlink(LCKLSTAT);
	fclose(fp);
	return(0);
}
#endif
