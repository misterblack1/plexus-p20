#include <sys/plexus.h>
#include <stdio.h>
#include <signal.h>
#include <a.out.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/sysinfo.h>
#include <sys/elog.h>
#include <sys/iobuf.h>


time_t times();

extern _sobuf;
extern int errno;
extern char *sys_errlist[];

struct sysinfo sia,sib;
struct iostat ib[8],ia[8];
#ifdef vax
struct nlist setup[] = {
	"_sysinfo",0,0,0,0,"_hpstat",0,0,0,0,
	0,0,0,0,0
};
#endif
#ifdef z8000
struct nlist setup[] = {
	"_sysinfo",0,0,"_hpstat",0,0,
	0,0,0
};
#endif
main(argc, argv)
char **argv;
{
	struct tms buffer, obuffer;
	int status;
	register p;
	time_t before, after;
	int	fc;

	if ((fc = open("/dev/kmem", 0)) < 0) {
		fprintf(stderr, "Can't read /dev/kmem\n");
		exit(1);
	}
	nlist(UNIXNAME, setup);
#ifdef vax
	setup[0].n_value -= 0x80000000;
	setup[1].n_value -= 0x80000000;
#endif
		lseek(fc, (long)setup[0].n_value, 0);
		read(fc, &sib, sizeof sib);
		lseek(fc,(long)setup[1].n_value, 0);
		read(fc, ib, sizeof ib);
		lseek(fc, (long)setup[0].n_value, 0);
		before = times(&obuffer);
		if(argc<=1)
			exit(0);
		p = fork();
		if(p == -1) {
			fprintf(stderr, "Try again.\n");
			exit(1);
		}
		if(p == 0) {
			close(fc);
			execvp(argv[1], &argv[1]);
			fprintf(stderr, "%s: %s\n", argv[1], sys_errlist[errno]);
			exit(1);
		}
		setbuf(stderr,&_sobuf);
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		while(wait(&status) != p);
		if((status&0377) != 0)
			fprintf(stderr,"Command terminated abnormally.\n");
	after = times(&buffer);
	read(fc, &sia, sizeof sia);
	sib.readch += sizeof sib + sizeof ib;
	sib.syscall += 12;
	lseek(fc,(long)setup[1].n_value,0);
	read(fc, ia, sizeof ia);
	fprintf(stderr,"\n");
	printt("real", cv50to60(after-before));
	printt("user", cv50to60(buffer.tms_cutime - obuffer.tms_cutime));
	printt("sys ", cv50to60(buffer.tms_cstime - obuffer.tms_cstime));
	fprintf(stderr, "\n");
	fprintf(stderr, "cpu idle\t%2.0f%%\n", (double)(sia.cpu[CPU_IDLE] - sib.cpu[CPU_IDLE]) /
		(double)((sia.cpu[CPU_IDLE] - sib.cpu[CPU_IDLE]) +
		(sia.cpu[CPU_KERNEL] - sib.cpu[CPU_KERNEL]) +
		(sia.cpu[CPU_USER] - sib.cpu[CPU_USER])) * 100.);
	fprintf(stderr, "wait bio\t%2.0f%%\n", (double)(sia.wait[W_IO] - sib.wait[W_IO]) /
		(double)((sia.cpu[CPU_IDLE] - sib.cpu[CPU_IDLE]) +
		(sia.cpu[CPU_KERNEL] - sib.cpu[CPU_KERNEL]) +
		(sia.cpu[CPU_USER] - sib.cpu[CPU_USER])) * 100.);
	fprintf(stderr, "wait swap\t%2.0f%%\n", (double)(sia.wait[W_SWAP] - sib.wait[W_SWAP]) /
		(double)((sia.cpu[CPU_IDLE] - sib.cpu[CPU_IDLE]) +
		(sia.cpu[CPU_KERNEL] - sib.cpu[CPU_KERNEL]) +
		(sia.cpu[CPU_USER] - sib.cpu[CPU_USER])) * 100.);
	fprintf(stderr, "wait pio\t%2.0f%%\n", (double)(sia.wait[W_PIO] - sib.wait[W_PIO]) /
		(double)((sia.cpu[CPU_IDLE] - sib.cpu[CPU_IDLE]) +
		(sia.cpu[CPU_KERNEL] - sib.cpu[CPU_KERNEL]) +
		(sia.cpu[CPU_USER] - sib.cpu[CPU_USER])) * 100.);
	fprintf(stderr, "cpu user\t%2.0f%%\n", (double)(sia.cpu[CPU_USER] - sib.cpu[CPU_USER]) /
		(double)((sia.cpu[CPU_IDLE] - sib.cpu[CPU_IDLE]) +
		(sia.cpu[CPU_KERNEL] - sib.cpu[CPU_KERNEL]) +
		(sia.cpu[CPU_USER] - sib.cpu[CPU_USER])) * 100.);
	fprintf(stderr, "cpu sys\t\t%2.0f%%\n", (double)(sia.cpu[CPU_KERNEL] - sib.cpu[CPU_KERNEL]) /
		(double)((sia.cpu[CPU_IDLE] - sib.cpu[CPU_IDLE]) +
		(sia.cpu[CPU_KERNEL] - sib.cpu[CPU_KERNEL]) +
		(sia.cpu[CPU_USER] - sib.cpu[CPU_USER])) * 100.);
	fprintf(stderr, "swap in\t\t%ld\n", sia.swapin - sib.swapin);
	fprintf(stderr, "swap out\t%ld\n", sia.swapout - sib.swapout);
	fprintf(stderr, "chars read\t%ld\n", sia.readch - sib.readch);
	fprintf(stderr, "chars written\t%ld\n", sia.writech - sib.writech);
	fprintf(stderr, "block reads\t%ld\n", sia.bread - sib.bread);
	fprintf(stderr, "logical reads\t%ld\n", sia.lread - sib.lread);
	fprintf(stderr, "cached reads\t%2.0f%%\n", ((sia.lread - sib.lread) -
		(sia.bread - sib.bread)) / (double) (sia.lread - sib.lread) * 100.);
	fprintf(stderr, "block writes\t%ld\n", sia.bwrite - sib.bwrite);
	fprintf(stderr, "logical writes\t%ld\n", sia.lwrite - sib.lwrite);
	fprintf(stderr, "cached writes\t%2.0f%%\n", ((sia.lwrite - sib.lwrite) -
		(sia.bwrite - sib.bwrite)) / (double) (sia.lwrite - sib.lwrite) * 100.);
	fprintf(stderr, "disk0 I/O\t%ld\n",ia[0].io_ops -ib[0].io_ops);
	fprintf(stderr, "disk1 I/O\t%ld\n",ia[1].io_ops -ib[1].io_ops);
	fprintf(stderr, "disk2 I/O\t%ld\n",ia[2].io_ops -ib[2].io_ops);
	fprintf(stderr, "p switch\t%ld\n", sia.pswitch - sib.pswitch);
	fprintf(stderr, "preempt\t\t%ld\n", sia.preempt - sib.preempt);
	fprintf(stderr, "syscall\t\t%ld\n", sia.syscall - sib.syscall);
	fprintf(stderr, "iget\t\t%ld\n", sia.iget - sib.iget);
	fprintf(stderr, "namei\t\t%ld\n", sia.namei - sib.namei);
	fprintf(stderr, "dirblk\t\t%ld\n", sia.dirblk - sib.dirblk);
	exit(status>>8);
}

char quant[] = { 6, 10, 10, 6, 10, 6, 10, 10, 10 };
char *pad  = "000      ";
char *sep  = "\0\0.\0:\0:\0\0";
char *nsep = "\0\0.\0 \0 \0\0";

printt(s, a)
char *s;
long a;
{
	char digit[9];
	register i;
	char c;
	int nonzero;

	for(i=0; i<9; i++) {
		digit[i] = a % quant[i];
		a /= quant[i];
	}
	fprintf(stderr,s);
	nonzero = 0;
	while(--i>0) {
		c = digit[i]!=0 ? digit[i]+'0':
		    nonzero ? '0':
		    pad[i];
		fprintf(stderr,"%c",c);
		nonzero |= digit[i];
		c = nonzero?sep[i]:nsep[i];
		fprintf(stderr,"%c",c);
	}
	fprintf(stderr,"%c",digit[0]*10/6+'0');
	fprintf(stderr,"\n");
}
