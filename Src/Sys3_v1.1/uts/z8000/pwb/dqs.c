#
/*
 * DQS11[AB] handler - IBM Bisync
 */

#define DQSFUDGE 1
#include "sys/param.h"
#include "sys/buf.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/dqs.h"

int dqstr[61];
struct device *dqs_addr[];
int	dqs_cnt;
extern struct dqsdat dqsx[];
#define PRI (PZERO+5)


struct device {
	int	csr;
	int	bcr;
	char	*bar;
	int	dbr;
};

#define CSR dqs->addr->csr
#define BCR dqs->addr->bcr
#define BAR dqs->addr->bar
#define DBR dqs->addr->dbr

#define GO  01
#define RX  02
#define IE  0100
#define RDY 0200
#define DTR	0400
#define ERR 0177000

#define DLY dqs->dly

#define EOT dqs->eot
#define ENQ dqs->enq
#define NAK dqs->nak
#define ETB dqs->etb
#define ETX dqs->etx

#define TTD  dqs->ttd
#define ACK0 dqs->ack0
#define WACK dqs->wack
#define ACKX dqs->ackx

#define R0 1
#define R1 2
#define R2 3
#define R3 4
#define R4 5
#define R5 6
#define W0 7
#define W1 8
#define W2 9
#define W3 10

#define RT0 11
#define RT1 12
#define RT2 13
#define RT3 14
#define RT4 15
#define RT5 16
#define WT0 17
#define WT1 18
#define WT2 19
#define WT3 20

#define TOUT 10

dqsopen(dev)
{
	register struct dqsdat *dqs;
	int dqstout();
	if (dev>=NDQ || dev>=dqs_cnt || (dqs = &dqsx[dev])->open) {
		u.u_error = ENXIO;
		return;
	}
	dqs->addr = dqs_addr[dev];
	dqs->open=1; 
	dqs->time=0;
	timeout(dqstout,dqs,20);
}

dqsclose(dev)
{
	register struct dqsdat *dqs;
	dqs = &dqsx[dev];
	CSR = DTR;
	dqs->qcase=0;
	dqs->open=2;
	dqsfin();
	CSR = 0;
}

dqsread(dev)
{
	register struct dqsdat *dqs;
	register struct dqsbuf *b;
	register int c;
	dqs = &dqsx[dev];
	spl5();
	while (dqs->state<0) sleep(dqs,PRI);
	spl0();
	if (dqs->state==0) {
		dqs->q=0;
		dqs->resp=ACK0; 
		dqs->try=3;
		dqs->state++; 
		dqs->qcase=R0;
		dqsrecv(); 
		dqs->time=20;
		dqs->u=0;
	}
	if (dqs->u==0) {
		dqs->uoff=0;
		spl5();
		while ((c=dqs->state)>0) {
			if ((dqs->u = (struct dqsbuf *)dqsget(1)) != (struct dqsbuf *)0) break;
			if (c==2 && dqs->x==0) {
				dqsbeg(); 
				spl5();
				if (dqs->qcase==R4) dqs->time=1;
				continue;
			}
			if (c>=3) {
				spl0(); 
				dqsfin(); 
				return;
			}
			dqsleep();
		}
		spl0();
		if (dqs->state<=0) return;
	}
	b=dqs->u;
	c=min(b->bufc,u.u_count);
	if (c&1 && c<u.u_count) {
		c++; 
		u.u_count++;
	}
	pimove(paddr(b->bufa)+dqs->uoff,c,B_READ);
	dqs->uoff+=c;
	if ((b->bufc-= c)>0) return;
	b->bufl=0; 
	dqs->u=0;
	spl5();
	if (dqs->qcase==R4) dqs->time=1;
	spl0();
}

dqswrite(dev)
{
	register struct dqsdat *dqs;
	register struct dqsbuf *b;
	register int c;
	dqs = &dqsx[dev];
	spl5();
	while (dqs->state>0) sleep(dqs,PRI);
	if (u.u_count==0) {
		while ((c=dqs->state)<0 && c>(-5)) {
			if (c==(-2)) {
				dqs->state--;
				if (dqs->qcase==W2) dqs->time=1;
			}
			dqsleep();
		}
		goto N;
	}
	spl0();
	if (dqs->state==0) {
		dqs->q=0;
		dqs->resp=ACK0; 
		dqs->try=3;
		dqs->state--; 
		dqs->qcase=W0;
		dqspoke(ENQ);
	}
	spl5();
	while ((c=dqs->state)<0 && c>(-5)) {
		if ((b = (struct dqsbuf *)dqsget(0)) != (struct dqsbuf *)0) break;
		if (c==(-2) && dqs->x==0) {
			dqsbeg(); 
			spl5(); 
			continue;
		}
		dqsleep();
	}
N: 
	spl0();
	if ((c=dqs->state)<=(-5)) {
		if (c==(-5)) u.u_error=EIO;
		dqsfin(); 
		return;
	}
	if (dqs->state>=0) return;
	b->bufc=c=min(512,u.u_count);
	if (c&1) {
		c++; 
		u.u_count++;
	}
	pimove(paddr(b->bufa),c,B_WRITE);
	b->bufl=1;
	spl5();
	if (dqs->qcase==W2) dqs->time=1;
	spl0();
}

