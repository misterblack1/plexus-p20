#include "sys/plexus.h"
#include "sys/param.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/inode.h"
#include "sys/proc.h"
#include "sys/seg.h"
#include "sys/map.h"
#include "sys/reg.h"
#include "sys/buf.h"
#include "sys/var.h"
#include "sys/psl.h"
#include "sys/utsname.h"
#include "sys/conf.h"

/*
 * Machine-dependent startup code
 *
 * There are two critical timing loops in this code ... the first has to
 * do with resetting the multibus, the second has to do with determining
 * the baud rate ... if the compiler changes for any reason, these loops
 * should be checked to make sure they are still valid.
 */
startup(firstaddr)
{
	register unsigned i;	/* index */
	register unsigned seg;	/* segment number */
	register page;		/* page in segment */
	long timo;		/* timer count (must not be a register var) */
	register unsigned mbseg;	/* multibus segment number */

	/*
	 * zero and free all of core
	 */

ledson(1);
	printf("\nSYS3/%s: %s%s\n", utsname.release, utsname.sysname, utsname.version);
	if(profbuf.profsf != -1) printf("profiling enabled\n");

	/*
	 * read in switch register
	 */

	diagswits = (~in_local(P_SWIT)) & 0xf;

	/*
	 * set up system profiling segment 
	 */

ledson(2);
	if(profbuf.profsf != -1) {
	    for(seg=PROFSEG,page=0; page < (32>>profbuf.profsf);page++) {
	    	out_local(mapport(seg,page),phypage);
	    	clearseg(phypage++);
	    }
	    for(;page < 32;page++) 
	    	out_local(mapport(seg,page),B_IP | B_RO);
	}

	/*
	 * zero and free all of core
	 */

	for(;;) {
ledson(3);
		out_local(P_MUI0, phypage);	/* set as first in user code */
		suiword(0, 0x0f0f);	/* write to first word of page */
		if(fuiword(0) != 0x0f0f) {
			break;
		}
ledson(4);
		clearseg(phypage);
		maxmem++;
ledson(5);
		mfree(coremap, 1, phypage);
		phypage++;
	}

ledson(6);
	printf("real mem = %D bytes\n",
		sysmem = ctob((long)(maxmem+firstaddr+USIZE)-FIRSTPPN));

	/*
	 * set up all map registers except those for the system code
	 * and data to have the invalid page bit set so that illegal
	 * memory references will get caught.
	 * profbuf.profsf == -1 turns off profiling.
	 */

ledson(7);
	for (seg = MINSEG; seg <= MAXSEG; seg += SEGINC) {
		if (seg != SDSEG &&
		    seg != SISEG &&
#ifdef	OVKRNL
		    seg != SDSEG1 &&
		    seg != SISEG1 &&
		    seg != SDSEG2 &&
		    seg != SISEG2 &&
		    seg != SDSEG3 &&
		    seg != SISEG3 &&
		    seg != SDSEG4 &&
		    seg != SISEG4 &&
#endif
		    (profbuf.profsf == -1 || seg != PROFSEG) &&
		    seg != PRMDSEG &&
		    seg != PRMISEG) {
			for( page = 0; page < NUMLOGPAGE; page++) {
				out_local(mapport(seg, page), B_IP | B_RO);
			}
		}
	}


	/* set up virtual buffer segment. allocate up to
	 * one segments worth of buffers. note that
	 * the unused slots in BFSEG (if any) have already
	 * been invalidated above.
	 */

	/* protect the system from an irrational number of buffers */
ledson(8);
	if ((v.v_buf < 4) || (v.v_buf > (NUMBUFMAPS * 64)))
		v.v_buf = 16;

	for (i=0;i<(v.v_buf+(PAGESIZE/BSIZE)-1)/(PAGESIZE/BSIZE);i++) {
		if ((page=malloc(coremap,1)) == 0) {
			panic("buffers");
			}
		out_local(mapport(BFSEG,i),page);
		maxmem--;
	}
	bufbase = (paddr_t)(segtoaddr(BFSEG));


	/*
	 * set up multibus mapper to its initial state.
	 * Most ports point to BADSEG which is a segment whose pages
	 * all are invalid. This is to detect bad data accesses coming
	 * into the memory manager thru the multibus mapper.
	 * Note that multibus map entries are zero true!
	 */

ledson(9);
	for (mbseg = MINMBSEG; mbseg <= MAXMBSEG; mbseg += MBSEGINC)
		out_local(mb_mapport(mbseg), ~SEGTOMAP(mbtoseg(mbseg)));


	/*
	 * allow multibus access to main memory.
	 */

ledson(10);
	out_local(P_MBUNLOCK, 0);


	/*
	 * reset i/o controller boards by holding multibus 'init' line
	 * for about one second.
	 */

ledson(11);
	spl7();
	out_local(P_PIOADAT, in_local(P_PIOADATA) & (~B_ALLOWRESET));
	out_local(P_PIOADAT, in_local(P_PIOADATA) & (~B_RESETOFF));
	out_local(P_PIOADAT, in_local(P_PIOADATA) | B_RESETOFF);
	for (timo = 0; timo < 60000; timo++) {
	}
ledson(12);
	out_local(P_PIOADAT, in_local(P_PIOADATA) | B_ALLOWRESET);
	spl0();


	/*
	 * call device controller initialization routines.
	 */

	for(i = 0; initsw[i].d_init; i++) {
		ledson(10-i);
		(*initsw[i].d_init)();
	}

ledson(14);
	printf("avail mem = %D bytes\n", ctob((long)maxmem));
	if(MAXMEM < maxmem)
		maxmem = MAXMEM;
#ifndef BIGBLK
	mfree(swapmap, nswap, 1);
#else
	mfree(swapmap, nswap - CXSIZE, CXSIZE);
#endif
	swplo--;
}

