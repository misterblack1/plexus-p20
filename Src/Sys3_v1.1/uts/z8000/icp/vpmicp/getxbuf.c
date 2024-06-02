#include "sys/param.h"
#include "sys/tty.h"
#ifdef COUNT
extern int profile[];
#endif
#ifdef WAITING
extern int profile[];
#endif

extern int tthiwat[], ttlowat[];

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
/*                                                                  */
/*  function: getxbuf                                               */
/*                                                                  */
/*  purpose:                                                        */
/*    Get (open) a transmit buffer.  The returned value is zero if  */
/*    a buffer  is available, otherwise it is non-zero.  If a       */
/*    buffer is obtained, the buffer parameters are copied into the */
/*    array specified by 'name'.  The array should be large         */
/*    enough to hold at least three bytes.  The meaning of the      */
/*    buffer parameters is driver-dependent.  If a transmit buffer  */
/*    has previously been opened via a 'getxbuf' call but has not   */
/*    yet been closed via a call to 'rtnxbuf', that buffer is       */
/*    reinitialized and remains the current buffer.                 */
/*                                                                  */
/*  calling syntax:                                                 */
/*                  n = getxbuf(tp,nameaddr);                       */
/*                                                                  */
/*                  where: n is the return value of "0" or "1".     */
/*                              "0" means a buffer is available.    */
/*                              "1" means a buffer is not available */
/*                                                                  */
/*                  where: tp is the pointer to the current entry   */
/*                               into "ttytab" for this current     */
/*                               device being serviced.             */
/*                                                                  */
/*                  where: nameaddr is the pointer to where the     */
/*                                  buffer parameters are copied.   */
/*                                                                  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

getxbuf(tp,array)

	register char *array;
	register struct tty *tp;
{
	register struct clist *ptr;
	register struct clist *ptr1;
	register struct cblock *tptr;
	register s;
	register char *cp;

#ifdef COUNT
	profile [103]++;
#endif
/*
	printf("getxbuf:\n");
*/
	ptr = &tp->t_outq;
	ptr1 = &tp->v_dupqoutq;
/*
	printf("tp=%x\n", tp);
	printf("tp->t_outq.c_cc=%x\n", tp->t_outq.c_cc);
*/
	s = spl6();

	if (tp->t_state&OASLP &&
		tp->t_outq.c_cc <= ttlowat[tp->t_cflag&CBAUD]) {
			tp->t_state &= ~OASLP;

			wakeup((caddr_t)&tp->t_outq);
	}
	if (tp->t_outq.c_cc == 0) {
		splx ( s );
		return(1);
	}
	if(tp->v_xbufin) {
		splx ( s );
		return(1);
	}
	if (ptr1->c_cf == 0) {
		ptr1->c_cf = ptr->c_cf;
		tp->v_outqptr = 0;
	}
   
	if (tp->t_outq.c_cc) {
                 
		if (ptr->c_cf == 0) {

			tp->t_outq.c_cc = 0;
		   
			splx( s );
			return (1);
		} else {
/*
			printf("array=%x\n", array);
			printf("*cp=%x\n", *cp);
*/
			tptr = tp->v_outqptr;
			if(tptr && tptr->c_next == 0 &&
						tp->v_cfirst >= tptr->c_last) {
				tp->v_outqptr = 0;
			}
			cp = array;
			*cp++ = tp->v_xbfname[0];
			*cp++ = tp->v_xbfname[1];
			*cp = tp->v_xbfname[2];

			splx ( s );
			return (0);
		}
	} else {

		splx( s );
		return(1);
	}
}
