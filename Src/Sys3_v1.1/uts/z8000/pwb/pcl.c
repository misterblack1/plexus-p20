/*
 *	PCL-11 Multiplexing / Demultiplexing Driver
 *	Permits 8 two-way communications between 16 machines.
 */

# include "sys/param.h"
# include "sys/proc.h"
# include "sys/dir.h"
# include "sys/user.h"
# include "sys/pcl.h"
# include "sys/buf.h"
# ifdef pdp11
# include "sys/map.h"
# endif

# ifdef pdp11
paddr_t	pcl_uba;		/* Unibus address for transfers */
# endif
struct	buf	pcl_buf;	/* buffer header for UBM */
struct	pcl *	pcl_ioq;	/* head of linked list of transmissions */
struct	pcl	pclctrl;	/* pcl control channel interface */
struct	pcldb	pcldb[PCLINDX];	/* buffer for debug info */
int	pclindx;		/* index of next debug record */
struct	pcl *	pclspare;	/* available slot for conversation */

/*
 *	open: first time only - call initialization routine.
 *	Find available communications structure	for this channel.
 *	Check that channel is not in use or waiting for close
 *	message from remote.
 */

pclopen(dev)
	int	dev;
{
	register  struct  pcl  *p;

	if(pcl_buf.b_flags == 0)
		pclinit();
	spl5();
	do {
		if(p = pclsrch(dev)) {
			if(p->pcl_flag & P_OPEN)
				u.u_error = EBUSY;
		} else if((p = pclspare) == NULL)
			u.u_error = ENOSPC;
		if(u.u_error) {
			spl0();
			return;
		}
	} while(pclwait(p));
	p->pcl_dev = dev;
	p->pcl_flag = p->pcl_flag & P_ROPEN | P_OPEN | P_RETRY;
	p->pcl_pgrp = u.u_procp->p_pgrp;
	p->pcl_icnt = p->pcl_ioff = 0;
	p->pcl_hdr = PCLOPEN | pclchan(dev);
	p->pcl_ocnt = 10;
	pclqueuer(p);
	pclwait(p);
	if(p->pcl_flag & P_XERR) {
		u.u_error = EIO;
		p->pcl_flag &= P_ROPEN;
	}
	spl0();
}

/*
 *	close: transmit close message to remote.  If header is
 *	set, a transmission is pending and pclxintr will send close
 *	message on completion.
 */

pclclose(dev)
	int	dev;
{
	register  struct  pcl  *p;

	spl5();
	p = pclsrch(dev);
	p->pcl_flag &= P_ROPEN;
	if(p->pcl_hdr == 0) {
		p->pcl_hdr = PCLCLOSE | pclchan(dev);
		p->pcl_ocnt = 0;
		pclqueuer(p);
	}
	spl0();
}

/*
 *	read: locate corresponding communication structure.
 *	Return characters from buffer, then EOF indication
 *	else wait for next transmission.  Last done only if
 *	remote is still open.
 */

pclread(dev)
	int	dev;
{
	register  struct  pcl  *p;
	register  int  c;

	spl5();
	p = pclsrch(dev);
  retry:
	if(p->pcl_icnt) {
		c = min(u.u_count, p->pcl_icnt);
		move(p->pcl_ibuf + p->pcl_ioff, c, B_READ);
		p->pcl_icnt -= c;
		p->pcl_ioff += c;
	} else if(p->pcl_flag & P_REOF)
		p->pcl_flag &= ~P_REOF;
	else if(p->pcl_flag & P_RERR) {
		p->pcl_flag &= ~P_RERR;
		u.u_error = EIO;
	} else if(p->pcl_flag & P_ROPEN) {
		p->pcl_flag |= P_READ;
		sleep((caddr_t) p, PCLRPRI);
		goto retry;
	} else
		u.u_error = EBADF;
	spl0();
}

/*
 *	write: wait until header is available for use.
 *	Gather up user data into output buffer and arrange
 *	for transmission.  Wait for completion.
 */

