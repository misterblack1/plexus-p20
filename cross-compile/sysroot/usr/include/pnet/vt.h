/* SID */
/* @(#)vt.h	5.1 4/22/86 */
#define NODESIZ	sizeof(utsname.nodename)
#define VYOREQT	(1*(20*50))	/* timeout for ACK/NAK to nopen */ 
#define VYROT	(1*(20*50))	/* periodic timeout awaiting remote open */
#define VYNR	(20)		/* periodic timeout awaiting NOPEN-RESP */
#define NOSCATTER	0	/* NCF arg.  copy buffer data */
#define SCATTER		1	/* NCF arg.  no copy of buffer data */
/* VTTXOHI and VTTXOLO control # char in tp->rawq+canq and are used
 * to determine when to send CSTOP/CSTART
 */
#define VTTXOLO (128)		/* vttread checks this */
#define VTTXOHI (256)		/* vputc() and vtdata() check these.
				 * SHOULD Be < VTTYHOG. check vtdata() 
				 */
/* VTTYHIWAT is # char in vp->vy_obuf to force nwrite, ~ICANON mode only.
 */
#define VTTYHOG 2048
#define VTTYHIWAT (256)
#define CCOBUF (VTTYHIWAT+CLSIZE+2)	/* Max no. of char. in obuf */
#define MAXOBUF (sizeof(struct VTM_DA)+CCOBUF) /* size of obuf MUST BE EVEN */
/* maximum message from attach queue*/
#define MAXATTACHMSG	(sizeof(struct VTM_NO)+sizeof(struct ucorm))
	/* MAXVTMSG must be > any of the VTM_XXX messages  (see below) */
#define MAXVTMSG	MAXOBUF  /* maximum message sent to VT by a vtty */

#define NCFACK	0
#define NCFNAK	1

#define	isnumeric(c)	((c >= '0') && (c <= '9'))
#define even(x) (((x)&1)?(((unsigned)(x))+1):(x))
#define vtaborted(vp) (vp->vy_flags & VY_ABORT)
#define vtclosed(vp) (vp->vy_flags & VY_CLOSED)
#define toomanyccwr(vp) (vp->vy_ccwr >= 128)
#define RBUFSIZE 10	/* must be even */
#define sz_vtm_da(c_cc) ((c_cc <= RBUFSIZE) ? sizeof(struct VTM_DA) : \
	(even(sizeof(struct VTM_DA)-RBUFSIZE+c_cc)))

struct VTTY {				/* tty modes needed at remote end */
	ushort		t_iflag;	/* input modes     */
	ushort		t_oflag;	/* output modes    */
	ushort		t_cflag;	/* control modes   */
	ushort		t_lflag;	/* line discipline modes */
	short		t_state;	/* internal state  [30-31]*/
	char		t_col;		/* current column */
	char		t_delct;	/* delimiter count */
	unsigned char	t_cc[NCC];	/* control chars */
};
	/* remote vtty's ioctl parameters */
struct VTM_RIO {
	ushort		t_iflag;	/* input modes     */
	ushort		t_oflag;	/* output modes    */
	ushort		t_cflag;	/* control modes   */
	ushort		t_lflag;	/* line discipline modes */
	unsigned char	t_cc[NCC];	/* control chars */
} ;

struct VYD {
	ushort           v_flags;	/* for login/nonlogin bits */
	struct HOSTLIST *v_hp;
	struct VYDEV    *v_vp;
};
	/* v_flags values */
#define	LOGINTYPE	0x0001
#define NONLOGINTYPE	0x0002
#define ALLHOSTS	0x0004	/* all remote hosts are allowed */

	/*
	 * used to keep track of open ports and by the VT to 
	 * communicate with user process
	 * ISOPEN in vy_tty is set when a local process opens
	 * vtty AND has established a virtual circuit to remote VT.
	 * CARR_ON in vy_tty is set when remotely open
	 * WOPEN is a nop
	 */
struct VYDEV {
	dev_t         vy_rpi;		/* remote port id */
	struct VYPID *vy_pid;		/* pointer to per process data list */
	struct VTB   *vy_rbuf;		/* pointer to read buffer */
	unsigned int  vy_tact;		/* timeout id of vysleep() (vy_rpi) */
	ushort        vy_flags;		/* misc. information */
	struct VT_VC *vy_vtvc;		/* pointer to VT_VC table */
	short         vy_ccwr;		/* no. unack. char. nwritten */
	struct VTREM *vy_vtr;		/* pointer to vtrem table */
	struct VTB   *vy_obuf;		/* rttin() puts output data here */
	short         vy_ccobuf;	/* count of char in vy_obuf */
	struct VTTY   vy_rio;		/* remote tty parameters */
	struct tty    vy_tty;		/* has tty data */
};
	/* vy_flags values */
