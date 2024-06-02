#include "sys/param.h"
#include "sys/tty.h"
#ifdef COUNT
extern int profile[];
#endif
#ifdef WAITING
extern int profile[];
#endif

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
/*                                                                  */
/*  function: rtnrbuf                                               */
/*                                                                  */
/*  purpose:                                                        */
/*    Return a 'receive' buffer.  The original values of the        */
/*    buffer parameters for the current receive buffer are          */
/*    replaced with values from the array specified by 'name'.  The */
/*    current receive buffer is then released to the driver.        */
/*                                                                  */
/*  calling syntax:                                                 */
/*                  rtnrbuf(tp,nameaddr);                           */
/*                                                                  */
/*                  where: tp is the pointer to the current entry   */
/*                               into "ttytab" for this current     */
/*                               device being serviced.             */
/*                                                                  */
/*                  where: nameaddr is the pointer to where the     */
/*                                  new buffer parameters are       */
/*                                  taken and placed into the area  */
/*                                  where the driver will pick them */
/*                                  up.                             */
/*                                                                  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

rtnrbuf(tp,array)

	register char *array;
	register struct tty *tp;
{
	register struct cblock *tptr;
	int firstime;
	register char *cp;

#ifdef COUNT
	profile [106]++;
#endif
/*
	printf("rtnrbuf:\n");
*/
	if (tp->v_rcvholdq.c_cc == 0) {
		return;
	}

	/*  wakeup the sleep in 'sioread()' */

	wakeup ((caddr_t) &tp->t_canq);

	firstime = 1;

	again:
		if ((tptr=getcb(&tp->v_rcvholdq)) == NULL) {
			return;

		} else {
			if(firstime) {

				cp = array;
				tp->v_rbfname[0] = tptr->c_data[0] = *cp++;
				tp->v_rbfname[1] = tptr->c_data[1] = *cp++;
				tp->v_rbfname[2] = tptr->c_data[2] = *cp;

				firstime = 0;
			}
			putcb(tptr,&tp->t_canq);
		}
		goto again;
}
