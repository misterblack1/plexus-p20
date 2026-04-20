/* SID */
/* @(#)fbkup68_is.h	5.1 4/22/86 */

/* (get from SYSIII  RA)
 * Revision Level 1.1
 * Last Delta     7/22/83 15:19:45
 * File Name      fbkup68_is.h
 */

#include "sys/disk.h"

#define	ISLOGVOL	16	/* total number of possible logical vols */
#define	ISUNITS		4	/* number of physical units */
#define	MAXTOCNT	350000	/* number of itt for time out loop */
#define	WUA	0xfff0	/* contents of wake up switches */
#ifdef z8000
#define	WUB	((struct wub_struct *) (WUA << 4))
#define	xmemto8086seg(s)	((s) << 12)	/* convert xmem to 8086 seg */
#define	ISINT		(0x20 << 1)
#else
#define WUB	((struct wub_struct *) ((WUA << 4)|LA_SYSIO))
#define addrto8086seg(s)	(((unsigned)(s) & 0xf0000)>>4)
#define CCBA	(LA_SYSIO)
#define CCB	((struct ccb_struct *) (CCBA))
#define CIBA	(CCBA + sizeof(struct ccb_struct))
#define CIB	((struct cib_struct *) (CIBA))
#define IOPA	(CIBA + sizeof(struct cib_struct))
#define IOP	((struct iop_struct *) (IOPA))
#endif
#define	OP_CLEAR	0x00
#define	OP_START	0x01
#define	OP_RESET	0x02
#define	DF_INIT	0x00
#define	DF_STATUS	0x01
#define	DF_READ	0x04
#define	DF_WRITE	0x06
#define	MOD_STD	0
#define	MOD_NOINT	1
#define	S_OPDONE(x)	(x & 1)
#define	S_UNIT(x)	((x >> 4) & 3)
#define	S_SEEK(x)	((x >> 1) & 1)
#define	S_MEDIACHG(x)	((x >> 2) & 1)
#define	S_HARD(x)	((x >> 6) & 1)
#define	S_SUMMARY(x)	((x >> 7) & 1)
#define	FIXED	0
#define	REMOVABLE	1
#define	UNIT0	0
#define	UNIT1	1
#define	UNIT2	2
#define	UNIT3	3
#define	UNIT(punit)	(punit & 3)
#define	SSTAT	1
#define	SIO	2
#define	DK_N	0

struct	wub_struct {
	char	w_wubr1;
	char	w_soc;
	unsigned short	w_ccboff;
	unsigned short	w_ccbseg;
};

struct	ccb_struct {
	char	c_bsy1;
	char	c_ccw1;
	unsigned short	c_ciboff;
	unsigned short	c_cibseg;
	unsigned short	c_ccbr1;
	char	c_bsy2;
	char	c_ccw2;
	unsigned short	c_cpoff;
	unsigned short	c_cpseg;
	unsigned short	c_cp;
};
#ifdef z8000
struct ccb_struct ccb;
#endif

struct	cib_struct {
	char	c_opstat;
	char	c_cibr1;
	char	c_statsem;
	char	c_cmdsem;
	unsigned short	c_csaoff;
	unsigned short	c_csaseg;
	unsigned short	c_iopoff;
	unsigned short	c_iopseg;
	unsigned short	c_cibr2;
	unsigned short	c_cibr3;
};
#ifdef z8000
struct cib_struct cib;
#endif

struct	iop_struct {
	unsigned short	i_iopr1;
	unsigned short	i_iopr2;
	unsigned short	i_ac1;
	unsigned short	i_ac2;
	unsigned short	i_iopr3;
	char	i_function;
	char	i_unit;
	unsigned short	i_modifier;
	unsigned short	i_cylinder;
	char	i_sector;
	char	i_head;
	unsigned short	i_dboff;
	unsigned short	i_dbseg;
	unsigned short	i_rc1;
	unsigned short	i_rc2;
	unsigned short	i_iopr4;
	unsigned short	i_iopr5;
};
#ifdef z8000
struct iop_struct iop;
#endif

struct initinfo is_init[ISUNITS];

struct diskconf	is_sizes[ISLOGVOL*ISUNITS];

