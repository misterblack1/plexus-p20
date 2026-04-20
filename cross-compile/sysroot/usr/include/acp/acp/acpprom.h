/* SID */
/* @(#)acpprom.h	5.1 4/22/86 */

/* NOTE!!!!!
	Anything changed in this file must be verified correct in
	ROOT/include/icp/siocprom.h also.  This file is used for ACP and
	the other for ICP.
 */

/* acp command structure */

struct a_cmd {
	ushort	c_gate;
	ushort	c_command;
	ushort	c_flags;
	ushort	c_char;
	long	c_mbadx;
	union {
	   struct {
		long	a_lcladx;
		short	a_len;
	   }v68k;
	   struct {
		ushort	z_lcladx;
		short	z_len;
	   }vz8k;
	} adj;
};

/* icp command structure */

struct i_cmd {
	ushort	i_gate;
	ushort	i_command;
	ushort	i_flags;
	ushort	i_char;
	long	i_mbadx;
	ushort	i_lcladx;
	short	i_len;
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
#define OPNBOOT	9		/* open for booting */
#define OPNDUMP	10		/* open for dumping */
#define CLSDUMP	11		/* close after dumping */

	/*------------------------------------------------------*\
	| Note that LA_SYSIO is defined as 0x7ff000 which is the |
	| system i/o page (defined in m68.h). SIOCBUF starts at  |
	| 0x7f0000. This means that the maximum size of SIOCBUF  |
	| is limited to 0xe000 (57344) = 0x7ff000 - 0x7f0000	 |
	\*------------------------------------------------------*/

#define BUFSIZE 0xe000
#define SIOCBUF 0x7f0000	/* hardwired buffer area */
