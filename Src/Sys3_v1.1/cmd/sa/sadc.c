/*
	sadc.c - writes system activity data in binary to a  file.
	Usage: sadc [n] filename
		if n is not specified, it writes system data once.
		if n is specified, it writes system data n times to the 
		file every 20 minutes.
*/
#include <sys/types.h>
#include <sys/sysinfo.h>

#include <sys/stat.h>
#include <sys/elog.h>
#include <a.out.h>
#ifndef STOCKIII
#include <sys/plexus.h>
#endif

#ifdef	vax
struct nlist setup[] = {
	"_sysinfo",0,0,0,0,
	"_hpstat",0,0,0,0,
	0,0,0,0,0
};
#endif
#ifdef	z8000
struct nlist setup[] = {
	"_sysinfo",0,0,
	"_dkstat",0,0,
	0,0,0
};
#endif
struct iostat ib[8];

struct sa {
	struct sysinfo si;
	long d0;
	long d1;
	long d2;
	long ts;
}d;

main(argc, argv)
char **argv;
int argc;
{
	char *flnm = "/tmp/sa.adrfl";
	long time();
	long lseek();
	unsigned sleep();
	int f,fa,fp;
	int ct;
	unsigned sec = 1200;
	long curt;
	struct stat buf,ubuf,sbuf;
	char *fname;

	ct = argc > 2? atoi(argv[1]): 1;
	fname = argc==2? argv[1]: argv[2];
	curt = time((long *) 0);
/*	check if data file is absent or is too old, create a new data file   */
	if ((stat(fname,&buf) == -1) || ((curt - buf.st_mtime) > 86400))
		goto credfl;
/*	open data file, if it fails, create a new file   */
	if ((fp = open(fname,2)) == -1){
credfl:
		if ((fp = creat(fname,00644)) == -1)
			exit(2);
		close(fp);
		fp = open (fname,2);
	}
/*	position the write pointer to the last good record  */
	else
		lseek(fp,-(long)(buf.st_size % sizeof d),2);
/*	open /dev/kmem   */

	if((f = open("/dev/kmem", 0)) == -1)
		exit(2);
/*	check if address file is absent, or is older than /unix,
	create a new address file   */
#ifdef STOCKIII
	stat ("/unix",&sbuf);
#else
	stat (UNIXNAME,&sbuf);
#endif
	if ((stat(flnm,&ubuf) == -1) || (ubuf.st_mtime < sbuf.st_mtime))
		goto creafl;
/*	open address file and read it, if it fails, create a new one   */
	fa = open(flnm,2);
	if (read(fa,setup, sizeof setup) == -1){
creafl:
		fa = creat(flnm,00644);
		close(fa);
		fa = open(flnm,2);
	}
/*	search name list to get offsets  */
#ifdef STOCKIII
	nlist ("/unix",setup);
#else
	nlist (UNIXNAME,setup);
#endif
#ifdef vax
		setup[0].n_value &= ~(0x80000000);
		setup[1].n_value &= ~(0x80000000);
#endif
/*	write offsets to address file   */
	write (fa,setup,sizeof setup);
	close (fa);
/*	read data from /dev/kmem to structure d	*/
	for(;;) {
		lseek(f,(long)setup[0].n_value,0);
		if (read(f, &d.si, sizeof d.si) == -1)
			exit(2);
		if (setup[1].n_value != 0){
			lseek(f,(long)setup[1].n_value,0);
			if (read(f, ib, sizeof ib) == -1)
				exit(2);
			d.ts = time ((long *) 0);
			d.d0 = ib[0].io_ops;
			d.d1 = ib[1].io_ops;
			d.d2 = ib[2].io_ops;

		}
		else
			d.ts = time((long *) 0);
/*	write data to data file from structure d	*/
		write(fp,&d,sizeof d);
		if(--ct)
			sleep(sec);
		else {
			close(fp);
			exit(0);
		}
	}
}
