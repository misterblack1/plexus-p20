static char	sccsid[] = "@(#)Zinit.c	4.2";

# include "saio.h"
#include <sys/is.h>
#include <sys/pd.h>
#include <stdio.h>
extern long	atol();

static char	dk1[] = "/dev/dk1";
static char	mt[] = "/dev/mt0";
static char	nmt[] = "/dev/nmt0";
static char	rmt[] = "/dev/rmt0";
static char	nrmt[] = "/dev/nrmt0";
static char	pt[] = "/dev/pt0";
static char	npt[] = "/dev/npt0";
static char	rm[] = "/dev/rm0";
static char	nrm[] = "/dev/nrm0";
static char	td[] = "/dev/td";
static char	us[] = "/dev/us";
static char	kmem[] = "/dev/kmem";
static char	null[] = "/dev/null";
static char	pm[] = "/dev/pm";

static char	*dummy = "/dev/nullXXXXXXXX";
static char	xdk[] = "dk";
static char	xpd[] = "pd";
static char	xis[] = "is";
static char	xmt[] = "mt";
static char	xnmt[] = "nmt";
static char	xpt[] = "pt";
static char	xnpt[] = "npt";
static char	xrm[] = "rm";
static char	xnrm[] = "nrm";
static char	xtd[] = "td";
static char	xus[] = "us";
static char	xkmem[] = "kmem";
static char	xnull[] = "null";
static char	xpm[] = "pm";

int imscdiskcount;
int imsctapecount;
int isbcdiskcount;
int rmtapecount;
int numdisks;
int numtapes;
short	initdone;
char	**initend;	/* initialized in sbrk.  */
extern	char end;
char *mkname();

