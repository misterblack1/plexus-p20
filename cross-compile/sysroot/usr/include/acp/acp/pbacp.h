/* SID */
/* @(#)pbacp.h	5.2 9/21/86 */

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
 * ACP interrupt queue entry structure.
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
 * ACP interrupt command block structure.
 */

	/*-----------------------------------------------------*\
	| NOTE! that size of si_parm is >= the largest control  |
	|       block sent by the main processor to the ACP.    |
	|       This is VERY VERY IMPORTANT! (see  acpcomm.h)   |
	\*-----------------------------------------------------*/

struct	scb {
	short	si_cblen;		/* command block length */
	ushort	si_cmd;			/* command */
	union {
		struct {
			long	si_addr; /* address associated w/ command */
			ushort	si_len; /* length associated w/ command */
		}gdc1;
		struct {
			long	si_chan; /* channel associated w/ command */	
#ifdef SEQ
			ushort si_seq;	 /* sequence # of reply to host */
#endif
		}gdc2;
		struct {
			ushort	si_mpgrp;	/* group to issue signal to */
			ushort	si_msig;	/* signal to be issued */
			ushort	si_unit;	/* unit being signalled */
		}gdc3;
		struct {
			long si_ctlu; /* control for flush */
		}gdc4;
		struct spr {
			short si_parm[15];	/* command to process */
/* NEED TO PUT THIS IN	short si_parm[38];	/* command to process */
		}gdc5;
	} si_un;
};

struct	scbw {
	short	cbw[IBSIZE];
};
 
/*
 * ACP write control structure.
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
 * Main processor control structure.  One in each ACP.  Contains control
 * information and queues.
 */

struct	pctl {
	long	pc_iaddr;		/* addr. of input command comm. area */
	long	pc_oaddr;		/* addr. of output command comm. area */
	long	pc_paddr;		/* addr. of putchar command area */
	int	pq_in;			/* next entry available (!full) */
	int	pq_out;			/* next entry out (!empty) */
	int	pq_cnt;			/* number of entries in queue */
	struct	scb *pq_e[PQSIZE];	/* queue entries */
};
/*
 * ACP control structure.  One for each ACP.  Contains control information
 * and queues.
 */

	/*-------------------------------------------------------*\
	| IMPORTANT!!! sc_pgrp corresponds to the same offset in  |
	|	       sctl.tp_un as does t_pgrp in tty.          |
	|	       See si.c for the reason in siopen().	  |
	\*-------------------------------------------------------*/

struct sctl {
	int	*sc_iaddr;		/* addr. of input ACP comm. area */
	int	*sc_oaddr;		/* addr. of output ACP comm. area */
	int	*sc_paddr;		/* putchar communications area */
	int	*sc_port;		/* port address of ACP */
	int	sc_oqrun;		/* output queue processor running */
	int	sc_attn;	/* # of attn's for this card */
	int	sc_irst;	/* # of reset's for this card */
	int	sc_ack;		/* # of ack's for this card */
	int	sc_wup;		/* # of wup's for this card */
	int	sc_bad;			/* number of bad ACP interrupts */
	int	sc_state;		/* state of this ACP */
	unsigned	sc_intseq;	/* ACP interrupt sequence number */
	unsigned	sc_toseq;	/* ACP interupt timeout seq. num. */
	char	*sc_pscbbuf;		/* pio scb buffer */
	char	*sc_prtnbuf;		/* pio return buffer */
	char	*sc_pbufs;		/* pio general buffer */
	struct	proc	*sc_pprocp;	/* pio procp */
	int	sc_ppid;		/* pio pid */
	int	sc_pspot;		/* pio spot */
	char	sc_notpok;		/* pio write not ok flag */
	struct sctltp {
/* 0 1 */	int	sc_tstat;	/* tty status */
/* 2 3 */	struct proc	*sc_oprocp;	/* open procp for this unit */
/* 4 5 */	int	sc_opid;	/* open pid for this unit */
/* 6 7 */	int	sc_ospot;	/* open spot for abort */
/* 8 9 */	struct proc	*sc_cprocp;	/* close procp for this unit */
/* 1011*/	int	sc_cpid;	/* close pid for this unit */
/* 1213*/	int	sc_cspot;	/* close spot for abort */
/* 1415*/	struct proc	*sc_rprocp;	/* read procp for this unit */
/* 1617*/	int	sc_rpid;	/* read pid for this unit */
/* 1819*/	int	sc_rspot;	/* read spot for abort */
/* 2021*/	struct proc	*sc_wprocp;	/* write procp for this unit */
/* 2223*/	int	sc_wpid;	/* write pid for this unit */
/* 2425*/	int	sc_wspot;	/* write spot for abort */
		short	sc_filler;	/* align to match tty.h */
/*     */	short	sc_pgrp;	/*>> dup tty entry: process grp name */
/* 2627*/	struct proc	*sc_iprocp;	/* ioctl procp for this unit */
/* 2829*/	int	sc_ipid;	/* ioctl pid for this unit */
/* 3031*/	int	sc_ispot;	/* ioctl spot for abort */
/* 3435*/	char	*sc_buf;	/* current position in saved read buf*/
/* 3637*/	int	sc_len;		/* number of characters left in buf */	
/* 3839*/	int	sc_error;	/* last error code returned by ACP */
/* 4041*/	char	*sc_ibuf;     /* start of saved ioctl general buffer */
/* 4243*/	char	*sc_rbufs;     /* start of saved read general buffer */
/* 4445*/	char	*sc_wbufs;    /* start of saved write general buffer */
/* 4647*/	int	sc_ibuflen;	/* size of general buffer */
/* 4849*/	char	*sc_oscbbuf;	/* open scb buffer */
/* 5051*/	char	*sc_cscbbuf;	/* close scbbuf */
/* 5253*/	char	*sc_iscbbuf;	/* ioctl scb buffer */
/* 5455*/	char	*sc_rscbbuf;	/* read scb buffer */
/* 5657*/	char	*sc_wscbbuf;	/* write scb buffer */
/* 5859*/	char	*sc_ortnbuf;	/* open return buffer */
/* 6061*/	char	*sc_crtnbuf;	/* close return buffer */
/* 6263*/	int	sc_lstcl;	/* count of retrys to close(see si.c)*/
/* 6465*/	char	*sc_irtnbuf;	/* ioctl return buffer */
/* 6667*/	char	*sc_rrtnbuf;	/* read return buffer */
/* 6869*/	char	*sc_wrtnbuf;	/* write return buffer */
/* 7071*/	int	sc_bspot;	/* spot in write buffer */
/* 72  */	char	sc_timstate;	/* timeout event state */
/* 73  */	char	sc_lflag;	/* copy c_lflag in tty.h used for
					   NOFLSH processing. initially 0 */
/*		char	sc_torun;	/* timeout running flag */
/*		char	sc_mwrt;	/* more writes since timeout start */
/*		char	sc_inwrt;	/* in write for this unit */
/* 7477*/	struct	ctlunit	cu_un;	/* unit and ctl pointers */
	} tp_un[18];
	struct sq s_oq;			/* output interrupt command queue */
};
