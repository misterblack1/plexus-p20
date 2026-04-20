/* SID @(#)disk.h	5.1 */
/*
 * This file contains structures used commonly by different disk drivers.
 */

struct	diskconf {
	long	blkoff;
	long	nblks;
};

/*
 * This structure defines the disk initialization information for the
 * ISBC and IMSC controllers.
 */
#ifndef robin
struct initinfo {
	unsigned short	cylinders;
	char	remheads;
	char	fixheads;
	char	lsb_sectsize;
	char	sectpertrack;
	char	altcylinders;
	char	msb_sectsize;
};
#endif
/*
 * This structure defines the layout of block zero of each disk.
 */

struct block0 {
	struct initinfo init;
	unsigned short	fsbsize;
	unsigned short	id;
	unsigned short	initsize;
	unsigned short	unused;

	char		bootname[0x50];
	char		nodenm[9];
	unsigned short	unused2;
	unsigned short	unused3;
	unsigned short	unused4;
	unsigned short	nswap;
	long		swplo;
	unsigned short	rootdev;
	unsigned short	pipedev;
	unsigned short	dumpdev;
	unsigned short	swapdev;
	unsigned short	unused5;
	struct diskconf	confinfo[32];

#ifdef robin
	unsigned short	type;		/* controller type */
	unsigned short	spec;		/* controller specific */
	unsigned long	initlen;	/* length of valid data */
	struct cdb 	cdb;		/* init cdb */
	unsigned long	scsiaddr;	/* Controller address (0-7) on 
					** the scsi bus 
					*/
	unsigned char	scsimap[40];	/* Alternate Sector Map */
	unsigned long	alttrks;	/* Number of Alternate Tracks */
#endif

	/* information about mirroring */
	short miversion;		/* version number (=1 for now) */
	unsigned short mirid; 		/* equals "md" when mirrored disk */
	time_t mitimestamp;		/* value of "time" when written */
	short miprimdriv;		/* primary's index into mipairs table */
	dev_t misecondary;		/* secondary drive's device */
	short miflags;			/* copied from mipairs "flags" */
};