_init ()
{
	int		diskdev;
	unsigned	isrpd;
	char		*nrmrpt;
	int		numdk;
	char		*rmrpt;
	int		tapedev;

/*	extern char edata, etext; */
	register i,j;
	struct diskconf *dk_sizes;

	if(initdone++) return 0;

/*	printf("Zinit: initdone=%d (0 means do init)\n",initdone); */

	datainit();
	doscaninit();
	getoptint();
	ttyinit();

/*	printf("Zinit: &end=0x%x &edata=0x%x etext=0x%x\n",&end,&edata,&etext);
*/
	*initend = &end;		/* for sbrk */
	
	dkinit();
	mtinit();
	
	if(imsctapecount == 0) {
		tapedev = 5;		/* major device # of rm driver */
		rmrpt = rm;
		nrmrpt = nrm;
	}
	else {
		tapedev = 3;		/* major device # of pt driver */
		rmrpt = pt;
		nrmrpt = npt;
	}
	if (MKNOD (xdk, 0, 0, (daddr_t) 0) < 0)
		perror (dk1);
	if (MKNOD (xpd, 2, 0, (daddr_t) 0) < 0)
		perror (dk1);
	if (MKNOD (xis, 4, 0, (daddr_t) 0) < 0)
		perror (dk1);

	if (MKNOD (mt, 1, 0, (daddr_t) 0) < 0)
		perror (mt);
	if (MKNOD (rmt, 1, 0, (daddr_t) 0) < 0)
		perror (rmt);
	if (MKNOD (xmt, 1, 0, (daddr_t) 0) < 0)
		perror (mt);

	if (MKNOD (nmt, 1, 4, (daddr_t) 0) < 0)
		perror (nmt);
	if (MKNOD (nrmt, 1, 4, (daddr_t) 0) < 0)
		perror (nrmt);
	if (MKNOD (xnmt, 1, 4, (daddr_t) 0) < 0)
		perror (nmt);

	if (MKNOD (rmrpt, tapedev, 0, (daddr_t) 0) < 0)
		perror (rmrpt);

	if (MKNOD (nrmrpt, tapedev, 4, (daddr_t) 0) < 0)
		perror (nrmrpt);

	if(imsctapecount != 0 && rmtapecount != 0) {
		if (MKNOD (rm, 5, 0, (daddr_t) 0) < 0)
			perror (rm);
		if (MKNOD (xrm, 5, 0, (daddr_t) 0) < 0)
			perror (rm);
		if (MKNOD (nrm, 5, 4, (daddr_t) 0) < 0)
			perror (nrm);
		if (MKNOD (xnrm, 5, 4, (daddr_t) 0) < 0)
			perror (nrm);
	}

	if (MKNOD (td, 6, 0, (daddr_t) 0) < 0)
		perror (td);
	if (MKNOD (xtd, 6, 0, (daddr_t) 0) < 0)
		perror (td);
/*
	if (MKNOD (us, 7, 0, (daddr_t) 0) < 0)
		perror (us); 
	if (MKNOD (xus, 7, 0, (daddr_t) 0) < 0)
		perror (us); 
*/
	if (MKNOD (kmem, 8, 0, (daddr_t) 0) < 0)
		perror (kmem);
	if (MKNOD (xkmem, 8, 0, (daddr_t) 0) < 0)
		perror (kmem);
	if (MKNOD (null, 8, 2, (daddr_t) 0) < 0)
		perror (null);
	if (MKNOD (xnull, 8, 2, (daddr_t) 0) < 0)
		perror (null);
	if (MKNOD (pm, 9, 0, (daddr_t) 0) < 0)
		perror (pm);
	if (MKNOD (xpm, 9, 0, (daddr_t) 0) < 0)
		perror (pm);

	/* build dk devices from dk offset info gotten from block 0.
	 */
	if(imscdiskcount == 0) {
		numdk = ISUNITS*ISLOGVOL;
		dk_sizes = is_sizes;
		isrpd = 'is';
		diskdev = 4;	/* major device # of is */
	}
	else {
		numdk = PDUNITS*PDLOGVOL;
		dk_sizes = pd_sizes;
		isrpd = 'pd';
		diskdev = 2;
	}

	for(i=0; i < numdk; i++) {
			/* shift right to convert 512 byte blocks to 1024 */
		MKNOD(mkname('dk',i),0,i,(daddr_t)(dk_sizes[i].blkoff>>1));
		MKNOD(mkname(isrpd,i),diskdev,i,
			(daddr_t)(dk_sizes[i].blkoff>>1));
	}
	if(imscdiskcount != 0 && isbcdiskcount != 0) {
		for(j=0; j < ISUNITS*ISLOGVOL; j++) {
			/* shift right to convert 512 byte blocks to 1024 */
			MKNOD(mkname('is',j),4,j,(daddr_t)
				(is_sizes[j].blkoff>>1));
		}
	}

	/* preallocate 5 slots in dtab for V7 file names: dev(X,Y) */
	for (i = 0; i < 5; i++) {
		dummy[sizeof(dummy)-1] = i+0x30;
		if(MKNOD (dummy, 8, 2, (daddr_t) 0) < 0)
			perror(dummy);
	}	

	if(rootdev != 0) {
		i=mkname('dk',rootdev);
		printf("mounting %s as /\n",i);
		if(mount(i,"") < 0)
			perror (i);
	}
	else {
		if (mount (dk1, "") < 0)
			perror (dk1);
	}

	printf("\n");
}
/* make a name of the type:
 *	/dev/<id><num>
 */
char *
mkname(id,num)
unsigned id; int num;
{

	extern char *malloc();
	register char *p;
		/* allocate area large enough for /dev/IDxxx */
	p = malloc((unsigned) 11);
	sprintf(p,"/dev/%2s%d",&id,num);
	return(p);
}

/* 	Scan for v7 standalone file names of the kind xx(unit,offset)
 *	Sets opencode to:
 *		1 for disk device
 *		2 for tape device
 *		0 otherwise
 *	Return
 *		-1 on error
 *		0 otherwise
 * 	Called by open.
 */
