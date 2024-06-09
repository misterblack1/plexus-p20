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
/*  function: xmt                                                   */
/*                                                                  */
/*  purpose: The VPM protocol script issues this function call      */
/*           in order to "send" a byte to the comm line.            */
/*                                                                  */
/*  calling syntax:                                                 */
/*                  n = xmt(tp,byteaddr);                           */
/*                                                                  */
/*                  where:  n is the returned value of the          */
/*                               function.                          */
/*                                                                  */
/*                               n = '1' means good return.         */
/*                               n = '0' means no cblock is         */
/*                                       available.  Try again      */
/*                                       later.                     */
/*                                                                  */
/*                  where: tp is the pointer to the current entry   */
/*                               into "ttytab" for this current     */
/*                               device being serviced.             */
/*                                                                  */
/*                  where: byteaddr is the byte address that        */
/*                                  is to be sent.                  */
/*                                                                  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

xmt(tp,byte)

	  register char *byte;
	  register struct tty *tp;
{

          register c;

#ifdef COUNT
       	  profile [100]++;
#endif
/*
          printf("xmt:\n");

	  printf("xmt:byte=");
	  printf("%x\n", byte);

	  printf("xmt:*byte=");
	  printf("%x\n", *byte);
*/
          if (putc(*byte, &tp->v_devq)) {
/*
	       printf("xmt: ret(0)\n");
*/
               return (0);
	  } else {
/*
		  printf("xmt: ret(1)\n");
*/
	          return(1);
          }
}
