/* SID */
/* @(#)dfsstat.h	5.1 4/22/86 */

#define DFSMAXP		50

#define DFSMAJ		0xff	/* pseudo-major device for DFS */
#define DFSRO		1	/* read-only flag for remote file system */
#define RMTPIPE		01	/* flag for remote pipe request */
#define RMTRQ1		02	/* flag for first remote pipe request */
#define ERPIPE 		35	/* err code for failed remote pipe request */
#define RDEOF  		36	/* EOF for remote read */
#define DFSPRI  	26	/* DFS priority */
#define PNETSIG		(1<<22)
#define DFSID		0x7fff	/* network user id */
#define MAXDFSREC	1024	/* size of file system data in message */
#define MAXIOCTLDATA	64	/* size of general ioctl data area */

#define DFSMFST		1	/* first message in chain indicator */
#define DFSMMID		0	/* middle message in chain indicator */
#define DFSMLST		2	/* last message in chain indicator */

/*
 * Errors for rmount/rumount
 */

#define ENOLDIR		0x7e	/* local directory is not dir */
#define	ENORDIR		0x7d	/* remote directory is not dir */
#define ELDBUSY		0x7c	/* local directory is in use /
#define ELDRMT		0x7b	/* local directory is remote */
#define ELDROOT		0x7a	/* local directory is root dir */
#define ERDBUSY		0x79	/* remote directory is already mounted */
#define	ENONWRS		0x78	/* no n/w resources, i.e. mnttab full, etc */

/*
 *  Dfs work entry types.
 */

#define	DWEMNT		0	/* mount work entry */
#define DWEUMNT		1	/* unmount work entry */
#define DWEOPN		2	/* open work entry */
#define DWECLS		3	/* close work entry */
#define DWEREAD		4	/* read work entry */
#define DWEWRT		5	/* write work entry */
#define	DWESEEK		6	/* seek work entry */
#define DWENMI		7	/* namei work entry */
#define DWEIP		8	/* iput work entry */
#define DWEPRE		9	/* prele work entry */
#define	DWESIS		10	/* sister process work entry */
#define DWECHMOD 	11	/* chmod work entry */
#define DWESTAT1 	12	/* stat1 work entry */
#define DWEMNODE	13	/* maknode work entry */
#define DWEIT		14	/* itrunc work entry */
#define DWECHOWN 	15	/* chown work entry */
#define DWEIC		16	/* icount work entry */
#define DWEPLCK 	17	/* plock work entry */
#define DWEUT		18	/* utime work entry */
#define DWEUNLINK 	19	/* unlink work entry */
#define DWELINK		20	/* link work entry */
#define DWERDWR		21	/* system readi/writei */
#define DWEMNTERR	22	/* mount error of some  kind  */
#define DWEACCESS	23	/* access work entry */
#define DWEISSRESP	24	/* service environment response */
#define DWESVCXIT	25	/* service environment exit resuest */



/*
 *  Dfs work entry status valuse.
 */

#define DFSOK		0       /* request completed ok */
#define	DNOPRO		1	/* no dfs processes available */
#define DFAIL		2	/* failed dfs request */
#define DNOMNTTAB	3	/* no mount table entry */
#define DMOUNTED	4	/* already mounted on */
#define DDOTDOT		5	/* can not find dotdot */
#define DNONATTACH	6	/* can not nattach */
#define DNOOPEN		7	/* can not open */
#define DBADMNT		8	/* bad mount response */
#define DNOPROC		9	/* no dfs process entries available */


/* dfshdr type values */

#define DFSMNTRQ	1	/* dfs mount request msg type */
#define DFSNMIRQ	2	/* dfs namei request msg type */
#define	DFSOPNRQ	3	/* remote open file request */
#define DFSCLSRQ	4	/* remote close file request */
#define DFSIPRQ		5	/* remote iput file request */
#define DFSPRERQ	6	/* remote prele file request */
#define DFSSISRQ	7	/* sister process request */
#define DFSRDRQ		8	/* read request */
#define DFSWRRQ		9	/* write request */
#define DFSCMRQ		10	/* chmod request */
#define DFSS1RQ		11	/* stat1 request */
#define DFSMKNRQ	12	/* maknode request */
#define DFSITRQ		13	/* itrunc request */
#define DFSCORQ		14	/* chown request */
#define DFSICRQ		15	/* icount request */
#define DFSPLCKRQ	16	/* plock request */
#define DFSFCRQ		17	/* fcntl request */
#define DFSSKRQ		18	/* seek request */
#define DFSUTRQ		19	/* utime request */
#define DFSUNLRQ	20	/* unlink request */
#define DFSLNKRQ	21	/* link request */
#define DFSIOCRQ	22	/* ioctl request */
#define DFSLCKRQ	23	/* locking request */
#define DFSACCRQ	24	/* access request */


