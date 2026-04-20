/* SID @(#)mba.h	5.1 */
/* @(#)mba.h	6.1 */
/* VAX Massbus adapter registers */

struct mba {
	union {
		struct mba_regs {
			int csr,
			    cr,
			    sr,
			    var,
			    bcr;
		} regs;
		int	fill1[256];
	} ireg;
	union {
		int	fill2[32];
	} ereg[8];
	int	map[256];
};

#define	mbacsr	ireg.regs.csr
#define	mbacr	ireg.regs.cr
#define	mbasr	ireg.regs.sr
#define	mbavar	ireg.regs.var
#define	mbabcr	ireg.regs.bcr

#define	MBADTABT	0x00001000
#define	MBAEBITS	0x000e0770
#define	MBATTN	0x10000

#define	MBAIE	0x4

struct mbacf {
	int	nexus;
	int	sbipri;
	int *	devptr;
};
