/* SID */
/* @(#)trap.h	5.1 4/22/86 */

/*
 * Trap type values
 */

#define BUSERR		0	/* bus error*/
#define ADDERR		1	/* address error */
#define ILLINST		2	/* illegal instruction */
#define ZERODIV		3	/* zero divide */
#define CHKINST		4	/* CHK instruction */
#define TRAPVINST 	5	/* TRAPV instruction */
#define PRIVVIO		6	/* privilege violation */
#define TRACETRAP 	7	/* trace */
#define SYSCALL		8	/* system call*/
#define T1010		9	/* unimp inst 1010 */
#define T1111		10	/* unimp inst 1111 */
#define TRESERVED 	11	/* reserved trap vector */
#define TUNINIT		12	/* uninitialized trap vector */
#define SPURIOUS 	13	/* spurious interrupt */
#define IBAD		14	/* unused interrupt */
#define URESERVED 	15	/* undefined user interrupt */
#define DMERR		16	/* dma/mem err interrupt */
#define PWR		17	/* powerfail/switch interrupt */
#define TBAD		18	/* unused trap instruction vector */
#define ILLINT		19	/* multibus int on unused vector */
#define ISWITCH 	20	/* interrupt switch */
#define DBG		21	/* fake for debug call */
#define RESCHED 	22	/* reschedule trap */
#define BKPT		23	/* user breakpoint trap */

#define	LEVEL3		24	/* level 3 interrupt */
#define	LEVEL4		25	/* level 4 interrupt */
#define	LEVEL5		26	/* level 5 interrupt */
#define	LEVEL6		27	/* level 6 interrupt */
#define	LEVEL7		28	/* level 7 interrupt */

#define IESC		29	/* SIO - external status change */
#define IRCA		30	/* SIO - receive character available */
#define ITBE		31	/* SIO - transmit buffer empty */
#define ISRC		32	/* SIO - special receive condition */
