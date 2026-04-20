/* SID */
/* @(#)sc.h	5.1 4/22/86 */

#define SCLOGVOL	16
#define SCUNITS		2
#define SCTYPES		2
#define MAXHDCTLRS	4

#define SSEEK 1
#define SIO 2
#define SRECAL 3

struct	initinfo sc_init[SCTYPES] = {
/* dtc */
11, 60, 0, 3, 1, 0x31, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* emulex */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int sclen[SCTYPES] = {
	DTCINITLEN,		/* dtc */
	EMXINITLEN		/* emulex */
	};

struct	diskconf sc_sizes[SCLOGVOL*SCUNITS*MAXHDCTLRS] = {
	0,	1000000,	/* blocks 0 thru end of disk */
	0,	40000,		/* blocks 0 thru 39999 */
	40000,	1000000,	/* blocks 40000 thru end of disk */
	60000,	1000000,	/* blocks 60000 thru end of disk */
	80000,	1000000,	/* blocks 80000 thru end of disk */
	100000,	1000000,	/* blocks 100000 thru end of disk */
	120000,	1000000,	/* blocks 120000 thru end of disk */
	140000,	1000000,	/* blocks 140000 thru end of disk */
	160000,	1000000,	/* blocks 160000 thru end of disk */
	180000,	1000000,	/* blocks 120000 thru end of disk */
	200000,	1000000,	/* blocks 200000 thru end of disk */
	220000,	1000000,	/* blocks 220000 thru end of disk */
	240000,	1000000,	/* blocks 240000 thru end of disk */
	260000,	1000000,	/* blocks 260000 thru end of disk */
	280000,	1000000, 	/* blocks 280000 thru end of disk */
	300000,	1000000, 	/* blocks 300000 thru end of disk */
};