pclwrite(dev)
	int	dev;
{
	register  struct  pcl  *p;

	spl5();
	p = pclsrch(dev);
	if((p->pcl_flag & P_ROPEN) == 0) {
		u.u_error = EBADF;
		spl0();
		return;
	}
	/* should add separate wait for xmit and busy */
	pclwait(p);
	if(u.u_count == 0) {
		p->pcl_hdr = PCLEOF | pclchan(p->pcl_dev);
		p->pcl_ocnt = 10;	/* send data to permit rejection */
		pclqueuer(p);
		pclwait(p);
	} else while(u.u_count && (p->pcl_flag & P_XERR) == 0) {
		p->pcl_ocnt = min(u.u_count, PCLBSZ);
		if(move(p->pcl_obuf, p->pcl_ocnt, B_WRITE))
			break;
		if(p->pcl_ocnt & 01) {
			p->pcl_ocnt++;
			p->pcl_hdr = PCLODATA | pclchan(p->pcl_dev);
		} else
			p->pcl_hdr = PCLEDATA | pclchan(p->pcl_dev);
		pclqueuer(p);
		pclwait(p);
	}
	if(p->pcl_flag & P_XERR) {
		p->pcl_flag &= ~P_XERR;
		u.u_error = EIO;
	}
	pclwake(p, P_WRITE);
}

/*
 *	start: prepare for transmission.  Verify that channel is still open
 *	as this may be a retry.  Exception: close message (of course).
 */

pclxstart()
{
	register  struct  pcl  *p;
	register  struct  pclhw  *hw;
#	ifdef pdp11
	paddr_t	uba;
#	else
	register  int  uba;
	extern  _sdata;
#	endif

	if((p = pcl_ioq) == NULL)
		return;
	if((p->pcl_flag & P_OPEN) == 0) {
		p->pcl_ocnt = 0;
		p->pcl_hdr = PCLCLOSE | pclchan(p->pcl_dev);
	}
	hw = *pcl_addr;
	pcl_buf.b_flags |= B_BUSY;
	p->pcl_flag &= ~P_XERR;
	hw->pcl_tcr = TXINIT;
	hw->pcl_tcr |= IE;
#	ifdef pdp11
	uba = pcl_uba + p->pcl_obuf - pcl_buf.b_paddr;
#	else
	uba = (int) p->pcl_obuf - (int) (&_sdata);
#	endif
	hw->pcl_tba = (short) uba;
	hw->pcl_tcr |= (short) ((uba & EABITS) >> EAOFF);
	hw->pcl_tbc = -(p->pcl_ocnt + sizeof pcl_pcl[0].pcl_hdr);
	hw->pcl_tdb = p->pcl_hdr;
	hw->pcl_tcr |= (pclmach(p->pcl_dev) << DSTOFF) |
	    SNDWD | STTXM | TXNPR | RIB;
	pcldebug(4, hw->pcl_tcr, hw->pcl_tsr, p->pcl_hdr);
}

/*
 *	start: prepare for reception.
 */

pclrstart(p)
	register  struct  pcl  *p;
{
	register  struct  pclhw  *hw;
#	ifdef pdp11
	paddr_t	uba;
#	else
	register  int  uba;
	extern  _sdata;
#	endif

	hw = *pcl_addr;
	hw->pcl_rbc = -PCLBSZ;
#	ifdef pdp11
	uba = pcl_uba + p->pcl_ibuf - pcl_buf.b_paddr;
#	else
	uba = (int) p->pcl_ibuf - (int) (&_sdata);
#	endif
	hw->pcl_rba = (short) uba;
	hw->pcl_rcr = (short) (RCVDAT | RCNPR | IE |
	     ((uba & EABITS) >> EAOFF));
}

/*
 *	queuer: place transmission request at the end of the
 *	linked list of requests.  Start output if the list
 *	was previously empty.
 */

pclqueuer(p)
	register  struct  pcl  *p;
{
	register  int  sps;
	register  struct  pcl  *q;

	sps = spl5();
	p->pcl_ioq = NULL;
	if(pcl_ioq == NULL) {
		pcl_ioq = p;
		pclxstart();
	} else {
		for(q = pcl_ioq; q->pcl_ioq; q = q->pcl_ioq);
		q->pcl_ioq = p;
	}
	splx(sps);
}

