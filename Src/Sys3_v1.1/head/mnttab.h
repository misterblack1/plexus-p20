#define	NMOUNT	20

/* Format of the /etc/mnttab file which is set by the mount(1m)
 * command
 */
struct mnttab {
	char	mt_dev[10],
		mt_filsys[10];
		short	mt_ro_flg;
	time_t	mt_time;
};