dqsgo(reg)
struct device *reg;
{
	reg->csr=IE|GO;
}

dqsintr(dev)
{
	int erf;
	register struct dqsdat *dqs;
	register struct dqsbuf *b;
	register int tmp;

	dqs = &dqsx[dev];
	tmp = (int)(&dqstr[dqstr[0]+1]);
	*((char *)tmp)++ = dev;
	*((char *)tmp)++ = (int)time;
	*((char *)tmp)++ = dqs->state;
	*((char *)tmp)++ = dqs->qcase;
	*((int *)tmp)++ = CSR;
	*((int *)tmp)++ = BCR;
	*((int *)tmp)++ = (int)(BAR);
	*((int *)tmp)++ = DBR;
	tmp=((int *)tmp)-(dqstr+1);
	if (tmp>54) tmp=0;
	dqstr[0]=tmp;
	erf=CSR&ERR;
	CSR=0; 
	dqs->time=0;
	switch (dqs->qcase) {
	case R0:
		tmp=dqs->cc;
		if (erf) tmp=0;
		if ((char)tmp==EOT) goto reot;
		if ((char)tmp==ENQ) {
			if (dqs->state==1) {
				dqs->state++; 
				dqswake(); 
				dqs->try=7;
			}
			goto rack;
		}
		dqs->qcase=R1; 
		goto rnak;
	case R1: 
	case RT1:
		dqs->qcase=R0; 
		dqsrecv(); 
		break;
	case R2: 
	case RT2:
		dqs->qcase=R3;
		tmp = (int)dqs->addr;
		((struct device *)tmp)->bcr=(-512);
		((struct device *)tmp)->bar=dqs->q->bufb;
		((struct device *)tmp)->csr=IE|RX|GO;
		dqs->time=40; 
		break;
	case R3:
		dqs->qcase=R2;
		if (erf) goto rnak;
		b=dqs->q;
		tmp=(*(b->bufb));
		if ((char)tmp==EOT) goto reot;
		if ((char)tmp==ENQ) goto rack;
		b->bufc=(BAR-(b->bufb));
		b->bufc--; 
		tmp=DBR;
		if ((char)tmp==ENQ) goto rnak;
		if ((char)tmp!=ETB && (char)tmp!=ETX) {
			b->bufc++; 
			tmp  >>= 8;
			if ((char)tmp!=ETB && (char)tmp!=ETX) goto rnak;
		}
		b->bufl=1; 
		dqs->q=0;
		dqswake();
		dqs->resp=ACKX-dqs->resp;
		dqs->try=7;
rack: 
		if (--dqs->try<0) goto rabt;
	case R4: 
	case RT4:
		if (dqs->q==(struct dqsbuf *)0 && (dqs->q=(struct dqsbuf *)dqsget(0))==(struct dqsbuf *)0) {
			if (dqs->qcase==RT4) {
				dqs->try++; 
				dqs->qcase=R1; 
				dqspoke(WACK); 
				break;
			}
			dqs->qcase=R4; 
			dqs->time=7; 
			break;
		}
		dqs->qcase=R2; 
		dqspoke(dqs->resp); 
		break;
rnak: 
		if (--dqs->try<=0) goto rabt;
		dqspoke(NAK); 
		break;
	case RT0: 
	case RT3:
rabt: 
		if (dqs->state==1) goto reot;
		dqs->qcase=R5; 
		dqspoke(EOT); 
		break;
	case R5: 
	case RT5:
reot: 
		dqs->state=3; 
		dqswake(); 
		dqs->qcase=0; 
		break;
	case W0: 
	case WT0:
		dqs->qcase=W1; 
		dqsrecv(); 
		break;
	case WT1:
		erf++;
	case W1:
		tmp=dqs->cc;
		if (erf) tmp=0;
		if (tmp!=dqs->resp) goto wnot;
		if (dqs->state==(-1)) dqs->state--;
		if (b=dqs->q) {
			b->bufl=0; 
			dqs->q=0;
		}
		dqswake();
		dqs->resp=ACKX-dqs->resp;
		dqs->try=7;
	case W2: 
	case WT2:
wsnd: 
		if (dqs->q==(struct dqsbuf *)0 && (dqs->q=(struct dqsbuf *)dqsget(1))==(struct dqsbuf *)0) {
			if (dqs->state==(-3)) goto weot;
			if (dqs->qcase==WT2) {
				dqs->try++; 
				dqs->qcase=W0; 
				dqspoke(TTD); 
				break;
			}
			dqs->qcase=W2; 
			dqs->time=7; 
			break;
		}
		b=dqs->q;
		dqs->qcase=W0;
		tmp = (int)(dqs->addr);
		((struct device *)tmp)->bcr=(-b->bufc);
		((struct device *)tmp)->bar=b->bufb;
		dqs->time=7+DLY;
		if (DLY==0) ((struct device *)tmp)->csr=IE|GO;
		else timeout(dqsgo,tmp,DLY);
		break;
wnot: 
		if (--dqs->try<=0) goto wabt;
		if ((char)tmp==ENQ || (char)tmp==EOT) goto wabt;
		if ((char)tmp==NAK) {
			if (dqs->state==(-1)) goto wabt; 
			goto wsnd;
		}
		dqs->qcase=W0; 
		dqspoke(ENQ); 
		break;
wabt: 
		if (dqs->state!=(-1)) dqs->state=(-4);
		else if ((char)tmp==ENQ) goto wend;
weot: 
		dqs->qcase=W3; 
		dqspoke(EOT); 
		break;
	case W3: 
	case WT3:
wend: 
		dqs->state=(dqs->state>(-4)? -6:-5);
		dqswake(); 
		dqs->qcase=0; 
		break;
	}
}

