
/*  */
/* @(#)sioccomm.h	5.3 8/8/86 */

/*
 * sioc communications structures
 * and constants
 */

	/*-----------------------------------------------------*\
	| NOTE! that size of si_param is >= the largest control |
	|      block sent by the main processor to the ICP.     |
	|      This is VERY VERY IMPORTANT!! (see pbsioc.h)     |
	\*-----------------------------------------------------*/

#define	VRBUFSIZE	520
#define	VWBUFSIZE	520
#define	RBUFSIZE	64
#define	WBUFSIZE	256

#define DOPEN 0
#define	DCLOSE 1
#define DREAD 2
#define	DWRITE 3
#define	DIOCTL 4
#define DCLEAN 5
#define DSIGLOCK 6

struct openlcb {
	ushort	lo_dev;
	ushort	lo_spdidx;
	ushort	lo_uid;
#ifdef z8000
	struct	proc	*lo_procp;
	int	lo_pid;
#endif
#ifdef m68
	long lo_procp;
	long lo_pid;
#endif
	ushort	lo_pgrp;
	ushort	lo_flag;
#ifdef z8000
	struct	tty	*lo_ttyp;
#endif
#ifdef m68
	long lo_ttyp;
#endif
	long	lo_rtnadx;
#ifdef z8000
	int	lo_wuadx;
#endif
#ifdef m68
	long lo_wuadx;
#endif
#ifdef SEQ
	ushort	lo_seq;
#endif
};

struct openrtn {
	ushort	ro_gate;
	ushort	ro_error;
	ushort	ro_pgrp;
#ifdef SEQ
	ushort	ro_seq;
#endif
};

struct closelcb {
	ushort	lc_dev;
#ifdef z8000
	struct	proc	*lc_procp;
	int	lc_pid;
#endif
#ifdef m68
	long lc_procp;
	long lc_pid;
#endif
	long	lc_rtnadx;
#ifdef z8000
	int	lc_wuadx;
#endif
#ifdef m68
	long lc_wuadx;
#endif
#ifdef SEQ
	ushort	lc_seq;
#endif
	ushort	lc_vpmhalt;		/* VPM specific */
};

struct closertn {
	ushort	rc_gate;
	ushort	rc_error;
#ifdef SEQ
	ushort	rc_seq;
#endif
};

struct readlcb {
	ushort	lr_dev;
#ifdef z8000
	struct	proc	*lr_procp;
	int	lr_pid;
#endif
#ifdef m68
	long lr_procp;
	long lr_pid;
#endif
	short	lr_fmode;
	short	lr_flag;
	long	lr_bufadx;
	long	lr_rtnadx;
#ifdef z8000
	int	lr_wuadx;
#endif
#ifdef m68
	long lr_wuadx;
#endif
#ifdef SEQ
	ushort	lr_seq;
#endif
        char    lr_rbufin[4];		/* VPM specific */
};

struct readrtn {
	ushort	rr_gate;
	ushort	rr_error;
	ushort	rr_count;
#ifdef SEQ
	ushort	rr_seq;
#endif
};

struct writelcb {
	ushort	lw_dev;
#ifdef z8000
	struct	proc	*lw_procp;
	int	lw_pid;
#endif
#ifdef m68
	long lw_procp;
	long lw_pid;
#endif
	long	lw_bufadx;
	short	lw_count;
	long	lw_rtnadx;
#ifdef z8000
	ushort	lw_wuadx;
	ushort	lw_ctlu;
#endif
#ifdef m68
	long lw_wuadx;
	long lw_ctlu;
#endif
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
#ifdef z8000
	struct	proc	*li_procp;
	int	li_pid;
#endif
#ifdef m68
	long li_procp;
	long li_pid;
#endif
	ushort	li_com;		/* ioctl command */
	ushort	li_arg;		/* ioctl argument (if integer) */
	long	li_parmadx;
	long	li_rtnadx;
#ifdef z8000
	ushort	li_wuadx;
#endif
#ifdef m68
	long li_wuadx;
#endif
#ifdef SEQ
	ushort	li_seq;
#endif
};

struct ioctlrtn {
	ushort	ri_gate;
	ushort	ri_error;
#ifdef SEQ
	ushort	ri_seq;
#endif
};

struct	cleanlcb {
	ushort	clr_dev;
#ifdef z8000
	struct	proc	*clr_procp;
	int	clr_pid;
	int	clr_wuadx;
#endif
#ifdef m68
	long clr_procp;
	long clr_pid;
	long clr_wuadx;
#endif
#ifdef SEQ
	ushort	clr_seq;
#endif
};
