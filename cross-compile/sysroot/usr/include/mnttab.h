/*  */
/* @(#)mnttab.h	5.1 4/22/86 */

/*	@(#)mnttab.h	1.2	*/
/*	3.0 SID #	1.1	*/
#define	NMOUNT	40
#define	MNTPATH	50

/* Format of the /etc/mnttab file which is set by the mount(1m)
 * command
 */
struct mnttab {
	char	mt_dev[MNTPATH],
		mt_node[10],
		mt_filsys[MNTPATH];
	short	mt_ro_flg;
	time_t	mt_time;
};
