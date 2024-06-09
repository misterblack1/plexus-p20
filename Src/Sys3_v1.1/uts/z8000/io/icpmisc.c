#include "sys/param.h"
#include "sys/map.h"
#ifdef VPMSYS
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/ioctl.h"
#include "sys/vpmd.h"
#include "sys/uba.h"
#include "sys/tty.h"
#endif

#include "icp/siocprom.h"	/* icp specific */
#include "icp/sioccomm.h"	/* icp specific */
#include "icp/icp.h"		/* icp specific */
#include "icp/pbsioc.h"		/* icp specific */
#ifdef VPMSYS
#include "icp/siocunix.h"       
#endif

/*
 * This file contains miscellaneous code that supports the icp
 * drivers.
 */


#define	AWRITE	4

#ifdef VPMSYS
extern siioctl();
extern short word1, word2;
extern struct vpmb vpmb[];

/* Buffer descriptors */
#define XBQMAX   2                  /* Number of buffer descriptors */
/*
#define BUFDES   (XBQMAX + 2)*VPMBS 
*/
#define BUFDES   1                  /* TEMPORARY */
struct vpmbd vpmbd[BUFDES];
#endif
/*
 * sioci - general routine for processing SIOC interrupt.  The correct SIOC
 *         control structure is passed on the call.
 *
 * Currently there are only three reasons that the SIOC will interrupt
 * the processor board.  1) Used to indicate that the previous 
 * command has been accepted by the SIOC, thus freeing the SIOC output 
 * communications area for another command.  2) Used to wake-up
 * a process waiting for the SIOC to complete a command.  3) Used to
 * signal a group of processes when a signal is issued from the device.
 *
 */
long	tsioci, tssst, tsack, tack, twak;
unsigned long	flshsent, flshrecv;
int	dbug;
#ifdef SEQ
int lastseq;
int lastcmd;
#endif

int	lstoutm;

