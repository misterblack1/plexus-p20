/*
 * Dump core to magtape
 * Assumes memory mapping has been disabled
 * and IPL has be set high ( > 0x15 )
 */

#define PHYSPAGES 650
#define UBA 0x20006000
#define mba1 0x20012000

struct mba_regs {
	int	mba_csr, mba_cr, mba_sr, mba_var,
		mba_bcr;
};

struct	device
{
	int	htcs1, htds, hter, htmr;
	int	htas, htfc, htdt, htck;
	int	htsn, httc;
};

#define	HTADDR	((struct device *)(mba1 + 0x400))
#define HTMAP ((int *) (mba1 + 0x800))

#define	GO	01
#define	WCOM	060
#define	WEOF	026
#define	REW	06
#define	DCLR	010
#define P800	01300		/* 800 + pdp11 mode */
#define	P1600	02300		/* 1600 + pdp11 mode */
#define	RDY	0200

tpdump()
{
	HTADDR->httc = P800;	/* set 800 bpi mode */
	twall((char *)0, PHYSPAGES);	/* write out memory */
	teof();
	teof();
	_rewnd();
	twait();
}

static
twall(start, num)
	char *start;
	int num;
{
	HTADDR->htcs1 = DCLR | GO;
	while (num--) {
		twrite(start);
		start += 512;
		}
}

static
twrite(buf)
char *buf;
{

	twait();
	HTADDR->htfc = -512;
	*HTMAP = (((int)buf)>>9) | 0x80000000;	/* map entry */
	((struct mba_regs *)mba1)->mba_sr = -1;
	((struct mba_regs *)mba1)->mba_bcr = -512;
	((struct mba_regs *)mba1)->mba_var = 0;
	HTADDR->htcs1 = WCOM | GO;
	return;
}

static
twait()
{
	register s;

	do
		s = HTADDR->htds;
	while ((s & RDY) == 0);
}

static
_rewnd()
{
	twait();
	HTADDR->htcs1 = REW | GO;
}

static
teof()
{
	twait();
	HTADDR->htcs1 = WEOF | GO;
}

