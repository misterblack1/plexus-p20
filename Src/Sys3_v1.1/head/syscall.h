/*
 * These numbers are the parameters used to indicate which system call
 * the sc xx instruction is invoking.
 */

#define	INDIR	0
#define	EXIT	1
#define	FORK	2
#define	READ	3
#define	WRITE	4
#define	OPEN	5
#define	CLOSE	6
#define	WAIT	7
#define	CREAT	8
#define	LINK	9
#define	UNLINK	10
#define	EXEC	11
#define	CHDIR	12
#define	TIME	13
#define	MKNOD	14
#define	CHMOD	15
#define	CHOWN	16
#define	BREAK	17
#define	STAT	18
#define	LSEEK	19
#define	GETPID	20
#define	MOUNT	21
#define	UMOUNT	22
#define	SETUID	23
#define	GETUID	24
#define	STIME	25
#define	PTRACE	26
#define	ALARM	27
#define	FSTAT	28
#define	PAUSE	29
#define	UTIME	30
#define	STTY	31
#define	GTTY	32
#define	ACCESS	33
#define	NICE	34
/* 		35	Version 7: FTIME */
#define	SYNC	36
#define	KILL	37
#define	CSW	38	/* Not in Weco R-III */
#define	SETPGRP	39
#define	DUP	41
#define	PIPE	42
#define	TIMES	43
#define	PROFIL	44
#define	LOCKING	45	/* Not in Weco R-III */
#define	SETGID	46
#define	GETGID	47
#define	SIGNAL	48
#define	ACCT	51
/*		52	Version 7: PHYS	*/
/*		53	Version 7: LOCK	 */
#define	IOCTL	54
#define	REBOOT	55	/* Not in Weco R-III */
#define FCNTL	56	/* Version 7: MPX */
#define PWBSYS	57	/* UNAME and USTAT sys calls.  V7: undefined */
#define	EXECE	59
#define	UMASK	60
#define	CHROOT	61
#define UGROW	62	/* R-III: FCNTL */
#define ULIMIT	63	/* Version 7: undefined */
