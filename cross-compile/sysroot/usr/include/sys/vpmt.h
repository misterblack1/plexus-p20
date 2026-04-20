
/* SID @(#)vpmt.h	5.1 */
/*	vpmt.h 6.1 of 8/22/83
	@(#)vpmt.h	6.1
 */

#ifndef PLEXUS
#define	XBQMAX	4
#else
#define	HXBQMAX	4
#endif
#define	EBQMAX	4
/*
 * per vpmt structure
 */
struct vpmt {
	struct csillist vt_rcvq;	/* Recieve queue */
	struct csibd *vt_rbc;		/* Current recieve buffer */
	short	vt_state;		/* Status info*/
	short	vt_bos;			/* Bytes left in buffer*/
	char	vt_idev;		/* VPM interface minor device number */
	char	vt_rpt[4];		/* VPM reports from interpretor */
	short   vt_errset[8];		/* values for setting error counters */
	char    vt_options;		/* protocol options */
	char	vt_intopts;		/* interpreter options */
};
/*
 * VPM information
 */
struct vpminfo {
	int xbq;	/* xmt buffer queue maximum */
	int ebq;	/* empty buffer queue maximum */
	int bufdes;	/* Number of buffer descriptors */
};

/*
 * table of buffer addresses for the writes.
 */
#ifdef PLEXUS
struct vpmwrtab {
	paddr_t	wrttab[HXBQMAX];
};
#endif

/* IOCTL defines */
#define VPMT		('V'<<8)
#ifndef PLEXUS
#define VPMCMD 		(VPMT|8)
#define VPMERRS		(VPMT|9)
#define VPMRPT		(VPMT|10)
#endif
#define VPMSDEV		(VPMT|11)
#define VPMATTACH	(VPMT|11)
#define VPMDETACH	(VPMT|31)
#define VPMERRSET       (VPMT|32)
#define VPMERRGET	(VPMT|34)
#define VPMOPTS		(VPMT|33)
#define VPMPCDOPTS      (VPMT|35)
#ifndef PLEXUS
#define VPMTRCO		(VPMT|16)
#endif
#define VPMGETC		(VPMT|17)
#define VPMSETC		(VPMT|18)
#define VPMCLRC		(VPMT|19)
#define VPMSTAT		(VPMT|36)


/*  Misc. Definitions */
#define VSLP	1
#define NVSLP	0

