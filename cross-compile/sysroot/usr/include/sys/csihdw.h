
/* SID @(#)csihdw.h	5.1 */
/*	csihdw.h 6.1 of 8/22/83
	@(#)csihdw.h	6.1
 */

/*      CSI- KMC device structure   */


struct csik {
	short state;
};


#define NOERRS	8			/* no. of error counters */

/*	CSI interface structure 	*/

struct csi {
	struct 	csillist	xmtq;
	struct  csillist	xqkmc;
	struct  csillist        emptq;
	struct  csillist        eqkmc;
	dev_t	       	mdev;	        /* The reconstructed device number */
	int      	(*rint)();      /* The protocol "interrupt" routine */
	int		pindex;		/* Protocol index number */
	short		state;		/* State of the csidev */
	char		xmax;           /* Max xmit buffers kmc can take */
	char            rmax;		/* Max rcv buffers kmc can take */
	short 		errors[NOERRS]; /* Interpreter error counters */
	struct csiflags	flags;		/* CSI start flags */
};


/*	CSI Buffer descriptor	*/

struct csibd {
	long		d_uadres;	/* unibus mapped addr */
	short		d_ct;		/* Character count or buf size */
	short		d_size;		/* Size of the buffer */
	char		d_octet1;
	char		d_octet2;	/* Octets  */
	char		d_octet3;
	char		d_admin;	/* Administrative byte */
#ifdef pdp11
	paddr_t		d_adres;	/* Virtual address */
#else
	char 		*d_adres;        /* Virtual address */
	int		d_uba;		/* uba index that protocol mapped */
#endif
	struct csibd	*d_next;	/* Pointer to next buffer */
};

/*	CSI buffer structure	*/


#define CSIMAP 64
struct csibuf {
	struct map	csimap[CSIMAP];		/* The map for this buffer */
	struct csibd	*freebdp;		/* First buffer descriptor */
	unsigned	csibsz;			/* Total size of the buffer */
#ifdef pdp11
	paddr_t		segp;			/* Pointer to buffer */
#else
	char		*segp;
	unsigned short	nexus;
#endif
	paddr_t		ubmd;			/* unibus map base */
	paddr_t		bubm;			/* unibus mapped base addr*/
};

/* CSI Structure 	State Definitions	*/

#define C_ATTACH	 001		/* The csiattach has been done */
#define C_RUN	 002			/* KMC is running */
#define C_START	 004			/* The csistart has been done */
#define C_RESET  010			/* Powerfail restarting */

/* CSI-KMC Structure    State Definitions       */

#define KMCRUN   001			/* KMC/KMS is Running    */
#define KMCOK    02000			/* KMC/KMS is  ok!       */
#define KMCRESET 004			/* KMC/KMS in reset      */

/*	Misc. definitions	*/

#define CSIBSZ	2048
#include "sys/csikmc.h"