sioci( ctl )
register struct	sctl	*ctl;
{

	register struct	scb	*sp;
#ifdef VPMSYS
	register struct vpmb *vp;
	register struct vpmbd   *dp;
#endif
	long	i;
	int	siwflsh(), unit;
#ifdef VPMSYS
	int	index, j;
        char type;
#endif

	/*
	 * The SIOC input communications area has a command in it.  Set-up
	 * the pointer to the communications area and switch on the command.
	 */
	tsioci++;
	sp = (struct scb *)ctl->sc_iaddr;
#ifdef SEQ
lastcmd = sp->si_cmd;
lastseq = sp->si_un.gdc2.si_seq;
#endif
	switch ( sp->si_cmd ) {

	/* SIOC is indicating that the previous command has been accepted.
	 * Reset the SIOC interrupt and see if there are anymore commands
	 * to issue.  If there are, issue the next one.  Otherwise stop
	 * until sstart is called again.
	 */
	case SIOCACK:
		ctl->sc_intseq++;
		tack++;
		out_multibus( ctl->sc_port, SIOCIRST );
		/*
		printf( "reset - a\n" );
		*/
		for ( i=0; i < 250; i++ ) { }
		if ( ctl->s_oq.sq_cnt > 0 ) {
			tsack++;
			dqsqo( ctl );
/*
			if ( dbug )
				printf( "int issueed - i!\n" );
*/
			out_multibus( ctl->sc_port, SIOCINT );
			/*
			printf( "int - a\n" );
			*/
		} else {
			if ( ctl->sc_oqrun != 1 ) {
				printf( "sc_oqrun != 0\n" );
				sysdebug( 12 );
			}
			ctl->sc_oqrun--;
		}
		break;

	/* SIOC is issuing a wake-up to a process waiting for a command
	 * to complete.  Wake-up the process and reset the SIOC interrupt.
	 */
	case SIOCWUP:
		twak++;
		if ( twak > tack ) {
			printf( "twak/tack\n" );
			sysdebug( 12 );
		}
		wakeup( (caddr_t) sp->si_un.gdc2.si_chan );
		lstoutm = 4;
		out_multibus( ctl->sc_port, SIOCIRST );
		/*
		printf( "reset - w\n" );
		*/
		break;

	/*
	 * SIOC has completed a flushed write.
	 * Return buffers and clear flags.
	 */

	case SIOCFLSH:
		flshrecv++;
		unit = sp->si_un.gdc4.si_ctlu->cu_unit;
		ctl->tp_un[unit].sc_tstat |= WOK;
		ctl->tp_un[unit].sc_timstate = 0;
		ctl->tp_un[unit].sc_bspot = 0;
		clrbufs( ctl, unit, AWRITE );
		wakeup( (caddr_t)&ctl->tp_un[unit] );
		out_multibus( ctl->sc_port, SIOCIRST );
		break;

	/*
	 * SIOC is issuing a signal to a process.  Send the signal to the 
	 * process and reset the SIOC interrupt.
	 */
	case SIOCSIG:
/*
		if ( dbug )
		printf( "sig = %x, for %x unit = %x\n", sp->si_un.gdc3.si_msig, sp->si_un.gdc3.si_mpgrp, sp->si_un.gdc3.si_unit );
*/
		signal( sp->si_un.gdc3.si_mpgrp, sp->si_un.gdc3.si_msig );
/*
		if ( dbug )
			printf( "sig rcvd!\n" );
*/
		lstoutm = 5;
		out_multibus( ctl->sc_port, SIOCIRST );
/*
		printf( "reset - s\n" );
*/
		break;
#ifdef VPMSYS
         
	/*
	 * ICP is sending a report.
	 * Process and reset the ICP interrupt.
	 */
	case VPMRPTI:
/*
		if ( dbug )
		printf( "vpmrpt = %x, %x, %x, %x, unit = %x\n", sp->si_un.gdc5.si_parm[1],sp->si_un.gdc5.si_parm[2],sp->si_un.gdc5.si_parm[3], sp->si_un.gdc5.si_parm[4], sp->si_un.gdc5.si_parm[0]);
		if ( dbug )
			printf( "vpmrpt rcvd!\n" );
*/
		lstoutm = 7;
		/* vp = (ICP # times 4 plus protocol #) */
	        vp = &vpmb[IDEV(sp->si_un.gdc5.si_parm[0]) * 4 +
	                        BDEV(sp->si_un.gdc5.si_parm[0])];
		for(j=0; j<4; j++) {
		     vp->vb_rpt[j] = (char)sp->si_un.gdc5.si_parm[j+1];
                }
		vp->vb_state |= VPMRPTA;

                type = 'P';
		goto dovpmsave;


	/*
	 * ICP is sending a report.
	 * Process and reset the ICP interrupt.
	 */
	case VPMERT:
/*
		if ( dbug )
		printf( "vpmert = %x, %x, %x, %x, unit = %x\n", sp->si_un.gdc5.si_parm[1],sp->si_un.gdc5.si_parm[2],sp->si_un.gdc5.si_parm[3], sp->si_un.gdc5.si_parm[4], sp->si_un.gdc5.si_parm[0]);
		if ( dbug )
			printf( "vpmert rcvd!\n" );
*/
		lstoutm = 8;

		out_multibus( ctl->sc_port, SIOCIRST );

		word1 = (sp->si_un.gdc5.si_parm[1] & 0x00ff) <<8 |
			(sp->si_un.gdc5.si_parm[2] & 0xff);
		word2 = (sp->si_un.gdc5.si_parm[3] & 0x00ff) <<8 |
			(sp->si_un.gdc5.si_parm[4] & 0xff);
		vpmsave('E',sp->si_un.gdc5.si_parm[0], word1, word2);

		vpmclean(sp->si_un.gdc5.si_parm[0]);
		break;



	/*
	 * ICP is sending a report.
	 * Process and reset the ICP interrupt.
	 */
	case VPMTRC:
/*
		if ( dbug )
		printf( "vpmtrc = %x, %x, %x, %x, unit = %x\n", sp->si_un.gdc5.si_parm[1],sp->si_un.gdc5.si_parm[2],sp->si_un.gdc5.si_parm[3], sp->si_un.gdc5.si_parm[4], sp->si_un.gdc5.si_parm[0]);
		if ( dbug )
			printf( "vpmtrc rcvd!\n" );
*/
		lstoutm = 9;

		type = 'T';
		goto dovpmsave;

	/*
	 * ICP is sending a report.
	 * Process and reset the ICP interrupt.
	 */
	case VPMSNAP:
/*
		if ( dbug )
		printf( "vpmsnp = %x, %x, %x, %x, unit = %x\n", sp->si_un.gdc5.si_parm[1],sp->si_un.gdc5.si_parm[2],sp->si_un.gdc5.si_parm[3], sp->si_un.gdc5.si_parm[4], sp->si_un.gdc5.si_parm[0]);
		if ( dbug )
			printf( "vpmsnp rcvd!\n" );
*/
		lstoutm = 10;

		type = 'S';
		goto dovpmsave;

		break;

	/*
	 * ICP is sending a report.
	 * Process and reset the ICP interrupt.
	 */
	case VPMSTUP:
/*
		if ( dbug )
		printf( "vpmstr = %x, %x, %x, %x, unit = %x\n", sp->si_un.gdc5.si_parm[1],sp->si_un.gdc5.si_parm[2],sp->si_un.gdc5.si_parm[3], sp->si_un.gdc5.si_parm[4], sp->si_un.gdc5.si_parm[0]);
		if ( dbug )
			printf( "vpmstr rcvd!\n" );
 */
		lstoutm = 11;

                type = 'S';

        dovpmsave:

		out_multibus( ctl->sc_port, SIOCIRST );

		word1 = (sp->si_un.gdc5.si_parm[1] & 0x00ff) <<8 |
			(sp->si_un.gdc5.si_parm[2] & 0xff);
		word2 = (sp->si_un.gdc5.si_parm[3] & 0x00ff) <<8 |
			(sp->si_un.gdc5.si_parm[4] & 0xff);
		vpmsave(type,sp->si_un.gdc5.si_parm[0], word1, word2);

		break;

	/* 
	 * ICP is sending a report
	 * Process and reset the ICP interrupt.
	 */
        case VPMRTNX:
 /*
		printf( "rtnxb rcvd!\n" );
 */
		lstoutm = 12;

		vpmsave('X',sp->si_un.gdc5.si_parm[0], 0, 0);

	        dp = &vpmbd[IDEV(sp->si_un.gdc5.si_parm[0])];

		dp->d_type = sp->si_un.gdc5.si_parm[1] & 0xff;
		dp->d_sta  = sp->si_un.gdc5.si_parm[2] & 0xff;
		dp->d_dev  = sp->si_un.gdc5.si_parm[3] & 0xff;

         dovbrint:

		out_multibus( ctl->sc_port, SIOCIRST );

		/* vp = (ICP # times 4 plus protocol #) */
	        vp = &vpmb[IDEV(sp->si_un.gdc5.si_parm[0]) * 4 +
	                        BDEV(sp->si_un.gdc5.si_parm[0])];
    

		vp->vb_state &= ~VPMWIP;
/*
                printf("dovbrint:*vp->vb_rint =%x\n",*vp->vb_rint);
*/
		if(*vp->vb_rint) {
  
		     word1 = (sp->si_un.gdc5.si_parm[1] & 0x00ff) <<8 |
			     (sp->si_un.gdc5.si_parm[2] & 0xff);
		     word2 = (sp->si_un.gdc5.si_parm[3] & 0x00ff) <<8 |
			     (sp->si_un.gdc5.si_parm[4] & 0xff);
	             (*vp->vb_rint)(sp->si_un.gdc5.si_parm[0],sp->si_cmd,
	                                (struct vpmd *)ubmrev(word1,word2));

                }
		break;

	/* 
	 * ICP is sending a report
	 * Process and reset the ICP interrupt.
	 */
        case VPMRTNR:
		lstoutm = 13;

		vpmsave('R',sp->si_un.gdc5.si_parm[0], 0, 0);

		goto dovbrint;

#endif

	/*
	 * There should not be any that fall through to "default".  Bump
	 * a counter for debug purposes.
	 */
	default:
		ctl->sc_bad++;
		out_multibus( ctl->sc_port, SIOCIRST );
/*
		printf( "reset - d\n" );
*/

	}
}
#ifdef VPMSYS