/*
 *	receiver interrupt: examine incoming header.
 *	Reject transmission if channel not open or input buffer
 *	is not available.  Some headers are not followed
 *	by data - sending a signal, sending status, closing
 *	a channel - and cannot be rejected.
 */

pclRintr()
{
	register  int  rsr, dev;
	register  struct  pclhw  *hw;
	static  struct  pcl  *p;
	static  short  hdr;

	hw = *pcl_addr;
	rsr = hw->pcl_rsr;
	dev = (((hw->pcl_rcr & SRCPCL) >> SRCOFF) - 1) << 3;
	if(rsr & ERR)
		if(p == NULL)
			printf("pcl recv err\n");
		else {
			p->pcl_flag |= P_RERR;
			pclwake(p, P_READ);
			if(pclhdr(hdr) == PCLOPEN)
				p->pcl_flag &= ~P_ROPEN;
		}
	else if(rsr & REJCOM)
		;
	else if(rsr & SUCTXF && p != NULL) {
		p->pcl_ioff = 0;
		if(pclhdr(hdr) == PCLEOF) {
			p->pcl_flag |= P_REOF;
			p->pcl_icnt = 0;
			pclwake(p, P_READ);
		} else if(pclhdr(hdr) == PCLOPEN) {
			/* fetch uid and gid from buffer */
			p->pcl_flag |= P_ROPEN;
			pclwake(p, P_WOPEN);
		} else {
			p->pcl_icnt = PCLBSZ + hw->pcl_rbc -
			    (pclhdr(hdr) == PCLODATA ? 1 : 0);
			pclwake(p, P_READ);
		}
	} else if(rsr & DTORDY) {
		hdr = hw->pcl_rdb;
		dev |= pclchan(hdr);
		p = pclsrch(dev);
		switch(pclhdr(hdr)) {
		case PCLEOF:
		case PCLEDATA:
		case PCLODATA:
			if(p == NULL || (p->pcl_flag & P_OPEN) == 0)
				break;
			if(p->pcl_flag & P_REOF || p->pcl_icnt)
				hw->pcl_rcr |= REJ;
			else
				pclrstart(p);
			pcldebug(2, hw->pcl_rcr, rsr, hdr);
			return;
		case PCLOPEN:
			if(p) {
				if(p->pcl_flag & P_WASC)
					break;
				if(p->pcl_flag & P_ROPEN) {
					pclwake(p, P_READ | P_WRITE | P_WOPEN);
					p->pcl_flag &= ~P_ROPEN;
					break;
				}
			} else {
				if((p = pclspare) == NULL) {
					hw->pcl_rcr |= REJ;
					return;
				}
				p->pcl_dev = dev;
			}
			p->pcl_flag |= P_ROPEN;
			pclrstart(p);
			return;
		case PCLCTRL:
			p = &pclctrl;
			if((pclctrl.pcl_flag & P_OPEN) == 0)
				break;
			if(pclctrl.pcl_icnt)
				hw->pcl_rcr |= REJ;
			else
				pclrstart(&pclctrl);
			return;
		case PCLSIGNAL:
			if(p && p->pcl_flag & P_OPEN)
				signal(p->pcl_pgrp, (hdr >> 4) & 037);
			break;
		case PCLCLOSE:
			if(p) {
				p->pcl_flag &= ~P_ROPEN;
				pclwake(p, P_READ | P_WRITE | P_WOPEN);
				if(p->pcl_flag & P_OPEN)
					p->pcl_flag |= P_WASC;
			}
			break;
		default:
			printf("pcl bad hdr: %o\n", hdr);
		}
	} else
		printf("pcl rsr err %o\n", rsr);
	p = NULL;
	pcldebug(1, hw->pcl_rcr, rsr, hdr);
	hw->pcl_rcr = RCINIT;
	hw->pcl_rcr |= IE | RCVWD;
}

/*
 *	transmitter interrupt: wakeup writer on successful transmission.
 *	Also, those waiting for write buffer.  Attempt retry
 *	on rejection if retry bit is set.
 */