#define ROPENREAD	0x0008	/* vtty open remotely read */
#define ROPENWRITE	0x0010	/* vtty open remotely write */
#define INVYCLOSE	0x0020	/* keeps vyopen from setting ISOPEN while in
				   vyclose */
#define VY_ABORT	0x0080	/* vtty has lost carrier: unexp nclose/error */
#define VY_CLOSED	0x0400	/* vtty has lost carrier: normal close by
				   remote proc */
#define VY_WOSTATE	0x0300	/* Defines the "writeout"  state */
#define VY_WO0		0x0000
#define VY_WO1		0x0100
#define VY_WO2		0x0200
#define VY_WO3		0x0300


struct VYPID {
	int           vcnop;	/* -1 when process has not done nopen */
	struct VYNAT *vyn;
	short         count;
	enum {			/* imstate: intermediate states of v.c. */
	     VCIM_START,
	     VCIM_NAK,
	     VCIM_ACK,
	     VCIM_ROPEN,
	     VCIM_RCLOSED,
	     VCIM_REJECT,
	     VCIM_NONIM
	}             vcimstate;
	char          vyp_flag;	/* open flag need for implied opens*/
	char          vyp_state;  /* for aborts of allocated channels */
	struct VYPID *next;
};
	/* vyp_state values */
#define VYP_ABORT	0x01
#define VYP_IMPOPEN	0x02

struct VYNAT {			/* per process data */
	int           vyn_nat;	/* nattach vc. -1 when process not attached */
	struct proc  *vyn_pidp;	/* pointer to process table entry */
	short         vyn_cnt;	/* cnt of vtty's open by proc; 0-slot unused.*/
	struct VYNAT *vyn_next;
};
struct VT_VC {
	int           vt_vc;
	struct VT_VC *next;
};
/*
 * Local VT has a one v.c. to every remote system's VT to which 
 * a user process has nopened.  The v.c. addr's are kept here
 * along with remote host name.
 */
#define NO_VTREM 10			/* number of VTREM entries */
struct VTREM	{			
	int  vtr_vc;			/* v.c. addr to remote VT */
	enum {
	    VTR_NULL,
	    VTR_INUSE,
	    VTR_NOPEN,
	    VTR_ACK,
	    VTR_DONTUSE,
	    VTR_ABORT
	}    vtr_stat;
	char vtr_rhost[NODESIZ];	/* remote host name */
};
#define NO_VTPRO 1
struct VTP {
	struct proc *vtpid;
	int	     vcnat;
};

/* Writeout Table.
 * Used to timeout on buffered characters that are to be nwritten
 * over a v.c.  rttin() sets timer and adds entry to writeout table.
 * When timer goes off, vywotio() sends SIGVT to appropriate
 * user process and changes state to VY_WO3.  When user process receives
 * signal (see sig.c), it calls vywoflush() to do nwrite.
 */
#define N_WO		8	/* number entries in VT_WO table */
#define VT_WOTICKS (10)	/* timeout in ticks before nwriting buffer */
struct VT_WO {
	struct proc  *pidp;
	struct VYDEV *vp;
	int           timoid;
};

/* HOSTLIST: a linked list of host names. Initialized in vy.c as
 * an array indexed with same index as vydev table.  For LOGIN vtty,
 * a list of remote host that may login on vtty.  For NONLOGIN vtty,
 * a single entry specifying the remote host to be connect to.
 */
struct HOSTLIST {
	struct HOSTLIST *h_next;
	char             h_rhost[NODESIZ];
};
/* Messages between VT and user processes */
#define NEXT	u1.vtm_next
struct VTM_NR {				/* NOPEN-RESPONSE message */
	ushort         vtm_type;	/* type of the message */
	short          vtm_lpi;
	struct VTM_RIO vtm_rio;		/* remote vtty's ioctl parameters */
	ushort         vtm_status;	/* remotely open,closed,or error */
};
/* WARNING: VTM_DA and VTB are templates defining same storage area */
struct VTM_DA {				/* Data to a user process */
	ushort	vtm_type;		/* type of the message */
	short	vtm_lpi;
	dev_t	vtm_rpi;
	short	vtm_ccnt;		/* no. char in vtm_p[] */
	short	vtm_lcnt;		/* no. full lines in vtm_p[], ICANON */
	char	vtm_p[RBUFSIZE];	/* beginning of data. MAKE EVEN */
};
struct VTB {				/* Data stored in vy_rbuf/vy_obuf */
	struct  VTB *vtb_next;		/* masks vtm_type (& on 68k, vtm_lpi) */
	short	vtb_first;		/* masks vtm_rpi; # char read */
	short	vtb_last;		/* masks vtm_ccnt */
	short	vtb_lcnt;		/* masks vtm_lcnt */
	char	vtb_p[RBUFSIZE];	/* same as vtm_p */
};
struct VTM_CL {				/* Last close of vtty at remote end */
	ushort	vtm_type;		/* type of the message */
	short	vtm_lpi;
	dev_t	vtm_rpi;
	char	vtm_host[NODESIZ];	/* host name of guy doing nclose */
};
struct VTM_NO {				/* buffer used in nopen request */
	ushort         vtm_type;	/* type of the message */
	short          vtm_lpi;
	dev_t          vtm_rpi;
	ushort         vtm_flags;	/* want login/nonlogin, etc. */
	struct VTM_RIO vtm_rio;
	char           vtm_host[NODESIZ]; /* host name of guy doing nopen */
};
struct VTM_SI {				/* intr or quit signal to user proc */
	ushort	vtm_type;		/* type of the message */
	short	vtm_lpi;
	ushort	vtm_param;		/* misc. parameter */
};
struct VTM_IO {				/* intr or quit signal to user proc */
	ushort	       vtm_type;	/* type of the message */
	short	       vtm_lpi;
	struct VTM_RIO vtm_rio;
};
/* VT message types: vtm_type */

