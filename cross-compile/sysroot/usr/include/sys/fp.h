/* SID */
/* @(#)fp.h	5.1 4/22/86 */
/* SID */
/* @(#)fp.h	1.3 2/19/85 */

#define FPUNITS  2
#define FPOFFSET 2		/* On the dtc floppy start at unit 2 */
#define FPINITOP 0xC0
#define FPFMTTYPE 0x8B

#define MAXFPBLKS 1280		/* 2(sides) * 80(trks) * 8 (blks/trk) */
#define BLKS_TRK  8		/* number of blocks per track */

#define SSEEK	1
#define SIO	2
#define SRECAL	3
#define C_FMTTRK 6

struct fpcmd {
	short	dknum;
	long	blkno;
	long	blkcnt;
};
