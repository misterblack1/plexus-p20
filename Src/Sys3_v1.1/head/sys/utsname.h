struct utsname {
	char	sysname[9];
	char	nodename[9];
	char	release[9];
	char	version[9];
};
extern struct utsname utsname;
