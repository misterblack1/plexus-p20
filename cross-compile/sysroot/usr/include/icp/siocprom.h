
/* SID @(#)siocprom.h	5.1 */
/* @(#)siocprom.h	1.0 11/14/84 */

/* command structure */

struct cmd {
	ushort	c_gate;
	ushort	c_command;
	ushort	c_flags;
	ushort	c_char;
	long	c_mbadx;
	ushort	c_lcladx;
	short	c_len;
};

/* command types */

#define	WBLK	0		/* write block from mb to sioc */
#define	RBLK	1		/* write block from sioc to mb */
#define	JMP	2		/* set sioc pc */
#define	WCHAR	3		/* write character to console */
#define	RCHAR	4		/* read character from console */
#define CRDY	5		/* charrdy function in the prom */
#define MEMPRMS 6		/* memory parameters, min max */
#define DBUGON	7		/* debug on in CPU */
#define DBUGOFF 8		/* debug off in CPU */

/* diagnostic commands */

#define DVT_INIT	0x40	/* init diagnostic virtual terminal */
#define DVT_OFF  	0x41	/* return to normal mode */
#define DVT_RCHAR	0x42	/* output char to diagnostic virtual terminal */
#define DVT_WCHAR	0x43	/* get char from diagnostic virtual terminal */
#define DVT_SLFTST	0x44	/* start selftest */
#define DVT_RESULTS	0x45	/* return selftest results */
#define DVT_NOP		0x46	/* do nothing */
#define DVT_REV		0x47	/* return revision of board */

	/*------------------------------------------------------*\
	| Note that LA_SYSIO is defined as 0x7ff000 which is the |
	| system i/o page (defined in m68.h). SIOCBUF starts at  |
	| 0x7f0000. This means that the maximum size of SIOCBUF  |
	| is limited to 0xe000 (57344) = 0x7ff000 - 0x7f0000	 |
	\*------------------------------------------------------*/

#define BUFSIZE 0xe000

#define SIOCBUF 0x7f0000	/* hardwired buffer area */
