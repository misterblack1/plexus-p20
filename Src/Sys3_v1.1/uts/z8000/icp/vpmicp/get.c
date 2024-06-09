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
/*  function: get                                                   */
/*                                                                  */
/*  purpose: The VPM protocol script issues this function call      */
/*           in order to obtain a byte from the transmit buffer.    */
/*                                                                  */
/*  calling syntax:                                                 */
/*                  n = get(tp,byteaddr);                           */
/*                                                                  */
/*                  where: n is the return value of "0" or "1".     */
/*                              "0" means no byte was obtained.     */
/*                              "1" means a byte is being returned. */
/*                                                                  */
/*                  where: tp is the pointer to the current entry   */
/*                               into "ttytab" for this current     */
/*                               device being serviced.             */
/*                                                                  */
/*                  where: byteaddr is the pointer to where the     */
/*                                  obtained byte from the transmit */
/*                                  will be placed.                 */
/*                                                                  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

get(tp,byte)

	  register char *byte;
	  register struct tty *tp;
{
	  register char *cp;
          register s;
          struct cblock *vpmgetcb();
	  struct cblock *tptr;

#ifdef COUNT
	profile [100]++;
#endif
/*
	       printf("get:\n");
        
	       printf("tp=");
	       printf("%x\n", tp);
               
	       printf("tp-t_buf=");
	       printf("%x\n", tp->t_buf);
*/ 
          s = spl4();
          if (tp->t_state&OASLP &&
	       tp->t_outq.c_cc <= ttlowat[tp->t_cflag&CBAUD]) {
		   tp->t_state &= ~OASLP;

		   wakeup((caddr_t)&tp->t_outq);
          }    

	  tptr = tp->t_buf;

	  if ( tp->t_buf && tp->v_cfirst >= tp->t_buf->c_last) {

/*
	       printf("tp-t_buf=NULL");
*/
	       tp->t_buf = NULL;
          }

	  if (tp->t_buf == NULL) {

	       /*  get the next cblock from the output queue  */

	       if ((tp->t_buf = vpmgetcb(tp,&tp->v_dupqoutq)) == NULL) {
   
	       /*  return a value of "0" if no more in the cblock  */
                           /*
			   printf("get.c = 0\n");
                           */
			   splx( s );
			   return(0);
	       } else {

		       tp->v_outqptr = tp->t_buf;
		       tp->v_cfirst = 0;
               }
          }
	  cp = &tp->t_buf->c_data[tp->v_cfirst];
          /*
	  if(tp->t_iflag) {
                           printf("cp=");
	                   printf("%x\n", cp);
	  }
          */
	  /*  return the byte gotten from the transmit queue  */

	  *byte = *cp;

	  /*  increment the pointer to the first byte to point to the  */
	  /*  next byte in the transmit queue.                         */

	  tp->v_cfirst++;
          /*
          printf("v_cfirst=");
	  printf("%x\n", tp->v_cfirst);
          */
	  /*  return "1" for the value of the function  */

	  splx( s );
	  return(1);
}
struct cblock *vpmgetcb(tp,ptr)
register struct tty *tp;
register struct clist *ptr;
{
    register struct cblock *tptr;
    register s;

#ifdef COUNT
	profile[29]++;
#endif

    tptr = tp->v_outqptr;
    if(tptr != 0) {
	 if(tptr->c_next == 0) {
	      return(0);
         }
         tptr = tptr->c_next;
         return tptr;
    }
    tptr=ptr->c_cf;
/*
	 printf("get: ptr->c_cf=");
	 printf("%x\n", ptr->c_cf);
*/
    return tptr;
}
