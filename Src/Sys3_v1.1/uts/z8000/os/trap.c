#include "sys/plexus.h"
#include "sys/param.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/reg.h"
#include "sys/psl.h"
#include "sys/trap.h"
#include "sys/seg.h"
#include "sys/sysinfo.h"
#include "sys/ecc.h"
#include "sys/utsname.h"

#define	USER	040		/* user-mode flag added to type */
#ifndef	PNETDFS
#define	NSYSENT	64
#else
#define NSYSENT 66
#endif
#define SYS	0x7f00		/* system call instruction */

/*
 * Offsets of the user's registers relative to
 * the saved r0. See reg.h
 */
char	regloc[] =
{
	R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, R15
};

/*
 * Called from trap.s when a processor trap occurs.
 * The arguments are the words saved on the system stack
 * by the hardware and software during the trap processing.
 * Their order is dictated by the hardware and the details
 * of C's calling sequence. They are peculiar in that
 * this call is not 'by value' and changed user registers
 * get copied back on return.
 * type is the kind of trap that occurred.
 *
 * Several conditions that enter the system as interrupts are turned
 * into what look like traps by `trap.s'. This is so that the handling 
 * of non i/o device generated processor interruptions can be handled
 * by one set of code.
 */
trap(type, r15, r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pcseg, pc)
int *pc;
dev_t type;
{
	register unsigned i;
	time_t syst;
	int *savear0;
	unsigned mr0, mr1, mr2, mr3;
	int osp;

	syst = u.u_stime;
	savear0 = u.u_ar0;
	u.u_ar0 = &r0;
	if ((fcw & B_SYS) == 0)
		type |= USER;	/* indicate that trap occurred in user mode */
	switch(minor(type)) {

	/*
	 * Trap not expected.
	 */
	default:
		printf("%s: illegal trap: %x, %x, %x, %x\n",
			utsname.sysname, id & 0xff, fcw, pcseg, pc); 
		panic("trap");

	/*
	 * floating point exception
	 */
	case ARTHTRP+USER:
		psignal(u.u_procp, SIGFPE);
		u.u_ar0 = savear0;
		return;
		break;
		
	/*
	 * unimplemented instruction
	 */
	case UNIMPL+USER:
		i = SIGILL;
		break;

	/*
	 * single step
	 */
	case BPTFLT:
		psignal(u.u_procp, SIGTRAP);
		u.u_ar0 = savear0;
		return;

	case BPTFLT+USER:
		i = SIGTRAP;
		break;

	/*
	 * privileged instruction
	 */
	case PRIVFLT+USER:
		i = SIGIOT;
		break;

	/*
	 * system call instruction
	 */
	case SYSCALL+USER:	/* sys call */
	{
		register *a;
		register struct sysent *callp;
		int (*fetch)();

		sysinfo.syscall++;
		u.u_error = 0;
		fcw &= ~B_CARRY;	/* clear error indicator */
		a = pc;
		i = fuiword((caddr_t)(a-1))&0377;
		if (i==(BPTINST&0377)) {
			i = SIGTRAP;
			break;
		}
		if(i >= NSYSENT)
			callp = &sysent[0];
		else if (i==0) {	/* indirect */
			a = (int *)fuiword((caddr_t)(a));
			pc++;
			i = fuword((caddr_t)a);
			a++;
			if ((i & ~0377) != SYS)
				i = 0;	/* illegal */
			i &= 0377;
			if (i >= NSYSENT)
				i = 0;
			callp = &sysent[i];
			fetch = fuword;
		} else {
			callp = &sysent[i];
			pc += callp->sy_narg - callp->sy_nrarg;
			fetch = fuiword;
		}
		for (i=0; i<callp->sy_nrarg; i++)
			u.u_arg[i] = u.u_ar0[regloc[i]];
		for(; i<callp->sy_narg; i++)
			u.u_arg[i] = (*fetch)((caddr_t)a++);
		u.u_dirp = (caddr_t)u.u_arg[0];
		u.u_rval1 = u.u_ar0[R0];
		u.u_rval2 = u.u_ar0[R1];
		u.u_ap = u.u_arg;
		if (setjmp(u.u_qsav)) {
			if (u.u_abreq)
				(*u.u_abreq)();
			if (u.u_error==0)
				u.u_error = EINTR;
		} else {
			(*callp->sy_call)();
		}
		if (u.u_error) {
			u.u_ar0[R0] = u.u_error;
			fcw |= B_CARRY;		/* carry bit */
			if (++u.u_errcnt > 16) {
				u.u_errcnt = 0;
				runrun++;
			}
		} else {
			u.u_ar0[R0] = u.u_rval1;
			u.u_ar0[R1] = u.u_rval2;
		}
	}
	{
		register struct proc *pp;

		pp = u.u_procp;
		pp->p_pri = (pp->p_cpu>>1) + PUSER + pp->p_nice - NZERO;
		curpri = pp->p_pri;
		if (runrun == 0)
			goto out;
	}

	case RESCHED + USER:	/* Allow process switch */
		sysinfo.preempt++;
		qswtch();
		goto out;

	/*
	 * segmentation trap
	 */
	case SEGFLT:		/* segmentation exception */
	case SEGFLT + USER:
		mr0 = in_local(P_MRE0);
		mr3 = in_local(P_MRE3);
		if ((mr0 & (B_MBERR | B_ECCERR | B_MECCERR | B_PFLT)) ==
		    (B_MBERR | B_ECCERR)) {
			logeccerr();
			out_local(P_MRINT, 0);
			u.u_ar0 = savear0;
			return;
		} else if ((mr0 & (B_MBERR | B_ECCERR | B_MECCERR | B_PFLT)) ==
		    (B_MBERR | B_ECCERR | B_MECCERR)) {
			prtecc();
			panic("memory");
/* CCF out for now 11/01/82 
		} else if (type & USER) {
*/
		} else if (mr0 & (1<<6)) {	/* user mode */
			out_local(P_MRINT, 0);
			if (mr0 & B_PFLT) {
				pc = (int *) mr3;  /* back up pc */
				osp = r15;
				if (backup() == 0) {
					if (grow((unsigned) osp)) {
						goto out;
					}
				}
				i = SIGSEGV;
			} else {
				i = SIGBUS;
			}
			break;
		} else {
			prtecc();
			panic("memory");
		}
		break;

	/*
	 * multibus generated memory request error.
	 */
	case MRERR:
	case MRERR+USER:
		mr0 = in_local(P_MRE0);
		if ((mr0 & (B_Z8KERR | B_ECCERR | B_MECCERR | B_PFLT)) ==
		    (B_Z8KERR | B_ECCERR)) {
			logeccerr();
			out_local(P_MRINT, 0);
			u.u_ar0 = savear0;
			return;
		} else {
			prtecc();
			panic("mb memory");
		}
		break;

	/*
	 * non maskable interrupt
	 */
	case NMINT:
	case NMINT+USER:
		printf("%s: power fail\n", utsname.sysname);
		out_local(P_PIOADAT, (~B_RESETOFF) | B_ALLOWRESET);
		debug();
		break;

	/*
	 * multibus timeout
	 */
	case BUSTO:
	case BUSTO+USER:
		if (initstate)
			mbusto++;
		else
			printf("%s: multibus i/o timeout\n", utsname.sysname);
		u.u_ar0 = savear0;
		return;
		break;

	/*
	 * illegal interrupts
	 */
	case ILLINT:
	case ILLINT+USER:
		logstray(id);
		printf("%s: stray interrupt: %x\n", utsname.sysname, id & 0xff);
		u.u_ar0 = savear0;
		return;
	}
	psignal(u.u_procp, i);

out:
	if(issig())
		psig();
	if(u.u_prof.pr_scale)
		addupc((caddr_t)pc, &u.u_prof, (int)(u.u_stime-syst));
}

