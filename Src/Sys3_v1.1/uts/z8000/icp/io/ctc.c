/*
 * ctc handling
 */

#include "icp/sioc.h"	/* icp specific */
#include "icp/dma.h"	/* icp specific */
#include "icp/ctc.h"	/* icp specific */

extern int pctc[];

	/*---------------------------*\
	| general ctc initialization. |
	\*---------------------------*/

ctcinit()
{

	/* initialize interrupt vectors */

	out_local(pctc[0], 64);
	out_local(pctc[3], 72);
	out_local(pctc[8], 80);
	out_local(pctc[6], 88);

	/* set up 50 hz real time clock */

	out_local(pctc[9], CTIME | CRESET | CCTRL);
	out_local(pctc[9], 100);
	out_local(pctc[14], CINTEN | CCTR | CTIME | CRESET | CCTRL);
	out_local(pctc[14], 50);

	/* set up 796 bus request clock to .5 ms */

	out_local(pctc[8], CTIME | CRESET | CCTRL);
#ifdef	VPMSYS
	out_local(pctc[8], 125);
#else
	out_local(pctc[8], 250);
#endif

	/* set up dma0, dma1, and dma2 interrupt generators */

	out_local(pctc[10], CINTEN | CCTR | CTIME | CRESET | CCTRL);
	out_local(pctc[10], 1);
	out_local(pctc[11], CINTEN | CCTR | CTIME | CRESET | CCTRL);
	out_local(pctc[11], 1);
	out_local(pctc[15], CINTEN | CCTR | CTIME | CRESET | CCTRL);
	out_local(pctc[15], 1);

	/* set up 796 bus grant and chan attn interrupt generators */

	out_local(pctc[12], CINTEN | CCTR | CUPCLK | CTIME | CRESET | CCTRL);
	out_local(pctc[12], 1);
	/*
	out_local(pctc[13], CINTEN | CCTR | CUPCLK | CTIME | CRESET | CCTRL);
	*/
	out_local(pctc[13], CCTR | CUPCLK | CTIME | CRESET | CCTRL);
	out_local(pctc[13], 1);
}