/*
 * vpmstart - startup a VPM associated with device 'dev'.
 */

vpmstart(dev, rint)
int (*rint)();
{
     register struct vpmb *vp;
     int arg, mode, type;

     if(BDEV(dev) >= VPMBS)
	  return(ENXIO);

     vp = &vpmb[IDEV(dev) * 4 + BDEV(dev)];

     if(vp->vb_howmany || vp->vb_state&OPEN)
	  return(EACCES);

     vp->vb_state |= OPEN;
     vp->vb_rint = rint;
     
     word1 = (vp->vb_state <<8) & 0xff00;
     vpmsave('s', dev, word1, 0);
     return(0);
}
#ifdef VPMINMOD

/*
 * vpmstop - stop a VPM associated with device 'dev'.
 */

vpmstop(dev)
{
     register struct vpmb *vp;
     int arg, mode;

     vp = &vpmb[IDEV(dev) * 4 + BDEV(dev)];
     vp->vb_state |= WCLOSE;
     
     if(vp->vb_howmany)
	  return;

     /* now send a HALT command to the ICP */

     siioctl(TDEV(dev)/NUMSIO, TDEV(dev), HLTCMD, arg, mode, dev, 1);
   
     word1 = (vp->vb_state <<8) & 0xff00;
     vpmsave('t', dev, word1, 0);

}

