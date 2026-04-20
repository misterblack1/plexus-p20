/* SID @(#)mount.h	5.1 */
/* @(#)mount.h	6.1 */
/*
 * Mount structure.
 * One allocated on every mount.
 */
struct	mount
{
	int	m_flags;	/* status */
	dev_t	m_dev;		/* device mounted */
	struct	dqp *m_dpro;	/* remote mount dfs process id */
	char	m_dir[50];	/* directory mounted on */
	char	m_node[10];	/* node where this directory mounted */
	struct	inode *m_inodp;	/* pointer to mounted on inode */
	struct	buf *m_bufp;	/* buffer for super block */
	struct	inode *m_mount;	/* pointer to mount root inode */
};

#define	MFREE	0
#define	MINUSE	1
#define	MINTER	2
#define	MREMOTE	4

extern struct mount mount[];
