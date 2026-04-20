/* SID @(#)stermio.h	5.1 */
/* <@(#)stermio.h	6.1> */
/*
 * ioctl commands for control channels
 */
#define STSTART		1	/* start protocol */
#define STHALT		2	/* cease protocol */
#define STPRINT		3	/* assign device to printer */
#define STENABLE	4	/* enable polling */
#define STDISABLE	5	/* disable polling */
#define STPOLL		6	/* set polling rate */
#define STCNTRS		7	/* poke for status reports */
#define STTCHAN		8	/* set trace channel number */

/*
 * ioctl commands for terminal and printer channels
 */
#define STGET	(('X'<<8)|0)	/* get line options */
#define STSET	(('X'<<8)|1)	/* set line options */
#define	STTHROW	(('X'<<8)|2)	/* throw away queued input */
#define	STWLINE	(('X'<<8)|3)	/* get synchronous line # */
#define STTSV	(('X'<<8)|4)	/* get all line information */

struct stio {
	unsigned short	ttyid;
	char		row;
	char		col;
	char		orow;
	char		ocol;
	char		tab;
	char		aid;
	char		ss1;
	char		ss2;
	unsigned short	imode;
	unsigned short	lmode;
	unsigned short	omode;
};

/*
**	Mode Definitions.
*/
#define	STFLUSH	00400	/* FLUSH mode; lmode */
#define	STWRAP	01000	/* WRAP mode; lmode */
#define	STAPPL	02000	/* APPLICATION mode; lmode */

struct sttsv {
	char	st_major;
	short	st_pcdnum;
	char	st_devaddr;
	int	st_csidev;
};

struct stcntrs {
	char	st_lrc;
	char	st_xnaks;
	char	st_rnaks;
	char	st_xwaks;
	char	st_rwaks;
	char	st_scc;
};

/* trace message definitions */

#define LOC	113	/* loss of carrier */
