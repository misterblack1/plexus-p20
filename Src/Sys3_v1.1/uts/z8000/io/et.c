

	/*-------------------------------*\
	| Interland Ethernet Board Driver |
	|				  |
	| 9/9/82 By Plexus Computers Inc. |
	|				  |
	| Programmed by Paul Fronberg	  |
	\*-------------------------------*/

#include <sys/param.h>
#include <sys/plexus.h>		/* get P_SPIC1 definition */
#include <sys/map.h>		/* for etalloc() and etfree() */
#include <ether/etconf.h>
#include <ether/reg.h>
#include <ether/status.h>
#include <ether/state.h>
#include <ether/sizes.h>
#include <ether/buffers.h>
#include <ether/maps.h>
#include <ether/command.h>
#include <ether/misc.h>

/* following used in testing only */
#include <sys/dir.h>
#include <sys/user.h>

int interlan;		/* Was interlan board found? */
int num_hosts;		/* number of ethernet hosts found in config tbl */
int etherstate;		/* current state of ethernet board */
int interrupt;		/* immage of `Interrupt Enable Register' */
int xmit_status;	/* status of transmission request */
int buffer_available;	/* space available to hold largest receivable buffer */
int et_read_waiting;	/* read is waiting for a buffer to become available */

struct etcom *rec_buf;	/* pointer to current buffer being read into memory */
struct etcom *head_rec;	/* head pointer to queue of received buffers */
struct etcom *tail_rec;	/* tail pointer to queue of received buffers */

char my_address[6];	/* my ethernet address */
extern int local_host;	/* identifier determined for local host */



	/*---------------------*\
	| driver initialization |
	\*---------------------*/

etinit()
{
	int status;
	int i;
	char *psb;
	struct sb sb;
	extern initstate;
	extern mbusto;

		/* initialize communication tables */
		/*
		 * Due to multibus byte swapping, address are swapped in
		 * tables.
		 */

	ptoet[3].et_addr[0] = 7;
	ptoet[3].et_addr[1] = 2;
	ptoet[3].et_addr[2] = 0;
	ptoet[3].et_addr[3] = 1;
	ptoet[3].et_addr[4] = 0x3d;
	ptoet[3].et_addr[5] = 2;

	ptoet[5].et_addr[0] = 7;
	ptoet[5].et_addr[1] = 2;
	ptoet[5].et_addr[2] = 0;
	ptoet[5].et_addr[3] = 1;
	ptoet[5].et_addr[4] = 0x45;
	ptoet[5].et_addr[5] = 2;

	ettop[0].p_addr[0] = 7;
	ettop[0].p_addr[1] = 2;
	ettop[0].p_addr[2] = 0;
	ettop[0].p_addr[3] = 1;
	ettop[0].p_addr[4] = 0x3d;
	ettop[0].p_addr[5] = 2;
	ettop[0].p_host    = 3;

	ettop[1].p_addr[0] = 7;
	ettop[1].p_addr[1] = 2;
	ettop[1].p_addr[2] = 0;
	ettop[1].p_addr[3] = 1;
	ettop[1].p_addr[4] = 0x45;
	ettop[1].p_addr[5] = 2;
	ettop[1].p_host    = 5;

	num_hosts = 2;

	buffer_available = YES;
	interlan = NO;

printf("starting ethernet initialization\n");
		/* Mark ethernet board in power up state */

	etherstate = INITIAL;

		/* See if there is a board plugged in */

	/* cause trap.c to increment mbusto on multibus timeout */
	initstate = 1;
	mbusto = 0;
	/* try to read in the status register */
	status = in_multibus(STATREG);
	initstate = 0;
	/* check if there's been a multibus timeout */
	if(mbusto)
		return;		/* Yes. There is no interland board */

		/* Interland ethernet board was seen */
	interlan = YES;

printf("Interland board found\n");
printf("status: %o\n",status);
		/* enable multibus interrupts for the Interlan board (lvl 0) */

	out_local( P_SPIC1, (in_local( P_SPIC1 ) & (~ETINIT)) );

		/* get Interland ethernet address */

	out_multibus(CMNDREG, STATISTICS);
	/* get status of command */
	in_multibus(STATREG);
	/* get the status vector */
	/* wait for the interrupt vector */
	while(in_multibus(INTREG)&SBA);
	for(i=0,psb=(char *)&sb; i<sizeof(sb); i++) {
		/* wait for status register */
		while((in_multibus(INTREG)&SRF) == 0);
		*psb++ = in_multibus(STATREG);
	}

		/* get interlan ethernet address */
	bcopy(&sb.physaddr[0], &my_address[0], 6);

for(i=0,psb = (char *)&sb; i<sizeof(sb); i++) printf("   %x",(*psb++ & 0xff));
printf("\n");

		/* place board on line */
	etherstate = STARTUP;
	interrupt = EN_SRF;
	out_multibus(IENBREG, EN_SRF);
	out_multibus(CMNDREG,ONLINE);

	return;
}



	/*-------------------------*\
	| Read buffer from ethernet |
	\*-------------------------*/

