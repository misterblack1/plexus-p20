/*  */
/* @(#)pwd.h	5.1 4/22/86 */

/*	@(#)pwd.h	1.1	*/
/*	3.0 SID #	1.2	*/
struct passwd {
	char	*pw_name;
	char	*pw_passwd;
	int	pw_uid;
	int	pw_gid;
	char	*pw_age;
	char	*pw_comment;
	char	*pw_gecos;
	char	*pw_dir;
	char	*pw_shell;
};

struct comment {
	char	*c_dept;
	char	*c_name;
	char	*c_acct;
	char	*c_bin;
};