/*
 * General routines to turn the leds on and off
 */
ledson(ledval) {
	out_local(P_PIOBDATA, ledval);
}

ledsor(ledval) {
	out_local(P_PIOBDATA, in_local(P_PIOBDATA) | ledval);
}

ledsand(ledval) {
	out_local(P_PIOBDATA, in_local(P_PIOBDATA) & ledval);
}

ledsxor(ledval) {
	out_local(P_PIOBDATA, in_local(P_PIOBDATA) ^ ledval);
}

/*
 * Mbtoseg determines the mapping between map segments and multibus.
 * NOTE -- This could be done more efficiently with a defined array.
 */
mbtoseg(mbseg)
unsigned mbseg;
{
	switch(mbseg) {
	case MBSDSEG:
		return(SDSEG);
	case MBIOSEG:
		return(MBSEG);
	case MBBFSEG:
		return(BFSEG);
	case MBBFSEG1:
		return(BFSEG1);
	case MBBFSEG2:
		return(BFSEG2);
	case MBBFSEG3:
		return(BFSEG3);
	default:
		return(BADSEG);
	}
}

/*
 * Segtomb determines the mapping between the multibus and map segments.
 * NOTE -- This could be done more efficiently with a defined array.
 */
segtomb(seg)
unsigned seg;
{
	switch(seg) {
	case SDSEG:
		return(MBSDSEG);
	case MBSEG:
		return(MBIOSEG);
	case BFSEG:
		return(MBBFSEG);
	case BFSEG1:
		return(MBBFSEG1);
	case BFSEG2:
		return(MBBFSEG2);
	case BFSEG3:
		return(MBBFSEG3);
	default:
		return(MBSDSEG);
	}
}

/*
 * set up a physical address
 * into users virtual address space.
 */
sysphys()
{
	register i, s, d;
	register struct a {
		int	segno;
		int	size;
		int	phys;
	} *uap;

	if(!suser())
		return;
	u.u_error = EINVAL;
}

