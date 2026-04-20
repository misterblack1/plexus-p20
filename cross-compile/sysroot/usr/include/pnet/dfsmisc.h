/* SID */
/* @(#)dfsmisc.h	5.1 4/22/86 */

/*
 * Dfs remote inode use list, used for failure recovery.
 */

struct riulst {
	struct riulst	*riu_link;	/* link to next in list */
	struct inode	*riu_ip;	/* inode pointer */
	ushort		riu_lock;	/* inode locked flag */
	ushort		riu_cnt		/* inode use count */
};

/*
 * Dfs processes work queue pointers.
 */

struct	dqp	{
	char	qp_name[9];		/* name of dfs process */
	struct	dfswe	*qp_flink;	/* fisrt work entry in chain */
	struct	dfswe	*qp_llink;	/* last work entry in chain */
	struct	dfswe	*qp_wflink;	/* fisrt in prog. work entry */
	struct	dfswe	*qp_wllink;	/* last in prog. work entry */
	struct	riulst	*qp_riflink;	/* remote inode use list first link */
	struct	riulst	*qp_rillink;	/* remote inode use list last link */
	ushort	qp_temps;		/* number of temporary processes */
	ino_t	qp_ddino;		/* dotdot ino for mounted direct */
	dev_t	qp_dddev;		/* dotdot dev for mounted direct */
	ushort  qp_rw;                  /* read only flag */
	char	qp_mntflg;		/* mount/rmntab flag for pointer */
	caddr_t	qp_mntptr;		/* pointer to mount/rmntab entry */
};

#define	RMNTAB	1			
#define	MNTAB	2

/*
 *  Basic dfs message header format.  First part of all messages.
 */

struct	dfshdr	{
	unsigned char	dh_type;	/* type of dfs request/response */
	unsigned char	dh_stat;	/* status of dfs request/response */
	unsigned char	dh_error;	/* additional error indicator */
	ushort dh_len;		/* length of dfs request/response */
	struct	dfswe	*dh_dwent;	/* dwe work entry queue pointer */
};


/*
 *  Mounted directory request/response message format.
 */
/*
struct	dmntmsg {
	struct	dfshdr	dmm_hdr;
};
*/

/*
 *  Dfs remote mount request/response message format.
 */

struct	dfsmnt {
	struct	dfshdr	dm_hdr;
	char	dm_rdir[DFSMAXP];	/* remote directory path name */
	char	dm_ldir[DFSMAXP];	/* local directory path name */
	char	dm_node[9];		/* node mount request to */
	char	dm_rnode[9];		/* node of request */
	char	dm_pro[9];		/* process name for remote req. */
	char	dm_rpro[9];		/* process name at remote node */
	ushort	dm_ip;			/* remote inode pointer ordinal */
	ushort  dm_rw;                  /* access mode */
};

#define	dm_type	dm_hdr.dh_type
#define dm_stat	dm_hdr.dh_stat
#define dm_error dm_hdr.dh_error
#define dm_len	dm_hdr.dh_len


/* 
 * Dfs read/write request message format.
 */

struct	dfsrdwr {
	struct	dfshdr	rwhdr;
	ushort	rw_fp;
	ushort		rw_flag;
	unsigned long	rw_count;
	ushort	rw_ip;
	off_t		rw_offset;
	union {
		struct rdwrm {
			struct dfswe	*rwm_dwe;
			unsigned char	rwm_data[MAXDFSREC];
		} rwm;
		struct iocm {
			unsigned long	iom_count;
			unsigned char	iom_data[MAXIOCTLDATA];
		}iom;
	} iorw;
};

#define	rw_type rwhdr.dh_type
#define rw_stat	rwhdr.dh_stat
#define rw_rerror rwhdr.dh_error
#define rw_data	iorw.rwm.rwm_data
#define rw_dwe	iorw.rwm.rwm_dwe
#define ioc_type rwhdr.dh_type
#define ioc_error rwhdr.dh_error
#define ioc_fp rw_fp
#define ioc_rval1 rw_ip
#define ioc_offset rw_offset
#define ioc_cmd	rw_flag
#define ioc_arg	rw_count
#define ioc_data iorw.iom.iom_data
#define ioc_count iorw.iom.iom_count

/*
 * Save User  structure.
 * One allocated on each time a routine in rsecure is called.
 */
struct	saveuser
{
	struct direct sudent;
	struct inode *supdir;
	struct inode *surdir;
	off_t suoffset;
	char *sudirp;
	char susegflg;
};

