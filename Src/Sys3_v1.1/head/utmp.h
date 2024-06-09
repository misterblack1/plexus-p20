/*
 * Format of /etc/utmp and /usr/adm/wtmp
 */

struct utmp {
	char	ut_line[8];		/* tty name */
	char	ut_name[8];		/* user id */
	long	ut_time;		/* time on */
};