/*
 * start the clock.
 *
 * to determine the rate of the clock, wait until the batteried clock
 * ticks, wait for at least a 4MHZ second, and see if the batteried clock
 * has ticked again -- if so, it is 4MHZ, otherwise it must be 5MHZ
 * the default here is 5MHZ if the batteried clock is broken.
 *
 */

#define MHZ4	100
#define MHZ5	125
#define SLOWSEC 84905		/* for 4MHZ, this is more than a second */
				/* but for 5MHZ, it is less than a second */
				/* this is calculated assuming the loop */
				/* executes 67163 times at 4MHZ and */
				/* 94339 times at 5 MHZ */			


clkstart()
{
	long i;
	long oldtime;

	i = 0;
	if ((oldtime = gettod()) <= 100000) {
		puttod(1L);
		oldtime = gettod();
	}
	while (oldtime == gettod())
		if(++i > SLOWSEC/100) break;	
	for (i=0; i<SLOWSEC; i++);
	if (gettod() - oldtime > 1)
		ctc0tim = MHZ4;
	else
		ctc0tim = MHZ5;

	out_local(P_CTC0, CTC0CTL);
	out_local(P_CTC0, ctc0tim);
	out_local(P_CTC1, CTC1CTL);
	out_local(P_CTC1, CTC1TIM);
	out_local(P_MPIC1, in_local(P_MPIC1) & (~CLKINT));  /* allow clk int */
}

clkset(oldtime)
time_t	oldtime;
{
	time = oldtime;
}

/*
 * Let a process handle a signal by simulating an interrupt
 */
sendsig(p, signo)
caddr_t p;
{
	register unsigned n;

	n = u.u_ar0[R15] - 6;
	grow(n);
	suword((caddr_t)n, signo);
	suword((caddr_t)n+2, u.u_ar0[FCW]);
	suword((caddr_t)n+4, u.u_ar0[PC]);
	u.u_ar0[R15] = n;
	u.u_ar0[FCW] &= ~B_SINGL;
	u.u_ar0[PC] = (int)p;
}

/* CCF -- mapalloc mapfree can be deleted when raw works with mbm.c
 * This code allocates the multibus map and sets it to point ar
 * the proper page numbers.
 */

int	maplock;

mapalloc(bp)
register struct buf *bp;
{
	register page;	/* logical page number in multibus segment */
	register ppn;	/* physical page number */
	unsigned maptemp[NUMLOGPAGE];

	spl6();
	while(maplock&B_BUSY) {
		maplock |= B_WANTED;
		sleep((caddr_t)&maplock, PSWP+1);
	}
	maplock |= B_BUSY;
	spl0();
	bp->b_flags |= B_MAP;
	/* Find the number of the first page in this physio block */
	for (ppn = bp->b_paddr>>S_PAGENUM, page = 0; page < NUMLOGPAGE;
		ppn++, page++) {
		maptemp[page] = ppn;
	}
	copymout(maptemp, P_MMB0, NUMLOGPAGE << 1);
	/* Here we change back from a raw physical address to new logical one */
	bp->b_paddr = ((paddr_t)(segtoaddr(MBSEG))) + (bp->b_paddr&M_BIP);
}

mapfree(bp)
struct buf *bp;
{

	bp->b_flags &= ~B_MAP;
	if(maplock&B_WANTED)
		wakeup((caddr_t)&maplock);
	maplock = 0;
}

/*
 * copy count bytes from from to to.
 */
bcopy(from, to, count)
caddr_t from, to;
register count;
{
	register char *f, *t;

	f = from;
	t = to;
	do
		*t++ = *f++;
	while(--count);
}

/*
 * create a duplicate copy of a process
 */
procdup(p)
register struct proc *p;
{
	register a1, a2, n;

	n = p->p_size;
	if ((a2 = malloc(coremap, n)) == NULL)
		return(NULL);

	a1 = p->p_addr;
	p->p_addr = a2;
	while(n--)
		copyseg(a1++, a2++);
	return(1);
}
