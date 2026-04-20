/* SID @(#)utsname.h	5.1 */
/* @(#)utsname.h	6.1 */
struct utsname {
	char	sysname[9];
	char	nodename[9];
	char	release[9];
	char	version[9];
	char	machine[9];
};
extern struct utsname utsname;
