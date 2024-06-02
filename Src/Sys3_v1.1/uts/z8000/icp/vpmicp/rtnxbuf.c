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
/*  function: rtnxbuf                                               */
/*                                                                  */
/*  purpose:                                                        */
/*    Return a 'transmit' buffer.  The original values of the       */
/*    buffer parameters for the current transmit buffer are         */
/*    replaced with values from the array specified by 'name'.  The */
/*    current transmit buffer is then released to the driver.       */
/*                                                                  */
/*  calling syntax:                                                 */
/*                  rtnxbuf(tp,nameaddr);                           */
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

rtnxbuf(tp,array)

	  register char *array[];
	  register struct tty *tp;
{

          register s;

#ifdef COUNT
       	  profile [103]++;
#endif
/*
	  printf("rtnxbuf:\n");
*/
	  s = spl6();
          vpmputcf(tp);
          splx( s );
}
vpmputcf(tp)

	  register struct tty *tp;
{
          register struct cblock *tptr;
          register struct clist *ptr;
          register struct clist *ptr1;

	  ptr = &tp->t_outq;
	  ptr1 = &tp->v_dupqoutq;
	    
	  if ((ptr->c_cf == tp->v_outqptr) && (tp->v_outqptr != NULL)) {

	       tptr = tp->v_outqptr;
	       if (tp->v_cfirst < tptr->c_last) {
		    ptr->c_cc -= tp->v_cfirst;
		    tptr->c_first = tp->v_cfirst;
		    tp->t_buf = tp->v_outqptr;
	            return;
               }
          }
     nextone:
          /*
	  printf("rtnxbuf: v_outqptr=");
	  printf("%x\n", tp->v_outqptr);
          */
          if (tp->v_outqptr != NULL) {

	       if ((tptr = getcb(&tp->t_outq)) == NULL) {
		    ptr1->c_cf = 0;
		    return;

               } else {
		       if (tptr == tp->v_outqptr) {
		            if (tp->v_cfirst < tptr->c_last) {

				 ptr = &tp->t_outq;
				 ptr->c_cc -= tp->v_cfirst;
				 tptr->c_first = tp->v_cfirst;
                                 tp->t_buf = tp->v_outqptr;
			         return;  
		            } else {
				    /*
		                    printf("rtnxbuf: tptr=");
				    printf("%x\n", tptr);
                                    */
                                    putcf(tptr);
				    tp->v_outqptr = NULL;
				    tp->t_buf = NULL;
				    tp->v_cfirst = 0;
			    }
                       } else {
                               /*
			       printf("rtnxbuf: tptr=");
			       printf("%x\n", tptr);
                               */
                               putcf(tptr);
                       }
               }
          } else {
		  ptr1->c_cf = 0;
		  return;
          }

	  ptr = &tp->t_outq;
          tptr = ptr->c_cf;
          /*
	  printf("rtnxbuf: ptr->c_cf=");
          printf("%x\n", ptr->c_cf);
          */
          if (tptr == 0) {
	       ptr1->c_cf = 0;
	       return;
          }
          if (tptr == tp->v_outqptr) {
               /*
	       printf("rtnxbuf: tptr->c_last=");
	       printf("%x\n", tptr->c_last);
               */
	       if (tp->v_cfirst < tptr->c_last) {
		    ptr->c_cc -= tp->v_cfirst;
		    tptr->c_first = tp->v_cfirst;
		    tp->t_buf = tp->v_outqptr;
	            return;  
	       } 
	  }
     goto nextone;
}
