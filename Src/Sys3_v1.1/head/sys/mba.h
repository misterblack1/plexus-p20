/* VAX Massbus adapter registers */

#ifdef vax
struct mba {
	union {
		struct mba_regs {
			int mba_csr,
			    mba_cr,
			    mba_sr,
			    mba_var,
			    mba_bcr;
		} mba_regs;
		int	fill1[256];
	} mba_ireg;
	union {
		int	fill2[32];
	} mba_ereg[8];
	int	mba_map[256];
};

#define	MBADTABT	0x00001000
#define	MBAEBITS	0x000e0770

#define	MBAIE	0x4
#endif
