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
/*  function: getrbuf                                               */
/*                                                                  */
/*  purpose:                                                        */
/*    Get (open) a receive buffer.  The returned value is zero if   */
/*    a buffer is available, otherwise it is non-zero.  If a buffer */
/*    is obtained, the buffer parameters are copied into the array  */
/*    specified by "name".  The array should be large enough to     */
/*    hold at least three bytes.  The meaning of the buffer         */
/*    parameters is driver-dependent.  If a receive buffer has      */
/*    previously been opened via a "getrbuf" call but has not yet   */
/*    been closed via a call to "rtnrbuf", that buffer is re-       */
/*    initialized and remains the current buffer.                   */
/*                                                                  */
/*  calling syntax:                                                 */
/*                  n = getrbuf(tp,nameaddr);                       */
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

getrbuf(tp,array)

register char *array;
register struct tty *tp;
{
	struct cblock *cp;
	register int i;
	register char *acp;
/*
	printf("getrbuf\n");
*/
	if(tp->v_rcvholdq.c_cc) {
		while((cp=getcb(&tp->v_rcvholdq)) != NULL) {
			putcf(cp);
		}
	}
	if(cfreelist.c_next) {
		for (i=0; i<4; i++) {
			if(putc(' ', &tp->v_rcvholdq)) return (1);
		}
		acp = array;
/*
		printf("acp=%x\n", acp);
		printf("*acp=%x\n", *acp);
		printf("acp+1=%x\n", acp+1);
*/
		*acp++ = tp->v_rbfname[0];
		*acp++ = tp->v_rbfname[1];
		*acp = tp->v_rbfname[2];

		return(0);
	} else {
		return(1);
	}
}