struct cb *et_read()
{
	int s;
	int dest,src;
	struct cb *cptr;
	struct etrdb *rptr;
	struct etcom *eptr;

	/* anything to read? */
again:
	s = spl5();
	while(!head_rec) {
		/* no, wait for a block to arrive */
		et_read_waiting = 1;
		sleep(&et_read_waiting, ETIPRI);
	}

	/* yes, take the first element from the queue */
	eptr = head_rec;
	head_rec = head_rec->et_next;
	splx(s);

	/* rptr is substructure contained within eptr */
	rptr = (struct etrdb *) &eptr->et_body[RDBOFF];

	/* cptr is substructure contained within eptr */
	cptr = (struct cb *) &eptr->et_body[CBOFF];

	/* translate source ethernet address to host # */
	if((src=et2h(&rptr->r_src[0])) == -1) {
		relbuf(cptr);
		goto again;
	}

	/* translate destination ethernet address to host # */
	if((dest=et2h(&rptr->r_dest[0])) == -1) {
		relbuf(cptr);
		goto again;
	}

	cptr->cb_dest = dest;
	cptr->cb_src = src;

	/* relocate buffer status */
	cptr->cb_status = rptr->r_status & 0xff;

	/* zero link to next communication block */
	cptr->cb_next = (struct cb *) 0;

	return cptr;

}


	/*----------------------------*\
	| Write buffer out to ethernet |
	\*----------------------------*/


et_write(cb)
struct cb *cb;
{
	struct ettdb *tptr;
	int dest;
	int type;
	int length;
	int s;
	int status;
	char *ptr;

	/* convert plexus # to ethernet address. If 0, then illegal */
	if(*(ptr = &ptoet[cb->cb_dest].et_addr[0]) == NULL_ADDR)
		return BAD_CMND;

		/* determine if board is currently busy */

	/* wait for indication that device is free */
	s = spl5();
	while(etherstate != ONLINE)
		sleep(&interrupt, ETIPRI);
	/* turn off multibus interrupts */
	out_multibus(IENBREG, DIABLE);
	/* indicate that we are waiting for transmission to complete */
	etherstate = SENDING;
	splx(s);

		/* set up ethernet buffer: destination and type */

	/* construct pointer to offset corresponding to transmission blk */
	tptr = (struct ettdb *) &cb->cb_pad[0];

	bcopy(ptr, &tptr->t_dest[0], 6);	/* destination */

		/* set up registers for DMA transfer to Interlan board */

	/* setup buffer address */
	out_multibus(EBUSREG,MBSDSEG);
	out_multibus(HBUSREG, HIBYTE((int) tptr));
	out_multibus(LBUSREG, LOBYTE((int) tptr));

printf("hibyte address: %x\n", HIBYTE((int) tptr));
printf("lobyte address: %x\n", LOBYTE((int) tptr));