#define VTM_NOPEN	0x00
#define VTM_RESPNOPEN	0x01
#define VTM_ORIGCLOSE	0x02		/* originated close of vtty */
#define VTM_RSPCLOSE	0x03		/* closing in response to remote proc
					   close, VTM_ORIGCLOSE */
#define VTM_INTR	0x04
#define VTM_QUIT	0x05
#define VTM_FLCANQ	0x06		/* flush canq */
#define VTM_FLOUTQ	0x07		/* flush outq */
#define VTM_FLRAWQ	0x08		/* flush rawq */
#define VTM_CSTART	0x09
#define VTM_CSTOP	0x0a
#define VTM_IOCTL	0x0b		/* ioctl change */
#define VTM_BREAK	0x0c
#define VTM_VTREQUEST	0x0d		/* nopen request from VT to another */
#define VTM_ACKCSTART	0x0e
#define VTM_NAKCSTOP	0x0f	
#define VTM_DATA	0x10
#define VTM_DONTUSE	0x11
#define VTM_DOUSE	0x12

/* VTM_NO->vtm_flags */

#define WANTNONLOGINVTTY	0x01
#define READFLAG		0x02
#define WRITEFLAG		0x04
#define WANTLOGINVTTY		0x08

/* VTM_NR vtm_status */

#define NR_ROPEN	0x00
#define NR_RCLOSED	0x01
#define NR_ERROR	0x02

typedef struct ucorm *UCORM;
typedef struct ucoam *UCOAM;
typedef struct uccm  *UCCM;
typedef struct ucem  *UCEM;

extern struct utsname utsname;
extern int numuser;	/* number of active users r/w */
extern struct map map[];
extern char partab[];	/* used by rttxput */

struct VT_VC	*addvt_vc();
struct VTREM	*addvtr();
struct VT_WO	*addwo();
struct VYNAT	*donattach();
struct VTREM	*dupvtr();
dev_t		 fdev_vp();
struct VTREM	*findvtr();
struct VYPID	*findvypid();
struct VYPID	*fprocvypid();
struct VYNAT	*getvynat();
struct VTREM	*invtr();
struct VYD	*rmvt_vc();
int		 vtparse();
struct VTREM	*vtrequest();
struct VTM_DA	*vtrbuf;	/* global buf pointer for nreading vc */
int		 vttyclose();
int		 vttyopab();
int		 vyabclose();
int		 vyabvclose();
struct VYDEV	*vydevalloc();
struct VYPID	*vychkabort();
struct VYPID	*vygetpid();
char		*vymkname();
int		 vyproc();
int		 vywotio();

#define SLP	0
#define NOSLP	1
#define VTCONF "/usr/lib/nos/vtconf"
char *vtconf;
struct VYD	*vydev;			/* points to start of VYD entries */
struct VYD	*vydevend;		/* adx of end of vydev table */
struct VYNAT	*vynat;
struct VT_WO	vt_wo[N_WO];
struct VYDEV	*vydum;			/* for following macros */
dev_t		vy_cnt;

#define fvp_tp(tp) ((struct VYDEV *) \
	((unsigned)tp - ((unsigned) &vydum->vy_tty - (unsigned) vydum)))
#define fdev_tp(tp) ((dev_t) fdev_vp(fvp_tp(tp)))
#define fvtp_tp(tp) ((struct VTTY *) \
	((unsigned)tp - ((unsigned) &vydum.vy_tty - (unsigned) &vydum.vy_rio)))
