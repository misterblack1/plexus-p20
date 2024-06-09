#include "sys/acct.h"
struct	acct	acctbuf;
struct	inode	*acctp;

#include "sys/tty.h"
struct	cblock	cfree[NCLIST];

#include "sys/buf.h"
struct	buf	bfreelist;	/* head of the free list of buffers */
#ifdef vax
struct	buf	buf[NBUF];	/* buffer headers */
char	buffers	[NBUF][BSIZE];
#else
struct	buf	buf[NBUF+NSABUF];	/* buffer headers */
char	sabuf	[NSABUF][BSIZE];
#endif

struct	hbuf	hbuf[NHBUF];	/* buffer hash table */

#include "sys/file.h"
struct	file	file[NFILE];	/* file table */

#include "sys/inode.h"
struct	inode	inode[NINODE];	/* inode table */

#include "sys/proc.h"
struct	proc	proc[NPROC];	/* process table */

#include "sys/text.h"
struct	text text[NTEXT];	/* text table */

#include "sys/map.h"
#ifdef pdp11
struct map coremap[CMAPSIZ];
#endif
struct map swapmap[SMAPSIZ];

#include "sys/callo.h"
struct callo callout[NCALL];

#include "sys/mount.h"
struct mount mount[NMOUNT];

#include "sys/elog.h"
#include "sys/err.h"
struct	err	err = {
	NESLOT,
};

#include "sys/sysinfo.h"
struct sysinfo sysinfo;
struct syswait syswait;

#include "sys/var.h"
struct var v = {
	NBUF,
	NCALL,
	NINODE,
	(char *)(&inode[NINODE]),
	NFILE,
	(char *)(&file[NFILE]),
	NMOUNT,
	(char *)(&mount[NMOUNT]),
	NPROC,
	(char *)(&proc[1]),
	NTEXT,
	(char *)(&text[NTEXT]),
	NCLIST,
	NSABUF,
	MAXUP,
#ifdef pdp11
	CMAPSIZ,
#endif
	SMAPSIZ,
	NHBUF,
	NHBUF-1
};

#ifndef PRF_0
prfintr() {}
int	prfstat;
#endif

#ifdef PCL11B_0
#include "sys/pcl.h"
#endif

#ifdef IMF_0
#include "sys/imf.h"
#endif

#ifdef PSF_0
#include "sys/psf.h"
#endif

#ifdef ST_0
#include "sys/st.h"
#endif

#if (DQS11B_0 || DQS11A_0)
#include "sys/dqs.h"
#endif

#include "icp/sioccomm.h"
#include "icp/icp.h"
#include "icp/siocunix.h"
#include "icp/pbsioc.h"
struct sctl	sctl[NUMICP];	/* UNIX <-> ICP control struct */
