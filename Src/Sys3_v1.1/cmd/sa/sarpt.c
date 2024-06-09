/*	sarpt.c - It reads the input data file and
		generates three temp files each of which
		is a section of the daily report.
		These three sections of a report are 
		concatenated to a report by shell 
		procedure sar.sh.
*/
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#define DASH '-'
#define TBEGIN 8
#define TEND  18

struct sa {
	struct sysinfo si;
	long d0;
	long d1;
	long d2;
	long ts;
} 
x;
long s[26];
int hrc,minc;
struct tm *localtime(), *dt2;
FILE *fp0,*fp1,*fp2;
int brkflg;   /* if one, indicate a break in input data just happened */
main (argc,argv)
char **argv;
int argc;
{
	int nflg=0; /*  diff = nefgative flag, bypass data  */
	int fp;
	long curt;
	long time();
	float f[26];
	int i;
	float temp = 0.0;
	float sttf[26];
/*	open data file  */
	if ((fp = open(argv[1],0)) == -1){
		printf("can not open  %s file",argv[1]);
		exit(2);
	}
	curt =time((long *)0);
	dt2 = localtime(&curt);	
	dt2->tm_hour = -1;
	while ((dt2->tm_hour < TBEGIN) || (temp < 0.0)){
		if(dt2->tm_hour >= TBEGIN)  
			goto trans;
		if (read(fp,&x,sizeof x) == 0){
			printf("data does not exist before 8:00\n");
			exit(2);
		}
		transfer();
		/*
					for (i=0;i<26;i++)
					*/
		/*  printf("data %ld\n",s[i]); */
		temp = s[0] + s[1] + s[2];
	}
trans:
	transfer();
	temp = s[0] + s[1] + s[2];
	for (i=0;i<26;i++)
		f[i]=s[i];
	hrc = dt2->tm_hour;
	minc = dt2->tm_min;
	if ((fp0 = fopen("/tmp/sa.tmp0","w")) == NULL){
		printf (" can not open tmp0\n");
		exit(2);
	}
	if ((fp1 = fopen("/tmp/sa.tmp1","w")) == NULL){
		printf (" can not open tmp1\n");
		exit(2);
	}
	if ((fp2 = fopen("/tmp/sa.tmp2","w")) == NULL){
		printf (" can not open tmp2\n");
		exit(2);
	}
	/*	print report heading	*/
	fprintf(fp0,"           SYSTEM ACTIVITY DAILY REPORT    %02d/%02d/%02d   \n\n",
		(dt2->tm_mon + 1),
		dt2->tm_mday,
		dt2->tm_year);
	fprintf(fp0,"%-7s%6s%6s%6s%6s%6s%6s%6s%6s%8s%8s\n",
		"Time",
		"CPU",
		"Wait",
		"Wait",
		"Wait",
		"CPU",
		"CPU",
		"Swap",
		"Swap",
		"Read",
		"Write");
	fprintf(fp0,"%7s%6s%6s%6s%6s%6s%6s%6s%6s%8s%8s\n",
		" ",
		"Idle",
		"BIO",
		"Swap",
		"PIO",
		"Usr",
		"Sys",
		"in",
		"out",
		"Chars",
		"Chars");
	fprintf(fp0,"%7s%6s%6s%6s%6s%6s%6s%6s%6s%8s%8s\n\n",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		"/min",
		"/min",
		" ",
		" ");
	fprintf(fp1,"%-7s%7s%8s%7s%7s%8s%7s%8s%8s%8s\n",
		"Time",
		"Block",
		"Logical",
		"Cached",
		"Block",
		"Logical",
		"Cached",
		"RP06-0",
		"RP06-1",
		"RP06-2");
	fprintf(fp1,"%7s%7s%8s%7s%7s%8s%7s%8s%8s%8s\n",
		" ",
		"Reads",
		"Reads",
		"Reads",
		"Writes",
		"Writes",
		"Writes",
		"I/O",
		"I/O",
		"I/O");
	fprintf(fp1,"%7s%7s%8s%7s%7s%8s%7s%8s%8s%8s\n\n",
		" ",
		"/min",
		"/min",
		" ",
		"/min",
		"/min",
		" ",
		"/min",
		"/min",
		"/min");

	fprintf(fp2,"%-6s%10s%11s%11s%11s%11s%11s\n",
		"Time",
		"Process",
		"Preemt",
		"System",
		"Iget",
		"Namei",
		"Directory");
	fprintf(fp2,"%6s%10s%11s%11s%11s%11s%11s\n",
		" ",
		"Switches",
		" ",
		"Call",
		" ",
		" ",
		"Blocks");
	fprintf(fp2,"%6s%10s%11s%11s%11s%11s%11s\n\n",
		" ",
		"/sec",
		"/sec",
		"/sec",
		"/sec",
		"/sec",
		"/sec");
	dt2->tm_hour = hrc;  /* restore the current hour    */

/*	process in-range data records   */
	for (;;)
	{
/*	skip not on the hour data records except the dummy record  */
		while (dt2->tm_hour < (hrc + 1)){
			if ((temp = s[0]+s[1]+s[2]) <= 0.0){
				prtdash();
			}
			if (read(fp,&x,sizeof x) == 0)
				goto endfl;
			transfer();
		}
/*	process on the hour data records  */
		if  ((temp = s[0]+s[1]+s[2] ) <= 0.0){
			hrc = dt2->tm_hour;
			prtdash();
			if (read(fp,&x,sizeof x) == 0)
				goto endfl;
			transfer();
		}
		else {
			if (brkflg == 0){
				for (i=0;i<26;i++){
					f[i] = s[i] - f[i];
					if (f[i] < 0.0){
						hrc = dt2->tm_hour;
						nflg = 1;
						goto testend;
					}
				}
			}
			else {
				brkflg = 0;
				for (i=0;i<26;i++)
					f[i] =s[i];
			}
			/*  accumulate subtotal   */
			for (i=0;i<26;i++)
				sttf[i] = sttf[i] +f[i];
			/*  print rate   */
			temp=f[0]+f[1]+f[2];
			minc = dt2->tm_min;
			hrc = dt2->tm_hour;
			fprintf(fp0,"%02d:%02d %6.0f%%%5.0f%%%5.0f%%%5.0f%%%5.0f%%%5.0f%%%6.1f%6.1f%8.0f%8.0f\n",
				hrc,
				minc,
				f[0] /temp *100.0,
				f[3]/temp * 100.0,
				f[4]/temp * 100.0,
				f[5]/temp * 100.0,
				f[1]/temp * 100.0,
				f[2]/temp * 100.0,
				f[10]/temp *60.0 *60.0,
				f[11]/temp *60.0 *60.0,
				f[20]/temp * 60.0,
				f[21]/temp * 60.0);
			fprintf(fp1,"%02d:%02d  %7.0f%8.0f%6.0f%%%7.0f%8.0f%6.0f%%%8.0f%8.0f%8.0f\n",
				hrc,
				minc,
				f[6]/temp * 60.0 * 60.0,
				f[8]/temp * 60.0 * 60.0,
				(f[8] - f[6])/f[8] * 100.0,
				f[7]/temp * 60.0 * 60.0,
				f[9]/temp * 60.0 * 60.0,
				(f[9] - f[7])/f[9] * 100.0,
				f[22]/temp *60.0 *60.0,
				f[23]/temp *60.0 *60.0,
				f[24]/temp *60.0 *60.0);
			fprintf(fp2,"%02d:%02d %10.0f%11.0f%11.0f%11.0f%11.0f%11.0f\n",
				hrc,
				minc,
				f[12]/temp * 60.0,
				f[15]/temp * 60.0,
				f[16]/temp * 60.0,
				f[17]/temp * 60.0,
				f[18]/temp * 60.0,
				f[19]/temp * 60.0);
		}
testend:
		if (hrc == TEND) {
endfl:
			if (nflg == 1)
				goto closfl;
			temp = sttf[0] + sttf[1] + sttf[2];
			fprintf(fp0,"Daily\nAverage%5.0f%%%5.0f%%%5.0f%%%5.0f%%%5.0f%%%5.0f%%%6.1f%6.1f%8.0f%8.0f\n\n\n",
				sttf[0]/temp *100.0,
				sttf[3]/temp *100.0,
				sttf[4]/temp *100.0,
				sttf[5]/temp *100.0,
				sttf[1]/temp *100.0,
				sttf[2]/temp *100.0,
				sttf[10]/temp *60.0 *60.0,
				sttf[11]/temp *60.0 *60.0,
				sttf[20]/temp *60.0,
				sttf[21]/temp *60.0);
			fprintf(fp1,"Daily\nAverage%7.0f%8.0f%6.0f%%%7.0f%8.0f%6.0f%%%8.0f%8.0f%8.0f\n\n\n",
				sttf[6]/temp * 60.0 * 60.0,
				sttf[8]/temp * 60.0 * 60.0,
				(sttf[8] - sttf[6])/sttf[8] * 100.0,
				sttf[7]/temp * 60.0 * 60.0,
				sttf[9]/temp * 60.0 * 60.0,
				(sttf[9] - sttf[7])/sttf[9] * 100.0,
				sttf[22]/temp *60.0 * 60.0,
				sttf[23]/temp *60.0 * 60.0,
				sttf[24]/temp *60.0 * 60.0);
			fprintf(fp2,"Daily\nAverage%9.0f%11.0f%11.0f%11.0f%11.0f%11.0f\n\n\n",
				sttf[12]/temp * 60.0,
				sttf[15]/temp * 60.0,
				sttf[16]/temp * 60.0,
				sttf[17]/temp * 60.0,
				sttf[18]/temp * 60.0,
				sttf[19]/temp * 60.0);
closfl:
			fclose(fp0);
			fclose(fp1);
			fclose(fp2);
			exit(0);
		}
		if (brkflg == 0) {
			for (i=0;i<26;i++)
				f[i]=s[i];
			nflg = 0;
		}
	}
}
transfer()
{
	s[0] = x.si.cpu[0];
	s[1] = x.si.cpu[1];
	s[2] = x.si.cpu[2];
	s[3] = x.si.wait[0];
	s[4] = x.si.wait[1];
	s[5] = x.si.wait[2];
	s[6] = x.si.bread;
	s[7] = x.si.bwrite;
	s[8] = x.si.lread;
	s[9] = x.si.lwrite;
	s[10] = x.si.swapin;
	s[11] = x.si.swapout;
	s[12] =x.si.pswitch;
	s[13] = x.si.qswitch;
	s[14] = x.si.idle;
	s[15] = x.si.preempt;
	s[16] = x.si.syscall;
	s[17] = x.si.iget;
	s[18] = x.si.namei;
	s[19] = x.si.dirblk;
	s[20] = x.si.readch;
	s[21] = x.si.writech;
	s[22] = x.d0;
	s[23] = x.d1;
	s[24] =x.d2;
	s[25] = x.ts;
	dt2 = localtime(&s[25]);
}
prtdash()
{
	minc = dt2->tm_min;
	brkflg = 1;
	fprintf(fp0,"%02d:%02d  %6c%6c%6c%6c%6c%6c%6c%6c%8c%8c\n",
	hrc,minc,DASH,DASH,DASH,DASH,DASH,DASH,DASH,DASH,DASH,DASH);
	fprintf(fp1,"%02d:%02d  %7c%8c%7c%7c%8c%7c%8c%8c%8c\n",
	hrc,minc,DASH,DASH,DASH,DASH,DASH,DASH,DASH,DASH,DASH);
	fprintf(fp2,"%02d:%02d  %9c%11c%11c%11c%11c%11c\n",
	hrc,minc,DASH,DASH,DASH,DASH,DASH,DASH);
}