v7files(str,opencode)
short *str, *opencode;
{
	extern char	*strchr();
	register char	*cp1,
			*cp2,
			savchar;
	short		*devname,
			unit,
			device;
	long		offset;

	/* str has gone thru _cond which prepends a '/' */
	if((cp1 = strchr(str,'/')) == NULL) return 0; 
	devname = ++cp1;	/* devname points to 'dk', 'mt', etc */
	if((cp1 = strchr(str,'(')) == NULL) return 0;
	unit = atoi(++cp1);
	if((cp1 = strchr(cp1,',')) == NULL) return -1;
	offset = atol(++cp1);
	if((cp1 = strchr(cp1,')')) == NULL) return -1;
	*opencode = 0;
	switch(*devname) {
	case 'dk':
		device = 0;
		unit =* PDLOGVOL;
		offset >>= 1;	/* convert 512 byte value to 1024 */
		break;
	case 'pd':
		device = 2;
		unit =* PDLOGVOL;
		offset >>= 1;	/* convert 512 byte value to 1024 */
		break;
	case 'is':
		device = 4;
		unit =* ISLOGVOL;
		offset >>= 1;	/* convert 512 byte value to 1024 */
		break;
	case 'mt':
		device = 1;
		*opencode = 2;
		break;
	case 'pt':
		device = 3;
		*opencode = 2;
		break;
	case 'rm':
		device = 5;
		*opencode = 2;
		break;
	default:
		return -1;	/* invalid device */
	}

	savchar = *++cp1;
	for(cp2 = cp1; *cp2 == ' ' || *cp2 == '\t'; cp2++);
	if(*cp2 != '\0') {
		*opencode = 1;	/* we have a pathname after the dev(X,X) */
		*cp1 = '\0';
	}

	if(MKNOD(str,device,unit,(daddr_t) offset) < 0)
		if(addtodummy(str,device,unit,(daddr_t) offset) < 0)
			return -1;

	*cp1 = savchar;
	return 0;
}

/* this routine adds the device 'str' to a "dummy" device entry in _dtab */

addtodummy(str,device,unit,offset)
char *str; short device, unit; daddr_t(offset);
{
	register struct dtab *dp;
	extern _devcnt;

	if (device >= _devcnt
	    || device < 0
	    || unit < 0
	    || offset < 0) {
		errno = EINVAL;
		return (-1);
	}

	for (dp = &_dtab[0]; dp < &_dtab[NDEV]; dp++)
		if (dp->dt_name == 0 || strncmp(dummy, dp->dt_name,
			sizeof(dummy)-2) == 0) break;

	if (dp == &_dtab[NDEV]) {
		_prs("%s: no space in device table\n",myname);
		return (-1);
	}
	strncpy(dp->dt_name, str, sizeof(dummy));
	dp->dt_devp = &_devsw[device];
	dp->dt_unit = unit;
	dp->dt_boff = (daddr_t) offset;
	return 0;
}

/* initialize variables for the libc/lib2 routine, doscan() */
#define	SPC	01
doscaninit() 
{
	extern char _sctab[128];
	register i;

	for(i = 0; i < sizeof (_sctab); i++)
		_sctab[i] = 0;
	_sctab[9] = _sctab[10] = _sctab[32] = SPC;
}

/* _iob are the predefined stdio buffers initialized in data.o */
datainit()
{
	extern char	_sibuf[BUFSIZ];
	extern char	_sobuf[BUFSIZ];
	extern struct	_iobuf	_iob[_NFILE];
	register i;

	_iob[0]._ptr = _sibuf;
	_iob[0]._cnt = 0;
	_iob[0]._base = _sibuf;
	_iob[0]._flag = _IOREAD;
	_iob[0]._file = 0;

	_iob[1]._ptr = NULL;
	_iob[1]._cnt = 0;
	_iob[1]._base = NULL;
	_iob[1]._flag = _IOWRT;
	_iob[1]._file = 1;

	_iob[2]._ptr = NULL;
	_iob[2]._cnt = 0;
	_iob[2]._base = NULL;
	_iob[2]._flag = _IOWRT+_IONBF;
	_iob[2]._file = 2;

	for (i = 3; i < _NFILE; i++) {
		_iob[i]._ptr = NULL;
		_iob[i]._cnt = 0;
		_iob[i]._base = NULL;
		_iob[i]._flag = 0;
		_iob[i]._file = 0;
	}
}
/* initialize variables used by getopt.o routine in libc/lib2 */
getoptint()
{
	extern 	int	XXsp;
	extern 	int	opterr;
	extern 	int	optind;
	XXsp = opterr = optind = 1;
}
# define TTYHOG 256
ttyinit()
{
	extern struct {
		char	*t_fptr;
		char	*t_lptr;
		char	t_buf[TTYHOG];
	} _tt;
	register char *p;
	_tt.t_fptr = _tt.t_lptr = _tt.t_buf;
	for(p = _tt.t_buf; p < &_tt.t_buf[TTYHOG]; p++) *p = 0;
}
