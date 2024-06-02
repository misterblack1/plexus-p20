/*	sadd.c - write a dummy record to file
	usage - sadd filename
*/

#include<stdio.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>

struct sa {
	struct sysinfo si;
	long d0;
	long d1;
	long d2;
	long ts;
}d;
main (argc,argv)
char **argv;
int argc;
{
	int fp;
	long lseek();
	long curt,time();
	struct stat buf;
	char *fname;
	int i;
 
	fname = argv[1];
	curt = time((long *) 0);

/*	Check if data file is absent, or is too old,
	create a new data file   */
	if ((stat(fname,&buf) == -1) || ((curt - buf.st_mtime) > 86400))
		goto crefl;
/*	open data file   */
	if ((fp = open(fname,2)) == -1){
crefl:
		creat (fname,00644);
		close(fp);
		fp = open(fname,2);
		goto writ;
	}
	else 
/*	reposition write pointer to the last good record  */
		lseek(fp,-(long)(buf.st_size % sizeof d),2);
/*	load data to dummy record and write it to file  */
writ:
	for (i=0;i<3;i++)
		d.si.cpu[i] = -300;
	d.ts =time ((long *)0);
	write(fp,&d,sizeof d);
	exit (0);
}