dqstout(ptr)
struct dqsdat *ptr;
{
	register struct dqsdat *dqs;
	dqs=ptr;
	if (dqs->open!=1) {
		dqs->open=0; 
		return;
	}
	if (dqs->time>0 && --dqs->time==0) {
		CSR=0; 
		dqs->qcase+=TOUT; 
		dqsintr(dqs-dqsx);
	}
	timeout(dqstout,dqs,20);
}


/* dqs passed in register beyond this point */

dqsrecv()
{
	register struct dqsdat *dqs;
	register struct device *reg;
	reg=dqs->addr;
	reg->bcr=(-2);
	reg->bar = (char *)(&dqs->cc);
	reg->csr=IE|RX|GO;
	dqs->time=11;
}

dqspoke(code)
{
	register struct dqsdat *dqs;
	register struct device *reg;
	register tmp;
	reg=dqs->addr;
	tmp=code; 
	dqs->cc=tmp;
	reg->bcr=(tmp==(char)tmp? -1:-2);
	reg->bar = (char *)(&dqs->cc);
	dqs->time=2+DLY;
	if (DLY==0) reg->csr=IE|GO;
	else timeout(dqsgo,reg,DLY);
}

dqsleep()
{
	register struct dqsdat *dqs;
	dqs->slp=1; 
	sleep(dqs->addr,PRI);
}

dqswake()
{
	register struct dqsdat *dqs;
	if (dqs->slp) wakeup(dqs->addr);
	dqs->slp=0;
}

dqsbeg()
{
	register struct dqsdat *dqs;
	register struct dqsbuf *b;
	register int i;
	dqs->x = (struct dqsbuf *)dqs;
	b=(&dqs->bf[NBF-1]);
	for (i=0;i<NBF;i++) {
		b=b->bufn=(&dqs->bf[i]); 
		b->bufl=0;
		b->bufb = (char *)(loword((b->bufa=getablk(1))->b_paddr));
	}
	dqs->x=b;
}

dqsget(flag)
{
	register struct dqsdat *dqs;
	register struct dqsbuf *b;
	if ((b=dqs->x) <= (struct dqsbuf *)dqs) return(0);
	if (flag!=0) {
		if (b->bufl!=1) return(0);
		dqs->x=b->bufn; 
		goto P;
	}
	while (b->bufl!=0) {
		b=b->bufn; 
		if (b==dqs->x) return(0);
	}
P: 
	b->bufl=2; 
	return((int)b);
}

dqsfin()
{
	register struct dqsdat *dqs;
	register struct dqsbuf *b;
	if ((b=dqs->x) == (struct dqsbuf *)dqs) return;
	if (b!=0) {
		do {
			brelse(b->bufa); 
			b=b->bufn;
		} 
		while (b!=dqs->x);
	}
	dqs->x=0;
	dqs->state=0;
	wakeup(dqs);
}
