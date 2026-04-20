
/*  */
/* @(#)pbsioc.h	5.2 4/19/86 */

/* states for write timeout event */
#define	IGNTIM	-1
#define	IDLETIM  0
#define PENDTIM	 1
#define	RETIME	 2
#define FINTIM	 3

/*
 * Multibus queue entry structure.
 */

struct	mq {
	int	mb_amb;			/* wake-up de-ambiquizer */
	int	*mb_to;			/* move to address */
	int	*mb_from;		/* move from address */
	int	mb_bcnt;		/* number of bytes to move */
	char	mb_ctl;			/* extended map control */
};
/*
 * Multibus main memory request queue.
 */

struct	mbq {
	int	mb_in;			/* next entry available (!full) */
	int	mb_out;			/* next entry to dequeue (!empty) */
	int	mb_cnt;			/* number of entries queued */
	struct	mq	mb_q[MBQSIZE];	/* queue entries */
};

/*
 * SIOC interrupt queue entry structure.
 */

struct sql {
	int	*sq_addr;
	int	sq_len;
};

/*
 * Interrupt command queue structure.
 */

struct sq {
	int	sq_in;
	int	sq_out;
	int	sq_cnt;
	struct	sql sq_e[SQSIZE];
};

struct	ctlunit {
	struct	sctl	*cu_ctl;
	int	cu_unit;
};

/*
 * SIOC interrupt command block structure.
 */

	/*-----------------------------------------------------*\
	| NOTE! that size of si_parm is >= the largest control |
	|       block sent by the main processor to the ICP.    |
	|       This is VERY VERY IMPORTANT! (see sioccomm.h)   |
	\*-----------------------------------------------------*/

struct	scb {
	short	si_cblen;		/* command block length */
	ushort	si_cmd;			/* command */
	union {
		/* gdc1 - unused. historical only */
		struct {
#ifdef m68
			long	si_addr; /* address associated w/ command */
#endif
#ifdef z8000
			int	*si_addr; /* address associated w/ command */
#endif
			ushort	si_len; /* length associated w/ command */
		}gdc1;
		/* gdc2 - wakeup. As from write completion */
		struct {
#ifdef m68
			long	si_chan; /* channel associated w/ command */	
#endif
#ifdef z8000
			char *	si_chan; /* channel associated w/ command */
#endif
#ifdef SEQ
			ushort si_seq;	 /* sequence # of reply to host */
#endif
		}gdc2;
		/* gdc3 - signal */
		struct {
			ushort	si_mpgrp;	/* group to issue signal to */
			ushort	si_msig;	/* signal to be issued */
			ushort	si_unit;	/* unit being signalled */
		}gdc3;
		/* gdc4 - flushed write. Single character I/O */
		struct {
#ifdef z8000
			struct	ctlunit	*si_ctlu;	/* control for flush */
#endif
#ifdef m68
			long si_ctlu; /* control for flush */
#endif
#ifdef SEQ
			ushort si_seq;	 /* sequence # of reply to host */
#endif
		}gdc4;
		struct spr {
#ifdef z8000
			int	si_parm[13];	/* command to process */
#endif
#ifdef m68
#ifdef SEQ
			short si_parm[16];	/* command to process */
#else
			short si_parm[15];	/* command to process */
#endif
#endif
		}gdc5;
	} si_un;
};

struct	scbw {
#ifdef z8000
	int	cbw[IBSIZE];
#endif
#ifdef m68
	short	cbw[IBSIZE];
#endif
};
 
/*
 * SIOC write control structure.
 */

struct	wctl {
	struct	sctl	*wc_sctl;
	char	wc_torun;
	char	wc_wflg;
	char	wc_unit;
	char	wc_wrtout;
	unsigned	wc_li;
	unsigned	wc_wi;
	char	wc_wbuf[WBUFSIZE];
	char	wc_lbuf[WBUFSIZE/2];
	struct	scb	wc_scb;
	struct	writertn	wc_rtn;
};
 
/*
 * Interrupt command queue structure to/from main processor.
 */

struct	pq {
	int	pq_in;			/* next entry available (!full) */
	int	pq_out;			/* next entry out (!empty) */
	int	pq_cnt;			/* numberof entries in queue */
	struct	scb	pq_e[PQSIZE];	/* queue entries */
};
/*
 * Main processor control structure.  One in each SIOC.  Contains control
 * information and queues.
 */

