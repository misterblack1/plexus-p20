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
/*  function: put                                                   */
/*                                                                  */
/*  purpose: The VPM protocol script issues this function call      */
/*           in order to insert a byte into the current receive     */
/*           buffer.  The return value is zero if a byte was        */
/*           transferred, otherwise it is non-zero.                 */
/*                                                                  */
/*  calling syntax:                                                 */
/*                  n = put(tp,byteaddr);                           */
/*                                                                  */
/*                  where: n is the return value of "0" or "1".     */
/*                              "0" means a byte was transferred.   */
/*                              "1" means a byte was not put.       */
/*                                                                  */
/*                  where: tp is the pointer to the current entry   */
/*                               into "ttytab" for this current     */
/*                               device being serviced.             */
/*                                                                  */
/*                  where: byteaddr is the pointer to where the     */
/*                                  byte resides that is to be put  */
/*                                  into the receive buffer.        */
/*                                                                  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

put(tp,byte)

	  register char *byte;
	  register struct tty *tp;
{

#ifdef COUNT
	profile [106]++;
#endif

	  if (putc(*byte, &tp->v_rcvholdq)) return (1);
	  return(0);
}
