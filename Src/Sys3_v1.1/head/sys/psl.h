/*
 *  	processor status
 */

#define	PS_C	0x1		/* carry bit */
#define	PS_V	0x2		/* overflow bit */
#define	PS_Z	0x4		/* zero bit */
#define	PS_N	0x8		/* negative bit */
#define	PS_T	0x10		/* trace enable bit */
#ifdef vax
#define	PS_IV	0x20		/* integer overflow enable bit */
#define	PS_FU	0x40		/* floating point underflow enable bit */
#define	PS_DV	0x80		/* decimal overflow enable bit */
#define	PS_IPL	0x1f0000	/* interrupt priority level */
#define	PS_PRV	0xc00000	/* previous mode */
#define	PS_CUR	0x3000000	/* current mode */
#define	PS_IS	0x4000000	/* interrupt stack */
#define	PS_FPD	0x8000000	/* first part done */
#define	PS_TP	0x40000000	/* trace pending */
#define	PS_CM	0x80000000	/* compatibility mode */
#else
#define	PS_IPL	0340		/* interrupt priority level */
#define	PS_PRV	0030000		/* previous mode */
#define	PS_CUR	0140000		/* current mode */
#endif
