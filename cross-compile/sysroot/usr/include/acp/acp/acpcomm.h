/* SID */
/* @(#)acpcomm.h	5.1 4/22/86 */

/*
 * ACP communications structures
 * and constants
 */

	/*-----------------------------------------------------*\
	| NOTE! that size of si_param is >= the largest control |
	|      block sent by the main processor to the ACP.     |
	|      This is VERY VERY IMPORTANT!! (see pbsioc.h)     |
	\*-----------------------------------------------------*/

#define	VRBUFSIZE	520
#define	VWBUFSIZE	520
#define	RBUFSIZE	64
#define	WBUFSIZE	256

#define DOPEN		0
#define	DCLOSE		1
#define DREAD		2
#define	DWRITE		3
#define	DIOCTL		4
#define DCLEAN		5
#define DSIGLOCK	6
#define DNLDOPEN	7
#define DNLDCLOSE	8
#define DNLDREAD	9
#define DNLDWRITE	10
#define DNLDCLEAN	11
#define ACPUSRWUP	12
#define ACPURDRTN	13
#define ACPUWRRTN	14
#define ACPUCTRTN	15
#define DACPINIT	16

/*
 *	open down-load lcb
 */
struct dopenlcb {
	ushort	dlo_dev;
	long	dlo_procp;
	long	dlo_pid;
	ushort	dlo_pgrp;
	long	dlo_ttyp;
	long	dlo_rtnadx;
	long	dlo_wuadx;
	unsigned mag;		/* magic number */
	unsigned tsize;		/* text size */
	unsigned dsize;		/* data size */
	unsigned bsize;		/* bss size */
	unsigned ssize;		/* symbol table size */
	unsigned rtsize;	/* text relocation size */
	unsigned rdsize;	/* data relocation size */
	unsigned entloc;	/* entry location */
	char	dlo_name[14];	/* name of user process */
};

struct openlcb {
	ushort	lo_dev;
	ushort	lo_spdidx;
	ushort	lo_uid;
	long lo_procp;
	long lo_pid;
	ushort	lo_pgrp;
	ushort	lo_flag;
	long lo_ttyp;
	long	lo_rtnadx;
	long lo_wuadx;
};

struct openrtn {
	ushort	ro_gate;
	ushort	ro_error;
	ushort	ro_pgrp;
};

struct closelcb {
	ushort	lc_dev;
	long lc_procp;
	long lc_pid;
	long	lc_rtnadx;
	long lc_wuadx;
#ifdef VPMICP
	ushort	lc_vpmhalt		/* VPM specific */
#endif
};

struct closertn {
	ushort	rc_gate;
	ushort	rc_error;
};

struct readlcb {
	ushort	lr_dev;
	long lr_procp;
	long lr_pid;
	short	lr_fmode;
	short	lr_flag;
	long	lr_bufadx;
	long	lr_rtnadx;
	long lr_wuadx;
#ifdef VPMICP
        char    lr_rbufin[4];		/* VPM specific */
#endif
};

struct readrtn {
	ushort	rr_gate;
	ushort	rr_error;
	ushort	rr_count;
};

struct writelcb {
	ushort	lw_dev;
	long lw_procp;
	long lw_pid;
	long	lw_bufadx;
	short	lw_count;
	long	lw_rtnadx;
	long	lw_wuadx;
	long	lw_ctlu;
	short	lw_flag;
#ifdef SEQ
	ushort	lw_seq;
#endif
};

struct writertn {
	ushort	rw_gate;
	ushort	rw_error;
#ifdef SEQ
	ushort	rw_seq;
#endif
};

struct ioctllcb {
	ushort	li_dev;
	long li_procp;
	long li_pid;
	ushort	li_com;		/* ioctl command */
	ushort	li_arg;		/* ioctl argument (if integer) */
	long	li_parmadx;
	long	li_rtnadx;
	long li_wuadx;
};

struct ioctlrtn {
	ushort	ri_gate;
	ushort	ri_error;
};

struct	cleanlcb {
	ushort	clr_dev;
	long clr_procp;
	long clr_pid;
	long clr_wuadx;
};
