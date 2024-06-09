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
/*  function: xeom                                                  */
/*                                                                  */
/*  purpose: The VPM protocol script issues this function call      */
/*           in order to "send" the "constant" in the argument      */
/*           passed and terminate the transmit block.               */
/*                                                                  */
/*  calling syntax:                                                 */
/*                  n = xeom(tp,constant);                          */
/*                                                                  */
/*                  where: n is the return value of "0" or "1".     */
/*                              "0" means retry "xeom" later with   */
/*                                  with the same calling args.     */
/*                              "1" means good return.              */
/*                                                                  */
/*                  where: tp is the pointer to the current entry   */
/*                               into "ttytab" for this current     */
/*                               device being serviced.             */
/*                                                                  */
/*                  where: constant is the value of a byte that     */
/*                                  is to be sent.                  */
/*                                                                  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

xeom(tp,constant)

	register struct tty *tp;
	char constant;
{
	register char *constptr;
	register struct clist *ptr;
	register struct clist *ptr1;
	register struct cblock *cp;

	char constff = 0xff;

#ifdef COUNT
	profile [102]++;
#endif
/*
      printf("xeom\n");

      printf("xeom: xeomstate=");
      printf("%d\n", tp->p_xeomstate);
*/
swtchagn:
      switch(tp->p_xeomstate) {

	/* ************************************* */
	/*					 */
	/*  case 0 : inserts the passed constant */
	/*	     into the transmit buffer 	 */
	/*					 */
	/* ************************************* */

        case 0:
          constptr = &constant;
/*
	  printf("xeom: constant=");
	  printf("%x\n", constant);
	  printf("xeom: &constant");
	  printf("%x\n", constptr);
*/
	  if (xmt(tp,constptr)) {

			     tp->p_xeomstate = 1;
                             break;
          } else {
/*
		  printf("xeom: ret(0) from xmt\n");
*/
		  tp->p_xeomstate = 0;
		  return(0);
          }

	/* ************************************* */
	/*					 */
	/*  case 1 : puts first '0xff' constant  */
	/*	     into the transmit buffer 	 */
	/*					 */
	/*   These extra 'mark' chars are	 */
	/*   needed because when 'RTS' is taken  */
	/*   down in the 'dma' interrupt routine */
	/*   two 'mark' chars still remain in    */
	/*   in the transmit SIO chip buffer.    */
	/*					 */
	/*   This in effect, delays four char    */
	/*   times.  The actual char time delay  */
	/*   depends on the speed of the sync    */
	/*   modem.                              */
	/*					 */
	/* ************************************* */

	case 1:
	  constptr = &constff;

	  if (xmt(tp,constptr)) {

			     tp->p_xeomstate = 2;
                             break;
          } else {
		  tp->p_xeomstate = 1;
		  return(0);
          }

	/* ************************************* */
	/*					 */
	/*  case 2 : puts 2nd '0xff' constant    */
	/*	     into the transmit buffer 	 */
	/*					 */
	/*   These extra 'mark' chars are	 */
	/*   needed because when 'RTS' is taken  */
	/*   down in the 'dma' interrupt routine */
	/*   two 'mark' chars still remain in    */
	/*   in the transmit SIO chip buffer.    */
	/*					 */
	/*   This in effect, delays four char    */
	/*   times.  The actual char time delay  */
	/*   depends on the speed of the sync    */
	/*   modem.                              */
	/*					 */
	/* ************************************* */

	case 2:
	  constptr = &constff;

	  if (xmt(tp,constptr)) {

			     tp->p_xeomstate = 3;
                             break;
          } else {
		  tp->p_xeomstate = 2;
		  return(0);
          }

	/* ************************************* */
	/*					 */
	/*  case 3 : puts 3rd '0xff' constant    */
	/*	     into the transmit buffer 	 */
	/*					 */
	/*   These extra 'mark' chars are	 */
	/*   needed because when 'RTS' is taken  */
	/*   down in the 'dma' interrupt routine */
	/*   two 'mark' chars still remain in    */
	/*   in the transmit SIO chip buffer.    */
	/*					 */
	/*   This in effect, delays four char    */
	/*   times.  The actual char time delay  */
	/*   depends on the speed of the sync    */
	/*   modem.                              */
	/*					 */
	/* ************************************* */

	case 3:
	  constptr = &constff;

	  if (xmt(tp,constptr)) {

			     tp->p_xeomstate = 4;
                             break;
          } else {
		  tp->p_xeomstate = 3;
		  return(0);
          }

	/* ************************************* */
	/*					 */
	/*  case 4 : puts 4th '0xff' constant    */
	/*	     into the transmit buffer 	 */
	/*					 */
	/*   These extra 'mark' chars are	 */
	/*   needed because when 'RTS' is taken  */
	/*   down in the 'dma' interrupt routine */
	/*   two 'mark' chars still remain in    */
	/*   in the transmit SIO chip buffer.    */
	/*					 */
	/*   This in effect, delays four char    */
	/*   times.  The actual char time delay  */
	/*   depends on the speed of the sync    */
	/*   modem.                              */
	/*					 */
	/* ************************************* */

	case 4:
	  constptr = &constff;

	  if (xmt(tp,constptr)) {

			     tp->p_xeomstate = 5;
                             break;
          } else {
		  tp->p_xeomstate = 4;
		  return(0);
          }

	/* ************************************* */
	/*					 */
	/*  case 5 : causes the buffer to be     */
	/*	     sent via the 'dma' chip. 	 */
	/*					 */
	/* ************************************* */

        case 5:
           
		  /*  The 'v_devq' will now be sent out on the comm line  */

		  spl6();

		  if (tp->t_state & BUSY) {
		       tp->p_xeomstate = 5;
		       spl0();
/*
		       printf("xeom: waiting");
*/
		       return(0);
		  }
/*
		  printf("xeom:case 5");
*/
		while ((cp = getcb(&tp->v_rcvholdq)) != NULL)
			putcf(cp);

		while ((cp = getcb(&tp->t_rawq)) != NULL)
			putcf(cp);

		  sioproc(tp,T_OUTPUT);
		  tp->p_xeomstate = 0;
		  spl0();

		  tp->t_buf = NULL;
		  ptr = &tp->t_outq;
		  ptr1 = &tp->v_dupqoutq;
		  ptr1->c_cf = ptr->c_cf;

		  return(1);
	}
	goto swtchagn;
}