	/* setup buffer length for DMA */

	length = tptr->t_size + XMIT_HDR;
	if(length&1) length++;
	out_multibus(HCNTREG, HIBYTE(length));
	out_multibus(LCNTREG, LOBYTE(length));

printf("hibyte length: %x\n", HIBYTE(length));
printf("lobyte length: %x\n", LOBYTE(length));

	/* request transmit DMA Done interrupt */

	xmit_status = 0;
	interrupt = EN_TDD;
	out_multibus(IENBREG, EN_TDD);

	/* wait for transfer to complete */
	s = spl5();
	while(etherstate == SENDING)
		sleep(&xmit_status, ETIPRI);
	splx(s); 

	/*
	 * save returned status so we can return it. Note that by saving
	 * it on the stack, we don't have to worry about an interrupt
	 * changing the value before the return.
	 */
	status = xmit_status;

	/* reassert a receive block available interrupt */
	etherstate = ONLINE;
	if(buffer_available) {
		interrupt = EN_RBA;
		out_multibus(IENBREG, EN_RBA);
	}

	/* wakeup anyone waiting to output a block */
	wakeup(&interrupt);

	/* return status of transmission */

	return (status);
}



	/*--------------------------*\
	| Interland interrupt server |
	\*--------------------------*/

etintr()
{
	int status;
	int len,base;
	struct etrdb *rptr;
	unsigned int etalloc();
	int rba_activate();

	/* interrupt latched by board. Clear it */
	out_multibus(IENBREG, CLR_INT);
	switch(interrupt) {
	case EN_SRF:	/* status register full interrupt */
		/* clear status register */
		status = in_multibus(STATREG);
printf("status: %o\n",status);

		switch(etherstate) {
		
		case STARTUP:	/* Comming on line */
printf("comming on line: %o\n", status);

			/* We are now online. */
			etherstate = ONLINE;

			/* enable receive-block-available interrupt */
			interrupt = EN_RBA;
			out_multibus(IENBREG, EN_RBA);
			break;

		case SENDING:	/* writing data out to ether */
printf("FIFO being sent out to ether\n");

			interrupt = 0;
			etherstate = DONE;
			/* data was sent out onto the ether cable */
			xmit_status = status;
			/* wakeup process waiting for write to complete */
			wakeup(&xmit_status);
			break;
		}
		break;

	case EN_RDD:		/* receive DMA done interrupt */
printf("receive buffer in core\n");
		/* clear status register */
		status = in_multibus(STATREG);
printf("status: %o\n",status);

		/* determine if part of buffer may be returned */
		rptr = (struct etrdb *) &rec_buf->et_body[RDBOFF];
printf("comm buffer length: %d\n",rec_buf->et_length);
		if((len=rptr->r_length) < ETSIZE-4-HDR-HEAD-RDBOFF) {
printf("receive buffer length: %d\n", len);
			base = (unsigned int)rec_buf + 4 + HDR + RDBOFF + len;
			len = ETSIZE - 4 - HDR - HEAD - RDBOFF - len;
			rec_buf->et_length -= len;
printf("start of free buffer: %x\n",base);
printf("length to free: %d\n", len);
printf("new length of rec_buf: %d\n",rec_buf->et_length);
			etfree(len,base);
		}
else
printf("receive buffer length: %d\n", len);

		/* put new block into input Q */
		if(head_rec)
			tail_rec->et_next = rec_buf;
		else
			/* Q empty */
			head_rec = rec_buf;

		tail_rec = rec_buf;
		rec_buf->et_next = 0;
printf("buffer in Q at: %x\n",tail_rec);

		/* if read is waiting for block, wake em up */
		if(et_read_waiting) {
			et_read_waiting = NO;
			wakeup(&et_read_waiting);
		}

		/* reenable receive buffer available interrupt */
		etherstate = ONLINE;
		interrupt = EN_RBA;
		out_multibus(IENBREG, EN_RBA);
		break;

	case EN_TDD:		/* transmitt DMA done interrupt */
printf("transmit DMA done interrupt\n");
		/* clear status register */
		/* success or success with retries */
		if((xmit_status=in_multibus(STATREG)) > 1) {
printf("status: %o\n",xmit_status);
			etherstate = FAILURE;
			wakeup(&xmit_status);
			break;
		}

		/* data now transferred to Interlan, now send it out */
		interrupt = EN_SRF;
		out_multibus(IENBREG, EN_SRF);
		out_multibus(CMNDREG, LDSND_DATA);
		break;

	case EN_RBA:		/* receive buffer available interrupt */
printf("receive buffer available\n");
		/* clear status register */
		status = in_multibus(STATREG);
printf("status: %o\n",status);

		/* set ethernet state to receiving (no longer available) */
		etherstate = RECEIVING;

		/*-------------------------------------------------------*\
		|  get a data buffer. If one isn't available, fire off a  |
		|  timeout event to initiate a "receive buffer available" |
		|  interrupt after 2 ticks. We will immediately abort at  |
		|  this point, since there is nothing to do. The Interlan |
		|  board will continue to queue up received buffers until |
		|  it runs out of space. It will then refuse to receive   |
		|  buffers which will cause a bad status return to the    |
		|  transmitting host.					  |
		\*-------------------------------------------------------*/

		if((rec_buf=(struct etcom *) etalloc(ETSIZE)) == 0) {
printf("ethernet buffer space exausted\n");
			buffer_available = NO;
			etherstate = ONLINE;
			interrupt = CLR_INT;
			timeout(rba_activate, 0, 2);
			break;
		}

		/* buffer was obtained. Read data from Interlan to host */

		/* set length of buffer */
		rec_buf->et_length = ETSIZE;

		/* setup buffer addresses (skip over header to etrdb) */
		out_multibus(EBUSREG, MBSDSEG);
		out_multibus(HBUSREG, HIBYTE((int)rec_buf+HDR+HEAD+RDBOFF));
		out_multibus(LBUSREG, LOBYTE((int)rec_buf+HDR+HEAD+RDBOFF));

		/* setup length */
		out_multibus(HCNTREG, HIBYTE(ETSIZE-HDR-HEAD));
		out_multibus(LCNTREG, LOBYTE(ETSIZE-HDR-HEAD));

		/* setup receive DMA transfer interrupt */
		interrupt = EN_RDD;
		out_multibus(IENBREG, EN_RDD);
		break;

	default:		/* unscheduled type of interrupt */
		/* clear status register */
		status = in_multibus(STATREG);
		printf("unknown type of interrupt\n");
	}
}



	/*--------------------------------------------------*\
	| timeout to pause if there is no blocks left for et |
	\*--------------------------------------------------*/