struct	pctl {
	long	pc_iaddr;		/* addr. of input command comm. area */
	long	pc_oaddr;		/* addr. of output command comm. area */
	long	pc_paddr;		/* addr. of putchar command area */
	long	pc_min;			/* minimun memory address over mb */
	long	pc_max;			/* maximum memory address over mb */
	int	pc_iqrun;		/* input q processor running flag */
	int	pc_oqrun;		/* output q processor running flag */
	int	pc_ibad;		/* number of bad processor interrupts */
	int	pc_rbad;		/* num. of bad proc. reset interrupts */
	struct	pq	p_oq;		/* output interrupt queue */
	struct	pq	p_iq;		/* input interrupt queue */
};
/*
 * SIOC control structure.  One for each SIOC.  Contains control information
 * and queues.
 */

	/*-------------------------------------------------------*\
	| IMPORTANT!!! sc_pgrp corresponds to the same offset in  |
	|	       sctl.tp_un as does t_pgrp in tty. This is  |
	|	       a hideous kludge for system 3. See si.c    |
	|	       for the reason in siopen().		  |
	\*-------------------------------------------------------*/

struct sctl {
	int	*sc_iaddr;		/* addr. of input SIOC comm. area */
	int	*sc_oaddr;		/* addr. of output SIOC comm. area */
	int	*sc_paddr;		/* putchar communications area */
	int	*sc_port;		/* port address of SIOC */
	int	sc_oqrun;		/* output queue processor running */
	int	sc_bad;			/* number of bad SIOC interrupts */
	int	sc_state;		/* state of this SIOC */
	unsigned	sc_intseq;	/* SIOC interrupt sequence number */
	unsigned	sc_toseq;	/* SIOC interupt timeout seq. num. */
	char	*sc_pscbbuf;		/* pio scb buffer */
	char	*sc_prtnbuf;		/* pio return buffer */
	char	*sc_pbufs;		/* pio general buffer */
	struct	proc	*sc_pprocp;	/* pio procp */
	int	sc_ppid;		/* pio pid */
	int	sc_pspot;		/* pio spot */
	char	sc_notpok;		/* pio write not ok flag */
	struct sctltp {
	int	sc_tstat;	/* tty status */
	struct proc	*sc_oprocp;	/* open procp for this unit */
	int	sc_opid;	/* open pid for this unit */
	int	sc_ospot;	/* open spot for abort */
	struct proc	*sc_cprocp;	/* close procp for this unit */
	int	sc_cpid;	/* close pid for this unit */
	int	sc_cspot;	/* close spot for abort */
	struct proc	*sc_rprocp;	/* read procp for this unit */
	int	sc_rpid;	/* read pid for this unit */
	int	sc_rspot;	/* read spot for abort */
	struct proc	*sc_wprocp;	/* write procp for this unit */
	int	sc_wpid;	/* write pid for this unit */
	int	sc_wspot;	/* write spot for abort */
#ifdef m68
		short	sc_pafhack;	/* align to match tty.h */
	short	sc_pgrp;	/*>> dup tty entry: process grp name */
#endif
	struct proc	*sc_iprocp;	/* ioctl procp for this unit */
	int	sc_ipid;	/* ioctl pid for this unit */
	int	sc_ispot;	/* ioctl spot for abort */
#ifdef z8000
	short	sc_pgrp;	/*>> dup tty entry: process grp name */
#endif
	char	*sc_buf;	/* current position in saved read buf*/
	int	sc_len;		/* number of characters left in buf */	
	int	sc_error;	/* last error code returned by SIOC */
	char	*sc_ibuf;     /* start of saved ioctl general buffer */
	char	*sc_rbufs;     /* start of saved read general buffer */
	char	*sc_wbufs;    /* start of saved write general buffer */
	int	sc_ibuflen;	/* size of general buffer */
	char	*sc_oscbbuf;	/* open scb buffer */
	char	*sc_cscbbuf;	/* close scbbuf */
	char	*sc_iscbbuf;	/* ioctl scb buffer */
	char	*sc_rscbbuf;	/* read scb buffer */
	char	*sc_wscbbuf;	/* write scb buffer */
	char	*sc_ortnbuf;	/* open return buffer */
	char	*sc_crtnbuf;	/* close return buffer */
	int	sc_lstcl;	/* count of retrys to close(see si.c)*/
	char	*sc_irtnbuf;	/* ioctl return buffer */
	char	*sc_rrtnbuf;	/* read return buffer */
	char	*sc_wrtnbuf;	/* write return buffer */
	int	sc_bspot;	/* spot in write buffer */
	char	sc_timstate;	/* timeout event state */
	char	sc_lflag;	/* copy c_lflag in tty.h used for
					   NOFLSH processing. initially 0 */
/*		char	sc_torun;	/* timeout running flag */
/*		char	sc_mwrt;	/* more writes since timeout start */
/*		char	sc_inwrt;	/* in write for this unit */
	struct	ctlunit	cu_un;	/* unit and ctl pointers */
	} tp_un[10];
	struct sq s_oq;			/* output interrupt command queue */
};
