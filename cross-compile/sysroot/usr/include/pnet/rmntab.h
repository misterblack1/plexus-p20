/* SID */
/* @(#)rmntab.h	5.1 4/22/86 */

/*
 * Remote mount table.
 */

struct	rmntab	{
	unsigned int	mt_rvc;		/* read mount virtual circuit */
	unsigned int	mt_wvc;		/* write mount virtual circuit */
	unsigned	mt_rw;		/* read only or read/write mode */
	time_t	mt_time;		/* time of remote mount */
	struct	inode	*mt_inode;	/* inode pointer for mounted dir. */
	char	mt_rnode[9];		/* remote node of this rmount */
	char	mt_dir[DFSMAXP];	/* mounted directory name */
	char	mt_rdir[DFSMAXP]; 	/* remote directory of rmount */
};

extern struct rmntab rmntab[];

