#include "sys/plexus.h"
#include "sys/param.h"
#include "sys/systm.h"
#include "sys/seg.h"
#include "sys/buf.h"
#include "sys/elog.h"
#include "sys/iobuf.h"
#include "sys/utsname.h"

/*
 * In case console is off,
 * panicstr contains argument to last call to panic.
 */
char	*panicstr;

/*
 * Scaled down version of C Library printf.
 * Only %s %u %d (==%u) %o %x %D are recognized.
 * Used to print diagnostic information
 * directly on console tty.
 * Since it is not interrupt driven,
 * all system activities are pretty much suspended.
 * Printf should not be used for chit-chat.
 */
printf(fmt, x1)
register char *fmt;
unsigned x1;
{
	register c;
	register unsigned int *adx;
	char *s;

	adx = &x1;
loop:
	while((c = *fmt++) != '%') {
		if(c == '\0')
			return;
		putchar(c);
	}
	c = *fmt++;
	if(c == 'd' || c == 'u' || c == 'o' || c == 'x')
		printn((long)*adx, c=='o'? 8: (c=='x'? 16:10));
	else if(c == 's') {
		s = (char *)*adx;
		while(c = *s++)
			putchar(c);
	} else if (c == 'D') {
		printn(*(long *)adx, 10);
		adx += (sizeof(long) / sizeof(int)) - 1;
	}
	adx++;
	goto loop;
}

printn(n, b)
long n;
register b;
{
	register i, nd, c;
	int	flag;
	int	plmax;
	char d[12];

	c = 1;
	flag = n < 0;
	if (flag)
		n = (-n);
	if (b==8)
		plmax = 11;
	else if (b==10)
		plmax = 10;
	else if (b==16)
		plmax = 8;
	if (flag && b==10) {
		flag = 0;
		putchar('-');
	}
	for (i=0;i<plmax;i++) {
		nd = n%b;
		if (flag) {
			nd = (b - 1) - nd + c;
			if (nd >= b) {
				nd -= b;
				c = 1;
			} else
				c = 0;
		}
		d[i] = nd;
		n = n/b;
		if ((n==0) && (flag==0))
			break;
	}
	if (i==plmax)
		i--;
	for (;i>=0;i--) {
		putchar("0123456789ABCDEF"[d[i]]);
	}
}

/*
 * Panic is called on unresolvable fatal errors.
 * It syncs, prints "panic: mesg" and then loops.
 */
panic(s)
char *s;
{
	panicstr = s;
	update();
	printf("%s: panic: %s\n", utsname.sysname, s);
	if ((~in_local(P_SWIT))&0x8) {
		out_local(P_PIOADAT, (~B_RESETOFF) | B_ALLOWRESET);
	} else {
		debug();
	}
	for(;;)
		idle();
}

/*
 * prdev prints a warning message of the
 * form "mesg on dev x/y".
 * x and y are the major and minor parts of
 * the device argument.
 */
prdev(str, dev)
char *str;
dev_t dev;
{

	printf("%s: %s on dev %u/%u\n", utsname.sysname, str, major(dev),
		minor(dev));
}

static char *nam[] = {
	"DK",
	"MT",
	"PD",
	"PT",
	"IS",
	"MT",
};
/*
 * Deverr prints a diagnostic from a device driver.
 * It prints the device, block number,
 * and two arguments, usually error status.
 */
deverr(dev, dp, o1, o2)
register char *dev;
register struct iobuf *dp;
{
	register struct buf *bp;

	bp = dp->b_actf;
	printf("%s: error on %s, minor %d\n", utsname.sysname,
		dev, minor(bp->b_dev));
	printf("%s: bn=%D er=0x%x, 0x%x\n", utsname.sysname,
		bp->b_blkno, o1, o2);
}
