/*  */
/* @(#)grp.h	5.1 4/22/86 */

/*	@(#)grp.h	1.1	*/
/*	3.0 SID #	1.1	*/
struct	group {	/* see getgrent(3) */
	char	*gr_name;
	char	*gr_passwd;
	int	gr_gid;
	char	**gr_mem;
};
