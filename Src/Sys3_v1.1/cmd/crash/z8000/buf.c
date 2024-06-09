#include	"crash.h"
#include	"sys/inode.h"
#include	"sys/ino.h"
#include	"sys/buf.h"
#include	"sys/plexus.h"

long pageoffset(longaddr)
long longaddr;
{
	long mapno;

	if (bufbase)
		mapno = bufbase +
			SEGTOMAP(addrtoseg((longaddr-segtoaddr(BFSEG))));
	else
		mapno = maps[SEGTOMAP(addrtoseg(longaddr))*NUMLOGPAGE];
	if (delta)		/* must be /dev/mem */
		return(logadx((mapno&M_BIP)) + (longaddr&0xffff));
	else
	return(logadx((mapno&M_BIP)) - 0x100000 + (longaddr&0xffff));



}

prbufhdr(c)
	register  int  c;
{
	struct	buf	bbuf;
	register  int  b_flags;

	if(c == -1)
		return;
	if(c >= (v.v_buf + v.v_sabuf)) {
		printf("%3d out of range\n", c);
		return;
	}
	lseek(kmem, (long)Buf->n_value + c * sizeof bbuf + delta, 0);
	if(read(kmem, &bbuf, sizeof bbuf) != sizeof bbuf) {
		printf("%3d read error\n");
		return;
	}
	printf("%3d %3d %4.4o %7ld", c,
		major(bbuf.b_dev), minor(bbuf.b_dev),
		bbuf.b_blkno);
	b_flags = bbuf.b_flags;
	printf("%s%s%s%s%s%s%s%s%s%s%s\n",
		b_flags & B_READ ? " read" : " write",
		b_flags & B_DONE ? " done" : "",
		b_flags & B_ERROR ? "error" : "",
		b_flags & B_BUSY ? " busy" : "",
		b_flags & B_PHYS ? " phys" : "",
		b_flags & B_MAP ? " map" : "",
		b_flags & B_WANTED ? " want" : "",
		b_flags & B_AGE ? " age" : "",
		b_flags & B_ASYNC ? " async" : "",
		b_flags & B_DELWRI ? " delwri" : "",
		b_flags & B_STALE ? " stale" : "");
}

