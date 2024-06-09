	/*  ub_sst.c 1.1  10/2/79  15:04:33  */
#include "uucp.h"
#ifdef UUSUB
#include <sys/types.h>
#include "uusub.h"
 
/*********
 *	ub_sst(flag)	record connection status
 *	short flag;
 *
 *	This routine searches thru L_sub file
 *	using "rmtname" as the key.
 *	If the entry is found, then modify the connection
 * 	status as indicated in "flag" and return.
 *
 *	return - 0 ok  | FAIL
 */
 
ub_sst(flag)
short flag;
{
	struct ub_l l;
	FILE *fp, *us_open();
 
	DEBUG(6, " enter ub_sst, status is : %d\n", flag);
	DEBUG(6,"Rmtname: %s\n", Rmtname);
	fp = us_open(L_sub, "a+", LCKLSUB, 5, 1);
	if (fp == NULL) return(FAIL);
 
	fseek(fp, 0L, 0);	/* rewind */
	while (fread(&l, sizeof(l), 1, fp) == 1)
		if (strncmp(l.sys, Rmtname, 7) == SAME) {
		  switch(flag) {
			case ub_ok:	l.ok++;	 time(&l.oktime);  break;
			case ub_noacu:	l.noacu++;  break;
			case ub_login:	l.login++;  break;
			case ub_nack:	l.nack++;  break;
			default:	l.other++;  break;
		  }
			l.call++;
			DEBUG(6, "in ub_sst name=Rmtname: %s\n", l.sys);
			fseek(fp, -(long)sizeof(l), 1);
			fwrite(&l, sizeof(l), 1, fp);
			break;		/* go to exit */
		}
	fclose(fp);		/* exit point */
	unlink(LCKLSUB);
	return(0);
}
#endif
