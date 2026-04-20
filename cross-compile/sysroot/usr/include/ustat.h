/*  */
/* @(#)ustat.h	5.1 4/22/86 */

/*	@(#)ustat.h	1.1	*/
/*	3.0 SID #	1.1	*/
struct  ustat {
	daddr_t	f_tfree;	/* total free */
	ino_t	f_tinode;	/* total inodes free */
	char	f_fname[6];	/* filsys name */
	char	f_fpack[6];	/* filsys pack name */
};
