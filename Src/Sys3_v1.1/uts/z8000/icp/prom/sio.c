/*
 * serial port handling
 */

#include "/p3/usr/include/icp/sioc.h"	/* icp specific */
#include "/p3/usr/include/icp/ctc.h"	/* icp specific */
#include "/p3/usr/include/icp/sio.h"	/* icp specific */

struct {
	int	timeconst;
	int	basewr4;
} speedtab[] = {
	175, CLK32X,    /* 110 baud */
	64, CLK32X,	/* 300 baud */
	32, CLK32X,	/* 600 baud */
	16, CLK32X,	/* 1200 baud */
	16, CLK16X,	/* 2400 baud */
	8, CLK16X,	/* 4800 baud */
	4, CLK16X,	/* 9600 baud */
	2, CLK16X,	/* 19200 baud */
};


/*
 * initialize console for non interrupt i/o
 */

initsio( speed )
register	speed;
{

	out_local(PSIO0CTC, CCTR | CTIME | CRESET | CCTRL);
	out_local(PSIO0CTC, speedtab[speed].timeconst);
	out_local(PSIO0CMD, RESETEXT | WR4);
	out_local(PSIO0CMD, speedtab[speed].basewr4 | SB1);
	out_local(PSIO0CMD, RESETEXT | WR5);
	out_local(PSIO0CMD, DTR | TBITS8 | TEN | RTS);
	out_local(PSIO0CMD, RESETEXT | WR3);
	out_local(PSIO0CMD, RBITS8 | REN);
	out_local(PSIO0CMD, RESETEXT | WR1);
	out_local(PSIO0CMD, 0);
}


/*
 * put a character out
 */

putchar(c)
register	c;
{
	register	timo;

	timo = 30000;
	while ((in_local(PSIO0CMD) & TXRDY) == 0) {
		if (--timo == 0) {
			break;
		}
	}
	if (c == 0) {
		return;
	}
	out_local(PSIO0DATA, c);
	if (c == '\n') {
		putchar('\r');
	}
	putchar(0);	/* wait for last character to complete */
}


/*
 * get a character
 */

getchar()
{
	register c;

	while ((in_local(PSIO0CMD) & RXRDY) == 0) {
	}
	c = in_local(PSIO0DATA) & 0177;
	return(c);
}
