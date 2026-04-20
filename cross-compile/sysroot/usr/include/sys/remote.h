/* SID @(#)remote.h	5.1 */

        /* remote.h --  common include file for remote kernal
	**		does not expect any other include files
	**		to preceed it .
	*/

#define SANE 1		/* verify the sanity of values */
#ifndef P60
#ifndef ROBIN
#define ROBIN
#endif
#ifndef robin
#define robin
#endif
#endif

#ifndef BSIZE
#define BSIZE 1024
#endif

#ifndef NCC
#define NCC 8
#endif

#define OUTBUFSZ 1024		/* must be power of 2 */
#define INBUFSZ 256		/* must be power of 2 */

#define EBUFSZ 10		/* round this so that sizeof r_tty is power
                                /* of 2. currently 64 .*/

#define NUMUNITS 8
#define MAXUNITS 8		/* MAX # of usarts */
#define NODEVICE (-1)
#define NOPTR	 (-1)

#define false 0
#define true 1

#define JDINTVECTOR	0xC2C2
#define DJINTVECTOR	0xC1C1

/* commands from the remote to the job processor */

#define INPUT   1               /* give unix an interrupt because delct = 1 */
#define SCSTAT  2		/* status from scsi completion */
#define ABORT   3               /* abort last disk request */
#define SDEBUG	4		/* sysdebug call */
#define BREAKINT 5		/* break interrupt on tty */
#define SCNODEV	 6		/* no device present */

/* commands from job processor to remote processor */

#define SCSICOM   1		/* disk com from job to remote */
#define TTYRFLUSH 2             /* flush tty read queue */
#define TTYWFLUSH 3             /* flush tty write que */
#define TTYBFLUSH 4             /* flush both input and output */
#define TTYOPEN   5		/* open tty */
#define TTYCLOSE  6		/* */
#define RESTART	  7		/* reset to boot */
#define INITSCSI  8		/* scsi init sequence */
#define SCSIRCOM  9		/* read on scsi. for non standard commands */
#define MSGCOM	 10		/* scsi message command */

/* Global data for remote CPU */

struct UCOM {			/* UNIX command */
        unsigned short op;      /* The command */
        unsigned short count;   /* the count, if any */
        short device;  		/* device # of interrupting device */
        struct 	 r_tty *RTP;   	/* pointer to r_tty struct */
        char 	 *src;     	/* pointer to char buffer or unix buf struct */
	int 	 status;	/* status of scsi command, if any */
	int 	 spare[2];	/* */
        } *UCOM;

struct RCOM {			/* REMOTE command */
	unsigned short op;
	unsigned short device;
	unsigned int spare[2];
	} *RCOM;

struct SCOM {                   /* SCSI command */
	unsigned char  cdblen;	/* length of the cdb , typically 6 */
        char device;		/* controller # */
	unsigned addr;		/* src / dest address */
	unsigned count;		/* number of bytes */
	struct buf *bp;		/* unix buffer pointer */
	unsigned spare[2];	/* */
        } *SCOM;


struct cdb {
	unsigned char op;	/* opcode field. */
	unsigned lun : 3;	/* lun field. */
	unsigned lad2 : 5;	/* logical address2 field */
	unsigned char lad1;	/* logical address1 field */
	unsigned char lad0;	/* logical address1 field */
	unsigned char byte4;	/* typically, number of blks or 0 */
	unsigned char byte5;	/* typically, control or 0 */
	};

struct initinfo {
	unsigned char	buffer[24];
};

/* this is the structure where the scsi command is found */

struct CDB {
        struct cdb cdb;
        } *CDB;


/* misc. defines for scsi interface */

#define NUMBLKS(p) p->byte4
#define LUN(p) p->lun
#define OP(p) p->op
#define LAD(p, x) p->lad0 = x ; p->lad1 = ( x >> 8 ); p->lad2 = ( x >> 16 )
#define DLEN(p,x) p->byte4 = x ; p->lad0 = ( x >> 8 )
#define DLENL(p, x) p->byte4 = x ; p->lad0 = ( x >> 8 ); p->lad1 = ( x >> 16 )

/* Generic scsi commands */

#define SCSIRDY		0x0
#define SCSIRECAL	0x1
#define SCSISENSE	0x3
#define SCSIFORMAT	0x4
#define SCSITRKFORMAT	0x6	/* track format used only for floppy disk */
#define SCSIREAD	0x8
#define SCSIWRITE	0xA
#define SCSISEEK	0xB
#define SCSIINIT	0xC2			/* not generic */

