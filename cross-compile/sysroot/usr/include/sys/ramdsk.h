/* @(#)ramdsk.h	5.1 8/5/86 */
/*
	Ram Disk driver ioctl calls. All calls require super user status
*/
#define RAM_SIZE	('RAM'<<8)|1	/* set the size of a ram drive */
#define RAM_PIPE        ('RAM'<<8)|2	/* set the pipedev to this device */

/* the limit on how many ram disks can be configured */
#define RAM_MAXDEVS	8
