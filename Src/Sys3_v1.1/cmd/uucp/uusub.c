	/*  uusub.c 1.1  10/2/79  16:45:32  */
 
#include "uucp.h"
#ifdef UUSUB
#include <time.h>
#include <sys/types.h>
#include "uusub.h"
#define	rid	0		/* user id for super user */
/*************
 *	uusub --- a command for monitoring uucp subnetwork
 *		 Probably should be limited to 'root', 'daemon', 'uucp'.
 */
 
main(argc, argv)
char **argv;
int argc;
{
	extern char *optarg;
	extern int optind;
	struct ub_l l;
	struct ub_r r;
	short aflag,dflag,rflag,lflag,fflag,cflag,uflag;
	short ua, c;
	char aa[10], da[10], ca[10], s[128];
	FILE *fp, *fq, *us_open();
 
	aflag=dflag=rflag=lflag=fflag=cflag=uflag=0;
	while ((c=getopt(argc, argv, "x:a:d:c:u:rlf")) != EOF )
		switch(c) {
		case 'x':
			Debug = atoi(optarg);
			break;
		case 'a':
			aflag++;
			strcpy(aa, optarg);
			break;
		case 'd':
			dflag++;
			strcpy(da, optarg);
			break;
		case 'c':
			cflag++;
			strcpy(ca, optarg);
			break;
		case 'u':
			uflag++;
			ua = atoi(optarg);
			break;
		case 'r':
			rflag++;
			break;
		case 'l':
			lflag++;
			break;
		case 'f':
			fflag++;
			break;
		case '?':
			fprintf(stderr,"Usage:uusub -a* -d* -c* -u* -r -l-f\n");
			exit(2);
		}
 
	if (fflag) {	/* flush L_sub file */
		DEBUG(6, "Flush file %s\n", L_sub);
		edit('f',"");		/* 2nd argument is irrelevant */
	}
 
	if (dflag) {	/* delete system "da" from "L_sub" */
		DEBUG(6, "Delete system %s from subnetwork\n",da);
		edit('d',da);
	}
 
	if (aflag) {	/* add 'aa' to L_sub file */
		DEBUG(6, "Add system %s to subnetwork\n",aa);
		fp = us_open(L_sub, "a+", LCKLSUB, 30, 1);
		if (fp==NULL) return(FAIL);
		fseek(fp, 0L, 0);	/* rewind */
		while (fread(&l, sizeof(l), 1, fp)!=NULL)
			if (strcmp(l.sys,aa)==SAME) goto out;
		DEBUG(6, "system %s is added\n", aa);
		strcpy(l.sys, aa);	/* manufacture a new entry */
		time(&l.oktime);
		l.call = l.ok = l.noacu = l.login = l.nack = l.other = 0;
		fwrite(&l,sizeof(l),1,fp);
	  out:  fclose(fp);
		unlink(LCKLSUB);
	}
 
	if (cflag) {	/* call system 'ca' */
		if (strcmp(ca, "all") != SAME)
			xuucico(ca);
		else {	/* call all systems */
			fp=us_open(L_sub,"r","dummy",1,1);
			if (fp==NULL) return(FAIL);
			while (fread(&l,sizeof(l),1,fp)==1)
				xuucico(l.sys);
		}
		fclose(fp);
	}
 
	if (uflag) {	/* update R_sub file from SYSLOG */
		time_t tick, oldtick;
		char rmt[10], s_r[10];
		long bytes;
 
		fp=us_open(L_sub, "r","dummy",1,1);
		if (fp==NULL) return(FAIL);
		fq=us_open(R_sub, "w",LCKRSUB,10,1);
		if (fq==NULL) return(FAIL);
		fseek(fq, 0L, 0);	/* rewind */
	/* Flush  R_sub file	*/
		DEBUG(6," Flush file %s\n", R_sub);
		r.sf = r.rf = (long) 0;
		r.sb = r.rb = 0;
		while(fread(&l,sizeof(l),1,fp)==1) {
			DEBUG(6," l.sys: %s\n", l.sys);
			strcpy(r.sys, l.sys);
			fwrite(&r,sizeof(r),1,fq);
		}
 
		fclose(fp);
		fclose(fq);
		unlink(LCKRSUB);
		fp=us_open(SYSLOG, "r","dummy",1,1);
		if (fp==NULL) return(FAIL);
		fq=us_open(R_sub,"r+",LCKRSUB,10,1);
		if (fq==NULL) return(FAIL);
		time(&tick);		/* get current time */
		while (fgets(s,128,fp)!=NULL) {
sscanf(s,"%*s %s (%*d/%*d-%*d:%*d) (%ld) %s %*s %ld",rmt,&oldtick,s_r,&bytes);
			DEBUG(9," Rmt: %s ",rmt);
			DEBUG(9," s_r: %s", s_r);
			DEBUG(9," bytes: %ld ",bytes);
			DEBUG(9," oldtick: %ld\n",oldtick);
			if ((tick-oldtick)>(ua*3600L)) continue;
			DEBUG(8, " %s inside the time\n",rmt);
			fseek(fq, 0L, 0);	/* rewind */
			while (fread(&r,sizeof(r),1,fq)==1)
				if (strcmp(r.sys,rmt)==SAME) {
					if (strcmp(s_r,"sent")==SAME) {
						r.sf++;
						r.sb += bytes;
					}
					else {
						r.rf++;
						r.rb += bytes;
					}
					fseek(fq, - (long) sizeof(r), 1);
					fwrite(&r,sizeof(r),1,fq);
					DEBUG(7,"%s is written\n",r.sys);
					break;	/* break inner 'while' */
				}
		}
		fclose(fp);
		fclose(fq);
		unlink(LCKRSUB);
	}
 
	if (rflag) {	/* print the R_sub file */
		fp = us_open(R_sub, "r", "dummy", 1, 1);
		if (fp==NULL) return(FAIL);
		printf("sysname\tsfile\tsbyte\trfile\trbyte\n");
		while (fread(&r, sizeof(r), 1, fp)!=NULL)
			printf("%s\t%d\t%ld\t%d\t%ld\n",r.sys,r.sf,r.sb,r.rf,r.rb);
		fclose(fp);
	}
 
	if (lflag) {	/* print the L_sub file */
		struct tm *tp, *localtime();
 
		fp = us_open(L_sub, "r", "dummy", 1, 1);
		if (fp==NULL) return(FAIL);
		printf("sysname\t\#call\t\#ok\tlatest-oktime\t\#noacu\t\#login");
		printf("\t\#nack\t\#other\n");
		while (fread(&l, sizeof(l), 1, fp)!=NULL) {
			tp=localtime(&l.oktime);
			printf("%s\t%d\t%d", l.sys,l.call,l.ok);
			printf("\t(%d/%d-%d:%2.2d)\t", tp->tm_mon+1,
			        tp->tm_mday, tp->tm_hour, tp->tm_min);
			printf("%d\t%d\t%d\t%d\n",l.noacu,l.login,l.nack,l.other);
		}
		fclose(fp);
	}
}
 
