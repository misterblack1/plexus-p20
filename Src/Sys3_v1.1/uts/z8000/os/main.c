#include "sys/plexus.h"
#include "sys/param.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/filsys.h"
#include "sys/mount.h"
#include "sys/proc.h"
#include "sys/inode.h"
#include "sys/seg.h"
#include "sys/conf.h"
#include "sys/buf.h"
#include "sys/tty.h"
#include "sys/map.h"
#include "sys/var.h"

/*
 * Initialization code.
 * Called from cold start routine as
 * soon as a stack and segmentation
 * have been established.
 * Functions:
 *	clear and free user core
 *	turn on clock
 *	hand craft 0th process
 *	call all initialization routines
 *	fork - process 0 to schedule
 *	     - process 1 execute bootstrap
 *
 * loop at low address in user mode -- /etc/init
 *	cannot be executed.
 */
main(firstaddr)
{

#ifdef	PNETDFS
	printf( "Pnetdfs debug call in main\n" );
	debug();
#endif
ledson(0);
	startup(phypage-1);
	/*
	 * set up system process
	 */

	proc[0].p_addr = firstaddr;
	proc[0].p_size = USIZE;
	proc[0].p_stat = SRUN;
	proc[0].p_flag |= SLOAD|SSYS;
	proc[0].p_nice = NZERO;
	u.u_procp = &proc[0];
	u.u_cmask = CMASK;
	u.u_limit = CDLIMIT;


	/*
	 * initialize system tables
	 */

ledson(14);
	clkstart();
ledson(13);
	cinit();
ledson(12);
	binit();
ledson(11);
	errinit();
ledson(10);
/* CCF not working yet
	mbminit();
*/
ledson(9);
	iinit();
ledson(8);
#ifdef	PNETDFS
	dfsinit();
#endif

ledson(7);
	if (gettod() >= 100000)		/* make sure clock chip is running */
		time = gettod();
ledson(8);
	rootdir = iget(rootdev, ROOTINO);
	rootdir->i_flag &= ~ILOCK;
	u.u_cdir = iget(rootdev, ROOTINO);
	u.u_cdir->i_flag &= ~ILOCK;
	u.u_rdir = NULL;
	u.u_start = time;
ledson(9);

	/*
	 * make init process
	 * enter scheduling loop
	 * with system process
	 */

	if(newproc()) {
ledson(10);
		expand(USIZE + btoc(szicode));
		estabur((unsigned)0, btoc(szicode), (unsigned)0, 0, RO);
		copyout((caddr_t)icode, (caddr_t)0, szicode);
		/*
		 * Return goes to loc. 0 of user init
		 * code just copied out.
		 */
		return;
	}
ledson(11);
#ifdef	PNETDFS
/* Fork the distributed file system processes. */

	if(sysproc()) {
		dfsl();
	}
	if(sysproc()) {
		dfsr();
	}
#endif
#ifdef PDLC
/* Fork the PDLC daemon for PDLC */
	if(newproc()) {
		pdlc();
	}
#endif
ledson(15);
	sched();
}

/*
 * iinit is called once (from main) very early in initialization.
 * It reads the root's super block and initializes the current date
 * from the last modified date.
 *
 * panic: iinit -- cannot read the super block.
 * Usually because of an IO error.
 */
iinit()
{
	register struct buf *bp;
	register struct filsys *fp;


	(*bdevsw[major(rootdev)].d_open)(minor(rootdev), 1);
	(*bdevsw[major(pipedev)].d_open)(minor(pipedev), 1);
	(*bdevsw[major(swapdev)].d_open)(minor(swapdev), 1);
	bp = bread(rootdev, SUPERB);
	if(u.u_error)
		panic("iinit");
	mount[0].m_bufp = getablk(1);
	fp = (struct filsys *)paddr(mount[0].m_bufp);
	copyio(paddr(bp), fp, sizeof(struct filsys), U_RKD);
	brelse(bp);
	mount[0].m_flags = MINUSE;
	mount[0].m_dev = rootdev;
	fp->s_flock = 0;
	fp->s_ilock = 0;
	fp->s_ronly = 0;
	fp->s_ninode = 0;
	fp->s_inode[0] = 0;

	clkset(fp->s_time);
}

struct chead cfreelist;

/*
 * Initialize clist by freeing all character blocks.
 */
cinit()
{
	register n;
	register struct cblock *cp;

	for(n = 0, cp = &cfree[0]; n < v.v_clist; n++, cp++) {
		cp->c_next = cfreelist.c_next;
		cfreelist.c_next = cp;
	}
	cfreelist.c_size = CLSIZE;
}

/*
 * Initialize the buffer I/O system by freeing
 * all buffers and setting all device buffer lists to empty.
 */
binit()
{
	register struct buf *bp;
	register struct buf *dp;
	register unsigned i;
	struct bdevsw *bdp;
	paddr_t nbase;

	dp = &bfreelist;
	dp->b_forw = dp->b_back =
	    dp->av_forw = dp->av_back = dp;
	nbase = bufbase;
	for (i=0, bp=buf; i<v.v_buf+v.v_sabuf; i++, bp++) {
		bp->b_dev = NODEV;
		if (i<v.v_sabuf)
			/* Add in the segment number for the system data seg */
			bp->b_paddr = (paddr_t)sabuf[i]
				+ (paddr_t)(segtoaddr(SDSEG));
		else {
			bp->b_paddr = nbase;
			nbase += BSIZE;
			/* If we have more than 64 buffers, we have to */
			/* increment the segment by one. */
			if (lobyte(hiword(nbase)) != 0) 
				nbase = (nbase & ADDRMASK)
					+ (paddr_t)(segtoaddr(SEGINC));
		}
		bp->b_back = dp;
		bp->b_forw = dp->b_forw;
		dp->b_forw->b_back = bp;
		dp->b_forw = bp;
		bp->b_flags = B_BUSY;
		bp->b_bcount = BSIZE;
		brelse(bp);
	}
	for (i=0, bdp = bdevsw; i<bdevcnt; i++, bdp++) {
		dp = (struct buf *)bdp->d_tab;
		if (dp) {
			dp->b_forw = dp;
			dp->b_back = dp;
		}
	}
	for (i=0; i < v.v_hbuf; i++ )
		hbuf[i].b_forw = hbuf[i].b_back = (struct buf *)&hbuf[i];
}
