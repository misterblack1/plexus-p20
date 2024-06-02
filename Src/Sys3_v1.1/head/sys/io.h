#include "sys/pd.h"

struct	initinfo pd_init[PDUNITS] = {
	589, 0, 7, 0, 34, 20, 2, 
	589, 0, 7, 0, 34, 20, 2,
	589, 0, 7, 0, 34, 20, 2,
	589, 0, 7, 0, 34, 20, 2,
};

struct	diskconf pd_sizes[PDLOGVOL*PDUNITS] = {
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

struct	initinfo is_init[ISUNITS] = {
	823, 0, 5, 0, 35, 20, 2, 
	823, 0, 5, 0, 35, 20, 2,
	823, 0, 5, 0, 35, 20, 2,
	823, 0, 5, 0, 35, 20, 2,
};

struct	diskconf is_sizes[ISLOGVOL*ISUNITS] = {
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
	280000,	1000000,	/* blocks 280000 thru end of disk */
	300000,	1000000,	/* blocks 300000 thru end of disk */
};