/* status of scsi device for each device controller */

#define NUMPTRS	6
#define DOPTR 0
#define DIPTR 1
#define COPTR 2
#define SIPTR 3
#define MOPTR 4
#define MIPTR 5
#define INPTR(x) (( x & 1 ) == 1)
#define OUTPTR(x) (( x & 1 ) == 0)

#define ZOMBIE	1
#define ACTIVE	2
#define DISC	4

struct scstat {
	unsigned char dir;
	unsigned char msg;
	struct	 buf *bp;
	unsigned short	state;
	short 	sctimeout;
	short 	timer;
	struct  pointer
	{
		long	address;
		long	count;
	} ptrs[NUMPTRS];
};

#define MAXCTLRS	8	/* MAX # of LOGICAL devices on scsi bus */
#define TAPEADDR	7	/* cartridge address on scsi bus */

/* controller types */
#define OMTI		0	/* omti */
#define	DTC		1	/* dtc */
#define	EMX		2	/* emulex */

/* length of valid data in initinfo */
#define DTCINITLEN	10
#define OMTIINITLEN	10
#define EMXINITLEN	24

struct scstat scdev[MAXCTLRS];
int master ;
int lastptr;
short *lastaddr;
short *leds;

#define RTRACE(F) if ( *lastaddr ) printf("Enter F\n");


struct r_tty {                  /* remote copy of the 'essential' part of
                                   the tty struct */
        unsigned short  iflag;	/* input modes */
        unsigned short  oflag;	/* output modes */
        unsigned short  cflag;	/* control modes */
        unsigned short  lflag;	/* line discipline modes */
        unsigned short  delct;	/* delimiter count */
        unsigned short 	state;	/* internal state */
        char 	*outbuf;     	/* pointer to this channel's output buffer */
        short   oremote;        /* remote processor output ptr */
        short   ojob;           /* job processor output ptr */
        char 	*iptr;     	/* ptr to the last input char put in dyn.mem */
	char	*lastint;	/* ptr of last int to unix */
        char 	*maxin;    	/* = &rtp->ttyp->inbuf[INBUFSZ] */
	char	*minin;		/* = &rtp->ttyp->inbuf[0] */
	struct usart *usp;	/* ptr to the usart device */
        char   	cc[NCC];	/* delimiter characters */
        short 	charcnt;	/* Number of characters in input queue */
        char 	echoptr;	/* pointer to last char put in echo queue */
        char 	untilptr;	/* ptr to last char echoed from echo queue*/
        char 	device;		/* tty number */
        char 	ebuf[EBUFSZ];   /* echo que */
        };

struct r_tty *remtty;
struct usart *usp[MAXUNITS] ;		/* pointer to each usart */

struct regs 
	{ int d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6 };

/* define physical locations in static memory for shared/common data */

#define UCOMADDR 0xC00000
#define RCOMADDR ( UCOMADDR + sizeof(struct UCOM ))
#define SCOMADDR ( RCOMADDR + sizeof(struct RCOM ))
#define CDBADDR  ( SCOMADDR + sizeof(struct SCOM ))
#define REMTTYADDR (CDBADDR + MAXCTLRS * sizeof(struct CDB ))
#define CONSADDR (REMTTYADDR + MAXUNITS * sizeof(struct r_tty))
#define LEDSADDR (CONSADDR + sizeof(int))
#define LASTRADDR (LEDSADDR + sizeof( short ))

/* time duration for timers . units are clock ticks */

#define ARBITTIME 	18000		/* 60*300 -  about 300 seconds */
#define SELECTTIME	15
#define RWTIME		15
#define STATUSTIME	15
#define INITTIME	15
#define ZILLION		1000000

/* default console */
#define CONSOLE		(&remtty[0])

/* Error codes. */
#define		ESCTIMOUT	1
#define		ETIMER		2
#define		ETTYCMD		3
#define		EUNEXP		4
#define 	EWRONGINT	5
#define		EACTIVE		6
#define		ECTLRNUM	7
#define		EPTRNUM		8
#define		ESCERRS		9
#define		ESAMEPTR	10

/* start of boot */
#define		BOOTSTART	0x808004

/* controller status */
#define		OKSTAT		0x00
#define		OKMASK		0x03

/* Error messages for scsi devices */

struct emesg
{
	short enumber;
	char *eptr;
};