pclXintr()
{
	register  struct  pcl  *p;
	register  struct  pclhw  *hw;
	register  int  tsr;

	hw = *pcl_addr;
	tsr = hw->pcl_tsr;
	if((pcl_buf.b_flags & B_BUSY) == 0)
		printf("pcl xmit int %o\n", tsr);
	else if(tsr & (ERR | SORE | SUCTXF | TBSBSY)) {
		p = pcl_ioq;
		pcl_ioq = pcl_ioq->pcl_ioq;
		if(tsr & MSTDWN)
			hw->pcl_mmr |= MASTER;
		if(tsr & SORE && p->pcl_flag & P_RETRY)
			timeout(pclqueuer, p, PCLDELAY);
		else if(p->pcl_flag & P_OPEN) {
			if(tsr & (SORE | ERR | TBSBSY))
				p->pcl_flag |= P_XERR;
			p->pcl_hdr = NULL;
			pclwake(p, P_WRITE);
		} else if(pclhdr(p->pcl_hdr) != PCLCLOSE) {
			p->pcl_hdr = PCLCLOSE | pclchan(p->pcl_dev);
			p->pcl_ocnt = 0;
			pclqueuer(p);
		} else
			p->pcl_hdr = NULL;
	} else
		printf("pcl tsr err %o\n", tsr);
	pcl_buf.b_flags &= ~B_BUSY;
	pcldebug(3, hw->pcl_tcr, tsr, p->pcl_hdr);
	pclxstart();
}

/*
 *	search: locate a communication structure for a channel
 *	given the machine id and the logical channel.  Leave address
 *	of an available slot in pclspare.
 */

struct  pcl  *
pclsrch(dev)
	register  int  dev;
{
	register  struct  pcl  *p, *e;

	e = &pcl_pcl[pcl_cnt];
	pclspare = NULL;
	for(p = pcl_pcl; p != e; p++)
		if(p->pcl_flag & (P_OPEN | P_ROPEN)) {
			if(p->pcl_dev == dev)
				return(p);
		} else if(pclspare == NULL)
			pclspare = p;
	return(NULL);
}

/*
 *	wait: wait until output buffer and header are available
 *	for use in a transmission.
 */

pclwait(p)
	register  struct  pcl  *p;
{
	register  int  sps, ret = 0;

	sps = spl5();
	while(p->pcl_hdr) {
		ret++;
		p->pcl_flag |= P_WRITE;
		sleep((caddr_t) p + 1, PCLWPRI);
	}
	splx(sps);
	return(ret);
}

/*
 *	control: used to send control information across the
 *	link and synchronize the two sides.
 */

