/* SID @(#)mc146818.h	5.1 */
/* Motorola Real-Time Clock plus RAM chip - MC146818 */

/* Register A defines */
#define B_CUIP		0x80		/* update-in-progress bit mask */
#define B_CDV2		0x40		/* time base freq dividers */
#define B_CDV1		0x20
#define B_CDV0		0x10
#define B_CRS3		0x8		/* rate selection bits */
#define B_CRS2		0x4
#define B_CRS1		0x2
#define B_CRS0		0x1

/* Register B defines */
#define B_CSET		0x80		/* inhibit 'update' cycles */
#define B_CPIE		0x40		/* periodic interrupt enable */
#define B_CAIE		0x20		/* alarm interrupt enable */
#define B_CUIE		0x10		/* update interrupt enanble */
#define B_CSQWE		0x8		/* square wave output enable */
#define B_CDM		0x4		/* data mode: 1=bin 0=bcd */
#define B_C2412		0x2		/* hours fmt: 1=24hr 0=12hr */
#define B_CDSE		0x1		/* enable daylight savings */

/* Register C defines */
#define B_CIRQF		0x80		/* interrupt req flag */
#define B_CPF		0x40		/* periodic int flag */
#define B_CAF		0x20		/* alarm int flag */
#define B_CUF		0x10		/* update int flag */

/*Register D defines */
#define B_CVRT		0x80		/* valid ram and time */