/*
 * vpmcmd - send a command to the protocol script
 *
 */

vpmcmd(dev,cmd)
int dev;
char cmd[];
{
     int unit, index, mode;

     /* now send a SCRIPT command to the ICP */
     unit = TDEV(dev);
     index = unit/NUMSIO;
     siioctl(index, unit, VPMCMD, cmd, mode, dev, 1);
}

/*
 * vpmrpt - get the last report sent by the protocol script
 */
char *
vpmrpt(dev)
{
     register struct vpmb *vp;
     
     /* vp = (ICP# times 4 plus protocol #) */
     vp = &vpmb[IDEV(dev) *  4 + BDEV(dev)];

     if(vp->vb_state&VPMRPTA) {
	  vp->vb_state &= ~VPMRPTA;
          return(vp->vb_rpt);
     }
     return(0);
}

/*
 * vpmerrs - get the last error counts sent by the protocol script
 */
char *
vpmerrs(dev,n)
int dev,n;
{
     register struct vpmb *vp;
     
     int unit, index, mode;

     /* vp = (ICP# times 4 plus protocol #) */
     vp = &vpmb[IDEV(dev) *  4 + BDEV(dev)];

     /* now send a VPMERRS request to the ICP */
     unit = TDEV(dev);
     index = unit/NUMSIO;
     siioctl(index, unit, VPMERRS, vp->vb_errs, mode, dev, 1);
}
#endif

/*
 * vpmclean - cleans up after an error report from the icp
 */

vpmclean(dev)
{
     register struct vpmb *vp;
     
     /* vp = (ICP# times 4 plus protocol #) */
     vp = &vpmb[IDEV(dev) *  4 + BDEV(dev)];
     if((vp->vb_state&WCLOSE) == 0)
	  return;

     word1 = ((vp->vb_state & ~WCLOSE) <<8) & 0xff00;

     vpmsave('C',dev, word1, 0);

     vp->vb_state |= VPMERR;
}
#ifdef VPMINMOD
/*
 * vpmdeq - This function removes the buffer-descriptor pointer
 *          at the head of the queue pointed to by 'clp' and
 *          returns it to the caller.  If the queue is empty,
 *          a null pointer is returned.
 */

