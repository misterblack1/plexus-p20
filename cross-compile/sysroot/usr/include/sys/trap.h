/* SID @(#)trap.h	5.1 */
/* @(#)trap.h	6.1 */
/*
 * Trap type values
 */

#ifdef m68
#define BUSERR	0	/* bus error*/
#define ADDERR	1	/* address error */
#define ILLINST	2	/* illegal instruction */
#define ZERODIV	3	/* zero divide */
#define CHKINST	4	/* CHK instruction */
#define TRAPVINST 5	/* TRAPV instruction */
#define PRIVVIO	6	/* privilege violation */
#define TRACETRAP 7	/* trace */
#define SYSCALL	8	/* system call*/
#define T1010	9	/* unimp inst 1010 */
#define T1111	10	/* unimp inst 1111 */
#define TRESERVED 11	/* reserved trap vector */
#define TUNINIT	12	/* uninitialized trap vector */
#define SPURIOUS 13	/* spurious interrupt */
#define IBAD	14	/* unused interrupt */
#define URESERVED 15	/* undefined user interrupt */
#define DMERR	16	/* dma/mem err interrupt */
#define PWR	17	/* powerfail/switch interrupt */
#define TBAD	18	/* unused trap instruction vector */
#define ILLINT	19	/* multibus int on unused vector */
#define ISWITCH 20	/* interrupt switch */
#define DBG	21	/* fake for debug call */
#define RESCHED 22	/* reschedule trap */
#define BKPT	23	/* user breakpoint trap */
#ifdef robin
#define	MBERR	24	/* multibus bus error */
#endif
#endif
#ifdef vax
#define	RSADFLT	0	/* reserved addressing fault */
#define	PRIVFLT	1	/* privileged instruction fault */
#define	BPTFLT	2	/* bpt instruction fault */
#define	XFCFLT	3	/* xfc instruction fault */
#define	RSOPFLT	4	/* reserved operand fault */
#define	SYSCALL	5	/* chmk instruction (syscall trap) */
#define	ARTHTRP	6	/* arithmetic trap */
#define	RESCHED	7	/* software level 1 trap (reschedule trap) */
#define	SEGFLT	8	/* segmentation fault */
#define	PROTFLT	9	/* protection fault */
#define	TRCTRAP	10	/* trace trap */
#define	CMPTFLT	11	/* compatibility mode fault */
#endif
