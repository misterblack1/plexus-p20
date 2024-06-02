/*
 * To recompile gettytab.c with getty.o:
 *	cp getty.osave getty.o
 *	cc -n -O gettytab.c -o getty getty.o
 * To install the new getty:
 *	mv /etc/getty /etc/OLDgetty
 *	mv getty /etc/getty
 *	chmod 744 /etc/getty
 *	chgrp sys /etc/getty
 *	chown root /etc/getty
 */	
#include <termio.h>
struct	tab {
	char	tname;		/* this table name */
	char	nname;		/* successor table name */
	unsigned short	iiflag;		/* initial input mode */
	unsigned short	ioflag;		/* initial output mode */
	unsigned short	icflag;		/* initial control mode */
	unsigned short	ilflag;		/* initial local mode */
	unsigned short	fiflag;		/* final input mode */
	unsigned short	foflag;		/* final output mode */
	unsigned short	fcflag;		/* final control mode */
	unsigned short	flflag;		/* final local mode */
	char	line;			/* line discipline */
	char	*message;	/* login message */
} uitab[] = {
/* PROTOTYPE VALUES FOR '0', '3', AND 'b' SPEED TABLES 
 *
 * TABLE '0'-1-2-'3' 300,150,110,1200:
 *
 *	'0', 1,
 *	0, ONLRET|CR1|OPOST,
 *	B300|CREAD|CS8, ECHOK,
 *	BRKINT|IGNPAR|ISTRIP|IXON|IXANY, ONLRET|CR1|OPOST,
 *	B300|CREAD|CS8, ECHO|ECHOK|ICANON|ISIG,
 *	0,
 *	"\n\r\033;\007login: ",
 *
 *	1, 2,
 *	0, ONLRET|CR1|OPOST,
 *	B150|CREAD|CS8, ECHOK,
 *	BRKINT|IGNPAR|ISTRIP|IXON|IXANY|INPCK, ONLRET|CR1|OPOST|TAB1|FFDLY,
 *	B150|CREAD|CS7|PARENB, ECHO|ECHOK|ICANON|ISIG,
 *	0,
 *	"\n\r\033:\006\006\017login: ",
 *
 *	2, '3',
 *	0, ONLRET|CR1|OPOST,
 *	B110|CREAD|CS8|CSTOPB, ECHOK,
 *	BRKINT|IGNPAR|ISTRIP|IXON|IXANY|ICRNL|IUCLC, ONLCR|CR1|OPOST|OLCUC|TAB3,
 *	B110|CREAD|CS7|PARENB|CSTOPB, ECHO|ECHOK|ICANON|ISIG|XCASE,
 *	0,
 *	"\n\rlogin: ",
 *
 * TABLE '3'-'0' -- 1200,300:
 *	'3', '0',
 *	0, ONLRET|CR1|OPOST,
 *	B1200|CREAD|CS8, ECHOK,
 *	BRKINT|IGNPAR|ISTRIP|IXON|IXANY|ICRNL, ONLCR|OPOST|TAB3,
 *	B1200|CREAD|CS8, ECHO|ECHOK|ICANON|ISIG,
 *	0,
 *	"\n\rlogin: ",
 */

 /* TABLE 'b' -- matches switch setting */
 /* Included ONLY to get gettytab to compile */
 	'b', 'b',
 	0, ONLCR|CR1|OPOST,
 	EXTB|CREAD|CS8, ECHOK,
 	BRKINT|IGNPAR|ISTRIP|IXON|ICRNL, ONLCR|OPOST|TAB3,
 	EXTB|CREAD|CS8|HUPCL, ECHO|ECHOK|ICANON|ISIG,
 	0,
 	"\nlogin: ",
	};
int	n_uitab = sizeof uitab/sizeof uitab[0];
