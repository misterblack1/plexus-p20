/*
 *	acctcms [-a] [-j] [-s] [file...]
 *	summarize per-process accounting
 *	-a	output in ascii, rather than cms.h format
 *	-c	sort by total cpu, rather than total kcore-minutes
 *	-j	anything used only once -> ***other
 *	-n	sort by number of processes
 *	-s	any following files already in cms.h format
 *	file	file in cms.h (if -s seen already) or acct.h (if not)
 *	expected use:
 *	acctcms /usr/adm/pacct? > today; acctcms -s old today >new
 *	cp new old; rm new
 *	acctcms -a today; acctcms -a old
 */
#include <sys/types.h>
#include "acctdef.h"
#include <stdio.h>
#include <sys/acct.h>
#define MYKIND(flag)	((flag & ACCTF) == 0)

#define CSIZE 1000
int	csize;
struct cms {
	char	cm_comm[8];	/* command name */
	long	cm_pc;		/* number of processes */
	float	cm_cpu;		/* cpu time(min) */
	float	cm_real;	/* real time(min) */
	float	cm_kcore;	/* kcore-minutes */
	long	cm_io;		/* chars transfered */
	long	cm_rw;		/* blocks read */
} cm[CSIZE];
struct	acct	ab;
struct	cms	cmtmp	= {"***other"};
int	aflg;
int	cflg;
int	jflg;
int	nflg;
int	sflg;

int	ccmp(), kcmp(), ncmp();
time_t	expand();

main(argc, argv)
char **argv;
{

	while (--argc > 0) {
		if (**++argv == '-')
			switch(*++*argv) {
			case 'a':
				aflg++;
				continue;
			case 'c':
				cflg++;
				continue;
			case 'j':
				jflg++;
				continue;
			case 'n':
				nflg++;
				continue;
			case 's':
					sflg++;
				continue;
			}
		else
			dofile(*argv);
	}
	if (jflg)
		fixjunk();
	squeeze();
	qsort(cm, csize, sizeof(cm[0]), nflg? ncmp: (cflg? ccmp: kcmp));
	if (aflg)
		outputa();
	else
		outputc();
}

dofile(fname)
char *fname;
{
	struct acct ab;
	struct cms cmt;

	if (freopen(fname, "r", stdin) == NULL) {
		fprintf(stderr,  "acctcms: cannot open %s\n", fname);
		return;
	}
	if (sflg)
		while (fread(&cmt, sizeof(cmt), 1, stdin) == 1)
			enter(&cmt);
	else
		while (fread(&ab, sizeof(ab), 1, stdin) == 1) {
			CPYN(cmt.cm_comm, ab.ac_comm);
			cmt.cm_pc = 1;
			cmt.cm_cpu = MINT(expand(ab.ac_stime)+expand(ab.ac_utime));
			cmt.cm_real = MINT(expand(ab.ac_etime));
			cmt.cm_kcore = MINT(KCORE(expand(ab.ac_mem)));
			cmt.cm_io = expand(ab.ac_io);
			cmt.cm_rw = expand(ab.ac_rw);
			enter(&cmt);
		}
}

enter(p)
register struct cms *p;
{
	register i;
	int j;
	for (i = j = 0; j < sizeof(p->cm_comm); j++) {
		if (p->cm_comm[j] && p->cm_comm[j] <= 037)
			p->cm_comm[j] = '?';
		i = i*7 + p->cm_comm[j];	/* hash function */
	}
	if (i < 0)
		i = -i;
	for (i %= CSIZE; cm[i].cm_comm[0]; i = (i+1)%CSIZE)
		if (EQN(p->cm_comm, cm[i].cm_comm))
			break;
	if (cm[i].cm_comm[0] == 0)
		CPYN(cm[i].cm_comm, p->cm_comm);
	cmadd(&cm[i], p);
	return(i);
}
fixjunk()	/* combine commands used only once */
{
	register i, j;
	j = enter(&cmtmp);
	for (i = 0; i < CSIZE; i++)
		if (i != j && cm[i].cm_comm[0] && cm[i].cm_pc <= 1) {
			cmadd(&cm[j], &cm[i]);
			cm[i].cm_comm[0] = 0;
		}
}

cmadd(p1, p2)
register struct cms *p1, *p2;
{
	p1->cm_pc += p2->cm_pc;
	p1->cm_cpu += p2->cm_cpu;
	p1->cm_real += p2->cm_real;
	p1->cm_kcore += p2->cm_kcore;
	p1->cm_io += p2->cm_io;
	p1->cm_rw += p2->cm_rw;
}

squeeze()	/* get rid of holes in hash table */
{
	register i, k;

	for (i = k = 0; i < CSIZE; i++)
		if (cm[i].cm_comm[0]) {
			CPYN(cm[k].cm_comm, cm[i].cm_comm);
			cm[k].cm_pc = cm[i].cm_pc;
			cm[k].cm_cpu = cm[i].cm_cpu;
			cm[k].cm_real = cm[i].cm_real;
			cm[k].cm_kcore = cm[i].cm_kcore;
			cm[k].cm_io = cm[i].cm_io;
			cm[k].cm_rw = cm[i].cm_rw;
			k++;
		}
	csize = k;
}

ccmp(p1, p2)
register struct cms *p1, *p2;
{
	if (p1->cm_cpu == p2->cm_cpu)
		return(0);
	return ((p2->cm_cpu > p1->cm_cpu)? 1 : -1);
}

kcmp(p1, p2)
register struct cms *p1, *p2;
{
	if (p1->cm_kcore == p2->cm_kcore)
		return(0);
	return ((p2->cm_kcore > p1->cm_kcore)? 1 : -1);
}

ncmp(p1, p2)
register struct cms *p1, *p2;
{
	if (p1->cm_pc == p2->cm_pc)
		return(0);
	return ((p2->cm_pc > p1->cm_pc)? 1 : -1);
}

char	hd1[] =
"COMMAND   NUMBER      TOTAL       TOTAL       TOTAL   MEAN     MEAN     HOG      CHARS        BLOCKS\n";
char	hd2[] =
"NAME        CMDS    KCOREMIN     CPU-MIN     REAL-MIN SIZE-K  CPU-MIN  FACTOR   TRNSFD         READ\n";
outputa()
{
	register i;

	printf(hd1);
	printf(hd2);
	printf("\n");
	for (i = 0; i < csize; i++)
		cmadd(&cmtmp, &cm[i]);
	CPYN(cmtmp.cm_comm, "TOTALS");
	print(&cmtmp);
	printf("\n");
	for (i = 0; i < csize; i++)
		print(&cm[i]);
}

print(p)
register struct cms *p;
{
	printf("%-8.8s", p->cm_comm);
	printf("%8ld", p->cm_pc);
	printf("%12.2f", p->cm_kcore);
	printf("%11.2f", p->cm_cpu);
	printf("%13.2f", p->cm_real);
	if(p->cm_cpu == 0)  p->cm_cpu = 1;
	printf("%7.2f", p->cm_kcore/p->cm_cpu);
	if(p->cm_pc == 0)  p->cm_pc = 1;
	printf("%8.2f", p->cm_cpu/p->cm_pc);
	if (p->cm_real == 0)
		p->cm_real = 1;
	printf(" %8.2f", p->cm_cpu/p->cm_real);
	printf("%12ld",p->cm_io);
	printf("%12ld\n",p->cm_rw);
}

outputc()
{
	register i;

	for (i = 0; i < csize; i++)
		fwrite(&cm[i], sizeof(cm[i]), 1, stdout);
}
