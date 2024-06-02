
#include "sys/reg.h"
#include "sys/param.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "icp/proc.h"
#include "icp/sioc.h"
#include "icp/sio.h"
#include "icp/sioccomm.h"
#include "icp/icp.h"
#include "icp/pbsioc.h"
#define	MAXREGS	18
#define	MAXBP	30
#define	BYADDRESS 0
#define	T_DBG	12
#define T_EI	0
#define NFMT	4		/* # of formats */

extern	char	mbiin, mbiout, pborun, pback;
/*
 * Offsets of the user's registers relative to
 * the saved r0. See reg.h
 */
char	uregs[MAXREGS] =
{
	R0, R1, R2, R3, R4, R5, R6, R7,
	R8+1, R9+1, R10+1, R11+1, R12+1, R13+1, R14+1, R15-1,
	FCW, PC,
};

unsigned char  *address = 0;
char	foundint;
char	formatset[NFMT];
int	fmt=0;		/* currently active format */
char cmd[80];
char *cmdptr;
struct bpstruct {
	unsigned adx1;
	unsigned val1;
	unsigned adx2;
	unsigned val2;
} bp[MAXBP];

struct siocmem {
	int	s_mem[32000];
};

struct	siocmem	*mp;

int *regp;
int j;
int sadx;
int	dlen;
int cnt;
int offs;
char	indebug;

/*
 * trap handler for non vectored interrupt which is used to get into the 
 * resident debugger.
 */

