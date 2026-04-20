/* @(#)mi.h	5.2  */

/* Simple ioctl commands. */
#define MI_SHUTDOWN	2	/*shutdown system and reboot */
#define MI_GETMIR	6	/* get the status of mirroring any drives */
				/* addr points at a struct onepair for results*/

/* Ioctl commands that take a struct miatt pointed to by addr */
#define MI_RECONNECT	7	/*connect a drive that was disconnected */
#define MI_CONNECT	5	/* connect two drives into a mirrored pair */

/* Ioctl commands ored with (drive_number << 8) */
#define MI_DISCONNECT	8	/*disconnect two drives that are connected */
				/* drive_number is primary drive of pair */
#define MI_GETSTATUS	4	/* get status of one drive */
#define MI_UNMIRROR	9	/* remove all traces of mirroring from bth drives */
#define MI_VERIFY	10	/* check disks in a mirror match */
				/* addr points at a struct onepair for results*/


#define MI_SHIFT		4	/*# of bits in minor number for same disk*/
#define MI_DRIVE     (0xffffffff<<MI_SHIFT) /*mask for major# & minor drive #*/
#define MI_PARTN     (~(MI_DRIVE)) 	/*mask for partition # on disk*/
#define MI_MAXPAIRS	(0x0100>>MI_SHIFT)	/* dimension of mipairs[] */
#define MI_MAXPARTNS	(MI_PARTN+1)	/* maximum number of partitions on a drive*/

struct onepair {
	short flags;	/*see descriptions below*/
	dev_t secondary; /* secondary device's minor #.  (primary uses the same
			    minor number as the mirror itself) */
        unsigned low;	/* low and hi mark the locked sectors during the 
			"being connected" copy from disk to disk*/
	unsigned hi;	/* during initialize only, low holds disk's timestamp, 
			and hi holds drive # of primary disk*/
	int driveinfo[2]; /*per mirrored drive, holds negative number of 
			pending I/O requests, or, if none, holds block number of
			last request that completed.  Used for picking which 
			drive to read from when there is a choice. */
	time_t timestamp; /* timestamp on disk */
	int primdriv;	/* drive number of the primary drive */
	struct {
		struct diskconf part[MI_MAXPARTNS];
	} dconf;	/* table of partition offsets and sizes from block 0 */
};
/*individual bits of "flags".  Variables in hi byte are faster to access.*/
#define M_USED		0x100	/*on for compatibilty with old stuff*/

#define M_NEEDSWRITE	0x200	/* need to update info in block 0 of mirror */

#define M_ONEDISCONNECTED	0x400	/* one of the 2 mirror disks is disconnected */

#define M_BEINGCONNECTED	0x800	/* one of the 2 disks is being connected */

#define M_PDISCONNECT	0x1000	/* set when the primary disk is disconnected or being connected */

#define M_DEFERREDDISCONNECT 0x2000	/* indicates good disk got error while being connected, and should be disconnected once connect is complete */

#define M_OPEN		0x4000	/*this mirror has been opened*/

#define M_CONFLOADED	0x0008	/* sector zero was successfully read at init, and the configuration table was loaded */

#define M_MIRRORED	0x0004	/* this drive is part of a mirror */

#define M_BROKEN	0x0002 /* buddy drive cannot be found */

#define M_INSYNC	0x0001 /* true iff no writes pending */

#define M_WRITESYNC	0x0010 /* true iff update() pending */

#define M_NEWMIRROR	0x0020 /* ok to write0() unmirrored drive */
#define M_VERIFY	0x0040 /* in verify */
#define M_DISOFF	0x0080 /* one disconnected, one offline */
#define M_UNMIRROR	0x8000 /* request to erase mirror info */

struct miatt {
	caddr_t mem; /*address of memory buffer used during disk to disk copy*/
	int nbytes; /*size of memory buffer in bytes*/
	dev_t source; /*1st partition on source disk, when dev=255*/
	dev_t dest; /*device id for 1st partition on destination, when dev=255*/ /*both source and dest must have BLOCK device major numbers*/ }; /*parameters passed to MICONNECT ioctl*/

