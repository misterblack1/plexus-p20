#include "sys/param.h"
#include "icp/proc.h"	/* icp specific */

#define SQSIZE 16	/* Must be power of 2 */
#define HASH(x)	(( (int) x >> 5) & (SQSIZE-1))
#ifdef COUNT
extern int profile[];
#endif
#ifdef WAITING
extern int profile[];
#endif
struct proc *slpque[SQSIZE];

#ifdef VPMSYS
struct proc proc[VPMNPROC];
#else
struct proc proc[NPROC];
#endif

int	nprocs = 0;
struct proc *runq;
struct proc *curproc;
int	runflag;
int curpri;

/*
 * Give up the processor till a wakeup occurs
 * on chan, at which time the process
 * enters the scheduling queue at priority pri.
 * Callers of this routine must be prepared for
 * premature return, and check that the reason for
 * sleeping has gone away.
 */
sleep(chan, pri)
caddr_t chan;
{
	register struct proc *rp;
	register s, h;
#ifdef COUNT
	profile[57]++;
#endif


	rp = curproc;
	s = spl6();
	rp->p_stat = SSLEEP;
	rp->p_wchan = chan;
	if (chan==0)
		panic("wchan = 0");
	rp->p_pri = pri;
	h = HASH(chan);
	rp->p_link = slpque[h];
	slpque[h] = rp;
	spl0();
	swtch();
	splx(s);
	return;
}

/*
 * Wake up all processes sleeping on chan.
 */
wakeup(chan)
register caddr_t chan;
{
	register struct proc *p, *q;
	register i;
#ifdef	VPMSYS
	int s;
#else
	register s;
	register struct proc *sp;
#endif

#ifdef COUNT
	profile[58]++;
#endif

	s = spl6();
	i = HASH(chan);
	p = slpque[i];
	q = NULL;
	while(p != NULL) {
		if(p->p_wchan==chan && p->p_stat!=SZOMB) {
#ifdef	VPMSYS
			struct proc *sp;
#endif

			if (q == NULL)
				sp = slpque[i] = p->p_link;
			else
				sp = q->p_link = p->p_link;
			p->p_wchan = 0;
			setrun(p);
			p = sp;
			continue;
		}
		q = p;
		p = p->p_link;
	}
	splx(s);
}


/*
 * Set the process running.
 */
setrun(p)
register struct proc *p;
{
	register caddr_t w;
	register s;

#ifdef COUNT
	profile[59]++;
#endif

#ifdef	VPMSYS
	if (p->p_stat==NULL || p->p_stat==SZOMB)
		panic("dead proc");
#endif
	/*
	 * The assignment to w is necessary because of
	 * race conditions. (Interrupt between test and use)
	 */
#ifdef	VPMSYS
	if (w = p->p_wchan) {
		wakeup(w);
		return;
	}
#endif
	p->p_stat = SRUN;

	/* link onto run queue */

	s = spl6();
	p->p_link = runq;
	runq = p;
	splx(s);
}


/*
 * This routine is called to reschedule the CPU.
 * if the calling process is not in RUN state,
 * arrangements for it to restart must have
 * been made elsewhere, usually by calling via sleep.
 * There is a race here. A process may become
 * ready after it has been examined.
 * In this case, idle() will be called and
 * will return in at most 1HZ time.
 * i.e. its not worth putting an spl() in.
 */
swtch()
{
	register n;
	register struct proc *p, *q, *pp, *pq;

#ifdef COUNT
	profile[60]++;
#endif

	if((curproc->p_stat != SZOMB) && save(curproc->p_rsav)) {
		return;	/* this is a resume of the current process */
	}

loop:
	spl6();
	pp = NULL;
	q = NULL;
	n = 128;
	/*
	 * Search for highest-priority runnable process
	 */
	for(p=runq; p!=NULL; p=p->p_link) {
		if(p->p_stat==SRUN) {
			if(p->p_pri <= n) {
				pp = p;
				pq = q;
				n = p->p_pri;
			}
		}
		q = p;
	}
	/*
	 * If no process is runnable, idle.
	 */
	p = pp;
	if(p == NULL) {
		runflag = 0;
		idle();
		goto loop;
	}
	runflag++;
	q = pq;
	if(q == NULL)
		runq = p->p_link;
	else
		q->p_link = p->p_link;

	/* if we are in a zombie then lay it to rest. */

	if(curproc->p_stat == SZOMB) {
		curproc->p_stat = NULL;
		nprocs--;
		wakeup( (caddr_t)&nprocs );
	}
	curpri = n;
	curproc = p;
	spl0();
	resume(curproc->p_rsav);	/* start the new process */
}


/*
 * start a new process
 */