sysdebug(dev, r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
unsigned pc;
{
	register int	i;
	register int len;
	register int junk1,junk2,junk3,junk4; /* force 6 registers onto stack */
	unsigned char *putpat();
	unsigned char *nullptr;

	indebug = 1;
	for (j = 0;j < 10000; j++);	/* wait for vt100 buffer to catch up */
	mp = (struct siocmem *)0x0;
	if (dev == T_DBG) {
		printf("\n*** siocdebug call ***\n\n");
	} else if (dev == T_EI) {
		pc -= 2;
		for (i = 0; i < MAXBP; i++) {
			if (pc == bp[i].adx1 || pc == bp[i].adx2) {
				break;
			}
		}
		if (i == MAXBP) {
			printf("\n*** siocdebug invalid breakpoint ***\n\n");
		} else if (pc == bp[i].adx1) {
			printf("\n*** siocdebug breakpoint at ");
			puthex(pc);
			printf(" ***\n\n");
			if (bp[i].adx2) {
				mp->s_mem[(bp[i].adx1/2)] = bp[i].val1;
				mp->s_mem[(bp[i].adx2/2)] = 0x8e00;
			} else {
				delbp(pc);
			}
		} else {
			mp->s_mem[(bp[i].adx2/2)] = bp[i].val2;
			mp->s_mem[(bp[i].adx1/2)] = 0x8e00;
			goto out;
		}
	} else {
		printf("\n*** siocdebug for unknown reason ***\n\n");
	}
	while (1) {
		printf("+ ");
		gets(cmd);
		cmdptr = cmd + 1;
		switch (cmd[0]) {

		/* stack trace */
		case 'f':
			regp = &r0;
			sadx = regp[R14+1];
			offs = 0;
			len = 0x20;
			goto dol;
			break;

		/* link list search and display */
		case 'l':
			sadx = gethex();
			offs = gethex();
			len = gethex();

			dol:
			while (sadx) {
				if ( (sadx + len) > 0xc000 )
					dlen = (0xc000 - sadx);
				else dlen = len;
				if (display(sadx + offs, dlen)) {
					break;
				}
				printf("\n");
				sadx = mp->s_mem[sadx/2];
			}
			break;

		/* read memory and display */
		case 'r':
			sadx = gethex();
			len = gethex();
			display(sadx, len);
			break;

		/* exit sioc debug */
		case 'e':
			goto out;
			break;

		/* register display */
		case 'x':
			puthex(2 + &pc);
			printf("\n");
			regp = &r0;
			for (i = 0; i < MAXREGS; i++) {
				puthex(regp[uregs[i]]);
				if ((i == 7) || (i == 15) || (i == 17)) {
					printf("\n");
				} else {
					printf(" ");
				}
			}
			break;

		/* display pc */	
		case 'p':
			pc = 0;
			goto out;
			break;

		/* display table entries */
		case 't':
			sadx = gethex();
			len = gethex();
			cnt = gethex();
			while (cnt--) {
				if (display(sadx, len)) {
					break;
				}
				printf("\n");
				sadx += len;
			}
			break;

		/* set break point */
		case 'b':
			if (eol()) {
				for (i = 0; i < MAXBP; i++) {
					if (bp[i].adx1) {
						puthex(bp[i].adx1);
						printf(" ");
						puthex(bp[i].adx2);
						printf("\n");
					}
				}
			} else {
				for (i = 0; i < MAXBP; i ++) {
					if (bp[i].adx1 == 0) {
						break;
					}
				}
				if (i == MAXBP) {
					printf("no more breakpoints\n");
				} else {
					bp[i].adx1 = gethex();
					bp[i].adx2 = gethex();
					bp[i].val1 = mp->s_mem[(bp[i].adx1/2)];
					if ( bp[i].adx2 )
						bp[i].val2 = mp->s_mem[(bp[i].adx2/2)];
					mp->s_mem[(bp[i].adx1/2)] = 0x8e00;
				}
			}
			break;

		/* delete breakpoint */
		case 'd':
			if (eol()) {
				for (i = 0; i < MAXBP; i++) {
					if (bp[i].adx1) {
						delbp(bp[i].adx1);
					}
				}
			} else {
				sadx = gethex();
				delbp(sadx);
			}
			break;

#ifndef SMALL
		/* print out current address. do not advance */
		case '.':
			sadx =  gethex();
			if(foundint) {
			    if((sadx<0) || (sadx>NFMT)) sadx = 0;
			    if(!formatset[sadx]) break;
			    fmt = sadx;
			}
			/* fmt passed as global arg */
			nullptr = putpat(address);
			break;

		/* set printout format */
		case 's':
			fmt = *cmdptr - '0';
			if((fmt<0) || (fmt>NFMT)) fmt = 0;
			else cmdptr++;
			makepat();	/* fmt passed as global arg */
			formatset[fmt]++;
			break;

		/* on new line print out current address then advance */
		case '\0':
			if(!formatset[fmt]) break;
			/* fmt passed as global arg */
			address = putpat(address);
			break;

		/* print current address in hex */
		case 'a':
			puthex(address);
			printf("\n");
			break;
#endif
		/* ignore everything except single hex # */
		default:
			cmdptr--;
			sadx = gethex();
			if(foundint) address = (unsigned char *) sadx;
			break;
		}
	}

	out:
	indebug = 0;
	return(0);
}


/*
 * get a string from the debug terminal
 */

gets(bufp)
char *bufp;
{
	register char *lp;
	register int c;

	lp = bufp;
	while (1) {
		c = getchar() & 0177;
		switch(c) {

		case '\n':
		case '\r':
			c = '\n';
			*lp++ = '\0';
			return;
			break;

		case '\b':
			lp--;
			if(lp < bufp) {
				lp = bufp;
			}
			break;
			
		case '\030':
			lp = bufp;
			putchar('\n');
			break;
		default:
			*lp++ = c;
			break;
		}
	}
}

	/*--------------------------------------------------------*\
	| get next character from buffer. If buffer becomes empty, |
	| get another buffer.					   |
	\*--------------------------------------------------------*/

nextchar()
{
    register int c;

    if((c = *cmdptr++) != '\0') return c;

    c = '\n';

    gets(cmd);
    cmdptr = cmd;

    return c;

}


/*
 * prints an integer value in hex, leading digits 0 pad.
 */

puthex(value)
unsigned value;
{
	static char ptab[] = "0123456789abcdef";
	char buf[5];
	register int i;

	buf[4] = '\0';
	for (i = 3; i >= 0; i--) {
		buf[i] = ptab[value & 0xf];
		value = value >> 4;
	}
	printf("%s", buf + i + 1);
}


/*
 * get the next number from cmd starting at cmdptr. Ignores leading
 * blanks and interprets the number in hex.
 */

gethex()
{
	register c;
	register int total;
	register int sign;

	while (*cmdptr == ' ') {
		cmdptr++;
	}
	foundint = 0;
	sign = 1;
	total = 0;
	while (1) {
		c = *cmdptr;
		if ((c >= '0') && (c <= '9')) {
			total = (total << 4) + c - '0';
		} else if ((c >= 'a') && (c <= 'f')) {
			total = (total << 4) + c - 'a' + 10;
		} else if (c == '-') {
			sign = -1;
		} else {
			break;
		}
		cmdptr++;
		foundint++;
	}
	return(sign * total);
}

/*
 * returns non zero if there are no more parameters in the cmd string.
 */

eol()
{
	while (*cmdptr == ' ') {
		cmdptr++;
	}
	if (*cmdptr == '\0') {
		return(1);
	} else {
		return(0);
	}
}


/*
 * displays memory starting at adx for len.
 * If len is zero it means go forever.
 */

#if BYADDRESS == 1
display(ad, le)
unsigned ad;
unsigned le;
{
	long inx;
	long adx;
	long len;

	inx = ad & 0xfff0;
	adx = (long) ad;
	len = (le == 0) ? ((long)0xc00 - adx) : (long) le;
	if ( (adx + len) > (long)0xc000 )
		len = (long)0xc000 - adx;
	while (1) {
		if ((inx % 16) == 0) {
			if (checkabort()) {
				return(1);
			}
			if (inx >= (adx + len)) {
				return(0);
			}
			puthex((unsigned) inx);
			printf(":");
		}
		if (inx < adx || inx >= (adx + len)) {
			printf("     ");
		} else {
			printf(" ");
			puthex( mp->s_mem[((unsigned) inx/2)] );
		}
		if ((inx % 16) == 14) {
			printf("\n");
		}
		inx += 2;
	}
}
#else
display(ad, le)
unsigned ad;
unsigned le;
{
	long inx;
	long adx;
	long len;

	inx = 0;
	adx = (long) ad;
	len = (le == 0) ? 100000 : (long) le;
	while (inx < len) {
		if ((inx % 16) == 0) {
			if (checkabort()) {
				return(1);
			}
			puthex((unsigned) adx);
			printf(":");
		}
		printf(" ");
		puthex( mp->s_mem[((unsigned) adx/2)] );
		if (inx >= (len - 2) || (inx % 16) == 14) {
			printf("\n");
		}
		inx += 2;
		adx += 2;
	}
	return(0);
}
#endif


/*
 * check for suspend or abort. If suspend just stay in this routine.
 * If abort then return true.
 */

checkabort()
{
	register int c;

	if (!charrdy()) {
		return(0);
	}
	c = getchar();
	if (c == '\023') {
		c = getchar();
	}
	if (c == '\177') {
		return(1);
	} else {
		return(0);
	}
}

/*
 * delete a breakpoint and repair the code.
 */

delbp(adx)
unsigned adx;
{
	register int i;
	register int saveudseg;

	for (i = 0; i < MAXBP; i++) {
		if (bp[i].adx1 == adx) {
			mp->s_mem[(bp[i].adx1/2)] = bp[i].val1;
			mp->s_mem[(bp[i].adx2/2)] = bp[i].val2;
			bp[i].adx1 = bp[i].adx2 = 0;
			bp[i].val1 = bp[i].val2 = 0;
		}
	}
}

charrdy()
{

	if ( (in_local(PSIO0CMD) & RXRDY) == 0 )
		return(0);
	else return(1);

}

/*
 * get a character.
 */

getchar()
{
	register c;

	while((in_local(PSIO0CMD) & RXRDY) == 0 ) {
	}

	c = in_local(PSIO0DATA) & 0177;
	if ( c == '\r' ) {
		c = '\n';
	}
	putchar(c);
	return(c);
}

/* 
 * Debug daemon.  Makes sure that activity is taking place in the SIOC.
 */

int	debugit;
long	debugc;
long	mycount;
long	dbtcalled;
long	dbtret;

godebug()
{
	wakeup( (caddr_t) &debugit );
}

debugger()
{

	mycount = 0;
	for ( ; ; )  {
		dbtcalled++;
#ifdef VPMSYS
		timeout( godebug, 0, 12000, 2098);
#else
		timeout( godebug, 0, 12000 );
#endif
		sleep( (caddr_t)&debugit , PUSER );
		dbtret++;
		if ( mycount >= debugc ) {
			if ( !indebug ) {
				debug();
			}
		}
		mycount = debugc;
	}
}

/*
checker()
{
	if ( !pborun ) {
		if ( pback  || pctl.p_oq.pq_cnt > 0 ) {
			printf( "OQ - not running.\n" );
			degug();
		}
	}
	if ( !mbrun ) {
		if ( mbiout || mbiin || mbq.mb_cnt > 0 ) {
			printf( "MB - not running!\n" );
			debug();
		}
	}
#ifdef VPMSYS
	timeout( checker, 0, 200, 2099 );
#else
	timeout( checker, 0, 200 );
#endif
}
 */

#ifndef SMALL
char format[NFMT][128];
#define STRING 01
makepat()
{
    register char *fp;
    register int c;

    fp = format[fmt];
    while((c=nextchar()) == ' ');
    for( ; c != '.'; c=nextchar()) {
	switch(c) {

	case 'o':		/* octal short word */
	case 'O':		/* octal long word */
	case 'd':		/* decimal short word */
	case 'D':		/* decimal long word */
	case 'x':		/* hex short word */
	case 'X':		/* hex long word */
	case 'c':		/* byte as char */
	case 'b':		/* byte in octal */
	case 'B':		/* byte in hex */
	case 'n':		/* newline */
	case 's':		/* "C" string */
	case 'S':		/* "C" string with conversions */
	case 'A':		/* current address in hex */
	case 'a':		/* current address in octal */
	case 'r':		/* space */
	case 't':		/* tab */

	    *fp++ = c;
	    break;

	case '"':		/* start of string */

	    *fp++ = STRING;
	    while((c=nextchar()) != '"') {
		if(c == '\n') continue;		/* ignore new lines */
		if(c == '\\') {
		    *fp++ = nextchar();
		    continue;
		}
		*fp++ = c;
	    }
	    *fp++ = '\0';			/* append EOS at end */
	    break;

	case '\\':

	    c = nextchar();
	    *fp++ = c;
	    break;

	default:
	    *fp++ = c;

	}

    }

    *fp++ = '.';		/* use period as end of format switch */

}


unsigned char *putpat(dot)
register unsigned char *dot;
{
    register char *fp;
    register int c;
    register unsigned int val;

    fp = format[fmt];
    while((c = *fp++) != '.') {

	switch(c) {

	case 'o':
	    val = *dot++;
	    val = *dot++ + 256*val;
	    printf("%o",val);
	    break;

	case 'd':
	    val= *dot++;
	    val = *dot++ + 256*val;
	    printf("%d",val);
	    break;

	case 'x':
	    val= *dot++;
	    val = *dot++ + 256*val;
	    printf("%x",val);
	    break;

	case 'b':
	    val= *dot++;
	    printf("%b",val);
	    break;

	case STRING:
	    while((c = *fp++) != '\0') putchar(c);
	    break;

	case 's':
	    while((c = *dot++) != '\0') putchar(c);
	    break;

	case 'c':
	    putchar(*dot++);
	    break;

	case 'S':
	    while((c = *dot++) != '\0')
		if(c < ' ') printf("^%c",c+'A'-1);
		else if(c & 0200) printf("\\%c",c);
		else putchar(c);
	    break;

	case 'C':
	    c = *dot++;
	    if(c < ' ') printf("^%c",c+'A'-1);
	    else if(c & 0200) printf("\\%c",c);
	    else putchar(c);
	    break;

	case 'n':
	    putchar('\n');
	    break;

	case 't':
	    putchar('\t');
	    break;

	case 'r':
	    putchar(' ');
	    break;

	case 'A':
	    printf("(%x)",dot);
	    break;

	case 'a':
	    printf("(0%o)",dot);
	    break;

	default:
	    putchar(c);
	    break;

	}
    }
    return dot;
}
#endif


printf(fmt,args)
char fmt[];
int args;
{
    register int *ap;
    register c,x;
    char *s;

    ap = &args;
    while(1) {
	while((c = *fmt++) != '%') {
	    if(c==0)
		return;
	    if(c == '\\') {
		if((c = *fmt++) == 'n') {
		    putchar(10);
		    continue;
		}
		else if(c == 't') {
		    putchar(9);
		    continue;
		}
		else {
		    putchar(c);
		    continue;
		}
	    }
	    putchar(c);
	}
	if((c = *fmt++) == 'd') {			/* format is decimal */
	    x = *ap++;
	    if(x<0) {			/* x is - infinity */
		x = -x;
		if(x < 0) {
		    printf("-32768");
		    continue;
		}
		putchar('-');
	    }
	    printd(x);
	}
	else if(c == 'o') 			/* format is octal */
	    printo(*ap++);
	else if(c == 'c') 			/* format is character */
	    putchar(*ap++);
	else if(c == 's') {			/* format is string */
	    s = *ap++;
	    while(c = *s++)
		putchar(c);
	}
	else if(c == 'x')			/* format is hex */
	    printx(*ap++);
	else if(c == 'b')			/* format is byte */
	    printx((*ap++)&0xff);
	else
	    putchar(c);
    }
}

printd(n)
int n;
{
    int a;
    if(a = n/10)
	printd(a);
    putchar(n%10 + '0');
}

printo(n)
int n;
{
    int a;
    if(a = ((n>>3) & 017777))
	printo(a);
    putchar((n&07) + '0');
}

printx(n)
int n;
{
    int a;
    if(a = (n>>4)&0xfff)
	printx(a);

    if((n&0xf) > 9)
	putchar((n&0xf) +'A' - 10);
    else
	putchar((n&0xf) + '0');
}

#ifdef TRACE
	/*---------------------------------------------------------*\
	| trace function. Holds last 100 events of interest. Format |
	| depends upon call. Use as trace(count,arg1,arg2,.. argn)  |
	\*---------------------------------------------------------*/

int trec[75][12];
int tindex;

trace(cnt,arg)
int cnt,arg;
{
    int num,*ptr,i;

    ptr = &arg;
    num = min(12,cnt);

    for(i=0; i<12; i++) trec[tindex][i] = 0;
    for(i=0; i<num; i++) trec[tindex][i] = *ptr++;

    if(++tindex >= 75) tindex = 0;

}
#endif
