#include "sys/param.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/tty.h"
#include "sys/ioctl.h"
#define OPEN	01
#define TRSLEEP	04
#define TRACE	2
#define TRQMAX	512
#define TRPRI	(PZERO + 3)

int tr_cnt;
struct trace{
	struct 	clist tr_outq;
	short	tr_state;
	short	tr_chbits;
	short	tr_rcnt;
	char	tr_chno;
	char	tr_ct;
}trace[TRACE];

tropen(dev)
{
	struct trace *tp;
/*
	if(dev >= tr_cnt){
*/
	if(dev >= 2) {
		u.u_error = ENXIO;
		return;
	}
	tp = &trace[dev];
	if(tp->tr_state&OPEN){
		u.u_error = EACCES;
		return;
	}
	tp->tr_state |= OPEN;
}
trioctl(dev,cmd,arg,mode)
{
	register struct trace *tp;

	tp = &trace[dev];
	switch(cmd){
	case VPMTRCO:
		tp->tr_chbits |= (01<<(int)arg);
		return;
	default:
		u.u_error = EINVAL;
		return;
	}
}
trclose(dev)
{
	struct trace *tp;

	tp = &trace[dev];
	tp->tr_chbits = 0;
	tp->tr_ct = 0;
	tp->tr_chno = 0;
	tp->tr_rcnt = 0;
	while(getc(&tp->tr_outq)>=0);
	tp->tr_state = 0;
}
trread(dev)
{
	register struct trace *tp;

	tp = &trace[dev];
	spl5();
	tp->tr_state |= TRSLEEP;
	while(tp->tr_rcnt == 0)
		sleep(&tp->tr_rcnt,TRPRI);
	spl0();
	while(u.u_count && tp->tr_rcnt){
		if(tp->tr_chno == 0){
			tp->tr_chno = getc(&tp->tr_outq);
			tp->tr_ct = getc(&tp->tr_outq);
		}
		if(u.u_count < (tp->tr_ct + 2))
			return;
		passc(tp->tr_chno);
		passc(tp->tr_ct);
		while(tp->tr_ct--)
			passc(getc(&tp->tr_outq));
		tp->tr_chno = 0;
		tp->tr_rcnt--;
	}
}
trsave(dev,chno,buf,ct)
char *buf,dev,chno,ct;
{

	register struct trace *tp;
	register int n;

	tp = &trace[dev];
	ct &= 0377;
	if((tp->tr_chbits&(1<<chno)) == 0)
		return;
	if((tp->tr_outq.c_cc + ct + 2) >TRQMAX)
		return;
	putc(1<<chno,&tp->tr_outq);
	putc(ct,&tp->tr_outq);
	for(n=0;n<ct;n++)
		putc(buf[n],&tp->tr_outq);
	tp->tr_rcnt++;
	if(tp->tr_state&TRSLEEP){
		tp->tr_state &= ~TRSLEEP;
		wakeup(&tp->tr_rcnt);
	}

}