rba_activate()
{
	if(buffer_available) return;

	buffer_available = YES;
	if(etherstate == ONLINE) {
		interrupt = EN_RBA;
		out_multibus(IENBREG, EN_RBA);
	}
}



	/*-----------------------------------------------------------*\
	| convert the ethernet address to a host id. Uses table ettop |
	\*-----------------------------------------------------------*/

et2h(r)
register int *r;
{
	register struct ettop *p;
	register int *q,*s;

	for(p = &ettop[0]; p < &ettop[num_hosts]; p++) {
		q = (int *) p;
		s = r;
		if(*q++ != *s++) continue;
		if(*q++ != *s++) continue;
		if(*q++ != *s++) continue;
		return *q;
	}
	return -1;
}



	/*---------------------------------*\
	| get a buffer for ethernet driver. |
	\*---------------------------------*/

struct cb *getbuf(size)
int size;
{
	unsigned int etalloc();
	struct etcom *base;
	int s;

	size += HDR + HEAD + DATOFF + CRCSIZ + TAIL;
	if(size&1) size++;

	s = spl7();
	base = (struct etcom *) etalloc(size);
	splx(s);

	base->et_length = size;
	return (struct cb *) ((unsigned int) base + HDR + HEAD + CBOFF);
}

	/*---------------------------*\
	| release an ethernet buffer. |
	\*---------------------------*/

