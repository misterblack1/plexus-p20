/* @(#)od.h	5.3 8/22/86 */
/*
	This include file defines the interface to the optical disk
	driver. All the ioctl calls and their parameters are
	defined. The structure of a partition directory entry is also
	defined.
*/

#define OD_MAXDRVS	8	/* the maximum number of supportable drives */
#define OD_MAXDEVS	32	/* the maximum number of minor devices */

/*
	the following ioctl commands are supported.
*/
#define OD_CREATE	1	/* create a partition with a unique name. */
#define OD_CREATE_DUP	2	/* create a partition, allow duplicate. */
#define OD_NAME_CONF	3	/* configure a device by partition name */
#define OD_VALUE_CONF	4	/* configure a device to specific values */
#define OD_FIND_BLANK	5	/* scan for a blank sector */
#define OD_FIND_DATA	6	/* scan for a written sector */
#define OD_NOREW_OFFSET	7	/* set the norewind offset to the arg value */
#define OD_DRIVE_INFO	8	/* return driver info about a drive. */
#define OD_DEVICE_INFO	9	/* return driver info about a minor device */
#define OD_EXEC_CMD	10	/* execute a drive dependant command */
#define OD_DECONFIG	11	/* force a device to not be configured at all */


/*
	The creation log and directory sectors have the following structure
*/
#define PR_NAMESZ	32	/* characters allowed in partition name */
#define PR_COMMENTSZ	256	/* limit on the size of the comment */
#define PR_MAGIC	(~'OD01')/* magic number identifying version 1 drives */
struct od_partition {
	/* the following values are the same for all partitions and only
	depend on the characteristics of the media */
	int	pr_magic;	/* magic number to identify the structure */
	int	pr_sectsz;	/* sector size in bytes */
	int	pr_drvsz;	/* drive size in sectors */
	int	pr_dirsz;	/* size of the directory in sectors */
	int 	pr_logsz;	/* size of creation log in sectors */
	/* the following values define this partition */
	int	pr_offset;	/* first sector in the partition */
	int	pr_size;	/* size of partition in sectors */
	int	pr_dirsect;	/* sector where directory entry is found */
	int	pr_tcreated;	/* time created */
	int	pr_tfixed;	/* time size was fixed */
	int	pr_type;	/* type of creation */
	char	pr_name[PR_NAMESZ];
	char	pr_comment[PR_COMMENTSZ];
};
/* the following creation types may exist */
#define PR_TYP_FIXED	1	/* size was specified when created */
#define PR_TYP_INDEF	2	/* indefinite size when created */
#define PR_TYP_RECOV	3	/* size recovered. It was indefinite when
				created, but was not closed normally */

/*
	The following structure is pointed to by the two ioctl create calls
*/
struct od_create {
	int	od_drive;	/* number of drive to create partition on */
	int	od_size;	/* size of new partition. Zero if indefinite */
	char	od_crname[PR_NAMESZ];	/* name string with null fill*/
	char	od_comment[PR_COMMENTSZ];/* comment string or any other data*/
	int	od_dirmax;	/* if media is empty and this is not zero and
				   not more than 1% of media this becomes the
				   size of the directory */
};

/*
	The following structrue is passed to the configure by name ioctl
	It configures to the latest entry with the indicated name on the
	first drive in the list containing a partition with this name.
*/
struct od_name_conf {
	char	od_list[OD_MAXDRVS];	/* list of drives to search. Unused
					entries are filled with 0xff. List
					defines search order. */
	char	od_cfname[PR_NAMESZ];	/* name string with null fill */
};

/*
	The following structure is passed to configure by value ioctl
*/
struct od_value_conf {
	int	od_drive;	/* drive number */
	int	od_size;	/* size of disk area in sectors */
	int	od_offset;	/* beginning of disk area in sectors */
};

/*
	The following structure is passed to the two find ioctl calls.
	If the start value is -1 it is set to the position just beyond 
	the last access. If the count is -1 it is set to the remainder
	of the partition. The start is always relative to the beginning
	of the partition and does not include the no-rewind offset.
	When the ioctl call returns the start and count that were
	really used are in the structure. The skipped field indicates
	how many sectors were skipped over before finding the desired
	type of sector (blank or data). If skipped equals count then
	no sector was found. If there is an i/o error then skipped is
	set to -1.
*/
struct od_find {
	int	od_fstart;	/* sector to start scanning at */
	int	od_fcount;	/* give up after this many sectors */
	int	od_skipped;	/* number of sectors of wrong type */
};

/*
	The following structure is pointed to by the drive information
	ioctl call. The driver returns this information.
	The drive to report on is shifted left 8 bits and ored with the command
*/
#define OD_SECTSZ	1024	/* currently only drives with 1024 byte
				   sectors are supported by driver */