/*
 * nonexistent system call-- signal bad system call.
 */
nosys()
{
	psignal(u.u_procp, SIGSYS);
}

/*
 * Ignored system call
 */
nullsys()
{
}

/*
 * prints memory error status on console
 */

prtecc()
{
	register mr0;
	register mr1;
	register mr2;
	register mr3;
	
	mr0 = in_local(P_MRE0);
	mr1 = in_local(P_MRE1);
	mr2 = in_local(P_MRE2);
	mr3 = in_local(P_MRE3);
	printf("%s: memory error: %x, %x, %x, %x, %x\n",
		utsname.sysname, mr0, mr1, mr2, mr3,
		M_PPN & in_local(mapport(MAPTOSEG(mr2 & 0x1f),
				 (mr1 >> S_PAGENUM) & M_PAGENUM)));
}


/*
 * log a single bit ecc error 
 */

struct ecc ecc[NUMECC];
int eccinx;
int ecccnt;
int eccstate;

logeccerr()
{
	register mr0;
	register mr1;
	register mr2;
	register ppn;

	if (eccstate == ECCLOGOFF)
		return;
	if ((++ecccnt) > ECCTHRESH) {
		mres();		/* Turn off single bit errors */
		printf("%s: ecc error logging turned off\n", utsname.sysname);
		prtecc();	/* Print error message anyway */
		ecc[eccinx].e_syndrome = ECCLOGOFF;
		ecc[eccinx].e_bank = 0;
		eccstate = ECCLOGOFF;
	} else {
		mr0 = in_local(P_MRE0);
		mr1 = in_local(P_MRE1);
		mr2 = in_local(P_MRE2);
		ecc[eccinx].e_syndrome = (mr0 >> S_SYNDROME) & M_SYNDROME;
		ppn = M_PPN & in_local(mapport(MAPTOSEG(mr2 & 0x1f),
				       (mr1 >> S_PAGENUM) & M_PAGENUM));
		ecc[eccinx].e_bank = ppn >> 4;
	}
	ecc[eccinx].e_time = time;
	logecc(ecc[eccinx].e_syndrome,ecc[eccinx].e_bank);
	eccinx = (eccinx + 1) % NUMECC;
}


/*
 * Return 0 if the current instruction is restartable else 1. 
 * Note that the users pc has already been moved back to
 * the address of the first word of the instruction that caused
 * the trap.
 */

backup()
{
	
	return(1);  /* be pessimistic for now */
}
