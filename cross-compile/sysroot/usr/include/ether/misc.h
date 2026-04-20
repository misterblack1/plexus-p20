/*  */
/* SID @(#)misc.h	5.1 */
/* @(#)misc.h	1.4 2/3/84 */




	/* miscellaneous defines */

#define NO	0
#define YES	1

#define NULL_ADD 0	/* ethernet null address entry */

#define ETIPRI	20	/* priority for ethernet sleep */

#define LOBYTE(x) (x&0xff)	/* mask for extracting low byte of word */
#define HIBYTE(x) ((x>>8)&0xff)	/* mask for extracting hi byte of word */
#define SWAB(x) (((x)&0xff)<<8 | (((x)>>8)&0xff)) /* swap bytes in integer */

int netup;