newproc(procadx, pri, initbuf, initlen)
int	procadx;
int	pri;
int	*initbuf;
int	initlen;
{
	register struct proc *p;
	register struct proc *rpp;
	register i;
	register j;
	register s;

#ifdef COUNT
	profile[61]++;
#endif

	/* find empty process table entry */

	s = spl4();

#ifdef VPMSYS
	while ( nprocs == VPMNPROC ) {
#else
	while ( nprocs == NPROC ) {
#endif

#ifdef WAITING
		profile[15]++;
#endif
		sleep( (caddr_t)&nprocs, PWAIT );
	}
	splx( s );
	p = NULL;
#ifdef VPMSYS
	for(rpp = &proc[0]; rpp < &proc[VPMNPROC]; rpp++) {
#else
	for(rpp = &proc[0]; rpp < &proc[NPROC]; rpp++) {
#endif
		if(rpp->p_stat == NULL) {
			p = rpp;
			break;
		}
	}
	if(p == NULL) {
		panic("no procs");
	}
	nprocs++;

	/* set up new process */

	p->p_stat = SRUN;
	p->p_pri = pri;
	p->p_wchan = 0;
	p->p_procp = 0;
	p->p_pid = 0;
	p->p_wrbuf = 0;
	p->p_rsav[0] = procadx;			/* starting address of code */
	p->p_rsav[7] = 0;			/* r14 = 0 */
#ifdef VPMSYS
	p->p_rsav[8] = (int) &p->p_stk[VPMSTKSIZE];/* r15 = top of stack area */
#else
	p->p_rsav[8] = (int) &p->p_stk[STKSIZE];/* r15 = top of stack area */
#endif
	if ( initlen & 1 )
		initlen++;
	for ( i = 0, j = 0; j < initlen; i++, j += 2 )
		p->p_stk[i] = initbuf[i];

	/* put process on run queue */

	setrun(p);
}

/*
 * set a processes associated process and process id in job processor.
 */

setproc( procp, pid )
struct	proc	*procp;
{
#ifdef COUNT
	profile[62]++;
#endif

	curproc->p_procp = procp;
	curproc->p_pid = pid;
}

/*
 * delete all processes associated with the specified process group
 */

delproc( procp, pid )
struct	proc	*procp;
{
	register struct proc *p;

#ifdef COUNT
	profile[63]++;
#endif

#ifdef VPMSYS
	for ( p = &proc[0]; p < &proc[VPMNPROC]; p++ ) {
#else
	for ( p = &proc[0]; p < &proc[NPROC]; p++ ) {
#endif
		if ( p->p_procp == procp && p->p_pid == pid ) {
			if ( p->p_stat == SSLEEP ) 
				delsleep( p );
			else if ( p->p_stat == SRUN ) 
				delrun( p );
		}
	}
}

/*
 * delete the specified process from the sleep queue.
 */

delsleep( p )
struct	proc *p;
{
	register struct proc *rp, *lrp;
	register	i;

#ifdef COUNT
	profile[64]++;
#endif

	for ( i = 0; i < SQSIZE; i++ ) {
		for ( rp = slpque[i], lrp = 0; rp; rp = rp->p_link ) {
			if ( rp == p ) {
				if ( !lrp )
					slpque[i] = rp->p_link;
				else 
					lrp->p_link = rp->p_link;
				if ( rp->p_wrbuf )
					smfree( rp->p_wrbuf );
	/*				smfree( rp->p_wrbsz, rp->p_wrbuf );*/
				rp->p_stat = NULL;
				nprocs--;
				wakeup( (caddr_t)&nprocs );
				return;
			} else {
				lrp = rp;
			}
		}
	}
}

/*
 * delete a process from the runq
 */

delrun( p )
register struct proc *p;
{
	register struct proc *rp, *lrp;

#ifdef COUNT
	profile[65]++;
#endif

	for ( rp = runq, lrp = 0; rp; rp = rp->p_link ) {
		if ( rp == p ) {
			if ( !lrp )
				runq = rp->p_link;
			else 
				lrp->p_link = rp->p_link;
			if ( rp->p_wrbuf )
				smfree( rp->p_wrbuf );
/*				smfree( rp->p_wrbsz, rp->p_wrbuf );*/
			rp->p_stat = NULL;
			nprocs--;
			wakeup( (caddr_t)&nprocs );
			return;
		} else lrp = rp;
	}
}

setbufp( buf )
char	*buf;
{
#ifdef COUNT
	profile[66]++;
#endif

	curproc->p_wrbuf = buf;
/*	curproc->p_wrbsz = WBUFSIZE;*/
}

clrbufp()
{
#ifdef COUNT
	profile[67]++;
#endif

	curproc->p_wrbuf = 0;
/*	curproc->p_wrbsz = 0;*/
}


/*
 * terminate a process
 */

exit()
{

#ifdef COUNT
	profile[68]++;
#endif

	curproc->p_stat = SZOMB;
	swtch();
}


/*
 * start up the process structure.
 */

startup()
{

#ifdef COUNT
	profile[69]++;
#endif

#ifdef VPMSYS
	curproc = &proc[VPMNPROC-1];
#else
	curproc = &proc[NPROC-1];
#endif
	nprocs++;
	exit();
}


