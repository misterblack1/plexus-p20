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
/*  function: rcv                                                   */
/*                                                                  */
/*  purpose: The VPM protocol script issues this function call      */
/*           in order to obtain a byte from the raw input queue.    */
/*                                                                  */
/*  calling syntax:                                                 */
/*                  n = rcv(tp,byteaddr);                           */
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
/*                                  byte gotten from the receive    */
/*                                  queue will be placed.           */
/*                                                                  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

rcv(tp,byte)

	register char *byte;
	register struct tty *tp;
{
	register char c;

#ifdef COUNT
	profile [105]++;
#endif
/*
	printf("rcv:t_rawq.c_cc=%x\n", tp->t_rawq.c_cc);
*/
	spl4();
	if (tp->t_rawq.c_cc == 0) {

		spl0();
		return(0);
	}
	spl0();

	c = getc(&tp->t_rawq);

	if(tp->t_lflag & SYNC) {
/*
		printf("RCV:c=%x", c);
*/
		*byte = c;
		return(1);
	}
	if(c >= 0) {
		*byte = c;
		return(1);
	}
	return(0);
}
