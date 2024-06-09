/*
 * pio handling routines
 */

#include "icp/sioc.h"	/* icp specific */
#include "icp/pio.h"	/* icp specific */

#ifdef COUNT
extern int profile[];
#endif
struct  pios pio;

/*
 * initialize the pio interrupt vectors, etc.
 */

pioinit()
{


	/* load interrupt vectors */

	out_local(ppioc[0], 96);
	out_local(ppioc[1], 98);

	/* set up operation modes */

	out_local(ppioc[0], PCTRLMODE | PMODE);	/* unit 0 , control mode */
	out_local(ppioc[0], 0x3e);	/* 7, 6, 0 output. 1-5 input */
	out_local(ppioc[0], PINTEN | PMSK | PICW);
	out_local(ppioc[0], 0xff);	/* don't cause any status interrupts */

	out_local(ppioc[1], POUTMODE | PMODE);	/* unit 1, output mode */
	out_local(ppioc[1], PMSK | PICW);
	out_local(ppioc[1], 0);	/* mask dosn't matter for unit 1 */

	/* the following is magic to the pio, it won't interrupt
	   right without it */

	out_local(PRETI, 0);
	 
	out_local( PCMD, ( cmdsav & ~PIOREQ ) );
	out_local( PCMD, cmdsav );

	/*
	 * input prime is active low for centronics, so initialize it high.
	 * ACTION ITEM 82-016  21/4/82 Fleitman.
	 */

	out_local(ppiod[0], 1);

}


/*
 * pio interrupt handler
 */

pioi(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
{
	register unit;

#ifdef COUNT
	profile[15]++;
#endif

	unit = (id >> 1) & 1;
	if ( unit == 0 ) {
		printf( "pio status: %x\n", (in_local( ppiod[0] ) & 0xff) );
	/*
	} else if ( pio.pb_busy ) {
		x = *pio.pb_buf++ & 0x7f;
		if ( pio.pb_len-- ) {
			out_local( ppiod[1], x );
		} else {
			pio.pb_busy = 0;
			wakeup( (caddr_t) &pio );
		}
	}
	*/
	} else printf( "invalid pio interrupt!!!\n" );
	putreti();
}

	/*----------------------*\
	| start the pio transfer |
	\*----------------------*/

piostart()
{

#ifdef COUNT
	profile[16]++;
#endif

	/*
 	 * Note that channel 10 (channel 2 DMA2) is the pio channel
	 * and channels 0,1 of DMA2 are used for cascading
	 */
	startdma( 10, pio.pb_buf, pio.pb_len );
}
