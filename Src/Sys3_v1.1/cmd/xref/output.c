#include	"output.h"


#define MAXLIN	256

TYPE L_INT	EXPR;
INT		mkfault;
INT		outfile = 1;
CHAR		*digitptr;
CHAR		printbuf[MAXLIN];
CHAR		*printptr = printbuf;

printc(c)
	CHAR		c;
{
	REG STRING	q;
	CHAR		d;
	INT		posn, tabs, p;

	IF mkfault
	THEN	return;
	ELIF (*printptr=c)==NL
	THEN	tabs=0; posn=0; q=printbuf;
		FOR p=0; p<printptr-printbuf; p++
		DO	d=printbuf[p];
			IF (p&7)==0 ANDF posn>1
			THEN	tabs++; posn=0;
			FI
			IF d==SP
			THEN	posn++;
			ELSE	WHILE tabs>0 DO *q++=TB; tabs--; OD
				WHILE posn>0 DO *q++=SP; posn--; OD
				*q++=d;
			FI
		OD
		*q++=NL;
		write(outfile,printbuf,q-printbuf);
		printptr=printbuf;
	ELIF c==TB
	THEN	*printptr++=SP;
		WHILE (printptr-printbuf)&7 DO *printptr++=SP; OD
	ELIF c
	THEN	printptr++;
	FI
}

print(format, value)
	STRING		format;
	PRINTF		value;
{
	STRING		scan = format;
	STRING		s;
	STRING		ecvt();
	INT		width, prec;
	CHAR		c, adj;
	INT		decpt, n;
	CHAR		digits[64];
	EXPR		e = value.p_expr;
	POS		i = value.p_int;

	WHILE c = *scan++
	DO IF c!='%'
	   THEN	printc(c);
	   ELSE	IF (adj = *scan)=='-' THEN scan++; FI
		width=convert(&scan);
		IF *scan=='.' THEN scan++; prec=convert(&scan); ELSE prec = -1; FI
		digitptr=digits;
		s=0;
		SWITCH c = *scan++ IN

		    case 'd':
			e=i;
		    case 'D':
			printn(e,-1,10); break;

		    case 'u':
			e=i;
		    case 'U':
			printn(e,0,10); break;

		    case 'o':
			e=i;
		    case 'O':
			printn(e,0,8); break;

		    case 'q':
			e=i;
		    case 'Q':
			printn(e,-1,8); break;

		    case 'x':
			e=i;
		    case 'e':
		    case 'X':
			printn(e,0,16); break;

		    case 'Y':
			printd(e); break;

		    case 'c':
			printc(value.p_char); break;

		    case 's':
			s=value.p_string; break;

		    case 'f':
		    case 'F':
			s=ecvt(value.p_real, prec, &decpt, &n);
			*digitptr++=(n?'-':'+');
			*digitptr++ = (decpt<=0 ? '0' : *s++);
			IF decpt>0 THEN decpt--; FI
			*digitptr++ = '.';
			WHILE *s ANDF prec-- DO *digitptr++ = *s++; OD
			WHILE *--digitptr=='0' DONE
			digitptr += (digitptr-digits>=3 ? 1 : 2);
			IF decpt
			THEN *digitptr++ = 'e'; printn((L_INT)(decpt),-1,10);
			FI
			s=0; prec = -1; break;

		    case 'm':
			break;

		    case 'M':
			width=i; break;

		    case 'T':
			width=i;

		    case 't':
			IF width
			THEN width -= charpos()%width;
			FI
			break;

		    default:
			printc(c);
		ENDSW

		IF s==0
		THEN	*digitptr=0; s=digits;
		FI
		n=length(s);
		n=(prec<n ANDF prec>=0 ? prec : n);
		width -= n;
		IF adj!='-'
		THEN	WHILE width-- > 0
			DO printc(SP); OD
		FI
		WHILE n-- DO printc(*s++); OD
		WHILE width-- > 0 DO printc(SP); OD
		digitptr=digits;
	   FI
	OD
}

LOCAL	printd(tvec)
	L_INT		tvec;
{
	REG INT		i;
	STRING		ctime();
	REG STRING	timeptr = ctime(&tvec);

	FOR i=20; i<24; i++ DO *digitptr++ = *(timeptr+i); OD
	FOR i=3; i<19; i++ DO *digitptr++ = *(timeptr+i); OD
} /*printd*/

prints(s)
	REG STRING	s;
{
	REG CHAR	c;

	WHILE c = *s++ DO printc(c); OD
}

LOCAL	convert(cp)
	REG STRING	*cp;
{
	REG CHAR	c;
	INT		n = 0;

	WHILE ((c = *(*cp)++)>='0') ANDF (c<='9') DO n=n*10+c-'0'; OD
	(*cp)--;
	return(n);
}

LOCAL	printn(n, sgn, base)
	EXPR		n;
	INT		sgn;
{
	REG INT		k;
	INT		digs[15];
	REG INT		*dptr = digs;

	IF n<0 ANDF sgn<0 THEN n = -n; *digitptr++ = '-'; FI
	IF base==8 ANDF n THEN *digitptr++='0';
	ELIF base==16 THEN *digitptr++='#';
	FI
	WHILE n
	DO *dptr++ = divide(&n,base);
	OD
	IF dptr==digs THEN *dptr++=0; FI
	WHILE dptr!=digs
	DO  k = *--dptr;
	    *digitptr++ = (k+(k<=9 ? '0' : 'a'-10));
	OD
}

charpos()
{
	return(printptr-printbuf);
}

flushbuf()
{
	IF printptr!=printbuf
	THEN	newline();
	FI
}

newline()
{
	printc(NL);
}

eqstr(s1, s2)
	REG STRING	s1, s2;
{
	REG STRING	 es1 = s1+8;

	IF *s1=='~' ORF *s1=='_'
	THEN IF *s2 != *s1 THEN s1++; FI
	FI
	WHILE *s1++ == *s2
	DO IF *s2++ == 0 ORF s1>=es1
	   THEN return(1);
	   FI
	OD
	return(0);
}
