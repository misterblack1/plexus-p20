#include "sys/disk.h"

#define	ISLOGVOL	16	/* total number of possible logical vols */
#define	ISUNITS		4	/* number of physical units */
#define	MAXTOCNT	350000	/* number of itt for time out loop */
#define	WUA	0xfff0	/* contents of wake up switches */
#define	WUB	((struct wub_struct *) (WUA << 4))
#define	xmemto8086seg(s)	((s) << 12)	/* convert xmem to 8086 seg */
#define	ISINT		(0x20 << 1)
#define	OP_CLEAR	0x00
#define	OP_START	0x01
#define	OP_RESET	0x02
#define	F_INIT	0x00
#define	F_STATUS	0x01
#define	F_READ	0x04
#define	F_WRITE	0x06
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
	unsigned	w_ccboff;
	unsigned	w_ccbseg;
};

struct	ccb_struct {
	char	c_bsy1;
	char	c_ccw1;
	unsigned	c_ciboff;
	unsigned	c_cibseg;
	unsigned	c_ccbr1;
	char	c_bsy2;
	char	c_ccw2;
	unsigned	c_cpoff;
	unsigned	c_cpseg;
	unsigned	c_cp;
} ccb;

struct	cib_struct {
	char	c_opstat;
	char	c_cibr1;
	char	c_statsem;
	char	c_cmdsem;
	unsigned	c_csaoff;
	unsigned	c_csaseg;
	unsigned	c_iopoff;
	unsigned	c_iopseg;
	unsigned	c_cibr2;
	unsigned	c_cibr3;
} cib;

struct	iop_struct {
	unsigned	i_iopr1;
	unsigned	i_iopr2;
	unsigned	i_ac1;
	unsigned	i_ac2;
	unsigned	i_iopr3;
	char	i_function;
	char	i_unit;
	unsigned	i_modifier;
	unsigned	i_cylinder;
	char	i_sector;
	char	i_head;
	unsigned	i_dboff;
	unsigned	i_dbseg;
	unsigned	i_rc1;
	unsigned	i_rc2;
	unsigned	i_iopr4;
	unsigned	i_iopr5;
} iop;

struct initinfo is_init[ISUNITS];

struct diskconf	is_sizes[ISLOGVOL*ISUNITS];

