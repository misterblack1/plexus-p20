/*
 * Routines to handle the timer functions.
 */

#include "sys/param.h"
#include "sys/callo.h"
#include "icp/proc.h"	/* icp specific */
#include "icp/sioc.h"	/* icp specific */

#ifdef COUNT
int profile[350];
#endif
#ifdef WAITING
int profile[350];
#endif
/*int clicks = 0;*/


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
/*                                                              */
/* Clock is called by the interrupt handler every 20ms (50 Hz)  */
/* This routine implements the callouts.                        */
/*                                                              */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

#ifdef VPMSYS
int timerid = 1;
#endif

extern	struct proc;
clock(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
register unsigned int pc;
{
	register struct callo *p1, *p2;
	extern cmdsav;

#ifdef COUNT
	profile[34]++;
#endif
	/*-------------------------------------------------------------------*\
	| toggle LED once a second. Bit is set in cmdsav to turn on/off led.  |
	| When acknowedgement to ICP request or signal/message to host, there |
	| will be a multibus interrupt. At this time the led will be updated  |
	| to the current state. This means there may be a slight delay        |
	| between setting the bit and when the led is lit.		      |
	\*-------------------------------------------------------------------*/

/*
	disable until account or someother periodic mb event is generated.
	if(++clicks >= 50) {
		clicks = 0;
		cmdsav ^= 020;
	}
*/

	/*
	 * callouts
	 * if none, just continue
	 * else update first non-zero time
	 */

	if(callout[0].c_func == NULL)
		goto out;
	p2 = &callout[0];
	while(p2->c_time<=0 && p2->c_func!=NULL)
		p2++;
	p2->c_time--;


	/*
	 * callout
	 */

	if(callout[0].c_time <= 0) {
		p1 = &callout[0];
		while(p1->c_func != 0 && p1->c_time <= 0) {
			(*p1->c_func)(p1->c_arg);
#ifdef VPMSYS
			p1->c_id = 0;
#endif
			p1++;
		}
		p2 = &callout[0];
		while(p2->c_func = p1->c_func) {
			p2->c_time = p1->c_time;
#ifdef VPMSYS
			p2->c_id = p1->c_id;
#endif
			p2->c_arg = p1->c_arg;
			p1++;
			p2++;
		}
	}

	out:
	putreti();	/* reset the clock interrupt */
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*                                                       */
/* timeout is called to arrange that                     */
/* fun(arg) is called in tim/HZ seconds.                 */
/*                                                       */
/* For VPM a timer id has been added.                    */
/*                                                       */
/* An entry is sorted into the callout                   */
/* structure. The time in each structure                 */
/* entry is the number of HZ's more                      */
/* than the previous entry.                              */
/* In this way, decrementing the                         */
/* first entry has the effect of                         */
/* updating all entries.                                 */
/*                                                       */
/* The panic is there because there is nothing           */
/* intelligent to be done if an entry won't fit.         */
/*                                                       */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef VPMSYS
timeout(fun, arg, tim, id)
#else
timeout(fun, arg, tim)
#endif
int (*fun)();
caddr_t arg;
#ifdef VPMSYS
register int id;
#endif
{
	register struct callo *p1, *p2;
	register int t;
	int s;

#ifdef COUNT
	profile[35]++;
#endif

	t = tim;
	p1 = &callout[0];
	s = spl7();

	/* The check for 1000 here is significant.  "Kernel" */
	/* timeouts have timer ids greater than 2000.  VPM   */
	/* protocol script timeouts have rotating timer ids  */
	/* of 1 thru 1000.  Therefore, the check does not    */
	/* only refer to the script timer ids but also       */
	/* static kernel timer ids.                          */

#ifdef VPMSYS
	if(id > 1000)
	     goto nonein;

	/* Make sure that the timerid for the protocol       */
	/* scripts never gets to zero.  Zero means the       */
	/* absence of an id.                                 */

	if(timerid > 1000)
	     timerid = 1;

	if(callout[0].c_func == NULL)
	     goto nonein;

	/* we must check if the timer id passed */
	/* equals any ids in the timer list.    */

        while(p1 <= &callout[NCALL-1]) {
	     if(p1->c_id == id) {
  
                 panic("to: ids same");
             }
	     p1++;
        }
	p1 = &callout[0];

	nonein:
#endif
	while(p1->c_func != 0 && p1->c_time <= t) {
		t -= p1->c_time;
		p1++;
	}
	if (p1 >= &callout[NCALL-1])
		panic("to ovrflo");
	p1->c_time -= t;
	p2 = p1;
	while(p2->c_func != 0)
		p2++;
	while(p2 >= p1) {
		(p2+1)->c_time = p2->c_time;
#ifdef VPMSYS
		(p2+1)->c_id   = p2->c_id;
#endif
		(p2+1)->c_func = p2->c_func;
		(p2+1)->c_arg = p2->c_arg;
		p2--;
	}
	p1->c_time = t;

#ifdef VPMSYS
	p1->c_id = id;
#endif

	p1->c_func = fun;
	p1->c_arg = arg;
	splx(s);
}

/* - - - - - - - - - - - - - - - - - - - - */
/*                                         */
/*  function delay                         */
/*                                         */
/*                                         */
/* - - - - - - - - - - - - - - - - - - - - */

#define	PDELAY	(PZERO-1)
delay(ticks)
{
	extern wakeup();
	extern struct proc *curproc;
	int s;

#ifdef COUNT
	profile[36]++;
#endif

	if (ticks<=0)
		return;
	s = spl5();

#ifdef VPMSYS
	timeout(wakeup, (caddr_t)&curproc->p_pid, ticks, 2001);
#else
	timeout(wakeup, (caddr_t)&curproc->p_pid, ticks);
#endif

#ifdef WAITING
	profile[5]++;
#endif
	sleep((caddr_t)&curproc->p_pid, PDELAY);
	splx(s);
}

#ifdef VPMSYS
/* - - - - - - - - - - - - - - - - - - - - */
/*                                         */
/*  cantimer is called to cancel the timer */
/*  entry associated with "id".            */
/*                                         */
/* - - - - - - - - - - - - - - - - - - - - */

cantimer(id)
register int id;
{
	register struct callo *p1;
	int s;

#ifdef COUNT
	profile[37]++;
#endif

	s = spl7();
	if (callout[0].c_func == NULL)
	     goto out1;

	p1 = &callout[0];
        while (p1 <= &callout[NCALL-1]) {
             if(p1->c_id == id) {
		  if(p1 == &callout[NCALL-1]) {
		          p1->c_time = 0;
		          p1->c_id = 0;
		          p1->c_func = 0;
		          p1->c_arg = 0;
		          goto out1;
                  } else {
			       /*				*/
			       /* Add the increment just for	*/
			       /* the first move up		*/
			       /*				*/
		               p1->c_time  += (p1+1)->c_time;
		               p1->c_id    = (p1+1)->c_id;
			       p1->c_func  = (p1+1)->c_func;
			       p1->c_arg   = (p1+1)->c_arg;
			       p1++;
			  while ((p1+1) <= &callout[NCALL-1]) {
		               p1->c_time  = (p1+1)->c_time;
		               p1->c_id    = (p1+1)->c_id;
			       p1->c_func  = (p1+1)->c_func;
			       p1->c_arg   = (p1+1)->c_arg;
			       p1++;
                          }
			  goto out1;
		  }
	     } else {
		     p1++;
             }
        }
	out1:
	splx(s);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*                                                     */
/* The following function is called by the "timer"     */
/* primitive to return the current value of the timer  */
/* value associated with the passed "id".              */
/*                                                     */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - */

timeval(id)
register int id;
{
     register struct callo *p1;
     int s;

     s = spl7();
     p1 = &callout[0];
     if(callout[0].c_func== NULL)
	  goto out;

     while(p1 <= &callout[NCALL-1]) {
	  if(p1->c_id == id) {
	       if(p1->c_time) {
		    splx( s );
		    return(1);
               } else {
		       splx( s );
		       return(0);
               }
           }
	   p1++;
     }
     out:
     splx( s );
     return(0);
}
#endif