pclioctl(dev, cmd, arg, mode)
{
	register  struct  pcl  *p;
	struct	ctrlmsg {
		char	*addr;
		int	count;
	} cb;

	spl5();
	p = pclsrch(dev);
	if(p->pcl_flag & P_NOCTRL)
		u.u_error = ENOTTY;
	else switch(cmd) {
		case WAIT:
			while((p->pcl_flag & (P_ROPEN | P_WASC)) == 0) {
				p->pcl_flag |= P_WOPEN;
				sleep((caddr_t) p + 2, PCLRPRI);
			}
			if(p->pcl_flag & P_WASC)
				u.u_error = EBADF;
			break;
		case FLAG:
			p->pcl_flag &= ~(P_RETRY | P_NOCTRL);
			p->pcl_flag |= (arg & (P_RETRY | P_NOCTRL));
			break;
		case SIG:
			pclwait(p);
			p->pcl_hdr = PCLSIGNAL | pclchan(dev) |
			    ((arg & 037) << 4);
			p->pcl_ocnt = 0;
			pclqueuer(p);
			pclwait(p);
			break;
		case CTRL:
			if(p->pcl_flags & P_RSTR)
				u.u_error = EPERM;
			else if(copyin((char *) arg, (char *) &cb, sizeof cb))
				u.u_error = EFAULT;
			else if(cb.count == 0 || cb.count > PCLBSZ)
				u.u_error = EINVAL;
			else {
				pclwait(p);
#				ifdef pdp11
				if(copyio(p->pcl_obuf,cb.addr,cb.count,U_WUD)) {
#				else
				if(copyin(cb.addr, p->pcl_obuf, cb.count)) {
#				endif
					u.u_error = EFAULT;
					break;
				}
				p->pcl_ocnt = cb.count;
				p->pcl_hdr = PCLCTRL | pclchan(dev);
				pclqueuer(p);
				pclwait(p);
				if(p->pcl_flag & P_XERR) {
					p->pcl_flag &= ~P_XERR;
					u.u_error = EIO;
				}
				pclwake(p, P_WRITE);
			}
			break;
		case RSTR:
			p->pcl_flags |= P_RSTR;
			break;
		default:
			u.u_error = EINVAL;
		}
	spl0();
}


/*
 *	wakeup: activate roadblocked process.
 */

pclwake(p, flag)
	register  struct  pcl  *p;
	register  int  flag;
{
	if(p->pcl_flag & P_READ)
		wakeup((caddr_t) p);
	if(p->pcl_flag & P_WRITE)
		wakeup((caddr_t) p + 1);
	if(p->pcl_flag & P_WOPEN)
		wakeup((caddr_t) p + 2);
	p->pcl_flag &= ~flag;
}

/*
 *	debug stuff: circular array of values captured at appropriate times.
 *	Contains type of record, command register, status register,
 *	and pcl structure header.
 */

pcldebug(t, c, s, h)
	int	t, c, s, h;
{
	pcldb[pclindx].pcl_record = t;
	pcldb[pclindx].pcl_cmdreg = c;
	pcldb[pclindx].pcl_statreg = s;
	pcldb[pclindx].pcl_header = h;
	if(++pclindx == PCLINDX)
		pclindx = 0;
}

/*
 *	init: allocate physical memory for buffers.  Insure the
 *	existance of a bus master.  Initialize buffer addresses.
 */

# ifdef pdp11

pclinit()
{
	register  struct  pclhw  *hw;
	register  struct  pcl  *p;
	register  char  (*c)[PCLBSZ] = 0;
	paddr_t  pa;
	int	b;

	b = PCLBSZ * 2 * (pcl_cnt + 1);
	if((pa = ctob((paddr_t) (unsigned) malloc(coremap, btoc(b)))) ==
	    (paddr_t) 0) {
		u.u_error = ENOMEM;
		return;
	}
	pcl_buf.b_paddr = pa;
	for(p = pcl_pcl; p != &pcl_pcl[pcl_cnt]; p++) {
		p->pcl_ibuf = pa + (paddr_t)(unsigned)c++;
		p->pcl_obuf = pa + (paddr_t)(unsigned)c++;
	}
	pclctrl.pcl_ibuf = pa + (paddr_t)(unsigned)c;
	pcl_buf.b_flags = B_PHYS;
	pcl_buf.b_bcount = b;
	pcl_uba = ubmaddr(&pcl_buf, ubmalloc(b));
	hw = *pcl_addr;
	hw->pcl_mmr |= MASTER;
	hw->pcl_rcr = RCINIT;
	hw->pcl_rcr |= IE | RCVWD;
}

# else

pclinit()
{
	register  struct  pclhw  *hw;

	pcl_buf.b_flags = B_PHYS;
	hw = *pcl_addr;
	hw->pcl_mmr |= MASTER;
	hw->pcl_rcr = RCINIT;
	hw->pcl_rcr |= IE | RCVWD;
}

# endif

/*
 *	control open: call initialization routine.  Insure
 *	exclusive use.
 */

pclcopen()
{
	if(pcl_buf.b_flags == 0)
		pclinit();
	if(pclctrl.pcl_flag & P_OPEN)
		u.u_error = EBUSY;
	else
		pclctrl.pcl_flag = P_OPEN;
}

/*
 *	control close: unlock control channel.  Discard any
 *	un-read data.
 */

pclcclose()
{
	spl5();
	pclctrl.pcl_icnt = 0;
	pclctrl.pcl_flag = 0;
	spl0();
}

/*
 *	control read: wait for stty control message.
 */

pclcread()
{
	register  struct  pcl  *p;

	p = &pclctrl;
	spl5();
	while(p->pcl_icnt == 0) {
		p->pcl_flag |= P_READ;
		sleep((caddr_t) p, PCLRPRI);
	}
	spl0();
	move(p->pcl_ibuf, min(u.u_count, p->pcl_icnt), B_READ);
	spl5();
	p->pcl_flag &= ~P_READ;
	p->pcl_icnt = 0;
	spl0();
}