edit(flag,arg)	/* edit "L_sub" file */
short flag;
char *arg;
{
	FILE *fq, *fp, *us_open();
	struct ub_l l;
	char s[64];
 
	DEBUG(6, "enter edit, flag: %d\n",flag);
	sprintf(s, "%s/%s.%.7d",Spool,"lsub",getpid());
	fq = us_open(s, "w", "dummy", 1, 1);
	if (fq==NULL) return(FAIL);
	fp = us_open(L_sub, "r", "dummy", 1, 1);
	if (fp==NULL) return(FAIL);
	while (fread(&l, sizeof(l), 1, fp)!=NULL)
	 switch(flag) {
	 case 'f':
		l.call = l.ok = l.noacu = l.login = l.nack = l.other = 0;
		fwrite(&l,sizeof(l),1,fq);
		break;
	 case 'd':
		if (strcmp(l.sys,arg)!=SAME) {
			DEBUG(7,"%s is retained\n",l.sys);
			fwrite(&l,sizeof(l),1,fq);
		}
		break;
	 }
	fclose(fp);
	fclose(fq);
	fp = us_open(L_sub, "w", LCKLSUB, 30, 1);
	if (fp==NULL) return(FAIL);
	fq = us_open(s, "r", "dummy", 1, 1);
	if (fq==NULL) return(FAIL);
	while (fread(&l, sizeof(l), 1, fq)!=NULL) {
		fwrite(&l,sizeof(l),1,fp);
	}
	fclose(fp);
	fclose(fq);
	unlink(LCKLSUB);
	unlink(s);
	unlink("dummy");
	return;
}
#else
main(argc, argv)
char **argv;
{	fprintf(stderr, "Uusub is not implemented on this system\n");
}
#endif
