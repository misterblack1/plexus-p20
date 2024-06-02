#include	"crash.h"
#include	"sys/acct.h"
#include	"sys/file.h"

struct	uarea	x;

pruarea()
{
	register  int  i;

	lseek(kmem, ka6 + delta, 0);
	if(read(kmem, &x, USIZE * CLICKSIZE) != USIZE * 64) {
		error("read error on u block");
		return;
	}
	printf("USER ID's:\t");
	printf("uid: %u, gid: %u, real uid: %u, real gid: %u\n",
		x.u.u_uid, x.u.u_gid, x.u.u_ruid, x.u.u_rgid);
	printf("PROCESS TIMES:\t");
	printf("user: %ld, sys: %ld, child user: %ld, child sys: %ld\n",
		x.u.u_utime, x.u.u_stime, x.u.u_cutime, x.u.u_cstime);
	printf("FILE I/O:\tuser addr: ");
	printf("%0.1o, file offset: %ld, bytes: %u,\n\t\tsegment: %s\n",
		x.u.u_base, x.u.u_offset, x.u.u_count,
		x.u.u_segflg == 0 ? "data" :
		(x.u.u_segflg == 1 ? "sys" : "text"));
	printf("ACCOUNTING:\t");
	printf("command: %.14s, memory: %ld, type: %s%s\n\t\tstart: %s",
		x.u.u_comm, x.u.u_mem, x.u.u_acflag & AFORK ? "fork" : "exec",
		x.u.u_acflag & ASU ? ", super user" : "",ctime(&x.u.u_start));
	printf("OPEN FILES:\t");
	printf("file desc: ");
	for(i = 0; i < NOFILE; i++)
		if(x.u.u_ofile[i] != 0)
			printf(" %3d", i);
	printf("\n\t\tfile slot: ");
	for(i = 0; i < NOFILE; i++)
		if(x.u.u_ofile[i] != 0)
			printf(" %3d", ((unsigned)x.u.u_ofile[i] -
				File->n_value) / sizeof (struct file));
	printf("\n");
}

prstack()
{
	register  int  *ip;

	lseek(kmem, ka6 + delta, 0);
	if(read(kmem, &x, USIZE * CLICKSIZE) != USIZE * 64) {
		error("read error on u block");
		return;
	}
	printf("KERNEL STACK:");
	for(ip = x.sysstk; ip != &x.sysstk[USIZE * CLICKSIZE - sizeof u]; ip++)
		if(*ip)
			break;
	while(((unsigned) ip - (unsigned) &x + STKMASK) % 16)
		ip--;
	for(; ip != &x.sysstk[USIZE * CLICKSIZE - sizeof u];) {
		if(((unsigned)ip - (unsigned)&x + STKMASK) % 16 == 0)
			printf("\n%6o:\t",
				(unsigned)ip - (unsigned)&x + STKMASK);
		printf("  %6o", *ip++);
	}
	printf("\nSTACK FRAMES:\n");
	printf("<-- r2 r3 r4 r5 ret args auto var r2 r3 r4 r5 ");
	printf("ret dev sp r1 nps r0 pc ps\n");
	printf("HW r5 _______|^|____________________________|^\n");
}

prtrace(d)
	int	d;
{
	register  int  funny;
	register  struct frame  *fp;
	register  int  *ip;
	struct	nlist	*sp;
	extern	int	r5;

	lseek(kmem, ka6 + delta, 0);
	if(read(kmem, &x, USIZE * CLICKSIZE) != USIZE * CLICKSIZE) {
		error("read error on u block");
		return;
	}
	if(d == 1) {
		printf("R5 STACK TRACE:\n");
		if((r5 & STKHI) != STKMASK) {
			printf("\tHW R5 invalid\n");
			return;
		}
		fp = (unsigned)&x + (unsigned)(r5 & ~STKMASK);
		do {
			sp = search(fp->f_ret, 042, 042);
			if(sp == 0)
				printf("\tno match\n");
			else if(sp->n_name[0] == '_')
				printf("\t%.7s\n", &sp->n_name[1]);
			else
				printf("\t%.8s\n", sp->n_name);
			if((fp->f_r5 & STKHI) != STKMASK) {
				printf("\tcurdled stack\n");
				return;
			}
			fp = (unsigned)&x + (unsigned)(fp->f_r5 & ~STKMASK);
		} while((fp->f_r5 & STKHI) == STKMASK);
		return;
	}
	printf("STACK TRACE:\n");
	funny = ((x.sysstk[USIZE * CLICKSIZE - sizeof u - 1]) & 0377) == 0360 ?
		18 : 16;
	ip = fp = &x.sysstk[USIZE * CLICKSIZE - sizeof u - funny];
	sp = search(fp->f_ret, 042, 042);
	if(sp == 0)
		printf("\tno match\n");
	else if(sp->n_name[0] == '_')
		printf("\t%.7s\n", &sp->n_name[1]);
	else
		printf("\t%.8s\n", sp->n_name);
	--ip;
	while(--ip != x.sysstk)
		if(*ip == STKMASK + (unsigned)fp - (unsigned)&x) {
			if(sp = search((struct frame *)ip->f_ret, 042, 042)) {
				fp = ip;
				if(sp->n_name[0] == '_')
					printf("\t%.7s\n", &sp->n_name[1]);
				else
					printf("\t%.8s\n", sp->n_name);
			}
		}
}