struct od_drvinfo {
	int	od_drflags;	/* flags defined below */
	/* these come from the controller and are valid if drive exists */
	int	od_sectsz;	/* sector size in bytes */
	int	od_drvsz;	/* drive size in sectors */
	/* these are defaults if no directory found yet else from partition 0 */
	int	od_magic;	/* magic number to identify the structure */
	int	od_dirsz;	/* size of the directory in sectors */
	int 	od_logsz;	/* size of creation log in sectors */
	int	od_datasz;	/* size of data area for partitions in sectors*/
	/* These values are calculated by scanning the directory */
	int	od_dirused;	/* directory sectors used */
	int	od_logused;	/* creation log sectors used */
	int	od_dataused;	/* data sectors used */
	int	od_dropens;	/* count of opens on this drive */
};
/* the following flags are defined in od_drflags */
#define	OD_EXISTS	0x0001	/* the drive exists */
#define OD_EXCL		0x0002	/* there is an exclusive open */
#define OD_DEFAULT	0x0004	/* sizes are assumed. not read from the disk */
#define	OD_INDEF	0x0008	/* indefinite partition currently open */
#define OD_VALID	0x0010	/* valid directory and creation log read */
				/* empty disk is valid. Indefinite that needs */
				/* recovery is not valid. Removal of media */
				/* looses validity */
#define OD_EMPTY	0x0020	/* directory and log are completely blank */
#define OD_SPINNING	0x0040	/* the media is spinning and ready */
#define OD_FULL		0x0080	/* either the directory or data area is full */
#define OD_DIRALLOC	0x0100	/* a process has locked the directory */
#define OD_DIRWANT	0x0200	/* a process is waiting to lock the directory */
#define OD_PROTECT	0x0400	/* the drive needs to be protected from the
				operator removing media as soon as it's spinning */


/*
	The following structure is pointed to by the device information
	ioctl call. The driver returns this information.
	The device to report on is shifted left 8 bits and ored with the command
*/
struct od_devinfo {
	int	od_drive;	/* drive number */
	int	od_size;	/* size of disk area in sectors */
	int	od_offset;	/* beginning of disk area in sectors */
	int	od_dvflags;	/* flags defined below */
	int	od_dirsect;	/* sector where directory entry is found */
	int	od_nrpos;	/* current posistion for no-rewind close */
	int	od_nroffset;	/* offset for no-rewind operation */
	int	od_wrpos;	/* write posistion for indefinite open */
	char	od_name[PR_NAMESZ];	/* ascii name of partition if any */
};

/* the following flags are defined in od_dvflags */
#define	OD_EXISTS	0x0001	/* the drive in od_drive exists */
#define OD_EXCL		0x0002	/* there is an exclusive open */
#define OD_WRITE	0x0004	/* the device is open for writting */
#define OD_INDEF	0x0008	/* the size of the device is indefinite */
#define OD_NOREW	0x0010	/* the no-rewind device is open */
#define OD_DIRTY	0x0020	/* a write has happened since opening */
#define OD_VERIFY	0x0040	/* it is necessary to verify that the same
				platter is in the drive */
#define OD_UMOUNT	0x0080	/* the disk was unmounted. A verify failed */
#define OD_NREOF	0x0100	/* last i/o operation was at no rewind eof */
#define OD_NAMED	0x0200	/* the device was configured by name */
#define OD_CONF		0x0400	/* the device has been configured */
#define OD_ROPEN	0x0800	/* raw device opens are outstanding */
#define OD_BOPEN	0x1000	/* block device opens are outstanding */
#define OD_OPEN		(OD_ROPEN|OD_BOPEN|OD_NOREW)

/*
	The following values can be shifted left 8 bits and ored with
	OD_EXEC_CMD to do device specific ioctl calls. The addr to the
	command points to an od_cmd_comm structure where the
	sense data and any results from the command will be returned.
	These values are converted to SCSI commands that are sent
	to the logical unit associated with the device. Note that 
	the low order 8 bits is the command field for a SCSI command.
	The nineth bit is used to flag varients of the same basic
	command. If the command expects data on input it should be
	in the data area of the structure when the call is made.
	When the call returns there will always be sense data. If
	there were no errors or the error was from the host
	adaptor then the sense data will be zero. If there are any
	errors from the hardware, the ioctl call will return -1
	and errno will be set to EIO. Problems with the call will
	generate other errors.
	For details on the sense data and the command operation see
	the Optimem 1000 interface manual.
*/
#define OD_SNSZ		10	/* number sense bytes always returned */
#define OD_DATASZ	22	/* number of data bytes returned */
#define ODC_INQUIRE	0x012	/* inquire drive type */
#define ODC_RDCAP	0x025	/* read capacities */
#define ODC_TSTRDY	0x000	/* test drive ready */
#define ODC_EBLKCHK	0x115	/* enable blank checking on wirtes */
#define ODC_DBLKCHK	0x015	/* disable blank checking on writes */
#define ODC_MODESNS	0x01a	/* report mode sense data */
#define ODC_START	0x11b	/* start drive spinning */
#define ODC_STOP	0x01b	/* stop drive spinning */
#define ODC_PREVENT	0x11e	/* prevent media removal */
#define ODC_ALLOW	0x01e	/* allow media removal */
#define ODC_VBLANK	0x12f	/* for internal use only - use OD_FIND_DATA */
#define ODC_VDATA	0x02f	/* for internal use only - use OD_FIND_BLANK */
#define ODC_REZERO	0x001	/* for internal use only */
#define ODC_SELFTEST	0x11d	/* self test diagnostics */
#define ODC_SENDDIAG	0x01d	/* send diagnostic */
#define ODC_RCVDIAG	0x01c	/* recieve diagnostic results */
#define ODC_RDSTAT	0x0e6	/* read drive status */
#define ODC_RDPSTAT	0x1e6	/* read previous drive status (at last error) */

/* structure argument points at */
struct od_cmd_comm {
	unsigned char od_sense[OD_SNSZ];
	unsigned char od_data[OD_DATASZ];
};
