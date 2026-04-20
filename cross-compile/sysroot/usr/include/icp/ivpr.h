/* 
 * SID @(#)ivpr.h	5.1
 */

	/*----------------------------------------------*\
	| Versatec includes. Mainly definition for ioctl |
	\*----------------------------------------------*/

struct vprio {
	ushort vpr_odata;	/* versatec output paramaters */
};

#define VP_SPP		000001		/* Simultaneous Print Plot */
#define VP_RESET	000002		/* Remote Reset */
#define VP_CLRCOM	000004		/* Remote Clear */
#define VP_EOTCOM	000010		/* Remote End of Transmission */
#define VP_FFCOM	000020		/* Remote Form Feed */
#define VP_TERMCOM	000040		/* Remote Line Terminate */

#define	VPRINT		000100		/* print mode */
#define VPLOT		000200		/* Plot mode */
#define	VPRINTPLOT	000400		/* Print/Plot */

#define VPSC_CMNDS	000076		/* Versatec commands */


#define	VGETSTATE	(('v'<<8)|0)
#define	VSETSTATE	(('v'<<8)|1)
