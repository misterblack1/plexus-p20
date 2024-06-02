#define MAXDEVS 7		/* Maximum number of devices (readers, etc.) */
#define LINEFIL "/usr/rje/lines"
#define RJECU	"/dev/dn2"	/* Dial-up RJE device */
#define RJELOGIN "rje"		/* rje login name */
#define RJEUID	68            /* User-Id for "rje" */
#define SHQUID	69            /* User-Id for "shqer" */
#define QUEDIR	"/usr/rje/sque"	/* shqer directory */
#define QUELOG	"log"		/* shqer log file name */
#define QUELEN	50		/* Length of a shqer log entry */
#define QDELAY	180		/* Delay time for shqer */
#define QNICE	0
#define RESPMAX 70000		/* Max resp file size */

#define DEVFD	0	/* KMC device file descriptor */
#define XMTRD	1	/* xmit read file descriptor */
#define ERRFD	2	/* errors file descriptor */
#define XMTWR	3	/* xmit write file descriptor */
#define DSPRD	4	/* disp read file descriptor */
#define DSPWR	5	/* disp write file descriptor */
#define JBLOG	6	/* joblog file descriptor */

#define NAMESZ	8

struct joblog {
	char j_file[NAMESZ];	/* Name of file to be sent */
	unsigned j_uid;		/* User ID of owner */
	int j_lvl;		/* Message level */
	long j_cnt;		/* Number of "cards" */
};

	/* joblog header info */

struct loghdr {
	int h_pgrp;		/* Process group Id */
};
#define LBUFMAX 100
#define MAXLNS	6
struct lines {
	char *l_host;		/* RJE host machine */
	char *l_sys;		/* This system */
	char *l_dir;		/* home directory */
	char *l_prefix;		/* rje prefix */
	char *l_dev;		/* device for transfer */
	char *l_peri;		/* Peripherals field */
	char *l_parm;		/* Parameters field */
	char l_buf[LBUFMAX];	/* buffer for fields */
};

struct dsplog {
	int d_type;		/* Type of record */
	union {
		struct {	/* record from xmit */
			char d_file[NAMESZ];	/* file sent */
			long d_cnt;		/* no. of cards sent */
			unsigned d_uid;		/* who sent the file */
			int d_lvl;		/* message level */
			int d_rdr;		/* reader sent from (0-6) */
		} x;
		struct {	/* record from recv */
			char d_file[NAMESZ];	/* file received */
			long d_cnt;		/* no. of records */
			int d_trunc;		/* file truncation flag */
		} r;
	} d_un;
};

struct sque {
	char sq_exfil[140];	/* Executable file */
	char sq_infil[48];	/* Input file */
	char sq_jobnm[9];	/* Remote job name */
	char sq_pgrmr[25];	/* Programmer name */
	char sq_jobno[9];	/* Remote job number */
	char sq_login[9];	/* Login name from usr= */
	char sq_homed[48];	/* Login directory */
	long sq_min;		/* Minimum file system space */
};
