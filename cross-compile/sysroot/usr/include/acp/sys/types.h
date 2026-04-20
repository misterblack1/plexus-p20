/* SID */
/* @(#)types.h	5.1 4/22/86 */

typedef	struct { int r[1]; } *	physadr;
typedef	long		daddr_t;
typedef	char *		caddr_t;
typedef	unsigned short	ushort;
typedef	ushort		ino_t;
typedef short		cnt_t;
typedef	long		time_t;
typedef	int		label_t[13];	/* a2-a7, d2-d7, & pc */
typedef	short		dev_t;
typedef	long		off_t;
typedef	long		paddr_t;
