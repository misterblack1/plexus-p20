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

struct initinfo {
	unsigned	cylinders;
	char	remheads;
	char	fixheads;
	char	lsb_sectsize;
	char	sectpertrack;
	char	altcylinders;
	char	msb_sectsize;
};

/*
 * This structure defines the layout of block zero of each disk.
 */

struct block0 {
	struct initinfo init;
	unsigned	fsbsize;
	unsigned	id;
	unsigned	initsize;
	unsigned	unused;

	char		bootname[0x50];
	char		nodenm[9];
	unsigned	unused2;
	unsigned	unused3;
	unsigned	unused4;
	unsigned	nswap;
	long		swplo;
	unsigned	rootdev;
	unsigned	pipedev;
	unsigned	dumpdev;
	unsigned	swapdev;
	unsigned	unused5;
	struct diskconf	confinfo[32];
};


