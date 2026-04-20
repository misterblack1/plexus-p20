/* SID */
/* @(#)utsname.h	5.1 4/22/86 */

struct utsname {
	char	sysname[9];
	char	release[9];
	char	version[9];
};
extern struct utsname utsname;
