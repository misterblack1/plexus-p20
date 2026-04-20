/*  */
/* SID @(#)misc.h	5.1 */
/* @(#)misc.h	1.6 1/2/84 */

#define BIT45678 0xf8
#define BIT34678 0xec
#define BIT12345 0x1f
#define BIT678	 0xe0
#define BIT234	 0x0e
#define BIT123   0x07
#define BIT34	 0x0c
#define BIT8     0x80
#define BIT5	 0x10
#define BIT2	 0x02
#define BIT1	 0x01

#define BRR	 0
#define BRNR	 4
#define BREJ	 8
#define BDM	 0x0c
#define BSABM	 0x2c
#define BDISC	 0x40
#define BUA	 0x60

#define A	 3
#define B	 1

#define RESP     1
#define COMM     0

/* type of received frame */
#define Iok_r	 111
#define Inok_r	 112
#define RR_r  	 113
#define REJ_r  	 114
#define RNR_r  	 115
#define SABM_r 	 116
#define DISC_r 	 117
#define UA_r 	 118
#define DM_r 	 119
#define INVLD_r	 121
#define DSCRD_r	 125

/* link states */
#define DISC_PH	 00		/* link disconnectec state */
#define SABM_t	 51		/* link establishment state */
#define DISC_t	 52		/* link termination state */
#define INFO_TR	 55		/* information transfer state */
#define STARTING 56		/* intermidiate state to INFO_TR */
#define ENDING	 57		/* intermidiate state to DISC_PH */
#define RESETING 58		/* intermidiate state to DISC_PH */

/* INFO_TR states */
#define NRSQ_IREM 00		/* N(R) received in sequence */
#define NRSQ_BREM 57		/* N(R) received in sequence but blocked */
#define SQER_IREM 58		/* N(R) received out of sequence */
#define SQER_BREM 59		/* N(R) received out of sequence and blocked */

/* response types */
#define NULLFRM	 0
#define RR	 0x01
#define REJ	 0x09
#define DM	 0x0f
#define SABM	 0x2f
#define DISC	 0x43
#define UA	 0x63

/* pdlctab pd_flags defines */
#define PFCYCLE	  01		/*transmit frame P bit expect reply F bit*/
#define LINUSE    02		/* this link is in use */
#define LWAKEME   04		/* someone is sleeping on pptab */
#define OINUSE	  0040		/* this frame is currently in use (output) */
#define TINUSE	  0100		/* this frame is currently in use (timeout)*/
#define IINUSE	  0200		/* this frame is currently in use (input) */

/* frame error types */
#define CRTLF_W	  131		/* control field error (unknown frame) */
#define BADNR	  133		/* bad NR in frame header */
#define INCLGTH_X 134		/* framing error: invalid length */

	/* indicates pdlc type of network message */
#define PDLCTYPE 1

	/* sleep priority for cb output collision */
#define PDLCPRI	 28

	/* timer constants */
/* 	NOS Beta Release Phase 1 timeout values */
#define T1	100	/* frame retransmit time 2 second */
#define HSTFT	3000	/* dead host activity inhibitor time 60 seconds */

	/* number of retries */
#define N2	 10	/* number of retries attempted before quiting */