struct vpmd *
vpmdeq(clp)
struct clist *clp;
{
     return(0);
}

/*
 * vpmenq - If 'bdp' is a null pointer, the number of buffer   
 *          descriptor pointers on the 'clist' queue pointed
 *          to by 'clp' is returned.  If 'bdp' is not a null 
 *          pointer, the buffer descriptor pointed to by 'bdp'
 *          is appended to the 'clist' queue pointed to by 'clp'
 *          and the number of pointers currently on that queue
 *          is passed as the return value.
 */

vpmenq(bdp,clp)
struct vpmbd *bdp;
struct clist *clp;
{
     return(0);
}

/*
 * vpmemptq-This function is used to pass an empty receive buffer
 *          for use by the interpreter in the ICP.  'bdp' is a 
 *          pointer to a buffer descriptor or null.  If 'bdp' is
 *          not a null pointer, the buffer descriptor is appended
 *          to the empty-receive-buffer queue for the interface 
 *          module specified by 'dev'.  If the VPM interpreter 
 *          currently has room for another empty receive buffer,
 *          the buffer at the head of the queue is removed and passed
 *          to the ICP.  The sum of the number of buffers on the
 *          empty-receive buffer queue and the number of receive
 *          buffers the VPM interpreter has in its queues is
 *          returned to the caller.  If 'bdp' is a null pointer,
 *          the above sum is returned and nothing else is done.
 */

vpmemptq(dev,bdp)
struct vpmbd *bdp;
{
     return(0);
}
#endif

/*
 * vpmsave - creates an event record.
 */

vpmsave(type,dev, word1, word2)
char type,dev;
short word1, word2;
{
     static int vpmseqn;
     register sps;
     struct {
	     short   e_seqn;
	     char    e_type;
	     char    e_dev;
	     short   e_word1;
	     short   e_word2;
     } vpment;

     sps = spl5();
     if(vpmseqn >= 077777)
	  vpmseqn = 0;
     vpment.e_seqn = ++vpmseqn;
     vpment.e_type = type;
     vpment.e_dev = dev;
     vpment.e_word1 = word1;
     vpment.e_word2 = word2;
     trsave(0,0,&vpment,sizeof(vpment));
     splx( sps );
}

/*
 * vpmsnap - creates an time-stamed event record.
 */

vpmsnap(type,dev,word1,word2)
char type,dev;
short word1, word2;
{
     static int vpmseqn;
     register sps;
     struct {
	     short   s_seqn;
	     char    s_type;
	     char    s_dev;
	     short   s_word1;
	     short   s_word2;
	     long    s_lbolt;
     } vpmsnt;

     sps = spl5();
     if(vpmseqn >= 077777)
	  vpmseqn = 0;
     vpmsnt.s_seqn = ++vpmseqn;
     vpmsnt.s_type = type;
     vpmsnt.s_dev = dev;
     vpmsnt.s_word1 = word1;
     vpmsnt.s_word2 = word2;
     trsave(1,0,&vpmsnt,sizeof(vpmsnt));
     splx( sps );
}

/* 
 * VPMTRINT - THIS CODE SHOULD EVENTUALLY BE MOVED TO ITS
 *            OWN MODULE, AS IT IS THE "TOP" PORTION OF THE
 *            VPM DRIVER.  FOR NOW IT STAYS HERE.
 */

vpmtrint(dev, type, bdp)
{
     register struct vpmb *vp;

     /* vp = (ICP# times 4 plus protocol #) */
     vp = &vpmb[IDEV(dev) *  4 + BDEV(dev)];

     switch(type) {
	     case VPMRTNX:
		     wakeup((caddr_t)&vp->vb_state);
		     break;
             case VPMRTNR:
		     break;
             case VPMERRS:
		     vp->vb_state |= VPMERR;
		     wakeup((caddr_t)&vp->vb_state);
		     break;
     }
}
#endif

/*
 * sstart - starts the processing of the SIOC interrupt command queue
 * if it is not already running.
 */

