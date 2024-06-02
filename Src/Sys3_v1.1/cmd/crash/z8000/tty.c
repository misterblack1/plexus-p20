#include	"crash.h"
#include	"sys/tty.h"

char	ttytype[10];
unsigned  ttyaddr;
unsigned  ttycnt;
prtty(c, md, all)
	register  int  c;
	int	md;
	int	all;
{
	struct	tty	tbuf;
	short	t_state;
	ushort	t_iflags;
	ushort	t_oflags;
	ushort	t_cflags;
	ushort	t_lflags;
	extern long int delta;

	if(c == -1)
		return;
	if(ttyaddr == 0) {
		error("no default tty structure");
		return;
	}
	if(c >= ttycnt) {
		printf("%2.2s%2d out of range\n", ttytype, c);
		return;
	}
	if(lseek(kmem, (long)(ttyaddr + c * sizeof tbuf) + delta, 0) == -1) {
		printf("bad seek of tty structure\n");
	}
	if(read(kmem, &tbuf, sizeof tbuf) != sizeof tbuf) {
		printf("%2.2s%2d read error on tty structure\n", ttytype, c);
		return;
	}
	if(!all && (tbuf.t_state & ISOPEN) == 0)
		return;
	printf("%2.2s%2d %3d %3d %3d %3d %5u %3u",
		ttytype, c, tbuf.t_rawq.c_cc, tbuf.t_delct & 0377,
		tbuf.t_canq.c_cc, tbuf.t_outq.c_cc,
		tbuf.t_pgrp, tbuf.t_col & 0377);
	t_state = tbuf.t_state;

	/* internal state */
	printf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
		t_state & TIMEOUT ? " timeout" : "",
		t_state & WOPEN ? " wopen" : "",
		t_state & ISOPEN ? " isopen" : "",
		t_state & TBLOCK ? " tblock" : "",
		t_state & CARR_ON ? " carr_on" : "",
		t_state & BUSY ? " busy" : "",
		t_state & OASLP ? " oaslp" : "",
		t_state & IASLP ? " iaslp" : "",
		t_state & TTSTOP ? " stop" : "",
		t_state & EXTPROC ? " extprc" : "",
		t_state & TACT ? " tact" : "",
		t_state & ESC ? " esc" : "",
		t_state & RTO ? " rto" : "",
		t_state & TTIOW ? " ttiow" : "",
		t_state & TTXON ? " ttxon" : "",
		t_state & TTXOFF ? " ttxoff" : "");
	if(md == 0)
		return;
	t_iflags = tbuf.t_iflag;
	t_oflags = tbuf.t_oflag;
	t_cflags = tbuf.t_cflag;
	t_lflags = tbuf.t_lflag;
	printf("     (%c,%c) out rtn: %.1o,",
		tbuf.t_cc[2], tbuf.t_cc[3], tbuf.t_proc);

	/* input modes */
	printf("%s%s%s%s%s%s%s%s%s%s%s%s%s",
		(t_iflags&IGNBRK) ? " ignbrk" : "",
		(t_iflags&BRKINT) ? " brkint" : "",
		(t_iflags&IGNPAR) ? " ignpar" : "",
		(t_iflags&PARMRK) ? " parmrk" : "",
		(t_iflags&INPCK)  ? " inpck"  : "",
		(t_iflags&ISTRIP) ? " istrip" : "",
		(t_iflags&INLCR)  ? " inlcr"  : "",
		(t_iflags&IGNCR)  ? " icncr"  : "",
		(t_iflags&ICRNL)  ? " icrnl"  : "",
		(t_iflags&IUCLC)  ? " iuclc"  : "",
		(t_iflags&IXON)   ? " ixon"   : "",
		(t_iflags&IXANY)  ? " ixany"  : "",
		(t_iflags&IXOFF)  ? " ixoff"  : "");

	/* output modes */
	printf("%s%s%s%s%s%s%s%s",
		(t_oflags&OPOST)  ? " opost"  : "",
		(t_oflags&OLCUC)  ? " olcuc"  : "",
		(t_oflags&ONLCR)  ? " onlcr"  : "",
		(t_oflags&OCRNL)  ? " ocrnl"  : "",
		(t_oflags&ONOCR)  ? " onocr"  : "",
		(t_oflags&ONLRET) ? " onlret" : "",
		(t_oflags&OFILL)  ? " ofill"  : "",
		(t_oflags&OFDEL)  ? " ofdel"  : "");
	printf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		(t_oflags&NLDLY)  == NL0  ? " nl0"  : "",
		(t_oflags&NLDLY)  == NL1  ? " nl1"  : "",
		(t_oflags&CRDLY)  == CR0  ? " cr0"  : "",
		(t_oflags&CRDLY)  == CR1  ? " cr1"  : "",
		(t_oflags&CRDLY)  == CR2  ? " cr2"  : "",
		(t_oflags&CRDLY)  == CR3  ? " cr3"  : "",
		(t_oflags&TABDLY) == TAB0 ? " tab0" : "",
		(t_oflags&TABDLY) == TAB1 ? " tab1" : "",
		(t_oflags&TABDLY) == TAB2 ? " tab2" : "",
		(t_oflags&TABDLY) == TAB3 ? " tab3" : "",
		(t_oflags&BSDLY)  == BS0  ? " bs0"  : "",
		(t_oflags&BSDLY)  == BS1  ? " bs1"  : "",
		(t_oflags&VTDLY)  == VT0  ? " vt0"  : "",
		(t_oflags&VTDLY)  == VT1  ? " vt1"  : "",
		(t_oflags&FFDLY)  == FF0  ? " ff0"  : "",
		(t_oflags&FFDLY)  == FF1  ? " ff1"  : "");

	/* control modes */
	printf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		(t_cflags&CBAUD)==B0    ? " b0"    : "",
		(t_cflags&CBAUD)==B50   ? " b50"   : "",
		(t_cflags&CBAUD)==B75   ? " b75"   : "",
		(t_cflags&CBAUD)==B110  ? " b110"  : "",
		(t_cflags&CBAUD)==B134  ? " b134"  : "",
		(t_cflags&CBAUD)==B150	? " b150"  : "",
		(t_cflags&CBAUD)==B200  ? " b200"  : "",
		(t_cflags&CBAUD)==B300  ? " b300"  : "",
		(t_cflags&CBAUD)==B600  ? " b600"  : "",
		(t_cflags&CBAUD)==B1200 ? " b1200" : "",
		(t_cflags&CBAUD)==B1800 ? " b1800" : "",
		(t_cflags&CBAUD)==B2400 ? " b2400" : "",
		(t_cflags&CBAUD)==B4800 ? " b4800" : "",
		(t_cflags&CBAUD)==B9600 ? " b9600" : "",
		(t_cflags&CBAUD)==EXTA  ? " exta"  : "",
		(t_cflags&CBAUD)==EXTB  ? " extb"  : "");
	printf("%s%s%s%s%s%s%s%s%s%s",
		(t_cflags&CSIZE)==CS5   ? " cs5"   : "",
		(t_cflags&CSIZE)==CS6   ? " cs6"   : "",
		(t_cflags&CSIZE)==CS7   ? " cs7"   : "",
		(t_cflags&CSIZE)==CS8   ? " cs8"   : "",
		(t_cflags&CSTOPB) ? " cstopb" : "",
		(t_cflags&CREAD)  ? " cread"  : "",
		(t_cflags&PARENB) ? " parenb" : "",
		(t_cflags&PARODD) ? " parodd" : "",
		(t_cflags&HUPCL)  ? " hupcl"  : "",
		(t_cflags&CLOCAL) ? " clocal" : "");

	/* line discipline 0 modes */
	printf("%s%s%s%s%s%s%s%s\n",
		t_lflags & ISIG   ? " isig"   : "",
		t_lflags & ICANON ? " icanon" : "",
		t_lflags & XCASE  ? " xcase"  : "",
		t_lflags & ECHO   ? " echo"   : "",
		t_lflags & ECHOE  ? " echoe"  : "",
		t_lflags & ECHOK  ? " echok"  : "",
		t_lflags & ECHONL ? " echonl" : "",
		t_lflags & NOFLSH ? " noflsh" : "");
}

settty(s)
	register  char  *s;
{
	char	name[20];
	register  struct  nlist  *sp1, *sp2;
	struct	nlist	*symsrch();
	int	count;
	extern long int delta;

	if(strlen(s) >4)
		goto bad;
	strcpy(name, "_");
	strcat(name, s);
	strcat(name, "_tty");
	if((sp1 = symsrch(name)) == 0)
		goto bad;
	strcpy(name, "_");
	strcat(name, s);
	strcat(name, "_cnt");
	if((sp2 = symsrch(name)) == 0)
		goto bad;
	if(lseek(kmem, (long)sp2->n_value + delta, 0) == -1){
		printf("bad seek of tty count\n");
	}
	if(read(kmem, &count, sizeof count) != sizeof count)
		goto bad;
	ttycnt = count;
	strcpy(ttytype, s);
	ttyaddr = (unsigned)sp1->n_value;
	return(1);
    bad:printf("invalid tty structure\n");
	return(NULL);
}