relbuf(adr)
unsigned int adr;
{
	unsigned int size;
	struct etcom *base;
	int s;

	base = (struct etcom *) (adr - HDR - HEAD - CBOFF);
	size = base->et_length;

	s = spl7();
	etfree(size, (int)base);
	splx(s);
}



	/*-------------------------------------------------------*\
	| The following is the memory management software used by |
	| NCF, PDLC, and et.c. It should be moved to its own file |
	| when the upper levels are being built and the et driver |
	| is working.						  |
	\*-------------------------------------------------------*/


	/*---------------------------------*\
	| Allocate 'size' bytes.	    |
	| Return the base of the allocated  |
	| space.			    |
	\*---------------------------------*/

unsigned int etalloc(size)
{
	register unsigned int a;
	register struct map *bp;
	extern struct map map[];

	while (1) {
		spl7();
		for (bp=map; bp->m_size; bp++) {
			if (bp->m_size >= size) {
				a = bp->m_addr;
				bp->m_addr += size;
				if ((bp->m_size -= size) == 0) {
					do {
						bp++;
						(bp-1)->m_addr = bp->m_addr;
					} while ((bp-1)->m_size = bp->m_size);
				}
				spl0();
				return(a);
			}
		}
		return 0;
	}
}



	/*--------------------------------------------------*\
	| Free the previously allocated space starting at a  |
	| of 'size' bytes.				     |
	| Disable interrupts when useing.		     |
 	\*--------------------------------------------------*/

etfree(size, a)
register int a;
{
	register struct map *bp;
	register unsigned int t;
	extern struct map map[];

	bp = map;
	for (; bp->m_addr<=a && bp->m_size!=0; bp++);
	if (bp>map && (bp-1)->m_addr+(bp-1)->m_size == a) {
		(bp-1)->m_size += size;
		if (a+size == bp->m_addr) {
			(bp-1)->m_size += bp->m_size;
			while (bp->m_size) {
				bp++;
				(bp-1)->m_addr = bp->m_addr;
				(bp-1)->m_size = bp->m_size;
			}
		}
	} else {
		if (a+size == bp->m_addr && bp->m_size) {
			bp->m_addr -= size;
			bp->m_size += size;
		} else if (size) {
			do {
				t = bp->m_addr;
				bp->m_addr = a;
				a = t;
				t = bp->m_size;
				bp->m_size = size;
				bp++;
			} while (size = t);
		}
	}
}



	/*--------------------------------------------*\
	| Test interfaces for special character device |
	\*--------------------------------------------*/

etopen(dev,flag)
dev_t dev;
int flag;
{
	int t;
	t = minor(dev);
	if(!interlan)
		u.u_error = ENXIO;
}

etclose(dev,flag)
dev_t dev;
int flag;
{
}

etread(dev)
{
	struct cb *tptr, *et_read();
	char *ptr;
	int len;

	tptr = et_read();
	ptr = (char *) &tptr->cb_data[0];
	len = min(tptr->cb_size, u.u_count);

	for(len; len; len--)
		passc(*ptr++);

	relbuf(tptr);
}

etwrite(dev)
{
	int t,len,i,status;
	struct cb *cptr;
	struct cb *getbuf();

	t = minor(dev);

	i = min(TBSIZE, u.u_count);

	cptr = getbuf(i);

	cpyinb(u.u_base, &cptr->cb_data[0], i, u.u_segflg);


	cptr->cb_dest = t;
	cptr->cb_type = 0;
	cptr->cb_size = i;
	status = et_write(cptr);

	relbuf(cptr);

	/* set up next transmission */
	u.u_count -= i;
	u.u_offset += i;
	u.u_base += i;

	u.u_error = (status>1)?EIO: 0;

}
