/*
 * Trap type values
 */

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
#else
#define	UNIMPL	0	/* unimplemented instruction */
#define	BPTFLT	1	/* single step */
#define	PRIVFLT	2	/* privileged instruction fault */
#define	SYSCALL	3	/* system call instruction */
#define	NVINT	4	/* non-vectored interrupt */
#define	SEGFLT	5	/* segmentation fault */
#define	NMINT	6	/* non-maskable interrupt */
#define	RESCHED	7	/* software level 1 trap (reschedule trap) */
#define	ILLINT	8	/* illegal interrupt */
#define	BUSTO	9	/* multibus timeout */
#define MRERR	10	/* multibus mem request error */
#define ARTHTRP	11	/* floating point exception */
#define DBG	12	/* debug interrupt */
#endif
