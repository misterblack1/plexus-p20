/*
 * This is the initial code run to bring up the unix kernal
 */

#include "sys/plexus.h"

/* externals */
.globl	_main
.globl	psa
.globl	_phypage

.globl	start
start:				/* start here **in segmented mode** */

ld	r1, r14($4)		/* get next available physical page number */

ld	r0, $(B_SYS | B_NVI)
ldctl	fcw, r0			/* turn off interrupts and seg mode */

ld	_phypage,r1		/* save next avail phy page num in global */
ld	r1, $ECCON		/* set up ecc */
sout	P_ECC, r1

ld	r1, $B_RESETOFF | B_ALLOWRESET | B_ECCOFF
sout	P_PIOADAT, r1
ld	r1, $PIOACTL
sout	P_PIOACTL, r1
ld	r1, $PIOAWRT
sout	P_PIOACTL, r1
ld	r1, $0xff
sout	P_PIOBDAT, r1
ld	r1, $PIOBCTL
sout	P_PIOBCTL, r1
ld	r1, $PIOBWRT
sout	P_PIOBCTL, r1

sout	P_MRINT, r1		/* clear all possible interrupt inputs */
sout	P_CLKINT, r1
sout	P_SSINT, r1
sout	P_MBTOINT, r1

ld	r1, $LA_UBLK		/* clear out user block */
ld	r2, $(LA_UBLK + 2)
ld	r3, $((PAGESIZE >> 1) - 1)
clr	*r1
ldir	*r2, *r1, r3

ld	r1, $LA_SYSIO		/* clear out system i/o page */
ld	r2, $(LA_SYSIO + 2)
ld	r3, $((PAGESIZE >> 1) - 1)
clr	*r1
ldir	*r2, *r1, r3

ld	r0, $(B_SEG | B_SYS | B_NVI)	/* set up the system stack pointer */
ldctl	fcw, r0
ld	r14, $SDSEG
ld	r15, $(LA_UBLK + PAGESIZE)
ld	r0, $(B_SYS | B_NVI)
ldctl	fcw, r0

ld	r1, $psa		/* set up psap pointer */
ldctl	psapoff, r1
ld	r1, $SISEG
ldctl	psapseg, r1

ld	r1, $MICW1	/* set up master 8259 interrupt controller */
sout	P_MPIC0, r1
ld	r1, $MICW2
sout	P_MPIC1, r1
ld	r1, $MICW3
sout	P_MPIC1, r1
ld	r1, $MICW4
sout	P_MPIC1, r1

ld	r1, $SICW1	/* set up slave 8259 interrupt controller */
sout	P_SPIC0, r1
ld	r1, $SICW2
sout	P_SPIC1, r1
ld	r1, $SICW3
sout	P_SPIC1, r1
ld	r1, $SICW4
sout	P_SPIC1, r1

ld	r1, $MPICMSK	/* set up initial interrupt masks */
sout	P_MPIC1, r1
ld	r1, $SPICMSK
sout	P_SPIC1, r1

ld	r1, $READIR	/* set slave pic to read ir status */
sout	P_SPIC0, r1

sin	r1,P_MUBLK	/* get user page for process table setup in main */
and	r1,$M_PPN	/* only look at significant bits */
ld	*r15,r1		/* put it on the stack for main */

ei	vi		/* enable interrupts */

call	_main		/* do the rest of the initialization */

mset			/* turn on memory error checking */
sout	P_MRINT, r0
sin	r0, P_PIOADAT
and	r0, $~B_ECCOFF
sout	P_PIOADAT, r0

ld	r0, $0
ldctl	nspoff, r0	/* set up user stack pointer */
push	*r15, $0	/* set up users pc offset */
push	*r15, _udseg	/* set up users pc seg */
push	*r15, $(B_VI + B_NVI)	/* set up users fcw */
push	*r15, $0	/* set identifier just for iret */
	
ld	r0, $(B_SEG | B_SYS | B_NVI)
ldctl	fcw, r0
iret			/* go to user */

