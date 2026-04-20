/* SID */
/* @(#)space.h	5.1 4/22/86 */

#include "acp/sys/tty.h"
struct	cblock	cfree[NCLIST];

#include "acp/sys/buf.h"
struct	buf	bfreelist;	/* head of the free list of buffers */
struct	buf	buf[NBUF];

#include "acp/sys/file.h"
struct	file	file[NFILE];	/* file table */

#include "acp/sys/text.h"
struct	text	text[NTEXT];	/* text table */

#include "acp/sys/proc.h"
struct	proc	proc[NPROC];

#include "acp/sys/map.h"
struct map coremap[CMAPSIZ];
struct map dmamap[DMAPSIZ];

#include "acp/sys/callo.h"
struct callo callout[NCALL];

#include "acp/sys/err.h"
struct	err	err = {
	NESLOT,
};

#include "acp/sys/sysinfo.h"
struct sysinfo sysinfo;
struct syswait syswait;

#include "acp/sys/var.h"
struct var v = {
	NBUF,
	NCALL,
	NFILE,
	(char *)(&file[NFILE]),
	NPROC,
	(char *)(&proc[1]),
	NTEXT,
	(char *)(&text[NTEXT]),
	NCLIST,
	CMAPSIZ,
	DMAPSIZ,
	NIOPG,
};
#ifndef PRF_0
prfintr() {}
int	prfstat;
#endif