sstart( q )
register struct sctl *q;
{
	int	s;

	/*
	 * If the interrupt process is not running and the command queue
	 * is not empty, issue the next command to the SIOC.
	 */
	s = spl6();
	if ( !q->sc_oqrun && ( q->s_oq.sq_cnt > 0 ) ) {
		tssst++;
		dqsqo( q );
/*
		if ( dbug )
			printf( "int out!\n" );
*/
		q->sc_intseq++;
		q->sc_oqrun++;
		lstoutm = 6;
		out_multibus( q->sc_port, SIOCINT );
/*
		printf( "int - s\n" );
*/
	}
	splx( s );
}
/*
 * This file contains code that manages the SIOC interrupt command output
 * queue.
 */


extern	int	dbug;
/* qsqo - queues the pointer to an SIOC interrupt command block on
 * the specified SIOC output command queue.
 */

qsqo( addr, q, pri )
int	*addr;
register struct	sctl	*q;
{
	int	s;

	s = spl6();
	for (;;) {
		/*
		 * If the queue is not full, queue this command and call sstart
		 * to start the output queue processing when it is not running.
		 * If the queue is full, sleep on the count of entries in 
		 * the queue.  The de-queuer (dqsqo) will issue a wakeup when
		 * an entry is taken off the queue.
		 */
		if ( q->s_oq.sq_cnt < SQSIZE ) {
			q->s_oq.sq_e[q->s_oq.sq_in].sq_addr = addr;
			if ( ++q->s_oq.sq_in == SQSIZE )
				q->s_oq.sq_in = 0;
			q->s_oq.sq_cnt++;
			sstart( q );
			break;
		} else sleep( (caddr_t)&q->s_oq.sq_cnt, pri );
	}
/*
	if ( dbug )
		printf( "qsqo!\n" );
*/
	splx( s );
}
/* dqsqo - dequeues an SIOC interrupt command block and moves it to the
 * SIOC output communications area.
 */

dqsqo( q ) 
register struct sctl *q;
{

	register struct	scbw	*ca;
	register	i, j, len, s;

	/*
	 * If the queue is not empty, move the next command to the output
	 * communications area.  Always wakeup those processes waiting on
	 * queue space.
	 */
	s = spl6();
	if ( q->s_oq.sq_cnt > 0 ) {
		ca = (struct scbw *)q->sc_oaddr;
		len = *q->s_oq.sq_e[q->s_oq.sq_out].sq_addr;
		if ( len & 1 )
			len++;
		for ( i = 0, j = 0; j < len; i++, j += 2 )
			ca->cbw[i] = *q->s_oq.sq_e[q->s_oq.sq_out].sq_addr++;
		if ( ++q->s_oq.sq_out == SQSIZE )
			q->s_oq.sq_out = 0;
		q->s_oq.sq_cnt--;
		wakeup( (caddr_t)&q->s_oq.sq_cnt );
	}
/*
	if ( dbug )
		printf( "dqsqo!\n" );
*/
	splx( s );
}

/*
 * defines
 */

#define MAPSIZE 128 

/*
 * static variables
 */

char roomneeded;
char siocbuf[BUFSIZE];
struct map map[MAPSIZE] = {
	BUFSIZE, (unsigned short) siocbuf,
};

/*
 * Allocate 'size' bytes.
 * Return the base of the allocated
 * space.
 * sleep on roomneeded if there is no space.
 */
smalloc(size)
{
	register unsigned int a;
	register struct map *bp;
	register int s;

	while (1) {
		s = spl7();
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
				splx(s);
				return(a);
			}
		}
		roomneeded = 1;
		sleep((caddr_t)&roomneeded, SIOCPRI);
	}
}

/*
 * Free the previously allocated space starting at a 
 * of 'size' bytes.
 */
smfree(size, a)
register int a;
{
	register struct map *bp;
	register unsigned int t;
	register int s;

	s = spl7();
	if (roomneeded) {
		roomneeded = 0;
		wakeup((caddr_t)&roomneeded);
	}
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
	splx(s);
}