prbuffer(c, sw)
	register  int  c;
	int  sw;
{
	char	buffer[BSIZE];
	struct	buf	bufhdr;
	register  int  *ip, i;
	struct	direct	*dp;
	struct	dinode	*dip;
	char	ch;
	long	_3to4();
	int	bad;
	int	j;
	char	*cp;
	char	*itoa();
	int	file;
	char	name[20];
	char	*fmtstr;

	if(c ==  -1)
		return;
	printf("\nBUFFER %d:   ", c);
	if(c >= (v.v_buf + v.v_sabuf)) {
		error("out of range");
		return;
	}
	lseek(kmem, (long)Buf->n_value + c * sizeof bufhdr + delta, 0);
	if(read(kmem, &bufhdr, sizeof bufhdr) != sizeof bufhdr) {
		error("buffer header read error");
		return;
	}
	if (c < v.v_sabuf)
		lseek(kmem, (bufhdr.b_paddr&0xffff) + delta, 0);
	else
		lseek(kmem, pageoffset(bufhdr.b_paddr), 0);
	if(read(kmem, buffer, sizeof buffer) != sizeof buffer) {
		error("buffer read error");
		return;
	}
	switch(sw) {

	default:
	case NULL:
		error("invalid mode");
		break;

	case HEX:

	case DECIMAL:

	case OCTAL:
		switch (sw) {
		case HEX:
			fmtstr = "%4x ";
			break;
		case OCTAL:
			fmtstr = "%7.7o ";
			break;
		case DECIMAL:
			fmtstr = "%5u ";
			break;
		}

		for(i=0, ip=buffer; ip != &buffer[BSIZE]; i++, ip++) {
			if(i % 8 == 0)
				printf("\n%5.5o:\t", i+i);
/*			printf(sw == OCTAL ? "%7.7o " : "%5u ", *ip); */
			printf(fmtstr, *ip);
		}
		printf("\n");
		break;


	case WRITE:
		strcpy(name, "buf.");
		strcat(name, itoa(c));
		if((file = creat(name, 0666)) < 0) {
			error("creat error");
			break;
		}
		if(write(file, buffer, sizeof buffer) != sizeof buffer)
			error("write error");
		else
			printf("file:  %s\n", name);
		close(file);
		break;

	case CHAR:
		for(i=0, cp = buffer; cp != &buffer[BSIZE]; i++, cp++) {
			if(i % 16 == 0)
				printf("\n%5.5o:\t", i);
			putch(*cp);
		}
		break;

	case INODE:
		for(i=1, dip = buffer; dip != &buffer[BSIZE]; i++, dip++) {
			printf("\ni#: %ld  md: ", (bufhdr.b_blkno - 2) *
				INOPB + i);
			switch(dip->di_mode & IFMT) {
			case IFCHR: ch = 'c'; break;
			case IFBLK: ch = 'b'; break;
			case IFDIR: ch = 'd'; break;
			case IFREG: ch = 'f'; break;
			default:    ch = '-'; break;
			}
			putc(ch, stdout);
			printf("%s%s%s%3o",
				dip->di_mode & ISUID ? "u" : "-",
				dip->di_mode & ISGID ? "g" : "-",
				dip->di_mode & ISVTX ? "v" : "-",
				dip->di_mode & 0777);
			printf("  ln: %u  uid: %u  gid: %u  sz: %ld",
				dip->di_nlink, dip->di_uid,
				dip->di_gid, dip->di_size);
			if((dip->di_mode & IFMT) == IFCHR ||
				(dip->di_mode & IFMT) == IFBLK)
				printf("\nmaj: %d  min: %1.1o\n",
					dip->di_addr[0] & 0377,
					dip->di_addr[1] & 0377);
			else
				for(j = 0; j < NADDR; j++) {
					if(j % 7 == 0)
						putc('\n', stdout);
					printf("a%d: %ld  ", j, 
						_3to4(&dip->di_addr[3 * j]));
				}
			printf("\nat: %s", ctime(&dip->di_atime));
			printf("mt: %s", ctime(&dip->di_mtime));
			printf("ct: %s", ctime(&dip->di_ctime));
		}
		break;

	case DIRECT:
		printf("\n");
		for(i=0, dp = buffer; dp != &buffer[BSIZE]; i++, dp++) {
			bad = 0;
			for(cp = dp->d_name; cp != &dp->d_name[DIRSIZ]; cp++)
				if((*cp < 040 || *cp > 0176) && *cp != '\0')
					bad++;
			printf("d%2d: %5u  ", i, dp->d_ino);
			if(bad) {
				printf("unprintable: ");
				for(cp = dp->d_name; cp != &dp->d_name[DIRSIZ];
					cp++)
					putch(*cp);
			} else
				printf("%.14s", dp->d_name);
			putc('\n', stdout);
		}
		break;

	}
}
 

char	*
itoa(n)
	register  int  n;
{
	register  int  i;
	static    char d[10];

	d[9] = '\0';
	for(i = 8; i >= 0; --i) {
		d[i] = n % 10 + '0';
		n /= 10;
		if(n == 0)
			break;
	}
	return(&d[i]);
}

putch(c)
	register  char  c;
{
	c &= 0377;
	if(c < 040 || c > 0176) {
		putc('\\', stdout);
		switch(c) {
		case '\0': c = '0'; break;
		case '\t': c = 't'; break;
		case '\n': c = 'n'; break;
		case '\r': c = 'r'; break;
		case '\b': c = 'b'; break;
		default:   c = '?'; break;
		}
	} else
		putc(' ', stdout);
	putc(c, stdout);
	putc(' ', stdout);
}

long
_3to4(ptr)
	register  char  *ptr;
{
	long	lret;
	register  char  *vptr;

	vptr = &lret;
	*vptr++ = *ptr++;
	*vptr++ = 0;
	*vptr++ = *ptr++;
	*vptr++ = *ptr++;
	return(lret);
}
